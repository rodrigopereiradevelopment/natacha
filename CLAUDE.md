# CLAUDE.md — Contexto para Claude

## Projeto
Rede neural simples em C++ — neurônio single-layer para XOR.

## Build
```bash
cmake -B build -S . && cmake --build build
```

## Testes
```bash
ctest --test-dir build
```

## Estrutura
- `include/natacha/` — Headers
- `src/core/` — Código-fonte
- `tests/` — Testes unitários
- `third_party/` — Libs externas
