#pragma once

#include <array>

#include "core/correction/CorrectionSettings.h"
#include "core/eq/EqBandType.h"

namespace lapidamix
{
    struct EqBandProfile
    {
        const char* name;
        EqBandType type;
        float frequency;
        float q;
        float minimumQ;
        float maximumQ;
        bool hasGain;
        bool hasAdjustableQ;
    };

    inline constexpr int numEqBands = 5;

    struct InstrumentProfile
    {
        const char* name;
        float minimumFundamental;
        float maximumFundamental;
        std::array<EqBandProfile, numEqBands> eqBands;
        CorrectionSettings correction;
        const char* dictionaryResource; // Nome do arquivo de textos embutido no plugin (D92).
    };

    // Valores iniciais de partida para a voz, a ajustar nos testes (D62).
    inline constexpr InstrumentProfile voiceProfile {
        "Voz",
        60.0f,
        1100.0f,
        { {
            { "Passa-alta",   EqBandType::highPass,  80.0f,    0.707f, 0.707f, 0.707f, false, false },
            { "Embolado",     EqBandType::bell,      300.0f,   1.0f,   0.3f,   10.0f,  true,  true },
            { "Ressonância",  EqBandType::bell,      1000.0f,  2.0f,   0.3f,   10.0f,  true,  true },
            { "Presença",     EqBandType::bell,      3500.0f,  0.7f,   0.3f,   10.0f,  true,  true },
            { "Ar",           EqBandType::highShelf, 12000.0f, 0.7f,   0.3f,   2.0f,   true,  true },
        } },
        // Limites provisórios, a calibrar no teste de ouvido (D94 a D98; ressonância: D87, D88).
        // A faixa do ar veio do ouvido de uma voz só: calibrar com outras vozes na Fase 5 (D108).
        {
            500.0f, 2000.0f, 1.0f, 10.0f,
            { 0.5f, 40.0f, 100.0f, 40.0f, 0.85f, 0.08f },
            { 200.0f, 500.0f, 1.25f, 1.25f, 3.0f, 5.0f, 1.0f },
            { 2000.0f, 5000.0f, 3500.0f, -19.0f, 3.0f, 0.7f },
            { 6.0f, 0.6f, 0.5f, 2500.0f, 4200.0f, 6000.0f, 1.0f / 3.0f, 3.0f, 4.0f },
            { 10000.0f, 14000.0f, 12000.0f, 0.7f, -54.0f, -38.0f, 3.0f, 5.0f, 1.0f, 5000.0f, 16000.0f, -50.0f, 0.5f }
        },
        "voicedictionary_json"
    };
}
