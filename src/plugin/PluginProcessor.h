#pragma once

#include <atomic>

#include <juce_audio_processors/juce_audio_processors.h>

#include "modules/EqModule.h"

// Ligação com o DAW: parâmetros, salvamento e a cadeia de processamento.
class LapidamixAudioProcessor final : public juce::AudioProcessor
{
public:
    static constexpr auto instrumentParameterId = "instrument";
    static constexpr auto bypassParameterId = "bypass";
    static constexpr auto outputGainParameterId = "output_gain";

    LapidamixAudioProcessor();
    ~LapidamixAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    using AudioProcessor::processBlock;

    void processBlockBypassed (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    using AudioProcessor::processBlockBypassed;

    juce::AudioProcessorParameter* getBypassParameter() const override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getParameters() noexcept { return parameters; }

private:
    void render (juce::AudioBuffer<float>& buffer) noexcept;
    float getTargetOutputGain() const noexcept;

    juce::AudioProcessorValueTreeState parameters;
    lapidamix::EqModule eq;

    std::atomic<float>* bypassValue = nullptr;
    std::atomic<float>* outputGainDbValue = nullptr;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapidamixAudioProcessor)
};
