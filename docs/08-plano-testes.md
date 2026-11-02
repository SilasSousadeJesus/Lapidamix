# 08 - Plano de Testes

🟡 Proposta inicial, a refinar ao longo do projeto.

## Material de teste
- **Voz principal:** minha voz seca no SM7dB, sem nenhum efeito, ~30 s cantados.
- **Variedade:** vozes graves e agudas, masculinas e femininas, microfones diferentes (dinâmicos e condensadores), gravações boas e problemáticas. Meta: de 3 a 5 vozes bem diferentes na Fase 3 e pelo menos 20 vozes diferentes na Fase 5 (D38).
- Arquivos de áudio de teste ficam fora do Git, na pasta `test-audio/`; a origem e os créditos ficam em `11-bancos-de-vozes.md`.

## O que verificar

**Técnico** (no Reaper, o DAW de testes — D21)
- Nenhum estalo, clique ou travamento durante a reprodução.
- Uso de CPU baixo.
- Parâmetros salvos e recuperados ao reabrir o projeto.
- Bypass sem diferença de volume nem atraso perceptível.

**Sonoro**
- Embolamento reduzido sem a voz ficar "fina".
- Mais clareza e presença sem soar estridente.
- Nenhuma correção acima dos limites de segurança.
- Fundamental preservada (peso da voz mantido).
- Teste A/B com bypass: o resultado soa melhor que o original.
- Banda de ressonância: validação final de ouvido, em A/B contra o bypass, vendo as bandas antes de aplicar (D85, D88).
- Teste "às cegas": ouvir sem saber qual versão é qual.

**Adaptação**
- Vozes diferentes recebem correções diferentes e coerentes.
- Uma voz que já está boa recebe pouca ou nenhuma correção.

## Registro de testes

| Data | Voz / material | Versão do plugin | O que foi testado | Resultado | Observações |
|---|---|---|---|---|---|
| 02/11/2026 | Voz do usuário, no Reaper | 0.1.0 (plugin vazio) | Os 5 passos da D53: carregar, conferir a lista, ouvir com e sem o plugin, teste de cancelamento, salvar e reabrir | ✅ Todos passaram | Teste feito e informado pelo usuário. Encerra a Fase 1 |
| 02/11/2026 | Sinais gerados por programa temporário, sem DAW | 0.1.0 (com EQ) | Transparência bit a bit, curvas das 5 bandas, bypass, ganho de saída, extremos de 22,05 a 192 kHz, denormais, mono, salvamento e pedidos de memória na thread de áudio | ✅ Todas as verificações passaram | Feito por Claude, em compilação Debug. Não substitui o teste no Reaper (D66) |
| 02/11/2026 | Voz do usuário e ruído, no Reaper, em 44,1 e 96 kHz | 0.1.0 (com EQ) | Os 5 passos da D66: transparência, curva de cada banda, mexer durante a reprodução, salvar e reabrir, mixar a voz à mão | ✅ Todos passaram | Teste feito e informado pelo usuário. Encerra a Fase 2 |
| 02/11/2026 | Tons e voz sintética, gerados por programa temporário, sem DAW | 0.1.0 (com análise) | Fundamental de 65 a 1047 Hz em 3 taxas; voz válida contra todo o som; média em potência; ressonâncias inseridas e falsas candidatas; ruído e silêncio; captura e RTA sem alterar o som nem pedir memória; meta de 30 s; salvar e reabrir | ✅ Todas as verificações passaram | Feito por Claude, em compilação Debug. Não substitui o teste com voz real (D77), que ainda não foi feito |
| 02/11/2026 | Voz cantada do usuário, SM7dB e SM57 (`test-audio/voz-usuario/`), por programa temporário, sem DAW | 0.1.0 (com análise) | A04, comparação de microfones e P40; detalhes abaixo | ⚠️ Fundamental e microfones: como esperado. Ressonâncias: nesta voz, falham em 300 e 500 Hz e funcionam a partir de 1 kHz, com dúvidas em 3 e 6 kHz (P41) | Feito por Claude. Uma só voz e uma só música. Não substitui o teste no Reaper nem a conferência no Praat (D77) |

| 02/11/2026 | Voz cantada do usuário, SM7dB e SM57, com `tools/medidor-voz` | 0.1.0 (com a D83) | Mesmas medições, depois de implementar a D83 no plugin | ✅ Mesmos números da simulação da P41 | Feito por Claude. Compilação Release do plugin sem erros. Não substitui o teste no Reaper (D77) |

| 02/11/2026 | Voz cantada do usuário, SM7dB e SM57, no Reaper | 0.1.0 (com a D83) | Análise com o plugin, olhando medidas e curvas | ✅ Análise funcionou nas duas; ⚠️ o resultado do SM57 não corresponde ao início da gravação (detalhes abaixo) | Teste feito e informado pelo usuário. Mostrou os problemas de texto corrigidos no registro seguinte |
| 02/11/2026 | Gravação do SM57 passada pelo plugin, sem DAW (`tools/captura-interface`) | 0.1.0 (textos corrigidos) | Acentos, casas decimais e curvas, numa imagem da janela | ✅ Conferido na imagem | Feito por Claude. Falta o usuário ver no Reaper |
| 02/11/2026 | 20 vozes do VocalSet (5 em detalhe), com `tools/medidor-voz` | 0.1.0 (com a D83) | Fundamental, candidatas, estabilidade e limite de 6 vezes a fundamental | ⚠️ Fundamental coerente; picos inseridos pouco encontrados nessas vozes (detalhes abaixo) | Feito por Claude, sem DAW. Sem gabarito: o VocalSet não informa notas nem ressonâncias |

