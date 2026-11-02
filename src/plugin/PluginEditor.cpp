#include "plugin/PluginEditor.h"

#include "core/DisplayFormat.h"
#include "profiles/InstrumentList.h"

namespace
{
    constexpr int windowWidth = 820;
    constexpr int margin = 16;
    constexpr int rowHeight = 32;
    constexpr int nameWidth = 110;
    constexpr int toggleWidth = 36;
    constexpr int columnGap = 8;
    constexpr int resultsHeight = 96;
    constexpr int plotHeight = 220;
    constexpr int refreshRateHz = 30;
    constexpr int maximumShownResonances = 3;

    juce::String utf8 (const char* text)
    {
        return juce::String::fromUTF8 (text);
    }

    juce::String seconds (float value)
    {
        return lapidamix::format::decimal (value, 1) + " s";
    }

    juce::String hertz (float value)
    {
        return lapidamix::format::decimal (value, 0) + " Hz";
    }

    juce::String percent (float fraction)
    {
        return lapidamix::format::decimal (fraction * 100.0f, 0) + "%";
    }
}

LapidamixAudioProcessorEditor::LapidamixAudioProcessorEditor (LapidamixAudioProcessor& processor)
    : AudioProcessorEditor (&processor),
      audioProcessor (processor)
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

    analyseButton.onClick = [this]
    {
        auto& analysis = audioProcessor.getAnalysis();

        if (analysis.getState() == lapidamix::AnalysisController::State::idle)
            audioProcessor.startAnalysis();
        else
            analysis.requestStop();

        updateAnalysisStatus();
    };

    addAndMakeVisible (analyseButton);
    addAndMakeVisible (analysisStatus);

    analysisResults.setJustificationType (juce::Justification::topLeft);
    addAndMakeVisible (analysisResults);
    addAndMakeVisible (spectrumPlot);

    setSize (windowWidth, 2 * margin + rowHeight * (lapidamix::numEqBands + 3) + resultsHeight + plotHeight);

    // O RTA só calcula com a janela aberta (D74).
    audioProcessor.getRta().setActive (true);
    updateAnalysisStatus();
    startTimerHz (refreshRateHz);
}

LapidamixAudioProcessorEditor::~LapidamixAudioProcessorEditor()
{
    stopTimer();
    audioProcessor.getRta().setActive (false);
}

void LapidamixAudioProcessorEditor::timerCallback()
{
    spectrumPlot.setRtaCurve (audioProcessor.getRta().update());
    updateAnalysisStatus();

    const auto version = audioProcessor.getResultVersion();

    if (version != shownResultVersion)
    {
        shownResultVersion = version;
        showAnalysisResult (audioProcessor.getLastAnalysisResult());
    }
}

void LapidamixAudioProcessorEditor::updateAnalysisStatus()
{
    using Controller = lapidamix::AnalysisController;
    const auto& analysis = audioProcessor.getAnalysis();

    switch (analysis.getState())
    {
        case Controller::State::capturing:
            analyseButton.setButtonText ("Parar");
            analyseButton.setEnabled (true);
            analysisStatus.setText (utf8 ("Ouvindo: ") + seconds (analysis.getVoiceSeconds()) + " de "
                                        + seconds (Controller::targetVoiceSeconds) + utf8 (" de voz válida (")
                                        + seconds (analysis.getCapturedSeconds()) + " de captura)",
                                    juce::dontSendNotification);
            break;

        case Controller::State::analysing:
            analyseButton.setEnabled (false);
            analysisStatus.setText (utf8 ("Calculando..."), juce::dontSendNotification);
            break;

        case Controller::State::idle:
            analyseButton.setButtonText ("Analisar");
            analyseButton.setEnabled (true);

            if (shownResultVersion <= 0)
                analysisStatus.setText (utf8 ("Clique em Analisar e toque a voz. A análise só mede: não altera o som."),
                                        juce::dontSendNotification);
            break;
    }
}

void LapidamixAudioProcessorEditor::showAnalysisResult (const lapidamix::AnalysisResult& result)
{
    if (! result.isValid)
    {
        analysisResults.setText ({}, juce::dontSendNotification);
        spectrumPlot.setAnalysisCurves ({}, {});
        return;
    }

    auto text = utf8 ("Voz válida: ") + seconds (result.voiceSeconds) + " | Com som: "
              + seconds (result.soundSeconds) + " | Captura: " + seconds (result.capturedSeconds) + "\n";

    if (result.voiceSeconds > 0.0f)
        text << "Fundamental: mediana " << hertz (result.fundamentalMedian)
             << " | grave (5%) " << hertz (result.fundamentalLow)
             << " | aguda (95%) " << hertz (result.fundamentalHigh)
             << utf8 (" | confiança ") << percent (result.confidence) << "\n";
    else
        text << utf8 ("Nenhuma voz válida encontrada.\n");

    if (result.resonances.empty())
    {
        text << utf8 ("Ressonâncias candidatas: nenhuma");

        if (result.numDistinctNotes < 3)
            text << utf8 (" (poucas notas diferentes para avaliar)");

        text << "\n";
    }
    else
    {
        text << utf8 ("Ressonâncias candidatas:");

        // A análise guarda mais candidatas para a regra de corte; a janela mostra só as mais fortes.
        const auto numShown = juce::jmin ((int) result.resonances.size(), maximumShownResonances);

        for (int index = 0; index < numShown; ++index)
        {
            const auto& resonance = result.resonances[(size_t) index];
            text << " " << hertz (resonance.frequency) << " (+" << lapidamix::format::decimal (resonance.prominenceDb, 1)
                 << " dB, " << percent (resonance.presence) << " do tempo)";
        }

        text << "\n";
    }

    if (result.voiceSeconds < lapidamix::AnalysisController::minimumVoiceSeconds)
        text << utf8 ("Atenção: menos de 10 s de voz válida; as medidas podem variar.");

    analysisResults.setText (text, juce::dontSendNotification);
    spectrumPlot.setAnalysisCurves (result.voiceCurve, result.soundCurve);

    analysisStatus.setText (utf8 ("Análise concluída. Nada foi alterado no som."), juce::dontSendNotification);
}

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

    auto analysisRow = area.removeFromTop (rowHeight).reduced (0, 3);
    analyseButton.setBounds (analysisRow.removeFromLeft (nameWidth));
    analysisRow.removeFromLeft (columnGap);
    analysisStatus.setBounds (analysisRow);

    analysisResults.setBounds (area.removeFromTop (resultsHeight));
    spectrumPlot.setBounds (area);
}
