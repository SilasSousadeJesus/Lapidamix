// Mede vozes de um banco, cantor por cantor, e procura de onde veio um resultado (D84).
// Uso:
//   banco varredura <pasta data_by_singer>
//   banco detalhe <pasta data_by_singer> <cantor> [outros cantores]
//   banco inicio <gravacao.wav> <segundos de captura> <mediana> <p5>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "core/analysis/SpectrumGrid.h"
#include "core/analysis/VoiceAnalyzer.h"
#include "core/analysis/YinPitchDetector.h"

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

    if (reader == nullptr || reader->sampleRate != kRate)
    {
        printf ("Ignorado: %s\n", file.getFileName().toRawUTF8());
        return;
    }

    juce::AudioBuffer<float> buffer ((int) reader->numChannels, (int) reader->lengthInSamples);
    reader->read (&buffer, 0, buffer.getNumSamples(), 0, true, true);
    const auto start = out.size();
    out.resize (start + (size_t) buffer.getNumSamples(), 0.0f);

    for (int c = 0; c < buffer.getNumChannels(); ++c)
        juce::FloatVectorOperations::addWithMultiply (out.data() + start, buffer.getReadPointer (c), 1.0f / (float) buffer.getNumChannels(), buffer.getNumSamples());
}

// Trechos cantados, na mesma ordem sempre. A fala fica de fora: o plugin mede voz cantada.
// Com ":escalas" depois do nome, usa escalas e arpejos, que alcançam notas mais agudas.
static std::vector<float> loadSinger (const juce::File& root, const juce::String& name)
{
    std::vector<float> mono;
    const auto singer = name.upToFirstOccurrenceOf (":", false, false);

    if (name.endsWith (":escalas"))
    {
        for (const auto* group : { "arpeggios", "scales" })
        {
            auto files = root.getChildFile (singer).getChildFile (group).getChildFile ("straight").findChildFiles (juce::File::findFiles, false, "*.wav");
            files.sort();
            for (const auto& file : files) appendWav (file, mono);
        }

        return mono;
    }

    for (const auto* style : { "straight", "vibrato" })
    {
        auto files = root.getChildFile (singer).getChildFile ("excerpts").getChildFile (style).findChildFiles (juce::File::findFiles, false, "*.wav");
        files.sort();
        for (const auto& file : files) appendWav (file, mono);
    }

    return mono;
}

struct Run { AnalysisResult result; int start = 0, length = 0; bool reached = false; };

