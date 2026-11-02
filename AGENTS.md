# AGENTS.md — Lapidamix

Este arquivo orienta as IAs que trabalham neste projeto. Leia-o inteiro antes de qualquer tarefa.

## O projeto em uma frase

**Lapidamix** é um **assistente de mixagem capaz de mixar qualquer instrumento**, em forma de plugin de insert para DAWs. **Não é um plugin de voz:** a voz é só o primeiro instrumento, por questão de escopo (D17, D18).

A primeira versão (v1.0) analisa uma trilha de voz e aplica automaticamente um **EQ corretivo, estático e suave**, adaptado àquela voz. O repositório é público desde 02/11/2026 (D40); o plugin é para uso pessoal primeiro e, depois de testado, para qualquer pessoa usar.

Detalhes: `docs/01-visao.md` e `docs/03-especificacao-v1.md`.

## Início e fim de cada sessão (D55)

- **Ao iniciar:** ler `docs/10-estado-atual.md` e as últimas entradas de `docs/06-diario.md`.
- **Durante e ao terminar:** manter os dois atualizados, conforme as regras de manutenção abaixo (D57).

## Fase atual

**Fase 3 — Análise (só medir): concluída**, com exceções registradas (D90). **A Fase 4 não foi iniciada** e só começa quando eu pedir. Nada corrige o som sozinho até lá. Licença do projeto: AGPLv3 (D42). Veja `docs/04-roadmap-v1.md`.

## Como trabalhar comigo (o usuário)

- Comunicação **em português**, de forma simples. Estou aprendendo desenvolvimento de áudio: explique o porquê das coisas, sem jargão desnecessário (use/atualize `docs/09-glossario.md`).
- **Uma coisa por vez.** Não adiantar fases, não adicionar processadores ou funcionalidades fora do escopo atual, mesmo que pareçam úteis. Ideias novas vão para `docs/05-ideias-futuro.md`.
- **Não tome decisões de projeto sozinho.** Se algo não está decidido, pergunte ou registre como proposta. Nunca transforme uma sugestão sua em decisão.
- **Não comece a implementar sem eu pedir.** Planejar e propor é bem-vindo; executar exige meu "pode fazer".
- **Método aprovado que não funciona (D81):** se uma verificação mostrar que um método aprovado não funciona, pare, explique e proponha a mudança antes de implementar.
- Mudanças pequenas e verificáveis. Depois de cada mudança, diga como eu testo.

## Regras técnicas inegociáveis

1. **Regra de ouro do tempo real:** nada dentro de `processBlock()` pode bloquear a thread de áudio.
   - Sem alocação de memória (`new`, `std::vector::push_back`, `resize`, criação de `juce::String`, coeficientes que alocam etc.).
   - Sem locks/mutex, sem prints/logs, sem acesso a disco ou rede, sem chamadas ao sistema.
   - Toda alocação acontece em `prepareToPlay()` ou no construtor.
   - Comunicação entre threads via atômicos ou filas lock-free.
2. **Separação em duas partes (D28):** a parte de tempo real só aplica o EQ; a análise roda fora da thread de áudio e entrega os valores prontos de forma segura.
3. **Não destruir o timbre:** correções suaves e com limites; na dúvida, corrigir menos.
4. **Adaptável:** nada de valores calibrados para uma voz ou um instrumento específico. As regras são fixas, mas o resultado depende da análise do áudio recebido (D25).
5. Plataforma: **Windows**. DAW de testes: **Reaper** (D21). Framework: **JUCE/C++** (D26), como submódulo na versão 9.0.4 (D49). Formato inicial: **VST3** (D27). Compilação: Visual Studio 2022 e CMake (D47, D48). O CMake vem com o Visual Studio e não está no PATH: `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`. Não compilar na pasta temporária do Windows. Comandos: `cmake -S . -B build -G "Visual Studio 17 2022" -A x64` e `cmake --build build --config Release --target Lapidamix_VST3 --parallel`.
6. **Crescer sem refazer:** a arquitetura precisa permitir adicionar instrumentos e processadores sem refazer o que existe (D20). Modelo: núcleo comum + perfis de instrumento + processadores como módulos encaixáveis (D30).

## Método para cada módulo novo (D37)

Antes de implementar um módulo novo (processador, análise ou perfil de instrumento), seguir estas 4 etapas, nesta ordem:

