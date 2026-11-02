# Comparação com o Praat

Compara a fundamental e o espectro do Lapidamix com os do [Praat](https://www.praat.org), no mesmo trecho de áudio (D77, D84). Sem áudio nesta pasta: os trechos exportados ficam em `test-audio/praat/`, fora do Git.

**Precisa de:** Praat instalado em `%LOCALAPPDATA%\Programs\Praat` (usamos a versão 7.0.02, baixada dos lançamentos oficiais, com o SHA-256 conferido) e o `banco` compilado (ver `tools/medidor-voz/README.md`).

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\praat\comparar.ps1
```

Cada voz é medida em dois trechos: `30s`, o que o plugin analisa na meta de 30 s de voz válida, e `inteiro`. O Praat usa a mesma faixa do plugin (60 a 1100 Hz), com os demais ajustes padrão. O espectro compara a forma, sem o volume geral, em terços de oitava de 125 Hz a 8 kHz.

Os scripts do Praat só imprimem resultados, por isso não precisam da opção `--FULL-TRUST`.
