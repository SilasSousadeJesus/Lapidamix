#pragma once

#include <juce_core/juce_core.h>

namespace lapidamix::format
{
    // Números como o usuário lê em português. O construtor juce::String (valor, 0) não
    // arredonda: com zero casas, ele escreve o número inteiro, com todos os decimais.
    inline juce::String decimal (float value, int numDecimals)
    {
        if (numDecimals <= 0)
            return juce::String (juce::roundToInt (value));

        return juce::String (value, numDecimals).replaceCharacter ('.', ',');
    }

    // Aceita vírgula ou ponto, porque o valor mostrado também pode ser digitado na caixa de texto.
    inline float parseDecimal (const juce::String& text)
    {
        return text.replaceCharacter (',', '.').getFloatValue();
    }
}
