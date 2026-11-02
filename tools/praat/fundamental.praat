# Mede fundamental e espectro de um WAV no Praat, para comparar com o Lapidamix (D77).
# Usa a mesma faixa do perfil de voz do plugin: 60 a 1100 Hz.
# Uso: Praat.exe --run fundamental.praat "voz.wav"
# Imprime na 1a linha: mediana, percentil de 5%, percentil de 95%, segundos com voz e
# parte dos trechos com voz acima de 400 Hz.
# Na 2a linha: espectro médio em dB, em terços de oitava de 125 Hz a 8 kHz.

form Fundamental
    sentence wav
endform

som = Read from file: wav$
pitch = To Pitch: 0.01, 60, 1100
mediana = Get quantile: 0, 0, 0.50, "Hertz"
grave = Get quantile: 0, 0, 0.05, "Hertz"
aguda = Get quantile: 0, 0, 0.95, "Hertz"
sonoros = Count voiced frames
acima = 0
quadros = Get number of frames
for quadro to quadros
    valor = Get value in frame: quadro, "Hertz"
    if valor <> undefined and valor > 400
        acima += 1
    endif
endfor
writeInfoLine: mediana, ",", grave, ",", aguda, ",", sonoros * 0.01, ",", acima / sonoros

selectObject: som
ltas = To Ltas: 20
linha$ = ""
for banda from -9 to 9
    centro = 1000 * 2 ^ (banda / 3)
    valor = Get mean: centro / 2 ^ (1 / 6), centro * 2 ^ (1 / 6), "energy"
    linha$ = linha$ + string$ (valor) + ","
endfor
appendInfoLine: linha$
removeObject: som, pitch, ltas
