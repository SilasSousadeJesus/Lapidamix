#include "core/analysis/VoiceAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <set>

#include "core/analysis/SpectrumGrid.h"

namespace lapidamix
{
    namespace
    {
        // Valores iniciais, a calibrar nos testes (D70, D71, D83).
        constexpr double blockSeconds = 0.025;
        constexpr float absoluteSilenceDb = -60.0f;
        constexpr float relativeSilenceDb = -20.0f;
        constexpr float maximumVoiceAperiodicity = 0.3f;
        constexpr double pitchSampleRate = 11025.0;

        constexpr double spectrumFrameSeconds = 0.186;
        constexpr float minimumFrameCoverage = 0.8f;

        constexpr float resonanceMinimumFrequency = 150.0f;
        constexpr float resonanceMaximumFrequency = 8000.0f;
        constexpr int gridPointsPerOctave = 24;
        constexpr float neighbourhoodOctaves = 1.0f;
        constexpr float harmonicSearchRatio = 0.02f;
        constexpr float maximumHarmonicSearch = 0.4f;
        constexpr int minimumNeighbours = 3;
        constexpr int minimumObservations = 8;
        constexpr float minimumProminenceDb = 4.0f; // Medido nos harmônicos (D83).
        constexpr float minimumHarmonicNumber = 6.0f; // A partir daqui, os harmônicos ficam a menos de 1/4 de oitava (D83).
        constexpr float framePresenceDb = 2.0f;
        constexpr float minimumPresence = 0.3f;
        constexpr int minimumNotes = 3;
        constexpr int narrownessOffset = gridPointsPerOctave / 4;
        constexpr float maximumShoulderRatio = 0.5f;
        constexpr size_t maximumCandidates = 20; // A regra de corte precisa ver além das mais fortes (D87).
        constexpr float halvesMatchOctaves = 1.0f / 12.0f;
        constexpr float octaveJumpMargin = 0.1f;

        // Ruído nas pausas (D108).
        constexpr float digitalSilenceDb = -100.0f;
        constexpr float noiseFloorShare = 0.02f; // O piso é o nível dos 2% de quadros mais fracos.
        constexpr float noiseFloorMarginDb = 6.0f;
        constexpr float minimumPauseDepthDb = 35.0f;
        constexpr int minimumPauseFrames = 8;
        constexpr int manyPauseFrames = 100; // Tanta pausa assim vale, mesmo com chiado forte encurtando a distância até a voz.

        // Limites em dB valem para potência: 10 dB, não 20 dB, multiplicam a potência por 10.
        float decibelsToPower (float decibels) noexcept
        {
            return std::pow (10.0f, decibels / 10.0f);
        }

        float getPercentile (std::vector<float> values, float fraction)
        {
            if (values.empty())
                return 0.0f;

            const auto index = (size_t) juce::jlimit (0, (int) values.size() - 1,
                                                      juce::roundToInt (fraction * (float) (values.size() - 1)));
            std::nth_element (values.begin(), values.begin() + (std::ptrdiff_t) index, values.end());
            return values[index];
        }

        // Tira blocos isolados, que costumam ser erro de classificação.
        void applyMedianOfThree (std::vector<char>& flags)
        {
            const auto original = flags;

            for (size_t index = 1; index + 1 < flags.size(); ++index)
                flags[index] = (original[index - 1] + original[index] + original[index + 1]) >= 2 ? 1 : 0;
        }

        // Quanto cada ponto da grade se destaca nos harmônicos das notas cantadas. Medir só nos
        // harmônicos evita que o intervalo entre eles, ou uma coincidência de notas, pareça um pico.
        struct ResonanceObservations
        {
            std::vector<float> deviationSum;
            std::vector<int> count;
            std::vector<int> countAboveFrame;
            std::vector<std::set<int>> notes;

            explicit ResonanceObservations (int numPoints)
                : deviationSum ((size_t) numPoints, 0.0f),
                  count ((size_t) numPoints, 0),
                  countAboveFrame ((size_t) numPoints, 0),
                  notes ((size_t) numPoints)
            {
            }
        };

