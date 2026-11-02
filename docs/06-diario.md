# 06 - Diário de Andamento

Uma entrada por sessão de trabalho, a mais recente no topo.

**Modelo de entrada:**

```
## DD/MM/AAAA — Título curto
**Fase:** …
**O que foi feito:** …
**Decisões tomadas:** (referência ao 02, ex.: D17)
**Commits:** (hashes dos commits da sessão; onde o hash ainda não existir, "este commit", trocado pelo hash real na próxima mudança — D68)
**Problemas / dúvidas:** …
**Próximos passos:** …
```

---

## 02/11/2026 — Segundo teste no Reaper, resultados parciais

**Fase:** 3 — Análise, só medir (etapa d). **A fase não foi fechada.** A Fase 4 não foi iniciada.

**O que foi feito:**
- Registrados os resultados informados da interface e do SM7dB.
- Candidata de 4800 Hz (+4,1 dB) conferida no medidor: está no limite de 4 dB, depende do ponto de início e não seria cortada pela regra D87/D88.
- Critérios da Fase 3 verificados: não dá para dar como atendidos.

**Decisões tomadas:** nenhuma.

**Commits:** este commit. O hash `139cd83` entrou nas entradas anteriores (D68).

**Problemas / dúvidas:** a mensagem do usuário trouxe sem preencher o campo com SM57 do início, cancelamento, RTA, aviso de menos de 10 s, salvar e reabrir, ressonância artificial e VocalSet. Sem esses resultados, em especial a comparação das vozes com outra ferramenta, a fase não fecha.

**Próximos passos:**
- O usuário informa os resultados que faltam, e a fase é reavaliada.

---

## 02/11/2026 — Ajuste da regra de ressonância encerrado

**Fase:** 3 — Análise, só medir (preparação do fechamento). A Fase 4 não foi iniciada.

**O que foi feito:**
- Teto de 6 kHz aprovado, sem o mínimo de +8 dB (D88, confirma P46). Ajuste encerrado para a v1.0.
- Registradas as zonas de atuação, as regiões excluídas e o motivo de cada uma, na especificação e nas decisões.
- Validação final de ouvido incluída na Fase 4, com A/B contra o bypass e mostrando antes de aplicar.
- Melhorias na detecção movidas para `05-ideias-futuro.md`.

**Decisões tomadas:** D88.

**Commits:** `139cd83`.

**Problemas / dúvidas:** a regra ainda não está no plugin e só foi simulada. Nas vozes do VocalSet, 17 de 72 janelas e 8 de 20 vozes recebem corte; se isso é pouco, só a validação de ouvido dirá.

**Próximos passos:**
- O usuário conclui os testes da lista em `08-plano-testes.md`.

---

## 02/11/2026 — Regra de ressonância aprovada e simulada

**Fase:** 3 — Análise, só medir (preparação do fechamento). A Fase 4 não foi iniciada.

**O que foi feito:**
- P45 aprovada com zona proibida de 2,5 a 4,2 kHz e pico nas duas metades da captura (D87). Zona que acompanhe cada voz registrada em `05-ideias-futuro.md`.
- Regra simulada em `banco regra`, com os ajustes separados e juntos, e com duas variantes para a proposta P46. Nada foi implementado no plugin.

**Decisões tomadas:** D87. Proposta nova: P46.

**Commits:** `139cd83`.

**Problemas / dúvidas:**
- A regra aprovada não atende ao critério de poucos cortes: 40 de 72 janelas e 14 de 20 vozes, contra 48 de 72 e 16 de 20 antes. 29 dos 40 cortes ficam acima de 6 kHz.
- Com a zona maior, várias vozes trocam o corte de cerca de 3,6 kHz para cerca de 7 kHz.
- Parei e propus a P46 sem mudar a regra (D81).

**Próximos passos:**
- O usuário conclui os testes da lista e decide a P46.

---

## 02/11/2026 — Modo sugestão decidido e regra de ressonância proposta

