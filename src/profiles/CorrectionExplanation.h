#pragma once

#include <vector>

#include <juce_core/juce_core.h>

#include "core/analysis/AnalysisResult.h"
#include "core/correction/CorrectionRules.h"
#include "profiles/InstrumentProfile.h"

namespace lapidamix
{
    struct DictionaryRegion
    {
        juce::String name;
        float lowFrequency = 0.0f;
        float highFrequency = 0.0f;
        juce::String boostEffect;
        juce::String cutEffect;
    };

    // O que cada região tende a causar. Os textos ficam num arquivo UTF-8, embutido ao compilar (D92).
    struct RegionDictionary
    {
        std::vector<DictionaryRegion> regions;

        const DictionaryRegion* findRegion (float frequency) const noexcept;
    };

    RegionDictionary parseRegionDictionary (const juce::String& json);
    RegionDictionary loadRegionDictionary (const InstrumentProfile& profile);

    // O que será feito, por que e o efeito, em cada banda. O texto vale antes e depois do Aplicar (D92).
    juce::String explainCorrectionPlan (const CorrectionPlan& plan, const AnalysisResult& analysis, float compensationDb,
                                        const InstrumentProfile& profile, const RegionDictionary& dictionary);
}