        void addFrameObservations (ResonanceObservations& observations,
                                   const float* powers, int numBins, double binWidth, float fundamental, int note)
        {
            struct Harmonic
            {
                float frequency;
                float power;
            };

            std::vector<Harmonic> harmonics;
            const auto searchLimit = resonanceMaximumFrequency * std::pow (2.0f, neighbourhoodOctaves * 0.5f);

            for (int number = 1; (float) number * fundamental <= searchLimit; ++number)
            {
                const auto frequency = (float) number * fundamental;
                // A busca não pode alcançar o harmônico vizinho, senão um pico vaza para os lados.
                const auto margin = juce::jmin (juce::jmax (1.5f * (float) binWidth, frequency * harmonicSearchRatio),
                                                fundamental * maximumHarmonicSearch);
                const auto first = juce::jlimit (0, numBins - 1, (int) ((frequency - margin) / (float) binWidth));
                const auto last = juce::jlimit (0, numBins - 1, (int) std::ceil ((frequency + margin) / (float) binWidth));
                auto peak = 0.0f;

                for (int bin = first; bin <= last; ++bin)
                    peak = juce::jmax (peak, powers[bin]);

                harmonics.push_back ({ frequency, peak });
            }

            const auto ratio = std::pow (2.0f, neighbourhoodOctaves * 0.5f);

            for (const auto& harmonic : harmonics)
            {
                if (harmonic.frequency < resonanceMinimumFrequency || harmonic.frequency > resonanceMaximumFrequency || harmonic.power <= 0.0f)
                    continue;

                // Mediana dos vizinhos de cada lado: o próprio pico não a puxa para cima, e um pico
                // precisa se destacar dos dois lados. Um degrau no espectro só se destaca de um.
                std::vector<float> below, above;

                for (const auto& other : harmonics)
                {
                    if (other.frequency >= harmonic.frequency / ratio && other.frequency < harmonic.frequency)
                        below.push_back (other.power);
                    else if (other.frequency > harmonic.frequency && other.frequency <= harmonic.frequency * ratio)
                        above.push_back (other.power);
                }

                if (below.empty() || above.empty() || (int) (below.size() + above.size()) < minimumNeighbours)
                    continue;

                const auto medianBelow = getPercentile (std::move (below), 0.5f);
                const auto medianAbove = getPercentile (std::move (above), 0.5f);

                if (medianBelow <= 0.0f || medianAbove <= 0.0f)
                    continue;

                // O ponto médio entre os dois lados compensa a queda natural do espectro nos agudos.
                const auto reference = std::sqrt (medianBelow * medianAbove);
                const auto isAboveBothSides = harmonic.power > medianBelow && harmonic.power > medianAbove;

                const auto point = juce::roundToInt (std::log2 (harmonic.frequency / resonanceMinimumFrequency) * gridPointsPerOctave);

                if (! juce::isPositiveAndBelow (point, (int) observations.count.size()))
                    continue;

                const auto deviation = isAboveBothSides ? spectrum::powerToDb (harmonic.power / reference) : 0.0f;

                observations.deviationSum[(size_t) point] += deviation;
                ++observations.count[(size_t) point];
                observations.notes[(size_t) point].insert (note);

                if (deviation >= framePresenceDb)
                    ++observations.countAboveFrame[(size_t) point];
            }
        }

        float getGridFrequency (int point) noexcept
        {
            return resonanceMinimumFrequency * std::pow (2.0f, (float) point / (float) gridPointsPerOctave);
        }

