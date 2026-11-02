#include "core/analysis/AudioCapture.h"

#include <juce_core/juce_core.h>

namespace lapidamix
{
    void AudioCapture::start (int capacityInSamples)
    {
        stop();

        data.assign ((size_t) capacityInSamples, 0.0f);
        writePosition.store (0, std::memory_order_release);
        capturing.store (true);
    }

    void AudioCapture::stop() noexcept
    {
        capturing.store (false);

        // A thread de áudio fica aqui por poucos microssegundos, e só quem chamou stop() espera.
        while (writerActive.load())
            juce::Thread::yield();
    }

    void AudioCapture::push (const juce::AudioBuffer<float>& buffer) noexcept
    {
        writerActive.store (true);

        if (capturing.load())
        {
            const auto position = writePosition.load (std::memory_order_relaxed);
            const auto capacity = (int) data.size();
            const auto numToWrite = juce::jmin (buffer.getNumSamples(), capacity - position);
            const auto numChannels = buffer.getNumChannels();

            if (numToWrite > 0 && numChannels > 0)
            {
                auto* destination = data.data() + position;
                const auto gain = 1.0f / (float) numChannels;

                juce::FloatVectorOperations::copyWithMultiply (destination, buffer.getReadPointer (0), gain, numToWrite);

                for (int channel = 1; channel < numChannels; ++channel)
                    juce::FloatVectorOperations::addWithMultiply (destination, buffer.getReadPointer (channel), gain, numToWrite);

                writePosition.store (position + numToWrite, std::memory_order_release);
            }

            if (position + numToWrite >= capacity)
                capturing.store (false);
        }

        writerActive.store (false);
    }
}