## Intensidade e compensação de volume (passo 4, 02/11/2026)

Verificado por Claude, sem DAW, com `verifica` e uma imagem da janela. Falta conferir de ouvido no Reaper.

| Item | Resultado |
|---|---|
| Intensidade de 50%, no cálculo | ✅ Ganhos pela metade; passa-alta de 70 Hz vira 52,9 Hz (D100) |
| Intensidade de 0% | ✅ Bandas desligadas e Saída em 0 dB |
| Mover a intensidade antes de aplicar | ✅ Nada muda no som; só a lista |
| Compensação em LUFS (D103), a 100% e a 50% | ✅ SM7dB: +1,7 dB e +0,8 dB. SM57: +1,9 dB e +0,9 dB. Com a ponderação A, o SM7dB dava +1,9 dB e +1,0 dB |
| LUFS integrado na gravação inteira, com o plugin de verdade | ✅ SM7dB: -21,09 antes e -21,17 depois. SM57: -23,29 antes e -23,36 depois. Diferença menor que 0,1 LU |
| Medidor de LUFS da verificação, com tons de 1 kHz (casos 1, 3, 4 e 5 da EBU Tech 3341), a 44,1 e 48 kHz | ✅ De -22,98 a -23,01, esperado -23,0 |
| Aviso junto à intensidade (D104) | ✅ Cabe na janela, conferido numa imagem |
| Saída durante a reprodução | ✅ Não muda (D102) |
| Saída ajustada à mão | ✅ Continua, inclusive ao reabrir o projeto, até a intensidade mudar |
| Análise nova ainda não aplicada | ✅ Mover a intensidade não muda o som (D104) |
| Antes de aplicar, com análise e RTA ligados | ✅ Saída idêntica à entrada, sem pedidos de memória na thread de áudio |

**Limites:** a igualdade de volume foi medida em duas gravações, com um medidor nosso. Falta a conferência com o medidor de LUFS do Reaper, e parte dos valores da norma não foi conferida na fonte (`07-referencias.md`). A banda de ar ajustada à mão não entra na compensação.

## Lista de bandas e botão Aplicar (passo 3, 02/11/2026)

Verificado por Claude, sem DAW, com `verifica` e uma imagem da janela (`captura`). Falta o usuário conferir no Reaper.

| Item | Resultado |
|---|---|
| Depois de analisar, sem clicar | ✅ Bandas continuam desligadas, e a lista aparece |
| Aplicar, com o SM7dB | ✅ Passa-alta em 71 Hz, -5,0 dB em 316 Hz e +3,0 dB em 3,5 kHz nos parâmetros |
| O som muda como o plano diz, com tons | ✅ -5,0 dB em 316 Hz; +3,0 dB em 3,5 kHz; -22,1 dB em 20 Hz |
| Banda de ar ajustada à mão | ✅ Aplicar não mexe nela (D97) |
| Analisar de novo | ✅ As bandas não mudam até novo clique |
| Plano novo em que uma banda não age mais | ✅ Ela desliga e volta a 0 dB |
| Reabrir o projeto | ✅ As bandas aplicadas voltam |
| Menos de 10 s de voz (6,0 s) | ✅ Medidas e aviso com a solução; sem lista; Aplicar desligado (D101) |
| Antes de aplicar, com análise e RTA ligados | ✅ Saída idêntica à entrada, sem pedidos de memória na thread de áudio |

O volume ainda não é compensado, e a intensidade ainda não existe (passo 4). Por isso, o A/B com o bypass ainda não é justo.

## Dados novos na análise do plugin (passo 2, 02/11/2026)

A análise agora calcula as duas metades da voz e os saltos de oitava, guarda até 20 candidatas (a janela mostra as 3 mais fortes) e salva tudo no projeto. Isso roda fora da thread de áudio.

- **Saltos de oitava, com tons conhecidos:** 0,0% em um tom constante e 12,0% em um tom que salta a cada 200 ms. Nesse caso, a análise e as regras juntas deixam o passa-alta em 40 Hz (D91).
- **Duas metades, com a voz do usuário:** um pico de +9 dB em toda a gravação conta como presente nas duas metades; o mesmo pico só na primeira metade vira candidata, mas não conta como persistente.
- **Salvar e restaurar:** os dados novos voltam. Projetos antigos não têm esses dados: nesse caso, nada é cortado na ressonância.
- **Plano nas 78 janelas, passo 1 × passo 2:** 77 iguais. A diferença está no male8, janela 2, que ganha -3,0 dB em 1100 Hz, porque as metades agora são calculadas dentro da mesma análise. A contagem de cortes de ressonância passa de 17 para 18 janelas nos homens.
- **Saltos de oitava em voz real:** no máximo 5,3%, abaixo do limite de 8%.
- **Restante da verificação:** saída idêntica à entrada, sem pedidos de memória na thread de áudio e 0 falhas. O som do plugin não muda neste passo.

