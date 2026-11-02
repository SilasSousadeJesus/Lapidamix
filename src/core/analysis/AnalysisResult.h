#pragma once

#include <vector>

#include <juce_data_structures/juce_data_structures.h>

namespace lapidamix
{
    struct ResonanceCandidate
    {
        float frequency = 0.0f;
        float prominenceDb = 0.0f;
        float presence = 0.0f; // Parte dos trechos de voz em que o pico aparece, de 0 a 1.
        int numNotes = 0;
        bool isInBothHalves = false; // Aparece nas duas metades da voz capturada (D87).
    };

    // O que a análise mede. Nada aqui altera o som (Fase 3).
    struct AnalysisResult
    {
        bool isValid = false;

        float capturedSeconds = 0.0f;
        float soundSeconds = 0.0f;
        float voiceSeconds = 0.0f;

        float fundamentalMedian = 0.0f;
        float fundamentalLow = 0.0f;  // Percentil de 5%: guiará o passa-alta na Fase 4 (D78).
        float fundamentalHigh = 0.0f; // Percentil de 95%.
        float confidence = 0.0f;
        float octaveJumpShare = 0.0f; // Parte dos blocos vizinhos que salta uma oitava, de 0 a 1 (D94).

        // Em dB, na grade de `spectrum::getDisplayFrequency` (D71).
        std::vector<float> voiceCurve;
        std::vector<float> soundCurve;

        std::vector<ResonanceCandidate> resonances;
        int numDistinctNotes = 0;

        juce::ValueTree toValueTree() const;
        static AnalysisResult fromValueTree (const juce::ValueTree& tree);

        static juce::Identifier getTreeType();
    };
}