**Fase:** 3 — Análise, só medir (preparação do fechamento). A Fase 4 não foi iniciada.

**O que foi feito:**
- Opção B do modo sugestão registrada e incluída na Fase 4 (D85). Candidata definida como não sendo defeito (D86).
- Artigo do VocalSet lido: mesma cabine e mesmo modelo de microfone (AT2020).
- Ferramenta `origem` criada, e o pico de 6,2 a 7,4 kHz testado por vogal, por pessoa e fora da voz.
- Regra conservadora proposta (P45) e simulada em `banco regra`. Nada foi implementado no plugin.
- Lista do que falta para fechar a Fase 3 registrada em `08-plano-testes.md`.
- Corrigido o `11-bancos-de-vozes.md`: são 11 homens e 9 mulheres, e não o contrário.

**Decisões tomadas:** D85, D86. Proposta nova: P45.

**Commits:** `139cd83`.

**Problemas / dúvidas:**
- Um pico fixo do equipamento ou da sala não explica o resultado sozinho: o pico muda com a vogal e a pessoa, e nem todos o têm. Uma contribuição do microfone segue possível, mas não dá para separar. Investigação encerrada.
- A regra simulada age em 48 de 72 janelas do VocalSet, e 12 cortes ficam logo acima da zona proibida.
- A exigência do pico nas duas metades da captura ainda não foi simulada.

**Próximos passos:**
- O usuário faz os testes da lista e decide a P45.

---

## 02/11/2026 — Reaper, correções na interface e vozes do VocalSet

**Fase:** 3 — Análise, só medir (preparação do fechamento). A Fase 4 não foi iniciada.

**O que foi feito:**
- Resultados do teste do usuário no Reaper registrados em `08-plano-testes.md`.
- Interface: corrigido o acento de "RTA (saída)"; casas decimais padronizadas, com vírgula; curvas de voz e de todo o som com cor e espessura bem diferentes. Plugin compilado em Release, sem erros, e conferido numa imagem da janela.
- Ferramentas novas: `tools/captura-interface` (imagem da janela) e `banco`, em `tools/medidor-voz`, com variantes sem o limite de busca e sem o corte de candidatas (D84).
- 20 vozes do VocalSet medidas e 5 detalhadas, com comparação do limite de 6 vezes a fundamental.
- Reavaliação do modo sugestão preparada em `05-ideias-futuro.md`.

**Decisões tomadas:** nenhuma. Propostas novas: P43 e P44.

**Commits:** `139cd83`.

**Problemas / dúvidas:**
- A causa das casas decimais: `juce::String (valor, 0)` não arredonda. Isso também afetava os controles de frequência e os números do gráfico.
- O resultado do SM57 no Reaper não bate com o início da gravação, e parece vir de perto de 90 s. Assim, 100 × 87 Hz parece ser diferença de trecho, não de tomada. A P42 continua necessária.
- Nas 5 vozes do VocalSet, os picos inseridos foram pouco encontrados, mesmo acima do início da busca, e o corte de 3 candidatas não explica. Causa desconhecida; nada foi mudado (D81). Proposta P43.
- O VocalSet não informa notas nem ressonâncias: não há gabarito. A aguda (95%) dobra em algumas janelas do male8 e do male6, sem confirmação se é registro agudo ou erro de oitava.
- Nenhum material testado passou de cerca de 395 Hz de mediana.

**Próximos passos:**
- O usuário conclui os testes no Reaper e decide P43 e P44.

---

## 02/11/2026 — Duração e ressonâncias decididas; medidores no repositório

**Fase:** 3 — Análise, só medir (etapa d)

**O que foi feito:**
- A04 encerrada: mantidos 30 s de voz válida (D82).
- P40 e P41 aprovadas (D83) e implementadas no `VoiceAnalyzer`: limite de 4 dB e busca acima de 6 vezes a fundamental mediana. Plugin compilado em Release, sem erros.
- Medidores guardados em `tools/medidor-voz`, sem áudio e usando o código real da análise (D84). Medições repetidas: mesmos números da simulação da P41.
- Opções da margem do passa-alta registradas para a Fase 4 (P42).

