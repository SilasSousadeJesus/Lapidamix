#include "plugin/PluginProcessor.h"

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
                .withStringFromValueFunction ([] (float value, int) { return juce::String (value, 1); })));

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

    ++resultVersion;
}

lapidamix::AnalysisResult LapidamixAudioProcessor::getLastAnalysisResult()
{
    const juce::ScopedLock lock (resultLock);
    return lastResult;
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
            parameters.replaceState (state);
            ++resultVersion;
        }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LapidamixAudioProcessor();
}
