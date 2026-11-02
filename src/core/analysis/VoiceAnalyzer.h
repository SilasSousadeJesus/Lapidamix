#pragma once

#include <array>
#include <functional>
#include <vector>

#include <juce_dsp/juce_dsp.h>

#include "core/analysis/AnalysisResult.h"
#include "core/analysis/YinPitchDetector.h"

namespace lapidamix
{
    // Mede uma gravação em mono, sem depender da interface nem do DAW (D76).
    // Roda só fora da thread de áudio (D28).
    class VoiceAnalyzer final
    {
    public:
        struct Settings
        {
            double sampleRate = 44100.0;
            float minimumFundamental = 60.0f;
            float maximumFundamental = 1100.0f;
        };

        explicit VoiceAnalyzer (const Settings& settings);

        // Processa as amostras novas de `samples`, que só cresce entre as chamadas.
        void update (const float* samples, int numSamples);

        // Segundos de voz válida até aqui; o resultado final recalcula tudo com a gravação inteira.
        float getEstimatedVoiceSeconds() const noexcept;

        AnalysisResult finish (const float* samples, int numSamples, const std::function<bool()>& shouldExit);

    private:
        struct Block
        {
            float power = 0.0f;
            float frequency = 0.0f;
            float aperiodicity = 1.0f;
        };

        void decimateNewSamples (const float* samples, int numSamples);
        void analyseCompleteBlocks (const float* samples, int numSamples);
        void classifyBlocks (std::vector<char>& hasSound, std::vector<char>& isVoice) const;

        Settings settings;
        int blockSize = 1;
        int decimationFactor = 1;
        double decimatedRate = 44100.0;
        int numDecimatedInput = 0;

        juce::dsp::IIR::Filter<float> antiAliasA;
        juce::dsp::IIR::Filter<float> antiAliasB;
        std::vector<float> decimated;

        YinPitchDetector pitchDetector;
        std::vector<Block> blocks;

        double runningPowerSum = 0.0;
        int runningPowerCount = 0;
        int runningVoiceBlocks = 0;
    };
}
