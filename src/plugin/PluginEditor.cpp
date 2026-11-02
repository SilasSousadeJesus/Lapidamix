#include "plugin/PluginEditor.h"

#include "profiles/InstrumentList.h"

namespace
{
    constexpr int windowWidth = 820;
    constexpr int margin = 16;
    constexpr int rowHeight = 32;
    constexpr int nameWidth = 110;
    constexpr int toggleWidth = 36;
    constexpr int columnGap = 8;
}

LapidamixAudioProcessorEditor::LapidamixAudioProcessorEditor (LapidamixAudioProcessor& processor)
    : AudioProcessorEditor (&processor)
{
    using Module = lapidamix::EqModule;
    auto& parameters = processor.getParameters();

    setUpLabel (instrumentLabel, "Instrumento");

    // Os itens precisam existir antes de ligar a lista ao parâmetro.
    instrumentBox.addItemList (lapidamix::getInstrumentNames(), 1);
    addAndMakeVisible (instrumentBox);
    instrumentAttachment = std::make_unique<ParameterState::ComboBoxAttachment> (
        parameters, LapidamixAudioProcessor::instrumentParameterId, instrumentBox);

    addAndMakeVisible (bypassButton);
    bypassAttachment = std::make_unique<ParameterState::ButtonAttachment> (
        parameters, LapidamixAudioProcessor::bypassParameterId, bypassButton);

    setUpLabel (outputGainLabel, "Saída (dB)");
    setUpSlider (outputGainSlider);
    outputGainAttachment = std::make_unique<ParameterState::SliderAttachment> (
        parameters, LapidamixAudioProcessor::outputGainParameterId, outputGainSlider);

    setUpLabel (frequencyHeader, "Frequência (Hz)");
    setUpLabel (gainHeader, "Ganho (dB)");
    setUpLabel (qHeader, "Q");

    for (int bandIndex = 0; bandIndex < lapidamix::numEqBands; ++bandIndex)
    {
        const auto& profile = lapidamix::voiceProfile.eqBands[(size_t) bandIndex];
        auto& band = bands[(size_t) bandIndex];

        setUpLabel (band.name, profile.name);

        band.enabled.setTitle (juce::String::fromUTF8 (profile.name));
        addAndMakeVisible (band.enabled);
        band.enabledAttachment = std::make_unique<ParameterState::ButtonAttachment> (
            parameters, Module::getEnabledParameterId (bandIndex), band.enabled);

        setUpSlider (band.frequency);
        band.frequencyAttachment = std::make_unique<ParameterState::SliderAttachment> (
            parameters, Module::getFrequencyParameterId (bandIndex), band.frequency);

        if (profile.hasGain)
        {
            setUpSlider (band.gain);
            band.gainAttachment = std::make_unique<ParameterState::SliderAttachment> (
                parameters, Module::getGainParameterId (bandIndex), band.gain);
        }

        if (profile.hasAdjustableQ)
        {
            setUpSlider (band.q);
            band.qAttachment = std::make_unique<ParameterState::SliderAttachment> (
                parameters, Module::getQParameterId (bandIndex), band.q);
        }
    }

    setSize (windowWidth, 2 * margin + rowHeight * (lapidamix::numEqBands + 2));
}

LapidamixAudioProcessorEditor::~LapidamixAudioProcessorEditor() = default;

void LapidamixAudioProcessorEditor::setUpLabel (juce::Label& label, const char* text)
{
    label.setText (juce::String::fromUTF8 (text), juce::dontSendNotification);
    addAndMakeVisible (label);
}

void LapidamixAudioProcessorEditor::setUpSlider (juce::Slider& slider)
{
    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 22);
    addAndMakeVisible (slider);
}

void LapidamixAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void LapidamixAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (margin);

    auto topRow = area.removeFromTop (rowHeight).reduced (0, 3);
    instrumentLabel.setBounds (topRow.removeFromLeft (nameWidth));
    instrumentBox.setBounds (topRow.removeFromLeft (130));
    topRow.removeFromLeft (columnGap * 2);
    bypassButton.setBounds (topRow.removeFromLeft (90));
    topRow.removeFromLeft (columnGap * 2);
    outputGainLabel.setBounds (topRow.removeFromLeft (80));
    outputGainSlider.setBounds (topRow);

    const auto columnsLeft = nameWidth + toggleWidth + columnGap;
    const auto columnWidth = (area.getWidth() - columnsLeft - 2 * columnGap) / 3;

    const auto layOutColumns = [columnWidth] (juce::Rectangle<int> row, juce::Component& first,
                                              juce::Component& second, juce::Component& third)
    {
        first.setBounds (row.removeFromLeft (columnWidth));
        row.removeFromLeft (columnGap);
        second.setBounds (row.removeFromLeft (columnWidth));
        row.removeFromLeft (columnGap);
        third.setBounds (row.removeFromLeft (columnWidth));
    };

    auto headerRow = area.removeFromTop (rowHeight).reduced (0, 3);
    headerRow.removeFromLeft (columnsLeft);
    layOutColumns (headerRow, frequencyHeader, gainHeader, qHeader);

    for (auto& band : bands)
    {
        auto row = area.removeFromTop (rowHeight).reduced (0, 3);
        band.name.setBounds (row.removeFromLeft (nameWidth));
        band.enabled.setBounds (row.removeFromLeft (toggleWidth));
        row.removeFromLeft (columnGap);
        layOutColumns (row, band.frequency, band.gain, band.q);
    }
}
