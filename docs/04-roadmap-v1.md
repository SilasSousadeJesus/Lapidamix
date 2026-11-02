# 04 - Roadmap da v1.0

Cada fase só começa quando a anterior atingir o critério de "pronto" **e o usuário autorizar**. ✅ Roadmap confirmado (D36) e ajustado (D38). Todo módulo novo segue as 4 etapas da D37: estudo, proposta, implementação própria, teste no Reaper e ajuste.

| Fase | Nome | Status |
|---|---|---|
| 0 | Planejamento | ✅ Concluída (02/11/2026) |
| 1 | Ambiente e plugin "vazio" | ✅ Concluída (02/11/2026) |
| 2 | EQ manual | ✅ Concluída (02/11/2026) |
| 3 | Análise (só medir) | ⚪ Não iniciada |
| 4 | Correção automática | ⚪ Não iniciada |
| 5 | Testes com várias vozes | ⚪ Não iniciada |
| 6 | Fechamento da v1.0 | ⚪ Não iniciada |

## Fase 0 — Planejamento
- **Entregas:** documentação do projeto (feito); pasta do projeto com `CLAUDE.md` e `docs/` (feito); decisões em aberto resolvidas ou adiadas (`02-decisoes.md`): licença A02 (D42), segundos de análise A04 (D34) e nome A05 (D45) (feito).
- **Propostas essenciais (D31):** P01 (JUCE), P02 (VST3), P09 (separação tempo real/análise), P11 (Git) e P13 (núcleo comum + perfis + módulos). ✅ Todas confirmadas (D26 a D30).
- **Demais propostas:** P03 a P08, P12 e P14 serão decididas nas fases em que forem usadas; não impedem o fechamento da Fase 0.
- **Pronto quando:** propostas essenciais confirmadas (feito). A gravação da voz de teste passou para a Fase 3 (D33).

## Fase 1 — Ambiente e plugin "vazio"
- **Como trabalhar:** em etapas, com aprovação do usuário entre elas (D46). Etapa 1 (plano): aprovada (D47 a D53). Etapa 2 (instalar as ferramentas): concluída, com MSVC 14.44, Windows SDK 10.0.26100 e CMake 3.31.6. Etapa 3 (criar submódulo, pastas e plugin vazio): concluída, ainda sem compilar. Etapa 4 (compilar e testar no Reaper): concluída, com os 5 passos da D53 aprovados pelo usuário.
- **Entregas:** ferramentas instaladas no Windows; um plugin que só deixa o áudio passar, carregando no Reaper (D21); estrutura da arquitetura (núcleo comum, perfis e módulos, ainda vazios — D30); lista de instrumentos só com "Voz".
- **Licença:** AGPLv3, decidida antes do primeiro commit com código (D41, D42).
- **Pronto quando:** o plugin abre no Reaper e o áudio passa sem alterações nem estalos.

## Fase 2 — EQ manual
- **Andamento (D37):** estudo concluído (`07-referencias.md`); proposta aprovada (D59 a D66); implementação concluída e verificada automaticamente; teste no Reaper aprovado pelo usuário (D66).
- **Entregas:** etapa de estudo do EQ (D37); o EQ como primeiro módulo da arquitetura, com as bandas funcionando com controles manuais; parâmetros salvos com o projeto; bypass.
- **Material:** pode usar qualquer gravação de voz que o usuário já tenha.
- **Pronto quando:** dá para mixar a voz "na mão" com o plugin, respeitando a regra de ouro do tempo real.

## Fase 3 — Análise (só medir)
- **Entregas:** etapa de estudo da análise, por onde a fase começa (D37); gravação da voz de teste (voz seca no SM7dB, sem efeitos, ~30 s — D33); resposta à A04 (quantos segundos de análise são necessários — D34); botão Analisar; detecção dos trechos cantados; espectro médio; detecção da fundamental e das ressonâncias. O plugin **mostra** as medidas, ainda sem aplicar nada. 🟡 RTA simples como ferramenta de diagnóstico (P14).
- **Pronto quando:** as medidas batem com o que se ouve/vê em outras ferramentas, em 3 a 5 vozes bem diferentes (as 20 vozes ficam para a Fase 5).

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
