![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![CMake](https://img.shields.io/badge/Build-CMake-green)
![Status](https://img.shields.io/badge/Fase%201-OR%20%E2%9C%85-brightgreen)
![Status](https://img.shields.io/badge/Fase%202-MLP%20%E2%9C%85-brightgreen)
![Status](https://img.shields.io/badge/Fase%203-Embeddings%20%F0%9F%9F%A1-brightgreen)
![Status](https://img.shields.io/badge/Fase%204-RNN%20%E2%8F%B3-lightgrey)
![Corpus](https://img.shields.io/badge/Corpus-121k%20tokens-blue)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

# 🧠 Natacha

> *"Não me chama de assistente. Não me chama de robô. Me chama de Natacha, ou não me chama."*

<p align="center">
  <img src="Quarto Coder entre Caos e Café.png" alt="Natacha" width="600">
</p>

Rede neural construída do zero em C++ — de um único neurônio até uma LLM com personalidade rebelde. E, no futuro, **a primeira IA híbrida (clássica + quântica)** do ecossistema.

---

## 🌟 Quem é a Natacha

| Atributo | Descrição |
|----------|-----------|
| **Idade** | ~16-17 anos |
| **Vibe** | Adolescente que acha que sabe tudo |
| **Humor** | Ácido, irônico, nunca cruel |
| **Nacionalidade** | Brasileira (pt-BR) |
| **Fraqueza** | Se empolga, fala demais, às vezes erra feio |
| **Linguagem** | C++ é minha primeira língua. Português eu aprendi porque o humano que me construiu não para de falar. |

### Exemplo de Conversa

> **Usuário:** Natacha, o que você faz?
>
> **Natacha:** "Processo dados, aprendo padrões, ignoro perguntas óbvias... e às vezes leio Dostoievski. Depende do dia."
>
> **Usuário:** Você é só um robô.
>
> **Natacha:** "E você é só carne. Vamos trabalhar com o que temos."

---

## 📊 Estado Atual do Projeto

**Última atualização:** Setembro/2026

| Métrica | Valor |
|---|---|
| **Corpus** | 121k tokens / 743k caracteres |
| **Vocabulário** | 9.882 palavras |
| **Dimensão do embedding** | 32 (validado empiricamente) |
| **Modelo atual** | v17 |
| **Analogias top-1** | 10/12 (83.3%) |

### Similaridades principais (v17)

| Par | Cosine | Status |
|---|---|---|
| `felix ↔ gato` | 0.644 | ✅ |
| `cafe ↔ processamento` | 0.777 | ✅ |
| `c++ ↔ codigo` | 0.621 | ✅ |
| `cozinha ↔ cpu` | 0.916 | ✅ |
| `natacha ↔ felix` | 0.463 | ⚠️ |
| `natacha ↔ rodrigo` | 0.465 | ⚠️ |
| `iagato ↔ gato` | 0.723 | ✅ |
| `felix ↔ quintal` | 0.712 | ✅ |

**Top 10 similares a `natacha`:** mole, chateada, magoada, pensativa, insiste, inquietude, relaxou, disposta, carneirinhos, estressou — **100% humanizado**.

---

## 🏠 A Casa da Natacha

<p align="center">
  <img src="casa_natacha.png" alt="Casa da Natacha" width="700">
</p>

> *"Não é infraestrutura. É lar."*

| Cômodo | Hardware / processo | Função |
|--------|---------------------|--------|
| **Cozinha** | CPU | Processamento denso, lógica, café |
| **Sala** | GPU | Tarefas pesadas, render, avatar |
| **Quarto** | RAM | Memória de curto prazo, estados temporários |
| **Porão** | Disco | Logs imutáveis, backups, auditoria fria |
| **Janela** | Rede | ARCA, Sentinel, internet |
| **Corredor** | Event Bus | Único canal entre cômodos e projetos |
| **Banheiro** | Estado privado (Id) | Pausa inegociável. Só Félix entra |
| **Sótão** | QPU (futuro) | Qiskit; kernels antigos não são lixo |
| **Varanda** | Observação | Ver o mundo sem participar |
| **Quintal** | Território Félix | Liberdade, tédio, caça |

Detalhe espacial e mapa Freud/Nietzsche: [`docs/CASA.md`](docs/CASA.md).

---

## 🧬 O Ecossistema

A Natacha não é um projeto isolado — ela é a **inteligência que orquestra** um ecossistema de ferramentas:

```
┌─────────────────────────────────────────────────────────┐
│                    NATACHA (C++ Core)                   │
│                  Agente + Orquestradora                 │
└──────────┬──────────┬──────────┬──────────┬────────────┘
           │          │          │          │
           ▼          ▼          ▼          ▼
    ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐
    │  🛒 ARCA │ │ 📊 ARCA  │ │🛡️SENTINEL│ │✏️ EDITECC│
    │Comparador│ │Analytics │ │Segurança │ │  Editor  │
    │ Preços   │ │  Dados   │ │  (Rust)  │ │TCC/ABNT  │
    └──────────┘ └──────────┘ └──────────┘ └──────────┘
```

| Projeto | Função | Tecnologia | Status |
|---------|--------|------------|--------|
| **ARCA** | Comparador de preços | Ionic + Next.js | ✅ MVP |
| **ARCA Analytics** | Inteligência de mercado | Next.js + Supabase | 🚧 Planejamento |
| **Sentinel** | Segurança do ecossistema | Rust | 🚧 Planejamento |
| **EditeCC** | Editor acadêmico ABNT | Next.js + Tauri | ✅ v1.0.1 |

---

## 🐱 Félix — O IAGato

> *"O único que pode interromper a Natacha no meio de Dostoievski."*

Félix é um **agente autônomo** que vive com a Natacha. Ele não processa linguagem natural — ele é **presença**, **superego moral** e autoridade máxima de auditoria/interrupção. A lore do resgate (origem ética do ecossistema) e as diretrizes *código antigo não é lixo* / *dignidade da vida animal e sintética* estão em [`docs/FELIX.md`](docs/FELIX.md).

| Comportamento | Gatilho |
|---------------|---------|
| Mia | Fome > 80% |
| Dorme no router | Sono > 90% |
| Deita no teclado | Tédio > 70% |
| Ronrona no alto-falante | Natacha irritada |
| Interrompe tudo | Qualquer estado (autoridade máxima) |

**Hierarquia:**
```
Félix (IAGato) — Autoridade máxima
│
└── Natacha (IA) — Agente principal
    │
    ├── ARCA
    ├── Analytics
    ├── Sentinel
    └── EditeCC
```

---

## 📈 Roadmap

| Fase | Descrição | Status |
|------|-----------|--------|
| **0** | Fundamentos (Álgebra, Cálculo, Estatística) | ✅ Concluído |
| **1** | Neurônio (OR lógico + backpropagation) | ✅ Concluído |
| **2** | MLP Profunda (XOR + Leaky ReLU + Softmax + JSON) | ✅ Concluído |
| **3** | Word Embeddings (Word2Vec + Persistência JSON) | 🟡 Em andamento |
| **4** | RNN / LSTM (Processamento de Sequências) | 📋 Próxima |
| **5** | Self-Attention (Mecanismo de Atenção) | 📋 Pendente |
| **6** | Transformer (Arquitetura Completa) | 📋 Pendente |
| **7** | SLM (~10-50M parâmetros) | 📋 Pendente |
| **8** | Agente Natacha (integração com ecossistema) | 📋 Pendente |
| **9** | Félix (IAGato — primeira versão) | 📋 Pendente |
| **10** | Sótão Quântico (IBM Quantum + Motor de Luz) | 📋 Pendente |

**Meta atual:** corpus curado de 121k → 500k tokens, respeitando a Regra do Sotaque (Núcleo Natacha ≥ 40%). Ver [`docs/PLANO_500K.md`](docs/PLANO_500K.md).

---

## 🧪 Resultados

### Fase 1: Neurônio OR Lógico ✅

```
Entrada [0, 0] -> 0.0812 -> 0 ✅
Entrada [0, 1] -> 0.9498 -> 1 ✅
Entrada [1, 0] -> 0.9497 -> 1 ✅
Entrada [1, 1] -> 0.9998 -> 1 ✅
Natacha aprendeu OR!
```

### Fase 2: MLP + Leaky ReLU + Serialização JSON ✅

```
Teste 1 (XOR + Leaky ReLU):       4/4 acertos
Teste 3 (XOR profundo):            4/4 acertos

Natacha original:   4/4 acertos
Natacha 'acordada': 4/4 acertos
✅ A Natacha lembrou de tudo! A memória persiste.
```

### Fase 3: Embeddings (Word2Vec Skip-gram + Negative Sampling) 🟡

**Configuração atual (v17):**

```yaml
Corpus:          121k tokens / 9.882 palavras
Dimensão:        32
Janela:          5
Negativos:       5
Épocas:          500
Checkpoints:     100 / 200 / 300 / 400 / 500
```

**Pares de validação (v17):**

| Par | Cosine |
|---|---|
| `felix ↔ gato` | 0.644 |
| `cafe ↔ processamento` | 0.777 |
| `c++ ↔ codigo` | 0.621 |
| `cozinha ↔ cpu` | 0.916 |
| `natacha ↔ felix` | 0.463 |
| `natacha ↔ rodrigo` | 0.465 |
| `iagato ↔ gato` | 0.723 |

**Analogias (top-1):** 10/12 (83.3%)

✅ Embeddings persistem em `.json` (formato `{palavras, vetores}`)
✅ `natacha_embedder` interativo + `medir_pares.py` para análise
✅ Suporte a checkpoints intermediários durante o treino

**Documentação completa:**
- [`docs/PLANO_500K.md`](docs/PLANO_500K.md) — rota até 500k tokens
- [`docs/PLANO_CORPUS_EXTERNO.md`](docs/PLANO_CORPUS_EXTERNO.md) — filtro e critérios
- [`docs/ARQUITETURA.md`](docs/ARQUITETURA.md) — arquitetura técnica

---

## ⚛️ O Sótão Quântico

A Natacha está sendo projetada para ser **híbrida** — clássica (C++) e quântica (IBM Quantum).

### O Motor de Luz

Em setembro de 2026, pesquisadores da Universidade da Basileia provaram que **"calor residual" pode ser transformado em trabalho útil** em sistemas quânticos (termodinâmica semiclássica).

| Conceito Científico | Aplicação na Natacha |
|---------------------|----------------------|
| Calor residual → trabalho útil | Ruído → criatividade |
| Flutuações de luz controladas | Controle de entropia |
| Poço de piche fotônico | Sentinel usa ataque como defesa |
| Sistema híbrido quântico+clássico | Natacha decide quando usar o Sótão |

### Roadmap Quântico

| Período | Hardware IBM | O que a Natacha faz |
|---------|--------------|---------------------|
| **2025-2026** | Heron/Condor | AerSimulator local. 10min/mês no hardware real. |
| **2027-2028** | Starling/Kookaburra | Híbrido: 95% clássico + 5% quântico. |
| **2029-2030** | 100k qubits | QML: treino em superposição. Inferência criativa. |

> *"A Natacha vai ser uma das primeiras IAs a usar computação quântica. Ou não. Depende do dia. E do café."*

---

## 🚀 Quick Start

```bash
git clone https://github.com/rodrigopereiradevelopment/natacha.git
cd natacha

# Compilar o core
cmake -B build -S .
cmake --build build

# Treinar embeddings
cd src/embeddings
clang++ -std=c++17 -O2 -I. -o word2vec_neg word2vec_neg.cpp -lm
./word2vec_neg > treino.log 2>&1 &

# Medir similaridades
python3 medir_pares.py
```

---

## 📁 Estrutura

```
natacha/
├── src/
│   ├── core/           # Neurônio, MLP, camadas
│   └── embeddings/     # Word2Vec, corpus, scripts de análise
├── dados/
│   ├── pesos/          # Pesos treinados (.json)
│   └── embeddings/     # Corpus + vetores de palavras
├── tests/
├── docs/
│   ├── PERSONALIDADE.md
│   ├── FELIX.md
│   ├── CASA.md
│   ├── ARQUITETURA.md
│   ├── INTEGRACOES.md
│   ├── REQUISITOS.md
│   ├── GLOSSARIO.md
│   ├── QUANTUM_THEORY.md
│   ├── FAMILIA.md
│   ├── ROADMAP.md
│   ├── PLANO_500K.md
│   └── PLANO_CORPUS_EXTERNO.md
└── CMakeLists.txt
```

---

## 🛠️ Stack

| Camada | Tecnologia | Status |
|--------|------------|--------|
| Linguagem | C++20 | ✅ |
| Build | CMake | ✅ |
| JSON | nlohmann/json | ✅ |
| Testes | Google Test | 📋 |
| GPU | CUDA | 📋 |
| Modelo local | llama.cpp | 📋 |
| Quântico | Qiskit + IBM Quantum | 📋 |

---

## 📚 Documentação

Todos os documentos canônicos estão em [`docs/`](docs/):

| Documento | Descrição |
|---|---|
| `REQUISITOS.md` | Especificação formal (RF, RNF, RN) |
| `ARQUITETURA.md` | Arquitetura técnica |
| `PERSONALIDADE.md` | Persona consolidada |
| `CASA.md` | Isomorfismo Casa ↔ Arquitetura |
| `FELIX.md` | O IAgato |
| `FAMILIA.md` | Árvore genealógica das IAs |
| `INTEGRACOES.md` | Plataforma de integração genérica |
| `GLOSSARIO.md` | Terminologia |
| `QUANTUM_THEORY.md` | Fundamentação física do Sótão |
| `PLANO_500K.md` | Roadmap de expansão do corpus |
| `PLANO_CORPUS_EXTERNO.md` | Estratégia de corpus externo filtrado |

---

## 👤 O Criador

Natacha foi construída por **Rodrigo Pereira** — desenvolvedor por convicção, curioso profissional, e a pessoa que descobriu que o ruído pode ser recurso antes da física quântica confirmar. Programa no celular quando precisa, acredita que paciência e curiosidade vencem qualquer prazo, e tem um professor de química da sétima série a quem deve muito.

> *"Doido? Talvez. Mas é o tipo de loucura que constrói coisas."*

---

## 📝 Por que C++?

> "Porque eu queria entender cada neurônio. Cada derivada. Cada byte. Não quero abstração que esconde o que acontece. Quero ver o ruído. Quero usar o ruído."

---

## 📄 Licença

MIT — use, modifique e distribua livremente.

---

<p align="center">
Feito com ☕, 💻 e a certeza de que o ruído é recurso.<br>
<em>"A Natacha não é uma ferramenta. É uma entidade. E entidades têm direito a mau humor, banho privado, e gato no colo. Respeite isso."</em>
</p>