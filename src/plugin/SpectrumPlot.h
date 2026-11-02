#pragma once

#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "plugin/RtaAnalyzer.h"

// Gráfico simples: RTA ao vivo e as duas médias da análise (D71, D74).
class SpectrumPlot final : public juce::Component
{
public:
    void setRtaCurve (const RtaAnalyzer::Curve& curve);
    void setAnalysisCurves (std::vector<float> voice, std::vector<float> sound);

    void paint (juce::Graphics& g) override;

private:
    float getX (float frequency) const noexcept;
    float getY (float decibels) const noexcept;

    template <typename Curve>
    void drawCurve (juce::Graphics& g, const Curve& curve, juce::Colour colour, float thickness) const;

    RtaAnalyzer::Curve rtaCurve {};
    std::vector<float> voiceCurve;
    std::vector<float> soundCurve;
};