## Regras da Fase 4 no código real (passo 1, 02/11/2026)

`src/core/correction/`, ainda sem ligação com o som nem com a janela. As duas metades e os saltos de oitava ainda vêm da ferramenta de teste (passo 2).

- **Casos conhecidos (`verifica`):** 18 verificações das regras, todas OK, inclusive a proteção do passa-alta (confiança baixa e saltos de oitava levam a 40 Hz), que a voz real não exercitou. O resto da verificação do plugin continua sem falhas.
- **Código real × simulação abaixo (78 janelas):** 72 iguais. As outras 6 têm a mudança esperada: realce de presença menor que 1 dB, que a simulação ainda mostrava e agora não liga a banda. Nenhuma diferença inesperada.
- **Plugin:** compila em Release, e o som não muda.

## Simulação das regras propostas para a Fase 4 (02/11/2026)

Feita com `banco proposta` e `banco equilibrio`, sem mudar o plugin, em janelas de 30 s. Limites simulados: embolado até +3 dB; presença a partir de -19 dB; ressonância pela D87/D88.

**Medidas, no material inteiro (dB):**

| Medida | VocalSet (20 vozes) | SM7dB | SM57 |
|---|---|---|---|
| 200–500 Hz contra 500 Hz–2 kHz | -5,1 a +2,5 | **+11,9** | **+12,0** |
| 2–5 kHz contra 500 Hz–2 kHz | -19,2 a -6,4 | **-24,2** | -18,2 |
| 10–14 kHz contra 2–5 kHz | -42,3 a -20,7 | -30,0 | -20,3 |

O embolado aparece igual nos dois microfones, então vem da voz, da distância ou da região grave, não só do SM7dB. O "escuro" (pouca presença) é o que muda: está fora da faixa no SM7dB e dentro no SM57. Isso combina com o que o usuário descrevia.

**O que cada banda faria:**

| Material | Passa-alta | Embolado | Ressonância | Presença |
|---|---|---|---|---|
| SM7dB (3 janelas) | 63–70 Hz | -5 dB em 316 Hz, nas 3 | Nada | +3 dB, nas 3 |
| SM57 (3 janelas) | 61–69 Hz | -5 dB em 316 Hz, nas 3 | -2,2 dB em 4941 Hz, em 1 | Abaixo de 1 dB |
| 9 mulheres (32 janelas) | 100 Hz (teto) | Não age: a fundamental ocupa a região | Nada | +1,1 a +2,7 dB, em 4 |
| 11 homens (40 janelas) | 87–100 Hz | -1,5 a -1,9 dB, em 3 | -2,0 a -3,0 dB, em 17 | +1,2 a +3,0 dB, em 2 (male9) |

**Limites:**
- Saltos de oitava: máximo de 5,1%; confiança: nunca abaixo de 0,85. A proteção do passa-alta não disparou nenhuma vez, nem no male6, em que a aguda está errada. Com a aguda errada, o embolado não age, o que é uma falha segura.
- Faixas medidas em cantores treinados, no mesmo modelo de microfone. A voz do usuário fica fora da faixa do embolado, mas pode ser por ser mais grave que todas as do banco (135 Hz contra 174 Hz ou mais).
- Só o A/B de ouvido dirá se -5 dB em 316 Hz limpa sem afinar.

## Verificação sem o Reaper (D89, 02/11/2026)

Feita por Claude, com `tools/praat` e `tools/verifica-plugin`. **O usuário fechou a Fase 3 com as 3 medidas fora da tolerância registradas como exceção (D90).** O erro de oitava do male6 levou à proteção do passa-alta na Fase 4 (D91).

**Fundamental, plugin × Praat 7.0.02, no trecho de 30 s de voz válida, do início.** Tolerância de ±3%.

| Voz | Mediana (plugin / Praat) | Dif. | Grave 5% | Dif. | Aguda 95% | Dif. |
|---|---|---|---|---|---|---|
| SM7dB | 132,8 / 132,9 Hz | 0,08% | 99,1 / 99,2 Hz | 0,19% | 153,0 / 153,0 Hz | 0,03% |
| SM57 | 132,2 / 132,4 Hz | 0,20% | 98,0 / 100,5 Hz | 2,50% | 151,1 / 151,1 Hz | 0,03% |
| female3 | 417,7 / 434,5 Hz | **3,86% ❌** | 304,9 / 305,2 Hz | 0,10% | 586,6 / 588,2 Hz | 0,27% |
| female7 | 390,5 / 391,3 Hz | 0,21% | 286,7 / 287,2 Hz | 0,15% | 581,9 / 581,8 Hz | 0,02% |
| male9 | 416,4 / 415,3 Hz | 0,26% | 281,8 / 282,1 Hz | 0,12% | 566,3 / 563,2 Hz | 0,54% |
| male8 | 192,1 / 192,0 Hz | 0,01% | 129,4 / 128,3 Hz | 0,83% | 258,2 / 257,9 Hz | 0,11% |
| male6 | 220,4 / 217,7 Hz | 1,23% | 146,4 / 146,5 Hz | 0,10% | 572,2 / 293,6 Hz | **94,87% ❌** |

