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

#include "core/Loudness.h"
#include "core/correction/CorrectionRules.h"
#include "plugin/PluginProcessor.h"
#include "profiles/CorrectionExplanation.h"
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
        if (auto* editor = dynamic_cast<juce::TextEditor*> (child)) text << editor->getText() << "\n";
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
    const auto warningText = juce::String::fromUTF8 ("Pouca voz para propor correções. Cante mais ou repita o trecho em loop até chegar a 10 s.");
    const auto findApplyButton = [] (juce::Component& parent) -> juce::Button*
    {
        for (auto* child : parent.getChildren())
            if (auto* button = dynamic_cast<juce::TextButton*> (child))
                if (button->getButtonText() == "Aplicar")
                    return button;
        return nullptr;
    };

    auto labels = collectLabelText (*editor);
    auto* applyButton = findApplyButton (*editor);
    check (shortResult.isValid && shortResult.voiceSeconds > 0.0f && shortResult.voiceSeconds < 10.0f && labels.contains (warningText),
           "Aviso com a solucao aparece com " + juce::String (shortResult.voiceSeconds, 1) + " s de voz (D101)");
    check (applyButton != nullptr && ! applyButton->isEnabled() && ! labels.contains (juce::String::fromUTF8 ("Correções propostas")),
           "Com pouca voz, nada e proposto e Aplicar fica desligado (D101)");
    check (labels.contains ("Fundamental: mediana"), "Com pouca voz, as medidas continuam na janela");

    // As medidas começam escondidas e voltam com o botão Detalhes (D107).
    const auto findTextButton = [] (juce::Component& parent, const juce::String& text) -> juce::Button*
    {
        for (auto* child : parent.getChildren())
            if (auto* button = dynamic_cast<juce::TextButton*> (child))
                if (button->getButtonText() == text)
                    return button;
        return nullptr;
    };
    const auto findMeasures = [] (juce::Component& parent) -> juce::Component*
    {
        for (auto* child : parent.getChildren())
            if (auto* label = dynamic_cast<juce::Label*> (child))
                if (label->getText().contains ("Fundamental: mediana"))
                    return label;
        return nullptr;
    };
    auto* measures = findMeasures (*editor);
    auto* detailsButton = findTextButton (*editor, "Detalhes");
    check (measures != nullptr && ! measures->isVisible() && detailsButton != nullptr, "Detalhes: as medidas comecam escondidas (D107)");
    if (detailsButton != nullptr) detailsButton->triggerClick();
    juce::MessageManager::getInstance()->runDispatchLoopUntil (100);
    check (measures != nullptr && measures->isVisible(), "Detalhes: um clique mostra as medidas");
    if (detailsButton != nullptr) detailsButton->triggerClick();
    juce::MessageManager::getInstance()->runDispatchLoopUntil (100);
    check (measures != nullptr && ! measures->isVisible(), "Detalhes: outro clique esconde as medidas");
    editor.reset();

    // Análise completa, sem aviso, para testar salvar e restaurar.
    const auto full = analyseExactly (*processor, audio, (int) (60.0 * kRate));
    editor.reset (processor->createEditor());
    juce::MessageManager::getInstance()->runDispatchLoopUntil (200);
    labels = collectLabelText (*editor);
    applyButton = findApplyButton (*editor);
    check (! labels.contains (warningText), "Sem aviso com " + juce::String (full.voiceSeconds, 1) + " s de voz");
    check (labels.contains (juce::String::fromUTF8 ("Correções propostas")) && applyButton != nullptr && applyButton->isEnabled(),
           "Com voz suficiente, a lista aparece e Aplicar fica ligado");

    // Os botões da D106, clicados na janela.
    {
        using lapidamix::EqModule;
        const auto getValue = [&] (const juce::String& id) { return processor->getParameters().getRawParameterValue (id)->load(); };
        auto* clearButton = findTextButton (*editor, "Zerar");
        applyButton->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil (100);
        const auto plan = processor->getCorrectionPlan();
        const auto compensation = getValue (LapidamixAudioProcessor::outputGainParameterId);
        check (applyButton->getButtonText() == juce::String::fromUTF8 ("Reaplicar análise"), "Botoes: depois de aplicar, vira Reaplicar analise (D106)");

        // Ajuste à mão: os controles continuam agindo, e Reaplicar volta à proposta.
        setParameter (*processor, EqModule::getGainParameterId (1), -1.0f);
        setParameter (*processor, LapidamixAudioProcessor::outputGainParameterId, 4.0f);
        check (std::abs (getValue (EqModule::getGainParameterId (1)) + 1.0f) < 0.06f, "Botoes: o ajuste a mao continua valendo ate clicar");
        applyButton->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil (100);
        check (std::abs (getValue (EqModule::getGainParameterId (1)) - plan.mud.gainDb) < 0.06f && std::abs (getValue (LapidamixAudioProcessor::outputGainParameterId) - compensation) < 0.06f,
               "Botoes: Reaplicar volta a proposta e a compensacao");

        // Zerar: tudo desligado, Saída em 0 dB, e mover a intensidade não religa nada.
        setParameter (*processor, EqModule::getEnabledParameterId (4), 1.0f);
        check (clearButton != nullptr, "Botoes: Zerar existe");
        if (clearButton != nullptr) clearButton->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil (100);
        auto allOff = true;
        for (int band = 0; band < lapidamix::numEqBands; ++band) allOff = allOff && getValue (EqModule::getEnabledParameterId (band)) < 0.5f;
        check (allOff && std::abs (getValue (LapidamixAudioProcessor::outputGainParameterId)) < 0.06f && applyButton->getButtonText() == "Aplicar",
               "Botoes: Zerar desliga as 5 bandas, poe a Saida em 0 dB e o botao volta a Aplicar");
        processor->handleIntensityChange();
        setParameter (*processor, LapidamixAudioProcessor::intensityParameterId, 50.0f);
        processor->handleIntensityChange();
        check (getValue (EqModule::getEnabledParameterId (1)) < 0.5f, "Botoes: depois de Zerar, mover a intensidade nao religa as bandas");
        setParameter (*processor, LapidamixAudioProcessor::intensityParameterId, 100.0f);
        processor->handleIntensityChange();
    }

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

