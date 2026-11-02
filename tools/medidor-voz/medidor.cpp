// Estabilidade da análise, comparação entre gravações e picos inseridos (D84).
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "core/analysis/SpectrumGrid.h"
#include "core/analysis/VoiceAnalyzer.h"
#include "core/analysis/YinPitchDetector.h"

using namespace lapidamix;

static constexpr double kRate = 44100.0;
static const float kInfinite = 1.0e9f;

struct Recording
{
    std::string name;
    std::vector<float> mono;
};

static float percentile (std::vector<float> v, float f)
{
    if (v.empty()) return 0.0f;
    const auto i = (size_t) juce::jlimit (0, (int) v.size() - 1, juce::roundToInt (f * (float) (v.size() - 1)));
    std::nth_element (v.begin(), v.begin() + (std::ptrdiff_t) i, v.end());
    return v[i];
}

static Recording load (const std::string& name, const juce::String& path)
{
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatReader> reader (format.createReaderFor (new juce::FileInputStream (juce::File (path)), true));

    if (reader == nullptr) { printf ("Nao leu %s\n", name.c_str()); std::exit (1); }

    juce::AudioBuffer<float> buffer ((int) reader->numChannels, (int) reader->lengthInSamples);
    reader->read (&buffer, 0, buffer.getNumSamples(), 0, true, true);

    Recording r;
    r.name = name;
    r.mono.assign ((size_t) buffer.getNumSamples(), 0.0f);
    const auto gain = 1.0f / (float) buffer.getNumChannels();

    for (int c = 0; c < buffer.getNumChannels(); ++c)
        juce::FloatVectorOperations::addWithMultiply (r.mono.data(), buffer.getReadPointer (c), gain, buffer.getNumSamples());

    double ll = 0, rr = 0, lr = 0;
    const auto* a = buffer.getReadPointer (0);
    const auto* b = buffer.getReadPointer (buffer.getNumChannels() - 1);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        ll += a[i] * a[i]; rr += b[i] * b[i]; lr += a[i] * b[i];
    }

    const auto n = (double) buffer.getNumSamples();
    printf ("%s: taxa %.0f Hz, %d canais, %.1f s, RMS E %.1f dB, RMS D %.1f dB, pico %.1f dB, correlacao E/D %.3f\n",
            name.c_str(), reader->sampleRate, buffer.getNumChannels(), n / reader->sampleRate,
            10 * std::log10 (ll / n + 1e-20), 10 * std::log10 (rr / n + 1e-20),
            juce::Decibels::gainToDecibels (buffer.getMagnitude (0, buffer.getNumSamples())),
            lr / std::sqrt (ll * rr + 1e-30));
    return r;
}

struct Run
{
    AnalysisResult result;
    bool reached = false;
    int start = 0;
    int length = 0;
};

// Mesmo fluxo do AnalysisController: entrega de 50 em 50 ms ate atingir a meta de voz valida.
static Run runCapture (const std::vector<float>& x, int start, float target, double capSeconds)
{
    VoiceAnalyzer::Settings settings;
    settings.sampleRate = kRate;
    VoiceAnalyzer analyzer (settings);

    const auto limit = juce::jmin ((int) x.size() - start, (int) (capSeconds * kRate));
    const auto* data = x.data() + start;
    Run run;
    run.start = start;
    int n = 0;

    while (n < limit)
    {
        n = juce::jmin (limit, n + 2205);
        analyzer.update (data, n);

        if (analyzer.getEstimatedVoiceSeconds() >= target) { run.reached = true; break; }
    }

    run.length = n;
    run.result = analyzer.finish (data, n, nullptr);
    return run;
}

// Diferenca de forma entre curvas, sem a diferenca de volume, de 100 Hz a 10 kHz.
struct CurveDistance { float rms = 0, maximum = 0, level = 0; };

static CurveDistance distance (const std::vector<float>& a, const std::vector<float>& b)
{
    std::vector<float> d;

    for (int p = 0; p < spectrum::numDisplayPoints; ++p)
    {
        const auto f = spectrum::getDisplayFrequency (p);
        if (f >= 100.0f && f <= 10000.0f) d.push_back (a[(size_t) p] - b[(size_t) p]);
    }

    CurveDistance out;
    for (auto v : d) out.level += v;
    out.level /= (float) d.size();
    auto s = 0.0f;
    for (auto v : d) { s += (v - out.level) * (v - out.level); out.maximum = juce::jmax (out.maximum, std::abs (v - out.level)); }
    out.rms = std::sqrt (s / (float) d.size());
    return out;
}