No material inteiro, a mediana e o grave ficam dentro de 3% nas 7 vozes (maior diferença: 0,96% e 1,13%). A aguda fica dentro em 6 vozes; no male6, dá 313,0 / 290,3 Hz (**7,82% ❌**). No total, **39 de 42 medidas ficam dentro da tolerância**.

- **female3:** a mediana cai entre duas notas próximas, e as pontas e o material inteiro concordam (0,41%). Provavelmente é a diferença de quais trechos cada programa conta como voz, mas isso não foi confirmado.
- **male6:** o Praat vê quase nada acima de 400 Hz (0,1% a 0,3% dos trechos com voz), enquanto o plugin põe pelo menos 5% acima de 572 Hz nos primeiros 30 s. A aguda do plugin está errada nessa voz, provavelmente por erro de oitava em parte dos trechos. A aguda não é usada por nenhuma regra decidida; a mediana (D83) e o grave (D78) dessa voz ficam dentro da tolerância. Nada foi mudado (D81).

**Espectro, plugin × Praat, em terços de oitava, sem o volume geral, nos 30 s:** diferença de forma de 0,40 a 1,94 dB rms, e maior diferença de 1,08 a 6,43 dB (female3). No material inteiro: de 0,31 a 1,88 dB rms. A D77 não define tolerância para o espectro.

**Plugin sem DAW (`verifica`), com 0 falhas:**

| Item | Resultado |
|---|---|
| Saída igual à entrada, com análise e RTA ligados | ✅ 0 amostras diferentes de 12.395.520 |
| Bypass com banda ligada | ✅ Saída idêntica |
| Pedidos de memória na thread de áudio | ✅ 0 |
| Análise mede antes do EQ | ✅ Médias iguais com +12 dB em 2,2 kHz |
| RTA reage ao EQ | ✅ +11,7 dB em 2,2 kHz e 0,0 dB em 100 Hz |
| Aviso com pouca voz | ✅ Aparece com 6,0 s; não aparece com 36,2 s |
| Salvar e restaurar | ✅ Medidas, curvas (com uma casa decimal) e candidatas voltam |
| Fechar o plugin com a análise rodando | ✅ Não trava |
| Pico de +9 dB em 2,2 kHz, Q 8, com filtro do JUCE | ✅ SM7dB: 2201 Hz (+4,8 dB, em 50 s). SM57: 2138 Hz (+4,1 dB, em 50 s) e 2201 Hz (+4,8 dB, em 100 s) |
| Tempo por bloco de 11,6 ms | Indício: média de 0,0003 ms, pior de 0,043 ms |

**Dispensado por decisão do usuário (D89), sem verificação:** estalos durante a reprodução, uso de CPU no DAW e teste de ouvido em 4,8 kHz.

**Limitação:** o Reaper não foi usado nos itens acima. Ficam sem conferência real no DAW: o ReaEQ de verdade, salvar dentro de um projeto, o cancelamento dentro do DAW e o motivo de o primeiro teste do SM57 ter dado um resultado parecido com o de 90 s de música. Com o mesmo áudio, do início, o plugin sem DAW dá o esperado (51,2 s, 132 Hz, 98 Hz).

## Segundo teste no Reaper (02/11/2026)

Informado pelo usuário: interface e SM7dB. **Os resultados dos outros itens não foram informados**: a mensagem trouxe esse campo sem preencher.

- **Interface:** acentos, casas decimais e curvas corretos.
- **SM7dB, duas análises seguidas:** 30,0 s de voz em 49,9 s e 49,2 s de captura; mediana 133 Hz; grave de 99 e 100 Hz; aguda de 153 Hz; confiança de 99%; candidata em 4800 Hz (+4,1 dB, 49% e 50% do tempo).
- **A candidata nova não é uma mudança no plugin:** ela está no limite de 4 dB. No medidor, começar 1 s depois basta para a candidata aparecer perto dali (4941 Hz, +4,0 dB), e começar do zero dá "nenhuma". Pequenas diferenças no ponto de início mudam o resultado nesse limite.
- **Não seria cortada pela regra D87/D88:** +4,1 dB fica abaixo do mínimo de +6 dB, e 49% a 50% do tempo fica abaixo do mínimo de 60%.

## Teste no Reaper (02/11/2026)

| | Voz válida / captura | Mediana | Grave (5%) | Aguda (95%) | Confiança | Candidatas |
|---|---|---|---|---|---|---|
| SM7dB | 30 s em 52,6 s | 133 Hz | 100 Hz | 153 Hz | 99% | Nenhuma |
| SM57 | 30 s em 38,1 s | 142 Hz | 87 Hz | 152 Hz | 99% | 4800 Hz (+6 dB, 72% do tempo) |

