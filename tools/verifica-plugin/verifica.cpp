// Confere o plugin sem DAW, com as gravações de voz do usuário (D77, D84).
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <vector>

#include "core/correction/CorrectionRules.h"
#include "plugin/PluginProcessor.h"
#include "profiles/InstrumentProfile.h"

// Conta pedidos de memória só enquanto a própria thread estiver dentro do processBlock.
static thread_local bool insideAudioCallback = false;
static std::atomic<int> audioThreadAllocations { 0 };

void* operator new (std::size_t size)
{
    if (insideAudioCallback) ++audioThreadAllocations;
    if (auto* pointer = std::malloc (size)) return pointer;
    throw std::bad_alloc();
}

void operator delete (void* pointer) noexcept { std::free (pointer); }
void operator delete (void* pointer, std::size_t) noexcept { std::free (pointer); }

static constexpr double kRate = 44100.0;
static constexpr int kBlock = 512;
static int failures = 0;

static void check (bool passed, const juce::String& text)
{
    printf ("%s  %s\n", passed ? "OK    " : "FALHOU", text.toRawUTF8());
    fflush (stdout);
    if (! passed) ++failures;
}

static juce::AudioBuffer<float> loadStereo (const juce::String& name)
{
    juce::WavAudioFormat format;
    const auto file = juce::File (juce::String (LAPIDAMIX_ROOT) + "/test-audio/voz-usuario/" + name);
    std::unique_ptr<juce::AudioFormatReader> reader (format.createReaderFor (new juce::FileInputStream (file), true));

    if (reader == nullptr) { printf ("Nao leu %s\n", name.toRawUTF8()); std::exit (2); }

    juce::AudioBuffer<float> buffer (2, (int) reader->lengthInSamples);
    reader->read (&buffer, 0, buffer.getNumSamples(), 0, true, true);
    return buffer;
}

static void prepare (LapidamixAudioProcessor& processor)
{
    processor.setPlayConfigDetails (2, 2, kRate, kBlock);
    processor.prepareToPlay (kRate, kBlock);
}

static void process (LapidamixAudioProcessor& processor, juce::AudioBuffer<float>& block)
{
    juce::MidiBuffer midi;
    insideAudioCallback = true;
    processor.processBlock (block, midi);
    insideAudioCallback = false;
}

static void setParameter (LapidamixAudioProcessor& processor, const juce::String& id, float realValue)
{
    auto* parameter = processor.getParameters().getParameter (id);
    parameter->setValueNotifyingHost (parameter->convertTo0to1 (realValue));
}

// Toca `numSamples` e pede para parar: com a entrega já encerrada, a captura tem tamanho exato.
static lapidamix::AnalysisResult analyseExactly (LapidamixAudioProcessor& processor, const juce::AudioBuffer<float>& audio, int numSamples)
{
    const auto versionBefore = processor.getResultVersion();
    processor.startAnalysis();
    juce::AudioBuffer<float> block (2, kBlock);

    for (int start = 0; start + kBlock <= numSamples; start += kBlock)
    {
        block.copyFrom (0, 0, audio, 0, start, kBlock);
        block.copyFrom (1, 0, audio, 1, start, kBlock);
        process (processor, block);
    }

    processor.getAnalysis().requestStop();

    for (int attempt = 0; attempt < 600 && processor.getResultVersion() == versionBefore; ++attempt)
        juce::MessageManager::getInstance()->runDispatchLoopUntil (50);

    return processor.getLastAnalysisResult();
}

static juce::String collectLabelText (juce::Component& parent)
{
    juce::String text;

    for (auto* child : parent.getChildren())
    {
        if (auto* label = dynamic_cast<juce::Label*> (child)) text << label->getText() << "\n";
        text << collectLabelText (*child);
    }

    return text;
}

