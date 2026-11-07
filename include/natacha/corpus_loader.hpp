#pragma once

#include "natacha/tokenizer.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace natacha {

struct FonteCorpus {
    std::string caminho;
    std::size_t repeticoes = 1;
    std::string nome;
};

struct EstatisticasFonte {
    std::string nome;
    std::string caminho;
    std::size_t repeticoes = 0;
    std::size_t linhas_lidas = 0;
    std::size_t linhas_ignoradas = 0;
    std::size_t frases_aceitas = 0;
};

struct CorpusCarregado {
    std::vector<std::vector<std::string>> frases;
    std::vector<EstatisticasFonte> estatisticas;
};

class CorpusLoader {
public:
    CorpusLoader();

    explicit CorpusLoader(const ConfigTokenizer& config);

    CorpusCarregado carregar(
        const std::vector<FonteCorpus>& fontes
    ) const;

private:
    Tokenizer tokenizer_;
};

} // namespace natacha
