// Testa de onde vem um pico que se repete entre cantores: voz, ou gravação (D84).
// Uso:
//   origem vogais <pasta data_by_singer> <freq. minima> <freq. maxima>
//   origem fora-da-voz <pasta data_by_singer> <freq. minima> <freq. maxima> [gravacoes wav de controle]
//
// Se o pico vem do trato vocal, ele muda com a vogal e com a pessoa.
// Se vem do equipamento ou da sala, fica no mesmo lugar, e pode aparecer no que não é voz.
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "core/analysis/SpectrumGrid.h"
#include "core/analysis/VoiceAnalyzer.h"

using namespace lapidamix;

static constexpr double kRate = 44100.0;

static float percentile (std::vector<float> v, float f)
{
    if (v.empty()) return 0.0f;
    const auto i = (size_t) juce::jlimit (0, (int) v.size() - 1, juce::roundToInt (f * (float) (v.size() - 1)));
    std::nth_element (v.begin(), v.begin() + (std::ptrdiff_t) i, v.end());
    return v[i];
}

static void appendWav (const juce::File& file, std::vector<float>& out)
{
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatReader> reader (format.createReaderFor (new juce::FileInputStream (file), true));
    if (reader == nullptr || reader->sampleRate != kRate) return;

    juce::AudioBuffer<float> buffer ((int) reader->numChannels, (int) reader->lengthInSamples);
    reader->read (&buffer, 0, buffer.getNumSamples(), 0, true, true);
    const auto start = out.size();
    out.resize (start + (size_t) buffer.getNumSamples(), 0.0f);

    for (int c = 0; c < buffer.getNumChannels(); ++c)
        juce::FloatVectorOperations::addWithMultiply (out.data() + start, buffer.getReadPointer (c), 1.0f / (float) buffer.getNumChannels(), buffer.getNumSamples());
}

static AnalysisResult analyse (const std::vector<float>& x)
{
    VoiceAnalyzer::Settings s; s.sampleRate = kRate;
    VoiceAnalyzer analyzer (s);
    return analyzer.finish (x.data(), juce::jmin ((int) x.size(), (int) (900 * kRate)), nullptr);
}

// Pico de uma curva em dB dentro da faixa, medido contra a mediana da faixa mais larga em volta.
struct Peak { float frequency = 0, height = 0; };

static Peak findPeak (const std::vector<float>& curve, float low, float high)
{
    std::vector<float> around;
    Peak peak; auto best = -1000.0f;

    for (int p = 0; p < spectrum::numDisplayPoints; ++p)
    {
        const auto f = spectrum::getDisplayFrequency (p);
        if (f >= low / 1.4f && f <= high * 1.4f) around.push_back (curve[(size_t) p]);
        if (f >= low && f <= high && curve[(size_t) p] > best) { best = curve[(size_t) p]; peak.frequency = f; }
    }

    peak.height = best - percentile (around, 0.5f);
    return peak;
}