**O que o usuário viu:**
- **SM7dB:** montanha larga de cerca de 120 a 250 Hz e queda forte a partir de cerca de 2 kHz (escuro e embolado, como ele descrevia). As curvas de voz e de todo o som ficaram quase iguais.
- **SM57:** graves mais concentrados em cerca de 200 Hz e bem mais energia de 2 a 8 kHz. A curva de todo o som ficou acima da de voz entre cerca de 5 e 8 kHz (sibilância).

**Conclusões do usuário:**
- A análise se adapta ao som (D25): é a mesma voz, com retratos diferentes por microfone.
- As duas médias funcionaram.
- A variação do grave (5%), de 100 para 87 Hz, confirma a necessidade da margem de segurança do passa-alta (P42).
- A candidata em 4,8 kHz pode ter contribuição da elevação natural de presença do SM57; o usuário vai conferir de ouvido.

**Ressalva da conferência, com o `banco inicio`:** o usuário indicou que tocou sempre do início, mas o resultado do SM57 não bate com o início da gravação.

| SM57 | Captura | Mediana | Grave (5%) | Candidata |
|---|---|---|---|---|
| Reaper | 38,1 s | 142 Hz | 87 Hz | 4800 Hz (+6 dB, 72%) |
| Medidor, do início | 51,0 s | 132 Hz | 98 Hz | 4941 Hz (+6,6 dB, 74%) |
| Plugin sem DAW, do início | 51,2 s | 132 Hz | 98 Hz | 4941 Hz (+6,5 dB, 74%) |
| Medidor, a partir de 90 s | 35,7 s | 142 Hz | 87 Hz | 4800 Hz (+5,8 dB, 71%) |

O SM7dB bate com o início (50,5 s no medidor, 52,6 s no Reaper, 133 Hz, 99 Hz). Com as duas gravações do início, o grave (5%) é 99 × 98 Hz. Portanto, 100 × 87 Hz parece vir de **trechos diferentes da música, não de tomadas diferentes**. A necessidade da P42 continua, porque trechos diferentes de uma mesma gravação também variam: o próprio SM57 dá 98 Hz no início e 87 Hz perto de 90 s. A causa no Reaper não foi confirmada; repetir o teste do SM57.

## Medições com o VocalSet (02/11/2026)

**Escolha.** Medi as 20 vozes e escolhi 5 pelo resultado, usando os trechos cantados, sem a fala. O material é fixo por cantor, por isso as vozes de cada grupo têm medianas parecidas.

| Voz | Por que entrou | Mediana (5% a 95%) | Busca a partir de | YIN × outro estimador | Candidatas no material inteiro |
|---|---|---|---|---|---|
| female3 | Aguda, a maior mediana, com picos fortes | 395 Hz (265 a 581) | 2371 Hz | 99,9% | 3701 (+10,8), 7192 (+9,8), 2854 (+5,0) |
| female7 | Aguda, quase sem picos: contraste com a female3 | 387 Hz (268 a 572) | 2320 Hz | 99,9% | 3921 (+4,2) |
| male9 | Homem cantando na faixa das vozes femininas | 377 Hz (260 a 562) | 2263 Hz | 99,0% | 2617 (+7,5), 3596 (+5,1) |
| male8 | A voz mais grave | 174 Hz (127 a 262) | 1046 Hz | 98,0% (1,8% de oitava) | 6987 (+9,2), 2694 (+7,0), 2138 (+5,8) |
| male6 | Fundamental mais difícil: menor concordância | 196 Hz (136 a 313) | 1175 Hz | 95,4% | 3494 (+11,7), 6987 (+8,4), 1347 (+6,4) |

Também medi escalas e arpejos da female3 e do male8. Eles não subiram a mediana (357 e 175 Hz). **Nenhum material testado passou de cerca de 395 Hz de mediana**: vozes muito agudas continuam sem teste.

**Esperado e observado.**
- **Fundamental:** esperava-se cerca de uma oitava entre os grupos, o que apareceu. O YIN concorda com o outro estimador em 95% a 100% dos blocos nas 20 vozes. Não é a conferência no Praat (D77).
- **Aguda (95%):** em janelas de 30 s, chega a cerca do dobro em duas vozes (male8: 521 Hz; male6: 572 e 663 Hz). Pode ser registro agudo real ou erro de oitava; não foi confirmado. O grave (5%) não sofreu com isso.

**Estabilidade.** Os valores estão como 30 s / 45 s.

| Voz | Pior erro da mediana | Grave (5%) entre janelas | Espectro de 500 Hz a 2 kHz | Espectro de 2 a 10 kHz |
|---|---|---|---|---|
| female3 | 5,7% / 1,1% | 10,6% / 9,6% | 4,8 / 4,0 dB | 4,3 / 2,4 dB |
| female7 | 6,6% / 10,2% | 8,8% / 7,8% | 4,7 / 5,4 dB | 3,1 / 3,2 dB |
| male9 | 10,4% / 2,6% | 3,7% / 6,9% | 4,4 / 3,5 dB | 3,0 / 2,3 dB |
| male8 | 10,2% / 1,7% | 1,5% / 0,7% | 4,7 / 5,0 dB | 2,4 / 2,5 dB |
| male6 | 12,6% / 9,3% | 4,4% / 7,6% | 4,8 / 3,1 dB | 4,4 / 3,9 dB |

