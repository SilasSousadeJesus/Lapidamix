# 08 - Plano de Testes

🟡 Proposta inicial, a refinar ao longo do projeto.

## Material de teste
- **Voz principal:** minha voz seca no SM7dB, sem nenhum efeito, ~30 s cantados.
- **Variedade:** vozes graves e agudas, masculinas e femininas, microfones diferentes (dinâmicos e condensadores), gravações boas e problemáticas. Meta: de 3 a 5 vozes bem diferentes na Fase 3 e pelo menos 20 vozes diferentes na Fase 5 (D38).
- Arquivos de áudio de teste ficam fora do Git (são grandes); registrar aqui de onde vieram.

## O que verificar

**Técnico** (no Reaper, o DAW de testes — D21)
- Nenhum estalo, clique ou travamento durante a reprodução.
- Uso de CPU baixo.
- Parâmetros salvos e recuperados ao reabrir o projeto.
- Bypass sem diferença de volume nem atraso perceptível.

**Sonoro**
- Embolamento reduzido sem a voz ficar "fina".
- Mais clareza e presença sem soar estridente.
- Nenhuma correção acima dos limites de segurança.
- Fundamental preservada (peso da voz mantido).
- Teste A/B com bypass: o resultado soa melhor que o original.
- Teste "às cegas": ouvir sem saber qual versão é qual.

**Adaptação**
- Vozes diferentes recebem correções diferentes e coerentes.
- Uma voz que já está boa recebe pouca ou nenhuma correção.

## Registro de testes

| Data | Voz / material | Versão do plugin | O que foi testado | Resultado | Observações |
|---|---|---|---|---|---|
| 02/11/2026 | Voz do usuário, no Reaper | 0.1.0 (plugin vazio) | Os 5 passos da D53: carregar, conferir a lista, ouvir com e sem o plugin, teste de cancelamento, salvar e reabrir | ✅ Todos passaram | Teste feito e informado pelo usuário. Encerra a Fase 1 |
| 02/11/2026 | Sinais gerados por programa temporário, sem DAW | 0.1.0 (com EQ) | Transparência bit a bit, curvas das 5 bandas, bypass, ganho de saída, extremos de 22,05 a 192 kHz, denormais, mono, salvamento e pedidos de memória na thread de áudio | ✅ Todas as verificações passaram | Feito por Claude, em compilação Debug. Não substitui o teste no Reaper (D66) |
| 02/11/2026 | Voz do usuário e ruído, no Reaper, em 44,1 e 96 kHz | 0.1.0 (com EQ) | Os 5 passos da D66: transparência, curva de cada banda, mexer durante a reprodução, salvar e reabrir, mixar a voz à mão | ✅ Todos passaram | Teste feito e informado pelo usuário. Encerra a Fase 2 |