        // Ressonâncias: picos estreitos em relação à vizinhança da própria voz (D14, D83).
        // Da mais forte para a mais fraca.
        std::vector<ResonanceCandidate> findResonances (const ResonanceObservations& observations, float fundamentalMedian)
        {
            const auto numGridPoints = (int) observations.count.size();

            // Cada ponto reúne também os vizinhos diretos, porque as notas não caem sempre no mesmo ponto.
            const auto getProminence = [&] (int point)
            {
                auto sum = 0.0f;
                auto count = 0;

                for (int index = juce::jmax (0, point - 1); index <= juce::jmin (numGridPoints - 1, point + 1); ++index)
                {
                    sum += observations.deviationSum[(size_t) index];
                    count += observations.count[(size_t) index];
                }

                return count >= minimumObservations ? sum / (float) count : 0.0f;
            };

            std::vector<ResonanceCandidate> resonances;

            for (int point = 1; point + 1 < numGridPoints; ++point)
            {
                // Com harmônicos mais espaçados que isso, não dá para medir a largura do pico.
                if (getGridFrequency (point) < minimumHarmonicNumber * fundamentalMedian)
                    continue;

                const auto value = getProminence (point);

                if (value < minimumProminenceDb || value < getProminence (point - 1) || value < getProminence (point + 1))
                    continue;

                // Um formante é largo: continua alto a 1/4 de oitava dos dois lados. Uma ressonância não.
                const auto below = getProminence (juce::jmax (0, point - narrownessOffset));
                const auto above = getProminence (juce::jmin (numGridPoints - 1, point + narrownessOffset));

                if (juce::jmin (below, above) > value * maximumShoulderRatio)
                    continue;

                auto observed = 0;
                auto observedAbove = 0;
                std::set<int> notes;

                for (int index = juce::jmax (0, point - 1); index <= juce::jmin (numGridPoints - 1, point + 1); ++index)
                {
                    observed += observations.count[(size_t) index];
                    observedAbove += observations.countAboveFrame[(size_t) index];
                    notes.insert (observations.notes[(size_t) index].begin(), observations.notes[(size_t) index].end());
                }

                const auto presence = (float) observedAbove / (float) observed;

                if (presence >= minimumPresence && (int) notes.size() >= minimumNotes)
                    resonances.push_back ({ getGridFrequency (point), value, presence, (int) notes.size() });
            }

            std::sort (resonances.begin(), resonances.end(),
                       [] (const ResonanceCandidate& a, const ResonanceCandidate& b) { return a.prominenceDb > b.prominenceDb; });

            return resonances;
        }

        // Média dos quadros sem voz perto do piso. Fica vazia sem pausa de verdade: o quadro mais fraco
        // de um canto contínuo ainda é voz ou consoante, não ruído.
        template <typename Fft, typename Window>
        std::vector<float> measureNoiseCurve (const float* samples, int numSamples, int frameSize, int hopSize, int blockSize,
                                              const std::vector<char>& isVoice, Fft& fft, Window& window, float normalisation, double binWidth)
        {
            struct Frame
            {
                float levelDb;
                int start;
            };

            std::vector<Frame> quietCandidates;
            std::vector<float> allLevels;

            for (int start = 0; start + frameSize <= numSamples; start += hopSize)
            {
                auto sum = 0.0f;

                for (int index = 0; index < frameSize; ++index)
                    sum += samples[start + index] * samples[start + index];

                const auto levelDb = spectrum::powerToDb (sum / (float) frameSize);

                if (levelDb <= digitalSilenceDb)
                    continue;

                allLevels.push_back (levelDb);

                const auto firstBlock = start / blockSize;
                const auto lastBlock = juce::jmin ((int) isVoice.size() - 1, (start + frameSize - 1) / blockSize);
                auto hasVoice = false;

                for (int block = firstBlock; block <= lastBlock; ++block)
                    hasVoice = hasVoice || isVoice[(size_t) block] != 0;

                if (! hasVoice)
                    quietCandidates.push_back ({ levelDb, start });
            }

            if (quietCandidates.empty())
                return {};

            std::sort (quietCandidates.begin(), quietCandidates.end(), [] (const Frame& a, const Frame& b) { return a.levelDb < b.levelDb; });

            // O piso vem de todos os quadros: só entre os sem voz, ele cairia, e um trecho sem pausa pareceria ter pausa.
            const auto floorDb = getPercentile (allLevels, noiseFloorShare);
            auto numFrames = 0;

            for (const auto& frame : quietCandidates)
                if (frame.levelDb <= floorDb + noiseFloorMarginDb)
                    ++numFrames;

            const auto isDeepEnough = getPercentile (std::move (allLevels), 0.5f) - floorDb >= minimumPauseDepthDb;

            if (numFrames < manyPauseFrames && (numFrames < minimumPauseFrames || ! isDeepEnough))
                return {};

            const auto numBins = frameSize / 2 + 1;
            std::vector<float> fftData ((size_t) frameSize * 2);
            std::vector<float> power ((size_t) numBins, 0.0f);

            for (int index = 0; index < numFrames; ++index)
            {
                const auto start = quietCandidates[(size_t) index].start;
                std::fill (fftData.begin(), fftData.end(), 0.0f);
                std::copy (samples + start, samples + start + frameSize, fftData.begin());
                window.multiplyWithWindowingTable (fftData.data(), (size_t) frameSize);
                fft.performFrequencyOnlyForwardTransform (fftData.data(), true);

                for (int bin = 0; bin < numBins; ++bin)
                    power[(size_t) bin] += fftData[(size_t) bin] * fftData[(size_t) bin] * normalisation / (float) numFrames;
            }

            std::vector<float> curve (spectrum::numDisplayPoints);
            spectrum::fillDisplayCurve (power.data(), numBins, binWidth, curve);
            return curve;
        }

