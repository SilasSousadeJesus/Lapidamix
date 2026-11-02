#pragma once

#include <array>

#include "plugin/PluginProcessor.h"

// Interface simples (D08, D65): lista de instrumento, bypass, ganho de saída e bandas.
class LapidamixAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit LapidamixAudioProcessorEditor (LapidamixAudioProcessor& processor);
    ~LapidamixAudioProcessorEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    using ParameterState = juce::AudioProcessorValueTreeState;

    struct BandControls
    {
        juce::Label name;
        juce::ToggleButton enabled;
        juce::Slider frequency;
        juce::Slider gain;
        juce::Slider q;
        std::unique_ptr<ParameterState::ButtonAttachment> enabledAttachment;
        std::unique_ptr<ParameterState::SliderAttachment> frequencyAttachment;
        std::unique_ptr<ParameterState::SliderAttachment> gainAttachment;
        std::unique_ptr<ParameterState::SliderAttachment> qAttachment;
    };

    void setUpLabel (juce::Label& label, const char* text);
    void setUpSlider (juce::Slider& slider);

    juce::Label instrumentLabel;
    juce::ComboBox instrumentBox;
    std::unique_ptr<ParameterState::ComboBoxAttachment> instrumentAttachment;

    juce::ToggleButton bypassButton { "Bypass" };
    std::unique_ptr<ParameterState::ButtonAttachment> bypassAttachment;

    juce::Label outputGainLabel;
    juce::Slider outputGainSlider;
    std::unique_ptr<ParameterState::SliderAttachment> outputGainAttachment;

    juce::Label frequencyHeader;
    juce::Label gainHeader;
    juce::Label qHeader;

    std::array<BandControls, lapidamix::numEqBands> bands;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapidamixAudioProcessorEditor)
};
