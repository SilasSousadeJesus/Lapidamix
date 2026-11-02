# 10 - Estado Atual

**Leia este arquivo primeiro.** Depois, leia as últimas entradas de `06-diario.md` e as regras em `../AGENTS.md`. Atualize este arquivo a cada mudança relevante e antes de cada commit (D57).

**Última atualização:** 02/11/2026

## Onde estamos

- **Fase:** Fase 3 (Análise, só medir) concluída, com exceções registradas (D90). Fase 4 não iniciada.
- **Último passo concluído:** explicação simples das correções decidida para a Fase 4 (D92), o que encerra a A03. A Fase 3 está fechada, com 39 de 42 medidas da fundamental dentro de ±3% do Praat e a verificação sem DAW sem falhas (`08-plano-testes.md`).
- **Próximo passo:** o usuário autorizar o início da Fase 4. Nada dela deve começar antes disso.

## Bloqueios

Nenhum bloqueio técnico.

## Pendências

- **Banda de ressonância:** regra decidida (D87, D88), a implementar na Fase 4, com validação final de ouvido em A/B contra o bypass (D85). Melhorias na detecção estão em `05-ideias-futuro.md`.
- **SM57 no Reaper:** o primeiro teste não correspondeu ao início da gravação, e a causa ficou sem confirmação (D89). Sem DAW, o plugin dá o esperado.
- **Propostas a decidir nas fases em que forem usadas (D31):** P03, P06, P08, P12 e P42.
- **Passa-alta na Fase 4:** decidir a margem de segurança (P42) e os detalhes da proteção contra erro de oitava (D91).
- **Limites herdados da Fase 3:** estalos, CPU no DAW e teste de ouvido em 4,8 kHz foram dispensados (D89); a aguda (95%) errou no male6; gravações de conhecidos não foram obtidas (D79). As 5 vozes do VocalSet cobriram o critério de 3 a 5 vozes.
- **Depois da v1.0:** tratamento de plosivos e de sibilância registrados em `05-ideias-futuro.md`.
- **Antes de distribuir o plugin:** analisar as licenças de terceiros em `external/JUCE/JUCE.spdx.json`.

## Últimos commits relevantes

| Commit | O que fez |
|---|---|
| este commit | Explicação simples das correções decidida para a Fase 4 e A03 encerrada (D92) |
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
