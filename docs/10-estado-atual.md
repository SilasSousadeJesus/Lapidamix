# 10 - Estado Atual

**Leia este arquivo primeiro.** Depois, leia as últimas entradas de `06-diario.md` e as regras em `../AGENTS.md`. Atualize este arquivo a cada mudança relevante e antes de cada commit (D57).

**Última atualização:** 02/11/2026

## Onde estamos

- **Fase:** Fase 3 (Análise, só medir), em andamento (D67).
- **Etapa:** teste com voz real (etapa d da D37).
- **Último passo concluído:** ajuste da regra da banda de ressonância encerrado para a v1.0 (D87, D88), com teto de 6 kHz: na simulação, corta em 17 de 72 janelas do VocalSet. Ainda não está no plugin.
- **Testes no Reaper:** interface e SM7dB informados e registrados. Os outros itens ainda não têm resultado, então a fase não foi fechada.
- **Próximo passo:** o usuário informar os resultados dos itens 2 a 10 da lista em `08-plano-testes.md`, para reavaliar o fechamento. A Fase 4 não começa antes do pedido do usuário.

## Bloqueios

Nenhum bloqueio técnico. O fechamento da Fase 3 depende do restante do teste no Reaper (D77).

## Pendências

- **Banda de ressonância:** regra decidida (D87, D88), a implementar na Fase 4, com validação final de ouvido em A/B contra o bypass (D85). Melhorias na detecção estão em `05-ideias-futuro.md`.
- **SM57 no Reaper:** o resultado não corresponde ao início da gravação (parece vir de perto de 90 s). Repetir.
- **Propostas a decidir nas fases em que forem usadas (D31):** P03, P06, P08, P12 e P42.
- **Margem do passa-alta (P42):** o percentil de 5% variou cerca de 11% a 13% entre trechos de 30 s da música. Opções registradas; decidir na Fase 4.
- **Fase 3:** restante do teste no Reaper com a voz gravada (`test-audio/voz-usuario/`) e as 5 vozes do VocalSet (`11-bancos-de-vozes.md`). Gravações de conhecidos ainda a obter (D79).
- **Antes de distribuir o plugin:** analisar as licenças de terceiros em `external/JUCE/JUCE.spdx.json`.

## Últimos commits relevantes

| Commit | O que fez |
|---|---|
| este commit | Resultados parciais do segundo teste no Reaper, sem fechar a Fase 3 |
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
