# 02 - Registro de Decisões

Toda decisão nova entra aqui primeiro. Nunca apagar uma decisão: se ela mudar, marcar como "Substituída por Dxx" e criar uma nova. Só entra como ✅ o que o usuário confirmou.

## ✅ Decisões tomadas

| # | Data | Decisão | Motivo |
|---|---|---|---|
| D01 | 01/11/2026 | O produto é um **plugin de insert** (tipo Waves), aplicado trilha a trilha | Mais simples que mixar o projeto inteiro de uma vez |
| D02 | 01/11/2026 | **Mix em contexto** (todas as trilhas juntas) fica para depois. Não é impossível, só mais difícil | Começar pelo que dá para fazer bem |
| D03 | 01/11/2026 | Começar pela **voz** | É o meu caso de uso principal |
| D04 | 01/11/2026 | A v1 tem **só EQ**. Os outros processadores entram um por vez | Validar bem cada parte antes de crescer |
| D05 | 01/11/2026 | EQ **corretivo** (corrigir problemas), não criativo | Mais seguro de acertar automaticamente |
| D06 | 01/11/2026 | **Substituída por D25.** Correção baseada em **curva/regras fixas**, não em estilo musical nem em faixa de referência | Simplicidade na v1 |
| D07 | 01/11/2026 | EQ **estático** (não dinâmico) | Bem mais simples para começar |
| D08 | 01/11/2026 | Interface **bem simples** na v1, mas com todos os parâmetros ("conectores") já funcionando. Interface bonita só com o sistema rodando | Não gastar esforço visual antes da hora |
| D09 | 01/11/2026 | **Regra de ouro do tempo real:** nada dentro do `processBlock()` pode bloquear a thread de áudio (sem prints, sem alocação de memória, sem acesso a disco) | Evitar estalos e travamentos |
| D10 | 01/11/2026 | Objetivo sonoro: tirar embolamento, trazer clareza e presença, **sem cortes agressivos** | Problema real: voz grave escura/embolada no SM7dB |
| D11 | 01/11/2026 | Não haverá mixagem por estilo musical por enquanto | Foco na voz em si |
| D12 | 01/11/2026 | Uso pessoal primeiro; depois de testado, **projeto aberto** para todos | Sem objetivo comercial |
| D13 | 01/11/2026 | O foco é ser **bom e funcional**, não se diferenciar. Usar outros projetos como referência é bem-vindo | Não reinventar a roda |
| D14 | 01/11/2026 | O plugin **não usa uma voz como molde**: analisa a voz recebida e adapta a correção a ela | Precisa funcionar com qualquer voz |
| D15 | 01/11/2026 | Plataforma de desenvolvimento: **Windows** | Meu computador |
| D16 | 01/11/2026 | Desenvolvimento feito com o **Claude Code**, guiado pelo `CLAUDE.md` e pela pasta `docs/` | Ele trabalha direto nos arquivos e compila sozinho |
| D17 | 01/11/2026 | **Identidade:** o projeto não é um plugin de voz, é um **assistente de mixagem capaz de mixar qualquer instrumento**. A voz é só o primeiro instrumento | Questão de escopo; nenhuma sessão futura deve achar que o projeto é sobre vocal |
| D18 | 01/11/2026 | **Ordem de instrumentos:** voz primeiro; quando a cadeia da voz estiver completa, violão; depois contrabaixo, guitarra, bateria (cada peça) e assim por diante | Crescer um instrumento por vez |
| D19 | 01/11/2026 | **Versões:** a v1.0 é o EQ da voz. As versões 1.1, 1.2 etc. adicionam um processador por vez até fechar a cadeia de sinal da voz | Validar cada processador antes de adicionar o próximo |
| D20 | 01/11/2026 | **Requisito de arquitetura:** o projeto precisa de um padrão que permita adicionar instrumentos e processadores sem refazer o que existe. O modelo está na D30 | O projeto vai crescer por instrumento e por processador |
| D21 | 01/11/2026 | O DAW de testes é o **Reaper** (responde A01) | Escolha do usuário |
| D22 | 01/11/2026 | A escolha da licença (A02) **fica para o futuro e não impede o desenvolvimento** | Premissa real: não é um projeto comercial (D12) |
| D23 | 01/11/2026 | A03 (explicar cada ajuste) sai das perguntas em aberto e fica só em `05-ideias-futuro.md` | Fora do escopo atual |
| D24 | 01/11/2026 | A pasta `docs/` é a **fonte da verdade**; o Google Drive fica só com a versão original (confirma P10) | Evita duas versões divergentes |
| D25 | 01/11/2026 | **As regras são fixas, mas o resultado se adapta à voz/instrumento analisado.** Substitui D06 | Deixa claro que regras fixas não contradizem a D14. Estilos musicais seguem de fora (D11) e faixa de referência segue fora do escopo (`03-especificacao-v1.md`, seção 6) |
| D26 | 01/11/2026 | Usar **JUCE (C++)** como framework (confirma P01). A licença segue adiada (D22) | Padrão da indústria |
| D27 | 01/11/2026 | Formato inicial **VST3** (confirma P02) | AAX (Pro Tools) exige acordo com a Avid; fica para depois |
| D28 | 01/11/2026 | Separar o plugin em **parte de tempo real** (aplica o EQ) e **parte de análise** (fora da thread de áudio) (confirma P09) | Forma de cumprir a D09 |
| D29 | 01/11/2026 | Versionar o projeto com **Git** desde o início (confirma P11) | Histórico e possibilidade de desfazer mudanças |
| D30 | 01/11/2026 | **Modelo de arquitetura:** núcleo comum (análise, EQ, limites de segurança) + perfis de instrumento + processadores como módulos encaixáveis. Na v1 o usuário escolhe o instrumento numa lista que só tem "Voz" (confirma P13) | Forma de cumprir a D20 |
| D31 | 01/11/2026 | As **propostas essenciais da Fase 0** eram P01, P02, P09, P11 e P13. As demais (P03 a P08, P12, P14) serão decididas nas fases em que forem usadas | Define o critério de "pronto" da Fase 0 |
| D32 | 02/11/2026 | O repositório fica na conta do GitHub `SilasSousadeJesus`, como **privado** | O projeto só abre depois de testado (D12) |
| D33 | 02/11/2026 | A **gravação da voz de teste** sai da Fase 0 e entra nas entregas da Fase 3 | Ela só é usada na análise |
| D34 | 02/11/2026 | A04 (segundos de análise) fica **adiada para a Fase 3** | A fase em que a análise é construída |
| D35 | 02/11/2026 | **"Mixing Assistant" é o nome provisório**; o nome definitivo (A05) será decidido na abertura do projeto | Não precisa ser decidido agora |
| D36 | 02/11/2026 | **Roadmap da v1.0 confirmado** | Confirmado pelo usuário após a revisão das fases |
| D37 | 02/11/2026 | **Esclarecida pela D39 quanto a código.** **Método de trabalho:** antes de implementar cada módulo novo (processador, análise ou perfil de instrumento), seguir 4 etapas: **(a) Estudo:** pesquisar plugins de referência, manuais, documentação pública e princípios de DSP, e escrever uma nota curta em `07-referencias.md`, separando o que é fato do que é suposição sobre o funcionamento interno de produtos fechados. **(b) Proposta:** propor como será o nosso módulo com base no estudo e esperar a aprovação do usuário. **(c) Implementação própria**, dentro da arquitetura da D30. **(d) Teste no Reaper e ajuste.** **Limites:** estudar documentação pública e princípios é permitido; não copiar código, interface visual nem presets, não usar nomes de marcas no nosso produto e não desmontar plugins originais | Aprender com o que existe, mantendo uma implementação própria |
| D38 | 02/11/2026 | **Ajustes no roadmap.** Fase 1: inclui a estrutura da arquitetura (núcleo comum, perfis e módulos, ainda vazios) e a lista de instrumentos só com "Voz". Fase 2: o EQ entra como o primeiro módulo da arquitetura, começando pela etapa de estudo, e pode usar qualquer gravação de voz que o usuário já tenha. Fase 3: começa pela etapa de estudo da análise e usa de 3 a 5 vozes bem diferentes; as 20 vozes continuam na Fase 5 | Resolve os pontos soltos encontrados na revisão do roadmap e aplica a D37 |
| D39 | 02/11/2026 | **Esclarece a D37:** "não copiar código" vale para os plugins de referência (produtos fechados). **Código aberto de terceiros** (bibliotecas, exemplos) pode ser usado se a licença for compatível, com crédito ao autor; cada caso precisa da aprovação do usuário e de registro neste arquivo. O JUCE já está aprovado (D26) | Evita reescrever o que já existe em código aberto, mantendo controle sobre cada uso |