static void testTransparency (const juce::AudioBuffer<float>& audio)
{
    LapidamixAudioProcessor processor;
    prepare (processor);
    processor.getRta().setActive (true);
    processor.startAnalysis();

    juce::AudioBuffer<float> block (2, kBlock);
    juce::int64 differentSamples = 0, totalSamples = 0;
    double worstBlockMs = 0, totalMs = 0;
    int numBlocks = 0;
    audioThreadAllocations = 0;

    for (int start = 0; start + kBlock <= audio.getNumSamples(); start += kBlock)
    {
        block.copyFrom (0, 0, audio, 0, start, kBlock);
        block.copyFrom (1, 0, audio, 1, start, kBlock);

        const auto before = juce::Time::getMillisecondCounterHiRes();
        process (processor, block);
        const auto elapsed = juce::Time::getMillisecondCounterHiRes() - before;
        worstBlockMs = juce::jmax (worstBlockMs, elapsed);
        totalMs += elapsed;
        ++numBlocks;

        for (int channel = 0; channel < 2; ++channel)
            for (int i = 0; i < kBlock; ++i)
            {
                ++totalSamples;
                if (! juce::exactlyEqual (block.getSample (channel, i), audio.getSample (channel, start + i))) ++differentSamples;
            }

        if (numBlocks % 16 == 0) processor.getRta().update(); // Como o temporizador da janela faz.
    }

    check (differentSamples == 0, "Saida identica a entrada, com analise e RTA ligados: " + juce::String (differentSamples) + " amostras diferentes de " + juce::String (totalSamples));
    check (audioThreadAllocations.load() == 0, "Nenhum pedido de memoria na thread de audio: " + juce::String (audioThreadAllocations.load()));

    const auto blockMs = 1000.0 * kBlock / kRate;
    printf ("INDICIO  Tempo por bloco de %.1f ms: media %.4f ms (%.2f%%), pior %.3f ms. Nao substitui o teste de CPU no DAW.\n", blockMs, totalMs / numBlocks, 100.0 * (totalMs / numBlocks) / blockMs, worstBlockMs);
    check (worstBlockMs < blockMs, "Pior bloco abaixo do tempo disponivel");
    processor.getAnalysis().abort();
}

static void testBypassAndGain (const juce::AudioBuffer<float>& audio)
{
    LapidamixAudioProcessor processor;
    prepare (processor);
    setParameter (processor, lapidamix::EqModule::getEnabledParameterId (1), 1.0f);
    setParameter (processor, lapidamix::EqModule::getGainParameterId (1), 6.0f);
    setParameter (processor, LapidamixAudioProcessor::bypassParameterId, 1.0f);
    processor.prepareToPlay (kRate, kBlock);

    juce::AudioBuffer<float> block (2, kBlock);
    juce::int64 different = 0;

    for (int start = 0; start < kBlock * 400; start += kBlock)
    {
        block.copyFrom (0, 0, audio, 0, start + 200000, kBlock);
        block.copyFrom (1, 0, audio, 1, start + 200000, kBlock);
        process (processor, block);

        for (int i = 0; i < kBlock; ++i)
            if (! juce::exactlyEqual (block.getSample (0, i), audio.getSample (0, start + 200000 + i))) ++different;
    }

    check (different == 0, "Bypass com banda ligada: saida identica a entrada");
}

static void testAnalysisIsBeforeEq (const juce::AudioBuffer<float>& audio)
{
    const auto numSamples = (int) (20.0 * kRate);

    LapidamixAudioProcessor plain;
    prepare (plain);
    const auto a = analyseExactly (plain, audio, numSamples);

    LapidamixAudioProcessor withEq;
    prepare (withEq);
    setParameter (withEq, lapidamix::EqModule::getEnabledParameterId (2), 1.0f);
    setParameter (withEq, lapidamix::EqModule::getFrequencyParameterId (2), 2200.0f);
    setParameter (withEq, lapidamix::EqModule::getGainParameterId (2), 12.0f);
    const auto b = analyseExactly (withEq, audio, numSamples);

    auto worst = 0.0f;
    for (size_t i = 0; i < a.voiceCurve.size() && i < b.voiceCurve.size(); ++i)
        worst = juce::jmax (worst, std::abs (a.voiceCurve[i] - b.voiceCurve[i]));

    check (a.isValid && b.isValid && ! a.voiceCurve.empty() && worst < 0.001f && juce::approximatelyEqual (a.fundamentalMedian, b.fundamentalMedian),
           "Analise mede antes do EQ: medias iguais com +12 dB em 2,2 kHz (maior diferenca " + juce::String (worst, 4) + " dB)");
}

