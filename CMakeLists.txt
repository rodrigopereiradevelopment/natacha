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
