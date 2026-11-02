# 03 - Especificação da v1.0

**Resumo:** primeira versão do Lapidamix, o assistente de mixagem (D17, D19): plugin de insert para uma trilha de voz que analisa o áudio e aplica automaticamente um EQ corretivo, estático e suave, adaptado àquela voz. A voz é só o primeiro instrumento.

Legenda: ✅ Decidido · 🟡 Proposta · ❓ Em aberto

## 1. Fluxo de uso

1. O usuário coloca o plugin no insert da trilha de voz e escolhe o instrumento numa lista que só tem "Voz". ✅ (D30)
2. Aperta **"Analisar"** e dá play num trecho cantado.
3. O plugin escuta, analisa e calcula as correções.
4. O plugin mostra as bandas que pretende usar, com a explicação de cada uma, e o usuário clica em **Aplicar**. ✅ (D85, D92, D99)
5. O usuário compara com o **bypass**, com o volume compensado por um valor fixo no controle Saída, e, se quiser, ajusta a **intensidade**, a Saída ou as bandas à mão. ✅ (D98, D99, D102)

## 2. Análise (o "cérebro")

Princípio ✅ (D14, D25): as regras são fixas, mas o resultado se adapta à voz/instrumento analisado, sem usar uma voz como molde.

- ✅ **Mede a entrada, antes do EQ**, numa thread separada; nesta fase só mostra os resultados (D69).
- ✅ **Duas classes de trecho:** voz válida e todo o som; silêncio é ignorado, e respiração e ruído ficam fora da voz válida (D70).
- ✅ **Duas médias do espectro**, feitas em potência: voz cantada (principal) e todo o som, incluindo consoantes como "s" (D71).
- ✅ **Fundamental:** mediana, percentis de 5% e 95% e confiança (D72). Na Fase 4, o passa-alta se baseia nas notas mais graves (D78).
- ✅ **Ressonâncias candidatas**, sempre relativas ao espectro da própria voz e presentes em notas diferentes, medidas nos harmônicos e só acima de 6 vezes a fundamental (D83).
- ✅ **Duração:** 30 s de voz válida ou até clicar Parar (D75, confirmada pelas medições na D82). Abaixo de 10 s, mostra as medidas e um aviso com a solução, sem propor bandas (D101).
- ✅ **Posicionar as correções** a partir da fundamental: a região do embolado começa acima das notas cantadas, e a banda não age se não sobrar região (D95).
- ✅ **Curva-alvo como faixa saudável:** o plugin só corrige o que estiver fora da faixa, e só o quanto precisa, com ação mínima de 1 dB (D98). As faixas são provisórias, a calibrar no teste de ouvido.

## 3. Correção (o EQ)

- ✅ Corretivo (D05) e estático (D07).
- ✅ Sem cortes agressivos (D10).
- ✅ Filtros biquad em série, sem latência (D59), com frequência limitada a 45% da taxa de amostragem (D62) e ganho manual de ±12 dB (D63).
- ✅ 5 bandas, cada uma com valores iniciais do perfil de instrumento (D61, D62):
  - **Passa-alta** — remove graves inúteis, meia oitava abaixo do grave (5%), entre 40 e 100 Hz. Se a medida do grave parecer inconsistente, fica em 40 Hz (D91, D94).
  - **Embolado** — reduz o acúmulo de graves/médio-graves: até -5 dB, com Q 1,0 (D95).
  - **Ressonância** — atenua um pico, só se cumprir a regra abaixo (D87, D88).
  - **Presença** — realce suave onde falta clareza: até +3 dB em 3,5 kHz, com Q 0,7 (D96).
  - **Ar** — mede 10 a 14 kHz contra os médios, sem o ruído das pausas; realça abaixo de -54 dB e atenua acima de -38 dB, até +3 e -5 dB. Com chiado alto, realça a metade e avisa; com o ar a menos de 1 dB do ruído, não age e avisa. Faixa provisória, a calibrar na Fase 5 (D108).
