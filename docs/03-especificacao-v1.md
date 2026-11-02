# 03 - Especificação da v1.0

**Resumo:** primeira versão do Lapidamix, o assistente de mixagem (D17, D19): plugin de insert para uma trilha de voz que analisa o áudio e aplica automaticamente um EQ corretivo, estático e suave, adaptado àquela voz. A voz é só o primeiro instrumento.

Legenda: ✅ Decidido · 🟡 Proposta · ❓ Em aberto

## 1. Fluxo de uso

1. O usuário coloca o plugin no insert da trilha de voz e escolhe o instrumento numa lista que só tem "Voz". ✅ (D30)
2. Aperta **"Analisar"** e dá play num trecho cantado.
3. O plugin escuta, analisa e calcula as correções.
4. O EQ é aplicado automaticamente.
5. O usuário compara com o **bypass** e, se quiser, ajusta a **intensidade** ou as bandas à mão. 🟡

## 2. Análise (o "cérebro")

Princípio ✅ (D14, D25): as regras são fixas, mas o resultado se adapta à voz/instrumento analisado, sem usar uma voz como molde.

- ✅ **Mede a entrada, antes do EQ**, numa thread separada; nesta fase só mostra os resultados (D69).
- ✅ **Duas classes de trecho:** voz válida e todo o som; silêncio é ignorado, e respiração e ruído ficam fora da voz válida (D70).
- ✅ **Duas médias do espectro**, feitas em potência: voz cantada (principal) e todo o som, incluindo consoantes como "s" (D71).
- ✅ **Fundamental:** mediana, percentis de 5% e 95% e confiança (D72). Na Fase 4, o passa-alta se baseia nas notas mais graves (D78).
- ✅ **Ressonâncias candidatas**, sempre relativas ao espectro da própria voz e presentes em notas diferentes, medidas nos harmônicos e só acima de 6 vezes a fundamental (D83).
- ✅ **Duração:** 30 s de voz válida ou até clicar Parar, com aviso abaixo de 10 s (D75, confirmada pelas medições na D82).
- 🟡 **Posicionar as correções** a partir da fundamental: "embolado" fica em lugares diferentes numa voz grave masculina e numa voz feminina aguda (Fase 4).
- 🟡 **Curva-alvo como faixa saudável:** o equilíbrio entre graves, médios e agudos deve ficar dentro de uma faixa. O plugin só corrige o que estiver fora, e só o quanto precisa.

## 3. Correção (o EQ)

- ✅ Corretivo (D05) e estático (D07).
- ✅ Sem cortes agressivos (D10).
- ✅ Filtros biquad em série, sem latência (D59), com frequência limitada a 45% da taxa de amostragem (D62) e ganho manual de ±12 dB (D63).
- ✅ 5 bandas, cada uma com valores iniciais do perfil de instrumento (D61, D62):
  - **Passa-alta** — remove graves inúteis, nunca acima da fundamental.
  - **Embolado** — reduz o acúmulo de graves/médio-graves (efeito de proximidade).
  - **Ressonância** — atenua um pico incômodo, se houver.
  - **Presença** — realce suave onde falta clareza (região de inteligibilidade).
  - **Ar** — realce suave dos agudos, se a voz estiver escura.
- 🟡 Limites da correção automática (Fase 4): cortes até ~-5 dB, realces até ~+3 dB, bandas largas (Q baixo).
- 🟡 Intensidade 0–100% escala todas as correções proporcionalmente.
- ✅ O plugin mostra as bandas que pretende usar antes de aplicá-las, com um botão Aplicar (D85).
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
- 🟡 Intensidade (Fase 4).
- 🟡 Os parâmetros devem ser salvos com o projeto do DAW e automatizáveis.

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
