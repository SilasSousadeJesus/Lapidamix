#include "plugin/PluginProcessor.h"

#include "core/DisplayFormat.h"
#include "plugin/PluginEditor.h"
#include "profiles/InstrumentList.h"

namespace
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID { LapidamixAudioProcessor::instrumentParameterId, 1 },
            "Instrumento",
            lapidamix::getInstrumentNames(),
            0));

        layout.add (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID { LapidamixAudioProcessor::bypassParameterId, 1 },
            "Bypass",
            false));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { LapidamixAudioProcessor::outputGainParameterId, 1 },
            juce::String::fromUTF8 ("Ganho de saída"),
            juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f },
            0.0f,
            juce::AudioParameterFloatAttributes()
                .withLabel ("dB")
                .withStringFromValueFunction ([] (float value, int) { return lapidamix::format::decimal (value, 1); })
                .withValueFromStringFunction ([] (const juce::String& text) { return lapidamix::format::parseDecimal (text); })));

        // Não automatizável: mover a intensidade reescreve outros parâmetros, o que não pode
        // acontecer durante a reprodução (P57).
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { LapidamixAudioProcessor::intensityParameterId, 1 },
            "Intensidade",
            juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
            100.0f,
            juce::AudioParameterFloatAttributes()
                .withLabel ("%")
                .withAutomatable (false)
                .withStringFromValueFunction ([] (float value, int) { return lapidamix::format::decimal (value, 0); })
                .withValueFromStringFunction ([] (const juce::String& text) { return lapidamix::format::parseDecimal (text); })));

        lapidamix::EqModule::addParameters (layout, lapidamix::voiceProfile);

        return layout;
    }
}

LapidamixAudioProcessor::LapidamixAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "LapidamixState", createParameterLayout()),
      eq (parameters, lapidamix::voiceProfile),
      bypassValue (parameters.getRawParameterValue (bypassParameterId)),
      outputGainDbValue (parameters.getRawParameterValue (outputGainParameterId))
{
    analysis.onResult = [this] (const lapidamix::AnalysisResult& result) { storeAnalysisResult (result); };
}

LapidamixAudioProcessor::~LapidamixAudioProcessor() = default;

void LapidamixAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Uma captura em andamento usaria a taxa de amostragem antiga.
    if (! juce::exactlyEqual (currentSampleRate.load(), sampleRate))
        analysis.abort();

    currentSampleRate.store (sampleRate);
    rta.setSampleRate (sampleRate);

    const juce::dsp::ProcessSpec spec { sampleRate,
                                        (juce::uint32) samplesPerBlock,
                                        (juce::uint32) getTotalNumOutputChannels() };

    eq.setBypassed (bypassValue->load (std::memory_order_relaxed) >= 0.5f);
    eq.prepare (spec);

    outputGain.reset (sampleRate, 0.02);
    outputGain.setCurrentAndTargetValue (getTargetOutputGain());
}

void LapidamixAudioProcessor::releaseResources()
{
    eq.reset();
}

bool LapidamixAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();

    if (output != juce::AudioChannelSet::mono() && output != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == output;
}

void LapidamixAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    render (buffer);
}

void LapidamixAudioProcessor::processBlockBypassed (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // O mesmo caminho do processBlock, para o bypass também ter transição curta (D64).
    juce::ignoreUnused (midiMessages);
    render (buffer);
}

void LapidamixAudioProcessor::render (juce::AudioBuffer<float>& buffer) noexcept
{
    // Regra de ouro do tempo real (D09): nada aqui pode bloquear a thread de áudio.
    const juce::ScopedNoDenormals noDenormals;

    // A análise mede a entrada, antes do EQ, para não medir a própria correção (D69).
    analysis.pushInput (buffer);

    eq.setBypassed (bypassValue->load (std::memory_order_relaxed) >= 0.5f);
    eq.process (buffer);

    outputGain.setTargetValue (getTargetOutputGain());

    if (outputGain.isSmoothing() || ! juce::exactlyEqual (outputGain.getTargetValue(), 1.0f))
        outputGain.applyGain (buffer, buffer.getNumSamples());

    rta.push (buffer);
}

void LapidamixAudioProcessor::startAnalysis()
{
    const auto sampleRate = currentSampleRate.load();

    if (sampleRate <= 0.0)
        return;

    analysis.start ({ sampleRate, lapidamix::voiceProfile.minimumFundamental, lapidamix::voiceProfile.maximumFundamental });
}

void LapidamixAudioProcessor::storeAnalysisResult (const lapidamix::AnalysisResult& result)
{
    {
        const juce::ScopedLock lock (resultLock);
        lastResult = result;
    }

    // A análise nova ainda não foi aplicada: o som continua como estava (D99).
    correctionIsApplied.store (false);
    analysisWasApplied.store (false);
    ++resultVersion;
}

lapidamix::AnalysisResult LapidamixAudioProcessor::getLastAnalysisResult()
{
    const juce::ScopedLock lock (resultLock);
    return lastResult;
}

lapidamix::CorrectionPlan LapidamixAudioProcessor::getCorrectionPlan()
{
    const auto& settings = lapidamix::voiceProfile.correction;
    const auto intensity = parameters.getRawParameterValue (intensityParameterId)->load() / 100.0f;
    return lapidamix::scaleCorrectionPlan (lapidamix::makeCorrectionPlan (getLastAnalysisResult(), settings), intensity, settings);
}

