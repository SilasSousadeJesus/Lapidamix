// Passa uma gravação pelo plugin, sem DAW, e salva uma imagem da janela (D84).
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cstdio>

#include "plugin/PluginProcessor.h"

int main (int argc, char** argv)
{
    if (argc < 3)
    {
        printf ("Uso: captura <gravacao.wav> <imagem.png>\n");
        return 1;
    }

    const juce::ScopedJuceInitialiser_GUI gui;

    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatReader> reader (format.createReaderFor (new juce::FileInputStream (juce::File (juce::String::fromUTF8 (argv[1]))), true));

    if (reader == nullptr)
    {
        printf ("Nao leu a gravacao\n");
        return 1;
    }

    constexpr int blockSize = 512;
    LapidamixAudioProcessor processor;
    processor.setPlayConfigDetails (2, 2, reader->sampleRate, blockSize);
    processor.prepareToPlay (reader->sampleRate, blockSize);
    processor.startAnalysis();

    juce::AudioBuffer<float> buffer (2, blockSize);
    juce::MidiBuffer midi;
    juce::int64 position = 0;

    const auto playBlock = [&]
    {
        reader->read (&buffer, 0, blockSize, position, true, true);
        position = (position + blockSize) % juce::jmax ((juce::int64) blockSize, reader->lengthInSamples - blockSize);
        processor.processBlock (buffer, midi);
    };

    // Dez vezes mais rápido que o tempo real: a thread da análise lê a captura a cada 50 ms.
    using State = lapidamix::AnalysisController::State;

    while (processor.getAnalysis().getState() == State::capturing)
    {
        playBlock();
        juce::Thread::sleep (1);
    }

    for (int attempt = 0; attempt < 300 && processor.getResultVersion() == 0; ++attempt)
        juce::MessageManager::getInstance()->runDispatchLoopUntil (100);

    std::unique_ptr<juce::AudioProcessorEditor> editor (processor.createEditor());

    for (int block = 0; block < 200; ++block)
    {
        playBlock();
        juce::MessageManager::getInstance()->runDispatchLoopUntil (5);
    }

    const auto image = editor->createComponentSnapshot (editor->getLocalBounds());
    juce::File output (juce::String::fromUTF8 (argv[2]));
    output.deleteFile();
    juce::FileOutputStream stream (output);
    juce::PNGImageFormat png;
    png.writeImageToStream (image, stream);

    const auto result = processor.getLastAnalysisResult();
    printf ("voz %.1f s, captura %.1f s, mediana %.1f, p5 %.1f, p95 %.1f\n",
            result.voiceSeconds, result.capturedSeconds, result.fundamentalMedian, result.fundamentalLow, result.fundamentalHigh);

    editor.reset();
    processor.releaseResources();
    return 0;
}