        bool hasResonanceNear (const std::vector<ResonanceCandidate>& resonances, float frequency) noexcept
        {
            for (const auto& resonance : resonances)
                if (std::abs (std::log2 (resonance.frequency / frequency)) <= halvesMatchOctaves)
                    return true;

            return false;
        }
    }

    VoiceAnalyzer::VoiceAnalyzer (const Settings& newSettings)
        : settings (newSettings)
    {
        blockSize = juce::jmax (1, juce::roundToInt (settings.sampleRate * blockSeconds));
        decimationFactor = juce::jmax (1, (int) (settings.sampleRate / pitchSampleRate));
        decimatedRate = settings.sampleRate / decimationFactor;

        const auto cutoff = (float) (decimatedRate * 0.4);
        const auto coefficients = new juce::dsp::IIR::Coefficients<float> (
            juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass (settings.sampleRate, cutoff));

        antiAliasA.coefficients = coefficients;
        antiAliasB.coefficients = coefficients;

        const juce::dsp::ProcessSpec spec { settings.sampleRate, 4096, 1 };
        antiAliasA.prepare (spec);
        antiAliasB.prepare (spec);

        pitchDetector.prepare (decimatedRate, settings.minimumFundamental, settings.maximumFundamental);
    }

    void VoiceAnalyzer::update (const float* samples, int numSamples)
    {
        decimateNewSamples (samples, numSamples);
        analyseCompleteBlocks (samples, numSamples);
    }

    void VoiceAnalyzer::decimateNewSamples (const float* samples, int numSamples)
    {
        for (int index = numDecimatedInput; index < numSamples; ++index)
        {
            const auto filtered = antiAliasB.processSample (antiAliasA.processSample (samples[index]));

            if (index % decimationFactor == 0)
                decimated.push_back (filtered);
        }

        numDecimatedInput = juce::jmax (numDecimatedInput, numSamples);
    }

    void VoiceAnalyzer::analyseCompleteBlocks (const float* samples, int numSamples)
    {
        const auto frameSize = pitchDetector.getFrameSize();
        const auto decimatedBlock = (double) blockSize / decimationFactor;
        const auto absoluteThreshold = decibelsToPower (absoluteSilenceDb);

        for (;;)
        {
            const auto blockIndex = (int) blocks.size();
            const auto blockEnd = (blockIndex + 1) * blockSize;
            const auto centre = (blockIndex + 0.5) * decimatedBlock;
            const auto frameStart = juce::jmax (0, (int) (centre - frameSize * 0.5));

            if (blockEnd > numSamples || frameStart + frameSize > (int) decimated.size())
                break;

            Block block;
            auto sum = 0.0f;

            for (int index = blockIndex * blockSize; index < blockEnd; ++index)
                sum += samples[index] * samples[index];

            block.power = sum / (float) blockSize;

            const auto pitch = pitchDetector.detect (decimated.data() + frameStart);
            block.frequency = pitch.frequency;
            block.aperiodicity = pitch.aperiodicity;
            blocks.push_back (block);

            // Estimativa para concluir a captura; o resultado final reclassifica tudo.
            if (block.power > absoluteThreshold)
            {
                runningPowerSum += block.power;
                ++runningPowerCount;

                const auto relativeThreshold = (float) (runningPowerSum / runningPowerCount)
                                             * decibelsToPower (relativeSilenceDb);

                if (block.power > relativeThreshold && block.aperiodicity < maximumVoiceAperiodicity)
                    ++runningVoiceBlocks;
            }
        }
    }

    float VoiceAnalyzer::getEstimatedVoiceSeconds() const noexcept
    {
        return (float) (runningVoiceBlocks * blockSize / settings.sampleRate);
    }

