# 00 - Índice do Projeto

**Lapidamix** é um assistente de mixagem capaz de mixar qualquer instrumento, em forma de plugin de insert para DAWs. A voz é só o primeiro instrumento, por questão de escopo (D17): a v1.0 é o EQ corretivo da voz, e o projeto cresce um processador e um instrumento por vez (D18, D19).

**Status atual:** Fase 1 concluída: o plugin vazio carrega no Reaper e deixa o áudio passar sem alterações. Fase 2 (EQ manual) concluída; Fase 3 (Análise) aguardando a autorização do usuário. Repositório público desde 02/11/2026 (D40); licença AGPLv3 (D42).
**Última atualização:** 02/11/2026

## Fonte da verdade

✅ A pasta `docs/` deste repositório é a versão oficial da documentação (D24). A pasta "Mixing Assistant" no Google Drive guarda apenas a versão original de 01/11/2026.

## Documentos

**Comece por `10-estado-atual.md`** (D55).

| Arquivo | Para que serve | Quando atualizar |
|---|---|---|
| `00-indice.md` | Porta de entrada: lista os documentos e o status geral | Sempre que mudar a fase do projeto |
| `01-visao.md` | O porquê do projeto, objetivos e princípios | Raramente; só se a ideia central mudar |
| `02-decisoes.md` | Todas as decisões tomadas, com data e motivo | A cada decisão nova (é o primeiro lugar a atualizar) |
| `03-especificacao-v1.md` | O que a v1.0 faz, em detalhe | Quando uma decisão afetar a v1 |
| `04-roadmap-v1.md` | Fases até a v1.0 pronta, com entregas e critérios | Ao concluir ou replanejar uma fase |
| `05-ideias-futuro.md` | Tudo que fica para depois da v1.0 | Sempre que surgir uma ideia fora do escopo atual |
| `06-diario.md` | O que foi feito em cada sessão de trabalho | Antes de cada commit (D57) |
| `07-referencias.md` | Plugins, projetos e materiais usados como inspiração | Ao estudar uma nova referência |
| `08-plano-testes.md` | Como validar se o plugin funciona e soa bem | Ao definir testes e ao registrar resultados |
| `09-glossario.md` | Termos técnicos explicados de forma simples | Sempre que surgir um termo novo |
| `10-estado-atual.md` | Fase e etapa atuais, próximo passo, bloqueios, pendências e últimos commits. É o primeiro arquivo a ser lido | A cada mudança relevante e antes de cada commit (D57) |

As regras de trabalho para as IAs estão em `AGENTS.md`, importado pelo `CLAUDE.md` (D56), e a apresentação do projeto está em `README.md`, todos na raiz do projeto.

## Convenções

- **✅ Decidido** — combinado e fechado.
- **🟡 Proposta** — sugestão ainda não confirmada.
- **❓ Em aberto** — precisa de resposta antes de avançar.
- Datas no formato DD/MM/AAAA.
- Fluxo para uma decisão nova: registrar primeiro em `02-decisoes.md`, depois ajustar os documentos afetados (03, 04, 05) e anotar em `06-diario.md`.
