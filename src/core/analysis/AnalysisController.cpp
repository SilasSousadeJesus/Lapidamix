#include "core/analysis/AnalysisController.h"

namespace lapidamix
{
    AnalysisController::AnalysisController()
        : juce::Thread ("Lapidamix analysis")
    {
    }

    AnalysisController::~AnalysisController()
    {
        cancelPendingUpdate();
        abort();
    }

    void AnalysisController::start (const VoiceAnalyzer::Settings& settings)
    {
        abort();

        analyzerSettings = settings;
        stopRequested.store (false);
        voiceSeconds.store (0.0f);
        capturedSeconds.store (0.0f);

        capture.start (juce::roundToInt (settings.sampleRate * maximumCaptureSeconds));
        state.store (State::capturing);
        startThread (juce::Thread::Priority::low);
    }

    void AnalysisController::requestStop() noexcept
    {
        stopRequested.store (true);
    }

    void AnalysisController::abort()
    {
        signalThreadShouldExit();
        stopThread (5000);
        capture.stop();
        state.store (State::idle);
    }

    void AnalysisController::run()
    {
        VoiceAnalyzer analyzer (analyzerSettings);

        while (! threadShouldExit())
        {
            const auto numSamples = capture.getNumSamples();
            analyzer.update (capture.getData(), numSamples);

            voiceSeconds.store (analyzer.getEstimatedVoiceSeconds());
            capturedSeconds.store ((float) (numSamples / analyzerSettings.sampleRate));

            if (stopRequested.load()
                || analyzer.getEstimatedVoiceSeconds() >= targetVoiceSeconds
                || ! capture.isCapturing())
                break;

            wait (50);
        }

        capture.stop();

        if (threadShouldExit())
            return;

        state.store (State::analysing);

        auto result = analyzer.finish (capture.getData(), capture.getNumSamples(),
                                       [this] { return threadShouldExit(); });

        if (threadShouldExit())
            return;

        voiceSeconds.store (result.voiceSeconds);

        {
            const juce::ScopedLock lock (resultLock);
            pendingResult = std::move (result);
        }

        state.store (State::idle);
        triggerAsyncUpdate();
    }

    void AnalysisController::handleAsyncUpdate()
    {
        AnalysisResult result;

        {
            const juce::ScopedLock lock (resultLock);
            result = pendingResult;
        }

        if (onResult != nullptr)
            onResult (result);
    }
}