**Decisões tomadas:** D82, D83, D84. Proposta nova: P42.

**Commits:** `139cd83`.

**Problemas / dúvidas:**
- Na entrada anterior, os medidores foram apagados, e esse código foi recuperado de uma cópia na pasta de rascunho. A D84 evita repetir isso.
- O limite de 6 vezes a fundamental ainda não foi testado em vozes agudas, em que a busca começa mais alto.
- A verificação sintética antiga foi apagada e não foi repetida com o limite de 4 dB. Nela, um pico de +9 dB perto de 4 kHz media só +3,3 dB, e agora ficaria de fora. Na voz real, esse pico mediu de 6,4 a 8,4 dB.

**Próximos passos:**
- O usuário faz o teste no Reaper e a conferência no Praat (D77), e testa outras vozes.

---

## 02/11/2026 — Voz real: A04, microfones e P40

**Fase:** 3 — Análise, só medir (etapa d, parte feita fora do DAW)

**O que foi feito:**
- Gravações do usuário (SM7dB e SM57) copiadas para `test-audio/voz-usuario/`, fora do Git; os originais continuam em Downloads.
- Estabilidade medida com 10, 20, 30 e 45 s de voz válida, em programa temporário fora do projeto que usa o `VoiceAnalyzer` sem alteração.
- Comparação dos dois microfones: a fundamental ficou quase igual, e o espectro, diferente, principalmente de 2 a 10 kHz.
- Picos inseridos no áudio para testar a D73 (reconstruída) e a P40. Uma cópia temporária do analisador simulou variações, sem mudar o código do plugin.
- Regra registrada no `AGENTS.md` (D81). Resultados em `08-plano-testes.md`. Programas temporários apagados.

**Decisões tomadas:** D81. Proposta nova: P41. A04 e P40 seguem aguardando o usuário.

**Commits:** `139cd83`. O hash `ecae02e` entrou nas entradas anteriores (D68).

**Problemas / dúvidas:**
- As gravações são tomadas diferentes, então a comparação bloco a bloco da fundamental não vale como teste.
- Nesta voz, a medida de ressonâncias não funcionou em 300 e 500 Hz: os picos inseridos não foram achados, e surgem candidatas duvidosas perto de 500 Hz. A partir de 1 kHz ela funcionou; 800 Hz não foi medido.
- Percentil de 5%: varia entre trechos, e mais tempo não resolve. Afeta a D78 na Fase 4.
- A D73 comparada é uma reconstrução, porque o código original não ficou guardado.
- Uma só voz e uma só música: tudo isso precisa ser confirmado em outras vozes.

**Próximos passos:**
- O usuário decide a A04 e as P40 e P41.
- Teste no Reaper e conferência no Praat (D77).

---

## 02/11/2026 — Banco de vozes VocalSet

**Fase:** 3 — Análise, só medir (preparação do teste com vozes reais)

**O que foi feito:**
- `test-audio/` adicionada ao `.gitignore`, com a confirmação do Git antes do download.
- VocalSet 1.2 baixado e conferido pelo MD5 publicado. Extraído com o 7-Zip em `test-audio/vocalset/` (7,5 GB, 10.687 arquivos WAV com cabeçalho válido). O zip foi apagado depois.
- Criado `11-bancos-de-vozes.md`, com a tabela de bancos, o crédito da licença CC BY 4.0 e o que já foi baixado; crédito também no `README.md`.

**Decisões tomadas:** D80.

**Commits:** `ecae02e`.

**Problemas / dúvidas:**
- O `tar` do Windows e o .NET não leram o zip, por ter mais de 4 GB sem zip64. O 7-Zip extraiu com um aviso sobre isso, e sem erros de CRC.
- A soma dos arquivos dá 25,4 horas porque as 3 pastas repetem as gravações; a página do banco informa cerca de 10,1 horas. A conta exata por pasta não foi conferida.

