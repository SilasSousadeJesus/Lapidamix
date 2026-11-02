#pragma once

#include <juce_core/juce_core.h>

namespace lapidamix
{
    // Lista de instrumentos que o usuário pode escolher. Na v1.0, só "Voz" (D30).
    inline juce::StringArray getInstrumentNames()
    {
        return { "Voz" };
    }
}
