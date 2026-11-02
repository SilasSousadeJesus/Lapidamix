# 05 - Ideias para o Futuro

Tudo que está fora da v1.0. Só está decidido o que cita um Dxx; o resto é um estacionamento de ideias. Quando uma ideia for aprovada, ela vai para `02-decisoes.md` e para o roadmap.

## Próximos processadores para a voz (um por vez)
Entram nas versões 1.1, 1.2 etc., até fechar a cadeia de sinal da voz (D19). A lista abaixo ainda não define a ordem.
- Compressor automático (controlar a variação de volume).
- De-esser automático (controlar os "S" e "X"). Detalhes em "Plosivos e sibilância".
- Tratamento de plosivos ("P" e "B"). Detalhes em "Plosivos e sibilância".
- EQ dinâmico / controle de ressonâncias que só age quando o problema aparece (referência: Soothe2).
- Nivelamento de ganho de saída.
- Efeitos de ambiência (reverb, delay) — bem mais à frente.

## Plosivos e sibilância
Depois da v1.0, fora do escopo atual. Seguem o método da D37 (estudo, proposta, implementação, teste) e entram como módulos encaixáveis (D30), um por vez, na ordem das versões 1.1, 1.2 etc. A ordem entre elas será decidida depois.

**Tratamento de plosivos ("p", "b")**
- **O que é:** o sopro de ar batendo na cápsula do microfone, com energia nos graves (geralmente abaixo de cerca de 150–200 Hz), em rajadas curtas. Microfones como o SM57 captam bastante; o SM7dB lida melhor, mas também capta.
- **Ideia:** detectar essas rajadas e atenuar só os graves naqueles instantes, sem afetar o resto da voz. É diferente do passa-alta, que age o tempo todo.
- **Relação com o que já existe:** o filtro de periodicidade da análise (P32, D70) tira os plosivos da média de voz, mas não os corrige. A exclusão ainda não foi medida com plosivos, e um trecho de voz da média ainda pode conter até 20% de blocos fora da voz.

**Tratamento de sibilância ("s", "ch", "x"): de-esser automático**
- **O que é:** energia excessiva nos agudos (aproximadamente de 4 a 10 kHz, variando por voz e microfone) nas consoantes sibilantes.
- **Ideia:** detectar a região de sibilância de cada voz e atenuar só nos instantes em que ela aparece.
- **Relação com o que já existe:** a análise da Fase 3 já calcula a média "todo o som", que inclui as sibilantes. A diferença entre ela e a média "voz" pode indicar onde a sibilância está e quanto ela pesa. No SM57, essa diferença apareceu entre cerca de 5 e 8 kHz.

## Embolado
- Testar com mais vozes graves e avaliar uma faixa que leve em conta a fundamental. Em vozes graves, 200–500 Hz contém harmônicos fortes, que podem ser corpo legítimo, e as faixas atuais vieram de vozes mais agudas (D95).

## Ressonâncias
Melhorias na detecção, fora da v1.0 (D88):
- Zona de proteção do formante que acompanhe cada voz, no lugar da faixa fixa de 2,5 a 4,2 kHz (D87).
- Vozes agudas: nelas, a busca só começa perto de 2,3 kHz, e nenhum material testado passou de cerca de 395 Hz de mediana.
- Picos pouco encontrados: nas vozes do VocalSet, picos inseridos acima do início da busca passam muitas vezes sem detecção, por causa ainda desconhecida.
- Região acima de 6 kHz: entender o pico que se repete perto de 7 kHz e decidir se a banda pode atuar ali.

## Fundamental
- Aguda (95%) errada no male6 do VocalSet, comparada ao Praat, provavelmente por erro de oitava em parte dos trechos (`08-plano-testes.md`). Nenhuma regra decidida usa essa medida.

## Modo sugestão / modo professor
- O plugin analisa e mostra o EQ sugerido numa interface bonita, e o usuário decide se aplica ou ajusta à mão.
- Reavaliar na Fase 3. O objetivo principal continua sendo a mixagem automática.
- **Decidido em 02/11/2026 (D85, ajustada pela D92):** opção B, mostrar as bandas antes de aplicar, na Fase 4, com uma explicação simples em texto. O restante do modo sugestão continua como ideia futura.
- **Reavaliação feita em 02/11/2026:**
  - **O que a análise já mostra:** fundamental e espectro médio se adaptam à voz e ao microfone, e dá para confiar neles como medida. As ressonâncias candidatas ainda não dizem o que é defeito: picos fortes se repetem entre cantores, e picos inseridos são pouco encontrados em várias vozes (`08-plano-testes.md`).
  - **O que isso significa:** a análise informa bem, mas ainda não sustenta sozinha todas as decisões de corte. A dúvida maior é sobre ressonâncias, e o modo sugestão permitiria conferir de ouvido antes de aplicar.
  - **Opção A — ficar de fora da v1.0:** a Fase 4 aplica tudo sozinha, como previsto. É o caminho mais simples, mas um erro de corte só aparece depois de aplicado; a comparação com bypass e os limites de segurança seriam a proteção.
  - **Opção B — mostrar antes de aplicar, na Fase 4:** o plugin mostra as bandas que pretende usar, com um botão Aplicar. Não muda as regras nem o objetivo automático, e ajuda a calibrar. Custa um pouco mais de interface.
  - **Opção C — modo sugestão completo na v1.0, com explicações e ajuste à mão:** é o mais útil para aprender, mas aumenta bastante o escopo e atrasa a v1.0.
  - **Recomendação:** opção B. Ela dá o benefício principal com pouco custo e mantém a mixagem automática como objetivo. As explicações seguem como ideia futura.