**Próximos passos:**
- O usuário termina a gravação e faz o teste com voz real (D77).

---

## 02/11/2026 — Fase 3: análise e RTA implementados

**Fase:** 3 — Análise, só medir (etapa c concluída; etapa d aguardando o usuário)

**O que foi feito:**
- Proposta aprovada, com média em potência, duas médias, meta em segundos de voz válida e proteção da fundamental pelas notas graves na Fase 4.
- A06 respondida (gravações de conhecidos e banco de trilhas), com licenças pesquisadas em `07-referencias.md`.
- Criados `AudioCapture`, `YinPitchDetector`, `VoiceAnalyzer`, `AnalysisController`, `AnalysisResult`, `RtaAnalyzer` e `SpectrumPlot`; plugin compilado em Release, com 0 erros e 0 avisos.
- Verificação com tons e voz sintética, em programa temporário apagado depois: todas as verificações passaram em 3 rodadas, sem pedidos de memória na thread de áudio.

**Decisões tomadas:** D69 a D79. Proposta nova: P40.

**Commits:** `ecae02e`.

**Problemas / dúvidas:**
- A verificação encontrou dois erros meus: limites de silêncio calculados como amplitude em vez de potência, e falsas ressonâncias vindas de harmônicos coincidentes. Ambos foram corrigidos.
- A correção das ressonâncias mudou o método aprovado na D73 e o limite de 4 para 3 dB, registrados na P40 para decisão do usuário.
- Picos perto de 4 kHz foram subestimados; os limites foram calibrados só em voz sintética.
- A FAQ da Cambridge-MT foi lida só pelo trecho da busca, porque a página bloqueou o acesso direto.

**Próximos passos:**
- O usuário faz o teste com voz real (D77).

---

## 02/11/2026 — Fase 3: estudo e proposta da análise

**Fase:** 3 — Análise, só medir (etapas a e b da D37)

**O que foi feito:**
- Fase 3 autorizada e status atualizado.
- Ajustada a citação de commits no `AGENTS.md` e no modelo deste diário (D68).
- Estudo feito, com a nota em `07-referencias.md`, separando fatos e suposições.
- Proposta registrada (P31 a P39), e A06 aberta. Nenhum código foi escrito.

**Decisões tomadas:** D67 e D68. Propostas novas: P31 a P39.

**Commits:** `ecae02e`.

**Problemas / dúvidas:**
- O PDF do artigo do YIN não pôde ser lido; o valor do limite citado na nota é de memória e está marcado como suposição.
- Não há fonte com tempo de análise para canto, nem algoritmo padrão de detecção de ressonâncias; a proposta deixa isso para medição.

**Próximos passos:**
- Aprovação ou ajuste da proposta e resposta à A06 pelo usuário.

---

## 02/11/2026 — Fechamento da Fase 2

**Fase:** 2 — EQ manual (concluída nesta sessão)

**O que foi feito:**
- O usuário fez os 5 passos do teste no Reaper (D66), e todos passaram.
- Resultado registrado em `08-plano-testes.md` e `07-referencias.md`, e status atualizado.

**Decisões tomadas:** nenhuma nova.

**Commits:** `e4adbce` (Fase 2 completa) e "Fase 2: registra o commit no diário e no estado atual (D57)", que traz esta correção.

**Problemas / dúvidas:**
- O resultado em 44,1 e 96 kHz foi avaliado no analisador, sem medição numérica da diferença entre as taxas.
- O commit `e4adbce` levou o estado atual e o diário ainda dizendo "sem commit", contra a D57; o commit seguinte corrige isso.

**Próximos passos:**
- A Fase 3 só começa quando o usuário autorizar, pela etapa de estudo da análise (D37, D38).

---

## 02/11/2026 — Fase 2: EQ implementado

**Fase:** 2 — EQ manual (etapa c concluída; etapa d aguardando o usuário)

