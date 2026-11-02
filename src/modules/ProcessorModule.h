#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace lapidamix
{
    // Interface comum dos processadores encaixáveis (D30).
    class ProcessorModule
    {
    public:
        virtual ~ProcessorModule() = default;

        // Chamado fora da thread de áudio: toda alocação acontece aqui.
        virtual void prepare (const juce::dsp::ProcessSpec& spec) = 0;

        virtual void reset() = 0;

        // Chamado na thread de áudio: não pode bloquear (D09).
        virtual void process (juce::AudioBuffer<float>& buffer) noexcept = 0;
    };
}
