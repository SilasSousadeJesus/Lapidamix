# Compara fundamental e espectro do Lapidamix com os do Praat (D77). Sem áudio no repositório:
# os trechos exportados ficam em test-audio/praat/, fora do Git.
# Uso, na raiz do repositório: powershell -File tools\praat\comparar.ps1

param(
    [string] $Praat = "$env:LOCALAPPDATA\Programs\Praat\Praat.exe",
    [string] $Banco = 'tools\medidor-voz\build\banco_artefacts\Release\banco.exe',
    [string] $Saida = 'test-audio\praat',
    [double] $Tolerancia = 3.0
)

New-Item -ItemType Directory -Force $Saida | Out-Null
$raiz = (Resolve-Path $Saida).Path
$script = (Resolve-Path 'tools\praat\fundamental.praat').Path
$cultura = [Globalization.CultureInfo]::InvariantCulture

& $Banco exportar 'test-audio\vocalset\data_by_singer' $raiz female3 female7 male9 male8 male6 `
    'test-audio\voz-usuario\VOZ-CANTADA-SHURE-SM7DB.wav' 'test-audio\voz-usuario\VOZ-CANTADA-SHURE-SM57.wav'

# Mesma grade do plugin: 240 pontos de 20 Hz a 20 kHz.
function Get-PluginBand([double[]] $curva, [double] $centro) {
    $soma = 0.0; $n = 0
    for ($p = 0; $p -lt 240; $p++) {
        $f = 20.0 * [math]::Pow(1000.0, $p / 239.0)
        if ($f -ge $centro / [math]::Pow(2, 1 / 6) -and $f -le $centro * [math]::Pow(2, 1 / 6)) {
            $soma += [math]::Pow(10, $curva[$p] / 10); $n++
        }
    }
    10 * [math]::Log10($soma / $n)
}

$foraDaTolerancia = 0
'voz; captura s; plugin mediana / praat / dif; plugin p5 / praat / dif; plugin p95 / praat / dif; acima de 400 Hz no praat; espectro: dif. rms / maior (dB)'

foreach ($linha in Import-Csv "$raiz\plugin.csv") {
    $saidaPraat = "$raiz\saida.txt"
    # O Praat escreve a saída em UTF-16; Start-Process espera ele terminar.
    Start-Process -FilePath $Praat -ArgumentList ('--run "{0}" "{1}"' -f $script, "$raiz\$($linha.arquivo)") `
        -Wait -NoNewWindow -RedirectStandardOutput $saidaPraat
    $linhas = @(Get-Content $saidaPraat -Encoding Unicode | Where-Object { $_ -match ',' })
    $resultado = $linhas[0].Split(',')
    $bandas = $linhas[1].TrimEnd(',').Split(',')
    $partes = @()

    foreach ($campo in @(@('mediana', 0), @('p5', 1), @('p95', 2))) {
        $a = [double]::Parse($linha.($campo[0]), $cultura)
        $b = [double]::Parse($resultado[$campo[1]], $cultura)
        $dif = 100 * [math]::Abs($a - $b) / $b
        if ($dif -gt $Tolerancia) { $foraDaTolerancia++ }
        $partes += ('{0,6:N1} / {1,6:N1} / {2,5:N2}%{3}' -f $a, $b, $dif, $(if ($dif -gt $Tolerancia) { ' FORA' } else { '' }))
    }

    $curva = [double[]] ($linha.curva.Trim().Split(' ') | ForEach-Object { [double]::Parse($_, $cultura) })
    $difs = @()
    for ($i = 0; $i -lt 19; $i++) {
        $centro = 1000 * [math]::Pow(2, ($i - 9) / 3)
        $difs += (Get-PluginBand $curva $centro) - [double]::Parse($bandas[$i], $cultura)
    }
    $media = ($difs | Measure-Object -Average).Average
    $forma = $difs | ForEach-Object { $_ - $media }
    $rms = [math]::Sqrt((($forma | ForEach-Object { $_ * $_ }) | Measure-Object -Average).Average)
    $maior = ($forma | ForEach-Object { [math]::Abs($_) } | Measure-Object -Maximum).Maximum

    '{0}; {1}; {2}; {3:P1}; {4:N2} / {5:N2}' -f $linha.nome, $linha.captura, ($partes -join '; '), [double]::Parse($resultado[4], $cultura), $rms, $maior
}

Remove-Item "$raiz\saida.txt" -ErrorAction SilentlyContinue
"medidas de fundamental fora de +-$Tolerancia%: $foraDaTolerancia"
