#pragma once

#include <atomic>
#include <functional>

#include <juce_events/juce_events.h>

#include "core/analysis/AudioCapture.h"
#include "core/analysis/VoiceAnalyzer.h"

namespace lapidamix
{
    // Coordena a captura e a análise, numa thread separada da de áudio (D28, D69, D75).
    class AnalysisController final : private juce::Thread,
                                     private juce::AsyncUpdater
    {
    public:
        static constexpr float targetVoiceSeconds = 30.0f;
        static constexpr double maximumCaptureSeconds = 120.0;

        enum class State
        {
            idle,
            capturing,
            analysing
        };

        // Chamado na thread de mensagens quando há um resultado novo.
        std::function<void (const AnalysisResult&)> onResult;

        AnalysisController();
        ~AnalysisController() override;

        void start (const VoiceAnalyzer::Settings& settings);
        void requestStop() noexcept;
        void abort();

        State getState() const noexcept { return state.load(); }
        float getVoiceSeconds() const noexcept { return voiceSeconds.load(); }
        float getCapturedSeconds() const noexcept { return capturedSeconds.load(); }

        // Na thread de áudio.
        void pushInput (const juce::AudioBuffer<float>& buffer) noexcept { capture.push (buffer); }

    private:
        void run() override;
        void handleAsyncUpdate() override;

        AudioCapture capture;
        VoiceAnalyzer::Settings analyzerSettings;

        std::atomic<State> state { State::idle };
        std::atomic<bool> stopRequested { false };
        std::atomic<float> voiceSeconds { 0.0f };
        std::atomic<float> capturedSeconds { 0.0f };

        juce::CriticalSection resultLock;
        AnalysisResult pendingResult;
    };
}