static float pct (float a, float b) { return b > 0 ? 100.0f * std::abs (a - b) / b : 0.0f; }

static std::string candidates (const std::vector<ResonanceCandidate>& list)
{
    std::string s;
    char t[96];
    for (const auto& c : list) { snprintf (t, sizeof t, "%.0fHz(%.1fdB,%.0f%%,%dn) ", c.frequency, c.prominenceDb, c.presence * 100, c.numNotes); s += t; }
    return s.empty() ? "nenhuma" : s;
}

static void printWhole (const char* tag, const Run& r)
{
    const auto& a = r.result;
    printf ("%s: captura %.1f s, som %.1f s, voz %.1f s, f0 mediana %.1f, p5 %.1f, p95 %.1f, confianca %.2f, notas %d\n  candidatas: %s\n",
            tag, a.capturedSeconds, a.soundSeconds, a.voiceSeconds, a.fundamentalMedian, a.fundamentalLow, a.fundamentalHigh,
            a.confidence, a.numDistinctNotes, candidates (a.resonances).c_str());
}

static std::vector<Run> disjointWindows (const std::vector<float>& x, float target)
{
    std::vector<Run> runs;
    int start = 0;

    for (;;)
    {
        auto run = runCapture (x, start, target, 120.0);
        if (! run.reached) break;
        start += run.length;
        runs.push_back (std::move (run));
    }

    return runs;
}

static void stability (const Recording& rec, const Run& whole)
{
    printf ("\n=== ESTABILIDADE %s ===\n", rec.name.c_str());

    for (float target : { 10.0f, 20.0f, 30.0f, 45.0f })
    {
        const auto windows = disjointWindows (rec.mono, target);
        printf ("\n-- meta %.0f s de voz: %d janelas separadas\n", target, (int) windows.size());

        std::vector<float> wMed, wLow, wHigh, wCurve, wMax, pMed, pLow, pHigh, pCurve, pMax;

        for (size_t i = 0; i < windows.size(); ++i)
        {
            const auto& a = windows[i].result;
            const auto dw = distance (a.voiceCurve, whole.result.voiceCurve);
            wMed.push_back (pct (a.fundamentalMedian, whole.result.fundamentalMedian));
            wLow.push_back (pct (a.fundamentalLow, whole.result.fundamentalLow));
            wHigh.push_back (pct (a.fundamentalHigh, whole.result.fundamentalHigh));
            wCurve.push_back (dw.rms);
            wMax.push_back (dw.maximum);

            printf ("  janela %d (inicio %.1f s, captura %.1f s, voz %.1f): f0 %.1f p5 %.1f p95 %.1f | espectro x total: %.2f dB rms, %.1f max | %s\n",
                    (int) i, windows[i].start / kRate, a.capturedSeconds, a.voiceSeconds, a.fundamentalMedian, a.fundamentalLow, a.fundamentalHigh,
                    dw.rms, dw.maximum, candidates (a.resonances).c_str());

            for (size_t j = i + 1; j < windows.size(); ++j)
            {
                const auto& b = windows[j].result;
                const auto dp = distance (a.voiceCurve, b.voiceCurve);
                pMed.push_back (pct (a.fundamentalMedian, b.fundamentalMedian));
                pLow.push_back (pct (a.fundamentalLow, b.fundamentalLow));
                pHigh.push_back (pct (a.fundamentalHigh, b.fundamentalHigh));
                pCurve.push_back (dp.rms);
                pMax.push_back (dp.maximum);
            }
        }

        const auto show = [] (const char* label, const std::vector<float>& v, const char* unit)
        {
            if (! v.empty())
                printf ("  %-28s mediana %.2f %s, pior %.2f %s\n", label, percentile (v, 0.5f), unit, *std::max_element (v.begin(), v.end()), unit);
        };

        show ("x total, f0 mediana", wMed, "%");
        show ("x total, p5", wLow, "%");
        show ("x total, p95", wHigh, "%");
        show ("x total, espectro rms", wCurve, "dB");
        show ("x total, espectro max", wMax, "dB");
        show ("entre janelas, f0 mediana", pMed, "%");
        show ("entre janelas, p5", pLow, "%");
        show ("entre janelas, p95", pHigh, "%");
        show ("entre janelas, espectro rms", pCurve, "dB");
        show ("entre janelas, espectro max", pMax, "dB");
    }
}

