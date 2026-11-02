#pragma once

#include <cmath>

namespace lapidamix::loudness
{
    struct Biquad
    {
        double b0, b1, b2, a1, a2;
    };

    // Ponderação K da ITU-R BS.1770, com os coeficientes publicados para 48 kHz (D103).
    inline constexpr double kWeightingSampleRate = 48000.0;
    inline constexpr Biquad kWeightingShelf { 1.53512485958697, -2.69169618940638, 1.19839281085285, -1.69065929318241, 0.73248077421585 };
    inline constexpr Biquad kWeightingHighPass { 1.0, -2.0, 1.0, -1.99004745483398, 0.99007225036621 };

    inline double getBiquadPower (const Biquad& filter, double frequency, double sampleRate) noexcept
    {
        const auto angle = 2.0 * 3.14159265358979323846 * frequency / sampleRate;
        const auto cos1 = std::cos (angle);
        const auto cos2 = std::cos (2.0 * angle);

        const auto numerator = filter.b0 * filter.b0 + filter.b1 * filter.b1 + filter.b2 * filter.b2
                             + 2.0 * (filter.b0 * filter.b1 + filter.b1 * filter.b2) * cos1
                             + 2.0 * filter.b0 * filter.b2 * cos2;
        const auto denominator = 1.0 + filter.a1 * filter.a1 + filter.a2 * filter.a2
                               + 2.0 * (filter.a1 + filter.a1 * filter.a2) * cos1
                               + 2.0 * filter.a2 * cos2;

        return numerator / denominator;
    }

    // Quanto a ponderação K multiplica a potência nessa frequência. Vale para qualquer taxa de amostragem,
    // porque só pesa uma curva de espectro: nenhum áudio passa por estes filtros no plugin.
    inline double getKWeightingPower (double frequency) noexcept
    {
        return getBiquadPower (kWeightingShelf, frequency, kWeightingSampleRate)
             * getBiquadPower (kWeightingHighPass, frequency, kWeightingSampleRate);
    }
}
