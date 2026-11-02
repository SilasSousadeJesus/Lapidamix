# 09 - Glossário

Termos técnicos usados no projeto, em linguagem simples.

## Áudio e mixagem
- **Ar** — agudos bem altos (acima de ~10 kHz) que dão sensação de abertura e brilho à voz.
- **Bypass** — desliga o efeito temporariamente para comparar antes e depois.
- **Biquad** — tipo comum de filtro digital, leve e sem latência, usado para construir cada banda de um EQ.
- **Sino (bell)** — banda de EQ que aumenta ou reduz uma região em torno de uma frequência central.
- **Prateleira (shelf)** — banda de EQ que aumenta ou reduz tudo acima (alta) ou abaixo (baixa) de uma frequência.
- **LUFS** — unidade padrão de volume percebido (norma ITU-R BS.1770). Uma diferença de 1 LU equivale a 1 dB. O LUFS integrado mede o volume médio de toda a gravação, ignorando os trechos muito baixos.
- **Ponderação K** — o peso que o LUFS dá a cada frequência: reduz os graves muito baixos e reforça um pouco os agudos, para se aproximar do ouvido.
- **Latência** — atraso entre o som entrar e sair do plugin.
- **Fase linear** — modo de EQ que preserva a fase do som, mas cria latência.
- **Oversampling** — processar internamente em uma taxa de amostragem mais alta; gasta mais processamento.
- **Taxa de amostragem** — quantas vezes por segundo o som é medido no áudio digital (ex.: 44,1 kHz). Limita as frequências que ele consegue representar a cerca da metade desse valor.
- **Suavização de parâmetro** — mudar um valor aos poucos, em vez de num salto, para evitar estalos.
- **Ruído rosa** — ruído com energia distribuída de forma equilibrada entre as oitavas; útil para enxergar a curva de um EQ.
- **Cadeia de sinal** — a sequência de processadores por onde o som passa (ex.: EQ, depois compressor, depois de-esser).
- **DSP (processamento digital de sinais)** — a matemática usada para analisar e modificar o som no computador.
- **Efeito de proximidade** — microfones direcionais (como o SM7dB) acentuam os graves quando se canta perto deles. Causa comum de voz escura e embolada.
- **Embolado** — excesso de energia nos graves e médio-graves que deixa a voz confusa e sem definição.
- **EQ (equalizador)** — aumenta ou reduz o volume de faixas de frequência.
- **EQ corretivo** — EQ usado para resolver problemas, não para criar um som estilizado.
- **EQ estático** — a correção é fixa durante toda a música.
- **EQ dinâmico** — a correção só age quando o problema aparece.
- **Espectro médio (LTAS)** — média do espectro ao longo de um trecho; mostra o que permanece no som e perde o que muda rapidamente.
- **FFT** — cálculo que transforma um pedaço de áudio no seu espectro.
- **Formante** — pico largo de energia causado pelo formato do trato vocal; muda com a vogal.
- **Formante do cantor** — concentração de energia, em geral perto de 2,5 a 3,5 kHz, que ajuda vozes treinadas a se destacar. É uma qualidade da voz, não um defeito.
- **Harmônicos** — frequências múltiplas da fundamental que acompanham cada nota.
- **Janela (em análise de áudio)** — curva aplicada a um pedaço de áudio antes da FFT, para reduzir erros nas bordas.
- **Percentil** — valor abaixo do qual fica certa parte das medidas. O percentil de 5% da fundamental indica as notas mais graves, ignorando os 5% mais baixos; o de 50% é a mediana, o valor do meio.
- **RMS** — forma de resumir várias diferenças num só número, sem que as positivas e as negativas se anulem.
- **Periodicidade** — o quanto um som se repete com regularidade: a voz cantada tem muita; respiração e ruído, pouca.
- **Planicidade espectral** — medida de quanto um som se parece com ruído (perto de 1) ou com um tom (perto de 0).
- **Proeminência** — quanto um pico se destaca da própria vizinhança no espectro.
- **YIN** — método publicado para detectar a fundamental, baseado na repetição do sinal.
- **Espectro** — o "mapa" de quanta energia o som tem em cada frequência.
- **Fundamental** — a frequência mais grave de uma nota cantada; define o "peso" da voz. Cortá-la deixa a voz fina.
- **Passa-alta** — filtro que remove os graves abaixo de certa frequência.
- **Presença** — região dos médio-agudos (aprox. 2–5 kHz) responsável pela clareza e inteligibilidade.
- **Q** — largura de uma banda de EQ. Q baixo = banda larga e suave (soa natural). Q alto = banda estreita e cirúrgica.
- **Ressonância** — pico de energia numa frequência específica que se destaca e incomoda.
- **RTA (analisador em tempo real)** — gráfico que mostra o espectro do som enquanto ele toca.
- **Teste de cancelamento (null test)** — tocar o áudio original junto com uma cópia de polaridade invertida. Se as duas forem idênticas, elas se anulam e o resultado é silêncio; assim se prova que um plugin não alterou o som.
- **Voz seca** — gravação sem nenhum efeito aplicado.