// ------------------------ Trilha de notas, com a mesma classificacao do analisador ------------------------

struct Track
{
    std::vector<float> power, f0, aperiodicity, other;
    std::vector<char> voiced, sound;
};

static std::vector<float> decimate (const std::vector<float>& x, int start, int length, int factor)
{
    const auto decimatedRate = kRate / factor;
    auto coefficients = new juce::dsp::IIR::Coefficients<float> (juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass (kRate, (float) (decimatedRate * 0.4)));
    juce::dsp::IIR::Filter<float> a, b;
    a.coefficients = coefficients;
    b.coefficients = coefficients;
    a.prepare ({ kRate, 4096, 1 });
    b.prepare ({ kRate, 4096, 1 });
    std::vector<float> out;

    for (int i = 0; i < length; ++i)
    {
        const auto v = b.processSample (a.processSample (x[(size_t) (start + i)]));
        if (i % factor == 0) out.push_back (v);
    }

    return out;
}

// Estimador independente do YIN: correlacao normalizada, escolhendo o menor periodo forte.
static float nccf (const float* frame, int window, int minLag, int maxLag, double rate)
{
    std::vector<float> c ((size_t) maxLag + 2, 0.0f);
    double e0 = 1e-12;
    for (int i = 0; i < window; ++i) e0 += frame[i] * frame[i];

    auto best = 0.0f;

    for (int lag = minLag; lag <= maxLag + 1; ++lag)
    {
        double xy = 0, e1 = 1e-12;
        for (int i = 0; i < window; ++i) { xy += frame[i] * frame[i + lag]; e1 += frame[i + lag] * frame[i + lag]; }
        c[(size_t) lag] = (float) (xy / std::sqrt (e0 * e1));
        if (lag <= maxLag) best = juce::jmax (best, c[(size_t) lag]);
    }

    if (best < 0.5f) return 0.0f;

    for (int lag = minLag + 1; lag <= maxLag; ++lag)
    {
        if (c[(size_t) lag] >= 0.9f * best && c[(size_t) lag] >= c[(size_t) lag - 1] && c[(size_t) lag] >= c[(size_t) lag + 1])
        {
            const auto denominator = c[(size_t) lag - 1] - 2 * c[(size_t) lag] + c[(size_t) lag + 1];
            const auto shift = std::abs (denominator) > 1e-9f ? 0.5f * (c[(size_t) lag - 1] - c[(size_t) lag + 1]) / denominator : 0.0f;
            return (float) (rate / (lag + shift));
        }
    }

    return 0.0f;
}

static void medianOfThree (std::vector<char>& f)
{
    const auto o = f;
    for (size_t i = 1; i + 1 < f.size(); ++i) f[i] = (o[i - 1] + o[i] + o[i + 1]) >= 2 ? 1 : 0;
}

static Track makeTrack (const std::vector<float>& x, int start, int length, bool withOther)
{
    const auto blockSize = juce::roundToInt (kRate * 0.025);
    const auto factor = (int) (kRate / 11025.0);
    const auto decimatedRate = kRate / factor;
    const auto low = decimate (x, start, length, factor);

    YinPitchDetector yin;
    yin.prepare (decimatedRate, 60.0f, 1100.0f);
    const auto frameSize = yin.getFrameSize();
    const auto minLag = (int) (decimatedRate / 1100.0);
    const auto maxLag = (int) (decimatedRate / 60.0);
    const auto window = frameSize - maxLag - 2;

    Track t;

    for (int b = 0;; ++b)
    {
        const auto centre = (b + 0.5) * blockSize / factor;
        const auto frameStart = juce::jmax (0, (int) (centre - frameSize * 0.5));
        if ((b + 1) * blockSize > length || frameStart + frameSize > (int) low.size()) break;

        auto sum = 0.0f;
        for (int i = b * blockSize; i < (b + 1) * blockSize; ++i) sum += x[(size_t) (start + i)] * x[(size_t) (start + i)];
        t.power.push_back (sum / (float) blockSize);

        const auto p = yin.detect (low.data() + frameStart);
        t.f0.push_back (p.frequency);
        t.aperiodicity.push_back (p.aperiodicity);
        t.other.push_back (withOther ? nccf (low.data() + frameStart, window, minLag, maxLag, decimatedRate) : 0.0f);
    }

    const auto absolute = std::pow (10.0f, -6.0f);
    double s = 0; int c = 0;
    for (auto v : t.power) if (v > absolute) { s += v; ++c; }
    const auto relative = c > 0 ? (float) (s / c) * 0.01f : 0.0f;

    t.voiced.assign (t.power.size(), 0);
    t.sound.assign (t.power.size(), 0);

    for (size_t i = 0; i < t.power.size(); ++i)
    {
        if (t.power[i] > absolute && t.power[i] > relative)
        {
            t.sound[i] = 1;
            if (t.aperiodicity[i] < 0.3f && t.f0[i] >= 60.0f && t.f0[i] <= 1100.0f) t.voiced[i] = 1;
        }
    }

    medianOfThree (t.sound);
    medianOfThree (t.voiced);
    return t;
}

