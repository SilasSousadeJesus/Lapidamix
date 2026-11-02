#include "profiles/CorrectionExplanation.h"

#include "LapidamixData.h"
#include "core/DisplayFormat.h"

namespace lapidamix
{
    namespace
    {
        constexpr int highPassBand = 0;
        constexpr int mudBand = 1;
        constexpr int resonanceBand = 2;
        constexpr int presenceBand = 3;
        constexpr int airBand = 4;

        juce::String utf8 (const char* text)
        {
            return juce::String::fromUTF8 (text);
        }

        juce::String hertz (float value)
        {
            return format::decimal (value, 0) + " Hz";
        }

        juce::String decibels (float value)
        {
            return format::decimal (std::abs (value), 1) + " dB";
        }

        // Só nas frases de "por quê": a ação usa Hz, a mesma unidade dos controles da janela.
        juce::String shortFrequency (float value)
        {
            if (value < 1000.0f)
                return hertz (value);

            return (juce::approximatelyEqual (std::fmod (value, 1000.0f), 0.0f) ? format::decimal (value / 1000.0f, 0)
                                                                               : format::decimal (value / 1000.0f, 1)) + " kHz";
        }

        juce::String aboveOrBelow (float value)
        {
            return decibels (value) + (value > 0.0f ? " acima" : " abaixo");
        }

        juce::String middleRegion (const CorrectionSettings& settings)
        {
            return utf8 ("da região média da voz (") + shortFrequency (settings.referenceLow) + " a " + shortFrequency (settings.referenceHigh) + ")";
        }

        juce::String percent (float fraction)
        {
            return format::decimal (fraction * 100.0f, 0) + "%";
        }

        juce::String unchangedEffect()
        {
            return utf8 ("A voz fica como está nessa região.");
        }

        juce::String getEffect (const RegionDictionary& dictionary, float frequency, bool isBoost)
        {
            if (const auto* region = dictionary.findRegion (frequency))
                return isBoost ? region->boostEffect : region->cutEffect;

            return {};
        }

        juce::String describeGain (const BandCorrection& band)
        {
            return (band.gainDb > 0.0f ? utf8 ("Realçar ") : juce::String ("Atenuar ")) + decibels (band.gainDb)
                 + " em " + hertz (band.frequency) + ", Q " + format::decimal (band.q, 1) + ".";
        }

        void addBand (juce::String& text, const char* name, const juce::String& what, const juce::String& why, const juce::String& effect)
        {
            text << "\n\n" << utf8 (name) << ": " << what
                 << utf8 ("\nPor quê: ") << why
                 << "\nEfeito: " << effect;
        }

        void addHighPass (juce::String& text, const CorrectionPlan& plan, const AnalysisResult& analysis,
                          const char* name, const RegionDictionary& dictionary)
        {
            const auto& band = plan.highPass;

            const auto why = band.reason == CorrectionReason::inconsistentFundamental
                ? utf8 ("O plugin ficou em dúvida sobre as notas graves (certeza de ") + percent (analysis.confidence) + ", com "
                    + percent (analysis.octaveJumpShare) + utf8 (" dos trechos parecendo saltar uma oitava). Por segurança, o filtro fica bem embaixo.")
                : utf8 ("As notas mais graves da voz ficam em torno de ") + hertz (analysis.fundamentalLow) + utf8 (". O filtro fica abaixo delas, para não cortar a própria voz.");

            if (! band.isEnabled)
            {
                addBand (text, name, utf8 ("Não filtrar: intensidade em 0%."), why, unchangedEffect());
                return;
            }

            // O filtro age abaixo da própria frequência: o efeito é o da região que ele tira.
            addBand (text, name, "Filtrar abaixo de " + hertz (band.frequency) + ".", why, getEffect (dictionary, band.frequency * 0.5f, false));
        }

        void addMud (juce::String& text, const CorrectionPlan& plan, const AnalysisResult& analysis,
                     const CorrectionSettings& settings, const char* name, const RegionDictionary& dictionary)
        {
            const auto& band = plan.mud;

            if (band.reason == CorrectionReason::protectedFundamental)
            {
                addBand (text, name, utf8 ("Não mexer."),
                         utf8 ("As notas mais agudas da voz chegam perto de ") + hertz (analysis.fundamentalHigh) + utf8 (": um corte aqui atingiria as notas cantadas."),
                         unchangedEffect());
                return;
            }

            const auto why = utf8 ("A região em torno de ") + hertz (band.frequency) + utf8 (" está ") + aboveOrBelow (band.measuredDb) + " " + middleRegion (settings)
                           + ". O aceito vai até " + aboveOrBelow (settings.mud.maximumLevelDb) + ".";

            if (band.isEnabled)
                addBand (text, name, describeGain (band), why, getEffect (dictionary, band.frequency, false));
            else
                addBand (text, name, band.reason == CorrectionReason::applied ? utf8 ("Não mexer: intensidade em 0%.") : utf8 ("Não mexer."), why, unchangedEffect());
        }

