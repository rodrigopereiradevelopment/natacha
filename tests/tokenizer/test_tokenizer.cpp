#include "natacha/tokenizer.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using natacha::ConfigTokenizer;
    using natacha::ModoTokenizer;
    using natacha::Tokenizer;

    // 1. Normalizacao: minusculas e acentos preservados.
    {
        Tokenizer tokenizer;
        assert(tokenizer.normalizar("NATACHA MUSICA") == "natacha musica");
        assert(tokenizer.normalizar("Música e não") == "música e não");
    }

    // 2. Remocao opcional de acentos.
    {
        ConfigTokenizer config;
        config.remover_acentos = true;

        Tokenizer tokenizer(config);
        assert(tokenizer.normalizar("Música, não e Schrödinger") ==
               "musica, nao e schrodinger");
    }

    // 3. Tokenizacao NATACHA: preservacao de informacoes.
    {
        Tokenizer tokenizer;

        const std::vector<std::string> esperado = {
            "natacha", "gosta", "de", "música", "o",
            "word2vec", "usa", "384", "dimensões",
            "_interno", "extraordinariamente"
        };

        assert(tokenizer.tokenizar(
            "Natacha gosta de música! O Word2Vec usa 384 dimensões. "
            "_interno extraordinariamente"
        ) == esperado);
    }

    // 4. Identificadores tecnicos e compostos.
    {
        Tokenizer tokenizer;

        const std::vector<std::string> esperado = {
            "c++", "e", "c#", "next.js", "word2vec",
            "bem-estar", "d'água", "python3", "teste123"
        };

        assert(tokenizer.tokenizar(
            "C++ e C# Next.js Word2Vec bem-estar d'água "
            "Python3 teste123"
        ) == esperado);
    }

    // 5. Compatibilidade com Gensim 4.4.0.
    {
        ConfigTokenizer config;
        config.modo = ModoTokenizer::GENSIM_COMPAT;

        Tokenizer tokenizer(config);

        const std::vector<std::string> esperado = {
            "palavra_teste", "teste", "abc_def", "palavra_",
            "interno_", "palavra", "palavra", "final",
            "next", "js", "word", "vec", "ação", "científica"
        };

        assert(tokenizer.tokenizar(
            "palavra_teste teste123 abc_def palavra_123_teste "
            "_interno interno_ 123palavra palavra123final "
            "C++ e C# Next.js Word2Vec ação científica"
        ) == esperado);
    }

    // 6. Texto vazio e somente pontuacao.
    {
        Tokenizer tokenizer;
        assert(tokenizer.tokenizar("").empty());
        assert(tokenizer.tokenizar("!!! ... ???").empty());
    }

    // 7. Limites minimo e maximo personalizados.
    {
        ConfigTokenizer config;
        config.tamanho_minimo = 3;
        config.tamanho_maximo = 5;

        Tokenizer tokenizer(config);

        const std::vector<std::string> esperado = {
            "abc", "abcde"
        };

        assert(tokenizer.tokenizar("eu abc abcde abcdef") == esperado);
    }

    // 8. Configuracao invalida.
    {
        ConfigTokenizer config;
        config.tamanho_minimo = 10;
        config.tamanho_maximo = 5;

        bool lancou_excecao = false;

        try {
            Tokenizer tokenizer(config);
        } catch (const std::invalid_argument&) {
            lancou_excecao = true;
        }

        assert(lancou_excecao);
    }

    // 9. Processamento em lote.
    {
        Tokenizer tokenizer;

        const std::vector<std::string> textos = {
            "Natacha gosta de rock",
            "Felix dorme",
            ""
        };

        const auto resultado = tokenizer.tokenizarLote(textos);

        assert(resultado.size() == 3);
        assert((resultado[0] ==
                std::vector<std::string>{"natacha", "gosta", "de", "rock"}));
        assert((resultado[1] ==
                std::vector<std::string>{"felix", "dorme"}));
        assert(resultado[2].empty());
    }

    std::cout << "Todos os testes do Tokenizer passaram!\n";

    return 0;
}