static void byVowel (const juce::File& root, float low, float high)
{
    auto folders = root.findChildFiles (juce::File::findDirectories, false);
    folders.sort();
    const char* vowels[] = { "a", "e", "i", "o", "u" };

    printf ("cantor; vogais: pico em Hz (altura em dB); espalhamento em oitavas entre as vogais; candidatas do analisador na faixa\n");
    std::map<std::string, std::vector<float>> byGroup;
    std::vector<float> spreads;

    for (const auto& folder : folders)
    {
        printf ("%s;", folder.getFileName().toRawUTF8());
        std::vector<float> peaks;
        int withCandidate = 0;

        for (const auto* vowel : vowels)
        {
            std::vector<float> x;

            for (const auto* group : { "arpeggios", "scales" })
            {
                auto files = folder.getChildFile (group).getChildFile ("straight").findChildFiles (juce::File::findFiles, false, juce::String ("*_") + vowel + ".wav");
                files.sort();
                for (const auto& file : files) appendWav (file, x);
            }

            auto files = folder.getChildFile ("long_tones").getChildFile ("straight").findChildFiles (juce::File::findFiles, false, juce::String ("*_") + vowel + ".wav");
            for (const auto& file : files) appendWav (file, x);

            if (x.empty()) { printf (" %s=-", vowel); continue; }

            const auto r = analyse (x);
            if (r.voiceCurve.empty()) { printf (" %s=-", vowel); continue; }

            const auto peak = findPeak (r.voiceCurve, low, high);
            peaks.push_back (peak.frequency);
            printf (" %s=%.0f(%.0f)", vowel, peak.frequency, peak.height);

            for (const auto& c : r.resonances)
                if (c.frequency >= low && c.frequency <= high) { ++withCandidate; break; }
        }

        std::sort (peaks.begin(), peaks.end());
        const auto spread = peaks.size() > 1 ? std::log2 (peaks.back() / peaks.front()) : 0.0f;
        spreads.push_back (spread);
        byGroup[folder.getFileName().startsWith ("female") ? "mulheres" : "homens"].push_back (percentile (peaks, 0.5f));
        printf ("; %.2f; %d de 5\n", spread, withCandidate);
        fflush (stdout);
    }

    printf ("espalhamento mediano entre vogais: %.2f oitava\n", percentile (spreads, 0.5f));
    for (const auto& [name, values] : byGroup)
        printf ("%s: pico mediano %.0f Hz (de %.0f a %.0f)\n", name.c_str(), percentile (values, 0.5f), *std::min_element (values.begin(), values.end()), *std::max_element (values.begin(), values.end()));
}

// Espectro médio só dos pedaços silenciosos e só das respirações/ruídos, sem a voz cantada.
struct Pieces { std::vector<float> silence, voiced; int numSilent = 0, numVoiced = 0; float silenceDb = 0; };

static Pieces measurePieces (const std::vector<float>& x)
{
    constexpr int order = 13;
    constexpr int frameSize = 1 << order;
    juce::dsp::FFT fft (order);
    juce::dsp::WindowingFunction<float> window ((size_t) frameSize, juce::dsp::WindowingFunction<float>::hann, false);
    const auto numBins = frameSize / 2 + 1;
    const auto binWidth = kRate / frameSize;

    std::vector<float> levels;
    for (int s = 0; s + frameSize <= (int) x.size(); s += frameSize / 2)
    {
        auto e = 0.0f;
        for (int i = 0; i < frameSize; ++i) e += x[(size_t) (s + i)] * x[(size_t) (s + i)];
        levels.push_back (e / frameSize);
    }

    // "Silêncio": os 5% de quadros mais baixos, que não sejam zeros digitais. "Voz": os 30% mais altos.
    std::vector<float> nonZero;
    for (auto v : levels) if (v > 1.0e-12f) nonZero.push_back (v);
    const auto quiet = percentile (nonZero, 0.05f);
    const auto loud = percentile (nonZero, 0.7f);

    std::vector<float> data ((size_t) frameSize * 2), silence ((size_t) numBins, 0.0f), voiced ((size_t) numBins, 0.0f);
    Pieces out;
    size_t index = 0;

    for (int s = 0; s + frameSize <= (int) x.size(); s += frameSize / 2, ++index)
    {
        const auto level = levels[index];
        const auto isQuiet = level > 1.0e-12f && level <= quiet;
        const auto isLoud = level >= loud;
        if (! isQuiet && ! isLoud) continue;

        std::fill (data.begin(), data.end(), 0.0f);
        std::copy (x.begin() + s, x.begin() + s + frameSize, data.begin());
        window.multiplyWithWindowingTable (data.data(), (size_t) frameSize);
        fft.performFrequencyOnlyForwardTransform (data.data(), true);

        auto& target = isQuiet ? silence : voiced;
        for (int b = 0; b < numBins; ++b) target[(size_t) b] += data[(size_t) b] * data[(size_t) b] * 16.0f / ((float) frameSize * (float) frameSize);
        if (isQuiet) ++out.numSilent; else ++out.numVoiced;
    }

    if (out.numSilent > 0) for (auto& v : silence) v /= (float) out.numSilent;
    if (out.numVoiced > 0) for (auto& v : voiced) v /= (float) out.numVoiced;

    out.silence.resize (spectrum::numDisplayPoints);
    out.voiced.resize (spectrum::numDisplayPoints);
    spectrum::fillDisplayCurve (silence.data(), numBins, binWidth, out.silence);
    spectrum::fillDisplayCurve (voiced.data(), numBins, binWidth, out.voiced);
    out.silenceDb = spectrum::powerToDb (quiet);
    return out;
}

