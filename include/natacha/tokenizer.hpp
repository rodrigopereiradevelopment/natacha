#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace natacha {

enum class ModoTokenizer {
    GENSIM_COMPAT,
    NATACHA
};

struct ConfigTokenizer {
    ModoTokenizer modo = ModoTokenizer::NATACHA;

    bool converter_minusculas = true;
    bool remover_acentos = false;

    // nullopt = utilizar o limite padrão do modo.
    std::optional<std::size_t> tamanho_minimo;
    std::optional<std::size_t> tamanho_maximo;
};

class Tokenizer {
public:
    explicit Tokenizer(const ConfigTokenizer& config = {});

    std::string normalizar(const std::string& texto) const;

    std::vector<std::string> tokenizar(
        const std::string& texto
    ) const;

    std::vector<std::vector<std::string>> tokenizarLote(
        const std::vector<std::string>& textos
    ) const;

private:
    ConfigTokenizer config_;
    std::size_t tamanho_minimo_;
    std::size_t tamanho_maximo_;
};

} // namespace natacha
