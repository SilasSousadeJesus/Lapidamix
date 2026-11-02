#include "core/analysis/AnalysisResult.h"

namespace lapidamix
{
    namespace
    {
        juce::String curveToString (const std::vector<float>& curve)
        {
            juce::StringArray values;
            values.ensureStorageAllocated ((int) curve.size());

            for (const auto value : curve)
                values.add (juce::String (value, 1));

            return values.joinIntoString (" ");
        }

        std::vector<float> stringToCurve (const juce::String& text)
        {
            const auto values = juce::StringArray::fromTokens (text, " ", {});

            std::vector<float> curve;
            curve.reserve ((size_t) values.size());

            for (const auto& value : values)
                curve.push_back (value.getFloatValue());

            return curve;
        }
    }

    juce::Identifier AnalysisResult::getTreeType()
    {
        return "AnalysisResult";
    }

    juce::ValueTree AnalysisResult::toValueTree() const
    {
        juce::ValueTree tree (getTreeType());
        tree.setProperty ("capturedSeconds", capturedSeconds, nullptr);
        tree.setProperty ("soundSeconds", soundSeconds, nullptr);
        tree.setProperty ("voiceSeconds", voiceSeconds, nullptr);
        tree.setProperty ("fundamentalMedian", fundamentalMedian, nullptr);
        tree.setProperty ("fundamentalLow", fundamentalLow, nullptr);
        tree.setProperty ("fundamentalHigh", fundamentalHigh, nullptr);
        tree.setProperty ("confidence", confidence, nullptr);
        tree.setProperty ("octaveJumpShare", octaveJumpShare, nullptr);
        tree.setProperty ("numDistinctNotes", numDistinctNotes, nullptr);
        tree.setProperty ("voiceCurve", curveToString (voiceCurve), nullptr);
        tree.setProperty ("soundCurve", curveToString (soundCurve), nullptr);
        tree.setProperty ("noiseCurve", curveToString (noiseCurve), nullptr);

        for (const auto& resonance : resonances)
        {
            juce::ValueTree child ("Resonance");
            child.setProperty ("frequency", resonance.frequency, nullptr);
            child.setProperty ("prominenceDb", resonance.prominenceDb, nullptr);
            child.setProperty ("presence", resonance.presence, nullptr);
            child.setProperty ("numNotes", resonance.numNotes, nullptr);
            child.setProperty ("isInBothHalves", resonance.isInBothHalves, nullptr);
            tree.appendChild (child, nullptr);
        }

        return tree;
    }

    AnalysisResult AnalysisResult::fromValueTree (const juce::ValueTree& tree)
    {
        AnalysisResult result;

        if (! tree.hasType (getTreeType()))
            return result;

        result.isValid = true;
        result.capturedSeconds = tree.getProperty ("capturedSeconds");
        result.soundSeconds = tree.getProperty ("soundSeconds");
        result.voiceSeconds = tree.getProperty ("voiceSeconds");
        result.fundamentalMedian = tree.getProperty ("fundamentalMedian");
        result.fundamentalLow = tree.getProperty ("fundamentalLow");
        result.fundamentalHigh = tree.getProperty ("fundamentalHigh");
        result.confidence = tree.getProperty ("confidence");
        result.octaveJumpShare = tree.getProperty ("octaveJumpShare"); // Projetos antigos não têm: fica 0.
        result.numDistinctNotes = tree.getProperty ("numDistinctNotes");
        result.voiceCurve = stringToCurve (tree.getProperty ("voiceCurve").toString());
        result.soundCurve = stringToCurve (tree.getProperty ("soundCurve").toString());
        result.noiseCurve = stringToCurve (tree.getProperty ("noiseCurve").toString()); // Projetos antigos não têm: fica sem pausa medida.

        for (const auto& child : tree)
        {
            ResonanceCandidate resonance;
            resonance.frequency = child.getProperty ("frequency");
            resonance.prominenceDb = child.getProperty ("prominenceDb");
            resonance.presence = child.getProperty ("presence");
            resonance.numNotes = child.getProperty ("numNotes");
            resonance.isInBothHalves = child.getProperty ("isInBothHalves"); // Projetos antigos não têm: fica falso, e nada é cortado.
            result.resonances.push_back (resonance);
        }

        return result;
    }
}
