#include "core/correction/CorrectionRules.h"

#include <cmath>

#include <juce_dsp/juce_dsp.h>

#include "core/Loudness.h"
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

        // A grade é logarítmica: cada ponto representa uma faixa proporcional à frequência.
        template <typename GetPower>
        float sumPerceivedDb (const std::vector<float>& curve, GetPower getBandPower)
        {
            auto sum = 0.0;

            for (int point = 0; point < spectrum::numDisplayPoints; ++point)
            {
                const auto frequency = spectrum::getDisplayFrequency (point);
                sum += std::pow (10.0, curve[(size_t) point] / 10.0) * loudness::getKWeightingPower (frequency) * frequency * getBandPower (frequency);
            }

            return 10.0f * (float) std::log10 (sum + 1.0e-20);
        }
    }

    CorrectionPlan scaleCorrectionPlan (const CorrectionPlan& plan, float intensity, const CorrectionSettings& settings)
    {
        auto scaled = plan;
        const auto isOn = intensity > 0.0f;

        const auto scaleGain = [&] (BandCorrection& band)
        {
            band.gainDb *= intensity;
            band.isEnabled = band.isEnabled && isOn;
        };

        scaleGain (scaled.mud);
        scaleGain (scaled.resonance);
        scaleGain (scaled.presence);

        const auto lowest = settings.highPass.minimumFrequency;

        if (plan.highPass.frequency > 0.0f)
            scaled.highPass.frequency = lowest * std::pow (plan.highPass.frequency / lowest, intensity);

        scaled.highPass.isEnabled = plan.highPass.isEnabled && isOn;
        return scaled;
    }

    float getPerceivedLevelDb (const std::vector<float>& curve)
    {
        if ((int) curve.size() != spectrum::numDisplayPoints)
            return spectrum::silenceDb;

        return sumPerceivedDb (curve, [] (float) { return 1.0; });
    }

    float estimateCompensationDb (const AnalysisResult& analysis, const CorrectionPlan& plan, double sampleRate)
    {
        if (! plan.isAvailable || (int) analysis.soundCurve.size() != spectrum::numDisplayPoints)
            return 0.0f;

        using Filter = juce::dsp::IIR::Coefficients<float>;
        using ArrayCoefficients = juce::dsp::IIR::ArrayCoefficients<float>;

        std::vector<Filter> filters;

        // A banda de ar não entra: o plano não mexe nela (D97).
        if (plan.highPass.isEnabled)
            filters.emplace_back (ArrayCoefficients::makeHighPass (sampleRate, plan.highPass.frequency, 0.707f));

        for (const auto* band : { &plan.mud, &plan.resonance, &plan.presence })
            if (band->isEnabled)
                filters.emplace_back (ArrayCoefficients::makePeakFilter (sampleRate, band->frequency, band->q, juce::Decibels::decibelsToGain (band->gainDb)));

        const auto before = getPerceivedLevelDb (analysis.soundCurve);
        const auto after = sumPerceivedDb (analysis.soundCurve, [&] (float frequency)
        {
            auto power = 1.0;

            for (const auto& filter : filters)
            {
                const auto magnitude = filter.getMagnitudeForFrequency ((double) frequency, sampleRate);
                power *= magnitude * magnitude;
            }

            return power;
        });

        return before - after;
    }

    CorrectionPlan makeCorrectionPlan (const AnalysisResult& analysis, const CorrectionSettings& settings)
    {
        CorrectionPlan plan;

        // Com pouca voz, as medidas variam demais para decidir um corte (D101).
        if (! analysis.isValid || analysis.voiceSeconds < settings.minimumVoiceSeconds || (int) analysis.voiceCurve.size() != spectrum::numDisplayPoints)
            return plan;

        plan.isAvailable = true;

        const auto referenceDb = spectrum::getCurveRegionDb (analysis.voiceCurve, settings.referenceLow, settings.referenceHigh);

        plan.highPass = makeHighPass (analysis, settings.highPass);
        plan.mud = makeMud (analysis, settings.mud, referenceDb, settings.minimumActionDb);
        plan.resonance = makeResonance (analysis, settings.resonance);
        plan.presence = makePresence (analysis, settings.presence, referenceDb, settings.minimumActionDb);
        plan.air.reason = CorrectionReason::notAutomatic; // D97.

        return plan;
    }
}