## 🟡 Propostas aguardando confirmação

Serão decididas nas fases em que forem usadas (D31).

| # | Proposta | Observação |
|---|---|---|
| P03 | Limites de segurança: cortes de no máximo ~-5 dB, realces de no máximo ~+3 dB, bandas largas (Q baixo) | Valores a calibrar nos testes |
| P04 | **Proteger a fundamental**: detectar a nota grave da voz e nunca colocar o passa-alta acima dela | Preserva o peso de vozes graves |
| P05 | Analisar **só os trechos cantados** (ignorar silêncio, respiração, ruído) | Evita distorcer a média |
| P06 | Controle de **intensidade** (0–100%) e **bypass** | Ajuste fino e comparação antes/depois |
| P07 | Até **5 bandas**: passa-alta, embolado, ressonância, presença, ar | Pode mudar após os testes |
| P08 | Curva-alvo como **faixa saudável de equilíbrio**, não como linha exata | Forma de cumprir a D14 |
| P12 | **Numeração seguinte:** v2 = violão | Continua a lógica da D19, seguindo a ordem da D18 |
| P14 | **RTA:** um RTA simples na Fase 3 como ferramenta de diagnóstico, e o RTA bonito na interface final | Ajuda a conferir as medidas da análise |

## Propostas encerradas

