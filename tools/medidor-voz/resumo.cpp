// Pontos de saturação, estabilidade do espectro por faixa e contagem de picos encontrados (D84).
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "core/analysis/SpectrumGrid.h"
#include "core/analysis/VoiceAnalyzer.h"

using namespace lapidamix;

static constexpr double kRate = 44100.0;
struct Recording { std::string name; std::vector<float> mono; };

static Recording load (const std::string& name, const juce::String& path)
{
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatReader> reader (format.createReaderFor (new juce::FileInputStream (juce::File (path)), true));

    if (reader == nullptr || reader->sampleRate != kRate) { printf ("Nao leu %s (precisa ser WAV de 44,1 kHz)\n", name.c_str()); std::exit (1); }

    juce::AudioBuffer<float> buffer ((int) reader->numChannels, (int) reader->lengthInSamples);
    reader->read (&buffer, 0, buffer.getNumSamples(), 0, true, true);

    Recording r { name, std::vector<float> ((size_t) buffer.getNumSamples(), 0.0f) };
    for (int c = 0; c < buffer.getNumChannels(); ++c)
        juce::FloatVectorOperations::addWithMultiply (r.mono.data(), buffer.getReadPointer (c), 1.0f / (float) buffer.getNumChannels(), buffer.getNumSamples());

    int over = 0, runs = 0; bool inside = false; auto peak = 0.0f; double firstAt = -1;
    for (size_t i = 0; i < r.mono.size(); ++i)
    {
        const auto v = std::abs (r.mono[i]);
        peak = juce::jmax (peak, v);
        const auto high = v >= 0.999f;
        if (high) { ++over; if (! inside) { ++runs; if (firstAt < 0) firstAt = (double) i / kRate; } }
        inside = high;
    }
    printf ("%s: pico %.4f; amostras >= 0,999: %d, em %d trechos; primeiro em %.2f s\n", name.c_str(), peak, over, runs, firstAt);
    return r;
}

struct Run { AnalysisResult result; int start = 0, length = 0; bool reached = false; };

// Mesmo fluxo do AnalysisController: entrega de 50 em 50 ms até atingir a meta de voz válida.
static Run capture (const std::vector<float>& x, int start, float target)
{
    VoiceAnalyzer::Settings s; s.sampleRate = kRate;
    VoiceAnalyzer analyzer (s);
    const auto limit = juce::jmin ((int) x.size() - start, (int) (120 * kRate));
    Run run; run.start = start;
    int n = 0;
    while (n < limit)
    {
        n = juce::jmin (limit, n + 2205);
        analyzer.update (x.data() + start, n);
        if (analyzer.getEstimatedVoiceSeconds() >= target) { run.reached = true; break; }
    }
    run.length = n;
    run.result = analyzer.finish (x.data() + start, n, nullptr);
    return run;
}

static std::vector<Run> windows (const std::vector<float>& x, float target)
{
    std::vector<Run> out; int start = 0;
    for (;;) { auto r = capture (x, start, target); if (! r.reached) break; start += r.length; out.push_back (std::move (r)); }
    return out;
}

static float bandRms (const std::vector<float>& a, const std::vector<float>& b, float low, float high)
{
    // Tira o volume geral pela faixa de 100 Hz a 10 kHz, e mede a forma só dentro da faixa pedida.
    auto level = 0.0f; int n = 0;
    for (int p = 0; p < spectrum::numDisplayPoints; ++p) { const auto f = spectrum::getDisplayFrequency (p); if (f >= 100 && f <= 10000) { level += a[(size_t) p] - b[(size_t) p]; ++n; } }
    level /= (float) n;
    auto s = 0.0f; n = 0;
    for (int p = 0; p < spectrum::numDisplayPoints; ++p) { const auto f = spectrum::getDisplayFrequency (p); if (f >= low && f < high) { const auto d = a[(size_t) p] - b[(size_t) p] - level; s += d * d; ++n; } }
    return std::sqrt (s / (float) n);
}

static std::vector<float> withPeak (const std::vector<float>& x, float f, float q, float gainDb)
{
    auto coefficients = new juce::dsp::IIR::Coefficients<float> (juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter (kRate, f, q, juce::Decibels::decibelsToGain (gainDb)));
    juce::dsp::IIR::Filter<float> filter;
    filter.coefficients = coefficients;
    filter.prepare ({ kRate, 4096, 1 });
    auto out = x;
    for (auto& v : out) v = filter.processSample (v);
    return out;
}

