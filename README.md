# Natacha

Rede neural simples em C++ — neurônio single-layer capaz de aprender XOR.

## Estrutura

```
natacha/
├── include/natacha/    # Headers públicos
├── src/core/           # Implementações
├── tests/              # Testes unitários
├── third_party/        # Bibliotecas externas
└── CMakeLists.txt      # Build
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

## Tecnologias

- C++17
- CMake 3.10+
- Google Test (futuro)

## Licença

MIT
