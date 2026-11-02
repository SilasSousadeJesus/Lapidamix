# 07 - Referências

Projetos e materiais usados como inspiração. Estudar documentação pública e princípios é permitido. Não copiamos código de plugins de referência (produtos fechados), interface visual nem presets, não usamos nomes de marcas no nosso produto e não desmontamos plugins originais (D37).

Código aberto de terceiros (bibliotecas, exemplos) pode ser usado se a licença for compatível com a AGPLv3 (D42), com crédito ao autor. Cada caso precisa da aprovação do usuário e de registro em `02-decisoes.md` (D39).

## Plugins comerciais (referência de funcionalidade e uso)

| Referência | O que estudar | Relevância |
|---|---|---|
| iZotope Nectar (Vocal Assistant) | Fluxo "aperta, escuta e ajusta" para voz | v1 — experiência de uso |
| Sonible smart:EQ | Aprende a trilha, compara com um perfil-alvo, corrige suave com controle de intensidade | v1 — lógica mais parecida com a nossa |
| iZotope Neutron (Track/Mix Assistant) | Assistente por trilha e conflitos entre trilhas | Futuro — mix em contexto |
| Oeksound Soothe2 | Controle dinâmico de ressonâncias | Futuro — EQ dinâmico |

## Código aberto e ferramentas