    void VoiceAnalyzer::classifyBlocks (std::vector<char>& hasSound, std::vector<char>& isVoice) const
    {
        const auto absoluteThreshold = decibelsToPower (absoluteSilenceDb);
        auto sum = 0.0;
        auto count = 0;

        for (const auto& block : blocks)
        {
            if (block.power > absoluteThreshold)
            {
                sum += block.power;
                ++count;
            }
        }

        const auto relativeThreshold = count > 0 ? (float) (sum / count) * decibelsToPower (relativeSilenceDb)
                                                 : 0.0f;

        hasSound.assign (blocks.size(), 0);
        isVoice.assign (blocks.size(), 0);

        for (size_t index = 0; index < blocks.size(); ++index)
        {
            const auto& block = blocks[index];

            if (block.power > absoluteThreshold && block.power > relativeThreshold)
            {
                hasSound[index] = 1;

                if (block.aperiodicity < maximumVoiceAperiodicity
                    && block.frequency >= settings.minimumFundamental
                    && block.frequency <= settings.maximumFundamental)
                    isVoice[index] = 1;
            }
        }

        applyMedianOfThree (hasSound);
        applyMedianOfThree (isVoice);
    }

    AnalysisResult VoiceAnalyzer::finish (const float* samples, int numSamples, const std::function<bool()>& shouldExit)
    {
        update (samples, numSamples);

        std::vector<char> hasSound, isVoice;
        classifyBlocks (hasSound, isVoice);

        AnalysisResult result;
        result.isValid = true;
        result.capturedSeconds = (float) (numSamples / settings.sampleRate);

        const auto blockLength = (float) (blockSize / settings.sampleRate);
        std::vector<float> frequencies;
        auto confidenceSum = 0.0f;
        auto numSoundBlocks = 0;

        for (size_t index = 0; index < blocks.size(); ++index)
        {
            numSoundBlocks += hasSound[index];

            if (isVoice[index] != 0)
            {
                frequencies.push_back (blocks[index].frequency);
                confidenceSum += 1.0f - blocks[index].aperiodicity;
            }
        }

        result.soundSeconds = (float) numSoundBlocks * blockLength;
        result.voiceSeconds = (float) frequencies.size() * blockLength;

        if (! frequencies.empty())
        {
            result.fundamentalMedian = getPercentile (frequencies, 0.5f);
            result.fundamentalLow = getPercentile (frequencies, 0.05f);
            result.fundamentalHigh = getPercentile (frequencies, 0.95f);
            result.confidence = confidenceSum / (float) frequencies.size();
        }

        // Saltos de oitava entre blocos de voz vizinhos deixam a medida do grave suspeita (D94).
        auto numVoicePairs = 0;
        auto numOctaveJumps = 0;
        std::vector<int> voiceBlocksBefore (blocks.size() + 1, 0);

        for (size_t index = 0; index < blocks.size(); ++index)
        {
            voiceBlocksBefore[index + 1] = voiceBlocksBefore[index] + isVoice[index];

            if (index > 0 && isVoice[index] != 0 && isVoice[index - 1] != 0)
            {
                const auto ratio = blocks[index].frequency / blocks[index - 1].frequency;
                ++numVoicePairs;

                if (std::abs (ratio - 2.0f) < 2.0f * octaveJumpMargin || std::abs (ratio - 0.5f) < 0.5f * octaveJumpMargin)
                    ++numOctaveJumps;
            }
        }

        if (numVoicePairs > 0)
            result.octaveJumpShare = (float) numOctaveJumps / (float) numVoicePairs;

        // Espectro médio pelo método de Welch: média em potência, só depois em dB (D71).
        const auto order = juce::roundToInt (std::log2 (settings.sampleRate * spectrumFrameSeconds));
        const auto frameSize = 1 << order;
        const auto hopSize = frameSize / 4;
        const auto numBins = frameSize / 2 + 1;
        const auto binWidth = settings.sampleRate / frameSize;

        juce::dsp::FFT fft (order);
        juce::dsp::WindowingFunction<float> window ((size_t) frameSize, juce::dsp::WindowingFunction<float>::hann, false);

        // Um seno de amplitude 1 vira 0 dB: a janela de Hann reduz a amplitude pela metade.
        const auto normalisation = 16.0f / ((float) frameSize * (float) frameSize);

        std::vector<float> fftData ((size_t) frameSize * 2);
        std::vector<float> framePower ((size_t) numBins);
        std::vector<float> voicePower ((size_t) numBins, 0.0f);
        std::vector<float> soundPower ((size_t) numBins, 0.0f);

        const auto numGridPoints = (int) std::floor (std::log2 (resonanceMaximumFrequency / resonanceMinimumFrequency) * gridPointsPerOctave) + 1;

        // As duas metades da voz válida, para saber se um pico persiste na captura (D87).
        ResonanceObservations observations (numGridPoints), firstHalf (numGridPoints), secondHalf (numGridPoints);
        const auto halfVoiceBlocks = (int) frequencies.size() / 2;
        std::set<int> allNotes;
        auto numVoiceFrames = 0;
        auto numSoundFrames = 0;

        for (int start = 0; start + frameSize <= numSamples; start += hopSize)
        {
            if (shouldExit != nullptr && shouldExit())
                return {};

            const auto firstBlock = start / blockSize;
            const auto lastBlock = juce::jmin ((int) blocks.size() - 1, (start + frameSize - 1) / blockSize);

            if (lastBlock < firstBlock)
                break;

            auto voiceCount = 0;
            auto soundCount = 0;
            std::vector<float> noteFrequencies;

            for (int block = firstBlock; block <= lastBlock; ++block)
            {
                voiceCount += isVoice[(size_t) block];
                soundCount += hasSound[(size_t) block];

                if (isVoice[(size_t) block] != 0)
                    noteFrequencies.push_back (blocks[(size_t) block].frequency);
            }

            const auto required = minimumFrameCoverage * (float) (lastBlock - firstBlock + 1);
            const auto isVoiceFrame = (float) voiceCount >= required;
            const auto isSoundFrame = (float) soundCount >= required;

            if (! isVoiceFrame && ! isSoundFrame)
                continue;

            std::fill (fftData.begin(), fftData.end(), 0.0f);
            std::copy (samples + start, samples + start + frameSize, fftData.begin());
            window.multiplyWithWindowingTable (fftData.data(), (size_t) frameSize);
            fft.performFrequencyOnlyForwardTransform (fftData.data(), true);

            for (int bin = 0; bin < numBins; ++bin)
                framePower[(size_t) bin] = fftData[(size_t) bin] * fftData[(size_t) bin] * normalisation;

            if (isSoundFrame)
            {
                juce::FloatVectorOperations::add (soundPower.data(), framePower.data(), numBins);
                ++numSoundFrames;
            }

            if (isVoiceFrame)
            {
                juce::FloatVectorOperations::add (voicePower.data(), framePower.data(), numBins);
                ++numVoiceFrames;

                const auto noteFrequency = getPercentile (noteFrequencies, 0.5f);

                if (noteFrequency > 0.0f)
                {
                    const auto note = juce::roundToInt (12.0f * std::log2 (noteFrequency / 440.0f));
                    allNotes.insert (note);
                    addFrameObservations (observations, framePower.data(), numBins, binWidth, noteFrequency, note);
                    addFrameObservations (voiceBlocksBefore[(size_t) firstBlock] < halfVoiceBlocks ? firstHalf : secondHalf,
                                          framePower.data(), numBins, binWidth, noteFrequency, note);
                }
            }
        }

        const auto fillCurve = [&] (std::vector<float>& power, int numFrames, std::vector<float>& curve)
        {
            if (numFrames == 0)
                return;

            juce::FloatVectorOperations::multiply (power.data(), 1.0f / (float) numFrames, numBins);
            curve.resize (spectrum::numDisplayPoints);
            spectrum::fillDisplayCurve (power.data(), numBins, binWidth, curve);
        };

        fillCurve (voicePower, numVoiceFrames, result.voiceCurve);
        fillCurve (soundPower, numSoundFrames, result.soundCurve);
        result.noiseCurve = measureNoiseCurve (samples, numSamples, frameSize, hopSize, blockSize, isVoice, fft, window, normalisation, binWidth);

        result.numDistinctNotes = (int) allNotes.size();

        result.resonances = findResonances (observations, result.fundamentalMedian);

        const auto firstHalfResonances = findResonances (firstHalf, result.fundamentalMedian);
        const auto secondHalfResonances = findResonances (secondHalf, result.fundamentalMedian);

        for (auto& resonance : result.resonances)
            resonance.isInBothHalves = hasResonanceNear (firstHalfResonances, resonance.frequency)
                                    && hasResonanceNear (secondHalfResonances, resonance.frequency);

        if (result.resonances.size() > maximumCandidates)
            result.resonances.resize (maximumCandidates);

        return result;
    }
}
