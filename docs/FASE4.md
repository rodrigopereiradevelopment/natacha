# Fase 4 — RNN / LSTM

> *"O forward pass já passa pelos dados. Agora vem a parte em que a rede precisa aprender com o erro."*

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 2.1 | Data: Novembro/2026
Documentos canônicos relacionados: ROADMAP.md v3.1, ARQUITETURA.md v2.1, REQUISITOS.md v3.1, GLOSSARIO.md v2.1

A Fase 4 implementa uma rede recorrente com células LSTM para processamento de sequências.

## 4.1 — Forward Pass ✅

### O que foi implementado

- `LSTMCell` com 4 portas: forget, input, candidate, output
- `RNNLayer` que processa uma sequência de embeddings
- `EmbeddingManager` que carrega os embeddings do Word2Vec (JSON)
- `main_fase4.cpp` que testa o pipeline com uma frase

### Teste registrado
Embeddings carregados: 37.556 palavras, 64 dimensões
Token 'felix' -> vetor de dimensão 64
Token 'observa' -> vetor de dimensão 64
Token 'logs' -> vetor de dimensão 64
Token 'natacha' -> vetor de dimensão 64

Passo t=0 (felix) | [0..3]: 0.1346 -0.0799 0.0809 0.0507
Passo t=1 (observa) | [0..3]: 0.1940 -0.0313 0.0585 0.0483
Passo t=2 (logs) | [0..3]: 0.2247 -0.1012 0.0701 0.0172
Passo t=3 (natacha) | [0..3]: 0.1986 -0.0914 0.0979 -0.0705

### Como compilar

```bash
cd ~/natacha
g++ -std=c++17 -O2 \
    src/rnn/main_fase4.cpp \
    src/rnn/LSTMCell.cpp \
    src/rnn/RNNLayer.cpp \
    src/embeddings/EmbeddingManager.cpp \
    -I src -I src/embeddings \
    -o natacha_engine_fase4 -lm

./natacha_engine_fase4
```

## 4.2 — Treinamento com BPTT ✅

### O que foi implementado

- Backward pass (Backpropagation Through Time)
- Camada de saída (projeção de h_t para o vocabulário)
- Função de perda (cross-entropy)
- Otimizador com learning rate decay linear (0,003 → 0,0003)
- Loop de treino com avaliação por perplexidade

### Métricas registradas

```text
Corpus:           32.507 tokens (corpus narrativo + auxiliar)
Vocab Local:      1.559 palavras
Sequências:       27.774 (janelas de 30 palavras)
Learning Rate:    0.003 → 0.0003 (decay linear)
Épocas:           20
Perplexidade:     884,5 → 176,7 (época 15), estável em ~180
```

### Objetivo

Dado um contexto (ex: felix observa), prever a próxima palavra (ex: logs). A avaliação foi feita por perplexidade no corpus narrativo.

### Exemplo de geração

```text
Prompt: "felix miou na cozinha e natacha"
Saída:  "felix miou na cozinha e natacha pensa ela quarenta e sorriu de uma casa natacha para de treino minutos a producao e tempo de de dois"
```

A rede aprendeu a estrutura local (sujeito + verbo + objeto) e associações da Casa (cozinha=CPU, quarto=Felix). A coerência de longo prazo ainda é limitada pelo tamanho do corpus (32k tokens) e pela dimensão (64d).

### Estrutura

```text
src/rnn/
├── LSTMCell.hpp
├── LSTMCell.cpp
├── RNNLayer.hpp
├── RNNLayer.cpp
└── main_fase4.cpp
```