// Curva conhecida com o ar no nível pedido. Sem `noiseAirDb`, a análise não encontrou pausa.
static lapidamix::AnalysisResult makeAirAnalysis (float airDb, float noiseAirDb = 0.0f, float hissDb = 0.0f)
{
    using namespace lapidamix;
    auto analysis = makeKnownAnalysis (-40.0f, -40.0f, -55.0f);

    if (noiseAirDb < 0.0f)
        analysis.noiseCurve.assign (spectrum::numDisplayPoints, -119.0f);

    for (int point = 0; point < spectrum::numDisplayPoints; ++point)
    {
        const auto frequency = spectrum::getDisplayFrequency (point);

        if (frequency >= 10000.0f && frequency < 14000.0f)
        {
            analysis.voiceCurve[(size_t) point] = airDb;

            if (noiseAirDb < 0.0f)
                analysis.noiseCurve[(size_t) point] = noiseAirDb;
        }
        else if (noiseAirDb < 0.0f && hissDb < 0.0f && frequency >= 5000.0f && frequency < 10000.0f)
        {
            analysis.noiseCurve[(size_t) point] = hissDb;
        }
    }

    analysis.soundCurve = analysis.voiceCurve;
    return analysis;
}

// O Ar: quantidade calculada, teto, desconto do ruído e os dois casos de chiado (D108).
static void testAirRules()
{
    using namespace lapidamix;
    const auto& settings = voiceProfile.correction;
    const auto near = [] (float a, float b) { return std::abs (a - b) < 0.06f; };
    const auto plan = [&] (const AnalysisResult& analysis) { return makeCorrectionPlan (analysis, settings).air; };

    // Os médios ficam em -40 dB, então -94 dB no ar medem -54 dB.
    check (near (plan (makeAirAnalysis (-95.5f)).gainDb, 1.5f) && plan (makeAirAnalysis (-95.5f)).reason == CorrectionReason::applied, "Ar: 1,5 dB abaixo do minimo realca 1,5 dB");
    check (! plan (makeAirAnalysis (-94.5f)).isEnabled, "Ar: falta menor que 1 dB nao age");
    check (near (plan (makeAirAnalysis (-110.0f)).gainDb, 3.0f), "Ar: o realce para no teto de +3 dB");
    check (! plan (makeAirAnalysis (-80.0f)).isEnabled, "Ar: dentro da faixa nao age");
    check (near (plan (makeAirAnalysis (-76.0f)).gainDb, -2.0f), "Ar: 2 dB acima do maximo atenua 2 dB");
    check (near (plan (makeAirAnalysis (-60.0f)).gainDb, -5.0f), "Ar: a atenuacao para no teto de -5 dB");

    // O ruído é descontado: -94 dB com ruído a -100 dB valem cerca de -95,25 dB de voz.
    const auto discounted = plan (makeAirAnalysis (-94.0f, -100.0f));
    check (near (discounted.measuredDb, -55.25f) && near (discounted.gainDb, 1.25f), "Ar: o ruido das pausas sai da medida: " + juce::String (discounted.measuredDb, 2) + " dB");

    const auto unreadable = plan (makeAirAnalysis (-70.0f, -70.5f));
    check (! unreadable.isEnabled && near (unreadable.gainDb, 0.0f) && unreadable.reason == CorrectionReason::airNotMeasurable, "Ar: a menos de 1 dB do ruido, nao mexe e avisa");

    // --- O realce pela metade, que nenhum material real disparou. ---
    // Ar a -100 dB e ruído a -104 dB: a voz mede cerca de -62 dB, e o realce pedido bate no teto.
    const auto quiet = makeAirAnalysis (-100.0f, -104.0f);
    const auto quietPlan = makeCorrectionPlan (quiet, settings);
    check (near (quietPlan.air.gainDb, 3.0f) && quietPlan.air.reason == CorrectionReason::applied, "Metade: com pouco chiado, o realce fica inteiro: " + juce::String (estimateHissDb (quiet, quietPlan, settings), 1) + " dB de chiado");

    const auto noisy = makeAirAnalysis (-100.0f, -104.0f, -60.0f);
    const auto noisyPlan = makeCorrectionPlan (noisy, settings);
    check (near (noisyPlan.air.gainDb, 1.5f) && noisyPlan.air.isEnabled && noisyPlan.air.reason == CorrectionReason::reducedForHiss,
           "Metade: com chiado alto, realca a metade, sem deixar de agir: +" + juce::String (noisyPlan.air.gainDb, 1) + " dB");

    // A fronteira: subindo o chiado, a regra dispara exatamente quando o chiado com o realce inteiro passa do limite.
    auto boundaryIsExact = true;
    auto numFull = 0, numHalf = 0;

    for (auto hissDb = -119.0f; hissDb < -40.0f; hissDb += 1.0f)
    {
        const auto analysis = makeAirAnalysis (-100.0f, -104.0f, hissDb);
        const auto result = makeCorrectionPlan (analysis, settings);
        auto full = result;
        full.air.isEnabled = true;
        full.air.gainDb = 3.0f;
        const auto isHalf = result.air.reason == CorrectionReason::reducedForHiss;
        boundaryIsExact = boundaryIsExact && isHalf == (estimateHissDb (analysis, full, settings) > settings.air.maximumHissDb);
        boundaryIsExact = boundaryIsExact && near (result.air.gainDb, isHalf ? 1.5f : 3.0f);
        ++(isHalf ? numHalf : numFull);
    }

    check (boundaryIsExact && numFull > 0 && numHalf > 0, "Metade: dispara so acima de -50 dB de chiado (" + juce::String (numFull) + " casos inteiros, " + juce::String (numHalf) + " pela metade)");

    // Só o realce leva a regra: atenuar não aumenta o chiado.
    const auto bright = makeCorrectionPlan (makeAirAnalysis (-70.0f, -80.0f, -40.0f), settings);
    check (bright.air.gainDb < 0.0f && bright.air.reason == CorrectionReason::applied, "Metade: atenuar nao passa pela regra do chiado");

    // A intensidade escala o ganho já reduzido.
    check (near (scaleCorrectionPlan (noisyPlan, 0.5f, settings).air.gainDb, 0.75f), "Metade: a intensidade de 50% escala o realce ja reduzido");

    // O texto mostra o aviso só nesse caso.
    const auto dictionary = loadRegionDictionary (voiceProfile);
    const auto warning = juce::String::fromUTF8 ("O realce fica menor do que a medida pede, porque há chiado nessa região. Se incomodar, considere um redutor de ruído.");
    check (explainCorrectionPlan (noisyPlan, noisy, 0.0f, voiceProfile, dictionary).contains (warning), "Metade: o texto avisa e sugere o redutor de ruido");
    check (! explainCorrectionPlan (quietPlan, quiet, 0.0f, voiceProfile, dictionary).contains ("redutor"), "Metade: sem chiado alto, nada de aviso");

    const auto unreadablePlan = makeCorrectionPlan (makeAirAnalysis (-70.0f, -70.5f), settings);
    check (explainCorrectionPlan (unreadablePlan, makeAirAnalysis (-70.0f, -70.5f), 0.0f, voiceProfile, dictionary).contains (juce::String::fromUTF8 ("não dá para medir")), "Ar: o texto explica quando nao da para medir");
}

