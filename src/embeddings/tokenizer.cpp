#include "natacha/tokenizer.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

#include <unicode/normalizer2.h>
#include <unicode/unistr.h>
#include <unicode/uchar.h>
#include <unicode/locid.h>

namespace natacha {

namespace {

bool ehCaracterePalavra(UChar32 c) {
    return u_isalnum(c) ||
           u_charType(c) == U_NON_SPACING_MARK ||
           c == '_';
}

bool ehCaractereGensim(UChar32 c) {
    const int8_t tipo = u_charType(c);

    return tipo == U_UPPERCASE_LETTER ||
           tipo == U_LOWERCASE_LETTER ||
           tipo == U_TITLECASE_LETTER ||
           tipo == U_MODIFIER_LETTER ||
           tipo == U_OTHER_LETTER ||
           tipo == U_LETTER_NUMBER ||
           tipo == U_OTHER_NUMBER ||
           c == '_';
}

bool ehConector(UChar32 c) {
    return c == '-' || c == '\'' ||
           c == 0x2019 || c == '.';
}

bool ehSufixoTecnico(UChar32 c) {
    return c == '+' || c == '#';
}

} // namespace

Tokenizer::Tokenizer(const ConfigTokenizer& config)
    : config_(config),
      tamanho_minimo_(
          config.tamanho_minimo.value_or(
              config.modo == ModoTokenizer::GENSIM_COMPAT ? 2 : 1)),
      tamanho_maximo_(
          config.tamanho_maximo.value_or(
              config.modo == ModoTokenizer::GENSIM_COMPAT
                  ? 15
                  : std::numeric_limits<std::size_t>::max())) {

    if (tamanho_minimo_ > tamanho_maximo_) {
        throw std::invalid_argument(
            "Tamanho minimo nao pode ser maior que o maximo.");
    }
}

std::string Tokenizer::normalizar(const std::string& texto) const {
    icu::UnicodeString unicode =
        icu::UnicodeString::fromUTF8(texto);

    if (config_.remover_acentos) {
        UErrorCode erro = U_ZERO_ERROR;
        const icu::Normalizer2* normalizador =
            icu::Normalizer2::getNFDInstance(erro);

        if (U_FAILURE(erro)) {
            throw std::runtime_error(
                "Falha ao inicializar normalizador Unicode.");
        }

        icu::UnicodeString normalizado;
        normalizador->normalize(unicode, normalizado, erro);

        if (U_FAILURE(erro)) {
            throw std::runtime_error(
                "Falha ao normalizar texto Unicode.");
        }

        unicode = normalizado;
        icu::UnicodeString sem_acentos;

        for (int32_t i = 0; i < unicode.length();) {
            UChar32 c = unicode.char32At(i);
            i += U16_LENGTH(c);

            if (u_charType(c) != U_NON_SPACING_MARK) {
                sem_acentos.append(c);
            }
        }

        unicode = sem_acentos;
    }

    if (config_.converter_minusculas) {
        unicode.toLower(icu::Locale::getRoot());
    }

    std::string resultado;
    unicode.toUTF8String(resultado);
    return resultado;
}

std::vector<std::string> Tokenizer::tokenizar(
    const std::string& texto) const {

    const icu::UnicodeString unicode =
        icu::UnicodeString::fromUTF8(normalizar(texto));

    const bool modoGensim =
        config_.modo == ModoTokenizer::GENSIM_COMPAT;

    std::vector<std::string> tokens;
    icu::UnicodeString token;

    std::size_t tamanho = 0;
    bool possui_letra = false;

    auto finalizarToken = [&]() {
        if (token.isEmpty()) {
            return;
        }

        if (tamanho >= tamanho_minimo_ &&
            tamanho <= tamanho_maximo_) {

            if (!modoGensim || token.charAt(0) != '_') {
                std::string utf8;
                token.toUTF8String(utf8);
                tokens.push_back(std::move(utf8));
            }
        }

        token.remove();
        tamanho = 0;
        possui_letra = false;
    };

    for (int32_t i = 0; i < unicode.length();) {
        UChar32 c = unicode.char32At(i);
        i += U16_LENGTH(c);

        const bool caractereValido = modoGensim
            ? ehCaractereGensim(c)
            : ehCaracterePalavra(c);

        if (caractereValido) {
            token.append(c);
            ++tamanho;

            if (u_isalpha(c)) {
                possui_letra = true;
            }

            continue;
        }

        if (!modoGensim &&
            ehConector(c) &&
            !token.isEmpty() &&
            possui_letra) {

            const bool existeProximo = i < unicode.length();
            const UChar32 proximo = existeProximo
                ? unicode.char32At(i)
                : -1;

            if (existeProximo && ehCaracterePalavra(proximo)) {
                token.append(c);
                ++tamanho;
                continue;
            }
        }

        if (!modoGensim &&
            ehSufixoTecnico(c) &&
            !token.isEmpty() &&
            possui_letra) {
            token.append(c);
            ++tamanho;
            continue;
        }

        finalizarToken();
    }

    finalizarToken();
    return tokens;
}

std::vector<std::vector<std::string>> Tokenizer::tokenizarLote(
    const std::vector<std::string>& textos) const {

    std::vector<std::vector<std::string>> resultado;
    resultado.reserve(textos.size());

    for (const auto& texto : textos) {
        resultado.push_back(tokenizar(texto));
    }

    return resultado;
}

} // namespace natacha
