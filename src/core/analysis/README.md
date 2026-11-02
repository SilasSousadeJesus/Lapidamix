# Análise

Análise do áudio recebido (D28, D69 a D76). Só `AudioCapture::push` roda na thread de áudio; o resto roda numa thread separada e não depende da interface.

- `AudioCapture`: guarda a entrada em mono.
- `YinPitchDetector`: detecta a fundamental.
- `VoiceAnalyzer`: separa voz e som, calcula as duas médias e as ressonâncias candidatas.
- `AnalysisController`: coordena a captura e a análise.
- `AnalysisResult`: as medidas, salvas com o projeto.