static bool hasNear (const std::vector<ResonanceCandidate>& list, float f)
{
    for (const auto& c : list) if (std::abs (std::log2 (c.frequency / f)) <= 1.0f / 12.0f) return true;
    return false;
}

int main (int argc, char** argv)
{
    const juce::String folder = juce::String (LAPIDAMIX_ROOT) + "/test-audio/voz-usuario/";
    const juce::String pathA = argc > 2 ? juce::String (argv[1]) : folder + "VOZ-CANTADA-SHURE-SM7DB.wav";
    const juce::String pathB = argc > 2 ? juce::String (argv[2]) : folder + "VOZ-CANTADA-SHURE-SM57.wav";
    const std::vector<Recording> recordings { load ("A", pathA), load ("B", pathB) };

    struct Band { const char* name; float low, high; };
    const Band bands[] = { { "100-500", 100, 500 }, { "500-2k", 500, 2000 }, { "2k-10k", 2000, 10000 } };

    printf ("\n=== ESPECTRO ENTRE JANELAS SEPARADAS, POR FAIXA (dB rms, mediana dos pares) ===\n");
    for (const auto& rec : recordings)
        for (float target : { 10.f, 20.f, 30.f, 45.f })
        {
            const auto w = windows (rec.mono, target);
            printf ("%s %2.0f s:", rec.name.c_str(), target);
            for (const auto& band : bands)
            {
                std::vector<float> d;
                for (size_t i = 0; i < w.size(); ++i) for (size_t j = i + 1; j < w.size(); ++j) d.push_back (bandRms (w[i].result.voiceCurve, w[j].result.voiceCurve, band.low, band.high));
                if (d.empty()) continue;
                std::sort (d.begin(), d.end());
                printf ("  %s %.2f", band.name, d[d.size() / 2]);
            }
            printf ("\n");
        }

    const auto ra = capture (recordings[0].mono, 0, 30.f), rb = capture (recordings[1].mono, 0, 30.f);
    printf ("Entre as gravacoes, primeiros 30 s:");
    for (const auto& band : bands) printf ("  %s %.2f", band.name, bandRms (ra.result.voiceCurve, rb.result.voiceCurve, band.low, band.high));
    printf ("\n");

    const float frequencies[] = { 300, 500, 1000, 2200, 3000, 4000, 6000 };
    struct Case { float gain, q; };
    const Case cases[] = { { 9, 8 }, { 6, 8 } };

    printf ("\n=== RESSONANCIAS, METODO ATUAL ===\n");

    for (const auto& rec : recordings)
    {
        const auto base = windows (rec.mono, 30.0f);
        int total = 0, repeated = 0;

        for (size_t i = 0; i < base.size(); ++i)
        {
            printf ("%s janela %d:", rec.name.c_str(), (int) i);
            for (const auto& c : base[i].result.resonances)
            {
                bool again = false;
                for (size_t j = 0; j < base.size(); ++j) if (j != i && hasNear (base[j].result.resonances, c.frequency)) again = true;
                ++total; if (again) ++repeated;
                printf (" %.0f(%.1f,%dn)%s", c.frequency, c.prominenceDb, c.numNotes, again ? "*" : "");
            }
            printf ("\n");
        }
        printf ("  candidatas %d; repetem em outra janela %d (*)\n", total, repeated);

        for (const auto& c : cases)
        {
            printf ("  +%.0f dB Q%.0f, achou (sem contar o que ja havia):", c.gain, c.q);
            for (auto f : frequencies)
            {
                const auto y = withPeak (rec.mono, f, c.q, c.gain);
                int hits = 0, already = 0;
                for (const auto& b : base)
                {
                    if (hasNear (b.result.resonances, f)) { ++already; continue; }
                    if (hasNear (capture (y, b.start, 30.0f).result.resonances, f)) ++hits;
                }
                printf (" %.0f=%d/%d", f, hits, (int) base.size() - already);
            }
            printf ("\n");
        }
    }

    fflush (stdout);
    return 0;
}
