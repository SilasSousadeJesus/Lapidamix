#include "core/analysis/YinPitchDetector.h"

#include <juce_core/juce_core.h>

namespace lapidamix
{
    void YinPitchDetector::prepare (double newSampleRate, float minimumFrequency, float maximumFrequency)
    {
        sampleRate = newSampleRate;
        minimumLag = juce::jmax (2, (int) (sampleRate / maximumFrequency));
        maximumLag = juce::jmax (minimumLag + 2, (int) std::ceil (sampleRate / minimumFrequency));
        windowSize = maximumLag;
        normalisedDifference.assign ((size_t) maximumLag + 2, 1.0f);
    }

    YinPitchDetector::Result YinPitchDetector::detect (const float* frame) noexcept
    {
        auto* difference = normalisedDifference.data();
        difference[0] = 1.0f;

        // Função de diferença, normalizada pela média acumulada (etapas 2 e 3 do artigo).
        auto runningSum = 0.0f;

        for (int lag = 1; lag <= maximumLag + 1; ++lag)
        {
            auto sum = 0.0f;

            for (int index = 0; index < windowSize; ++index)
            {
                const auto delta = frame[index] - frame[index + lag];
                sum += delta * delta;
            }

            runningSum += sum;
            difference[lag] = runningSum > 0.0f ? sum * (float) lag / runningSum : 1.0f;
        }

        // Primeiro vale abaixo do limite; se não houver, o menor valor da faixa (etapas 4 e 5).
        auto bestLag = -1;

        for (int lag = minimumLag; lag <= maximumLag; ++lag)
        {
            if (difference[lag] < absoluteThreshold)
            {
                while (lag < maximumLag && difference[lag + 1] < difference[lag])
                    ++lag;

                bestLag = lag;
                break;
            }
        }

        if (bestLag < 0)
        {
            bestLag = minimumLag;

            for (int lag = minimumLag + 1; lag <= maximumLag; ++lag)
                if (difference[lag] < difference[bestLag])
                    bestLag = lag;
        }

        // Interpolação entre amostras, para não limitar a precisão à taxa de amostragem.
        const auto previous = difference[bestLag - 1];
        const auto current = difference[bestLag];
        const auto next = difference[bestLag + 1];
        const auto denominator = previous - 2.0f * current + next;
        const auto offset = denominator > 1.0e-9f ? juce::jlimit (-1.0f, 1.0f, 0.5f * (previous - next) / denominator) : 0.0f;

        Result result;
        result.frequency = (float) (sampleRate / ((double) bestLag + offset));
        result.aperiodicity = juce::jlimit (0.0f, 1.0f, current);
        return result;
    }
}
