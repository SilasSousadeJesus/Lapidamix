#include "plugin/PluginEditor.h"

#include "profiles/InstrumentList.h"

LapidamixAudioProcessorEditor::LapidamixAudioProcessorEditor (LapidamixAudioProcessor& processor)
    : AudioProcessorEditor (&processor)
{
    instrumentLabel.setText ("Instrumento", juce::dontSendNotification);
    addAndMakeVisible (instrumentLabel);

    // Os itens precisam existir antes de ligar a lista ao parâmetro.
    instrumentBox.addItemList (lapidamix::getInstrumentNames(), 1);
    addAndMakeVisible (instrumentBox);

    instrumentAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor.getParameters(),
        LapidamixAudioProcessor::instrumentParameterId,
        instrumentBox);

    setSize (320, 80);
}

LapidamixAudioProcessorEditor::~LapidamixAudioProcessorEditor() = default;

void LapidamixAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void LapidamixAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (16).withSizeKeepingCentre (getWidth() - 32, 28);

    instrumentLabel.setBounds (area.removeFromLeft (100));
    instrumentBox.setBounds (area);
}
