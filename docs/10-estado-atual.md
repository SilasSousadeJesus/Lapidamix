# 10 - Estado Atual

**Leia este arquivo primeiro.** Depois, leia as últimas entradas de `06-diario.md` e as regras em `../AGENTS.md`. Atualize este arquivo a cada mudança relevante e antes de cada commit (D57).

**Última atualização:** 02/11/2026

## Onde estamos

- **Fase:** Fase 4 (Correção automática), em andamento (D93). Fase 3 concluída, com exceções registradas (D90).
- **Etapa:** implementação (etapa c da D37), passos 1 e 2 de 6 da D99 concluídos.
- **Último passo concluído:** a análise calcula as duas metades e os saltos de oitava, guarda até 20 candidatas e salva tudo no projeto. As regras ainda não ligam ao som nem à janela. Verificação sem falhas (`08-plano-testes.md`).
- **Próximo passo:** passo 3, lista de bandas e botão Aplicar. Abaixo de 10 s de voz, sem propor bandas (D101). A/B no Reaper, pelo usuário, no fim da fase.

## Bloqueios

Nenhum bloqueio técnico.

## Pendências

- **Banda de ressonância:** regra decidida (D87, D88), a implementar na Fase 4, com validação final de ouvido em A/B contra o bypass (D85). Melhorias na detecção estão em `05-ideias-futuro.md`.
- **SM57 no Reaper:** o primeiro teste não correspondeu ao início da gravação, e a causa ficou sem confirmação (D89). Sem DAW, o plugin dá o esperado.
- **Propostas a decidir nas fases em que forem usadas (D31):** P12.
- **Faixas provisórias:** o embolado pode ser cortado demais em vozes graves (D95). Calibrar no A/B.
- **Limites herdados da Fase 3:** estalos, CPU no DAW e teste de ouvido em 4,8 kHz foram dispensados (D89); a aguda (95%) errou no male6; gravações de conhecidos não foram obtidas (D79). As 5 vozes do VocalSet cobriram o critério de 3 a 5 vozes.
- **Depois da v1.0:** tratamento de plosivos e de sibilância registrados em `05-ideias-futuro.md`.
- **Antes de distribuir o plugin:** analisar as licenças de terceiros em `external/JUCE/JUCE.spdx.json`.

## Últimos commits relevantes

| Commit | O que fez |
|---|---|
| este commit | Fase 4, passos 1 e 2: regras no código e dados novos na análise, ainda sem ligar ao som (D93 a D101) |
| `0671ca7` | Explicação simples das correções decidida para a Fase 4 e A03 encerrada (D92) |
| `a9b79c0` | Fechamento da Fase 3, comparação com o Praat e verificação sem DAW (D89 a D91) |
| `691ae5e` | Resultados parciais do segundo teste no Reaper, sem fechar a Fase 3 |
| `139cd83` | Ajustes da análise com voz real, correções na interface, ferramentas de teste e regras para a Fase 4 (D81 a D88) |
| `ecae02e` | Análise e RTA da Fase 3, ainda sem o teste com voz real, e registro do VocalSet (D67 a D80) |
| `cbf6f27` | Correção do diário e do estado atual da Fase 2 (D57) |
| `e4adbce` | EQ manual de 5 bandas e conclusão da Fase 2 (D58 a D66) |
| `b0548ff` | Documentos evoluem junto com o trabalho (D57) |
| `48724f0` | Regras movidas para o `AGENTS.md` (D56) |
| `a7d04e9` | Contexto vivo do projeto (D54, D55) |
| `9228abe` | Plugin vazio, JUCE 9.0.4 como submódulo e conclusão da Fase 1 |
| `9b8b9cc` | Nome definitivo Lapidamix e README |
| `ccf7d79` | Licença AGPLv3 e arquivo `LICENSE` |
