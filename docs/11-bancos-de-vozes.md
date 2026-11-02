# 11 - Bancos de Vozes

Onde encontrar vozes para testar o plugin (D79, D80). Os áudios ficam na pasta `test-audio/`, **ignorada pelo Git**: nunca enviar áudio de terceiros ao repositório.

Licenças conferidas em 02/11/2026. Isto não é orientação jurídica: reler os termos antes de baixar ou usar.

## Bancos

| Banco | Situação | Licença | Tamanho | O que tem | Observações |
|---|---|---|---|---|---|
| [VocalSet](https://zenodo.org/records/1442513) | ✅ Baixado em `test-audio/vocalset/` (versão 1.2) | CC BY 4.0 | 7,5 GB descompactado; 5,58 GB compactado | Voz solo de 20 cantores profissionais (11 homens e 9 mulheres), com escalas, arpejos, notas longas e trechos; WAV mono, 44,1 kHz. Gravado em cabine, com microfone Audio-Technica AT2020 (seção 3.2 do artigo) | A licença mais clara das quatro; exige crédito (abaixo). As 3 pastas organizam as gravações por cantor, por técnica e por vogal, então o conteúdo se repete: o total de 25,4 horas não é a duração real, que a página do banco informa ser de cerca de 10,1 horas. Há também uma versão menor no mesmo endereço (1,93 GB) |
| [Cambridge-MT](https://cambridge-mt.com/ms/mtk-faq) | Não baixado | Sem licença padrão; uso educacional gratuito | Varia por música | Multipistas de músicas completas, com vozes em contexto real | A FAQ chama o uso em pesquisa de área cinzenta, por não ter sido combinado com os contribuidores. FAQ lida só pelo trecho da busca, porque a página bloqueou o acesso direto: conferir antes de usar |
| [MUSDB18-HQ](https://zenodo.org/records/3338373) | Não baixado | Não comercial, uso educacional | 22,7 GB | 150 músicas com a trilha de voz separada | Termos variam por música; 100 delas seguem a biblioteca Mixing Secrets, 46 vêm da MedleyDB (CC BY-NC-SA 4.0) e 2 são CC BY-NC-SA 3.0 |
| [MedleyDB](https://zenodo.org/records/1649325) | Não baixado | CC BY-NC-SA 4.0 | Não informado na página | Multipistas | Só pesquisa não comercial; acesso por pedido no Zenodo |

**Para baixar de novo o VocalSet:** usar o [7-Zip](https://www.7-zip.org/) para extrair, porque o `tar` do Windows e o .NET não leem esse zip (mais de 4 GB, sem zip64). O arquivo baixado foi conferido pelo MD5 publicado e apagado depois da extração. Os arquivos auxiliares do Mac (`__MACOSX`) ficaram de fora.

## Crédito exigido: VocalSet

**VocalSet: A Singing Voice Dataset** (versão 1.2), de Julia Wilkins, Prem Seetharaman, Alison Wahl e Bryan Pardo. [DOI 10.5281/zenodo.1442513](https://doi.org/10.5281/zenodo.1442513). Licença [Creative Commons Attribution 4.0 International (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/).

Usamos as gravações sem alterá-las, só como material de teste. Elas não são distribuídas com o Lapidamix. Se algum trecho for alterado ou publicado, é preciso indicar a alteração.

## Vozes do VocalSet usadas na Fase 3

female3, female7, male9, male8 e male6, escolhidas por medição entre as 20. Motivos e resultados em `08-plano-testes.md`. Para o Reaper, usar os arquivos de `test-audio/vocalset/data_by_singer/<cantor>/excerpts/straight/`.

## Outras vozes

- Gravações de conhecidos, sempre com permissão, para testar microfones e salas comuns (D79). O VocalSet traz cantores profissionais e não substitui esse teste.
- Voz cantada do usuário no SM7dB e no SM57 (D33), em `test-audio/voz-usuario/`: duas tomadas da mesma música, cerca de 140 s cada. O original continua na pasta Downloads do usuário.
