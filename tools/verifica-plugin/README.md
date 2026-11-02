# Verificação do plugin sem DAW

Roda o plugin de verdade, sem o Reaper, e confere automaticamente o que dá para medir (D84). Sem áudio nesta pasta: usa as gravações de `test-audio/voz-usuario/`.

```powershell
$cmake = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
& $cmake -S tools/verifica-plugin -B tools/verifica-plugin/build -G "Visual Studio 17 2022" -A x64
& $cmake --build tools/verifica-plugin/build --config Release --parallel
tools\verifica-plugin\build\verifica_artefacts\Release\verifica.exe
```

Cada verificação imprime `OK` ou `FALHOU`, e o programa termina com código 1 se alguma falhar.

**O que confere:** saída idêntica à entrada com análise e RTA ligados; nenhum pedido de memória na thread de áudio; análise medindo antes do EQ; RTA reagindo ao EQ; aviso com menos de 10 s; salvar e restaurar; interromper a análise; ressonância artificial em 2,2 kHz, com um filtro do JUCE parecido com o ReaEQ.

**O que não confere:** comportamento dentro do Reaper, estalos de verdade e qualquer teste de ouvido. O tempo por bloco é só um indício do uso de CPU. O filtro da ressonância não é o ReaEQ.