| # | Proposta | Resultado |
|---|---|---|
| P01 | Usar **JUCE (C++)** como framework | Confirmada: ver D26 |
| P02 | Formato inicial **VST3** | Confirmada: ver D27 |
| P09 | Separar o plugin em **parte de tempo real** (aplica o EQ) e **parte de análise** (fora da thread de áudio) | Confirmada: ver D28 |
| P10 | A pasta `docs/` passa a ser a **fonte da verdade**; o Google Drive fica só com a versão original | Confirmada: ver D24 |
| P11 | Versionar o projeto com **Git** desde o início | Confirmada: ver D29 |
| P13 | **Modelo de arquitetura:** núcleo comum (análise, EQ, limites de segurança) + perfis de instrumento + processadores como módulos encaixáveis. Na v1 o usuário escolhe o instrumento numa lista que só tem "Voz" | Confirmada: ver D30 |

## ❓ Em aberto

| # | Pergunta |
|---|---|
| A02 | Qual licença de código aberto usar? **Adiada (D22):** precisa ser decidida no futuro, mas não impede o desenvolvimento |
| A04 | Quantos segundos de análise são necessários? **Adiada para a Fase 3 (D34)** |
| A05 | Nome definitivo do plugin. **"Mixing Assistant" é provisório; decidir na abertura do projeto (D35)** |

## Perguntas encerradas

| # | Pergunta | Resultado |
|---|---|---|
| A01 | Qual DAW será usado nos testes principais? | Reaper: ver D21 |
| A03 | O plugin deve explicar cada ajuste que fez (ex.: "cortei 3 dB em 250 Hz porque…")? | Movida para `05-ideias-futuro.md`: ver D23 |