## Interface
- Interface bonita (só com o sistema rodando — ver D08).
- 🟡 RTA bonito, com o espectro e as bandas aplicadas (P14).
- Explicação de cada ajuste em interface bonita, em outras línguas e com dicionários de outros instrumentos. A versão simples, só em texto, entra na Fase 4 (D92).

## Explicação didática das correções
✅ A versão simples, só em texto, entra na Fase 4 (D92), com as regras registradas lá. Esta é a ideia completa, pensada para quem vai usar o projeto aberto, e o rascunho a revisar no estudo.

Para cada correção, o plugin mostra um texto com três partes:
1. **O que fez:** tipo de ajuste, frequência e quantidade (ex.: "Atenuei 3 dB em 250 Hz"; na versão da D92, "Atenuar 3 dB em 250 Hz").
2. **Por que fez:** o que a análise encontrou (ex.: "havia excesso de energia nessa região em relação ao equilíbrio esperado").
3. **O efeito:** a sensação e a função daquela região no instrumento (ex.: "isso reduz a sensação de proximidade exagerada e deixa a voz mais limpa").

A parte 3 vem de um **dicionário de regiões por instrumento**, parte do perfil de instrumento (D30): cada região tem nome, faixa aproximada, sensação ao realçar e sensação ao cortar. Cada instrumento novo terá o seu dicionário. Os textos ficam num arquivo de dados separado do código, para facilitar correções, contribuições e traduções.

**Rascunho do dicionário da voz, a revisar na etapa de estudo:**

| Região | Sensação ao realçar | Sensação ao cortar |
|---|---|---|
| Abaixo de ~80 Hz | Ronco, sopro de plosivos, ruído de manuseio | Som mais limpo, sem "peso inútil" (cuidado: em vozes muito graves, a fundamental chega perto disso) |
| ~100–250 Hz (corpo) | Voz mais cheia, quente, próxima | Voz mais fina, distante, sem força |
| ~200–500 Hz (embolado) | Voz "embolada", abafada, confusa | Voz mais limpa e definida |
| ~400–800 Hz (caixa) | Som de "caixa", de sala pequena | Voz mais seca e pronta |
| ~800 Hz–1,5 kHz (nasal) | Voz anasalada, "de telefone" | Voz oca, sem corpo nos médios, mais distante |
| ~2–5 kHz (presença) | Voz à frente da mix, clara, inteligível; em excesso, áspera e cansativa | Voz opaca, recuada, menos inteligível |
| ~5–10 kHz (sibilância e brilho) | Consoantes mais nítidas; em excesso, "s" agressivos | Voz abafada, sem definição nas consoantes |
| ~10 kHz+ (ar) | Voz aberta, arejada; em excesso, chiado | Voz fechada, sem brilho |

## Outros instrumentos
- Ordem decidida (D18): voz, depois violão, contrabaixo, guitarra, bateria (cada peça) e assim por diante. 🟡 Numeração seguinte: v2 = violão (P12).
- Perfis de instrumento (D30). Teclado ainda sem posição definida na ordem.
- Detecção automática do tipo de instrumento.

## Mix em contexto
- Instâncias do plugin conversando entre si para evitar conflitos entre trilhas (ex.: voz vs. guitarra, kick vs. baixo).
- Alternativa para o Reaper: script (ReaScript) com acesso ao projeto inteiro.

## Modos extras de correção
- Estilos musicais (pop, gospel, rock…).
- Usar uma faixa de referência como alvo.

## Compatibilidade
- AAX para Pro Tools (exige acordo com a Avid).
- Mac (AU).

## Abertura do projeto
- Nome definitivo: Lapidamix (A05, D45).
- Licença de código aberto: AGPLv3 (A02, D42).
- Repositório já público no GitHub desde 02/11/2026 (D40).
- Guia de instalação e uso para outras pessoas.
