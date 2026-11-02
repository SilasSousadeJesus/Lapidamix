#include "plugin/RtaAnalyzer.h"

#include <algorithm>

namespace
{
    constexpr float displaySmoothing = 0.6f;
}

RtaAnalyzer::RtaAnalyzer()
    : fifoData ((size_t) fifoSize, 0.0f),
      history ((size_t) fftSize, 0.0f),
      fftData ((size_t) fftSize * 2, 0.0f),
      powers ((size_t) fftSize / 2 + 1, 0.0f)
{
    target.fill (lapidamix::spectrum::silenceDb);
    display.fill (lapidamix::spectrum::silenceDb);
}

void RtaAnalyzer::push (const juce::AudioBuffer<float>& buffer) noexcept
{
    if (! active.load (std::memory_order_relaxed))
        return;

    const auto numChannels = buffer.getNumChannels();

    if (numChannels == 0)
        return;

    const auto gain = 1.0f / (float) numChannels;
    const auto scope = fifo.write (buffer.getNumSamples());

    const auto writeBlock = [&] (int destinationStart, int sourceStart, int numSamples)
    {
        if (numSamples <= 0)
            return;

        auto* destination = fifoData.data() + destinationStart;
        juce::FloatVectorOperations::copyWithMultiply (destination, buffer.getReadPointer (0, sourceStart), gain, numSamples);

        for (int channel = 1; channel < numChannels; ++channel)
            juce::FloatVectorOperations::addWithMultiply (destination, buffer.getReadPointer (channel, sourceStart), gain, numSamples);
    };

    // Se a fila encher, as amostras que não couberem são descartadas: só o gráfico sente.
    writeBlock (scope.startIndex1, 0, scope.blockSize1);
    writeBlock (scope.startIndex2, scope.blockSize1, scope.blockSize2);
}

const RtaAnalyzer::Curve& RtaAnalyzer::update()
{
    const auto numReady = fifo.getNumReady();

    if (numReady > 0)
    {
        const auto scope = fifo.read (numReady);

        const auto appendBlock = [this] (int start, int numSamples)
        {
            if (numSamples <= 0)
                return;

            const auto numToKeep = juce::jmin (numSamples, fftSize);
            const auto* source = fifoData.data() + start + (numSamples - numToKeep);

            std::move (history.begin() + numToKeep, history.end(), history.begin());
            std::copy (source, source + numToKeep, history.end() - numToKeep);
        };

        appendBlock (scope.startIndex1, scope.blockSize1);
        appendBlock (scope.startIndex2, scope.blockSize2);

        std::copy (history.begin(), history.end(), fftData.begin());
        std::fill (fftData.begin() + fftSize, fftData.end(), 0.0f);
        window.multiplyWithWindowingTable (fftData.data(), (size_t) fftSize);
        fft.performFrequencyOnlyForwardTransform (fftData.data(), true);

        const auto normalisation = 16.0f / ((float) fftSize * (float) fftSize);

        for (size_t bin = 0; bin < powers.size(); ++bin)
            powers[bin] = fftData[bin] * fftData[bin] * normalisation;

        lapidamix::spectrum::fillDisplayCurve (powers.data(), (int) powers.size(), sampleRate.load() / fftSize, target);
    }

    for (size_t point = 0; point < display.size(); ++point)
        display[point] += (target[point] - display[point]) * displaySmoothing;

    return display;
}