// Mesmo fluxo do AnalysisController: entrega de 50 em 50 ms até atingir a meta de voz válida.
static Run capture (const std::vector<float>& x, int start, float target, double capSeconds = 120.0)
{
    VoiceAnalyzer::Settings s; s.sampleRate = kRate;
    VoiceAnalyzer analyzer (s);
    const auto limit = juce::jmin ((int) x.size() - start, (int) (capSeconds * kRate));
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

static float pct (float a, float b) { return b > 0 ? 100.0f * std::abs (a - b) / b : 0.0f; }

static float bandRms (const std::vector<float>& a, const std::vector<float>& b, float low, float high)
{
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

static std::string candidateText (const std::vector<ResonanceCandidate>& list)
{
    std::string s;
    char t[64];
    for (const auto& c : list) { snprintf (t, sizeof t, "%.0f(%.1fdB,%.0f%%,%dn) ", c.frequency, c.prominenceDb, c.presence * 100, c.numNotes); s += t; }
    return s.empty() ? "nenhuma" : s;
}

// Concordância do YIN com um segundo estimador (correlação normalizada), nos blocos de voz.
struct Agreement { int compared = 0; float within3 = 0, octave = 0; };

static Agreement compareEstimators (const std::vector<float>& x)
{
    const auto factor = (int) (kRate / 11025.0);
    const auto rate = kRate / factor;
    auto coefficients = new juce::dsp::IIR::Coefficients<float> (juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass (kRate, (float) (rate * 0.4)));
    juce::dsp::IIR::Filter<float> a, b;
    a.coefficients = coefficients; b.coefficients = coefficients;
    a.prepare ({ kRate, 4096, 1 }); b.prepare ({ kRate, 4096, 1 });

    std::vector<float> low;
    for (size_t i = 0; i < x.size(); ++i) { const auto v = b.processSample (a.processSample (x[i])); if (i % (size_t) factor == 0) low.push_back (v); }

    YinPitchDetector yin;
    yin.prepare (rate, 60.0f, 1100.0f);
    const auto frame = yin.getFrameSize();
    const auto minLag = (int) (rate / 1100.0), maxLag = (int) (rate / 60.0);
    const auto window = frame - maxLag - 2;
    const auto hop = (int) (rate * 0.025);

    int compared = 0, close = 0, octave = 0;
    std::vector<float> c ((size_t) maxLag + 2);

    for (int start = 0; start + frame < (int) low.size(); start += hop)
    {
        const auto* f = low.data() + start;
        double energy = 0; for (int i = 0; i < window; ++i) energy += f[i] * f[i];
        if (energy / window < 1.0e-5) continue;

        const auto p = yin.detect (f);
        if (p.aperiodicity >= 0.3f || p.frequency < 60.0f || p.frequency > 1100.0f) continue;

        auto best = 0.0f;
        for (int lag = minLag; lag <= maxLag + 1; ++lag)
        {
            double xy = 0, e1 = 1e-12;
            for (int i = 0; i < window; ++i) { xy += f[i] * f[i + lag]; e1 += f[i + lag] * f[i + lag]; }
            c[(size_t) lag] = (float) (xy / std::sqrt ((energy + 1e-12) * e1));
            if (lag <= maxLag) best = juce::jmax (best, c[(size_t) lag]);
        }
        if (best < 0.5f) continue;

        auto other = 0.0f;
        for (int lag = minLag + 1; lag <= maxLag; ++lag)
            if (c[(size_t) lag] >= 0.9f * best && c[(size_t) lag] >= c[(size_t) lag - 1] && c[(size_t) lag] >= c[(size_t) lag + 1])
            {
                const auto d = c[(size_t) lag - 1] - 2 * c[(size_t) lag] + c[(size_t) lag + 1];
                const auto shift = std::abs (d) > 1e-9f ? 0.5f * (c[(size_t) lag - 1] - c[(size_t) lag + 1]) / d : 0.0f;
                other = (float) (rate / (lag + shift));
                break;
            }
        if (other <= 0) continue;

        ++compared;
        const auto ratio = p.frequency / other;
        if (std::abs (ratio - 1.0f) <= 0.03f) ++close;
        else if (std::abs (ratio - 2.0f) <= 0.06f || std::abs (ratio - 0.5f) <= 0.015f) ++octave;
    }

    return { compared, compared ? 100.0f * close / compared : 0.0f, compared ? 100.0f * octave / compared : 0.0f };
}

static void scan (const juce::File& root)
{
    auto folders = root.findChildFiles (juce::File::findDirectories, false);
    folders.sort();
    printf ("cantor; material s; voz s; mediana; p5; p95; confianca; notas; busca a partir de; yin x outro %%; oitava %%; candidatas\n");

    for (const auto& folder : folders)
    {
        const auto x = loadSinger (root, folder.getFileName());
        const auto r = capture (x, 0, 1.0e9f, 1000.0).result;
        const auto a = compareEstimators (x);
        printf ("%s; %.0f; %.0f; %.0f; %.0f; %.0f; %.2f; %d; %.0f; %.1f; %.1f; %s\n", folder.getFileName().toRawUTF8(), (double) x.size() / kRate, r.voiceSeconds,
                r.fundamentalMedian, r.fundamentalLow, r.fundamentalHigh, r.confidence, r.numDistinctNotes, 6.0f * r.fundamentalMedian, a.within3, a.octave, candidateText (r.resonances).c_str());
        fflush (stdout);
    }
}

static void detail (const juce::File& root, const juce::String& singer)
{
    const auto x = loadSinger (root, singer);
    const auto whole = capture (x, 0, 1.0e9f, 1000.0).result;
    const auto a = compareEstimators (x);
    printf ("\n=== %s === material %.0f s, voz %.0f s\n", singer.toRawUTF8(), (double) x.size() / kRate, whole.voiceSeconds);
    printf ("f0 mediana %.1f, p5 %.1f, p95 %.1f, confianca %.2f, notas %d; busca a partir de %.0f Hz; yin x outro: %.1f%% dentro de 3%%, %.1f%% oitava (%d blocos)\n",
            whole.fundamentalMedian, whole.fundamentalLow, whole.fundamentalHigh, whole.confidence, whole.numDistinctNotes, 6.0f * whole.fundamentalMedian, a.within3, a.octave, a.compared);
    printf ("candidatas, material inteiro: %s\n", candidateText (whole.resonances).c_str());

    printf ("estabilidade; janelas; captura s; f0 pior x total %%; p95 pior x total %%; p5 entre janelas %%; espectro entre janelas 100-500; 500-2k; 2k-10k\n");

    for (float target : { 10.f, 20.f, 30.f, 45.f })
    {
        const auto w = windows (x, target);
        std::vector<float> med, high, low, b1, b2, b3, cap;

        for (size_t i = 0; i < w.size(); ++i)
        {
            med.push_back (pct (w[i].result.fundamentalMedian, whole.fundamentalMedian));
            high.push_back (pct (w[i].result.fundamentalHigh, whole.fundamentalHigh));
            cap.push_back (w[i].result.capturedSeconds);

            for (size_t j = i + 1; j < w.size(); ++j)
            {
                low.push_back (pct (w[i].result.fundamentalLow, w[j].result.fundamentalLow));
                b1.push_back (bandRms (w[i].result.voiceCurve, w[j].result.voiceCurve, 100, 500));
                b2.push_back (bandRms (w[i].result.voiceCurve, w[j].result.voiceCurve, 500, 2000));
                b3.push_back (bandRms (w[i].result.voiceCurve, w[j].result.voiceCurve, 2000, 10000));
            }
        }

        const auto worst = [] (const std::vector<float>& v) { return v.empty() ? 0.0f : *std::max_element (v.begin(), v.end()); };
        printf ("%.0f s; %d; %.0f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f\n", target, (int) w.size(), percentile (cap, 0.5f), worst (med), worst (high),
                percentile (low, 0.5f), percentile (b1, 0.5f), percentile (b2, 0.5f), percentile (b3, 0.5f));
    }

    const auto base = windows (x, 30.0f);
    for (size_t i = 0; i < base.size(); ++i)
        printf ("janela 30 s %d (inicio %.0f s, captura %.0f s, f0 %.0f, p5 %.0f, p95 %.0f, busca %.0f): %s\n", (int) i, base[i].start / kRate, base[i].result.capturedSeconds,
                base[i].result.fundamentalMedian, base[i].result.fundamentalLow, base[i].result.fundamentalHigh, 6.0f * base[i].result.fundamentalMedian, candidateText (base[i].result.resonances).c_str());

    for (float gain : { 9.f, 6.f })
    {
        printf ("+%.0f dB Q8, achou (sem contar o que ja havia):", gain);
        for (float f : { 300.f, 500.f, 1000.f, 2200.f, 3000.f, 4000.f, 6000.f })
        {
            const auto y = withPeak (x, f, 8.0f, gain);
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
    fflush (stdout);
}

// Simula a regra da banda de ressonância (D87), sem tocar no plugin.
// Usar com banco_todas_candidatas, para o corte de 3 candidatas não esconder uma elegível.
struct RuleCut { float frequency = 0, prominence = 0, cutDb = 0; };

struct RuleOptions
{
    const char* name;
    float zoneMaximum;
    bool bothHalves;
    float ceiling = 8000.0f;
    float minimumProminence = 6.0f;
};

// As duas metades da voz válida de uma captura de 30 s, para medir a persistência.
struct Halves { AnalysisResult first, second; };

static Halves measureHalves (const std::vector<float>& x, const Run& run)
{
    const auto first = capture (x, run.start, 15.0f);
    return { first.result, capture (x, run.start + first.length, 15.0f).result };
}

static bool chooseCut (const AnalysisResult& result, const Halves& halves, const RuleOptions& options, RuleCut& cut)
{
    for (const auto& c : result.resonances) // Já vêm da mais forte para a mais fraca.
    {
        const auto isSingerFormant = c.frequency >= 2500.0f && c.frequency <= options.zoneMaximum;
        const auto notesShare = (float) c.numNotes / (float) juce::jmax (1, result.numDistinctNotes);
        const auto isInBothHalves = hasNear (halves.first.resonances, c.frequency) && hasNear (halves.second.resonances, c.frequency);

        if (c.prominenceDb >= options.minimumProminence && c.frequency <= options.ceiling && c.presence >= 0.6f && notesShare >= 0.5f && ! isSingerFormant
            && (isInBothHalves || ! options.bothHalves))
        {
            cut = { c.frequency, c.prominenceDb, juce::jmin (3.0f, c.prominenceDb / 3.0f) };
            return true;
        }
    }

    return false;
}

static void ruleTest (const juce::File& root, int argc, char** argv)
{
    const RuleOptions variants[] = { { "antes: zona ate 3,5 kHz", 3500.0f, false },
                                     { "zona ate 4,2 kHz", 4200.0f, false },
                                     { "duas metades, zona ate 3,5 kHz", 3500.0f, true },
                                     { "regra aprovada: zona ate 4,2 kHz e duas metades", 4200.0f, true },
                                     { "aprovada, com teto de 6 kHz", 4200.0f, true, 6000.0f },
                                     { "aprovada, com minimo de +8 dB", 4200.0f, true, 8000.0f, 8.0f },
                                     { "aprovada, com teto de 6 kHz e minimo de +8 dB", 4200.0f, true, 6000.0f, 8.0f } };

    struct Measured { juce::String name; std::vector<Run> runs; std::vector<Halves> halves; };
    std::vector<Measured> voices;

    auto folders = root.findChildFiles (juce::File::findDirectories, false);
    folders.sort();

    for (const auto& folder : folders)
    {
        const auto x = loadSinger (root, folder.getFileName());
        Measured measured { folder.getFileName(), windows (x, 30.0f), {} };
        for (const auto& run : measured.runs) measured.halves.push_back (measureHalves (x, run));
        voices.push_back (std::move (measured));
    }

    for (const auto& options : variants)
    {
        printf ("\n=== %s ===\n", options.name);
        int totalCuts = 0, totalWindows = 0, voicesCut = 0, region[4] = {};

        for (const auto& voice : voices)
        {
            int cuts = 0;
            printf ("%s:", voice.name.toRawUTF8());

            for (size_t i = 0; i < voice.runs.size(); ++i)
            {
                RuleCut cut;
                if (chooseCut (voice.runs[i].result, voice.halves[i], options, cut))
                {
                    ++cuts;
                    ++region[cut.frequency < 2500.0f ? 0 : (cut.frequency <= 4200.0f ? 1 : (cut.frequency < 6000.0f ? 2 : 3))];
                    printf (" %.0fHz(+%.1f,-%.1f)", cut.frequency, cut.prominence, cut.cutDb);
                }
                else
                    printf (" nada");
            }

            printf (" | %d de %d\n", cuts, (int) voice.runs.size());
            totalCuts += cuts;
            totalWindows += (int) voice.runs.size();
            if (cuts > 0) ++voicesCut;
        }

        printf ("TOTAL: %d de %d janelas; %d de %d vozes; abaixo de 2,5 kHz %d, de 2,5 a 4,2 kHz %d, de 4,2 a 6 kHz %d, acima de 6 kHz %d\n",
                totalCuts, totalWindows, voicesCut, (int) voices.size(), region[0], region[1], region[2], region[3]);
        fflush (stdout);
    }

    const auto& approved = variants[3];

    for (int i = 3; i < argc; ++i)
    {
        std::vector<float> x;
        const juce::File file (juce::String::fromUTF8 (argv[i]));
        appendWav (file, x);
        printf ("\n%s, regra aprovada:", file.getFileNameWithoutExtension().toRawUTF8());

        for (const auto& run : windows (x, 30.0f))
        {
            RuleCut cut;
            if (chooseCut (run.result, measureHalves (x, run), approved, cut)) printf (" %.0fHz(+%.1f,-%.1f)", cut.frequency, cut.prominence, cut.cutDb);
            else printf (" nada");
        }

        printf ("\n");

        for (float f : { 1000.f, 2200.f, 3000.f, 4000.f, 6000.f })
        {
            int hits = 0, total = 0;
            const auto y = withPeak (x, f, 8.0f, 9.0f);
            for (const auto& run : windows (y, 30.0f))
            {
                RuleCut cut;
                ++total;
                if (chooseCut (run.result, measureHalves (y, run), approved, cut) && std::abs (std::log2 (cut.frequency / f)) <= 1.0f / 12.0f) ++hits;
            }
            printf ("  +9 dB Q8 em %.0f Hz: a regra corta o pico em %d de %d janelas\n", f, hits, total);
        }
    }
}

// Grava o trecho que o plugin analisa, para outra ferramenta medir exatamente o mesmo áudio.
static void writeWav (const juce::File& file, const float* samples, int numSamples)
{
    file.deleteFile();
    std::unique_ptr<juce::OutputStream> stream = file.createOutputStream();
    juce::WavAudioFormat format;
    auto writer = format.createWriterFor (stream, juce::AudioFormatWriterOptions{}.withSampleRate (kRate).withNumChannels (1).withBitsPerSample (24));
    const float* channels[] = { samples };
    writer->writeFromFloatArrays (channels, 1, numSamples);
}

static void exportForComparison (const juce::File& root, const juce::File& output, int argc, char** argv)
{
    juce::FileOutputStream table (output.getChildFile ("plugin.csv"));
    table.setPosition (0);
    table.truncate();
    table.writeText ("nome,trecho,arquivo,captura,mediana,p5,p95,curva\n", false, false, nullptr);

    const auto writeRow = [&] (const juce::String& name, const char* part, const std::vector<float>& x, const Run& run)
    {
        const auto label = name + "_" + part;
        writeWav (output.getChildFile (label + ".wav"), x.data() + run.start, run.length);
        const auto& r = run.result;
        juce::String curveText;
        for (const auto value : r.soundCurve) curveText << juce::String (value, 2) << " ";
        table.writeText (label + "," + part + "," + label + ".wav," + juce::String (r.capturedSeconds, 1) + "," + juce::String (r.fundamentalMedian, 2) + ","
                             + juce::String (r.fundamentalLow, 2) + "," + juce::String (r.fundamentalHigh, 2) + "," + curveText + "\n", false, false, nullptr);
    };

    for (int i = 4; i < argc; ++i)
    {
        const juce::String argument = juce::String::fromUTF8 (argv[i]);
        std::vector<float> x;
        juce::String name = argument;

        if (argument.endsWithIgnoreCase (".wav"))
        {
            name = juce::File (argument).getFileNameWithoutExtension();
            appendWav (juce::File (argument), x);
        }
        else
            x = loadSinger (root, argument);

        writeRow (name, "30s", x, capture (x, 0, 30.0f));
        writeRow (name, "inteiro", x, capture (x, 0, 1.0e9f, 1000.0));
    }
}

// Procura de que ponto da gravação sai um resultado visto no DAW.
static void findStart (const juce::File& file, float capturedSeconds, float median, float low)
{
    std::vector<float> x;
    appendWav (file, x);
    printf ("inicio s; captura; mediana; p5; p95; candidatas (melhores 5)\n");

    struct Row { float score, start; AnalysisResult r; };
    std::vector<Row> rows;

    for (float start = 0.0f; start + 20.0f < (float) ((double) x.size() / kRate); start += 1.0f)
    {
        const auto run = capture (x, (int) (start * kRate), 30.0f);
        if (! run.reached) continue;
        const auto score = std::abs (run.result.capturedSeconds - capturedSeconds) / 2.0f + std::abs (run.result.fundamentalMedian - median) / 2.0f + std::abs (run.result.fundamentalLow - low) / 2.0f;
        rows.push_back ({ score, start, run.result });
    }

    std::sort (rows.begin(), rows.end(), [] (const Row& a, const Row& b) { return a.score < b.score; });
    for (size_t i = 0; i < juce::jmin ((size_t) 5, rows.size()); ++i)
        printf ("%.0f; %.1f; %.1f; %.1f; %.1f; %s\n", rows[i].start, rows[i].r.capturedSeconds, rows[i].r.fundamentalMedian, rows[i].r.fundamentalLow, rows[i].r.fundamentalHigh, candidateText (rows[i].r.resonances).c_str());
    for (const auto& row : rows)
        if (row.start == 0.0f)
            printf ("do inicio da gravacao: captura %.1f; mediana %.1f; p5 %.1f; p95 %.1f; %s\n", row.r.capturedSeconds, row.r.fundamentalMedian, row.r.fundamentalLow, row.r.fundamentalHigh, candidateText (row.r.resonances).c_str());
}

int main (int argc, char** argv)
{
    const juce::String mode = argc > 1 ? argv[1] : "";

    if (mode == "varredura" && argc == 3)
        scan (juce::File (juce::String::fromUTF8 (argv[2])));
    else if (mode == "detalhe" && argc >= 4)
        for (int i = 3; i < argc; ++i) detail (juce::File (juce::String::fromUTF8 (argv[2])), juce::String::fromUTF8 (argv[i]));
    else if (mode == "exportar" && argc >= 5)
        exportForComparison (juce::File (juce::String::fromUTF8 (argv[2])), juce::File (juce::String::fromUTF8 (argv[3])), argc, argv);
    else if (mode == "regra" && argc >= 3)
        ruleTest (juce::File (juce::String::fromUTF8 (argv[2])), argc, argv);
    else if (mode == "inicio" && argc == 6)
        findStart (juce::File (juce::String::fromUTF8 (argv[2])), (float) std::atof (argv[3]), (float) std::atof (argv[4]), (float) std::atof (argv[5]));
    else
        printf ("Uso: banco varredura <pasta> | banco detalhe <pasta> <cantor>... | banco inicio <wav> <captura> <mediana> <p5>\n");

    return 0;
}
