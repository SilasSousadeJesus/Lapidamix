#include "core/eq/EqBandFilter.h"

namespace lapidamix
{
    namespace
    {
        constexpr double parameterSmoothingSeconds = 0.05;
        constexpr double mixSmoothingSeconds = 0.02;
        constexpr float minimumFrequency = 20.0f;
        constexpr float highestFrequency = 20000.0f;
        constexpr float maximumFrequencyRatio = 0.45f;
    }

    void EqBandFilter::prepare (EqBandType bandType,
                                double newSampleRate,
                                juce::uint32 maximumBlockSize,
                                const EqBandSettings& settings)
    {
        type = bandType;
        sampleRate = newSampleRate;
        maximumFrequency = juce::jmin (highestFrequency, maximumFrequencyRatio * (float) sampleRate);

        frequency.reset (sampleRate, parameterSmoothingSeconds);
        q.reset (sampleRate, parameterSmoothingSeconds);
        gainDb.reset (sampleRate, parameterSmoothingSeconds);
        mix.reset (sampleRate, mixSmoothingSeconds);

        frequency.setCurrentAndTargetValue (limitFrequency (settings.frequency));
        q.setCurrentAndTargetValue (settings.q);
        gainDb.setCurrentAndTargetValue (settings.gainDb);
        mix.setCurrentAndTargetValue (settings.enabled ? 1.0f : 0.0f);

        // Os coeficientes nascem aqui, com a ordem definitiva: mudar a ordem pediria memória.
        coefficients = new Coefficients (juce::dsp::IIR::ArrayCoefficients<float>::makeAllPass (sampleRate, 1000.0f));
        updateCoefficients (0);

        const juce::dsp::ProcessSpec spec { sampleRate, maximumBlockSize, 1 };

        for (auto& filter : filters)
        {
            filter.coefficients = coefficients;
            filter.prepare (spec);
        }
    }

    void EqBandFilter::reset() noexcept
    {
        for (auto& filter : filters)
            filter.reset();
    }

    void EqBandFilter::setSettings (const EqBandSettings& settings) noexcept
    {
        const auto limitedFrequency = limitFrequency (settings.frequency);

        mix.setTargetValue (settings.enabled ? 1.0f : 0.0f);

        if (isSilentlyOff())
        {
            // Desligada, a banda não precisa de transição: ao ligar, já começa no valor certo.
            frequency.setCurrentAndTargetValue (limitedFrequency);
            q.setCurrentAndTargetValue (settings.q);
            gainDb.setCurrentAndTargetValue (settings.gainDb);
            coefficientsNeedUpdate = true;
            return;
        }

        frequency.setTargetValue (limitedFrequency);
        q.setTargetValue (settings.q);
        gainDb.setTargetValue (settings.gainDb);
    }

    void EqBandFilter::process (juce::AudioBuffer<float>& buffer) noexcept
    {
        if (isSilentlyOff())
            return;

        const auto numChannels = juce::jmin (buffer.getNumChannels(), maximumNumChannels);
        const auto numSamples = buffer.getNumSamples();
        auto* const* channels = buffer.getArrayOfWritePointers();

        for (int start = 0; start < numSamples; start += coefficientUpdateInterval)
        {
            const auto blockEnd = juce::jmin (start + coefficientUpdateInterval, numSamples);

            if (coefficientsNeedUpdate || frequency.isSmoothing() || q.isSmoothing() || gainDb.isSmoothing())
                updateCoefficients (blockEnd - start);

            for (int sample = start; sample < blockEnd; ++sample)
            {
                const auto currentMix = mix.getNextValue();

                for (int channel = 0; channel < numChannels; ++channel)
                {
                    const auto dry = channels[channel][sample];
                    const auto wet = filters[(size_t) channel].processSample (dry);
                    channels[channel][sample] = dry + currentMix * (wet - dry);
                }
            }
        }

        for (auto& filter : filters)
            filter.snapToZero();

        if (isSilentlyOff())
            reset();
    }

    float EqBandFilter::limitFrequency (float newFrequency) const noexcept
    {
        return juce::jlimit (minimumFrequency, maximumFrequency, newFrequency);
    }

    bool EqBandFilter::isSilentlyOff() const noexcept
    {
        return ! mix.isSmoothing() && juce::exactlyEqual (mix.getTargetValue(), 0.0f);
    }

    void EqBandFilter::updateCoefficients (int numSamples) noexcept
    {
        const auto currentFrequency = frequency.skip (numSamples);
        const auto currentQ = q.skip (numSamples);
        const auto gainFactor = juce::Decibels::decibelsToGain (gainDb.skip (numSamples));

        using ArrayCoefficients = juce::dsp::IIR::ArrayCoefficients<float>;

        // ArrayCoefficients não pede memória, e a atribuição reaproveita a já existente (D60).
        switch (type)
        {
            case EqBandType::highPass:
                *coefficients = ArrayCoefficients::makeHighPass (sampleRate, currentFrequency, currentQ);
                break;

            case EqBandType::bell:
                *coefficients = ArrayCoefficients::makePeakFilter (sampleRate, currentFrequency, currentQ, gainFactor);
                break;

            case EqBandType::highShelf:
                *coefficients = ArrayCoefficients::makeHighShelf (sampleRate, currentFrequency, currentQ, gainFactor);
                break;
        }

        coefficientsNeedUpdate = false;
    }
}
