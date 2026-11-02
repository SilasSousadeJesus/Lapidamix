#pragma once

#include <array>
#include <atomic>

#include <juce_audio_processors/juce_audio_processors.h>

#include "core/eq/EqBandFilter.h"
#include "modules/ProcessorModule.h"
#include "profiles/InstrumentProfile.h"

namespace lapidamix
{
    // Primeiro módulo da arquitetura: EQ manual de 5 bandas (D38, D61).
    class EqModule final : public ProcessorModule
    {
    public:
        static juce::String getEnabledParameterId (int bandIndex);
        static juce::String getFrequencyParameterId (int bandIndex);
        static juce::String getGainParameterId (int bandIndex);
        static juce::String getQParameterId (int bandIndex);

        static void addParameters (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                   const InstrumentProfile& profile);

        EqModule (juce::AudioProcessorValueTreeState& parameters, const InstrumentProfile& profile);

        void prepare (const juce::dsp::ProcessSpec& spec) override;
        void reset() override;
        void process (juce::AudioBuffer<float>& buffer) noexcept override;

        void setBypassed (bool shouldBeBypassed) noexcept { bypassed = shouldBeBypassed; }

    private:
        struct BandParameters
        {
            std::atomic<float>* enabled = nullptr;
            std::atomic<float>* frequency = nullptr;
            std::atomic<float>* gainDb = nullptr;
            std::atomic<float>* q = nullptr;
        };

        EqBandSettings readSettings (int bandIndex) const noexcept;

        const InstrumentProfile& profile;
        std::array<BandParameters, numEqBands> bandParameters;
        std::array<EqBandFilter, numEqBands> bands;
        bool bypassed = false;
    };
}
