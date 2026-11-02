#pragma once

#include <array>
#include <atomic>
#include <vector>

#include <juce_dsp/juce_dsp.h>

#include "core/analysis/SpectrumGrid.h"

// RTA simples de diagnóstico (D74). `push` roda na thread de áudio, sem pedir memória nem
// usar travas; `update` roda na thread de mensagens, e só com a janela aberta.
class RtaAnalyzer final
{
public:
    using Curve = std::array<float, lapidamix::spectrum::numDisplayPoints>;

    RtaAnalyzer();

    void setActive (bool shouldBeActive) noexcept { active.store (shouldBeActive); }
    void setSampleRate (double newSampleRate) noexcept { sampleRate.store (newSampleRate); }

    void push (const juce::AudioBuffer<float>& buffer) noexcept;

    const Curve& update();

private:
    static constexpr int fftOrder = 12;
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int fifoSize = 1 << 16;

    std::atomic<bool> active { false };
    std::atomic<double> sampleRate { 44100.0 };

    juce::AbstractFifo fifo { fifoSize };
    std::vector<float> fifoData;

    juce::dsp::FFT fft { fftOrder };
    juce::dsp::WindowingFunction<float> window { (size_t) fftSize, juce::dsp::WindowingFunction<float>::hann, false };

    std::vector<float> history;
    std::vector<float> fftData;
    std::vector<float> powers;
    Curve target {};
    Curve display {};
};
