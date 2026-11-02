# 06 - Diário de Andamento

Uma entrada por sessão de trabalho, a mais recente no topo.

**Modelo de entrada:**

```
## DD/MM/AAAA — Título curto
**Fase:** …
**O que foi feito:** …
**Decisões tomadas:** (referência ao 02, ex.: D17)
**Problemas / dúvidas:** …
**Próximos passos:** …
```

---

## 02/11/2026 — Esclarecimento sobre código de terceiros

**Fase:** 0 concluída; Fase 1 aguardando a autorização do usuário.

**O que foi feito:**
- Esclarecido que "não copiar código" vale para os plugins de referência (produtos fechados).
- Regra do código aberto de terceiros adicionada ao `CLAUDE.md` e ao `07-referencias.md`.

**Decisões tomadas:** D39.

**Problemas / dúvidas:** a D39 exige licença compatível, mas a licença do projeto ainda está adiada (A02, D22). Cada caso precisará avaliar isso antes da aprovação.

**Próximos passos:**
- A Fase 1 só começa quando o usuário autorizar.

---

## 02/11/2026 — Método de trabalho e ajustes no roadmap

**Fase:** 0 concluída; Fase 1 aguardando a autorização do usuário.

**O que foi feito:**
- Registrado o método de 4 etapas para cada módulo novo, com seus limites, em `02-decisoes.md` e como regra no `CLAUDE.md`.
- Roadmap ajustado nas Fases 1, 2 e 3, resolvendo os três pontos soltos da entrada anterior.
- `07-referencias.md`, `01-visao.md` e `08-plano-testes.md` alinhados às novas decisões; modelo de nota de estudo criado em `07-referencias.md`.

**Decisões tomadas:** D37 e D38.

**Problemas / dúvidas:** nenhum no momento.

**Próximos passos:**
- A Fase 1 só começa quando o usuário autorizar.

---

## 02/11/2026 — Fechamento da Fase 0

**Fase:** 0 — Planejamento (concluída nesta sessão)

**O que foi feito:**
- Gravação da voz de teste movida para as entregas da Fase 3.
- A04 adiada para a Fase 3; "Mixing Assistant" mantido como nome provisório até a abertura do projeto.
- `01-visao.md` passa a dizer "não destruir o timbre", com a voz como exemplo.
- `.gitignore` completado com pastas de compilação e mais formatos de áudio.
- Roadmap confirmado e status da Fase 0 atualizado para concluída.

**Decisões tomadas:** D33 a D36.

**Problemas / dúvidas:**
- A Fase 2 pede mixar uma voz "na mão", antes de a gravação de teste existir (D33); pode ser preciso usar outro áudio.
- A Fase 3 pede medidas "em vozes diferentes", mas só uma voz está prevista até lá.
- O modelo de arquitetura (D30) ainda não aparece nas entregas de nenhuma fase.

**Próximos passos:**
- A Fase 1 só começa quando o usuário autorizar.

---

## 02/11/2026 — Repositório Git e GitHub

**Fase:** 0 — Planejamento

**O que foi feito:**
- Repositório Git criado na pasta do projeto, com `.gitignore` para deixar os áudios de fora.
- Primeiro commit com o `CLAUDE.md` e a pasta `docs/`.
- GitHub CLI (`gh`) instalado no computador e login feito na conta `SilasSousadeJesus`.
- Repositório privado criado e enviado: https://github.com/SilasSousadeJesus/MixingAssistant

**Decisões tomadas:** D32 (repositório privado na conta `SilasSousadeJesus`).

**Problemas / dúvidas:** nenhum no momento. A branch se chama `master`, o padrão do Git neste computador.

**Próximos passos:**
- Gravar ~30 s de voz seca no SM7dB (sem efeitos) para testes.
- Responder ou adiar A04 e A05.
- Confirmar o roadmap.
- A Fase 1 só começa quando o usuário autorizar.

---

## 01/11/2026 — Confirmação das propostas essenciais

**Fase:** 0 — Planejamento

**O que foi feito:**
- Regra 3 do `CLAUDE.md` renomeada para "Não destruir o timbre", porque vale para qualquer instrumento.
- Confirmadas as cinco propostas essenciais da Fase 0 e atualizados os documentos afetados.
- Roadmap atualizado com as propostas essenciais e as que ficam para as fases seguintes.

**Decisões tomadas:** D26 a D31.

**Problemas / dúvidas:** o repositório Git foi criado (D29), mas ainda não tem nenhum commit. A04 e A05 seguem em aberto.

**Próximos passos:**
- Gravar ~30 s de voz seca no SM7dB (sem efeitos) para testes.
- Responder ou adiar A04 e A05.
- Fazer o primeiro commit da documentação.
- A Fase 1 só começa quando o usuário autorizar.

---

## 01/11/2026 — Identidade do projeto e revisão de pendências

**Fase:** 0 — Planejamento

**O que foi feito:**
- Leitura de toda a documentação e levantamento das pendências.
- Identidade reescrita: o projeto é um assistente de mixagem para qualquer instrumento, e a voz é só o primeiro.
- Registrados a ordem de instrumentos, o esquema de versões e o requisito de arquitetura.
- A01 respondida, A02 adiada, A03 movida para `05-ideias-futuro.md`, P10 confirmada e D06 substituída.
- Documentos ajustados para usar "v1.0" onde se referem à primeira versão, e termos novos adicionados ao glossário.

**Decisões tomadas:** D17 a D25. Propostas novas: P12 (v2 = violão), P13 (modelo de arquitetura) e P14 (RTA).

**Problemas / dúvidas:**
- P01 (JUCE) segue como proposta; a licença (A02) fica para o futuro e não impede o desenvolvimento (D22).
- A opção de código aberto do JUCE é a AGPLv3; anotado em `07-referencias.md` para quando a A02 for decidida.

**Próximos passos:**
- Revisar as propostas 🟡 restantes, incluindo P01 e P02, que definem as ferramentas da Fase 1.
- Responder A04 e A05.
- Gravar ~30 s de voz seca no SM7dB (sem efeitos) para testes.

---

## 01/11/2026 — Preparação para o Claude Code

**Fase:** 0 — Planejamento

**O que foi feito:**
- Documentos convertidos para Markdown e colocados em `docs/`.
- Criado o `CLAUDE.md` com as regras de trabalho e as regras técnicas do projeto.

**Decisões tomadas:** D16 (desenvolvimento com Claude Code).

**Problemas / dúvidas:** propostas P10 (docs como fonte da verdade) e P11 (Git) a confirmar.

**Próximos passos:**
- Revisar as propostas 🟡 e confirmar ou ajustar.
- Responder os itens em aberto (A01 a A05).
- Gravar ~30 s de voz seca no SM7dB (sem efeitos) para testes.

---

## 01/11/2026 — Concepção do projeto

**Fase:** 0 — Planejamento

**O que foi feito:**
- Discussão da ideia: plugin que mixa automaticamente, inicialmente pensado para o projeto inteiro.
- Definido começar por trilha, pela voz, e só com EQ.
- Discutidas referências (iZotope Nectar, Sonible smart:EQ, Neutron, Soothe2, Matchering).
- Criada a estrutura de documentação do projeto (documentos 00 a 09) no Google Drive.

**Decisões tomadas:** D01 a D15.

**Problemas / dúvidas:** itens em aberto A01 a A05 e propostas P01 a P09.
