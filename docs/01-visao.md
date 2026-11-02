# 01 - Visão do Projeto

O Mixing Assistant é um **assistente de mixagem capaz de mixar qualquer instrumento**. **Não é um plugin de voz:** a voz é só o primeiro instrumento, por questão de escopo (D17).

## O problema

Quero focar no que gosto — cantar — e não ter que mixar. Meu caso concreto, que é o ponto de partida: tenho voz grave e, gravando com o Shure SM7dB, a voz fica escura e embolada. Corrigir isso na mão toma tempo e exige conhecimento de mixagem.

## O objetivo

Um plugin de insert (como os da Waves) que, colocado na trilha, analisa o áudio e aplica sozinho uma boa mixagem: tira o embolamento, traz clareza e presença, **sem matar a voz com cortes agressivos**.

## Para quem

1. **Agora:** uso pessoal.
2. **Futuro próximo, depois de testado:** projeto aberto, para qualquer pessoa usar.

Não há objetivo comercial. O foco não é se diferenciar de produtos existentes, é fazer algo **bom e funcional**.

## Princípios

- **Soar bem e funcionar** vale mais do que ser original. Podemos nos inspirar livremente em projetos existentes.
- **Não matar a voz:** correções suaves e com limites. Na dúvida, corrigir menos.
- **Inteligente e adaptável:** as regras são fixas, mas o resultado se adapta à voz/instrumento analisado (D25). Nada de usar uma voz específica como molde, porque isso faria outras vozes não funcionarem.
- **Começar pequeno:** uma coisa por vez. Primeiro só EQ; os outros processadores entram depois, um a um.
- **Crescer sem refazer:** adicionar instrumentos e processadores não pode exigir refazer o que já existe (D20).
- **Tempo real seguro:** o processamento de áudio nunca pode travar (sem estalos).
- **Interface depois:** interface simples até o sistema estar rodando; a interface bonita vem no final.

## O que a v1.0 é

- Plugin de insert para **uma trilha de voz**.
- **EQ corretivo, estático**, calculado automaticamente a partir da análise da voz.

## O que a v1.0 não é

- Não mixa por estilo musical.
- Não mixa considerando as outras trilhas (mix em contexto).
- Não tem compressor, de-esser, reverb etc. (os processadores entram nas versões 1.1, 1.2 etc., um por vez, até fechar a cadeia da voz — D19).
- Não tem interface elaborada.

(Tudo isso está registrado em `05-ideias-futuro.md`.)

## Visão de longo prazo

Um assistente de mixagem que cuida de cada trilha — na ordem: voz, violão, contrabaixo, guitarra, bateria (cada peça) e assim por diante (D18) — e, mais à frente, da música inteira com as trilhas conversando entre si — aberto e livre para quem quiser usar.
