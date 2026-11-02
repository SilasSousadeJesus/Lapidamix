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
        reducedForHiss,
        airNotMeasurable
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
        bool isAvailable = false; // Falso sem análise ou com pouca voz: nada é proposto (D101).

        BandCorrection highPass;
        BandCorrection mud;
        BandCorrection resonance;
        BandCorrection presence;
        BandCorrection air;
    };

    // Regras fixas, com resultado que depende da voz analisada (D25). Sem áudio nem interface.
    CorrectionPlan makeCorrectionPlan (const AnalysisResult& analysis, const CorrectionSettings& settings);

    // Intensidade de 0 a 1: escala os ganhos (D98). No passa-alta, sobe em oitavas a partir do mínimo (D100).
    CorrectionPlan scaleCorrectionPlan (const CorrectionPlan& plan, float intensity, const CorrectionSettings& settings);

    // Volume de uma curva de espectro, com a ponderação K do LUFS (D103). Serve para comparar antes e depois:
    // sem os portões da BS.1770, o valor sozinho não é uma leitura de LUFS.
    float getPerceivedLevelDb (const std::vector<float>& curve);

    // Valor fixo que deixa o volume igual ao de antes do plano, calculado a partir da análise (D102).
    float estimateCompensationDb (const AnalysisResult& analysis, const CorrectionPlan& plan, double sampleRate);

    // Chiado das pausas contra o volume da voz, em dB, com o plano e a compensação aplicados (D108).
    // Sem pausa medida, devolve o valor de silêncio.
    float estimateHissDb (const AnalysisResult& analysis, const CorrectionPlan& plan, const CorrectionSettings& settings);
}