        void addResonance (juce::String& text, const CorrectionPlan& plan, const char* name, const RegionDictionary& dictionary)
        {
            const auto& band = plan.resonance;

            if (band.reason != CorrectionReason::applied)
            {
                addBand (text, name, utf8 ("Não mexer."),
                         utf8 ("Nenhuma frequência se destaca o bastante: para agir, ela precisa ser forte e aparecer na maior parte do tempo."),
                         unchangedEffect());
                return;
            }

            const auto why = utf8 ("A frequência de ") + hertz (band.frequency) + utf8 (" fica ") + decibels (band.measuredDb) + utf8 (" acima das vizinhas, na maior parte do tempo.");

            if (band.isEnabled)
                addBand (text, name, describeGain (band), why, getEffect (dictionary, band.frequency, false));
            else
                addBand (text, name, utf8 ("Não mexer: intensidade em 0%."), why, unchangedEffect());
        }

        void addPresence (juce::String& text, const CorrectionPlan& plan, const CorrectionSettings& settings,
                          const char* name, const RegionDictionary& dictionary)
        {
            const auto& band = plan.presence;
            const auto why = utf8 ("A região de ") + shortFrequency (settings.presence.regionLow) + " a " + shortFrequency (settings.presence.regionHigh)
                           + utf8 (" está ") + aboveOrBelow (band.measuredDb) + " " + middleRegion (settings)
                           + ". O aceito começa em " + aboveOrBelow (settings.presence.minimumLevelDb) + ".";

            if (band.isEnabled)
                addBand (text, name, describeGain (band), why, getEffect (dictionary, band.frequency, true));
            else
                addBand (text, name, band.reason == CorrectionReason::applied ? utf8 ("Não mexer: intensidade em 0%.") : utf8 ("Não mexer."), why, unchangedEffect());
        }

        void addOutput (juce::String& text, float compensationDb)
        {
            const auto rounded = juce::roundToInt (compensationDb * 10.0f);

            if (rounded == 0)
            {
                addBand (text, utf8 ("Saída").toRawUTF8(), "Manter em 0 dB.", utf8 ("As correções quase não mudam o volume."), "O volume continua o mesmo.");
                return;
            }

            addBand (text, utf8 ("Saída").toRawUTF8(), (compensationDb > 0.0f ? "Somar " : "Reduzir ") + decibels (compensationDb) + ".",
                     utf8 ("As correções deixam a voz ") + decibels (compensationDb) + (compensationDb > 0.0f ? " mais baixa" : " mais alta")
                         + utf8 (", na medida de volume percebido (LUFS)."),
                     utf8 ("Tende a manter o volume igual, para comparar com o bypass só pela diferença de som."));
        }
    }

    const DictionaryRegion* RegionDictionary::findRegion (float frequency) const noexcept
    {
        for (const auto& region : regions)
            if (frequency >= region.lowFrequency && frequency < region.highFrequency)
                return &region;

        return nullptr;
    }

    RegionDictionary parseRegionDictionary (const juce::String& json)
    {
        RegionDictionary dictionary;
        const auto root = juce::JSON::parse (json);

        if (const auto* regions = root["regions"].getArray())
        {
            for (const auto& item : *regions)
            {
                DictionaryRegion region;
                region.name = item["name"].toString();
                region.lowFrequency = (float) item["low"];
                region.highFrequency = (float) item["high"];
                region.boostEffect = item["boost"].toString();
                region.cutEffect = item["cut"].toString();
                dictionary.regions.push_back (std::move (region));
            }
        }

        return dictionary;
    }

    RegionDictionary loadRegionDictionary (const InstrumentProfile& profile)
    {
        auto size = 0;
        const auto* data = LapidamixData::getNamedResource (profile.dictionaryResource, size);

        return data != nullptr ? parseRegionDictionary (juce::String::fromUTF8 (data, size)) : RegionDictionary {};
    }

    juce::String explainCorrectionPlan (const CorrectionPlan& plan, const AnalysisResult& analysis, float compensationDb,
                                        const InstrumentProfile& profile, const RegionDictionary& dictionary)
    {
        if (! plan.isAvailable)
            return {};

        const auto& bands = profile.eqBands;
        auto text = utf8 ("Correções propostas (só valem depois de clicar em Aplicar):");

        addHighPass (text, plan, analysis, bands[highPassBand].name, dictionary);
        addMud (text, plan, analysis, profile.correction, bands[mudBand].name, dictionary);
        addResonance (text, plan, bands[resonanceBand].name, dictionary);
        addPresence (text, plan, profile.correction, bands[presenceBand].name, dictionary);

        addBand (text, bands[airBand].name, utf8 ("Não age sozinha; ajuste à mão, se quiser."),
                 utf8 ("Esses agudos muito altos ficam perto do chiado da gravação, por isso o plugin não mexe neles sozinho."),
                 utf8 ("Se realçar à mão, ") + getEffect (dictionary, bands[airBand].frequency, true).replaceSection (0, 1, "t"));

        addOutput (text, compensationDb);
        return text;
    }
}
