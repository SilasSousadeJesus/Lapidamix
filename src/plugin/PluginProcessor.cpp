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

        return layout;
    }
}

LapidamixAudioProcessor::LapidamixAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "LapidamixState", createParameterLayout())
{
}

LapidamixAudioProcessor::~LapidamixAudioProcessor() = default;

void LapidamixAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void LapidamixAudioProcessor::releaseResources()
{
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
    // Regra de ouro do tempo real (D09): nada aqui pode bloquear a thread de áudio.
    // A entrada e a saída usam o mesmo buffer, então não mexer nele deixa o áudio passar intacto.
    juce::ignoreUnused (buffer, midiMessages);
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
