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
}

LapidamixAudioProcessor::~LapidamixAudioProcessor() = default;

void LapidamixAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
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

    eq.setBypassed (bypassValue->load (std::memory_order_relaxed) >= 0.5f);
    eq.process (buffer);

    outputGain.setTargetValue (getTargetOutputGain());

    if (outputGain.isSmoothing() || ! juce::exactlyEqual (outputGain.getTargetValue(), 1.0f))
        outputGain.applyGain (buffer, buffer.getNumSamples());
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
    const auto state = parameters.copyState();

    if (const auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void LapidamixAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (const auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LapidamixAudioProcessor();
}
