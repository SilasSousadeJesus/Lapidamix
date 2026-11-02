# 09 - Glossário

Termos técnicos usados no projeto, em linguagem simples.

## Áudio e mixagem
- **Ar** — agudos bem altos (acima de ~10 kHz) que dão sensação de abertura e brilho à voz.
- **Bypass** — desliga o efeito temporariamente para comparar antes e depois.
- **Cadeia de sinal** — a sequência de processadores por onde o som passa (ex.: EQ, depois compressor, depois de-esser).
- **DSP (processamento digital de sinais)** — a matemática usada para analisar e modificar o som no computador.
- **Efeito de proximidade** — microfones direcionais (como o SM7dB) acentuam os graves quando se canta perto deles. Causa comum de voz escura e embolada.
- **Embolado** — excesso de energia nos graves e médio-graves que deixa a voz confusa e sem definição.
- **EQ (equalizador)** — aumenta ou reduz o volume de faixas de frequência.
- **EQ corretivo** — EQ usado para resolver problemas, não para criar um som estilizado.
- **EQ estático** — a correção é fixa durante toda a música.
- **EQ dinâmico** — a correção só age quando o problema aparece.
- **Espectro** — o "mapa" de quanta energia o som tem em cada frequência.
- **Fundamental** — a frequência mais grave de uma nota cantada; define o "peso" da voz. Cortá-la deixa a voz fina.
- **Passa-alta** — filtro que remove os graves abaixo de certa frequência.
- **Presença** — região dos médio-agudos (aprox. 2–5 kHz) responsável pela clareza e inteligibilidade.
- **Q** — largura de uma banda de EQ. Q baixo = banda larga e suave (soa natural). Q alto = banda estreita e cirúrgica.
- **Ressonância** — pico de energia numa frequência específica que se destaca e incomoda.
- **RTA (analisador em tempo real)** — gráfico que mostra o espectro do som enquanto ele toca.
- **Voz seca** — gravação sem nenhum efeito aplicado.

## Plugins e software
- **Lapidamix** — nome deste projeto: lapidar + mix, revelar o brilho que já existe no som, sem destruir o timbre (D45).
- **DAW** — programa de gravação e mixagem (Reaper, Pro Tools etc.).
- **Plugin de insert** — efeito colocado diretamente numa trilha, processando todo o áudio dela.
- **VST3 / AU / AAX** — formatos de plugin. VST3: maioria dos DAWs no Windows. AU: Mac. AAX: Pro Tools.
- **JUCE** — framework em C++ muito usado para criar plugins de áudio.
- **Parâmetro** — cada controle do plugin (ganho, frequência…), que o DAW pode salvar e automatizar.
- **Núcleo comum** — a parte do programa compartilhada por todos os instrumentos (D30).
- **Perfil de instrumento** — o conjunto de informações específicas de um instrumento, usado pelo núcleo comum (D30).
- **Módulo encaixável** — um processador construído para ser adicionado sem mexer no resto do programa (D30).
- **Código aberto / licença** — código publicado para qualquer um usar; a licença define as regras de uso.
- **AGPLv3** — licença de código aberto, a opção aberta do JUCE e a licença deste projeto (D42). Em termos simples: exige que o código de quem a usa também fique disponível sob ela.
- **Git** — ferramenta que guarda o histórico de todas as mudanças do projeto e permite voltar atrás.
- **Commit** — um "ponto salvo" no Git, com uma mensagem dizendo o que mudou.
- **CLAUDE.md** — arquivo na raiz do projeto que o Claude Code lê sempre que abre a pasta; contém as regras de trabalho.

## Tempo real
- **Thread de áudio** — a parte do programa que processa o som, em pedaços pequenos, centenas de vezes por segundo.
- **processBlock()** — a função do plugin que processa cada pedaço de áudio.
- **prepareToPlay()** — função chamada antes de o áudio começar; é onde se prepara tudo (inclusive memória) com antecedência.
- **Alocação de memória** — pedir memória nova ao sistema operacional; pode demorar e por isso é proibida dentro do `processBlock()`.
- **Regra de ouro do tempo real** — nada no `processBlock()` pode travar: sem prints, sem pedir memória nova, sem acessar disco. Se travar, o pedaço chega atrasado e ouvem-se estalos.
