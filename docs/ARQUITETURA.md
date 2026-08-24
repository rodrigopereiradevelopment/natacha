# ARQUITETURA Tecnica

> "De neuronio a Transformer, tijolo por tijolo."

## Evolucao do Modelo

Natacha sera construida em fases progressivas:

FASE 0: Fundamentos
- Algebra Linear (vetores, matrizes)
- Calculo (derivadas, gradientes)
- Estatistica (probabilidades)

FASE 1: Neuronio Simples
- Perceptron
- Funcoes de ativacao (step, sigmoid, ReLU)
- Dataset: OR, AND, XOR logico

FASE 2: MLP (Multilayer Perceptron)
- Camadas ocultas
- Backpropagation
- Funcao de custo (MSE)

FASE 3: Word Embeddings
- Representacao vetorial de palavras
- Skip-gram / CBOW

FASE 4: RNN / LSTM
- Processamento de sequencias
- Memoria de curto prazo

FASE 5: Self-Attention
- Mecanismo de atencao
- Query · Key · Value

FASE 6: Transformer
- Multi-Head Attention
- Feed-Forward Network
- Add & Norm

FASE 7: LLM Miniatura
- Treinamento com dados reais
- ~10-50M parametros
- Inferencia local

FASE 8: Escalar
- Mais dados, mais parametros
- GPU (CUDA)
- Modelos pre-treinados (llama.cpp)

## Stack Tecnologico

| Camada | Tecnologia |
|--------|------------|
| Linguagem | C++17 |
| Build System | CMake |
| Testes | Google Test |
| JSON | nlohmann/json |
| HTTP | cpr |
| Logging | spdlog |
| GPU (futuro) | CUDA |
| Modelo Local (futuro) | llama.cpp |

## Pipeline de Dados

Texto bruto → Tokenizacao → Embeddings → Transformer → Saida → Detokenizacao → Resposta da Natacha

## Regra de Ouro de Seguranca

Nunca acesso direto ao banco. So funcoes controladas:

- buscar_produto(nome) ✅
- comparar_precos(produto_id) ✅
- criar_lista(usuario_id, itens) ✅
- executar_sql_qualquer_coisa(sql) ❌