1. **Estudo:** pesquisar plugins de referência, manuais, documentação pública e princípios de DSP, e escrever uma nota curta em `docs/07-referencias.md`, separando o que é fato do que é suposição sobre o funcionamento interno de produtos fechados.
2. **Proposta:** propor como será o nosso módulo com base no estudo e **esperar minha aprovação**.
3. **Implementação própria**, dentro da arquitetura da D30.
4. **Teste no Reaper e ajuste.**

**Limites:** estudar documentação pública e princípios é permitido. Não copiar código de plugins de referência (produtos fechados), interface visual nem presets, não usar nomes de marcas no nosso produto e não desmontar plugins originais.

**Código aberto de terceiros (D39):** bibliotecas e exemplos podem ser usados se a licença for compatível com a AGPLv3 (D42), com crédito ao autor. Cada caso precisa da minha aprovação e de registro em `docs/02-decisoes.md`. O JUCE já está aprovado (D26).

## Ferramentas de teste (D84)

Guardar medidores e scripts de teste em `tools/`, nunca como programas temporários fora do repositório, para repetir as medições sempre que o método mudar. Sem áudio: as gravações ficam em `test-audio/`, fora do Git. Depois de mudar um método de análise, repetir a medição e registrar o resultado em `docs/08-plano-testes.md`.

## Documentação (fonte da verdade)

A pasta `docs/` é a **fonte da verdade** do projeto (✅ D24). O Google Drive guarda apenas a versão original.

| Arquivo | Conteúdo |
|---|---|
| `docs/00-indice.md` | Índice, status geral, convenções |
| `docs/01-visao.md` | Problema, objetivo, princípios |
| `docs/02-decisoes.md` | Decisões (✅), propostas (🟡) e itens em aberto (❓) |
| `docs/03-especificacao-v1.md` | O que a v1.0 faz |
| `docs/04-roadmap-v1.md` | Fases da v1.0 e critérios de "pronto" |
| `docs/05-ideias-futuro.md` | Tudo fora do escopo atual |
| `docs/06-diario.md` | Registro de cada sessão de trabalho |
| `docs/07-referencias.md` | Plugins e projetos de referência |
| `docs/08-plano-testes.md` | Como validar o plugin |
| `docs/09-glossario.md` | Termos técnicos explicados |
| `docs/10-estado-atual.md` | Fase e etapa atuais, próximo passo e pendências. É o primeiro arquivo a ser lido |
| `docs/11-bancos-de-vozes.md` | Bancos de vozes para teste, com licenças e créditos. Áudios em `test-audio/`, fora do Git |

### Manutenção dos documentos

**Tudo o que for feito no projeto precisa ser refletido nos documentos, para que eles evoluam junto com o trabalho (D57):**

- **A cada mudança relevante:** atualizar `docs/10-estado-atual.md`, sem esperar o fim da sessão.
- **Antes de cada commit:** o diário (entrada da sessão) e o estado atual devem estar atualizados com o que o commit leva. Onde o hash ainda não existir, escrever "este commit" e trocar pelo hash real na próxima mudança, sem criar commit só para isso (D68).
- **No mesmo commit:** atualizar qualquer outro documento afetado pela mudança (decisões, roadmap, especificação, glossário etc.).

- **Decisão nova:** registrar primeiro em `docs/02-decisoes.md` (próximo número Dxx, com data e motivo), depois ajustar os documentos afetados. Só registre como decisão (✅) o que eu confirmei.
- **Diário e status:** manter uma entrada da sessão no topo de `docs/06-diario.md`, seguindo o modelo do arquivo e citando os commits da sessão, e atualizar o status em `docs/00-indice.md` e `docs/04-roadmap-v1.md` se algo mudou.
- Legenda: ✅ Decidido · 🟡 Proposta · ❓ Em aberto. Datas em DD/MM/AAAA.
- Nunca apagar decisões antigas: marcar como "Substituída por Dxx".
- As regras ficam só neste arquivo (D56). O `CLAUDE.md` apenas importa este arquivo; não acrescentar regras lá.

## Convenções de código (✅ D54)

- Nomes de código (classes, funções, variáveis) em inglês; comentários e documentação em português.
- Commits pequenos, mensagens em português descrevendo o que mudou e citando fase/etapa e as decisões relacionadas (D55).
