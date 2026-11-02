#pragma once

#include "core/analysis/AnalysisResult.h"
#include "core/correction/CorrectionSettings.h"

namespace lapidamix
{
    // O motivo de cada decisão, para a explicação ao usuário (D92).
    enum class CorrectionReason
    {
        noAnalysis,
        applied,
        withinRange,
        protectedFundamental,
        inconsistentFundamental,
        noEligiblePeak,
        notAutomatic
    };

    struct BandCorrection
    {
        bool isEnabled = false;
        float frequency = 0.0f;
        float gainDb = 0.0f;
        float q = 0.0f;
        float measuredDb = 0.0f; // O que foi medido, relativo à própria voz.
        CorrectionReason reason = CorrectionReason::noAnalysis;
    };

    // O que o plugin pretende fazer. Nada aqui altera o som: quem aplica é o usuário (D85, D99).
    struct CorrectionPlan
    {
        BandCorrection highPass;
        BandCorrection mud;
        BandCorrection resonance;
        BandCorrection presence;
        BandCorrection air;
    };

    // Regras fixas, com resultado que depende da voz analisada (D25). Sem áudio nem interface.
    CorrectionPlan makeCorrectionPlan (const AnalysisResult& analysis, const CorrectionSettings& settings);
}