- ✅ Limites da correção automática: cortes até -5 dB, realces até +3 dB (D98).
- ✅ Intensidade 0–100% escala os ganhos das correções (D98). No passa-alta, 0% desliga a banda, e a frequência sobe de 40 Hz até o valor calculado, em proporção de oitavas (D100).
- ✅ O plugin mostra as bandas que pretende usar antes de aplicá-las, com um botão Aplicar (D85), e explica cada uma em texto simples: o que será feito, por que e o efeito, inclusive quando a banda não age (D92).
- ✅ Uma ressonância candidata não basta para justificar um corte (D86). A banda de ressonância é conservadora e limitada de propósito (D87, D88):
  - **Corta só:** pico de pelo menos +6 dB, estreito e persistente, com corte de até -3 dB.
  - **Atua:** de 6 vezes a fundamental até 2,5 kHz, e de 4,2 a 6 kHz.
  - **Não atua:** abaixo de 6 vezes a fundamental (largura do pico não mensurável), de 2,5 a 4,2 kHz (proteção do formante do cantor) nem acima de 6 kHz (origem desconhecida do pico perto de 7 kHz).
  - **Validação final:** de ouvido, na Fase 4, com A/B contra o bypass.

## 4. Interface (v1)

- ✅ Bem simples, mas com todos os parâmetros já conectados (D08), para a interface bonita ser só "trocar a casca" depois.
- ✅ Lista de instrumento, só com "Voz" (D30).
- ✅ Bypass, ganho de saída de ±12 dB e os controles de cada banda: liga/desliga, frequência, ganho e Q (D65). A passa-alta não tem ganho e tem Q fixo (D62).
- ✅ Botão Analisar/Parar, indicador de status e resultados em texto (D69, D75).
- ✅ Gráfico com o RTA simples e as duas médias; o RTA bonito fica para a interface final (D71, D74).
- ✅ Lista de bandas pretendidas, explicação em texto, botão Aplicar e intensidade (Fase 4; D85, D92, D98, D99). Aplicar vira "Reaplicar análise"; botão Zerar; medidas atrás do botão Detalhes (D106, D107).
- 🟡 Os parâmetros devem ser salvos com o projeto do DAW e automatizáveis. ✅ Exceção: a intensidade não é automatizável (D104).

## 5. Arquitetura técnica

- ✅ **Regra de ouro do tempo real (D09):** nada no `processBlock()` pode bloquear a thread de áudio — sem prints, sem alocação de memória (ex.: `std::vector::push_back`), sem acesso a disco. Detalhes em `AGENTS.md`.
- ✅ Duas partes separadas (D28):
  - **Tempo real:** só aplica o EQ, de forma rápida e previsível.
  - **Análise:** roda fora da thread de áudio, calcula as bandas e entrega os valores prontos de forma segura.
- ✅ **Crescer sem refazer (D20):** o padrão do projeto precisa permitir adicionar instrumentos e processadores sem refazer o que existe.
- ✅ Modelo (D30): núcleo comum (análise, EQ, limites de segurança) + perfis de instrumento + processadores como módulos encaixáveis.
- ✅ Framework: JUCE (C++) (D26). Formato inicial: VST3 (D27). Plataforma: Windows (D15). DAW de testes: Reaper (D21).

## 6. Fora do escopo da v1.0

Compressor, de-esser, EQ dinâmico, reverb, outros instrumentos, mix em contexto, estilos musicais, faixa de referência, interface elaborada, AAX/Pro Tools, Mac. Ver `05-ideias-futuro.md`.

## 7. Critérios de "v1.0 pronta" 🟡

- Funciona de forma estável no Reaper, sem estalos.
- Na minha voz (SM7dB), remove o embolamento e traz clareza sem soar "fina" ou artificial.
- Funciona razoavelmente bem com outras vozes de teste (ver `08-plano-testes.md`).
- Ajustes salvos e recuperados corretamente com o projeto.