static void testRtaFollowsEq()
{
    // Ruído branco: energia conhecida em todas as frequências.
    const auto measure = [] (bool eqOn)
    {
        LapidamixAudioProcessor processor;
        prepare (processor);
        processor.getRta().setActive (true);

        if (eqOn)
        {
            setParameter (processor, lapidamix::EqModule::getEnabledParameterId (2), 1.0f);
            setParameter (processor, lapidamix::EqModule::getFrequencyParameterId (2), 2200.0f);
            setParameter (processor, lapidamix::EqModule::getGainParameterId (2), 12.0f);
            setParameter (processor, lapidamix::EqModule::getQParameterId (2), 2.0f);
            processor.prepareToPlay (kRate, kBlock);
        }

        juce::Random random (1234);
        juce::AudioBuffer<float> block (2, kBlock);
        RtaAnalyzer::Curve curve {};

        for (int n = 0; n < 2000; ++n)
        {
            for (int i = 0; i < kBlock; ++i)
            {
                const auto v = 0.1f * (random.nextFloat() * 2.0f - 1.0f);
                block.setSample (0, i, v);
                block.setSample (1, i, v);
            }

            process (processor, block);
            if (n % 8 == 0) curve = processor.getRta().update();
        }

        return curve;
    };

    const auto off = measure (false), on = measure (true);
    const auto at = [] (const RtaAnalyzer::Curve& curve, float frequency)
    {
        auto point = 0; auto nearest = 1.0e9f;
        for (int p = 0; p < lapidamix::spectrum::numDisplayPoints; ++p)
        {
            const auto e = std::abs (lapidamix::spectrum::getDisplayFrequency (p) - frequency);
            if (e < nearest) { nearest = e; point = p; }
        }
        return curve[(size_t) point];
    };

    const auto change2200 = at (on, 2200.0f) - at (off, 2200.0f);
    const auto change100 = at (on, 100.0f) - at (off, 100.0f);
    check (change2200 > 6.0f && std::abs (change100) < 3.0f,
           "RTA reage ao EQ: " + juce::String (change2200, 1) + " dB em 2,2 kHz e " + juce::String (change100, 1) + " dB em 100 Hz, com +12 dB em 2,2 kHz");
}

static void testShortAnalysisAndState (const juce::AudioBuffer<float>& audio)
{
    // Parar cedo: poucos segundos de voz devem gerar o aviso na janela.
    auto processor = std::make_unique<LapidamixAudioProcessor>();
    prepare (*processor);
    const auto shortResult = analyseExactly (*processor, audio, (int) (10.0 * kRate));

    std::unique_ptr<juce::AudioProcessorEditor> editor (processor->createEditor());
    juce::MessageManager::getInstance()->runDispatchLoopUntil (200);
    const auto warning = collectLabelText (*editor).contains (juce::String::fromUTF8 ("Atenção: menos de 10 s"));
    check (shortResult.isValid && shortResult.voiceSeconds > 0.0f && shortResult.voiceSeconds < 10.0f && warning,
           "Aviso aparece com " + juce::String (shortResult.voiceSeconds, 1) + " s de voz");
    editor.reset();

    // Análise completa, sem aviso, para testar salvar e restaurar.
    const auto full = analyseExactly (*processor, audio, (int) (60.0 * kRate));
    editor.reset (processor->createEditor());
    juce::MessageManager::getInstance()->runDispatchLoopUntil (200);
    check (! collectLabelText (*editor).contains (juce::String::fromUTF8 ("Atenção")), "Sem aviso com " + juce::String (full.voiceSeconds, 1) + " s de voz");
    editor.reset();

    juce::MemoryBlock state;
    processor->getStateInformation (state);
    processor.reset();

    LapidamixAudioProcessor restored;
    restored.setStateInformation (state.getData(), (int) state.getSize());
    const auto r = restored.getLastAnalysisResult();

    auto curvesEqual = r.voiceCurve.size() == full.voiceCurve.size() && r.soundCurve.size() == full.soundCurve.size() && ! r.voiceCurve.empty();
    for (size_t i = 0; curvesEqual && i < r.voiceCurve.size(); ++i)
        curvesEqual = std::abs (r.voiceCurve[i] - full.voiceCurve[i]) < 0.051f && std::abs (r.soundCurve[i] - full.soundCurve[i]) < 0.051f; // O estado guarda as curvas com uma casa decimal.

    auto resonancesEqual = r.resonances.size() == full.resonances.size();
    for (size_t i = 0; resonancesEqual && i < r.resonances.size(); ++i)
        resonancesEqual = std::abs (r.resonances[i].frequency - full.resonances[i].frequency) < 0.01f
                       && r.resonances[i].isInBothHalves == full.resonances[i].isInBothHalves;

    check (r.isValid && curvesEqual && resonancesEqual && std::abs (r.octaveJumpShare - full.octaveJumpShare) < 0.0001f
               && std::abs (r.fundamentalMedian - full.fundamentalMedian) < 0.01f
               && std::abs (r.fundamentalLow - full.fundamentalLow) < 0.01f && std::abs (r.fundamentalHigh - full.fundamentalHigh) < 0.01f,
           "Salvar e restaurar: medidas, curvas e candidatas voltam (" + juce::String ((int) state.getSize()) + " bytes, sem audio)");
}

