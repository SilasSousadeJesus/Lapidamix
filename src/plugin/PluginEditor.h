#pragma once

#include "plugin/PluginProcessor.h"

// Interface simples (D08): por enquanto, só a lista de instrumentos.
class LapidamixAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit LapidamixAudioProcessorEditor (LapidamixAudioProcessor& processor);
    ~LapidamixAudioProcessorEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label instrumentLabel;
    juce::ComboBox instrumentBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> instrumentAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapidamixAudioProcessorEditor)
};
