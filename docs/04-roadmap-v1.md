# 04 - Roadmap da v1.0

Cada fase só começa quando a anterior atingir o critério de "pronto" **e o usuário autorizar**. 🟡 Roadmap proposto, a confirmar.

| Fase | Nome | Status |
|---|---|---|
| 0 | Planejamento | 🔵 Em andamento |
| 1 | Ambiente e plugin "vazio" | ⚪ Não iniciada |
| 2 | EQ manual | ⚪ Não iniciada |
| 3 | Análise (só medir) | ⚪ Não iniciada |
| 4 | Correção automática | ⚪ Não iniciada |
| 5 | Testes com várias vozes | ⚪ Não iniciada |
| 6 | Fechamento da v1.0 | ⚪ Não iniciada |

## Fase 0 — Planejamento
- **Entregas:** documentação do projeto (feito); pasta do projeto com `CLAUDE.md` e `docs/` (feito); decisões em aberto resolvidas (`02-decisoes.md`), exceto a licença (A02), que foi adiada e não impede o desenvolvimento (D22); gravações de teste coletadas (voz seca, sem efeitos, ~30 s).
- **Propostas essenciais (D31):** P01 (JUCE), P02 (VST3), P09 (separação tempo real/análise), P11 (Git) e P13 (núcleo comum + perfis + módulos). ✅ Todas confirmadas (D26 a D30).
- **Demais propostas:** P03 a P08, P12 e P14 serão decididas nas fases em que forem usadas; não impedem o fechamento da Fase 0.
- **Pronto quando:** propostas essenciais confirmadas (feito) e material de teste disponível.

## Fase 1 — Ambiente e plugin "vazio"
- **Entregas:** ferramentas instaladas no Windows; um plugin que só deixa o áudio passar, carregando no Reaper (D21).
- **Pronto quando:** o plugin abre no Reaper e o áudio passa sem alterações nem estalos.

## Fase 2 — EQ manual
- **Entregas:** as bandas de EQ funcionando com controles manuais; parâmetros salvos com o projeto; bypass.
- **Pronto quando:** dá para mixar a voz "na mão" com o plugin, respeitando a regra de ouro do tempo real.

## Fase 3 — Análise (só medir)
- **Entregas:** botão Analisar; detecção dos trechos cantados; espectro médio; detecção da fundamental e das ressonâncias. O plugin **mostra** as medidas, ainda sem aplicar nada. 🟡 RTA simples como ferramenta de diagnóstico (P14).
- **Pronto quando:** as medidas batem com o que se ouve/vê em outras ferramentas, em vozes diferentes.

## Fase 4 — Correção automática
- **Entregas:** regras que transformam as medidas em bandas de EQ; limites de segurança; intensidade.
- **Pronto quando:** na minha voz, o resultado soa melhor que o original no teste A/B.

## Fase 5 — Testes com várias vozes
- **Entregas:** testes com vozes graves, agudas, masculinas, femininas e microfones diferentes; ajuste fino das regras.
- **Pronto quando:** os critérios de `08-plano-testes.md` forem atendidos.

## Fase 6 — Fechamento da v1.0
- **Entregas:** correções de bugs; estabilidade; guia de uso simples.
- **Pronto quando:** atender aos critérios de "v1.0 pronta" de `03-especificacao-v1.md`.

## Depois da v1.0
- **v1.1, v1.2 etc.:** um processador por vez, até fechar a cadeia de sinal da voz (D19).
- **Instrumentos seguintes:** violão, contrabaixo, guitarra, bateria (cada peça) e assim por diante (D18). 🟡 A numeração seguinte seria v2 = violão (P12).
- Interface bonita e abertura do projeto — ver `05-ideias-futuro.md`.
