#include "plugin/SpectrumPlot.h"

#include <cmath>

namespace
{
    constexpr float topDb = 0.0f;
    constexpr float bottomDb = -90.0f;

    const juce::Colour rtaColour { 0xff6f7f8f };
    const juce::Colour voiceColour { 0xfff2a33a };
    const juce::Colour soundColour { 0xff4cc2a0 };
}

void SpectrumPlot::setRtaCurve (const RtaAnalyzer::Curve& curve)
{
    rtaCurve = curve;
    repaint();
}

void SpectrumPlot::setAnalysisCurves (std::vector<float> voice, std::vector<float> sound)
{
    voiceCurve = std::move (voice);
    soundCurve = std::move (sound);
    repaint();
}

float SpectrumPlot::getX (float frequency) const noexcept
{
    using namespace lapidamix::spectrum;
    return (float) getWidth() * std::log (frequency / displayMinimumFrequency)
         / std::log (displayMaximumFrequency / displayMinimumFrequency);
}

float SpectrumPlot::getY (float decibels) const noexcept
{
    return juce::jmap (decibels, topDb, bottomDb, 0.0f, (float) getHeight());
}

template <typename Curve>
void SpectrumPlot::drawCurve (juce::Graphics& g, const Curve& curve, juce::Colour colour, float thickness) const
{
    if (curve.size() != lapidamix::spectrum::numDisplayPoints)
        return;

    juce::Path path;

    for (int point = 0; point < lapidamix::spectrum::numDisplayPoints; ++point)
    {
        const auto x = getX (lapidamix::spectrum::getDisplayFrequency (point));
        const auto y = getY (juce::jlimit (bottomDb, topDb, curve[(size_t) point]));

        if (point == 0)
            path.startNewSubPath (x, y);
        else
            path.lineTo (x, y);
    }

    g.setColour (colour);
    g.strokePath (path, juce::PathStrokeType (thickness));
}

void SpectrumPlot::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.35f));

    g.setFont (11.0f);

    for (const auto frequency : { 100.0f, 1000.0f, 10000.0f })
    {
        const auto x = getX (frequency);
        g.setColour (juce::Colours::white.withAlpha (0.2f));
        g.drawVerticalLine ((int) x, 0.0f, (float) getHeight());
        g.setColour (juce::Colours::white.withAlpha (0.6f));
        g.drawText (frequency >= 1000.0f ? juce::String (frequency / 1000.0f, 0) + " kHz" : juce::String (frequency, 0) + " Hz",
                    (int) x + 3, getHeight() - 15, 50, 14, juce::Justification::centredLeft);
    }

    for (auto decibels = -30.0f; decibels > bottomDb; decibels -= 30.0f)
    {
        const auto y = getY (decibels);
        g.setColour (juce::Colours::white.withAlpha (0.2f));
        g.drawHorizontalLine ((int) y, 0.0f, (float) getWidth());
        g.setColour (juce::Colours::white.withAlpha (0.6f));
        g.drawText (juce::String (decibels, 0) + " dB", 3, (int) y + 1, 50, 14, juce::Justification::centredLeft);
    }

    drawCurve (g, rtaCurve, rtaColour, 1.0f);
    drawCurve (g, soundCurve, soundColour, 1.5f);
    drawCurve (g, voiceCurve, voiceColour, 2.0f);

    auto legend = getLocalBounds().removeFromTop (16).reduced (6, 0);
    const auto drawLegend = [&g, &legend] (const juce::String& text, juce::Colour colour, int width)
    {
        g.setColour (colour);
        g.drawText (text, legend.removeFromRight (width), juce::Justification::centredRight);
    };

    drawLegend ("RTA (saída)", rtaColour, 90);
    drawLegend (juce::String::fromUTF8 ("Média: todo o som"), soundColour, 130);
    drawLegend (juce::String::fromUTF8 ("Média: voz"), voiceColour, 90);
}