static void testInterruption (const juce::AudioBuffer<float>& audio)
{
    {
        LapidamixAudioProcessor processor;
        prepare (processor);
        processor.startAnalysis();
        juce::AudioBuffer<float> block (2, kBlock);

        for (int start = 0; start < kBlock * 200; start += kBlock)
        {
            block.copyFrom (0, 0, audio, 0, start, kBlock);
            block.copyFrom (1, 0, audio, 1, start, kBlock);
            process (processor, block);
        }
        // Fecha o plugin com a análise rodando.
    }

    check (true, "Fechar o plugin com a analise rodando nao trava");
}

static void testResonance (const juce::AudioBuffer<float>& audio, const juce::String& name)
{
    // Quem está antes do plugin na trilha, como um ReaEQ com um pico estreito.
    juce::AudioBuffer<float> filtered (audio);
    juce::dsp::IIR::Coefficients<float>::Ptr coefficients = new juce::dsp::IIR::Coefficients<float> (juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter (kRate, 2200.0f, 8.0f, juce::Decibels::decibelsToGain (9.0f)));

    for (int channel = 0; channel < 2; ++channel)
    {
        juce::dsp::IIR::Filter<float> filter;
        filter.coefficients = coefficients;
        filter.prepare ({ kRate, 4096, 1 });
        auto* samples = filtered.getWritePointer (channel);
        for (int i = 0; i < filtered.getNumSamples(); ++i) samples[i] = filter.processSample (samples[i]);
    }

    for (const auto seconds : { 50.0, 100.0 })
    {
        LapidamixAudioProcessor processor;
        prepare (processor);
        const auto r = analyseExactly (processor, filtered, (int) (seconds * kRate));
        juce::String list;
        auto found = false;

        for (const auto& c : r.resonances)
        {
            list << juce::String (c.frequency, 0) << " Hz (+" << juce::String (c.prominenceDb, 1) << " dB) ";
            if (std::abs (std::log2 (c.frequency / 2200.0f)) <= 1.0f / 12.0f) found = true;
        }

        printf ("%s  %s, primeiros %.0f s, pico de +9 dB em 2,2 kHz: %s\n", found ? "ACHOU " : "NAO   ", name.toRawUTF8(), seconds, list.isEmpty() ? "nenhuma" : list.toRawUTF8());
        fflush (stdout);
    }
}