// O mesmo caminho do realce pela metade, com a voz real, a análise real e o plugin de verdade.
static void testAirHissWithRealVoice (const juce::AudioBuffer<float>& audio)
{
    using namespace lapidamix;
    const auto& settings = voiceProfile.correction;

    // Ruído branco gerado, com semente fixa, somado à gravação.
    juce::AudioBuffer<float> noisy (2, (int) (60.0 * kRate));
    juce::Random random (1234);
    const auto amplitude = juce::Decibels::decibelsToGain (-78.0f) * std::sqrt (3.0f);

    for (int channel = 0; channel < 2; ++channel)
        noisy.copyFrom (channel, 0, audio, channel, 0, noisy.getNumSamples());

    for (int sample = 0; sample < noisy.getNumSamples(); ++sample)
    {
        const auto hiss = amplitude * (2.0f * random.nextFloat() - 1.0f);
        noisy.addSample (0, sample, hiss);
        noisy.addSample (1, sample, hiss);
    }

    LapidamixAudioProcessor processor;
    prepare (processor);
    analyseExactly (processor, noisy, noisy.getNumSamples());
    const auto analysis = processor.getLastAnalysisResult();
    const auto plan = processor.getCorrectionPlan();

    auto full = plan;
    full.air.gainDb = plan.air.gainDb / settings.air.hissCautionRatio;

    check (! analysis.noiseCurve.empty() && plan.air.reason == CorrectionReason::reducedForHiss,
           "Metade, voz real com chiado de -78 dB: medida " + juce::String (plan.air.measuredDb, 1) + " dB; chiado " + juce::String (estimateHissDb (analysis, full, settings), 1)
               + " dB com o realce inteiro; realce final +" + juce::String (plan.air.gainDb, 1) + " dB");

    processor.applyCurrentCorrection();
    const auto written = processor.getParameters().getRawParameterValue (EqModule::getGainParameterId (4))->load();
    check (std::abs (written - plan.air.gainDb) < 0.06f && processor.getParameters().getRawParameterValue (EqModule::getEnabledParameterId (4))->load() > 0.5f,
           "Metade, voz real: Aplicar escreve o realce reduzido na banda de Ar");
}

