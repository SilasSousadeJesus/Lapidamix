# Lapidamix

Assistente de mixagem capaz de mixar qualquer instrumento, em forma de plugin de insert para DAWs. O nome vem de lapidar + mix: revelar o brilho que já existe no som, sem destruir o timbre.

A voz é só o primeiro instrumento. A primeira versão (v1.0) analisa uma trilha de voz e aplica automaticamente um EQ corretivo, estático e suave, adaptado àquela voz. Depois, o projeto cresce um processador e um instrumento por vez.

## Status

**Em desenvolvimento.** Já existe um plugin VST3 com EQ manual de 5 bandas, análise da voz e RTA simples, ainda sem correção automática. Ainda não há versão para instalar. O estado atual está em [`docs/10-estado-atual.md`](docs/10-estado-atual.md), o andamento em [`docs/04-roadmap-v1.md`](docs/04-roadmap-v1.md), e a documentação começa em [`docs/00-indice.md`](docs/00-indice.md).

## Licença

[AGPLv3](LICENSE).

## Créditos

Feito com o [JUCE](https://juce.com), usado sob a licença AGPLv3.

Testado com o VocalSet (Wilkins, Seetharaman, Wahl e Pardo, [DOI 10.5281/zenodo.1442513](https://doi.org/10.5281/zenodo.1442513)), sob a licença [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). As gravações não fazem parte deste repositório. Detalhes em [`docs/11-bancos-de-vozes.md`](docs/11-bancos-de-vozes.md).
