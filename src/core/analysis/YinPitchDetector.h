#pragma once

#include <vector>

namespace lapidamix
{
    // Detecção da fundamental pelo método YIN (de Cheveigné e Kawahara, 2002), escrita a
    // partir do artigo (D72). Roda só fora da thread de áudio.
    class YinPitchDetector final
    {
    public:
        struct Result
        {
            float frequency = 0.0f;
            float aperiodicity = 1.0f; // Perto de 0: muito periódico. Perto de 1: parecido com ruído.
        };

        void prepare (double sampleRate, float minimumFrequency, float maximumFrequency);

        int getFrameSize() const noexcept { return windowSize + maximumLag + 1; }

        Result detect (const float* frame) noexcept;

    private:
        static constexpr float absoluteThreshold = 0.15f;

        double sampleRate = 11025.0;
        int minimumLag = 1;
        int maximumLag = 2;
        int windowSize = 2;
        std::vector<float> normalisedDifference;
    };
}
