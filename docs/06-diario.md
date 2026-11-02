# 06 - Diário de Andamento

Uma entrada por sessão de trabalho, a mais recente no topo.

**Modelo de entrada:**

```
## DD/MM/AAAA — Título curto
**Fase:** …
**O que foi feito:** …
**Decisões tomadas:** (referência ao 02, ex.: D17)
**Commits:** (hashes dos commits da sessão; o commit que traz a própria entrada é citado pela mensagem)
**Problemas / dúvidas:** …
**Próximos passos:** …
```

---

## 02/11/2026 — Contexto vivo do projeto

**Fase:** 1 concluída; Fase 2 aguardando a autorização do usuário.

**O que foi feito:**
- Convenções de código confirmadas.
- Criado `docs/10-estado-atual.md` como primeiro arquivo a ser lido, com as regras de início e fim de sessão no `CLAUDE.md`.
- Modelo do diário passa a citar os commits; mensagens de commit passam a citar fase/etapa e decisões.
- Proposto o `AGENTS.md` (P22), ainda não criado.

**Decisões tomadas:** D54 e D55. Proposta nova: P22.

**Commits:** `9228abe` (conclusão da Fase 1) e "Fase 1: adiciona o contexto vivo do projeto (D54, D55)", que traz esta entrada.

**Problemas / dúvidas:** um commit não pode citar o próprio hash, então o commit que traz a entrada é citado pela mensagem.

**Próximos passos:**
- Decidir a P22.
- A Fase 2 só começa quando o usuário autorizar.

---

## 02/11/2026 — Fechamento da Fase 1

**Fase:** 1 — Ambiente e plugin "vazio" (concluída nesta sessão)

**O que foi feito:**
- O usuário fez os 5 passos do teste no Reaper (D53), e todos passaram.
- Resultado registrado em `08-plano-testes.md` e status atualizado.

**Decisões tomadas:** nenhuma nova.

**Problemas / dúvidas:**
- O código, o submódulo e os documentos desta fase ainda não têm commit.
- As convenções de código do `CLAUDE.md` continuam como proposta, a confirmar.

**Próximos passos:**
- Confirmar as convenções de código.
- A Fase 2 só começa quando o usuário autorizar, pela etapa de estudo do EQ (D37, D38).

---

## 02/11/2026 — Plugin vazio compilado

**Fase:** 1 — Ambiente e plugin "vazio" (Etapa 4 em andamento)

**O que foi feito:**
- Plugin compilado em Release com 0 erros e 0 avisos, e copiado para `C:\Users\USER\AppData\Local\Programs\Common\VST3`.
- Maior caminho gerado na compilação: 179 caracteres, sem necessidade de mover o projeto nem ativar caminhos longos.

**Decisões tomadas:** nenhuma nova.

**Problemas / dúvidas:** o teste no Reaper ainda não foi feito; a Fase 1 só fica pronta depois dele. Nada desta fase tem commit.

**Próximos passos:**
- O usuário faz os 5 passos do teste no Reaper (D53) e informa o resultado.

---

## 02/11/2026 — Plugin vazio criado

**Fase:** 1 — Ambiente e plugin "vazio" (Etapa 3 concluída)

**O que foi feito:**
- Medido o caminho do projeto: 54 caracteres. Pior caso dentro do JUCE: 202 caracteres; arquivos de compilação ainda não medidos.
- JUCE 9.0.4 adicionado como submódulo em `external/JUCE`.
- Criados o `CMakeLists.txt`, as pastas da D50 e o plugin vazio em `src/plugin/`, com a lista de instrumentos só com "Voz".
- Crédito ao JUCE adicionado ao `README.md`.

**Decisões tomadas:** nenhuma nova.

**Problemas / dúvidas:** o código ainda não foi compilado; isso é a Etapa 4. Nada desta etapa tem commit.

**Próximos passos:**
- Aprovação da Etapa 4 (compilar e testar no Reaper).

---

## 02/11/2026 — Ferramentas de compilação conferidas

**Fase:** 1 — Ambiente e plugin "vazio" (Etapa 2 concluída)

**O que foi feito:**
- Usuário instalou a carga de C++ no Visual Studio 2022.
- Conferidos: MSVC 14.44.35207 (compilador 19.44), Windows SDK 10.0.26100.0 e CMake 3.31.6-msvc6, sem necessidade de instalar o CMake separado (D48).
- Programa de teste em C++17 compilado com CMake e executado com sucesso, fora do projeto; os arquivos de teste foram removidos.

**Decisões tomadas:** nenhuma nova.

**Problemas / dúvidas:** a primeira tentativa falhou ao compilar dentro da pasta temporária do Windows, em um caminho longo; o mesmo teste funcionou em um caminho curto, fora dela. Causa exata não confirmada, mas não afeta a pasta do projeto.

**Próximos passos:**
- Aprovação da Etapa 3 (submódulo do JUCE, pastas e plugin vazio).

---

## 02/11/2026 — Plano da Fase 1 aprovado

**Fase:** 1 — Ambiente e plugin "vazio" (Etapa 1 concluída)