static void describeTrack (const Recording& rec, const Track& t)
{
    std::vector<float> all;
    for (auto v : t.power) all.push_back (v);
    const auto floorDb = spectrum::powerToDb (percentile (all, 0.05f));
    const auto loudDb = spectrum::powerToDb (percentile (all, 0.95f));

    int voiced = 0, compared = 0, agree = 0, octave = 0, other = 0;
    std::map<int, int> histogram;

    for (size_t i = 0; i < t.power.size(); ++i)
    {
        if (! t.voiced[i]) continue;
        ++voiced;
        ++histogram[juce::roundToInt (12 * std::log2 (t.f0[i] / 440.0f)) + 69];

        if (t.other[i] > 0)
        {
            ++compared;
            const auto ratio = t.f0[i] / t.other[i];
            if (std::abs (ratio - 1.0f) <= 0.03f) ++agree;
            else if (std::abs (ratio - 2.0f) <= 0.06f || std::abs (ratio - 0.5f) <= 0.015f) ++octave;
            else ++other;
        }
    }

    printf ("\n%s: blocos 5%% mais baixos %.1f dB, 95%% %.1f dB (distancia %.1f dB); YIN x estimador independente em %d blocos: %.1f%% dentro de 3%%, %.1f%% erro de oitava, %.1f%% outros\n",
            rec.name.c_str(), floorDb, loudDb, loudDb - floorDb, compared, compared ? 100.0f * agree / compared : 0.0f,
            compared ? 100.0f * octave / compared : 0.0f, compared ? 100.0f * other / compared : 0.0f);

    static const char* names[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
    printf ("  notas (%% dos blocos de voz):");
    for (const auto& [midi, count] : histogram)
        if (100.0f * count / voiced >= 0.5f)
            printf (" %s%d=%.1f", names[midi % 12], midi / 12 - 1, 100.0f * count / voiced);
    printf ("\n");
}

static void compareMics (const Recording& a, const Recording& b, const Track& ta, const Track& tb, const Run& wa, const Run& wb)
{
    printf ("\n=== COMPARACAO DE MICROFONES ===\n");

    // Mesma tomada? Correlacao do envelope em dB, procurando o melhor deslocamento.
    const auto env = [] (const Track& t) { std::vector<float> e; for (auto v : t.power) e.push_back (spectrum::powerToDb (v + 1e-11f)); return e; };
    const auto ea = env (ta), eb = env (tb);
    const auto best = [&]
    {
        auto bestCorr = -2.0; auto bestLag = 0;

        for (int lag = -1200; lag <= 1200; ++lag)
        {
            double xy = 0, xx = 0, yy = 0, mx = 0, my = 0; int n = 0;
            for (int i = 0; i < (int) ea.size(); ++i) { const auto j = i + lag; if (j >= 0 && j < (int) eb.size()) { mx += ea[(size_t) i]; my += eb[(size_t) j]; ++n; } }
            if (n < 400) continue;
            mx /= n; my /= n;
            for (int i = 0; i < (int) ea.size(); ++i) { const auto j = i + lag; if (j >= 0 && j < (int) eb.size()) { const auto u = ea[(size_t) i] - mx, v = eb[(size_t) j] - my; xy += u * v; xx += u * u; yy += v * v; } }
            const auto c = xy / std::sqrt (xx * yy + 1e-20);
            if (c > bestCorr) { bestCorr = c; bestLag = lag; }
        }

        return std::make_pair (bestCorr, bestLag);
    }();

    printf ("envelope: melhor correlacao %.3f com deslocamento %.3f s\n", best.first, best.second * 0.025);

    // Correlacao da forma de onda perto do melhor deslocamento: alta so se for a mesma tomada.
    {
        const auto centre = (int) (best.second * 0.025 * kRate);
        auto top = 0.0;
        const int first = (int) (20 * kRate), count = (int) (20 * kRate);

        for (int lag = centre - 2205; lag <= centre + 2205; lag += 1)
        {
            if (first + lag < 0 || first + lag + count >= (int) b.mono.size()) continue;
            double xy = 0, xx = 0, yy = 0;
            for (int i = first; i < first + count; i += 8) { const auto u = a.mono[(size_t) i], v = b.mono[(size_t) (i + lag)]; xy += u * v; xx += u * u; yy += v * v; }
            top = juce::jmax (top, std::abs (xy / std::sqrt (xx * yy + 1e-20)));
        }

        printf ("forma de onda (20 s a 40 s), maior correlacao: %.3f\n", top);
    }

    int both = 0, close = 0, octave = 0;
    std::vector<float> diffs;

    for (int i = 0; i < (int) ta.f0.size(); ++i)
    {
        const auto j = i + best.second;
        if (j < 0 || j >= (int) tb.f0.size() || ! ta.voiced[(size_t) i] || ! tb.voiced[(size_t) j]) continue;
        ++both;
        const auto ratio = ta.f0[(size_t) i] / tb.f0[(size_t) j];
        diffs.push_back (100.0f * std::abs (ratio - 1.0f));
        if (std::abs (ratio - 1.0f) <= 0.03f) ++close;
        if (std::abs (ratio - 2.0f) <= 0.06f || std::abs (ratio - 0.5f) <= 0.015f) ++octave;
    }

    printf ("f0 bloco a bloco, alinhado: %d blocos com voz nos dois; %.1f%% dentro de 3%%; %.1f%% diferenca de oitava; diferenca mediana %.2f%%\n",
            both, both ? 100.0f * close / both : 0.0f, both ? 100.0f * octave / both : 0.0f, percentile (diffs, 0.5f));

    const auto& ra = wa.result; const auto& rb = wb.result;
    printf ("gravacao inteira: mediana %.1f x %.1f Hz (%.2f%%), p5 %.1f x %.1f (%.2f%%), p95 %.1f x %.1f (%.2f%%)\n",
            ra.fundamentalMedian, rb.fundamentalMedian, pct (ra.fundamentalMedian, rb.fundamentalMedian),
            ra.fundamentalLow, rb.fundamentalLow, pct (ra.fundamentalLow, rb.fundamentalLow),
            ra.fundamentalHigh, rb.fundamentalHigh, pct (ra.fundamentalHigh, rb.fundamentalHigh));

    const auto d = distance (ra.voiceCurve, rb.voiceCurve);
    printf ("espectro inteiro: forma difere %.2f dB rms, %.1f dB no maximo; volume difere %.1f dB\n", d.rms, d.maximum, d.level);

    const auto r30a = runCapture (a.mono, 0, 30.0f, 120.0), r30b = runCapture (b.mono, 0, 30.0f, 120.0);
    const auto d30 = distance (r30a.result.voiceCurve, r30b.result.voiceCurve);
    printf ("fluxo real de 30 s: mediana %.1f x %.1f Hz (%.2f%%), p5 %.1f x %.1f, p95 %.1f x %.1f; espectro %.2f dB rms, %.1f max\n",
            r30a.result.fundamentalMedian, r30b.result.fundamentalMedian, pct (r30a.result.fundamentalMedian, r30b.result.fundamentalMedian),
            r30a.result.fundamentalLow, r30b.result.fundamentalLow, r30a.result.fundamentalHigh, r30b.result.fundamentalHigh, d30.rms, d30.maximum);

    printf ("\nHz; %s; %s; diferenca sem o volume (A-B)\n", a.name.c_str(), b.name.c_str());

    for (float f : { 63.f,80.f,100.f,125.f,160.f,200.f,250.f,315.f,400.f,500.f,630.f,800.f,1000.f,1250.f,1600.f,2000.f,2500.f,3150.f,4000.f,5000.f,6300.f,8000.f,10000.f,12500.f,16000.f })
    {
        auto point = 0; auto nearest = 1e9f;
        for (int p = 0; p < spectrum::numDisplayPoints; ++p)
        {
            const auto e = std::abs (spectrum::getDisplayFrequency (p) - f);
            if (e < nearest) { nearest = e; point = p; }
        }
        printf ("%6.0f  %7.1f  %7.1f  %+6.1f\n", f, ra.voiceCurve[(size_t) point], rb.voiceCurve[(size_t) point],
                ra.voiceCurve[(size_t) point] - rb.voiceCurve[(size_t) point] - d.level);
    }
}

// ------------------------ Reconstrucao do metodo da D73, so para comparar com o metodo atual ------------------------

static std::vector<ResonanceCandidate> d73Method (const std::vector<float>& x, int start, int length)
{
    const auto t = makeTrack (x, start, length, false);
    const auto blockSize = juce::roundToInt (kRate * 0.025);
    const auto order = juce::roundToInt (std::log2 (kRate * 0.186));
    const auto frameSize = 1 << order;
    const auto numBins = frameSize / 2 + 1;
    const auto binWidth = kRate / frameSize;
    const auto points = (int) std::floor (std::log2 (8000.0f / 150.0f) * 24) + 1;

    juce::dsp::FFT fft (order);
    juce::dsp::WindowingFunction<float> window ((size_t) frameSize, juce::dsp::WindowingFunction<float>::hann, false);
    std::vector<float> data ((size_t) frameSize * 2), power ((size_t) numBins), sum ((size_t) points, 0.0f);
    std::vector<int> above ((size_t) points, 0);
    std::vector<std::set<int>> notes ((size_t) points);
    int frames = 0;

    for (int s = 0; s + frameSize <= length; s += frameSize / 4)
    {
        const auto first = s / blockSize;
        const auto last = juce::jmin ((int) t.power.size() - 1, (s + frameSize - 1) / blockSize);
        if (last < first) break;
        int voiced = 0; std::vector<float> f0;
        for (int b = first; b <= last; ++b) if (t.voiced[(size_t) b]) { ++voiced; f0.push_back (t.f0[(size_t) b]); }
        if ((float) voiced < 0.8f * (float) (last - first + 1)) continue;

        std::fill (data.begin(), data.end(), 0.0f);
        std::copy (x.begin() + start + s, x.begin() + start + s + frameSize, data.begin());
        window.multiplyWithWindowingTable (data.data(), (size_t) frameSize);
        fft.performFrequencyOnlyForwardTransform (data.data(), true);
        for (int b = 0; b < numBins; ++b) power[(size_t) b] = data[(size_t) b] * data[(size_t) b];

        const auto note = juce::roundToInt (12.0f * std::log2 (percentile (f0, 0.5f) / 440.0f));
        ++frames;

        for (int p = 0; p < points; ++p)
        {
            const auto f = 150.0f * std::pow (2.0f, (float) p / 24.0f);
            const auto fine = spectrum::getBandPower (power.data(), numBins, binWidth, f, 1.0f / 24.0f);
            const auto coarse = spectrum::getBandPower (power.data(), numBins, binWidth, f, 1.0f);
            if (fine <= 0 || coarse <= 0) continue;
            const auto dev = 10.0f * std::log10 (fine / coarse);
            sum[(size_t) p] += dev;
            if (dev >= 2.0f) { ++above[(size_t) p]; notes[(size_t) p].insert (note); }
        }
    }

    std::vector<ResonanceCandidate> out;
    if (frames == 0) return out;

    for (int p = 1; p + 1 < points; ++p)
    {
        const auto v = sum[(size_t) p] / frames;
        if (v < 4.0f || v < sum[(size_t) p - 1] / frames || v < sum[(size_t) p + 1] / frames) continue;
        const auto presence = (float) above[(size_t) p] / (float) frames;
        if (presence >= 0.3f && notes[(size_t) p].size() >= 3)
            out.push_back ({ 150.0f * std::pow (2.0f, (float) p / 24.0f), v, presence, (int) notes[(size_t) p].size() });
    }

    std::sort (out.begin(), out.end(), [] (const auto& l, const auto& r) { return l.prominenceDb > r.prominenceDb; });
    if (out.size() > 3) out.resize (3);
    return out;
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

static const ResonanceCandidate* near (const std::vector<ResonanceCandidate>& list, float f)
{
    const ResonanceCandidate* found = nullptr;
    for (const auto& c : list)
        if (std::abs (std::log2 (c.frequency / f)) <= 1.0f / 12.0f && (found == nullptr || c.prominenceDb > found->prominenceDb))
            found = &c;
    return found;
}

static void resonanceTests (const Recording& rec)
{
    printf ("\n=== RESSONANCIAS %s ===\n", rec.name.c_str());

    // Janelas reais de 30 s, a partir de 0 e da continuacao.
    const auto base = disjointWindows (rec.mono, 30.0f);

    for (size_t w = 0; w < base.size(); ++w)
    {
        printf ("sem pico, janela %d (%.1f s): atual %s\n", (int) w, base[w].start / kRate, candidates (base[w].result.resonances).c_str());
        printf ("                          D73 reconstruida %s\n", candidates (d73Method (rec.mono, base[w].start, base[w].length)).c_str());
    }

    struct Case { float gain, q; };

    for (const auto& c : { Case { 9, 8 }, Case { 6, 8 }, Case { 9, 4 } })
    {
        printf ("\npico inserido +%.0f dB, Q %.0f (janelas: %d)\n", c.gain, c.q, (int) base.size());

        for (float f : { 300.f, 500.f, 1000.f, 2200.f, 3000.f, 4000.f, 6000.f })
        {
            const auto y = withPeak (rec.mono, f, c.q, c.gain);
            std::string current = "", d73 = "";
            char t[64];

            for (size_t w = 0; w < base.size(); ++w)
            {
                const auto run = runCapture (y, base[w].start, 30.0f, 120.0);
                const auto* hit = near (run.result.resonances, f);
                const auto* before = near (base[w].result.resonances, f);
                if (hit) snprintf (t, sizeof t, "%.0fHz %.1fdB%s; ", hit->frequency, hit->prominenceDb, before ? "(ja havia)" : "");
                else snprintf (t, sizeof t, "--; ");
                current += t;

                const auto old = d73Method (y, run.start, run.length);
                const auto oldBase = d73Method (rec.mono, base[w].start, base[w].length);
                const auto* oldHit = near (old, f);
                const auto* oldBefore = near (oldBase, f);
                if (oldHit) snprintf (t, sizeof t, "%.0fHz %.1fdB%s; ", oldHit->frequency, oldHit->prominenceDb, oldBefore ? "(ja havia)" : "");
                else snprintf (t, sizeof t, "--; ");
                d73 += t;
            }

            printf ("  %5.0f Hz | atual %s | D73 %s\n", f, current.c_str(), d73.c_str());
        }
    }
}

int main (int argc, char** argv)
{
    const juce::String folder = juce::String (LAPIDAMIX_ROOT) + "/test-audio/voz-usuario/";
    const auto sm7 = load ("A", argc > 2 ? juce::String (argv[1]) : folder + "VOZ-CANTADA-SHURE-SM7DB.wav");
    const auto sm57 = load ("B", argc > 2 ? juce::String (argv[2]) : folder + "VOZ-CANTADA-SHURE-SM57.wav");

    printf ("\n=== GRAVACAO INTEIRA ===\n");
    const auto wholeA = runCapture (sm7.mono, 0, kInfinite, 1000.0);
    const auto wholeB = runCapture (sm57.mono, 0, kInfinite, 1000.0);
    printWhole ("A", wholeA);
    printWhole ("B", wholeB);

    const auto trackA = makeTrack (sm7.mono, 0, (int) sm7.mono.size(), true);
    const auto trackB = makeTrack (sm57.mono, 0, (int) sm57.mono.size(), true);
    describeTrack (sm7, trackA);
    describeTrack (sm57, trackB);

    stability (sm7, wholeA);
    stability (sm57, wholeB);
    compareMics (sm7, sm57, trackA, trackB, wholeA, wholeB);
    resonanceTests (sm7);
    resonanceTests (sm57);

    fflush (stdout);
    return 0;
}
