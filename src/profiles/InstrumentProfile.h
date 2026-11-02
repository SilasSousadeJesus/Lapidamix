#pragma once

#include <array>

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
        std::array<EqBandProfile, numEqBands> eqBands;
    };

    // Valores iniciais de partida para a voz, a ajustar nos testes (D62).
    inline constexpr InstrumentProfile voiceProfile {
        "Voz",
        { {
            { "Passa-alta",   EqBandType::highPass,  80.0f,    0.707f, 0.707f, 0.707f, false, false },
            { "Embolado",     EqBandType::bell,      300.0f,   1.0f,   0.3f,   10.0f,  true,  true },
            { "Ressonância",  EqBandType::bell,      1000.0f,  2.0f,   0.3f,   10.0f,  true,  true },
            { "Presença",     EqBandType::bell,      3500.0f,  0.7f,   0.3f,   10.0f,  true,  true },
            { "Ar",           EqBandType::highShelf, 12000.0f, 0.7f,   0.3f,   2.0f,   true,  true },
        } }
    };
}