// As regras da Fase 4, com medidas conhecidas, para exercitar também o que a voz real não exercita.
static lapidamix::AnalysisResult makeKnownAnalysis (float midsDb, float mudDb, float presenceDb)
{
    using namespace lapidamix;
    AnalysisResult analysis;
    analysis.isValid = true;
    analysis.voiceSeconds = 30.0f;
    analysis.fundamentalLow = 100.0f;
    analysis.fundamentalMedian = 135.0f;
    analysis.fundamentalHigh = 150.0f;
    analysis.confidence = 0.99f;
    analysis.numDistinctNotes = 10;
    analysis.voiceCurve.assign (spectrum::numDisplayPoints, -80.0f);
    analysis.soundCurve = analysis.voiceCurve;

    for (int point = 0; point < spectrum::numDisplayPoints; ++point)
    {
        const auto frequency = spectrum::getDisplayFrequency (point);
        auto& value = analysis.voiceCurve[(size_t) point];

        if (frequency >= 500.0f && frequency < 2000.0f) value = midsDb;
        else if (frequency >= 200.0f && frequency < 500.0f) value = mudDb;
        else if (frequency >= 2000.0f && frequency < 5000.0f) value = presenceDb;
    }

    return analysis;
}

static void testRules()
{
    using namespace lapidamix;
    const auto& settings = voiceProfile.correction;
    const auto near = [] (float a, float b) { return std::abs (a - b) < 0.05f; };

    {
        const auto plan = makeCorrectionPlan ({}, settings);
        check (! plan.highPass.isEnabled && ! plan.mud.isEnabled && ! plan.resonance.isEnabled && ! plan.presence.isEnabled && ! plan.air.isEnabled,
               "Regras: sem analise, nada e proposto");
    }

    {
        const auto plan = makeCorrectionPlan (makeKnownAnalysis (-40.0f, -40.0f, -55.0f), settings);
        check (near (plan.highPass.frequency, 70.7f) && plan.highPass.reason == CorrectionReason::applied, "Regras: passa-alta meia oitava abaixo do grave de 100 Hz: " + juce::String (plan.highPass.frequency, 1) + " Hz");
        check (! plan.mud.isEnabled && plan.mud.reason == CorrectionReason::withinRange, "Regras: embolado dentro da faixa nao age");
        check (! plan.presence.isEnabled && plan.presence.reason == CorrectionReason::withinRange, "Regras: presenca dentro da faixa nao age");
        check (! plan.air.isEnabled && plan.air.reason == CorrectionReason::notAutomatic, "Regras: ar nao age sozinho (D97)");
    }

    {
        const auto plan = makeCorrectionPlan (makeKnownAnalysis (-40.0f, -28.0f, -70.0f), settings);
        check (plan.mud.isEnabled && near (plan.mud.gainDb, -5.0f) && near (plan.mud.q, 1.0f) && near (plan.mud.frequency, 316.2f), "Regras: embolado de +12 dB corta no teto de -5 dB, Q 1, em 316 Hz");
        check (plan.presence.isEnabled && near (plan.presence.gainDb, 3.0f) && near (plan.presence.q, 0.7f) && near (plan.presence.frequency, 3500.0f), "Regras: falta de presenca realca no teto de +3 dB, Q 0,7, em 3,5 kHz");
    }

    {
        const auto small = makeCorrectionPlan (makeKnownAnalysis (-40.0f, -36.5f, -59.5f), settings);
        check (! small.mud.isEnabled && ! small.presence.isEnabled, "Regras: menos de 1 dB de acao nao liga a banda");
        const auto partial = makeCorrectionPlan (makeKnownAnalysis (-40.0f, -35.0f, -60.5f), settings);
        check (near (partial.mud.gainDb, -2.0f) && near (partial.presence.gainDb, 1.5f), "Regras: corrige so o que passa da faixa");
    }

    {
        // Grave medido uma oitava acima: o teto impede subir demais, e a medida inconsistente derruba o filtro.
        auto analysis = makeKnownAnalysis (-40.0f, -40.0f, -55.0f);
        analysis.fundamentalLow = 500.0f;
        check (near (makeCorrectionPlan (analysis, settings).highPass.frequency, 100.0f), "Regras: passa-alta nunca passa de 100 Hz");
        analysis.fundamentalLow = 40.0f;
        check (near (makeCorrectionPlan (analysis, settings).highPass.frequency, 40.0f), "Regras: passa-alta nunca fica abaixo de 40 Hz");

        analysis.fundamentalLow = 200.0f;
        analysis.confidence = 0.7f;
        auto plan = makeCorrectionPlan (analysis, settings);
        check (near (plan.highPass.frequency, 40.0f) && plan.highPass.reason == CorrectionReason::inconsistentFundamental, "Regras: confianca baixa deixa o passa-alta em 40 Hz (D91)");

        analysis.confidence = 0.99f;
        analysis.octaveJumpShare = 0.2f;
        plan = makeCorrectionPlan (analysis, settings);
        check (near (plan.highPass.frequency, 40.0f) && plan.highPass.reason == CorrectionReason::inconsistentFundamental, "Regras: saltos de oitava deixam o passa-alta em 40 Hz (D91)");
    }

    {
        auto analysis = makeKnownAnalysis (-40.0f, -28.0f, -55.0f);
        analysis.fundamentalHigh = 580.0f;
        const auto plan = makeCorrectionPlan (analysis, settings);
        check (! plan.mud.isEnabled && plan.mud.reason == CorrectionReason::protectedFundamental, "Regras: voz aguda protege a fundamental, e o embolado nao age");
    }

    {
        const auto withCandidate = [&] (ResonanceCandidate candidate)
        {
            auto analysis = makeKnownAnalysis (-40.0f, -40.0f, -55.0f);
            analysis.resonances.push_back (candidate);
            return makeCorrectionPlan (analysis, settings).resonance;
        };

        const ResonanceCandidate good { 1500.0f, 9.0f, 0.7f, 8, true };
        const auto cut = withCandidate (good);
        check (cut.isEnabled && near (cut.gainDb, -3.0f) && near (cut.q, 4.0f), "Regras: candidata elegivel corta um terco, ate -3 dB, Q 4");

        auto weak = good;       weak.prominenceDb = 5.9f;
        auto brief = good;      brief.presence = 0.5f;
        auto fewNotes = good;   fewNotes.numNotes = 4;
        auto oneHalf = good;    oneHalf.isInBothHalves = false;
        auto formant = good;    formant.frequency = 3000.0f;
        auto top = good;        top.frequency = 7000.0f;
        check (! withCandidate (weak).isEnabled && ! withCandidate (brief).isEnabled && ! withCandidate (fewNotes).isEnabled && ! withCandidate (oneHalf).isEnabled,
               "Regras: candidata fraca, passageira, em poucas notas ou em uma so metade nao corta");
        check (! withCandidate (formant).isEnabled && ! withCandidate (top).isEnabled, "Regras: nao corta de 2,5 a 4,2 kHz nem acima de 6 kHz (D87, D88)");

        auto limitLow = good;   limitLow.frequency = 4200.0f;
        auto limitHigh = good;  limitHigh.frequency = 6000.0f;
        check (! withCandidate (limitLow).isEnabled && withCandidate (limitHigh).isEnabled, "Regras: limites das zonas: 4,2 kHz protegido, 6 kHz ainda atua");
    }
}

