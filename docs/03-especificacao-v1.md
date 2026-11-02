# 03 - Especificação da v1.0

**Resumo:** primeira versão do assistente de mixagem (D17, D19): plugin de insert para uma trilha de voz que analisa o áudio e aplica automaticamente um EQ corretivo, estático e suave, adaptado àquela voz. A voz é só o primeiro instrumento.

Legenda: ✅ Decidido · 🟡 Proposta · ❓ Em aberto

## 1. Fluxo de uso

1. O usuário coloca o plugin no insert da trilha de voz e escolhe o instrumento numa lista que só tem "Voz". ✅ (D30)
2. Aperta **"Analisar"** e dá play num trecho cantado.
3. O plugin escuta, analisa e calcula as correções.
4. O EQ é aplicado automaticamente.
5. O usuário compara com o **bypass** e, se quiser, ajusta a **intensidade** ou as bandas à mão. 🟡

## 2. Análise (o "cérebro")

Princípio ✅ (D14, D25): as regras são fixas, mas o resultado se adapta à voz/instrumento analisado, sem usar uma voz como molde.

- 🟡 **Só trechos cantados:** ignorar silêncio, respiração e ruído.
- 🟡 **Espectro médio** da voz: quanto de energia há em cada faixa de frequência.
- 🟡 **Detectar a fundamental** (a nota grave característica de quem canta) e posicionar as correções a partir dela. "Embolado" fica em lugares diferentes numa voz grave masculina e numa voz feminina aguda.
- 🟡 **Ressonâncias** detectadas de forma relativa: picos que se destacam dos vizinhos no espectro da própria voz.
- 🟡 **Curva-alvo como faixa saudável:** o equilíbrio entre graves, médios e agudos deve ficar dentro de uma faixa. O plugin só corrige o que estiver fora, e só o quanto precisa.

## 3. Correção (o EQ)

- ✅ Corretivo (D05) e estático (D07).
- ✅ Sem cortes agressivos (D10).
- 🟡 Até 5 bandas:
  - **Passa-alta** — remove graves inúteis, nunca acima da fundamental.
  - **Embolado** — reduz o acúmulo de graves/médio-graves (efeito de proximidade).
  - **Ressonância** — atenua um pico incômodo, se houver.
  - **Presença** — realce suave onde falta clareza (região de inteligibilidade).
  - **Ar** — realce suave dos agudos, se a voz estiver escura.
- 🟡 Limites: cortes até ~-5 dB, realces até ~+3 dB, bandas largas (Q baixo).
- 🟡 Intensidade 0–100% escala todas as correções proporcionalmente.

## 4. Interface (v1)

- ✅ Bem simples, mas com todos os parâmetros já conectados (D08), para a interface bonita ser só "trocar a casca" depois.
- ✅ Lista de instrumento, só com "Voz" (D30).
- 🟡 Outros elementos mínimos: botão Analisar, indicador de status (analisando / pronto), intensidade, bypass, e os controles de cada banda (frequência, ganho, Q, liga/desliga).
- 🟡 RTA simples como ferramenta de diagnóstico, a partir da Fase 3; o RTA bonito fica para a interface final (P14).
- 🟡 Os parâmetros devem ser salvos com o projeto do DAW e automatizáveis.

## 5. Arquitetura técnica

- ✅ **Regra de ouro do tempo real (D09):** nada no `processBlock()` pode bloquear a thread de áudio — sem prints, sem alocação de memória (ex.: `std::vector::push_back`), sem acesso a disco. Detalhes em `CLAUDE.md`.
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
