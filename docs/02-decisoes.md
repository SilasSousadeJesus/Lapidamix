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
| D12 | 01/11/2026 | **Parte de "aberto" antecipada pela D40.** Uso pessoal primeiro; depois de testado, **projeto aberto** para todos | Sem objetivo comercial |
| D13 | 01/11/2026 | O foco é ser **bom e funcional**, não se diferenciar. Usar outros projetos como referência é bem-vindo | Não reinventar a roda |
| D14 | 01/11/2026 | O plugin **não usa uma voz como molde**: analisa a voz recebida e adapta a correção a ela | Precisa funcionar com qualquer voz |
| D15 | 01/11/2026 | Plataforma de desenvolvimento: **Windows** | Meu computador |
| D16 | 01/11/2026 | Desenvolvimento feito com o **Claude Code**, guiado pelo `CLAUDE.md` e pela pasta `docs/` | Ele trabalha direto nos arquivos e compila sozinho |
| D17 | 01/11/2026 | **Identidade:** o projeto não é um plugin de voz, é um **assistente de mixagem capaz de mixar qualquer instrumento**. A voz é só o primeiro instrumento | Questão de escopo; nenhuma sessão futura deve achar que o projeto é sobre vocal |
| D18 | 01/11/2026 | **Ordem de instrumentos:** voz primeiro; quando a cadeia da voz estiver completa, violão; depois contrabaixo, guitarra, bateria (cada peça) e assim por diante | Crescer um instrumento por vez |
| D19 | 01/11/2026 | **Versões:** a v1.0 é o EQ da voz. As versões 1.1, 1.2 etc. adicionam um processador por vez até fechar a cadeia de sinal da voz | Validar cada processador antes de adicionar o próximo |
| D20 | 01/11/2026 | **Requisito de arquitetura:** o projeto precisa de um padrão que permita adicionar instrumentos e processadores sem refazer o que existe. O modelo está na D30 | O projeto vai crescer por instrumento e por processador |
| D21 | 01/11/2026 | O DAW de testes é o **Reaper** (responde A01) | Escolha do usuário |
| D22 | 01/11/2026 | **Substituída pela D41.** A escolha da licença (A02) **fica para o futuro e não impede o desenvolvimento** | Premissa real: não é um projeto comercial (D12) |
| D23 | 01/11/2026 | A03 (explicar cada ajuste) sai das perguntas em aberto e fica só em `05-ideias-futuro.md` | Fora do escopo atual. **Substituída pela D92** |
| D24 | 01/11/2026 | A pasta `docs/` é a **fonte da verdade**; o Google Drive fica só com a versão original (confirma P10) | Evita duas versões divergentes |
| D25 | 01/11/2026 | **As regras são fixas, mas o resultado se adapta à voz/instrumento analisado.** Substitui D06 | Deixa claro que regras fixas não contradizem a D14. Estilos musicais seguem de fora (D11) e faixa de referência segue fora do escopo (`03-especificacao-v1.md`, seção 6) |
| D26 | 01/11/2026 | Usar **JUCE (C++)** como framework (confirma P01). A licença segue adiada (D22) | Padrão da indústria |
| D27 | 01/11/2026 | Formato inicial **VST3** (confirma P02) | AAX (Pro Tools) exige acordo com a Avid; fica para depois |
| D28 | 01/11/2026 | Separar o plugin em **parte de tempo real** (aplica o EQ) e **parte de análise** (fora da thread de áudio) (confirma P09) | Forma de cumprir a D09 |
| D29 | 01/11/2026 | Versionar o projeto com **Git** desde o início (confirma P11) | Histórico e possibilidade de desfazer mudanças |
| D30 | 01/11/2026 | **Modelo de arquitetura:** núcleo comum (análise, EQ, limites de segurança) + perfis de instrumento + processadores como módulos encaixáveis. Na v1 o usuário escolhe o instrumento numa lista que só tem "Voz" (confirma P13) | Forma de cumprir a D20 |
| D31 | 01/11/2026 | As **propostas essenciais da Fase 0** eram P01, P02, P09, P11 e P13. As demais (P03 a P08, P12, P14) serão decididas nas fases em que forem usadas | Define o critério de "pronto" da Fase 0 |
| D32 | 02/11/2026 | **Substituída pela D40.** O repositório fica na conta do GitHub `SilasSousadeJesus`, como **privado** | O projeto só abre depois de testado (D12) |
| D33 | 02/11/2026 | A **gravação da voz de teste** sai da Fase 0 e entra nas entregas da Fase 3 | Ela só é usada na análise |
| D34 | 02/11/2026 | A04 (segundos de análise) fica **adiada para a Fase 3** | A fase em que a análise é construída |
| D35 | 02/11/2026 | **Substituída pela D43.** **"Mixing Assistant" é o nome provisório**; o nome definitivo (A05) será decidido na abertura do projeto | Não precisa ser decidido agora |
| D36 | 02/11/2026 | **Roadmap da v1.0 confirmado** | Confirmado pelo usuário após a revisão das fases |
| D37 | 02/11/2026 | **Esclarecida pela D39 quanto a código.** **Método de trabalho:** antes de implementar cada módulo novo (processador, análise ou perfil de instrumento), seguir 4 etapas: **(a) Estudo:** pesquisar plugins de referência, manuais, documentação pública e princípios de DSP, e escrever uma nota curta em `07-referencias.md`, separando o que é fato do que é suposição sobre o funcionamento interno de produtos fechados. **(b) Proposta:** propor como será o nosso módulo com base no estudo e esperar a aprovação do usuário. **(c) Implementação própria**, dentro da arquitetura da D30. **(d) Teste no Reaper e ajuste.** **Limites:** estudar documentação pública e princípios é permitido; não copiar código, interface visual nem presets, não usar nomes de marcas no nosso produto e não desmontar plugins originais | Aprender com o que existe, mantendo uma implementação própria |
| D38 | 02/11/2026 | **Ajustes no roadmap.** Fase 1: inclui a estrutura da arquitetura (núcleo comum, perfis e módulos, ainda vazios) e a lista de instrumentos só com "Voz". Fase 2: o EQ entra como o primeiro módulo da arquitetura, começando pela etapa de estudo, e pode usar qualquer gravação de voz que o usuário já tenha. Fase 3: começa pela etapa de estudo da análise e usa de 3 a 5 vozes bem diferentes; as 20 vozes continuam na Fase 5 | Resolve os pontos soltos encontrados na revisão do roadmap e aplica a D37 |
| D39 | 02/11/2026 | **Esclarece a D37:** "não copiar código" vale para os plugins de referência (produtos fechados). **Código aberto de terceiros** (bibliotecas, exemplos) pode ser usado se a licença for compatível, com crédito ao autor; cada caso precisa da aprovação do usuário e de registro neste arquivo. O JUCE já está aprovado (D26) | Evita reescrever o que já existe em código aberto, mantendo controle sobre cada uso |
| D40 | 02/11/2026 | O repositório no GitHub (https://github.com/SilasSousadeJesus/MixingAssistant) é **público desde 02/11/2026**. Isso antecipa a parte de "aberto" da D12. Substitui a D32 | Decisão do usuário |
| D41 | 02/11/2026 | A licença (A02) **precisa ser decidida antes do primeiro commit com código da Fase 1**. Substitui a D22 | O repositório já é público (D40); a licença deixa de ser adiada sem prazo |
| D42 | 02/11/2026 | A licença do projeto é a **AGPLv3** (responde A02) | Escolha do usuário. É o caminho de código aberto do JUCE (D26) e combina com um projeto aberto e não comercial |
| D43 | 02/11/2026 | **"Mixing Assistant" continua como nome provisório.** O nome definitivo (A05) deve ser decidido **antes do lançamento da v1.0** e **não pode lembrar nomes de produtos existentes** (ex.: "Mix Assistant", da iZotope). Substitui a D35 | A abertura do projeto foi antecipada (D40), e o nome precisa ser próprio |
| D44 | 02/11/2026 | **Substituída pela D45.** O nome definitivo do projeto é **Prumix** (prumo + mix: a mix "no prumo", alinhada e equilibrada). Substitui o provisório "Mixing Assistant" e encerra a A05. **Busca feita pelo usuário:** não há produto de áudio com esse nome; o mais parecido é "Primix", fabricante de misturadores industriais, de outra categoria. **Nomes descartados:** "Mixo" (já existe um app de DJ) e "MixCraft" (já existe a DAW da Acoustica) | Nome próprio, de acordo com a D43. **Verificação complementar (Claude, 02/11/2026):** também não encontrou produto chamado exatamente "Prumix", mas encontrou nomes de áudio parecidos: "ProMix Bundle" (AyaicWare) e "pMix" (Oli Larkin) |
| D45 | 02/11/2026 | O nome definitivo do projeto é **Lapidamix** (lapidar + mix: revelar o brilho que já existe no som, sem destruir o timbre). Substitui a D44 (Prumix), descartada pela semelhança com o "ProMix Bundle", da mesma categoria | Nome próprio, de acordo com a D43. **Busca complementar (Claude, 02/11/2026):** não encontrou produto nem repositório do GitHub com esse nome; os nomes de áudio mais próximos, "LAMA Mix" (Ross Video/HHB) e "LiquidMix", têm semelhança fraca. Não foi uma pesquisa de marca registrada |
| D46 | 02/11/2026 | **Início da Fase 1 autorizado**, em etapas, com aprovação do usuário entre elas. Etapa 1: plano, sem instalar nada nem criar código | Autorização do usuário; mudanças pequenas e verificáveis |
| D47 | 02/11/2026 | **Compilador:** Visual Studio Community 2022 com a carga "Desenvolvimento para desktop com C++" (MSVC e Windows SDK) (confirma P15) | O VS 2022 já está instalado e é estável, ao contrário da VS 2026 Insiders |
| D48 | 02/11/2026 | **Compilação com CMake**, gerando projeto do Visual Studio 2022, **usando o CMake que vem com a carga de C++**, desde que seja 3.22 ou superior; nesse caso, não instalar o CMake separado (confirma P16, com ajuste) | Evita uma instalação a mais. A carga lista as "Ferramentas do CMake" como recomendadas. Conferido depois da instalação (02/11/2026): CMake 3.31.6-msvc6 |
| D49 | 02/11/2026 | **JUCE como submódulo Git** em `external/JUCE`, fixado na versão 9.0.4 (confirma P17) | Versão exata registrada, sem copiar o código do JUCE. O `LICENSE.md` da versão 9.0.4 mantém a opção AGPLv3 (conferido em 02/11/2026) |
| D50 | 02/11/2026 | **Estrutura de pastas:** `src/plugin/`, `src/core/` (`analysis/`, `eq/`, `safety/`), `src/profiles/`, `src/modules/` e `external/` (confirma P18) | Forma de seguir a D30 e a D28 |
| D51 | 02/11/2026 | **Identificação do plugin:** produto e fabricante "Lapidamix", códigos `Lpmx` (fabricante) e `Lpd1` (plugin), formato só VST3, trilhas mono e estéreo, sem latência (confirma P19) | Depois do primeiro uso real, não trocar os códigos: projetos salvos deixariam de achar o plugin |
| D52 | 02/11/2026 | **Instalação durante o desenvolvimento:** copiar o plugin para a pasta VST3 do usuário (`%LOCALAPPDATA%\Programs\Common\VST3`) após cada compilação (confirma P20) | Não exige administrador |
| D53 | 02/11/2026 | **Teste da Fase 1 no Reaper:** carregar o plugin, conferir a lista só com "Voz", ouvir com e sem bypass, fazer o teste de cancelamento, salvar e reabrir o projeto (confirma P21) | Mostra de forma objetiva se o áudio passa sem alterações |
| D54 | 02/11/2026 | **Convenções de código:** nomes em inglês, comentários e documentação em português, commits pequenos com mensagens em português | Confirmação do usuário |
| D55 | 02/11/2026 | **Contexto vivo:** `docs/10-estado-atual.md` é o primeiro arquivo a ser lido. Ao iniciar uma sessão, ler o estado atual e as últimas entradas do diário; ao terminar, atualizar os dois. Cada entrada do diário cita os commits da sessão, e cada mensagem de commit cita fase/etapa e as decisões relacionadas | Qualquer IA deve conseguir continuar o trabalho sem depender do histórico do chat |
| D56 | 02/11/2026 | **`AGENTS.md` é o arquivo único de regras.** O `CLAUDE.md` fica só com a linha `@AGENTS.md`, que faz o Claude Code importar o outro arquivo (confirma P22) | Evita duplicar o conteúdo; ferramentas que leem `AGENTS.md` recebem as regras completas |
| D57 | 02/11/2026 | **Documentos evoluem junto com o trabalho:** tudo o que for feito no projeto precisa ser refletido nos documentos. O `10-estado-atual.md` é atualizado a cada mudança relevante, não só no fim da sessão. Antes de cada commit, o diário (entrada da sessão) e o estado atual devem estar atualizados com o que o commit leva. Qualquer outro documento afetado (decisões, roadmap, especificação, glossário etc.) é atualizado no mesmo commit. Complementa a D55 | Manter os documentos sempre fiéis ao estado real do projeto |
| D58 | 02/11/2026 | **Início da Fase 2 autorizado**, começando pela etapa de estudo do EQ (D37). Nenhum código antes da aprovação da proposta do módulo | Autorização do usuário |
| D59 | 02/11/2026 | **EQ com filtros biquad** (`juce::dsp::IIR::Filter`) em série, sem latência; passa-alta de 12 dB por oitava; sem fase linear nem oversampling na v1.0 (confirma P23) | Simples, leve e coerente com a D51. Base: estudo de 02/11/2026 em `07-referencias.md` |
| D60 | 02/11/2026 | **Tempo real:** ler os parâmetros por atômicos, suavizá-los e recalcular os coeficientes com `ArrayCoefficients`, a cada 32 amostras e só enquanto houver mudança; filtros de ordem fixa, preparados no `prepareToPlay()`; `juce::ScopedNoDenormals` no `processBlock()` (confirma P24, com ajuste) | Cumpre a D09; a proteção evita picos de processamento quando o som vai sumindo |
| D61 | 02/11/2026 | **Lugar do EQ na arquitetura:** motor de filtros em `src/core/eq/`; `EqModule` em `src/modules/`, implementando a interface comum `ProcessorModule`; nomes e valores iniciais das bandas em `src/profiles/` (confirma P25) | Concilia a D30 e a D38 |
| D62 | 02/11/2026 | **As 5 bandas:** 1 Passa-alta (80 Hz, Q fixo de 0,707, sem ganho); 2 Embolado, sino (300 Hz, Q 1,0); 3 Ressonância, sino (1 kHz, Q 2,0); 4 Presença, sino (3,5 kHz, Q 0,7); 5 Ar, prateleira alta (12 kHz, Q 0,7). **Q:** 0,3 a 10 nos sinos e 0,3 a 2 na prateleira. **Frequência:** de 20 Hz até o menor valor entre 20 kHz e 45% da taxa de amostragem, ajustado automaticamente (confirma P07 e P26, com ajustes) | Valores iniciais de partida para a voz; perto do limite superior os filtros se comportam mal; a banda de ressonância precisa de Q alto. A faixa de Q da prateleira foi escolhida por Claude, porque Q alto nela cria saliências |
| D63 | 02/11/2026 | **Ganho manual de ±12 dB nas bandas.** Os limites da P03 ficam para a correção automática da Fase 4 (confirma P27) | Na Fase 2 quem decide é o usuário, e a mudança precisa ser audível nos testes |
| D64 | 02/11/2026 | **Transparência:** bandas começam desligadas e com 0 dB; banda desligada não processa o áudio; ligar, desligar e o bypass geral fazem transição curta (cerca de 20 ms), e o bypass é informado ao DAW (confirma P28) | O plugin recém-aberto não altera o som, e ligar ou desligar não causa estalo |
| D65 | 02/11/2026 | **Interface simples:** lista de instrumento, bypass, ganho de saída de ±12 dB e, para cada banda, liga/desliga, frequência, ganho e Q; sem gráfico nem RTA nesta fase (confirma P29, com ajuste) | O ganho de saída permite comparar com o bypass no mesmo volume, porque um som mais alto tende a parecer melhor. O bypass também ignora o ganho de saída |
| D66 | 02/11/2026 | **Teste da Fase 2 no Reaper:** repetir o cancelamento com tudo desligado; conferir cada banda com ruído rosa e analisador de espectro, em 44,1 e 96 kHz; mexer nos controles durante a reprodução; salvar e reabrir; mixar uma gravação de voz à mão (confirma P30) | Cobre os critérios da Fase 2 e as suposições do estudo |
| D67 | 02/11/2026 | **Início da Fase 3 autorizado**, começando pela etapa de estudo da análise (D37). Nenhum código antes da aprovação da proposta | Autorização do usuário |
| D68 | 02/11/2026 | **Citação de commits:** no estado atual e no diário, escrever "este commit" quando o hash ainda não existir, e atualizar com o hash real na próxima mudança. Ajusta a D55 e a D57 | Evita commits só de correção |
| D69 | 02/11/2026 | **Fluxo da análise:** botão Analisar/Parar. O áudio de entrada, antes do EQ e somado em mono, é copiado para memória reservada ao clicar, e uma thread separada analisa; o plugin só mostra resultados (confirma P31) | Medir antes do EQ evita medir a própria correção; cumpre D09 e D28 |
| D70 | 02/11/2026 | **Trechos analisados:** blocos de 25 ms em duas classes. **Com som:** acima de -60 dBFS e no máximo 20 dB abaixo da média da gravação. **Voz válida:** com som e periódico pelo YIN, o que rejeita respiração e ruído (confirma P05 e P32, com ajuste) | Valores iniciais a calibrar nos testes |
| D71 | 02/11/2026 | **Espectro médio:** método de Welch com janela de Hann, cerca de 186 ms e 75% de sobreposição; **a média é feita em potência e só depois convertida para dB**. Duas curvas, ambas mostradas: **voz cantada** (principal) e **todos os trechos com som**, incluindo consoantes como "s". Suavização de 1/6 de oitava (confirma P33, com ajustes) | Média em dB distorceria o resultado; as duas curvas dão à Fase 4 a informação para decidir sobre o ar, e ajudam o de-esser no futuro |
| D72 | 02/11/2026 | **Fundamental:** YIN escrito por nós a partir do artigo, de 60 a 1100 Hz, com mediana, percentis de 5% e 95% e confiança; faixa no perfil de instrumento (confirma P34) | Percentis são menos sensíveis a erros isolados |
| D73 | 02/11/2026 | **Ressonâncias candidatas:** picos de 150 Hz a 8 kHz pelo menos 4 dB acima da vizinhança, comparando 1/24 e 1/1 oitava, presentes em notas diferentes; até 3 candidatas (confirma P35) | Sempre relativo à própria voz (D14); nada é corrigido nesta fase. Valores a calibrar. **Substituída em parte pela D83** |
| D74 | 02/11/2026 | **RTA simples:** saída do plugin, de 20 Hz a 20 kHz, FFT de 4096 pontos com janela de Hann, cerca de 30 quadros por segundo, com `AbstractFifo`; só calcula com a janela aberta. O RTA bonito fica para a interface final (confirma P14 e P36) | Ferramenta de diagnóstico |
| D75 | 02/11/2026 | **Duração da análise:** concluir ao atingir **30 s de voz válida** (não de tempo total) ou ao clicar Parar, com aviso se houver menos de 10 s de voz; limite de 120 s de captura. Confirmar a meta medindo a estabilidade com 10, 20, 30 e 45 s (confirma P37, com ajuste) | Pausas e respirações não contam como voz. As fontes só cobrem fala; a resposta final à A04 depende das medições |
| D76 | 02/11/2026 | **Arquitetura e salvamento:** análise em `src/core/analysis/`, sem depender da interface; RTA em `src/plugin/`; resultados salvos com o projeto, sem guardar áudio (confirma P38) | Segue D28 e D30, e permite testar a análise sem DAW |
| D77 | 02/11/2026 | **Teste da Fase 3:** sinais com resposta conhecida; ressonância artificial inserida com o ReaEQ; comparação de f0 e espectro com outras ferramentas em 3 a 5 vozes (f0 dentro de ±3%); repetir o cancelamento com análise e RTA ligados (confirma P39) | Cobre o critério da Fase 3 e prova que medir não altera o som |
| D78 | 02/11/2026 | **Proteção da fundamental na Fase 4:** o passa-alta se baseia nas notas mais graves (ex.: percentil de 5%), não na mediana, e nunca fica acima delas (confirma P04, com ajuste) | O passa-alta precisa ficar abaixo das notas mais baixas cantadas. **Complementada pela D91** |
| D79 | 02/11/2026 | **Vozes de teste (responde A06):** gravações de conhecidos, com permissão, e um banco de trilhas separadas. Os áudios ficam fora do Git. Opções e licenças em `11-bancos-de-vozes.md` | Combina microfones e salas comuns com material de referência |
| D80 | 02/11/2026 | **VocalSet aprovado como banco de vozes**, baixado na pasta `test-audio/vocalset/`, ignorada pelo Git. O crédito da licença CC BY 4.0 fica em `11-bancos-de-vozes.md` e no `README.md`. Os bancos são registrados em `11-bancos-de-vozes.md` | Licença mais clara entre as opções; o documento próprio mostra onde achar mais vozes no futuro |
| D81 | 02/11/2026 | **Método aprovado que não funciona:** se uma verificação mostrar que um método aprovado não funciona, parar, explicar e propor a mudança antes de implementar. Regra no `AGENTS.md` | A P40 foi implementada antes de ser aprovada; a decisão sobre um método aprovado cabe ao usuário |
| D82 | 02/11/2026 | **Duração da análise (responde A04):** manter a meta de **30 s de voz válida** da D75 | Medido com a voz do usuário em dois microfones: a medida melhora até 30 s, e 45 s quase não acrescenta (`08-plano-testes.md`) |
| D83 | 02/11/2026 | **Medida de ressonâncias (confirma P40 e P41; substitui em parte a D73):** medir cada pico só nos harmônicos das notas cantadas, em relação à mediana dos harmônicos dos dois lados, numa vizinhança de 1 oitava; o pico precisa ficar acima dos dois lados e ser mais estreito que um formante; limite de **4 dB**; busca só acima de **6 vezes a fundamental mediana**, dentro da faixa de 150 Hz a 8 kHz | Com voz real, a comparação de resoluções da D73 quase não achou picos; abaixo de 6 vezes a fundamental, os harmônicos ficam espaçados demais. Consequência aceita: ressonâncias abaixo desse ponto não são detectadas. Valores a validar em outras vozes |
| D84 | 02/11/2026 | **Ferramentas de teste no repositório:** medidores e scripts de teste ficam em `tools/`, sem áudio, para repetir as medições sempre que o método mudar. Os áudios continuam em `test-audio/`, fora do Git | Medidores temporários apagados obrigavam a refazer o trabalho, e não dá para comparar resultados sem repetir a mesma medição |
| D85 | 02/11/2026 | **Mostrar antes de aplicar (confirma a opção B da P44):** na Fase 4, o plugin mostra as bandas que pretende usar e tem um botão Aplicar. O objetivo continua sendo a mixagem automática | Permite conferir de ouvido antes de aplicar, com pouco custo de interface. As explicações seguiam como ideia futura. **Ajustada pela D92:** a versão simples das explicações, só em texto, entra junto na Fase 4 |
| D86 | 02/11/2026 | **Ressonância candidata não é defeito (parte da P43):** uma candidata é só um pico medido, e não basta para justificar um corte. Sem investigação sem fim: a v1.0 usa uma regra conservadora, e o resto fica para depois | No VocalSet, picos fortes se repetem entre cantores, e a regra 3 manda não destruir o timbre |
| D87 | 02/11/2026 | **Regra da banda de ressonância na v1.0 (confirma P45, com ajustes), a implementar na Fase 4:** só corta uma candidata com pelo menos +6 dB, presente em pelo menos 60% do tempo, em pelo menos metade das notas, estreita (teste da D83) e presente nas duas metades da captura. Nunca entre **2,5 e 4,2 kHz**. Só a mais forte, na única banda de ressonância. Corte de um terço da altura do pico, até -3 dB, com Q 4. Sem candidata elegível, a banda fica desligada. **Observação:** a zona fixa em Hz é uma simplificação para a v1.0 | Protege o timbre, com corte pequeno. Uma zona que acompanhe cada voz fica em `05-ideias-futuro.md`. Simulação em `08-plano-testes.md`. **Completada pela D88** |
| D88 | 02/11/2026 | **Banda de ressonância da v1.0, ajuste encerrado (confirma P46; completa a D87):** a banda é conservadora e limitada de propósito. **Atua** de 6 vezes a fundamental até 2,5 kHz, e de 4,2 a 6 kHz. **Não atua** abaixo de 6 vezes a fundamental (os harmônicos ficam espaçados demais para medir a largura do pico), de 2,5 a 4,2 kHz (proteger o formante do cantor) nem acima de 6 kHz (origem desconhecida do pico que se repete perto de 7 kHz, sem evidência de defeito). Sem mínimo de +8 dB: continua +6 dB. **Validação final de ouvido, na Fase 4**, com A/B contra o bypass e mostrando antes de aplicar (D85) | Na simulação com o teto, 17 de 72 janelas e 8 de 20 vozes do VocalSet recebem corte. O mínimo de +8 dB foi recusado porque deixaria passar um pico real de +9 dB. Melhorias na detecção ficam em `05-ideias-futuro.md` |
| D89 | 02/11/2026 | **Testes restantes da Fase 3 sem o Reaper (ajusta a D77):** a conferência automática sem DAW substitui os itens que faltavam. Estalos, CPU no DAW e o teste de ouvido em 4,8 kHz ficam **dispensados por decisão do usuário**. A comparação de fundamental usa o Praat pela linha de comando | Decisão do usuário. Limitação anotada em `08-plano-testes.md` |
| D90 | 02/11/2026 | **Fase 3 fechada, com exceções registradas:** 39 de 42 medidas da fundamental ficam dentro de ±3% do Praat. As 3 exceções são a mediana da female3 nos 30 s (3,86%) e a aguda do male6 (94,87% nos 30 s e 7,82% no material inteiro) | Decisão do usuário. O grave e a mediana, usados nas regras decididas, ficam dentro da tolerância nessas vozes, exceto a mediana da female3, com pouco efeito. Limitações em `08-plano-testes.md` |
| D91 | 02/11/2026 | **Proteção do passa-alta contra erro de oitava, na Fase 4 (complementa a D78):** se a medida do grave parecer inconsistente (confiança baixa ou saltos de oitava), o passa-alta fica na posição mais conservadora. Os detalhes serão decididos na Fase 4 | O male6 mostrou erro de oitava. Um grave medido uma oitava acima subiria o passa-alta e cortaria o peso da voz |
| D92 | 02/11/2026 | **Explicação das correções, versão simples na Fase 4 (confirma P47; ajusta a D85; encerra a A03; substitui a D23):** só texto, ao lado das bandas mostradas antes de aplicar, com o que será feito, por que e o efeito. O efeito vem de um dicionário de regiões no perfil de instrumento (D30), num arquivo de dados separado do código. Regras: (1) texto que sirva antes e depois do Aplicar ("Atenuar 3 dB em 250 Hz"); (2) o "por que" diz só o que foi medido, e o efeito é escrito como tendência; (3) explicar também quando a banda não age; (4) arquivo em UTF-8, embutido no plugin ao compilar; (5) dicionário revisado no estudo da Fase 4; (6) implementar depois das regras | Pensado para quem vai usar o projeto aberto, e ajuda a validar as regras nos testes. Interface bonita, explicações em outras línguas e dicionários de outros instrumentos ficam fora. Rascunho do dicionário em `05-ideias-futuro.md` |
| D93 | 02/11/2026 | **Início da Fase 4**, autorizado pelo usuário, começando pelo estudo e pela proposta (D37) | Fase 3 fechada (D90). Nenhum código de plugin antes da aprovação |
| D94 | 02/11/2026 | **Passa-alta automático (confirma P48; responde P42 pela opção a; detalha D91):** meia oitava abaixo do grave (5%), entre 40 e 100 Hz, com Q fixo. Se a confiança for menor que 0,85 ou mais de 8% dos pares de blocos vizinhos saltarem uma oitava, fica em 40 Hz | A opção b tornaria o filtro dinâmico, contra a D07. Meia oitava cobre a variação medida, e a nota mais grave perde cerca de 1 dB |
| D95 | 02/11/2026 | **Embolado automático (confirma P49, com ajuste):** região de 200 a 500 Hz, começando acima de 1,25 vez a aguda (95%); se sobrar menos de 1,25 vez entre as pontas, a banda não age. Mede a região contra os médios (500 Hz a 2 kHz) e corta o que passar de +3 dB, até -5 dB, com **Q 1,0**, no centro da região. **Risco registrado:** as faixas vieram de vozes mais agudas que a do usuário e podem cortar demais em vozes graves, em que 200–500 Hz contém harmônicos fortes, que podem ser corpo legítimo. Os limites são provisórios, a calibrar no teste de ouvido | Pedido do usuário. O teto de -5 dB fica mantido. Testar mais vozes graves e avaliar uma faixa que leve em conta a fundamental fica em `05-ideias-futuro.md` |
| D96 | 02/11/2026 | **Presença automática (confirma P50, com ajuste):** mede 2 a 5 kHz contra os médios e realça o que faltar para chegar a -19 dB, até +3 dB, em 3,5 kHz, com **Q 0,7** | Banda larga e suave, como pedido pelo usuário (Q de 0,7 a 1,0). Limite provisório |
| D97 | 02/11/2026 | **Substituída por D108.** Ar na v1.0 (confirma P51): a banda não age sozinha e continua disponível à mão. Substitui, na v1.0, o realce automático previsto na especificação | A medida fica quase sempre dentro da faixa, e a região está perto do ruído |
| D98 | 02/11/2026 | **Limites comuns (confirma P52, P03, P06 e P08):** faixa saudável medida contra a própria voz, sem corrigir o que está dentro; ação mínima de 1 dB; cortes até -5 dB (-3 dB na ressonância, D87), realces até +3 dB; intensidade de 0 a 100%, que escala os ganhos. Valores no perfil de instrumento (D30). O efeito no passa-alta, que não tem ganho, depende da P55 | Faixas provisórias, vindas do VocalSet |
| D99 | 02/11/2026 | **Fluxo e ordem (confirma P53 e P54):** Analisar → lista de bandas e explicação (D92) → Aplicar, que escreve nos parâmetros existentes; volume compensado no ganho de saída, para o A/B ser justo. Implementar em passos pequenos: regras; dados da análise; lista e Aplicar; intensidade e volume; explicações; verificação sem DAW e **A/B no Reaper, feito pelo usuário no fim da fase** | O A/B é o critério da fase e não pode ser dispensado |
| D100 | 02/11/2026 | **Intensidade no passa-alta (confirma P55; completa a D98):** em 0%, a banda fica desligada. De 0% a 100%, a frequência sai de 40 Hz e sobe até o valor calculado, em proporção de oitavas. O Q fica fixo. É a única banda em que a intensidade muda a frequência | O passa-alta não tem ganho para escalar. Assim ele age menos em intensidade baixa e nunca sobe acima do calculado |
| D101 | 02/11/2026 | **Pouca voz:** abaixo de 10 s de voz válida, o plugin não propõe bandas: mostra as medidas e um aviso que orienta a solução: "Pouca voz para propor correções. Cante mais ou repita o trecho em loop até chegar a 10 s." | Com tão pouca voz, as medidas variam demais para decidir um corte. O aviso diz o que fazer |
| D102 | 02/11/2026 | **Compensação de volume (detalha a D99):** um valor fixo, calculado a partir da análise no momento do Aplicar e escrito no controle Saída, visível e ajustável à mão. Quando a intensidade mudar, a compensação acompanha. Nada de ganho que muda durante a reprodução | Um ganho que muda continuamente seria um compressor, fora do escopo (D04) |
| D103 | 02/11/2026 | **Volume em LUFS (substitui a P56):** a compensação compara o volume antes e depois do plano com a ponderação K da ITU-R BS.1770, a mesma do LUFS, em vez da ponderação A. O plugin aplica a ponderação ao espectro da análise, sem os portões da norma. A conferência usa um medidor de LUFS integrado completo, em `tools/verifica-plugin`, e o medidor do Reaper | Pedido do usuário: é o padrão para volume percebido de música e pode ser conferido no Reaper. A troca foi simples: só mudou o peso de cada frequência |
| D105 | 02/11/2026 | **Dicionário da voz (confirma a P58, com ajuste; cumpre a regra 5 da D92):** 8 regiões em sequência, sem sobreposição: ronco até 80 Hz; corpo até 200 Hz; embolado até 500 Hz; caixa até 800 Hz; nasal até 2 kHz; presença até 5 kHz; sibilância e brilho até 10 kHz; ar acima disso. Textos em `src/profiles/voice-dictionary.json`, como tendência. "Por quê" e "Efeito" ficam em linguagem de leigo, sem jargão | Sem sobreposição, cada frequência acha uma só região, e embolado e presença coincidem com as regiões das regras (D95, D96). A linguagem simples é pedido do usuário. Corpo, caixa e nasal seguem sem fonte conferida. Regiões fixas em Hz têm limite registrado em `05-ideias-futuro.md` |
| D104 | 02/11/2026 | **Intensidade depois de aplicar (confirma a P57, com ajuste):** mover a intensidade reaplica o plano e sobrescreve ajustes manuais nas bandas e na Saída. Com uma análise nova ainda não aplicada, só muda a proposta. Não é automatizável. Fica um aviso curto junto ao controle: "Ajuste a intensidade antes de mexer nas bandas à mão" | O aviso é pedido do usuário, para ninguém perder ajustes sem perceber. Automatizar a intensidade reescreveria outros parâmetros durante a reprodução |
| D106 | 02/11/2026 | **Ajustada pela D109 no texto do botão depois de Zerar.** **Botões depois do A/B (pedido do usuário):** os controles continuam agindo em tempo real. Aplicar, na primeira vez, aplica a análise; depois vira "Reaplicar análise", que volta à proposta original e desfaz os ajustes à mão. "Zerar" desliga todas as bandas, inclusive o Ar, e volta a Saída a 0 dB. O Bypass não muda. Escolhas de implementação, a conferir pelo usuário: Reaplicar usa a intensidade atual; Zerar mantém os valores dos controles, só desliga as bandas; depois de Zerar ou de uma análise nova, o botão volta a "Aplicar" | Pedido no resultado do A/B, para voltar à proposta e recomeçar sem mexer em cada banda |
| D107 | 02/11/2026 | **Medidas escondidas (pedido do usuário):** o bloco de voz válida, com som, captura, fundamental, grave, aguda, confiança e ressonâncias candidatas fica escondido, e o espaço vai para as correções. O botão "Detalhes" mostra o bloco de novo. O aviso de pouca voz (D101) passa para a linha de status, para continuar aparecendo | Dá lugar ao texto das correções, sem perder o diagnóstico |
| D108 | 02/11/2026 | **Ar automático (confirma a P59; substitui a D97):** mede 10 a 14 kHz contra os médios (500 Hz a 2 kHz) e desconta o ruído medido nas pausas, quando há pausa. Realça abaixo de -54 dB e atenua acima de -38 dB, na quantidade calculada caso a caso, com ação mínima de 1 dB e **teto de +3 dB e -5 dB**, como nas outras bandas. Se o chiado com o realce passar de -50 dB em relação à voz: metade do realce e aviso no texto. Se o ar estiver a menos de 1 dB do ruído: não age e avisa. Aplicar e a intensidade passam a escrever na banda de Ar, e a compensação de volume a conta. **A faixa de -54 a -38 dB é provisória, a calibrar em outras vozes na Fase 5** | No A/B, o Ar fez falta no SM7dB. A prioridade é a mixagem; limpar chiado fica para um módulo futuro (`05-ideias-futuro.md`). Simulação e medidas em `08-plano-testes.md` |
| D109 | 02/11/2026 | **Texto do botão Aplicar (pedido do usuário; ajusta a D106):** uma vez aplicada a análise, o botão continua "Reaplicar análise", mesmo depois de Zerar e ao reabrir o projeto. Só volta a "Aplicar" com uma análise nova, ainda não aplicada | Inconsistência achada no Reaper: a análise já tinha sido aplicada, e o clique faz o mesmo que antes de Zerar |
| D110 | 02/11/2026 | **Fase 4 concluída (autorizada pelo usuário, se não houvesse pendências):** o critério da fase passou, com o A/B na voz do usuário e a conferência do Ar e dos botões no Reaper. Vão para a Fase 5: calibrar a faixa do Ar, observar o SM57 na fronteira de -38 dB (-38,3 dB no Reaper), ouvir a atenuação, o realce de +3 dB e a regra de chiado. A validação da banda de ressonância fica só no A/B geral do SM57 (`08-plano-testes.md`) | Nenhum desses itens impede o critério "na minha voz, o resultado soa melhor que o original" (`04-roadmap-v1.md`) |

## 🟡 Propostas aguardando confirmação

Serão decididas nas fases em que forem usadas (D31).

| # | Proposta | Observação |
|---|---|---|
| P12 | **Numeração seguinte:** v2 = violão | Continua a lógica da D19, seguindo a ordem da D18 |

## Propostas encerradas

| # | Proposta | Resultado |
|---|---|---|
| P07 | Até **5 bandas**: passa-alta, embolado, ressonância, presença, ar | Confirmada: ver D62 |
| P01 | Usar **JUCE (C++)** como framework | Confirmada: ver D26 |
| P02 | Formato inicial **VST3** | Confirmada: ver D27 |
| P09 | Separar o plugin em **parte de tempo real** (aplica o EQ) e **parte de análise** (fora da thread de áudio) | Confirmada: ver D28 |
| P10 | A pasta `docs/` passa a ser a **fonte da verdade**; o Google Drive fica só com a versão original | Confirmada: ver D24 |
| P11 | Versionar o projeto com **Git** desde o início | Confirmada: ver D29 |
| P13 | **Modelo de arquitetura:** núcleo comum (análise, EQ, limites de segurança) + perfis de instrumento + processadores como módulos encaixáveis. Na v1 o usuário escolhe o instrumento numa lista que só tem "Voz" | Confirmada: ver D30 |
| P15 | Compilador: Visual Studio Community 2022 com a carga de C++ | Confirmada: ver D47 |
| P16 | Compilação com CMake instalado à parte (`Kitware.CMake`, via winget), gerando projeto do Visual Studio 2022 | Confirmada com ajuste: usar o CMake da carga de C++, ver D48 |
| P17 | JUCE como submódulo Git em `external/JUCE`, versão 9.0.4 | Confirmada: ver D49 |
| P18 | Estrutura de pastas do código | Confirmada: ver D50 |
| P19 | Identificação do plugin | Confirmada: ver D51 |
| P20 | Instalação na pasta VST3 do usuário | Confirmada: ver D52 |
| P21 | Teste da Fase 1 no Reaper | Confirmada: ver D53 |
| P22 | `AGENTS.md` como arquivo único de regras, importado pelo `CLAUDE.md` | Confirmada: ver D56 |
| P23 | EQ com filtros biquad em série, sem latência | Confirmada: ver D59 |
| P24 | Tempo real: parâmetros suavizados e `ArrayCoefficients` | Confirmada com ajuste: ver D60 |
| P25 | Lugar do EQ na arquitetura | Confirmada: ver D61 |
| P26 | As 5 bandas | Confirmada com ajustes: ver D62 |
| P27 | Ganho manual de ±12 dB | Confirmada: ver D63 |
| P28 | Transparência das bandas e do bypass | Confirmada: ver D64 |
| P29 | Interface simples | Confirmada com ajuste: ver D65 |
| P30 | Teste da Fase 2 no Reaper | Confirmada: ver D66 |
| P04 | Proteger a fundamental: nunca colocar o passa-alta acima da nota grave da voz | Confirmada com ajuste: ver D78 |
| P05 | Analisar só os trechos cantados | Confirmada com ajuste: ver D70 e D71 |
| P14 | RTA simples na Fase 3 e RTA bonito na interface final | Confirmada: ver D74 |
| P31 | Fluxo da análise | Confirmada: ver D69 |
| P32 | Trechos com voz | Confirmada com ajuste: ver D70 |
| P33 | Espectro médio | Confirmada com ajustes: ver D71 |
| P34 | Fundamental com YIN | Confirmada: ver D72 |
| P35 | Ressonâncias candidatas | Confirmada: ver D73 |
| P36 | RTA simples | Confirmada: ver D74 |
| P37 | Duração da análise | Confirmada com ajuste: ver D75 |
| P38 | Arquitetura e salvamento | Confirmada: ver D76 |
| P39 | Teste da Fase 3 | Confirmada: ver D77 |
| P40 | Medida de ressonâncias nos harmônicos, com limite de 3 dB | Confirmada, com o limite ajustado pela P41: ver D83 |
| P41 | Limite de 4 dB e busca só acima de 6 vezes a fundamental mediana | Confirmada: ver D83 |
| P43 | Ressonância candidata não é defeito | Confirmada: ver D86. A investigação sem fim foi descartada, e a regra virou a P45 |
| P44 | Modo sugestão | Opção B confirmada: ver D85 |
| P45 | Regra da banda de ressonância | Confirmada, com zona até 4,2 kHz e pico nas duas metades: ver D87 |
| P46 | Teto de 6 kHz na banda de ressonância | Confirmada, sem o mínimo de +8 dB: ver D88 |
| P47 | Explicação das correções, versão simples na Fase 4 | Confirmada, com os seis ajustes: ver D92 |
| P03 | Limites de segurança | Confirmada: ver D98 |
| P06 | Intensidade e bypass | Confirmada: ver D98 |
| P08 | Curva-alvo como faixa saudável | Confirmada: ver D98 |
| P42 | Margem de segurança do passa-alta | Opção a confirmada: ver D94 |
| P48 | Passa-alta automático | Confirmada: ver D94 |
| P49 | Embolado automático | Confirmada, com Q 1,0 e risco registrado: ver D95 |
| P50 | Presença automática | Confirmada, com Q 0,7: ver D96 |
| P51 | Ar na v1.0 | Confirmada: ver D97 |
| P52 | Limites comuns | Confirmada: ver D98 |
| P53 | Fluxo | Confirmada: ver D99 |
| P54 | Ordem e teste | Confirmada: ver D99 |
| P55 | Intensidade no passa-alta | Confirmada: ver D100 |
| P56 | Volume da compensação com ponderação A | Substituída por LUFS (ponderação K): ver D103 |
| P57 | Intensidade depois de aplicar | Confirmada, com um aviso na janela: ver D104 |
| P58 | Dicionário da voz revisado | Confirmada, com linguagem de leigo: ver D105 |
| P59 | Ar automático | Confirmada, com teto de +3 e -5 dB: ver D108 |

## ❓ Em aberto

| # | Pergunta |
|---|---|
| — | Nenhuma no momento |

## Perguntas encerradas

| # | Pergunta | Resultado |
|---|---|---|
| A04 | Quantos segundos de análise são necessários? | 30 s de voz válida: D82 |
| A01 | Qual DAW será usado nos testes principais? | Reaper: ver D21 |
| A02 | Qual licença de código aberto usar? | AGPLv3: ver D42 |
| A03 | O plugin deve explicar cada ajuste que fez (ex.: "cortei 3 dB em 250 Hz porque…")? | Sim, em versão simples, só em texto, na Fase 4: ver D92. Antes, tinha ido para `05-ideias-futuro.md` (D23) |
| A05 | Nome definitivo do plugin | Lapidamix: ver D45 |
| A06 | De onde virão as outras vozes do teste da Fase 3? | Gravações de conhecidos e banco de trilhas: ver D79 |
