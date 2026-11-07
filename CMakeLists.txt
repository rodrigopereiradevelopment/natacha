cmake_minimum_required(VERSION 3.15)
project(Natacha VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Diretorios
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CORE_DIR ${SRC_DIR}/core)

# Executavel principal
add_executable(natacha
    ${SRC_DIR}/main.cpp
    ${CORE_DIR}/neuronio.cpp
)

# Inclui headers
target_include_directories(natacha PRIVATE ${INCLUDE_DIR})

# Flags de compilacao (debug + otimizacao basica)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(natacha PRIVATE -Wall -Wextra -O2)
elseif(MSVC)
    target_compile_options(natacha PRIVATE /W4 /O2)
endif()

# Mensagem de boas-vindas
message(STATUS "============================================")
message(STATUS "  Natacha — Build Configurado")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compilador: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "============================================")

# Tokenizer — biblioteca independente
find_package(PkgConfig REQUIRED)
pkg_check_modules(ICU REQUIRED IMPORTED_TARGET icu-uc)

add_library(natacha_tokenizer
    ${SRC_DIR}/embeddings/tokenizer.cpp
)

target_include_directories(natacha_tokenizer
    PUBLIC ${INCLUDE_DIR}
)

target_link_libraries(natacha_tokenizer
    PUBLIC PkgConfig::ICU
)

target_compile_options(natacha_tokenizer PRIVATE
    -Wall -Wextra -Wpedantic
)

# Testes do Tokenizer
enable_testing()

add_executable(test_tokenizer
    ${CMAKE_SOURCE_DIR}/tests/tokenizer/test_tokenizer.cpp
)

target_link_libraries(test_tokenizer
    PRIVATE natacha_tokenizer
)

add_test(NAME tokenizer COMMAND test_tokenizer)