**O que foi feito:**
- Plano aprovado, com fabricante "Lapidamix" e uso do CMake da carga de C++.
- Conferido que o JUCE 9.0.4 mantém a opção de licença AGPLv3.
- Conferido que a carga de C++ lista as "Ferramentas do CMake" como componente recomendado.

**Decisões tomadas:** D47 a D53.

**Problemas / dúvidas:** a versão do CMake dessa carga não foi confirmada por nenhuma fonte; será conferida depois da instalação. Nada foi instalado.

**Próximos passos:**
- Aprovação da Etapa 2 (instalar a carga de C++ e conferir a versão do CMake).

---

## 02/11/2026 — Início da Fase 1: plano

**Fase:** 1 — Ambiente e plugin "vazio" (Etapa 1)

**O que foi feito:**
- Fase 1 autorizada e status atualizado no roadmap, no índice e no `CLAUDE.md`.
- Levantamento, sem alterar nada: faltam as ferramentas de C++ do Visual Studio, o Windows SDK e o CMake; Reaper 7.57 e Git já instalados.
- Plano apresentado e escolhas técnicas registradas como propostas.

**Decisões tomadas:** D46. Propostas novas: P15 a P21.

**Problemas / dúvidas:** o usuário não usa o terminal como administrador, o que influenciou a proposta da pasta de instalação (P20).

**Próximos passos:**
- Aprovação do plano pelo usuário. Nada será instalado nem criado antes disso.

---

## 02/11/2026 — Nome definitivo: Lapidamix

**Fase:** 0 concluída; Fase 1 aguardando a autorização do usuário.

**O que foi feito:**
- Prumix descartado pela semelhança com o "ProMix Bundle".
- Busca complementar por Lapidamix e nomes parecidos, sem conflito com a D43.
- Nome Lapidamix aplicado aos documentos, ao `CLAUDE.md` e ao `README.md`. O histórico de decisões e do diário ficou como estava.
- Repositório renomeado no GitHub pelo usuário e endereço atualizado no computador: https://github.com/SilasSousadeJesus/Lapidamix

**Decisões tomadas:** D45 (substitui D44).

**Problemas / dúvidas:** a pasta do Google Drive continua se chamando "Mixing Assistant".

**Próximos passos:**
- A Fase 1 só começa quando o usuário autorizar.

---

## 02/11/2026 — Nome definitivo: Prumix

**Fase:** 0 concluída; Fase 1 aguardando a autorização do usuário.

**O que foi feito:**
- Nome Prumix registrado, A05 encerrada e documentos atualizados. O histórico de decisões e do diário ficou como estava.
- Criado o `README.md`.

**Decisões tomadas:** D44.

**Problemas / dúvidas:**
- A verificação complementar encontrou nomes de áudio parecidos, como "ProMix Bundle" e "pMix"; ver D44.
- A pasta do Google Drive continua se chamando "Mixing Assistant".

**Próximos passos:**
- O usuário vai renomear o repositório no GitHub para Prumix; depois, atualizar o endereço no computador.
- A Fase 1 só começa quando o usuário autorizar.

---

## 02/11/2026 — Prazo e critério do nome

**Fase:** 0 concluída; Fase 1 aguardando a autorização do usuário.

**O que foi feito:**
- Definido quando e como escolher o nome definitivo, resolvendo a dúvida da entrada anterior; incluído nas entregas da Fase 6.

**Decisões tomadas:** D43 (substitui D35).

**Problemas / dúvidas:** nenhum no momento.

**Próximos passos:**
- A Fase 1 só começa quando o usuário autorizar.

---

## 02/11/2026 — Escolha da licença

**Fase:** 0 concluída; Fase 1 aguardando a autorização do usuário.

**O que foi feito:**
- AGPLv3 registrada como licença do projeto, e A02 encerrada.
- Arquivo `LICENSE` adicionado, com o texto da AGPLv3.
- Documentos ajustados, inclusive a regra de código de terceiros, que agora exige compatibilidade com a AGPLv3.

**Decisões tomadas:** D42.

**Problemas / dúvidas:**
- Segue a dúvida sobre o momento de decidir o nome (A05, D35).

**Próximos passos:**
- A Fase 1 só começa quando o usuário autorizar.

---

## 02/11/2026 — Repositório público e prazo da licença

**Fase:** 0 concluída; Fase 1 aguardando a autorização do usuário.

**O que foi feito:**
- Registrado que o repositório é público desde 02/11/2026; visibilidade conferida no GitHub.
- A licença (A02) ganhou prazo: antes do primeiro commit com código da Fase 1.

**Decisões tomadas:** D40 (substitui D32) e D41 (substitui D22).

**Problemas / dúvidas:**
- O repositório é público, mas ainda não tem licença.
- A05 (nome) estava ligada à "abertura do projeto" (D35), que agora foi antecipada em parte; falta definir se isso muda o momento de decidir o nome.

**Próximos passos:**
- Decidir a licença (A02).
- A Fase 1 só começa quando o usuário autorizar.

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