Essas vozes variam mais que a do usuário, provavelmente porque cada janela pega um trecho musical diferente. Com 45 s, a mediana melhora em 4 das 5 vozes, mas há só 2 ou 3 janelas. Isso não muda a D82, mas mostra que a estabilidade depende do que é cantado.

**Limite de 6 vezes a fundamental (D83).** Picos de +9 dB, Q 8, nas 5 vozes, sem contar trechos que já tinham candidata na frequência. Para comparar, a voz do usuário está na última linha.

| Variante | 1 kHz | 2,2 kHz | 3 kHz | 4 kHz | 6 kHz |
|---|---|---|---|---|---|
| Com o limite (plugin atual) | 0/19 | 5/15 | 6/14 | 4/18 | 3/19 |
| Sem o limite | 3/19 | 10/14 | 8/16 | 4/18 | 1/19 |
| Com o limite, sem o corte de 3 candidatas | 0/19 | 5/14 | 6/14 | 3/17 | 4/19 |
| Voz do usuário, com o limite | 6/6 | 5/6 | 3/6 | 5/5 | 2/6 |

- O limite custa acertos logo abaixo do ponto de início: em 2,2 kHz nas vozes agudas e em 1 kHz no male8.
- Ele também tira candidatas duvidosas: sem ele, surgem picos de 1,4 a 2 kHz nas vozes agudas, com 3 a 9 notas e mudando de lugar.
- **O problema maior não é o limite:** mesmo acima do início, os picos inseridos são pouco encontrados nessas vozes, e o corte de 3 candidatas não explica isso. A causa ainda é desconhecida.

**Picos fortes que se repetem entre cantores.** Nas 20 vozes, as candidatas chegam a +12 dB, contra +4 a +6 dB na voz do usuário. Há um pico de 6,2 a 7,4 kHz em 13 das 20 vozes, e picos de cerca de 2,1 a 4 kHz na maioria. A região de 2,5 a 3,5 kHz pode ser o formante do cantor, uma qualidade desejada; isso não foi confirmado. Candidata não significa defeito (D86).

**O pico de 6,2 a 7,4 kHz vem do equipamento ou da sala? (`origem`)** O artigo do VocalSet confirma a mesma cabine e o mesmo modelo de microfone: "Audio-Technica AT2020 condenser microphone" (seção 3.2), com os cantores perto dele. Não informa o pré-amplificador. Testes, considerando 5,5 a 8,5 kHz:

| Pergunta | Esperado se for o equipamento | Observado |
|---|---|---|
| O pico muda com a vogal? | Não | Muda: espalhamento mediano de 0,25 oitava, de 0 a 0,54 |
| O pico muda de pessoa para pessoa? | Quase não | Muda: de 5,6 a 8,2 kHz |
| Todos têm o pico? | Sim, com altura parecida | Não: de quase nada (female7, com até 2 dB) a 14 dB (male6, male10) |
| Aparece na respiração inspirada e nos silêncios, no mesmo lugar? | Sim | Não há um lugar comum |
| Homens e mulheres diferem? | Não | Medianas parecidas: 7066 e 6864 Hz. É o único ponto a favor |
| O analisador cria o pico? | — | Não parece: nas gravações do usuário, a mesma medida dá de 1,6 a 3,5 dB |

**Conclusão:** um pico fixo do equipamento ou da sala não explica o resultado sozinho. Pode haver uma contribuição do microfone na região, mas não dá para separar sem a curva dele, e a voz varia nesse pico. Investigação encerrada aqui, como pediu o usuário (D86). Os silêncios variam 30 dB entre cantores (-48 a -78 dB), o que indica ajustes de gravação ou edição diferentes, ainda que no mesmo lugar.

**Simulação da regra P45**, no `banco regra`, em janelas de 30 s; os valores da regra estão em `02-decisoes.md`.

| Material | Resultado |
|---|---|
| SM7dB | Nenhum corte (0 de 3 janelas) |
| SM57 | 4941 Hz, -2,2 dB, em 1 de 3 janelas |
| 20 vozes do VocalSet | Corta em 48 de 72 janelas; 16 das 20 vozes têm ao menos um corte; 4 nunca têm |
| Onde cortou no VocalSet | 23 cortes de 6 a 7,4 kHz; 12 de 3,6 a 4,2 kHz, logo acima da zona proibida; 13 em outros pontos |
| Pico de +9 dB, Q 8, no SM7dB / SM57 | 1 kHz: 2/3 e 2/3. 2,2 kHz: 2/3 e 1/3. 3 kHz: 0/3 e 0/3, como previsto. 4 kHz: 3/3 e 3/3. 6 kHz: 0/3 e 1/3 |