float LapidamixAudioProcessor::getCompensationDb (const lapidamix::CorrectionPlan& plan)
{
    return lapidamix::estimateCompensationDb (getLastAnalysisResult(), plan, currentSampleRate.load());
}

void LapidamixAudioProcessor::applyCurrentCorrection()
{
    const auto plan = getCorrectionPlan();

    if (! plan.isAvailable)
        return;

    applyCorrectionPlan (plan);
    appliedIntensity.store (parameters.getRawParameterValue (intensityParameterId)->load());
    correctionIsApplied.store (true);
    analysisWasApplied.store (true);
}

void LapidamixAudioProcessor::handleIntensityChange()
{
    const auto intensity = parameters.getRawParameterValue (intensityParameterId)->load();

    if (correctionIsApplied.load() && ! juce::approximatelyEqual (intensity, appliedIntensity.load()))
        applyCurrentCorrection();
}

void LapidamixAudioProcessor::setParameterValue (const juce::String& id, float value)
{
    auto* parameter = parameters.getParameter (id);
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost (parameter->convertTo0to1 (value));
    parameter->endChangeGesture();
}

void LapidamixAudioProcessor::clearCorrection()
{
    for (int bandIndex = 0; bandIndex < lapidamix::numEqBands; ++bandIndex)
        setParameterValue (lapidamix::EqModule::getEnabledParameterId (bandIndex), 0.0f);

    setParameterValue (outputGainParameterId, 0.0f);

    // Sem correção aplicada, mover a intensidade não pode religar as bandas (D104).
    correctionIsApplied.store (false);
}

void LapidamixAudioProcessor::applyCorrectionPlan (const lapidamix::CorrectionPlan& plan)
{
    if (! plan.isAvailable)
        return;

    const auto setValue = [this] (const juce::String& id, float value) { setParameterValue (id, value); };

    const auto applyBand = [&] (int bandIndex, const lapidamix::BandCorrection& band)
    {
        using Module = lapidamix::EqModule;
        const auto& profile = lapidamix::voiceProfile.eqBands[(size_t) bandIndex];

        if (band.frequency > 0.0f)
            setValue (Module::getFrequencyParameterId (bandIndex), band.frequency);

        if (profile.hasGain)
            setValue (Module::getGainParameterId (bandIndex), band.gainDb);

        if (profile.hasAdjustableQ && band.q > 0.0f)
            setValue (Module::getQParameterId (bandIndex), band.q);

        setValue (Module::getEnabledParameterId (bandIndex), band.isEnabled ? 1.0f : 0.0f);
    };

    // Índices na ordem do perfil da voz.
    applyBand (0, plan.highPass);
    applyBand (1, plan.mud);
    applyBand (2, plan.resonance);
    applyBand (3, plan.presence);
    applyBand (4, plan.air);

    // Valor fixo: não muda durante a reprodução, senão seria um compressor (D04, D102).
    setValue (outputGainParameterId, getCompensationDb (plan));
}

float LapidamixAudioProcessor::getTargetOutputGain() const noexcept
{
    // O bypass também ignora o ganho de saída (D65).
    if (bypassValue->load (std::memory_order_relaxed) >= 0.5f)
        return 1.0f;

    return juce::Decibels::decibelsToGain (outputGainDbValue->load (std::memory_order_relaxed));
}

juce::AudioProcessorParameter* LapidamixAudioProcessor::getBypassParameter() const
{
    return parameters.getParameter (bypassParameterId);
}

juce::AudioProcessorEditor* LapidamixAudioProcessor::createEditor()
{
    return new LapidamixAudioProcessorEditor (*this);
}

bool LapidamixAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String LapidamixAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LapidamixAudioProcessor::acceptsMidi() const
{
    return false;
}

bool LapidamixAudioProcessor::producesMidi() const
{
    return false;
}

bool LapidamixAudioProcessor::isMidiEffect() const
{
    return false;
}

double LapidamixAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LapidamixAudioProcessor::getNumPrograms()
{
    return 1;
}

int LapidamixAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LapidamixAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String LapidamixAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void LapidamixAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void LapidamixAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    state.setProperty ("correctionIsApplied", correctionIsApplied.load(), nullptr);
    state.setProperty ("appliedIntensity", appliedIntensity.load(), nullptr);
    state.setProperty ("analysisWasApplied", analysisWasApplied.load(), nullptr);

    {
        // As medidas são salvas com o projeto, sem guardar áudio (D76).
        const juce::ScopedLock lock (resultLock);

        if (lastResult.isValid)
            state.appendChild (lastResult.toValueTree(), nullptr);
    }

    if (const auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void LapidamixAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (const auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
        {
            auto state = juce::ValueTree::fromXml (*xml);
            const auto savedResult = state.getChildWithName (lapidamix::AnalysisResult::getTreeType());

            {
                const juce::ScopedLock lock (resultLock);
                lastResult = lapidamix::AnalysisResult::fromValueTree (savedResult);
            }

            state.removeChild (savedResult, nullptr);

            correctionIsApplied.store (state.getProperty ("correctionIsApplied", false));
            appliedIntensity.store (state.getProperty ("appliedIntensity", 0.0f));
            analysisWasApplied.store (state.getProperty ("analysisWasApplied", correctionIsApplied.load())); // Projetos antigos não têm.
            state.removeProperty ("correctionIsApplied", nullptr);
            state.removeProperty ("appliedIntensity", nullptr);
            state.removeProperty ("analysisWasApplied", nullptr);

            parameters.replaceState (state);
            ++resultVersion;
        }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LapidamixAudioProcessor();
}
