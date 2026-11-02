# Medidor de voz

Mede a análise do Lapidamix em gravações reais, fora do DAW (D84). Usa o código de `src/core/analysis` sem cópia, então **repetir a medição depois de mudar o método mostra o efeito da mudança**. Não faz parte do plugin.

Não guarde áudio nesta pasta. As gravações ficam em `test-audio/`, fora do Git, e os resultados importantes vão para `docs/08-plano-testes.md`.

## Como compilar

O CMake vem com o Visual Studio e não está no PATH. Na raiz do repositório, com a pasta `tools/medidor-voz/build` fora do Git:

```powershell
$cmake = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
& $cmake -S tools/medidor-voz -B tools/medidor-voz/build -G "Visual Studio 17 2022" -A x64
& $cmake --build tools/medidor-voz/build --config Release --parallel
```

## Como rodar

Sem argumentos, os dois programas usam as gravações de `test-audio/voz-usuario/`. Também é possível indicar duas gravações WAV:

```powershell
tools\medidor-voz\build\medidor_artefacts\Release\medidor.exe > medicao.txt
tools\medidor-voz\build\resumo_artefacts\Release\resumo.exe "voz-a.wav" "voz-b.wav" > resumo.txt
```

Cada programa leva cerca de 1 minuto. As gravações são comparadas entre si, então fazem sentido duas tomadas ou dois microfones da mesma voz.

## O que cada programa mede

| Programa | Mede |
|---|---|
| `medidor` | Gravação inteira; estabilidade com 10, 20, 30 e 45 s em janelas separadas; concordância do YIN com um segundo estimador; comparação entre as gravações; picos inseridos, no método atual e na reconstrução da D73 |
| `resumo` | Pontos de saturação; espectro entre janelas, por faixa; candidatas que se repetem; contagem de picos encontrados |

| `banco` | Vozes de um banco com pastas por cantor, como o VocalSet: varredura de todos os cantores, detalhe de alguns e busca do ponto da gravação de onde saiu um resultado |
| `origem` | De onde vem um pico que se repete entre cantores: por vogal (`vogais`) ou fora da voz cantada (`fora-da-voz`) |
| `banco_sem_limite` | O `banco`, sem o limite de 6 vezes a fundamental (D83) |
| `banco_todas_candidatas` | O `banco`, sem o corte de 3 candidatas |

As duas variantes do `banco` são geradas na pasta de compilação, a partir do `VoiceAnalyzer.cpp`; o código do plugin não muda.

```powershell
$banco = 'tools\medidor-voz\build\banco_artefacts\Release\banco.exe'
& $banco varredura 'test-audio\vocalset\data_by_singer'
& $banco detalhe 'test-audio\vocalset\data_by_singer' female3 female7 male9 male8 male6 female3:escalas
& $banco inicio 'voz.wav' 38.1 142 87

# Regra da banda de ressonância (D87) e variantes; use a versão sem o corte de candidatas.
& 'tools\medidor-voz\build\banco_todas_candidatas_artefacts\Release\banco_todas_candidatas.exe' regra 'test-audio\vocalset\data_by_singer' 'voz.wav'

$origem = 'tools\medidor-voz\build\origem_artefacts\Release\origem.exe'
& $origem vogais 'test-audio\vocalset\data_by_singer' 5500 8500
& $origem fora-da-voz 'test-audio\vocalset\data_by_singer' 5500 8500 'voz.wav'
```

Por padrão, o `banco` usa os trechos cantados de cada cantor (`excerpts`, sem a fala). Com `:escalas` depois do nome, usa arpejos e escalas sem efeitos. No modo `inicio`, os números são os segundos de captura, a mediana e o grave (5%) vistos no DAW.

**A D73 dentro do `medidor`** é uma reconstrução do método antigo, porque o código original não foi guardado. Serve só para comparar.

**Para simular uma mudança antes de aprová-la:** altere a constante numa cópia local, meça e desfaça, sem levar a mudança para o plugin (D81).