- A regra é segura na sua intensidade (no máximo -3 dB) e deixa a voz do usuário quase intacta.
- Ela ainda age muito em vozes treinadas, e os 12 cortes de 3,6 a 4,2 kHz podem atingir o formante em vozes mais agudas. A faixa do formante muda com o tipo de voz: daí a sugestão de ampliar a zona até cerca de 4,2 kHz.
- A frequência escolhida muda entre janelas em algumas vozes, como a female3 (3596 e 6987 Hz). Daí a sugestão de exigir o pico nas duas metades da captura. Essa exigência ainda não foi simulada.
- Nada disso está no plugin. A regra só existe na ferramenta de teste.

**Simulação da regra aprovada (D87).** Critério do usuário: as vozes do VocalSet são treinadas e gravadas em estúdio, então devem receber poucos cortes. **A regra aprovada não atende a esse critério.**

| Variante | Janelas com corte | Vozes com corte | Abaixo de 2,5 kHz | 2,5 a 4,2 kHz | 4,2 a 6 kHz | Acima de 6 kHz |
|---|---|---|---|---|---|---|
| Antes (zona até 3,5 kHz) | 48 de 72 | 16 de 20 | 9 | 12 | 4 | 23 |
| Só a zona até 4,2 kHz | 41 de 72 | 14 de 20 | 9 | 0 | 4 | 28 |
| Só as duas metades | 47 de 72 | 16 de 20 | 7 | 12 | 4 | 24 |
| **Regra aprovada (os dois ajustes)** | **40 de 72** | **14 de 20** | 7 | 0 | 4 | 29 |
| Aprovada + teto de 6 kHz (P46) | 17 de 72 | 8 de 20 | 10 | 0 | 7 | 0 |
| Aprovada + mínimo de +8 dB | 32 de 72 | 11 de 20 | 5 | 0 | 1 | 26 |
| Aprovada + teto de 6 kHz + mínimo de +8 dB | 6 de 72 | 4 de 20 | 5 | 0 | 1 | 0 |

"Metade" é a metade da voz válida: cerca de 15 s cada. O pico conta como presente se a metade tiver uma candidata a até 1/12 de oitava dele.

- Quando uma candidata fica de fora, a regra passa à seguinte elegível. Por isso, os cortes acima de 6 kHz aumentam com a zona maior: várias vozes trocam de 3,6 kHz para cerca de 7 kHz.
- A exigência das duas metades quase não muda o total: esses picos são mesmo persistentes.
- **Voz do usuário, com a regra aprovada:** SM7dB sem cortes; SM57 com -2,2 dB em 4941 Hz, em 1 de 3 janelas.
- **Pico de +9 dB, Q 8, com a regra aprovada, SM7dB / SM57:** 1 kHz: 2/3 e 2/3. 2,2 kHz: 2/3 e 1/3. 3 e 4 kHz: 0/3 e 0/3, por estarem na zona proibida. 6 kHz: 0/3 e 1/3.
- **Mínimo de +8 dB:** picos de +9 dB inseridos na voz do usuário mediram quase sempre menos de 8 dB, então a banda quase nunca agiria em um pico real desse tamanho.
- **Decisão do usuário (D88):** regra aprovada com teto de 6 kHz, sem o mínimo de +8 dB. Na simulação, 17 de 72 janelas e 8 de 20 vozes recebem corte. Ajuste encerrado para a v1.0; a validação final será de ouvido, na Fase 4.

## O que falta para fechar a Fase 3 (D77)

**Situação em 02/11/2026:** o item 1 foi concluído no Reaper. Por decisão do usuário (D89), os demais não serão feitos no Reaper: ver "Verificação sem o Reaper". Lista mantida como referência.

Fazer no Reaper, com o plugin reaberto:

1. ✅ Interface: acentos, casas decimais e as duas curvas.
2. SM57 do início, com o cursor no zero: cerca de 51 s de captura, 132 Hz, grave de 98 Hz, candidata em 4941 Hz (+6,5 dB).
3. Cancelamento com análise e RTA ligados: deve dar silêncio.
4. ReaEQ antes do plugin, com +9 dB estreitos em 2,2 kHz, no SM7dB: candidata perto de 2,2 kHz, medindo menos que 9 dB.
5. RTA: ligar uma banda do EQ muda o RTA, mas não as médias.
6. Parar antes de 10 s de voz: aparece o aviso.
7. Salvar e reabrir: medidas e curvas voltam.
8. Vozes female3, female7, male9, male8 e male6, com arquivos de `excerpts/straight`: fundamental no Praat dentro de ±3%. Se possível, ouvir male6 e male8 para saber se há trechos realmente agudos.
9. Durante os testes: nenhum estalo e CPU baixa.
10. De ouvido: comparar a região de 4,8 kHz entre os dois microfones.

## Medições com voz real (02/11/2026)

Para repetir, ver `tools/medidor-voz/README.md` (D84). Para comparar com o Praat, `tools/praat/README.md`; para conferir o plugin sem DAW, `tools/verifica-plugin/README.md`. A tabela de ressonâncias abaixo mostra os métodos comparados; a linha da P41 é o método atual do plugin (D83).

**Material:** duas tomadas diferentes da mesma música, não gravadas ao mesmo tempo (correlação da forma de onda 0,12), com 140 e 136 s. Por isso a comparação bloco a bloco da fundamental não vale como teste. O SM57 chega a 0 dB em 3 trechos curtos (35 amostras), o que não deve pesar na média. Voz do usuário: fundamental mediana perto de 136 Hz, notas de E2 a D#3.

