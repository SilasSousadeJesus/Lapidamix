#pragma once

#include <array>
#include <cmath>

#include <juce_core/juce_core.h>

namespace lapidamix::spectrum
{
    // Grade logarítmica comum às curvas da análise e do RTA.
    inline constexpr int numDisplayPoints = 240;
    inline constexpr float displayMinimumFrequency = 20.0f;
    inline constexpr float displayMaximumFrequency = 20000.0f;
    inline constexpr float silenceDb = -120.0f;

    inline float getDisplayFrequency (int pointIndex) noexcept
    {
        const auto position = (float) pointIndex / (float) (numDisplayPoints - 1);
        return displayMinimumFrequency * std::pow (displayMaximumFrequency / displayMinimumFrequency, position);
    }

    inline float powerToDb (float power) noexcept
    {
        return power > 1.0e-12f ? 10.0f * std::log10 (power) : silenceDb;
    }

    // Média de potência na faixa de `octaveWidth` oitavas em torno da frequência (D71).
    // `powers` tem `numBins` valores, de 0 até a metade da taxa de amostragem.
    inline float getBandPower (const float* powers, int numBins, double binWidth, float frequency, float octaveWidth) noexcept
    {
        const auto ratio = std::pow (2.0f, octaveWidth * 0.5f);
        const auto first = juce::jlimit (0, numBins - 1, (int) std::ceil ((frequency / ratio) / (float) binWidth));
        const auto last = juce::jlimit (0, numBins - 1, (int) std::floor ((frequency * ratio) / (float) binWidth));

        if (last < first)
            return powers[juce::jlimit (0, numBins - 1, juce::roundToInt (frequency / (float) binWidth))];

        auto sum = 0.0f;

        for (int bin = first; bin <= last; ++bin)
            sum += powers[bin];

        return sum / (float) (last - first + 1);
    }

    // Nível de uma região numa curva já em dB, com a média feita em potência (D71).
    template <typename Curve>
    float getCurveRegionDb (const Curve& curve, float lowFrequency, float highFrequency) noexcept
    {
        auto sum = 0.0f;
        auto count = 0;

        for (int point = 0; point < numDisplayPoints; ++point)
        {
            const auto frequency = getDisplayFrequency (point);

            if (frequency >= lowFrequency && frequency < highFrequency)
            {
                sum += std::pow (10.0f, curve[(size_t) point] / 10.0f);
                ++count;
            }
        }

        return count > 0 ? powerToDb (sum / (float) count) : silenceDb;
    }

    // Curva em dB, com suavização de 1/6 de oitava feita em potência.
    template <typename Output>
    void fillDisplayCurve (const float* powers, int numBins, double binWidth, Output& output) noexcept
    {
        for (int point = 0; point < numDisplayPoints; ++point)
            output[(size_t) point] = powerToDb (getBandPower (powers, numBins, binWidth, getDisplayFrequency (point), 1.0f / 6.0f));
    }
}
