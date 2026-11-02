#pragma once

#include <array>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include "core/eq/EqBandType.h"

namespace lapidamix
{
    struct EqBandSettings
    {
        bool enabled = false;
        float frequency = 1000.0f;
        float gainDb = 0.0f;
        float q = 0.707f;
    };

    // Uma banda de EQ: um filtro biquad por canal, com parâmetros suavizados (D59, D60).
    class EqBandFilter final
    {
    public:
        void prepare (EqBandType bandType,
                      double newSampleRate,
                      juce::uint32 maximumBlockSize,
                      const EqBandSettings& settings);

        void reset() noexcept;

        void setSettings (const EqBandSettings& settings) noexcept;

        // Banda desligada não processa o áudio (D64).
        void process (juce::AudioBuffer<float>& buffer) noexcept;

    private:
        static constexpr int coefficientUpdateInterval = 32;
        static constexpr int maximumNumChannels = 2;

        using Filter = juce::dsp::IIR::Filter<float>;
        using Coefficients = juce::dsp::IIR::Coefficients<float>;

        float limitFrequency (float frequency) const noexcept;
        bool isSilentlyOff() const noexcept;
        void updateCoefficients (int numSamples) noexcept;

        EqBandType type = EqBandType::bell;
        double sampleRate = 44100.0;
        float maximumFrequency = 20000.0f;
        bool coefficientsNeedUpdate = true;

        Coefficients::Ptr coefficients;
        std::array<Filter, maximumNumChannels> filters;

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> frequency;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> q;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainDb;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix;
    };
}