static void outsideVoice (const juce::File& root, float low, float high, int argc, char** argv)
{
    auto folders = root.findChildFiles (juce::File::findDirectories, false);
    folders.sort();

    printf ("gravacao; silencio dB; pico no silencio Hz (altura); pico no inspirado Hz (altura); pico na fala Hz (altura); pico na voz alta Hz (altura)\n");

    const auto printRow = [&] (const std::string& name, const std::vector<float>& sung, const std::vector<float>& inhaled, const std::vector<float>& spoken)
    {
        const auto pieces = measurePieces (sung);
        const auto s = findPeak (pieces.silence, low, high);
        const auto v = findPeak (pieces.voiced, low, high);
        printf ("%s; %.0f; %.0f (%.1f); ", name.c_str(), pieces.silenceDb, s.frequency, s.height);

        if (inhaled.empty()) printf ("-; ");
        else { const auto p = measurePieces (inhaled); const auto i = findPeak (p.voiced, low, high); printf ("%.0f (%.1f); ", i.frequency, i.height); }

        if (spoken.empty()) printf ("-; ");
        else { const auto p = measurePieces (spoken); const auto i = findPeak (p.voiced, low, high); printf ("%.0f (%.1f); ", i.frequency, i.height); }

        printf ("%.0f (%.1f)\n", v.frequency, v.height);
        fflush (stdout);
    };

    for (const auto& folder : folders)
    {
        std::vector<float> sung, inhaled, spoken;

        for (const auto* style : { "straight", "vibrato" })
        {
            auto files = folder.getChildFile ("excerpts").getChildFile (style).findChildFiles (juce::File::findFiles, false, "*.wav");
            files.sort();
            for (const auto& file : files) appendWav (file, sung);
        }

        for (const auto& file : folder.getChildFile ("long_tones").getChildFile ("inhaled").findChildFiles (juce::File::findFiles, false, "*.wav")) appendWav (file, inhaled);
        for (const auto& file : folder.getChildFile ("excerpts").getChildFile ("spoken").findChildFiles (juce::File::findFiles, false, "*.wav")) appendWav (file, spoken);

        printRow (folder.getFileName().toStdString(), sung, inhaled, spoken);
    }

    for (int i = 5; i < argc; ++i)
    {
        std::vector<float> control;
        const juce::File file (juce::String::fromUTF8 (argv[i]));
        appendWav (file, control);
        printRow ("controle " + file.getFileNameWithoutExtension().toStdString(), control, {}, {});
    }
}

int main (int argc, char** argv)
{
    const juce::String mode = argc > 1 ? argv[1] : "";

    if (mode == "vogais" && argc == 5)
        byVowel (juce::File (juce::String::fromUTF8 (argv[2])), (float) std::atof (argv[3]), (float) std::atof (argv[4]));
    else if (mode == "fora-da-voz" && argc >= 5)
        outsideVoice (juce::File (juce::String::fromUTF8 (argv[2])), (float) std::atof (argv[3]), (float) std::atof (argv[4]), argc, argv);
    else
        printf ("Uso: origem vogais <pasta> <min> <max> | origem fora-da-voz <pasta> <min> <max> [wav de controle]\n");

    return 0;
}