| Referência | Uso | Observação |
|---|---|---|
| JUCE (C++) | Framework para construir o plugin (VST3/AU/AAX) | Uso aprovado (D26). Opções de licença: AGPLv3 (código aberto) ou licença comercial, conforme o [LICENSE.md do JUCE](https://raw.githubusercontent.com/juce-framework/JUCE/master/LICENSE.md), consultado em 01/11/2026. O projeto usa a AGPLv3 (D42). Na versão usada, 9.0.4 (D49), o [LICENSE.md](https://raw.githubusercontent.com/juce-framework/JUCE/9.0.4/LICENSE.md) mantém essa opção (conferido em 02/11/2026) |
| Matchering (Python) | Matemática de comparação de espectro com um alvo | Licença GPL. Estudar os princípios é permitido; usar o código só com licença compatível, crédito ao autor, aprovação do usuário e registro em `02-decisoes.md` (D39) |
| VST3 SDK (Steinberg) | Formato do plugin | Usado através do JUCE |

## Material de teste

- Minha voz seca no Shure SM7dB (a gravar na Fase 3 — D33).
- Gravações de conhecidos, sempre com permissão (D79).
- Bancos de vozes, com licenças e créditos: ver `11-bancos-de-vozes.md` (D79, D80).

## Anotações de estudo

Uma nota curta por módulo, escrita na etapa de estudo (D37). **Modelo de nota:**

```
### DD/MM/AAAA — Nome do módulo
**Fontes consultadas:** …
**Fatos (documentados):** …
**Suposições (funcionamento interno de produtos fechados):** …
**O que levar para a proposta:** …
```

### 02/11/2026 — Módulo de EQ

**Fontes consultadas:**
- Princípios de DSP: [Audio EQ Cookbook (W3C)](https://www.w3.org/TR/audio-eq-cookbook/), [Peaking Equalizers (J. O. Smith)](https://ccrma.stanford.edu/~jos/fp/Peaking_Equalizers.html), [Direct Form II biquad (musicdsp)](https://www.musicdsp.org/en/latest/Filters/275-direct-form-ii-biquad.html) e [About Biquad Topology (Harman)](https://audioworxdevelopers.harman.com/?p=37662).
- Plugins: [modos de processamento do FabFilter Pro-Q](https://www.fabfilter.com/help/pro-q/using/processingmode), [ajuda do iZotope Nectar 3](https://s3.amazonaws.com/izotopedownloads/docs/nectar300/en/print/index.html), [sonible smart:EQ](https://www.sonible.com/?p=162301), [manual do oeksound soothe2](https://www.oeksound.com/manuals/soothe2) e [ReaEQ, wiki da Cockos](https://www.cockos.com/wiki/index.php/ReaEQ) (só o trecho da busca; a página não abriu).
- JUCE 9.0.4, lido no próprio código: `juce_IIRFilter.h`, `juce_IIRFilter.cpp`, `juce_IIRFilter_Impl.h`, `juce_SmoothedValue.h` e `juce_AudioProcessor.h`.
- Prática de mixagem: guias da [Avid](https://www.avid.com/resource-center/how-to-eq-vocals) e da [Audient](https://audient.com/tutorial/how-to-eq-vocals/).

**Fatos (documentados):**
- O Cookbook descreve filtros biquad para passa-alta, sino e prateleiras, com frequência, ganho e Q, usando a transformação bilinear. Nele, um corte e um realce de mesmo valor, com a mesma frequência e o mesmo Q, se anulam no sino.
- A transformação bilinear comprime a resposta perto do limite superior de frequência (metade da taxa de amostragem). Existem métodos que reduzem isso, como o de [Vicanek](https://www.vicanek.de/articles/BiquadFits.pdf) (só os títulos das seções e um trecho da busca puderam ser lidos).
- O filtro do JUCE (`dsp::IIR::Filter`) usa a estrutura Transposed Direct Form II. A `musicdsp` a recomenda para ponto flutuante, por ser numericamente melhor que a Direct Form II.
- No JUCE, `Coefficients::make...` faz `new`, ou seja, pede memória. `ArrayCoefficients::make...` devolve um `std::array`, sem pedir memória.
- Atribuir um `std::array` a um `Coefficients` já criado reaproveita a memória existente (capacidade para 8 valores). O filtro só pede memória nova se a ordem do filtro mudar.
- `SmoothedValue` faz transições suaves de valores, nos modos linear e multiplicativo; a própria documentação indica o multiplicativo para frequência.
- O JUCE permite informar um parâmetro de bypass ao DAW (`getBypassParameter`) e, nesse caso, o `processBlock` precisa tratá-lo.
- O Pro-Q tem modo sem latência, descrito como "suficiente para a maioria das aplicações". O modo de fase linear tem de 3072 a 66560 amostras de latência em 44,1 kHz e é tratado como ferramenta para problemas específicos.
- O EQ do Nectar 3 oferece, por banda, liga/desliga, frequência, ganho e largura, e o Vocal Assistant define um EQ a partir da análise da voz.
- O smart:EQ tem controle de tempo de aprendizado e equilibra o sinal em direção a um perfil-alvo, como voz. O soothe2 é um processador dinâmico de ressonâncias (fora da v1.0).
- O ReaEQ tem sino, prateleiras, passa-baixa e passa-alta, com 4 bandas por padrão.

**Prática comum, não regra:** os guias citam passa-alta em 80–100 Hz, embolado em 200–500 Hz, presença em 3–5 kHz e ar acima de 10 kHz. Os próprios guias os tratam como pontos de partida, e nenhum deu faixas de fundamental por tipo de voz.

**Suposições (funcionamento interno de produtos fechados):**
- Não se sabe quais fórmulas de filtro o Pro-Q, o Nectar, o smart:EQ ou o ReaEQ usam internamente, nem como o smart:EQ e o Vocal Assistant transformam a análise em bandas.
- Suponho que a compressão perto do limite superior afete pouco o sino de presença, mas possa alterar o desenho da banda de ar em 44,1 kHz. Isso será conferido nos testes, em mais de uma taxa de amostragem.
- Suponho que biquads com parâmetros suavizados bastem para mudanças manuais sem estalos, porque nenhuma fonte trouxe uma medição comparativa. O teste no Reaper precisa confirmar.

**Resultado do teste no Reaper (02/11/2026):** o usuário informou que os 5 passos da D66 passaram, inclusive a curva das bandas em 44,1 e 96 kHz e a mudança dos controles durante a reprodução, sem medição numérica da diferença entre as taxas.

**O que levar para a proposta:** biquads de ordem fixa, sem latência; `ArrayCoefficients` para não pedir memória na thread de áudio; parâmetros suavizados; bandas e valores iniciais vindos do perfil de instrumento, sem copiar presets nem interface de nenhum produto.

### 02/11/2026 — Módulo de análise e RTA

**Fontes consultadas:**
- Espectro médio: [método de Welch](https://en.wikipedia.org/wiki/Welch%27s_method), [LTAS no VoceVista](https://www.vocevista.com/en/doc/theory-long-term-average/), [estudo sobre LTAS e idioma](https://www.researchgate.net/publication/308843972_The_Impact_of_Language_on_Voice_An_LTAS_Study) e [relatório sobre LTAS e intensidade vocal (KTH)](https://scispace.com/pdf/effect-on-ltas-of-vocal-loudness-variation-5g9vn08xsl.pdf) (estes dois últimos, só pelo trecho da busca), e [artigo sobre duração de amostras de voz](https://www.researchgate.net/publication/397898434_On_Acoustic_Voice_Quality_Index_measurement_reliability_in_digital_health_applications_a_narrative_review_and_empirical_evaluation_of_speech_sample_length_requirements) (idem).
- Trechos com som: [planicidade espectral](https://en.wikipedia.org/wiki/Spectral_flatness), [detecção de respiração no canto (Nakano et al., AIST)](https://staff.aist.go.jp/t.nakano/Breath/) e [resumo do gating de loudness da EBU R 128](https://en.wikipedia.org/wiki/EBU_R_128).
- Fundamental: [YIN (de Cheveigné e Kawahara, 2002)](https://sonicfield.org/library/yin-a-fundamental-frequency-estimator-for-speech-and-music) e [nota da LabROSA](https://www.ee.columbia.edu/~dpwe/LabROSA/doc/yin.html), [pYIN](https://code.soundsoftware.ac.uk/projects/pyin), [estudo comparativo em canto](https://arxiv.org/abs/1912.12609), [comparação com CREPE](https://arxiv.org/pdf/2206.14357) e [faixas de canto (DPA)](https://www.dpamicrophones.com/dict/singing-voices-and-frequencies/). O PDF do artigo do YIN não pôde ser lido.
- Ressonâncias: [artigo da CEDAR](https://www.cedaraudio.com/article/000010/removing-audio-resonances), [Sound On Sound](https://www.soundonsound.com/sound-advice/q-how-do-pinpoint-nasty-resonances), [equalização de ressonâncias com redes neurais](https://arxiv.org/abs/1807.08636), [TarsosDSP](https://0110.be/releases/TarsosDSP/TarsosDSP-latest/TarsosDSP-latest-Documentation/be/tarsos/dsp/SpectralPeakProcessor.html) e [formantes](https://en.wikipedia.org/wiki/Formant).
- RTA e captura: [tutorial de analisador do JUCE](https://juce.com/tutorials/tutorial_spectrum_analyser/), [Voxengo SPAN](https://www.voxengo.com/product/span) e o código do JUCE 9.0.4 (`juce_FFT.h`, `juce_AbstractFifo.h`).

**Fatos (documentados):**
- O método de Welch divide o sinal em trechos sobrepostos, aplica uma janela, calcula o espectro de potência de cada um e tira a média. Isso reduz a variação da medida, ao custo de resolução em frequência.
- O LTAS mostra o que permanece na voz e perde o que muda, como vibrato. O VoceVista recomenda material uniforme, porque silêncio na média eleva o piso de ruído.
- Faixas citadas para LTAS de fala: estabiliza depois de cerca de 40 s (Fritzell et al., 1974, citado por outro autor) e estudos usam de 20 a 90 s. **Não encontrei medição para canto nem só para trechos vozeados.**
- A planicidade espectral vai de perto de 0 (som tonal) a perto de 1 (som parecido com ruído).
- O gating de loudness usa dois estágios: um limite absoluto (-70 LUFS) e um relativo à média do material (-10 LU). É uma norma de loudness, não de voz.
- Em canto sem acompanhamento, Nakano et al. mediram 1.488 respirações. O espectro médio delas tem um pico perto de 1,6 kHz em homens e 1,7 kHz em mulheres, e o detector deles usa um modelo estatístico treinado (HMM), com 97,5% de recall e 77,7% de precisão.
- O YIN parte da autocorrelação, com modificações que reduzem erros; os autores relatam erro cerca de 3 vezes menor que o dos melhores concorrentes, em fala. O limite dele define a proporção de potência não periódica: subir evita alguns erros, baixar evita outros (LabROSA).
- O pYIN é um YIN com vários candidatos e um modelo estatístico que escolhe o caminho mais provável. Uma revisão relata resultados parecidos entre pYIN e CREPE em canto, e uma comparação relata precisão fina parecida entre YIN e CREPE.
- Faixa de canto, segundo a DPA: baixo de 73 a 330 Hz; soprano de 247 a 1047 Hz. Cantores treinados podem ir além.
- Formantes são picos largos, causados pelo trato vocal, e mudam com a vogal (F1 de cerca de 240 a 850 Hz, em voz masculina). O formante do cantor fica perto de 3 kHz.
- A CEDAR descreve ressonâncias como picos estreitos, tipo agulha, e elevações largas como timbre da voz. A Sound On Sound cita picos esporádicos de 1 a 4 kHz em voz, avisa que o pico mais alto nem sempre é o culpado e manda decidir de ouvido. **Não encontrei um algoritmo padrão de detecção.** O TarsosDSP estima um piso com filtro de mediana e guarda picos acima dele por um fator.
- O tutorial do JUCE usa FFT de 2048 pontos, janela de Hann e atualização de 30 vezes por segundo, mas sincroniza as threads com um `bool` comum. O `AbstractFifo` do JUCE é a alternativa sem trava para um escritor e um leitor.

**Suposições (a confirmar nos testes):**
- Os produtos fechados citados (como soothe2, Slick EQ e Blade EQ) não revelam o algoritmo; nada aqui sabe como funcionam por dentro.
- De memória, sem ter lido o artigo: o YIN usa um limite absoluto de cerca de 0,1. Precisa de calibração em canto.
- Periodicidade baixa deve separar a maior parte das respirações sem um modelo treinado. Nenhuma fonte testou isso em canto.
- Comparar o espectro em duas resoluções deve separar ressonâncias estreitas de formantes largos. Essa regra é minha, inspirada na CEDAR e no TarsosDSP.
- Em nota longa, harmônicos parecem ressonâncias. Exigir que o pico persista em notas diferentes deve evitar esse erro, mas nenhuma fonte validou essa regra.
- Excluir consoantes como "s" da média deve reduzir o peso dos agudos. O efeito ainda precisa ser medido.
- Os tempos de análise da fala podem não valer para canto.

**Resultado da verificação com voz sintética (02/11/2026):** a comparação direta de resoluções gerou falsas candidatas, por causa de harmônicos que coincidem entre notas. A medida passou a usar só os harmônicos (P40). Limites observados: picos perto de 4 kHz foram subestimados (+9 dB inseridos apareceram como +3,3 dB) e localizados com 2% de erro; na região grave, a detecção depende de haver notas suficientes. Com voz real, o método foi ajustado na D83 (limite de 4 dB e busca acima de 6 vezes a fundamental); resultados em `08-plano-testes.md`.

**O que levar para a proposta:** captura sem bloquear o áudio e análise fora da thread de áudio; média de Welch só com trechos vozeados; YIN e estatísticas robustas (mediana e percentis), em vez de um único valor; ressonâncias apresentadas como candidatas, sempre relativas ao espectro da própria voz; RTA com `AbstractFifo`; duração da análise medida com vozes reais.

### 02/11/2026 — Módulo de correção automática

**Fontes consultadas:**
- Plugins: [atualização do Nectar Pro](https://www.izotope.com/en/learn/pro-plugin-assistants-updates?page=11), [Vocal Assistant do Nectar](https://www.izotope.com/en/learn/nectar-vocal-assistant), [ajuda do Nectar Elements](https://s3.amazonaws.com/izotopedownloads/docs/nectar-elements300/en/print/index.html), [Nectar 4 na Sound On Sound](https://soundonsound.com/reviews/izotope-nectar-4-advanced) e [smart:EQ 3 na Mix](https://www.mixonline.com/technology/product-of-the-week-sonible-smarteq-3) (estes dois últimos, só pelo trecho da busca).
- Pesquisa: [Pestana et al., 2013](https://ciencia.ucp.pt/en/publications/spectral-characteristics-of-popular-commercial-recordings-1950-20/), só o resumo, sobre música completa, não voz isolada.
- Prática de mixagem: [Audient](https://audient.com/tutorial/how-to-eq-vocals/). O guia da [Avid](https://www.avid.com/resource-center/how-to-eq-vocals) recusou o acesso nesta consulta.
- Medições próprias: voz do usuário e 20 vozes do VocalSet, com `banco equilibrio` e `banco proposta` (`08-plano-testes.md`).

**Fatos (documentados):**
- A EQ de caráter do Nectar Pro só ajusta uma região quando a voz sai da faixa-alvo. Dentro da faixa, não mexe. É a mesma ideia da P08.
- O Vocal Assistant aplica uma curva inicial e oferece controles de escala: em 0,0x a EQ de caráter fica sem efeito, e em 2,0x dobra. O smart:EQ 3 também tem uma curva para aumentar ou reduzir o quanto corrige.
- A Audient cita corte suave de 3 a 6 dB em 200–500 Hz, sem deixar a voz fina; realce de 2 a 4 dB para presença; 2 a 3 dB para ar; e passa-alta para tirar ronco, mantendo o calor. São pontos de partida, não regras.
- Pestana et al. encontraram uma tendência a uma curva-alvo em gravações comerciais. Não encontrei uma curva-alvo publicada só para voz cantada isolada.

**Suposições:**
- Nenhum produto fechado revela como transforma a análise em bandas.
- Uma faixa medida em 20 cantores treinados, no mesmo modelo de microfone, é um ponto de partida razoável, mas pode não valer para outras vozes e microfones. A voz mais grave do banco tem 174 Hz de mediana; a do usuário, cerca de 135 Hz.
- O dicionário da voz (D92) tem apoio do guia nas regiões de ronco, embolado, presença, sibilância e ar. **As regiões de corpo, caixa e nasal ficam como prática comum não conferida nesta consulta.** Tudo será validado de ouvido.

**O que levar para a proposta:** corrigir só o que estiver fora de uma faixa; medidas relativas à própria voz; limites parecidos com os da prática citada; controle de intensidade; regras simuladas antes de entrar no plugin.

### 02/11/2026 — Medida de volume em LUFS (D103)

**Fontes:** [ITU-R BS.1770](https://www.itu.int/rec/R-REC-BS.1770) e [EBU Tech 3341](https://tech.ebu.ch/publications/tech3341).

**Conferido:** a [versão BS.1770-1](https://itu.int/dms_pubrec/itu-r/rec/bs/R-REC-BS.1770-1-200709-S!!PDF-F.pdf) mostrou, num trecho de busca, a1 e a2 do segundo filtro e a2 e b2 do primeiro, iguais aos usados aqui, e confirma que os valores são para 48 kHz.

**Não conferido na fonte:** os PDFs da BS.1770-5 e da Tech 3341 não puderam ser lidos nesta consulta. Os outros três coeficientes do primeiro filtro, os portões de -70 LUFS e -10 LU e os níveis dos casos 1, 3, 4 e 5 da Tech 3341 foram escritos de memória. Os resultados esperados batem com a conta e com o medidor, mas falta a comparação com um medidor externo: o do Reaper.
