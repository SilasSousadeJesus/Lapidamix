#pragma once

#include <atomic>
#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>

namespace lapidamix
{
    // Guarda o áudio de entrada, em mono, para a análise. `push` roda na thread de áudio
    // e nunca pede memória nem espera (D09); o resto roda fora dela (D28).
    class AudioCapture final
    {
    public:
        // Fora da thread de áudio: toda a memória é reservada aqui.
        void start (int capacityInSamples);

        // Fora da thread de áudio: ao retornar, a thread de áudio não escreve mais.
        void stop() noexcept;

        // Na thread de áudio.
        void push (const juce::AudioBuffer<float>& buffer) noexcept;

        const float* getData() const noexcept { return data.data(); }
        int getNumSamples() const noexcept { return writePosition.load (std::memory_order_acquire); }
        int getCapacity() const noexcept { return (int) data.size(); }
        bool isCapturing() const noexcept { return capturing.load(); }

    private:
        std::vector<float> data;
        std::atomic<int> writePosition { 0 };
        std::atomic<bool> capturing { false };
        std::atomic<bool> writerActive { false };
    };
}
