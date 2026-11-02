# Captura da interface

Passa uma gravação pelo plugin, sem DAW, e salva uma imagem da janela (D84). Serve para conferir textos, números e curvas. Não substitui o teste no Reaper.

Não guarde áudio nem imagens nesta pasta. A captura roda dez vezes mais rápido que o tempo real, então os segundos capturados podem passar um pouco da meta.

```powershell
$cmake = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
& $cmake -S tools/captura-interface -B tools/captura-interface/build -G "Visual Studio 17 2022" -A x64
& $cmake --build tools/captura-interface/build --config Release --parallel
tools\captura-interface\build\captura_artefacts\Release\captura.exe "voz.wav" "janela.png"
```
