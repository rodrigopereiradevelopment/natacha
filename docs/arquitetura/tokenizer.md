# Tokenizer — Natacha

## Objetivo

O Tokenizer é responsável pela normalização Unicode e pela transformação de textos em tokens.

Foi desenvolvido em C++17 com ICU, preservando informações linguísticas e técnicas importantes para o projeto Natacha.

## Modos de operação

### NATACHA

Modo padrão, voltado à preservação de informações.

Características:
- Converte letras para minúsculas por padrão.
- Preserva acentos por padrão.
- Reconhece letras e números Unicode.
- Preserva identificadores técnicos.
- Mantém conectores internos: hífen, apóstrofo e ponto.
- Reconhece sufixos técnicos `+` e `#`.
- Preserva identificadores com sublinhado.
- Tamanho mínimo padrão: 1.
- Tamanho máximo padrão: sem limite.

Exemplos:

| Entrada | Token |
|---|---|
| Word2Vec | word2vec |
| C++ | c++ |
| C# | c# |
| Next.js | next.js |
| bem-estar | bem-estar |
| d'água | d'água |
| minha_variavel | minha_variavel |

### GENSIM_COMPAT

Modo destinado à compatibilidade com o pré-processamento utilizado no treinamento original do Word2Vec v38.

Referência: Gensim 4.4.0, utilizando:

`simple_preprocess(texto, deacc=False, min_len=2)`

Características:
- Converte para minúsculas por padrão.
- Preserva acentos.
- Utiliza regras Unicode para identificação de palavras.
- Preserva sublinhados internos.
- Descarta tokens iniciados por sublinhado.
- Divide identificadores técnicos conforme as regras de tokenização.
- Tamanho mínimo padrão: 2.
- Tamanho máximo padrão: 15.

A implementação foi comparada com o Gensim 4.4.0 em casos representativos de Unicode e identificadores.

Essa validação não representa equivalência formal para todos os caracteres Unicode.

## Configuração

A configuração é realizada por `ConfigTokenizer`.

Parâmetros:

| Parâmetro | Padrão | Descrição |
|---|---|---|
| modo | NATACHA | Modo de tokenização |
| converter_minusculas | true | Converte o texto para minúsculas |
| remover_acentos | false | Remove marcas diacríticas após decomposição Unicode |
| tamanho_minimo | automático | Limite mínimo do token |
| tamanho_maximo | automático | Limite máximo do token |

Os limites podem ser personalizados.

Quando `tamanho_minimo` ou `tamanho_maximo` não são informados, são utilizados os padrões do modo escolhido.

Uma configuração com tamanho mínimo maior que o máximo gera `std::invalid_argument`.

## API

Arquivo de cabeçalho:

`include/natacha/tokenizer.hpp`

Implementação:

`src/embeddings/tokenizer.cpp`

Namespace:

`natacha`

Operações disponíveis:

- `normalizar(texto)`: normaliza o texto e retorna uma string UTF-8.
- `tokenizar(texto)`: transforma um texto em uma lista de tokens.
- `tokenizarLote(textos)`: processa vários textos e retorna uma lista de listas de tokens.

## Unicode e ICU

A implementação utiliza ICU para:

- Conversão entre UTF-8 e UnicodeString.
- Classificação de caracteres Unicode.
- Conversão para minúsculas.
- Decomposição Unicode NFD.
- Remoção opcional de marcas diacríticas.

A remoção de acentos é opcional e não deve ser confundida com o comportamento padrão do treinamento v38.

## Integração com Word2Vec v38

O treinamento original foi realizado em Python com Gensim 4.4.0.

O pipeline original utiliza:

- Leitura das fontes linha por linha.
- Ignorar linhas vazias.
- Ignorar linhas iniciadas por `#`.
- Tokenização individual de cada linha.
- Descartar frases com menos de três tokens.
- Repetições configuradas por fonte.
- Parâmetros de treinamento preservados no script original.

O Tokenizer implementa somente a normalização e a tokenização.

A leitura dos arquivos, repetição das fontes, filtragem de frases e montagem do corpus são responsabilidades de um futuro componente CorpusLoader.

O objetivo é migrar o pipeline para C++ sem confundir tokenização com preparação do corpus.

## Dependências

- C++17
- GCC ou compilador C++17 compatível
- ICU `icu-uc`
- CMake 3.15 ou superior

## Compilação e testes

Configuração:

`cmake -S . -B build-tokenizer`

Compilação:

`cmake --build build-tokenizer --target test_tokenizer -j2`

Execução dos testes:

`ctest --test-dir build-tokenizer --output-on-failure`

## Cobertura atual

A suíte de testes contempla:

1. Normalização e conversão para minúsculas.
2. Preservação de acentos.
3. Remoção opcional de acentos.
4. Preservação de informações no modo NATACHA.
5. Identificadores técnicos e palavras compostas.
6. Casos representativos de compatibilidade com Gensim 4.4.0.
7. Texto vazio e pontuação.
8. Limites personalizados.
9. Configuração inválida.
10. Processamento em lote.

## Estado

Implementação inicial concluída.

Biblioteca independente configurada no CMake.

Testes automatizados registrados no CTest.

A integração com o pipeline completo de migração do Word2Vec v38 permanece como próxima etapa.
