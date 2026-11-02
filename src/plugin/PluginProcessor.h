#pragma once

#include <atomic>

#include <juce_audio_processors/juce_audio_processors.h>

#include "core/analysis/AnalysisController.h"
#include "core/correction/CorrectionRules.h"
#include "modules/EqModule.h"
#include "plugin/RtaAnalyzer.h"

// Ligação com o DAW: parâmetros, salvamento e a cadeia de processamento.
class LapidamixAudioProcessor final : public juce::AudioProcessor
{
public:
    static constexpr auto instrumentParameterId = "instrument";
    static constexpr auto bypassParameterId = "bypass";
    static constexpr auto outputGainParameterId = "output_gain";
    static constexpr auto intensityParameterId = "correction_intensity";

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

    lapidamix::AnalysisController& getAnalysis() noexcept { return analysis; }
    RtaAnalyzer& getRta() noexcept { return rta; }

    void startAnalysis();
    lapidamix::AnalysisResult getLastAnalysisResult();
    int getResultVersion() const noexcept { return resultVersion.load(); }

    // O que o plugin pretende fazer com a última análise, na intensidade atual.
    // Nada muda até o usuário aplicar (D85).
    lapidamix::CorrectionPlan getCorrectionPlan();
    float getCompensationDb (const lapidamix::CorrectionPlan& plan);

    // Tudo isto roda só na thread de mensagens.
    // Escreve o plano e a compensação nos parâmetros: fica visível, ajustável à mão e salvo no projeto (D99, D102).
    void applyCorrectionPlan (const lapidamix::CorrectionPlan& plan);
    void applyCurrentCorrection();
    void handleIntensityChange();

private:
    void storeAnalysisResult (const lapidamix::AnalysisResult& result);

    void render (juce::AudioBuffer<float>& buffer) noexcept;
    float getTargetOutputGain() const noexcept;

    juce::AudioProcessorValueTreeState parameters;
    lapidamix::EqModule eq;

    std::atomic<float>* bypassValue = nullptr;
    std::atomic<float>* outputGainDbValue = nullptr;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGain;

    lapidamix::AnalysisController analysis;
    RtaAnalyzer rta;
    std::atomic<double> currentSampleRate { 0.0 };
    std::atomic<int> resultVersion { 0 };

    juce::CriticalSection resultLock; // Nunca usado na thread de áudio.
    lapidamix::AnalysisResult lastResult;

    // A correção aplicada corresponde à última análise? Mover a intensidade só a refaz nesse caso.
    std::atomic<bool> correctionIsApplied { false };
    std::atomic<float> appliedIntensity { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapidamixAudioProcessor)
};
