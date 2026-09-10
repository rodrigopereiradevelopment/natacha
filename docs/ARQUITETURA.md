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

## Nucleo operacional (ego)

O core neste repositorio e **C++17** (`CMakeLists.txt`), nao C++20: RAII, treino proprio (neuronio, MLP, skip-gram), persistencia JSON/bin. Dependencias ja usadas: nlohmann/json. Google Test, CUDA, llama.cpp, cpr e spdlog aparecem como intencao de stack — confirmar no CMake antes de tratar como entregue.

No mapa da casa, esse nucleo e o **ego**: executa a logica; Felix+Sentinel sao o **superego**; o banheiro e o **id** (purga, pausa). Ver `CASA.md`.

Falhas de treino atualizam a funcao de perda (vontade de potencia): nao apagam identidade nem codigo antigo.

## Stack Tecnologico

| Camada | Tecnologia | Neste repo |
|--------|------------|------------|
| Linguagem | C++17 | Sim |
| Build | CMake | Sim |
| JSON | nlohmann/json | Sim |
| Embeddings | Skip-gram + negative sampling (`word2vec_neg`, `--decay`) | Sim |
| Corpus | `dados/embeddings/corpus.txt` (~25k palavras) | Sim |
| Testes | Google Test | Planejado / parcial |
| HTTP | cpr | Planejado |
| Logging | spdlog | Planejado |
| GPU | CUDA | Futuro |
| Modelo local | llama.cpp | Futuro |
| Sentinel | Rust | Outro projeto / planejamento |
| ARCA | PWA (Ionic/Next), Mogi Mirim | Fora deste repo |
| EditeCC | ABNT / Next+Tauri | Fora deste repo |

## Pipeline de Dados

Hoje (Fase 3): texto do corpus → tokens por espaco → skip-gram + negative sampling → vetores `.bin`/`.json`.

Alvo: Texto bruto → Tokenizacao → Embeddings → Transformer → Saida → Detokenizacao → Resposta da Natacha.

O corpus mistura identidade da Natacha, casa, Felix, C++/Python/Rust/Qiskit, folclore local e literatura (Dostoievski, Machado, Nietzsche, Raul Seixas). Acentos ainda nao sao uniformes.

## Regra de Ouro de Seguranca

Nunca acesso direto ao banco. So funcoes controladas:

- buscar_produto(nome) ✅
- comparar_precos(produto_id) ✅
- criar_lista(usuario_id, itens) ✅
- executar_sql_qualquer_coisa(sql) ❌
