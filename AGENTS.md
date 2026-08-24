# AGENTS.md — Contexto para IAs

## Visão Geral
Projeto C++ de rede neural simples com um único neurônio capaz de aprender XOR.

## Estrutura
```
natacha/
├── include/natacha/    # Headers públicos
├── src/core/           # Implementações
├── tests/              # Testes unitários
├── third_party/        # Libs externas
└── CMakeLists.txt      # Build
```

## Comandos
```bash
cmake -B build -S .          # Configurar
cmake --build build          # Compilar
ctest --test-dir build       # Rodar testes
```

## Convenções
- Namespace: `natacha`
- Headers: `.hpp` (pragma once)
- Implementações: `.cpp`
- Testes: `test_*.cpp`