// A análise de verdade calculando os dados novos, com sinais de resposta conhecida.
static lapidamix::AnalysisResult analyseMono (const std::vector<float>& mono)
{
    lapidamix::VoiceAnalyzer::Settings settings;
    settings.sampleRate = kRate;
    lapidamix::VoiceAnalyzer analyzer (settings);
    return analyzer.finish (mono.data(), (int) mono.size(), nullptr);
}

static void testAnalysisData (const juce::AudioBuffer<float>& audio)
{
    using namespace lapidamix;

    // Tom constante × tom que salta uma oitava a cada 200 ms.
    const auto makeTone = [] (bool jumps)
    {
        std::vector<float> tone ((size_t) (20.0 * kRate));
        auto phase = 0.0;
        for (size_t i = 0; i < tone.size(); ++i)
        {
            const auto isHigh = jumps && ((int) ((double) i / kRate / 0.2) % 2) == 1;
            phase += juce::MathConstants<double>::twoPi * (isHigh ? 300.0 : 150.0) / kRate;
            tone[i] = 0.2f * (float) std::sin (phase);
        }
        return tone;
    };

    const auto steady = analyseMono (makeTone (false));
    const auto jumping = analyseMono (makeTone (true));
    check (steady.octaveJumpShare < 0.001f, "Analise: tom constante, " + juce::String (steady.octaveJumpShare * 100.0f, 1) + "% de saltos de oitava");
    check (jumping.octaveJumpShare > voiceProfile.correction.highPass.maximumOctaveJumpShare,
           "Analise: tom que salta uma oitava, " + juce::String (jumping.octaveJumpShare * 100.0f, 1) + "% de saltos");
    check (makeCorrectionPlan (jumping, voiceProfile.correction).highPass.reason == CorrectionReason::inconsistentFundamental,
           "Analise e regras juntas: saltos reais deixam o passa-alta em 40 Hz (D91)");

    // Um pico inserido em toda a gravação × só na primeira metade da voz.
    std::vector<float> mono ((size_t) (60.0 * kRate));
    for (size_t i = 0; i < mono.size(); ++i)
        mono[i] = 0.5f * (audio.getSample (0, (int) i) + audio.getSample (1, (int) i));

    const auto voiceHalfSample = [&]
    {
        // Metade da voz válida, procurada pela própria análise.
        const auto wholeVoice = analyseMono (mono).voiceSeconds;
        for (int seconds = 5; seconds < 60; ++seconds)
        {
            const std::vector<float> part (mono.begin(), mono.begin() + (std::ptrdiff_t) (seconds * kRate));
            if (analyseMono (part).voiceSeconds >= wholeVoice * 0.5f)
                return (size_t) (seconds * kRate);
        }
        return mono.size() / 2;
    }();

    const auto applyPeak = [&] (size_t endSample)
    {
        auto peaked = mono;
        juce::dsp::IIR::Filter<float> filter;
        filter.coefficients = new juce::dsp::IIR::Coefficients<float> (juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter (kRate, 1000.0f, 8.0f, juce::Decibels::decibelsToGain (9.0f)));
        filter.prepare ({ kRate, 4096, 1 });
        for (size_t i = 0; i < endSample; ++i) peaked[i] = filter.processSample (peaked[i]);
        return peaked;
    };

    const auto findPeak = [] (const AnalysisResult& result) -> const ResonanceCandidate*
    {
        for (const auto& candidate : result.resonances)
            if (std::abs (std::log2 (candidate.frequency / 1000.0f)) <= 1.0f / 12.0f)
                return &candidate;
        return nullptr;
    };

    const auto everywhere = analyseMono (applyPeak (mono.size()));
    const auto* inBoth = findPeak (everywhere);
    check (inBoth != nullptr && inBoth->isInBothHalves, "Analise: pico em toda a gravacao aparece nas duas metades");

    const auto oneHalf = analyseMono (applyPeak (voiceHalfSample));
    const auto* inOne = findPeak (oneHalf);
    check (inOne == nullptr || ! inOne->isInBothHalves,
           juce::String ("Analise: pico so na primeira metade nao conta como persistente (") + (inOne == nullptr ? "nem virou candidata)" : "candidata, mas nao nas duas metades)"));
}

int main()
{
    const juce::ScopedJuceInitialiser_GUI gui;
    testRules();

    const auto sm7 = loadStereo ("VOZ-CANTADA-SHURE-SM7DB.wav");
    const auto sm57 = loadStereo ("VOZ-CANTADA-SHURE-SM57.wav");

    testAnalysisData (sm7);
    testTransparency (sm7);
    testBypassAndGain (sm7);
    testAnalysisIsBeforeEq (sm7);
    testRtaFollowsEq();
    testShortAnalysisAndState (sm7);
    testInterruption (sm7);
    testResonance (sm7, "SM7dB");
    testResonance (sm57, "SM57");

    printf ("\nFalhas: %d\n", failures);
    fflush (stdout);
    return failures == 0 ? 0 : 1;
}
