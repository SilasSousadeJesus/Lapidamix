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
- Bancos abertos de trilhas separadas (stems) com vozes variadas — a pesquisar e escolher na Fase 5. ❓

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