static void testRules()
{
    using namespace lapidamix;
    const auto& settings = voiceProfile.correction;
    const auto near = [] (float a, float b) { return std::abs (a - b) < 0.05f; };

    {
        const auto plan = makeCorrectionPlan ({}, settings);
        check (! plan.isAvailable && ! plan.highPass.isEnabled && ! plan.mud.isEnabled && ! plan.resonance.isEnabled && ! plan.presence.isEnabled && ! plan.air.isEnabled,
               "Regras: sem analise, nada e proposto");

        auto analysis = makeKnownAnalysis (-40.0f, -28.0f, -70.0f);
        analysis.voiceSeconds = 9.9f;
        const auto littleVoice = makeCorrectionPlan (analysis, settings);
        analysis.voiceSeconds = 10.0f;
        check (! littleVoice.isAvailable && ! littleVoice.mud.isEnabled && makeCorrectionPlan (analysis, settings).isAvailable, "Regras: abaixo de 10 s de voz, nada e proposto; com 10 s, sim (D101)");
    }

    {
        const auto plan = makeCorrectionPlan (makeKnownAnalysis (-40.0f, -40.0f, -55.0f), settings);
        check (near (plan.highPass.frequency, 70.7f) && plan.highPass.reason == CorrectionReason::applied, "Regras: passa-alta meia oitava abaixo do grave de 100 Hz: " + juce::String (plan.highPass.frequency, 1) + " Hz");
        check (! plan.mud.isEnabled && plan.mud.reason == CorrectionReason::withinRange, "Regras: embolado dentro da faixa nao age");
        check (! plan.presence.isEnabled && plan.presence.reason == CorrectionReason::withinRange, "Regras: presenca dentro da faixa nao age");
        check (! plan.air.isEnabled && plan.air.reason == CorrectionReason::withinRange, "Regras: ar dentro da faixa nao age (D108)");
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

// O nível de um seno depois de passar pelo plugin, em dB, com os parâmetros já estabilizados.
static float measureToneDb (LapidamixAudioProcessor& processor, float frequency)
{
    juce::AudioBuffer<float> block (2, kBlock);
    auto phase = 0.0;
    double sum = 0.0;
    int count = 0;

    for (int n = 0; n < 200; ++n)
    {
        for (int i = 0; i < kBlock; ++i)
        {
            phase += juce::MathConstants<double>::twoPi * frequency / kRate;
            const auto v = 0.1f * (float) std::sin (phase);
            block.setSample (0, i, v);
            block.setSample (1, i, v);
        }

        process (processor, block);

        if (n >= 100)
            for (int i = 0; i < kBlock; ++i)
            {
                sum += block.getSample (0, i) * block.getSample (0, i);
                ++count;
            }
    }

    return juce::Decibels::gainToDecibels ((float) std::sqrt (sum / count) / (0.1f / std::sqrt (2.0f)));
}

// O botão Aplicar: nada muda antes dele, e depois o som muda como o plano diz (D85, D99).
static void testApply (const juce::AudioBuffer<float>& audio)
{
    using lapidamix::EqModule;
    LapidamixAudioProcessor processor;
    prepare (processor);

    // Um ajuste à mão no Ar, que Aplicar deve substituir pela proposta (D108).
    setParameter (processor, EqModule::getEnabledParameterId (4), 1.0f);
    setParameter (processor, EqModule::getGainParameterId (4), 5.0f);

    const auto getValue = [&] (const juce::String& id) { return processor.getParameters().getRawParameterValue (id)->load(); };
    const auto near = [] (float a, float b, float tolerance) { return std::abs (a - b) <= tolerance; };

    const auto emptyPlan = processor.getCorrectionPlan();
    processor.applyCorrectionPlan (emptyPlan);
    check (! emptyPlan.isAvailable && getValue (EqModule::getEnabledParameterId (1)) < 0.5f, "Aplicar: sem analise, nada muda");

    analyseExactly (processor, audio, (int) (60.0 * kRate));
    const auto plan = processor.getCorrectionPlan();
    check (plan.isAvailable && getValue (EqModule::getEnabledParameterId (1)) < 0.5f && getValue (EqModule::getEnabledParameterId (3)) < 0.5f,
           "Aplicar: depois de analisar, as bandas continuam desligadas ate clicar");

    const auto before316 = measureToneDb (processor, 316.0f);
    processor.applyCorrectionPlan (plan);

    check (near (getValue (EqModule::getFrequencyParameterId (0)), plan.highPass.frequency, 0.5f) && getValue (EqModule::getEnabledParameterId (0)) > 0.5f
               && near (getValue (EqModule::getFrequencyParameterId (1)), plan.mud.frequency, 0.5f) && near (getValue (EqModule::getGainParameterId (1)), plan.mud.gainDb, 0.06f)
               && near (getValue (EqModule::getQParameterId (1)), plan.mud.q, 0.01f) && getValue (EqModule::getEnabledParameterId (1)) > 0.5f
               && near (getValue (EqModule::getFrequencyParameterId (3)), plan.presence.frequency, 0.5f) && near (getValue (EqModule::getGainParameterId (3)), plan.presence.gainDb, 0.06f)
               && near (getValue (EqModule::getQParameterId (3)), plan.presence.q, 0.01f) && getValue (EqModule::getEnabledParameterId (3)) > 0.5f,
           "Aplicar: os parametros recebem o plano (passa-alta " + juce::String (plan.highPass.frequency, 0) + " Hz, embolado " + juce::String (plan.mud.gainDb, 1)
               + " dB, presenca +" + juce::String (plan.presence.gainDb, 1) + " dB)");
    check (plan.air.isEnabled && plan.air.gainDb > 0.0f && getValue (EqModule::getEnabledParameterId (4)) > 0.5f && near (getValue (EqModule::getGainParameterId (4)), plan.air.gainDb, 0.06f)
               && near (getValue (EqModule::getFrequencyParameterId (4)), 12000.0f, 0.5f) && near (getValue (EqModule::getQParameterId (4)), 0.7f, 0.01f),
           "Aplicar: a banda de ar recebe a proposta: +" + juce::String (plan.air.gainDb, 1) + " dB (D108)");

    auto withoutAir = plan;
    withoutAir.air.isEnabled = false;
    check (processor.getCompensationDb (plan) < processor.getCompensationDb (withoutAir), "Aplicar: a compensacao de volume conta o realce do Ar");

    // Os tons passam também pela compensação escrita na Saída.
    const auto compensation = getValue (LapidamixAudioProcessor::outputGainParameterId);
    const auto after316 = measureToneDb (processor, (float) plan.mud.frequency) - compensation;
    const auto after3500 = measureToneDb (processor, plan.presence.frequency) - compensation;
    const auto after20 = measureToneDb (processor, 20.0f) - compensation;
    check (near (after316 - before316, plan.mud.gainDb, 1.0f), "Aplicar: o som muda como o plano diz em " + juce::String (plan.mud.frequency, 0) + " Hz: " + juce::String (after316 - before316, 1) + " dB");
    check (near (after3500, plan.presence.gainDb, 1.0f), "Aplicar: o som muda como o plano diz em 3,5 kHz: " + juce::String (after3500, 1) + " dB");
    check (after20 < -6.0f, "Aplicar: o passa-alta age em 20 Hz: " + juce::String (after20, 1) + " dB");

    // Analisar de novo não muda o som até um novo clique (D99).
    const auto gainBefore = getValue (EqModule::getGainParameterId (1));
    analyseExactly (processor, audio, (int) (40.0 * kRate));
    check (near (getValue (EqModule::getGainParameterId (1)), gainBefore, 0.001f), "Aplicar: analisar de novo nao muda as bandas");

    // Um plano novo apaga o anterior nas bandas que não agem mais.
    auto withoutMud = plan;
    withoutMud.mud.isEnabled = false;
    withoutMud.mud.gainDb = 0.0f;
    processor.applyCorrectionPlan (withoutMud);
    check (getValue (EqModule::getEnabledParameterId (1)) < 0.5f && near (getValue (EqModule::getGainParameterId (1)), 0.0f, 0.06f), "Aplicar: banda que nao age mais desliga e volta a 0 dB");

    // Depois de aplicar, o projeto guarda as bandas.
    processor.applyCorrectionPlan (plan);
    juce::MemoryBlock state;
    processor.getStateInformation (state);
    LapidamixAudioProcessor restored;
    restored.setStateInformation (state.getData(), (int) state.getSize());
    const auto restoredGain = restored.getParameters().getRawParameterValue (EqModule::getGainParameterId (1))->load();
    check (near (restoredGain, plan.mud.gainDb, 0.06f), "Aplicar: as bandas aplicadas voltam ao reabrir o projeto");
}

static lapidamix::AnalysisResult analyseMono (const std::vector<float>& mono)
{
    lapidamix::VoiceAnalyzer::Settings settings;
    settings.sampleRate = kRate;
    lapidamix::VoiceAnalyzer analyzer (settings);
    return analyzer.finish (mono.data(), (int) mono.size(), nullptr);
}

// A BS.1770 só publica os coeficientes de 48 kHz: refaz a transformação bilinear para outra taxa.
static lapidamix::loudness::Biquad retuneBiquad (const lapidamix::loudness::Biquad& f, double sampleRate)
{
    const auto scale = 1.0 - f.a1 + f.a2;
    const auto squaredK = (1.0 + f.a1 + f.a2) / scale;
    const auto kOverQ = 2.0 * (1.0 - f.a2) / scale;
    const auto d = 4.0 / scale;

    const auto k = std::sqrt (squaredK);
    const auto newK = std::tan (std::atan (k) * lapidamix::loudness::kWeightingSampleRate / sampleRate);
    const auto low = d * (f.b0 + f.b1 + f.b2) / (4.0 * squaredK);
    const auto high = d * (f.b0 - f.b1 + f.b2) / 4.0;
    const auto middle = (f.b0 - f.b2) * d / 2.0 * newK / k;
    const auto newKOverQ = kOverQ * newK / k;
    const auto newSquaredK = newK * newK;
    const auto newD = 1.0 + newKOverQ + newSquaredK;

    return { (high + middle + low * newSquaredK) / newD,
             2.0 * (low * newSquaredK - high) / newD,
             (high - middle + low * newSquaredK) / newD,
             2.0 * (newSquaredK - 1.0) / newD,
             (1.0 - newKOverQ + newSquaredK) / newD };
}

// LUFS integrado completo (ITU-R BS.1770): ponderação K, blocos de 400 ms e os dois portões.
// Medidor independente do plugin, que só compara curvas de espectro (D103).
static double measureLufs (const juce::AudioBuffer<float>& audio, double sampleRate)
{
    using namespace lapidamix::loudness;
    const auto shelf = retuneBiquad (kWeightingShelf, sampleRate);
    const auto highPass = retuneBiquad (kWeightingHighPass, sampleRate);

    const auto blockSize = (int) std::round (0.4 * sampleRate);
    const auto hopSize = blockSize / 4;
    const auto numSamples = audio.getNumSamples();
    std::vector<double> squared ((size_t) numSamples, 0.0);

    for (int channel = 0; channel < audio.getNumChannels(); ++channel)
    {
        const auto* input = audio.getReadPointer (channel);
        double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0, z1 = 0.0, z2 = 0.0;

        for (int i = 0; i < numSamples; ++i)
        {
            const auto x = (double) input[i];
            const auto y = shelf.b0 * x + shelf.b1 * x1 + shelf.b2 * x2 - shelf.a1 * y1 - shelf.a2 * y2;
            const auto z = highPass.b0 * y + highPass.b1 * y1 + highPass.b2 * y2 - highPass.a1 * z1 - highPass.a2 * z2;
            x2 = x1; x1 = x; y2 = y1; y1 = y; z2 = z1; z1 = z;
            squared[(size_t) i] += z * z;
        }
    }

    std::vector<double> blocks;
    auto running = 0.0;

    for (int i = 0; i < numSamples; ++i)
    {
        running += squared[(size_t) i];
        if (i >= blockSize) running -= squared[(size_t) (i - blockSize)];
        if (i >= blockSize - 1 && (i - blockSize + 1) % hopSize == 0) blocks.push_back (running / blockSize);
    }

    const auto toLufs = [] (double power) { return -0.691 + 10.0 * std::log10 (power + 1.0e-30); };
    const auto gatedMean = [&] (double threshold)
    {
        auto sum = 0.0;
        auto count = 0;
        for (const auto block : blocks)
            if (toLufs (block) > threshold) { sum += block; ++count; }
        return count > 0 ? sum / count : 0.0;
    };

    return toLufs (gatedMean (toLufs (gatedMean (-70.0)) - 10.0));
}

// O próprio medidor conferido com sinais de resultado conhecido (EBU Tech 3341, casos 1, 3, 4 e 5).
static void testLufsMeter()
{
    const auto makeTones = [] (std::initializer_list<std::pair<double, double>> parts, double sampleRate)
    {
        auto total = 0.0;
        for (const auto& part : parts) total += part.second;

        juce::AudioBuffer<float> tones (2, (int) (total * sampleRate));
        auto position = 0;

        for (const auto& part : parts)
        {
            const auto gain = std::pow (10.0, part.first / 20.0);
            const auto length = (int) (part.second * sampleRate);

            for (int i = 0; i < length && position + i < tones.getNumSamples(); ++i)
            {
                const auto value = (float) (gain * std::sin (juce::MathConstants<double>::twoPi * 1000.0 * i / sampleRate));
                tones.setSample (0, position + i, value);
                tones.setSample (1, position + i, value);
            }

            position += length;
        }

        return tones;
    };

    const auto report = [&] (const char* name, std::initializer_list<std::pair<double, double>> parts)
    {
        for (const auto sampleRate : { 44100.0, 48000.0 })
        {
            const auto lufs = measureLufs (makeTones (parts, sampleRate), sampleRate);
            check (std::abs (lufs + 23.0) <= 0.1, juce::String ("Medidor de LUFS, ") + name + " a " + juce::String ((int) sampleRate) + " Hz: " + juce::String (lufs, 2) + " (esperado -23,0)");
        }
    };

    report ("caso 1", { { -23.0, 20.0 } });
    report ("caso 3", { { -36.0, 10.0 }, { -23.0, 60.0 }, { -36.0, 10.0 } });
    report ("caso 4", { { -72.0, 10.0 }, { -36.0, 10.0 }, { -23.0, 60.0 }, { -36.0, 10.0 }, { -72.0, 10.0 } });
    report ("caso 5", { { -26.0, 20.0 }, { -20.0, 20.1 }, { -26.0, 20.0 } });
}

// Intensidade e compensação de volume (D98, D100, D102).
static void testIntensityAndVolume (const juce::AudioBuffer<float>& audio, const juce::String& name)
{
    using namespace lapidamix;
    const auto& settings = voiceProfile.correction;
    const auto near = [] (float a, float b, float tolerance) { return std::abs (a - b) <= tolerance; };

    {
        CorrectionPlan plan;
        plan.isAvailable = true;
        plan.highPass.isEnabled = true;
        plan.highPass.frequency = 70.0f;
        plan.mud.isEnabled = true;
        plan.mud.gainDb = -5.0f;
        plan.presence.isEnabled = true;
        plan.presence.gainDb = 3.0f;

        const auto half = scaleCorrectionPlan (plan, 0.5f, settings);
        check (near (half.highPass.frequency, 52.9f, 0.1f) && near (half.mud.gainDb, -2.5f, 0.01f) && near (half.presence.gainDb, 1.5f, 0.01f), "Intensidade 50%: passa-alta 52,9 Hz, ganhos pela metade (D98, D100)");

        const auto full = scaleCorrectionPlan (plan, 1.0f, settings);
        check (near (full.highPass.frequency, 70.0f, 0.01f) && near (full.mud.gainDb, -5.0f, 0.01f), "Intensidade 100%: plano inteiro");

        const auto off = scaleCorrectionPlan (plan, 0.0f, settings);
        check (! off.highPass.isEnabled && ! off.mud.isEnabled && ! off.presence.isEnabled && near (off.mud.gainDb, 0.0f, 0.001f), "Intensidade 0%: todas as bandas desligadas (D100)");
    }

    LapidamixAudioProcessor processor;
    prepare (processor);
    const auto getValue = [&] (const juce::String& id) { return processor.getParameters().getRawParameterValue (id)->load(); };
    const auto outputId = juce::String (LapidamixAudioProcessor::outputGainParameterId);
    const auto intensityId = juce::String (LapidamixAudioProcessor::intensityParameterId);

    check (! processor.getParameters().getParameter (intensityId)->isAutomatable(), "Intensidade nao e automatizavel (P57)");

    analyseExactly (processor, audio, (int) (60.0 * kRate));

    // Mover a intensidade antes de aplicar não muda o som.
    setParameter (processor, intensityId, 50.0f);
    processor.handleIntensityChange();
    check (getValue (EqModule::getEnabledParameterId (1)) < 0.5f && near (getValue (outputId), 0.0f, 0.001f), "Intensidade antes de aplicar: nada muda");
    setParameter (processor, intensityId, 100.0f);

    processor.applyCurrentCorrection();
    const auto full = getValue (outputId);
    check (full > 0.0f && full < 6.0f, "Compensacao fixa na Saida, a 100%: " + juce::String (full, 1) + " dB");

    // O volume em LUFS, com o plugin de verdade, na gravação inteira, antes e depois (D103).
    const auto numSamples = audio.getNumSamples() - audio.getNumSamples() % kBlock;
    juce::AudioBuffer<float> input (2, numSamples), output (2, numSamples);
    juce::AudioBuffer<float> block (2, kBlock);
    auto outputChanged = false;

    for (int start = 0; start < numSamples; start += kBlock)
    {
        for (int channel = 0; channel < 2; ++channel)
        {
            block.copyFrom (channel, 0, audio, channel, start, kBlock);
            input.copyFrom (channel, start, block, channel, 0, kBlock);
        }

        process (processor, block);

        for (int channel = 0; channel < 2; ++channel)
            output.copyFrom (channel, start, block, channel, 0, kBlock);

        if (! juce::approximatelyEqual (getValue (outputId), full)) outputChanged = true;
    }

    check (! outputChanged, "Compensacao nao muda durante a reproducao (D102)");

    const auto lufsIn = measureLufs (input, kRate);
    const auto lufsOut = measureLufs (output, kRate);
    check (std::abs (lufsOut - lufsIn) <= 0.5, name + ": LUFS integrado na gravacao inteira, " + juce::String (lufsIn, 2) + " antes e " + juce::String (lufsOut, 2) + " depois (Saida " + juce::String (full, 1) + " dB)");

    // A intensidade, depois de aplicar, leva a compensação junto.
    setParameter (processor, intensityId, 50.0f);
    processor.handleIntensityChange();
    const auto half = getValue (outputId);
    check (near (getValue (EqModule::getGainParameterId (1)), -2.5f, 0.06f) && near (getValue (EqModule::getFrequencyParameterId (0)), 40.0f * std::sqrt (processor.getCorrectionPlan().highPass.frequency > 0.0f ? 71.17f / 40.0f : 1.0f), 1.0f)
               && half > 0.0f && half < full,
           "Intensidade 50% depois de aplicar: bandas e Saida acompanham (" + juce::String (half, 1) + " dB)");

    setParameter (processor, intensityId, 0.0f);
    processor.handleIntensityChange();
    check (getValue (EqModule::getEnabledParameterId (0)) < 0.5f && getValue (EqModule::getEnabledParameterId (1)) < 0.5f && near (getValue (outputId), 0.0f, 0.051f), "Intensidade 0% depois de aplicar: bandas desligadas e Saida em 0 dB");

    setParameter (processor, intensityId, 100.0f);
    processor.handleIntensityChange();
    check (near (getValue (outputId), full, 0.06f), "Intensidade de volta a 100%: mesma compensacao");

    // Ajuste à mão fica, e o projeto reabre sem refazer nada.
    setParameter (processor, outputId, 1.0f);
    processor.handleIntensityChange();
    check (near (getValue (outputId), 1.0f, 0.06f), "Saida ajustada a mao continua ate a intensidade mudar");

    juce::MemoryBlock state;
    processor.getStateInformation (state);
    LapidamixAudioProcessor restored;
    prepare (restored);
    restored.setStateInformation (state.getData(), (int) state.getSize());
    restored.handleIntensityChange();
    const auto restoredOutput = restored.getParameters().getRawParameterValue (outputId)->load();
    check (near (restoredOutput, 1.0f, 0.06f), "Ao reabrir o projeto, o ajuste a mao continua");
    setParameter (restored, intensityId, 50.0f);
    restored.handleIntensityChange();
    check (near (restored.getParameters().getRawParameterValue (EqModule::getGainParameterId (1))->load(), -2.5f, 0.06f), "Depois de reabrir, a intensidade continua agindo");

    // Análise nova não aplicada: mover a intensidade só muda a proposta (P57).
    const auto gainBefore = getValue (EqModule::getGainParameterId (1));
    analyseExactly (processor, audio, (int) (40.0 * kRate));
    setParameter (processor, intensityId, 30.0f);
    processor.handleIntensityChange();
    check (near (getValue (EqModule::getGainParameterId (1)), gainBefore, 0.001f), "Analise nova nao aplicada: mover a intensidade nao muda o som");
}

// Explicações de cada banda e dicionário embutido (D92).
static void testExplanations (const juce::AudioBuffer<float>& audio)
{
    using namespace lapidamix;
    const auto& profile = voiceProfile;
    const auto dictionary = loadRegionDictionary (profile);
    const auto utf8 = [] (const char* text) { return juce::String::fromUTF8 (text); };

    auto regionsAreComplete = dictionary.regions.size() == 8;
    auto previousHigh = dictionary.regions.empty() ? 0.0f : dictionary.regions.front().lowFrequency;

    for (const auto& region : dictionary.regions)
    {
        regionsAreComplete = regionsAreComplete && region.name.isNotEmpty() && region.boostEffect.startsWith ("Tende a") && region.cutEffect.startsWith ("Tende a")
                          && juce::approximatelyEqual (region.lowFrequency, previousHigh);
        previousHigh = region.highFrequency;
    }

    check (regionsAreComplete, "Dicionario embutido: 8 regioes em sequencia, com os textos como tendencia (D92)");

    const auto regionName = [&] (float frequency)
    {
        const auto* region = dictionary.findRegion (frequency);
        return region != nullptr ? region->name : juce::String();
    };

    check (regionName (35.0f) == "Ronco" && regionName (316.0f) == "Embolado" && regionName (3500.0f) == utf8 ("Presença") && regionName (12000.0f) == "Ar",
           "Dicionario: cada frequencia acha a regiao certa, com acentos");

    AnalysisResult analysis;
    analysis.fundamentalLow = 99.0f;
    analysis.fundamentalHigh = 153.0f;
    analysis.confidence = 0.7f;
    analysis.octaveJumpShare = 0.12f;

    CorrectionPlan plan;
    plan.isAvailable = true;
    plan.highPass = { true, 70.0f, 0.0f, 0.0f, 0.0f, CorrectionReason::applied };
    plan.mud = { true, 316.0f, -5.0f, 1.0f, 8.0f, CorrectionReason::applied };
    plan.resonance = { true, 1000.0f, -2.0f, 4.0f, 6.0f, CorrectionReason::applied };
    plan.presence = { true, 3500.0f, 3.0f, 0.7f, -22.0f, CorrectionReason::applied };
    plan.air = { true, 12000.0f, 1.6f, 0.7f, -55.6f, CorrectionReason::applied };

    const auto acting = explainCorrectionPlan (plan, analysis, 1.7f, profile, dictionary);
    check (juce::StringArray::fromLines (acting).size() > 0 && acting.contains ("Filtrar abaixo de 70 Hz.") && acting.contains ("Atenuar 5,0 dB em 316 Hz, Q 1,0.")
               && acting.contains (utf8 ("Realçar 3,0 dB em 3500 Hz")) && acting.contains ("Somar 1,7 dB."),
           "Explicacao: o que sera feito, com a mesma frase antes e depois do Aplicar");
    check (! acting.contains ("Atenuei") && ! acting.contains (utf8 ("Realcei")), "Explicacao: sem verbo no passado (D92, regra 1)");
    check (acting.contains (utf8 ("Realçar 1,6 dB em 12000 Hz")) && acting.contains (utf8 ("região de 10 kHz a 14 kHz está 55,6 dB abaixo")) && acting.contains (dictionary.findRegion (12000.0f)->boostEffect),
           "Explicacao: banda de ar diz o que faz, por que e o efeito (D108)");
    check (acting.contains (utf8 ("está 8,0 dB acima da região média da voz (500 Hz a 2 kHz)")) && acting.contains (utf8 ("região de 2 kHz a 5 kHz está 22,0 dB abaixo")),
           "Explicacao: compara com a regiao media em linguagem simples");

    auto hasJargon = false;

    for (const auto* word : { "contra os m", "(5%)", "(95%)", "fundamental", "pondera", "plosivos", "manuseio", "inconsistente", "entorno", "timbre", "conservadora" })
        hasJargon = hasJargon || acting.containsIgnoreCase (juce::String (word));

    check (! hasJargon, "Explicacao: sem os termos tecnicos da versao anterior");

    const auto count = [] (const juce::String& text, const juce::String& word)
    {
        auto total = 0;
        for (auto index = text.indexOf (word); index >= 0; index = text.indexOf (index + 1, word)) ++total;
        return total;
    };

    check (count (acting, utf8 ("Por quê: ")) == 6 && count (acting, "Efeito: ") == 6, "Explicacao: as 6 linhas da lista tem o por que e o efeito");
    check (acting.contains (dictionary.findRegion (316.0f)->cutEffect) && acting.contains (dictionary.findRegion (3500.0f)->boostEffect) && acting.contains (dictionary.findRegion (35.0f)->cutEffect),
           "Explicacao: o efeito vem do dicionario, inclusive o do passa-alta");

    auto idle = plan;
    idle.highPass.frequency = 40.0f;
    idle.highPass.reason = CorrectionReason::inconsistentFundamental;
    idle.mud = { false, 316.0f, 0.0f, 1.0f, 1.0f, CorrectionReason::withinRange };
    idle.resonance = { false, 0.0f, 0.0f, 4.0f, 0.0f, CorrectionReason::noEligiblePeak };
    idle.presence = { false, 3500.0f, 0.0f, 0.7f, -15.0f, CorrectionReason::withinRange };
    idle.air = { false, 12000.0f, 0.0f, 0.7f, -45.0f, CorrectionReason::withinRange };

    const auto notActing = explainCorrectionPlan (idle, analysis, 0.0f, profile, dictionary);
    check (count (notActing, utf8 ("Não mexer.")) == 4 && notActing.contains ("1,0 dB acima") && notActing.contains ("15,0 dB abaixo") && notActing.contains (utf8 ("Nenhuma frequência")),
           "Explicacao: tambem diz por que a banda nao age, com o valor medido (D92, regra 3)");
    check (notActing.contains ("Filtrar abaixo de 40 Hz.") && notActing.contains (utf8 ("em dúvida")) && notActing.contains ("70%") && notActing.contains ("12%"),
           "Explicacao: passa-alta conservador mostra a medida que causou a escolha");
    check (notActing.contains ("Manter em 0 dB."), "Explicacao: sem correcoes, a Saida fica em 0 dB");

    auto highVoice = idle;
    highVoice.mud = { false, 0.0f, 0.0f, 1.0f, 0.0f, CorrectionReason::protectedFundamental };
    check (explainCorrectionPlan (highVoice, analysis, 0.0f, profile, dictionary).contains (utf8 ("atingiria as notas cantadas")), "Explicacao: regiao protegida pela fundamental");

    const auto off = explainCorrectionPlan (scaleCorrectionPlan (plan, 0.0f, profile.correction), analysis, 0.0f, profile, dictionary);
    check (off.contains (utf8 ("Não filtrar: intensidade em 0%.")) && count (off, utf8 ("Não mexer: intensidade em 0%.")) == 4, "Explicacao: intensidade em 0%");

    check (explainCorrectionPlan (plan, analysis, -0.8f, profile, dictionary).contains ("Reduzir 0,8 dB."), "Explicacao: compensacao negativa reduz a Saida");

    CorrectionPlan unavailable;
    check (explainCorrectionPlan (unavailable, analysis, 0.0f, profile, dictionary).isEmpty(), "Explicacao: pouca voz nao mostra lista (D101)");

    // O texto de verdade, com a voz do usuário, para leitura.
    LapidamixAudioProcessor processor;
    prepare (processor);
    analyseExactly (processor, audio, (int) (60.0 * kRate));
    const auto realPlan = processor.getCorrectionPlan();
    printf ("\n%s\n\n", explainCorrectionPlan (realPlan, processor.getLastAnalysisResult(), processor.getCompensationDb (realPlan), profile, dictionary).toRawUTF8());
}

// A análise de verdade calculando os dados novos, com sinais de resposta conhecida.
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

    // Ruído nas pausas (D108): canto contínuo não tem pausa; com pausas de ruído, a curva aparece.
    check (steady.noiseCurve.empty(), "Analise: sem pausa, o ruido nao e medido, mesmo com um tom constante");

    auto withPauses = makeTone (false);
    juce::Random random (99);
    for (size_t i = 0; i < withPauses.size(); ++i)
        if (((int) ((double) i / kRate) % 4) == 3)
            withPauses[i] = 1.0e-4f * (2.0f * random.nextFloat() - 1.0f);

    const auto paused = analyseMono (withPauses);
    check ((int) paused.noiseCurve.size() == spectrum::numDisplayPoints, "Analise: com pausas, o ruido e medido");

    const auto restoredNoise = AnalysisResult::fromValueTree (paused.toValueTree());
    auto oldProject = paused.toValueTree();
    oldProject.removeProperty ("noiseCurve", nullptr);
    check (restoredNoise.noiseCurve.size() == paused.noiseCurve.size() && AnalysisResult::fromValueTree (oldProject).noiseCurve.empty(), "Analise: o ruido volta ao reabrir; projeto antigo fica sem pausa medida");

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
    testAirRules();

    const auto sm7 = loadStereo ("VOZ-CANTADA-SHURE-SM7DB.wav");
    const auto sm57 = loadStereo ("VOZ-CANTADA-SHURE-SM57.wav");

    testAnalysisData (sm7);
    testAirHissWithRealVoice (sm7);
    testApply (sm7);
    testExplanations (sm7);
    testLufsMeter();
    testIntensityAndVolume (sm7, "SM7dB");
    testIntensityAndVolume (sm57, "SM57");
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
