#include "modules/EqModule.h"

namespace lapidamix
{
    namespace
    {
        constexpr int parameterVersion = 1;
        constexpr float minimumFrequency = 20.0f;
        constexpr float maximumFrequency = 20000.0f;
        constexpr float maximumGainDb = 12.0f;

        juce::String getBandParameterId (int bandIndex, const char* suffix)
        {
            return "eq_band" + juce::String (bandIndex + 1) + "_" + suffix;
        }

        juce::String getBandParameterName (const EqBandProfile& band, const char* suffix)
        {
            return juce::String::fromUTF8 (band.name) + " - " + juce::String::fromUTF8 (suffix);
        }

        juce::AudioParameterFloatAttributes withDecimals (const char* label, int numDecimals)
        {
            return juce::AudioParameterFloatAttributes()
                .withLabel (label)
                .withStringFromValueFunction ([numDecimals] (float value, int)
                {
                    return juce::String (value, numDecimals);
                });
        }

        juce::NormalisableRange<float> makeSkewedRange (float minimum, float maximum, float centre)
        {
            juce::NormalisableRange<float> range { minimum, maximum };
            range.setSkewForCentre (centre);
            return range;
        }
    }

    juce::String EqModule::getEnabledParameterId (int bandIndex)   { return getBandParameterId (bandIndex, "enabled"); }
    juce::String EqModule::getFrequencyParameterId (int bandIndex) { return getBandParameterId (bandIndex, "frequency"); }
    juce::String EqModule::getGainParameterId (int bandIndex)      { return getBandParameterId (bandIndex, "gain"); }
    juce::String EqModule::getQParameterId (int bandIndex)         { return getBandParameterId (bandIndex, "q"); }

    void EqModule::addParameters (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                  const InstrumentProfile& profile)
    {
        for (int bandIndex = 0; bandIndex < numEqBands; ++bandIndex)
        {
            const auto& band = profile.eqBands[(size_t) bandIndex];

            layout.add (std::make_unique<juce::AudioParameterBool> (
                juce::ParameterID { getEnabledParameterId (bandIndex), parameterVersion },
                getBandParameterName (band, "Ligada"),
                false));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { getFrequencyParameterId (bandIndex), parameterVersion },
                getBandParameterName (band, "Frequência"),
                makeSkewedRange (minimumFrequency, maximumFrequency, 1000.0f),
                band.frequency,
                withDecimals ("Hz", 0)));

            if (band.hasGain)
                layout.add (std::make_unique<juce::AudioParameterFloat> (
                    juce::ParameterID { getGainParameterId (bandIndex), parameterVersion },
                    getBandParameterName (band, "Ganho"),
                    juce::NormalisableRange<float> { -maximumGainDb, maximumGainDb, 0.1f },
                    0.0f,
                    withDecimals ("dB", 1)));

            if (band.hasAdjustableQ)
                layout.add (std::make_unique<juce::AudioParameterFloat> (
                    juce::ParameterID { getQParameterId (bandIndex), parameterVersion },
                    getBandParameterName (band, "Q"),
                    makeSkewedRange (band.minimumQ, band.maximumQ, 1.0f),
                    band.q,
                    withDecimals ("", 2)));
        }
    }

    EqModule::EqModule (juce::AudioProcessorValueTreeState& parameters, const InstrumentProfile& instrumentProfile)
        : profile (instrumentProfile)
    {
        for (int bandIndex = 0; bandIndex < numEqBands; ++bandIndex)
        {
            const auto& band = profile.eqBands[(size_t) bandIndex];
            auto& params = bandParameters[(size_t) bandIndex];

            params.enabled = parameters.getRawParameterValue (getEnabledParameterId (bandIndex));
            params.frequency = parameters.getRawParameterValue (getFrequencyParameterId (bandIndex));

            if (band.hasGain)
                params.gainDb = parameters.getRawParameterValue (getGainParameterId (bandIndex));

            if (band.hasAdjustableQ)
                params.q = parameters.getRawParameterValue (getQParameterId (bandIndex));
        }
    }

    void EqModule::prepare (const juce::dsp::ProcessSpec& spec)
    {
        for (int bandIndex = 0; bandIndex < numEqBands; ++bandIndex)
            bands[(size_t) bandIndex].prepare (profile.eqBands[(size_t) bandIndex].type,
                                               spec.sampleRate,
                                               spec.maximumBlockSize,
                                               readSettings (bandIndex));
    }

    void EqModule::reset()
    {
        for (auto& band : bands)
            band.reset();
    }

    void EqModule::process (juce::AudioBuffer<float>& buffer) noexcept
    {
        for (int bandIndex = 0; bandIndex < numEqBands; ++bandIndex)
        {
            auto& band = bands[(size_t) bandIndex];
            band.setSettings (readSettings (bandIndex));
            band.process (buffer);
        }
    }

    EqBandSettings EqModule::readSettings (int bandIndex) const noexcept
    {
        const auto& params = bandParameters[(size_t) bandIndex];
        const auto& band = profile.eqBands[(size_t) bandIndex];

        EqBandSettings settings;
        settings.enabled = ! bypassed && params.enabled->load (std::memory_order_relaxed) >= 0.5f;
        settings.frequency = params.frequency->load (std::memory_order_relaxed);
        settings.gainDb = params.gainDb != nullptr ? params.gainDb->load (std::memory_order_relaxed) : 0.0f;
        settings.q = params.q != nullptr ? params.q->load (std::memory_order_relaxed) : band.q;

        return settings;
    }
}
