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

        // A decisão do chiado não depende da taxa do projeto: a mesma análise dá sempre o mesmo plano.
        constexpr double hissSampleRate = 48000.0;

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

        std::vector<juce::dsp::IIR::Coefficients<float>> makePlanFilters (const CorrectionPlan& plan, double sampleRate)
        {
            using ArrayCoefficients = juce::dsp::IIR::ArrayCoefficients<float>;

            std::vector<juce::dsp::IIR::Coefficients<float>> filters;

            if (plan.highPass.isEnabled)
                filters.emplace_back (ArrayCoefficients::makeHighPass (sampleRate, plan.highPass.frequency, 0.707f));

            for (const auto* band : { &plan.mud, &plan.resonance, &plan.presence })
                if (band->isEnabled)
                    filters.emplace_back (ArrayCoefficients::makePeakFilter (sampleRate, band->frequency, band->q, juce::Decibels::decibelsToGain (band->gainDb)));

            if (plan.air.isEnabled)
                filters.emplace_back (ArrayCoefficients::makeHighShelf (sampleRate, plan.air.frequency, plan.air.q, juce::Decibels::decibelsToGain (plan.air.gainDb)));

            return filters;
        }

        double getFiltersPower (const std::vector<juce::dsp::IIR::Coefficients<float>>& filters, float frequency, double sampleRate)
        {
            auto power = 1.0;

            for (const auto& filter : filters)
            {
                const auto magnitude = filter.getMagnitudeForFrequency ((double) frequency, sampleRate);
                power *= magnitude * magnitude;
            }

            return power;
        }

        // Depende das outras bandas: o chiado é medido com o plano inteiro (D108).
        BandCorrection makeAir (const AnalysisResult& analysis, const CorrectionPlan& plan, const CorrectionSettings& settings, float referenceDb)
        {
            const auto& air = settings.air;
            const auto hasNoise = (int) analysis.noiseCurve.size() == spectrum::numDisplayPoints;
            auto airDb = spectrum::getCurveRegionDb (analysis.voiceCurve, air.regionLow, air.regionHigh);

            BandCorrection band;
            band.frequency = air.frequency;
            band.q = air.q;
            band.measuredDb = airDb - referenceDb;

            if (hasNoise)
            {
                const auto noiseDb = spectrum::getCurveRegionDb (analysis.noiseCurve, air.regionLow, air.regionHigh);

                // O ruído soma na medida: sem descontar, chiado seria lido como ar sobrando.
                if (airDb - noiseDb < air.minimumAboveNoiseDb)
                {
                    band.reason = CorrectionReason::airNotMeasurable;
                    return band;
                }

                airDb = 10.0f * std::log10 (std::pow (10.0f, airDb / 10.0f) - std::pow (10.0f, noiseDb / 10.0f));
                band.measuredDb = airDb - referenceDb;
            }

            const auto missingDb = air.minimumLevelDb - band.measuredDb;
            const auto excessDb = band.measuredDb - air.maximumLevelDb;

            if (missingDb >= settings.minimumActionDb)
            {
                band.isEnabled = true;
                band.gainDb = juce::jmin (air.maximumBoostDb, missingDb);
                band.reason = CorrectionReason::applied;

                auto withAir = plan;
                withAir.air = band;

                if (hasNoise && estimateHissDb (analysis, withAir, settings) > air.maximumHissDb)
                {
                    band.gainDb *= air.hissCautionRatio;
                    band.reason = CorrectionReason::reducedForHiss;
                }
            }
            else if (excessDb >= settings.minimumActionDb)
            {
                band.isEnabled = true;
                band.gainDb = -juce::jmin (air.maximumCutDb, excessDb);
                band.reason = CorrectionReason::applied;
            }
            else
            {
                band.reason = CorrectionReason::withinRange;
            }

            return band;
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
        scaleGain (scaled.air);

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

        const auto filters = makePlanFilters (plan, sampleRate);
        const auto before = getPerceivedLevelDb (analysis.soundCurve);
        const auto after = sumPerceivedDb (analysis.soundCurve, [&] (float frequency) { return getFiltersPower (filters, frequency, sampleRate); });

        return before - after;
    }

    float estimateHissDb (const AnalysisResult& analysis, const CorrectionPlan& plan, const CorrectionSettings& settings)
    {
        if ((int) analysis.noiseCurve.size() != spectrum::numDisplayPoints || (int) analysis.soundCurve.size() != spectrum::numDisplayPoints)
            return spectrum::silenceDb;

        const auto filters = makePlanFilters (plan, hissSampleRate);

        const auto voice = sumPerceivedDb (analysis.soundCurve, [&] (float frequency) { return getFiltersPower (filters, frequency, hissSampleRate); });
        const auto hiss = sumPerceivedDb (analysis.noiseCurve, [&] (float frequency)
        {
            return frequency >= settings.air.hissLow && frequency < settings.air.hissHigh ? getFiltersPower (filters, frequency, hissSampleRate) : 0.0;
        });

        // A compensação iguala o volume da voz, então só a diferença importa.
        return hiss - voice;
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
        plan.air = makeAir (analysis, plan, settings, referenceDb);

        return plan;
    }
}
