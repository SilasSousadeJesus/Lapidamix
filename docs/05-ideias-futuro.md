# 05 - Ideias para o Futuro

Tudo que está fora da v1.0. Só está decidido o que cita um Dxx; o resto é um estacionamento de ideias. Quando uma ideia for aprovada, ela vai para `02-decisoes.md` e para o roadmap.

## Próximos processadores para a voz (um por vez)
Entram nas versões 1.1, 1.2 etc., até fechar a cadeia de sinal da voz (D19). A lista abaixo ainda não define a ordem.
- Compressor automático (controlar a variação de volume).
- De-esser automático (controlar os "S" e "X").
- EQ dinâmico / controle de ressonâncias que só age quando o problema aparece (referência: Soothe2).
- Nivelamento de ganho de saída.
- Efeitos de ambiência (reverb, delay) — bem mais à frente.

## Ressonâncias
Melhorias na detecção, fora da v1.0 (D88):
- Zona de proteção do formante que acompanhe cada voz, no lugar da faixa fixa de 2,5 a 4,2 kHz (D87).
- Vozes agudas: nelas, a busca só começa perto de 2,3 kHz, e nenhum material testado passou de cerca de 395 Hz de mediana.
- Picos pouco encontrados: nas vozes do VocalSet, picos inseridos acima do início da busca passam muitas vezes sem detecção, por causa ainda desconhecida.
- Região acima de 6 kHz: entender o pico que se repete perto de 7 kHz e decidir se a banda pode atuar ali.

## Modo sugestão / modo professor
- O plugin analisa e mostra o EQ sugerido numa interface bonita, e o usuário decide se aplica ou ajusta à mão.
- Reavaliar na Fase 3. O objetivo principal continua sendo a mixagem automática.
- **Decidido em 02/11/2026 (D85):** opção B, mostrar as bandas antes de aplicar, na Fase 4. O restante do modo sugestão continua como ideia futura.
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
- Explicação de cada ajuste ("cortei 3 dB em 250 Hz porque…"). Era a pergunta A03; fica só aqui (D23).

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
