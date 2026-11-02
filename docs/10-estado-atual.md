# 10 - Estado Atual

**Leia este arquivo primeiro.** Depois, leia as últimas entradas de `06-diario.md` e as regras em `../AGENTS.md`. Atualize este arquivo a cada mudança relevante e antes de cada commit (D57).

**Última atualização:** 02/11/2026

## Onde estamos

- **Fase:** Fase 4 (Correção automática) concluída (D110). Fase 3 concluída, com exceções registradas (D90).
- **Etapa:** entre fases. A Fase 5 (Testes com várias vozes) ainda não começou.
- **Último passo concluído:** conferência do Ar e dos botões no Reaper e correção do texto do botão depois de Zerar (D109), sem falhas na verificação sem DAW.
- **Próximo passo:** o usuário autorizar a Fase 5.
## Bloqueios

Nenhum bloqueio técnico.

## Pendências

- **Banda de ressonância:** implementada (D87, D88). A validação de ouvido ficou só no A/B geral do SM57, sem observação específica; ouvir um corte na Fase 5. Melhorias na detecção estão em `05-ideias-futuro.md`.
- **SM57 no Reaper:** o primeiro teste não correspondeu ao início da gravação, e a causa ficou sem confirmação (D89). Sem DAW, o plugin dá o esperado.
- **Propostas a decidir nas fases em que forem usadas (D31):** P12.
- **Faixa do Ar:** -54 e -38 dB são provisórios, a calibrar em outras vozes na Fase 5 (D108). O SM57 ficou na fronteira, em -38,3 dB: observar na Fase 5. Falta ouvir a atenuação, o realce de +3 dB e a regra de chiado. Sem pausa na captura, chiado forte ainda é lido como ar (`05-ideias-futuro.md`).
- **Faixas provisórias:** no A/B, -5 dB no embolado não tirou corpo da voz do usuário; outras vozes graves seguem sem teste (D95).
- **Limites herdados da Fase 3:** estalos, CPU no DAW e teste de ouvido em 4,8 kHz foram dispensados (D89); a aguda (95%) errou no male6; gravações de conhecidos não foram obtidas (D79). As 5 vozes do VocalSet cobriram o critério de 3 a 5 vozes.
- **Depois da v1.0:** tratamento de plosivos e de sibilância registrados em `05-ideias-futuro.md`.
- **Antes de distribuir o plugin:** analisar as licenças de terceiros em `external/JUCE/JUCE.spdx.json`.

## Últimos commits relevantes

| Commit | O que fez |
|---|---|
| este commit | Conferência no Reaper, texto do botão depois de Zerar e fechamento da Fase 4 (D109, D110) |
| `56855ec` | `08-plano-testes.md` reorganizado por fase, com o status de cada uma e os registros da Fase 4 |
| `b64de41` | Fase 4: resultado do A/B, Ar automático, botões Reaplicar e Zerar e medidas atrás de Detalhes (D106 a D108) |
| `70a628d` | Fase 4, passo 5: explicações das correções, dicionário da voz e roteiro do A/B (D105) |
| `5a5b038` | Fase 4, passos 3 e 4: lista, Aplicar, intensidade e compensação em LUFS (D102 a D104) |
| `3162b6a` | Fase 4, passos 1 e 2: regras no código e dados novos na análise, ainda sem ligar ao som (D93 a D101) |
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