**O que foi feito:**
- Proposta aprovada, com limite de frequência de 45% da taxa de amostragem, faixas de Q por banda, `ScopedNoDenormals` e ganho de saída de ±12 dB.
- Criados `ProcessorModule`, `EqBandFilter`, `EqModule` e o perfil da voz; `PluginProcessor` e `PluginEditor` atualizados.
- Plugin compilado em Release, com 0 erros e 0 avisos, e copiado para a pasta VST3.
- Verificação automática em programa temporário, em Debug, apagado depois: todas as verificações passaram, sem nenhum pedido de memória na thread de áudio.

**Decisões tomadas:** D59 a D66.

**Commits:** `ecae02e`.

**Problemas / dúvidas:**
- A faixa de frequência dos parâmetros continua de 20 Hz a 20 kHz, porque o DAW precisa de uma faixa fixa; o limite de 45% é aplicado no processamento, e a janela pode mostrar um valor acima do aplicado em taxas de amostragem baixas.
- O primeiro relatório acusou pedidos de memória, mas eles vinham do temporizador do JUCE em outra thread; o verificador foi corrigido para contar só a thread de áudio.
- A aparência da janela e o som ainda não foram avaliados: isso depende do teste no Reaper.

**Próximos passos:**
- O usuário faz o teste no Reaper (D66).

---

## 02/11/2026 — Fase 2: estudo e proposta do EQ

**Fase:** 2 — EQ manual (etapas a e b da D37)

**O que foi feito:**
- Fase 2 autorizada e status atualizado.
- Estudo do EQ feito, com a nota em `07-referencias.md`, separando fatos, prática comum e suposições.
- Proposta do módulo registrada (P23 a P30). Nenhum código foi escrito.
- Termos novos adicionados ao glossário.

**Decisões tomadas:** D58. Propostas novas: P23 a P30.

**Commits:** `ecae02e`.

**Problemas / dúvidas:**
- A página do ReaEQ não abriu e o artigo de Vicanek só pôde ser lido em parte, como indicado na nota.
- A D30 põe o EQ no núcleo comum e a D38 o chama de primeiro módulo; a P25 propõe como conciliar.

**Próximos passos:**
- Aprovação ou ajuste da proposta pelo usuário.

---

## 02/11/2026 — Documentos evoluem junto com o trabalho

**Fase:** 1 concluída; Fase 2 aguardando a autorização do usuário.

**O que foi feito:**
- Regra de manutenção ajustada no `AGENTS.md`: estado atual atualizado a cada mudança relevante; diário e estado atual atualizados antes de cada commit; outros documentos afetados atualizados no mesmo commit.
- Índice e estado atual alinhados à nova regra.

**Decisões tomadas:** D57 (complementa a D55).

**Commits:** "Fase 1: faz os documentos evoluírem junto com o trabalho (D57)", que traz esta entrada.

**Problemas / dúvidas:** nenhum no momento.

**Próximos passos:**
- A Fase 2 só começa quando o usuário autorizar, pela etapa de estudo do EQ (D37, D38).

---

## 02/11/2026 — Regras movidas para o AGENTS.md

**Fase:** 1 concluída; Fase 2 aguardando a autorização do usuário.

**O que foi feito:**
- Regras movidas para o `AGENTS.md`; o `CLAUDE.md` ficou só com a linha `@AGENTS.md`.
- Teste numa sessão nova, sem usar ferramentas: ela citou corretamente a regra 3, a seção da D37 com as 4 etapas, a leitura de início de sessão e o nome do projeto. A importação funciona no Claude Code.
- Ideia do modo sugestão / modo professor registrada em `05-ideias-futuro.md`, para reavaliar na Fase 3.

**Decisões tomadas:** D56 (confirma P22).

**Commits:** "Fase 1: move as regras para o AGENTS.md (D56)", que traz esta entrada.

**Problemas / dúvidas:** o `AGENTS.md` só foi testado no Claude Code, não em outras ferramentas de IA.

**Próximos passos:**
- A Fase 2 só começa quando o usuário autorizar, pela etapa de estudo do EQ (D37, D38).

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
