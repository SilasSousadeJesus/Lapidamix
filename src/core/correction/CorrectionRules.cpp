#include "core/correction/CorrectionRules.h"

#include <cmath>

#include "core/analysis/SpectrumGrid.h"

namespace lapidamix
{
    namespace
    {
        BandCorrection makeHighPass (const AnalysisResult& analysis, const CorrectionSettings::HighPass& settings)
        {
            BandCorrection band;
            band.isEnabled = true;

            // Um grave medido uma oitava acima subiria o filtro e cortaria o peso da voz (D91).
            if (analysis.confidence < settings.minimumConfidence || analysis.octaveJumpShare > settings.maximumOctaveJumpShare)
            {
                band.frequency = settings.conservativeFrequency;
                band.reason = CorrectionReason::inconsistentFundamental;
                return band;
            }

            band.frequency = juce::jlimit (settings.minimumFrequency, settings.maximumFrequency,
                                           analysis.fundamentalLow / std::pow (2.0f, settings.marginOctaves));
            band.reason = CorrectionReason::applied;
            return band;
        }

        BandCorrection makeMud (const AnalysisResult& analysis, const CorrectionSettings::Mud& settings,
                                float referenceDb, float minimumActionDb)
        {
            BandCorrection band;
            band.q = settings.q;

            // A região começa acima das notas cantadas, para nunca cortar a fundamental (D95).
            const auto regionLow = juce::jmax (settings.regionLow, settings.fundamentalMargin * analysis.fundamentalHigh);

            if (settings.regionHigh / regionLow < settings.minimumRegionRatio)
            {
                band.reason = CorrectionReason::protectedFundamental;
                return band;
            }

            band.frequency = std::sqrt (regionLow * settings.regionHigh);
            band.measuredDb = spectrum::getCurveRegionDb (analysis.voiceCurve, regionLow, settings.regionHigh) - referenceDb;

            const auto excessDb = band.measuredDb - settings.maximumLevelDb;

            if (excessDb < minimumActionDb)
            {
                band.reason = CorrectionReason::withinRange;
                return band;
            }

            band.isEnabled = true;
            band.gainDb = -juce::jmin (settings.maximumCutDb, excessDb);
            band.reason = CorrectionReason::applied;
            return band;
        }

        BandCorrection makeResonance (const AnalysisResult& analysis, const CorrectionSettings::Resonance& settings)
        {
            BandCorrection band;
            band.q = settings.q;
            band.reason = CorrectionReason::noEligiblePeak;

            for (const auto& candidate : analysis.resonances) // Já vêm da mais forte para a mais fraca.
            {
                const auto isProtected = candidate.frequency >= settings.protectedLow && candidate.frequency <= settings.protectedHigh;
                const auto notesShare = (float) candidate.numNotes / (float) juce::jmax (1, analysis.numDistinctNotes);

                if (candidate.prominenceDb >= settings.minimumProminenceDb
                    && candidate.presence >= settings.minimumPresence
                    && notesShare >= settings.minimumNotesShare
                    && candidate.isInBothHalves
                    && candidate.frequency <= settings.maximumFrequency
                    && ! isProtected)
                {
                    band.isEnabled = true;
                    band.frequency = candidate.frequency;
                    band.gainDb = -juce::jmin (settings.maximumCutDb, candidate.prominenceDb * settings.cutRatio);
                    band.measuredDb = candidate.prominenceDb;
                    band.reason = CorrectionReason::applied;
                    return band;
                }
            }

            return band;
        }

        BandCorrection makePresence (const AnalysisResult& analysis, const CorrectionSettings::Presence& settings,
                                     float referenceDb, float minimumActionDb)
        {
            BandCorrection band;
            band.frequency = settings.frequency;
            band.q = settings.q;
            band.measuredDb = spectrum::getCurveRegionDb (analysis.voiceCurve, settings.regionLow, settings.regionHigh) - referenceDb;

            const auto missingDb = settings.minimumLevelDb - band.measuredDb;

            if (missingDb < minimumActionDb)
            {
                band.reason = CorrectionReason::withinRange;
                return band;
            }

            band.isEnabled = true;
            band.gainDb = juce::jmin (settings.maximumBoostDb, missingDb);
            band.reason = CorrectionReason::applied;
            return band;
        }
    }

    CorrectionPlan makeCorrectionPlan (const AnalysisResult& analysis, const CorrectionSettings& settings)
    {
        CorrectionPlan plan;

        // Sem voz medida, nada é proposto.
        if (! analysis.isValid || analysis.voiceSeconds <= 0.0f || (int) analysis.voiceCurve.size() != spectrum::numDisplayPoints)
            return plan;

        const auto referenceDb = spectrum::getCurveRegionDb (analysis.voiceCurve, settings.referenceLow, settings.referenceHigh);

        plan.highPass = makeHighPass (analysis, settings.highPass);
        plan.mud = makeMud (analysis, settings.mud, referenceDb, settings.minimumActionDb);
        plan.resonance = makeResonance (analysis, settings.resonance);
        plan.presence = makePresence (analysis, settings.presence, referenceDb, settings.minimumActionDb);
        plan.air.reason = CorrectionReason::notAutomatic; // D97.

        return plan;
    }
}