**Estabilidade (A04).** Cada gravação foi dividida em janelas separadas, que não compartilham áudio; valores são a mediana dos pares, SM7dB / SM57. Com 45 s há só 1 par, e com 30 s, 3 pares.

| Voz válida | Captura real | Espectro 100–500 Hz | Espectro 500 Hz–2 kHz | Espectro 2–10 kHz | Mediana da f0, pior caso contra a gravação inteira | Percentil de 95%, pior caso contra a gravação inteira | Percentil de 5%, entre janelas |
|---|---|---|---|---|---|---|---|
| 10 s | 11 a 23 s | 7,1 / 5,6 dB | 3,9 / 3,7 dB | 2,4 / 2,8 dB | 5,2% / 5,0% | 1,8% / 2,0% | 13,9% / 11,6% |
| 20 s | 24 a 39 s | 5,0 / 3,8 dB | 2,9 / 3,4 dB | 1,6 / 2,2 dB | 4,2% / 3,8% | 0,9% / 0,9% | 11,5% / 16,9% |
| 30 s | 36 a 51 s | 5,6 / 3,7 dB | 1,9 / 2,0 dB | 1,6 / 1,9 dB | 3,2% / 3,0% | 0,4% / 0,7% | 10,9% / 12,5% |
| 45 s | 53 a 78 s | 4,7 / 3,1 dB | 2,7 / 2,6 dB | 1,6 / 2,3 dB | 3,0% / 2,4% | 0,4% / 0,2% | 9,5% / 11,7% |

- De 10 para 30 s há ganho claro; de 30 para 45 s, não, exceto um pequeno ganho na fundamental.
- Abaixo de 500 Hz e no percentil de 5%, mais tempo quase não ajuda: o resultado depende de quais notas foram cantadas. Isso importa para a D78 na Fase 4, porque a análise pode não ver as notas mais graves se elas só aparecerem em outra parte da música.

**Microfones (fluxo real de 30 s / gravação inteira).**
- Fundamental mediana: 132,8 × 132,2 Hz (0,5%) / 137,2 × 135,5 Hz (1,2%). Percentil de 95%: diferença de 1,2% / 0,9%. Percentil de 5%: 1,1% / 4,1%.
- Espectro: 5,7 dB rms de diferença na forma, nos dois casos. De 2 a 10 kHz, a diferença é de 7,7 dB, contra 1,6 a 1,9 dB entre janelas do mesmo microfone. O SM57 tem de 8 a 13 dB a mais de 5 a 12,5 kHz, comparado ao SM7dB. De 100 a 500 Hz, a diferença (4,9 dB) não se separa da variação entre janelas (3,7 a 5,6 dB).
- O YIN concordou com um segundo estimador, escrito só para esta medição, em 99,9% e 99,4% dos blocos, dentro de 3%. Não é o Praat.

**Ressonâncias (P40, D73 e P41).** Picos estreitos inseridos no áudio, em 3 janelas de 30 s por microfone; picos que já existiam não entram na conta. A D73 é uma reconstrução minha, porque o código original não ficou guardado.

| Método | Candidatas sem pico inserido | Repetem em outra janela | +9 dB, Q 8: 300 Hz | 500 Hz | 1 kHz | 2,2 kHz | 3 kHz | 4 kHz | 6 kHz |
|---|---|---|---|---|---|---|---|---|---|
| D73 (reconstruída) | 0 | — | 0/6 | 0/6 | 0/6 | 0/6 | 0/6 | 2/6 | 1/6 |
| P40 como está (3 dB) | 18 | 11 | 0/6 | 0/3 | 6/6 | 6/6 | 3/6 | 5/5 | 3/6 |
| 4 dB, sem limite pela f0 | 14 | 11 | 0/6 | 0/3 | 6/6 | 5/6 | 3/6 | 6/6 | 2/6 |
| 3 dB e acima de 6× a f0 | 18 | 8 | 0/6 | 0/6 | 6/6 | 6/6 | 3/6 | 4/4 | 3/6 |
| P41: 4 dB e acima de 6× a f0 | 9 | 5 | 0/6 | 0/6 | 6/6 | 5/6 | 3/6 | 5/5 | 2/6 |

- Sem pico inserido, a P41 acha cerca de 4,9 kHz em 5 das 6 janelas, nos dois microfones e em 10 a 12 notas: é o único achado que se repete de forma clara. Não sabemos se vem da voz, da sala ou dos microfones.
- As candidatas de 490 a 583 Hz têm só 3 a 5 notas e mudam de lugar entre as janelas. Podem ser o primeiro formante, mas isso é suposição.
- Picos em 3 kHz e 6 kHz continuam incertos, e nenhum método achou 300 Hz.
- **Risco a conferir em outras vozes:** o limite de 6 vezes a fundamental acompanha a voz. Numa voz com mediana de 400 Hz, por exemplo, a busca só começaria em 2,4 kHz. Essa conta ainda não foi testada com vozes agudas.
