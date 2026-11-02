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

#include "plugin/PluginProcessor.h"

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
        resonancesEqual = std::abs (r.resonances[i].frequency - full.resonances[i].frequency) < 0.01f;

    check (r.isValid && curvesEqual && resonancesEqual && std::abs (r.fundamentalMedian - full.fundamentalMedian) < 0.01f
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

int main()
{
    const juce::ScopedJuceInitialiser_GUI gui;
    const auto sm7 = loadStereo ("VOZ-CANTADA-SHURE-SM7DB.wav");
    const auto sm57 = loadStereo ("VOZ-CANTADA-SHURE-SM57.wav");

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