## Plugins e software
- **Lapidamix** — nome deste projeto: lapidar + mix, revelar o brilho que já existe no som, sem destruir o timbre (D45).
- **DAW** — programa de gravação e mixagem (Reaper, Pro Tools etc.).
- **Plugin de insert** — efeito colocado diretamente numa trilha, processando todo o áudio dela.
- **VST3 / AU / AAX** — formatos de plugin. VST3: maioria dos DAWs no Windows. AU: Mac. AAX: Pro Tools.
- **Compilador** — programa que transforma o código em um arquivo que o computador executa. No Windows, usaremos o MSVC, da Microsoft.
- **CMake** — ferramenta que descreve, em um arquivo de texto, como compilar o projeto, e prepara essa compilação.
- **Windows SDK** — conjunto de arquivos da Microsoft necessário para criar programas para o Windows.
- **Submódulo Git** — forma de um repositório apontar para uma versão exata de outro repositório, sem copiar o código dele.
- **JUCE** — framework em C++ muito usado para criar plugins de áudio.
- **Parâmetro** — cada controle do plugin (ganho, frequência…), que o DAW pode salvar e automatizar.
- **Núcleo comum** — a parte do programa compartilhada por todos os instrumentos (D30).
- **Perfil de instrumento** — o conjunto de informações específicas de um instrumento, usado pelo núcleo comum (D30).
- **Módulo encaixável** — um processador construído para ser adicionado sem mexer no resto do programa (D30).
- **Código aberto / licença** — código publicado para qualquer um usar; a licença define as regras de uso.
- **AGPLv3** — licença de código aberto, a opção aberta do JUCE e a licença deste projeto (D42). Em termos simples: exige que o código de quem a usa também fique disponível sob ela.
- **Git** — ferramenta que guarda o histórico de todas as mudanças do projeto e permite voltar atrás.
- **Commit** — um "ponto salvo" no Git, com uma mensagem dizendo o que mudou.
- **AGENTS.md** — arquivo na raiz do projeto com as regras de trabalho para as IAs (D56).
- **CLAUDE.md** — arquivo na raiz do projeto que o Claude Code lê sempre que abre a pasta; aqui, ele só importa o `AGENTS.md`.

## Tempo real
- **Thread de áudio** — a parte do programa que processa o som, em pedaços pequenos, centenas de vezes por segundo.
- **processBlock()** — a função do plugin que processa cada pedaço de áudio.
- **prepareToPlay()** — função chamada antes de o áudio começar; é onde se prepara tudo (inclusive memória) com antecedência.
- **Alocação de memória** — pedir memória nova ao sistema operacional; pode demorar e por isso é proibida dentro do `processBlock()`.
- **Denormais** — números minúsculos que os filtros geram quando o som vai sumindo. Podem deixar o processador muito lento; `juce::ScopedNoDenormals` protege contra isso.
- **Regra de ouro do tempo real** — nada no `processBlock()` pode travar: sem prints, sem pedir memória nova, sem acessar disco. Se travar, o pedaço chega atrasado e ouvem-se estalos.
