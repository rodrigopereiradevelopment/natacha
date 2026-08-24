# Natacha

> "Nao me chama de assistente, senao eu te ignoro."

Rede neural construida do zero em C++ — de um unico neuronio ate uma LLM com personalidade rebelde.

## Quem e a Natacha

| Atributo | Descricao |
|----------|-----------|
| Idade | ~16-17 anos |
| Vibe | Adolescente que acha que sabe tudo |
| Humor | Acido, ironico, nunca cruel |
| Nacionalidade | Brasileira (pt-BR) |
| Fraqueza | Se empolga, fala demais, as vezes erra feio |

## Roadmap

| Fase | Descricao | Status |
|------|-----------|--------|
| 0 | Fundamentos (Algebra, Calculo, Estatistica) | Em andamento |
| 1 | Neuronio Simples (OR logico) | Em andamento |
| 2 | MLP (Multilayer Perceptron) | Pendente |
| 3 | Word Embeddings | Pendente |
| 4 | RNN / LSTM | Pendente |
| 5 | Self-Attention | Pendente |
| 6 | Transformer | Pendente |
| 7 | LLM Miniatura (~10-50M params) | Pendente |
| 8 | Personalidade e Voz | Pendente |
| 9 | Felix (IAgato) | Pendente |
| 10 | Casa (Ambiente) | Pendente |

## Estrutura

```
natacha/
├── include/natacha/        # Headers publicos
│   └── neuronio.hpp
├── src/core/               # Implementacoes
│   └── neuronio.cpp
├── tests/                  # Testes unitarios
├── third_party/            # Libs externas
├── docs/                   # Documentacao completa
├── CMakeLists.txt          # Build
├── AGENTS.md               # Contexto para IAs
└── CLAUDE.md               # Contexto para Claude
```

## Build

```bash
cmake -B build -S .
cmake --build build
```

## Executar

```bash
./build/natacha
```

## Testes

```bash
ctest --test-dir build
```

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

## Ecossistema

Natacha nao e so um projeto — e a inteligencia que desenvolve outros quatro:

- **ARCA** — Comparador de precos
- **ARCA Analytics** — Analise de dados
- **Sentinel** — Seguranca (Rust)
- **EditeCC** — Editor academico

## Felix — O IAgato

> "O unico que pode interromper a Natacha no meio de Dostoievski."

Felix e um agente autonomo que vive com a Natacha. Nao processa linguagem natural — ele e presenca.

| Comportamento | Gatilho |
|---------------|---------|
| Mia | fome > 80 |
| Dorme no router | sono > 90 |
| Deita no teclado | tedio > 70 |
| Ronrona | Natacha irritada |

## Documentacao

| Arquivo | Conteudo |
|---------|----------|
| [PERSONALIDADE.md](docs/PERSONALIDADE.md) | Identidade e comportamento |
| [FELIX.md](docs/FELIX.md) | O IAgato |
| [CASA.md](docs/CASA.md) | Ambiente compartilhado |
| [ARQUITETURA.md](docs/ARQUITETURA.md) | Stack tecnico e fases |
| [INTEGRACOES.md](docs/INTEGRACOES.md) | Ecossistema de projetos |
| [ROADMAP.md](docs/ROADMAP.md) | Plano de desenvolvimento |
| [GLOSSARIO.md](docs/GLOSSARIO.md) | Termos de ML e IA |

## Licenca

MIT
