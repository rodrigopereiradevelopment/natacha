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

// Verifica se um caractere Unicode faz parte de uma palavra (alfanumérico,
// marca de não-spacing ou underline). Usado no modo padrão do tokenizer.
bool ehCaracterePalavra(UChar32 c) {
    return u_isalnum(c) ||
           u_charType(c) == U_NON_SPACING_MARK ||
           c == '_';
}

// Verifica se um caractere é considerado letra pelo padrão Gensim.
// Inclui maiúsculas, minúsculas, titlecase, modificadoras, outras letras,
// números de letra e números outros, além do underline.
// Usado quando o tokenizer está em modo compatível com Gensim.
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

// Verifica se um caractere é um conector que pode aparecer dentro de uma
// palavra (hífen, apóstrofo, aspas, ponto). No modo não-Gensim, conectores
// são mantidos dentro de palavras se houver letras em ambos os lados.
bool ehConector(UChar32 c) {
    return c == '-' || c == '\'' ||
           c == 0x2019 || c == '.';
}

// Verifica se um caractere é sufixo técnico (+ ou #), usado em nomes como
// "C++" ou "C#". Mantido dentro da palavra no modo não-Gensim.
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
    // Converte string UTF-8 para icu::UnicodeString para processamento Unicode correto
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

        // Remove marcas de não-spacing (acentos) mantendo a letra base
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

// Tokeniza um texto em uma lista de tokens (palavras).
// Fluxo: normaliza -> itera caracteres Unicode -> agrupa em tokens -> filtra por tamanho.
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

    // Lambda para finalizar o token atual e adicioná-lo à lista se válido
    auto finalizarToken = [&]() {
        if (token.isEmpty()) {
            return;
        }

        // Filtra tokens fora do intervalo [tamanho_minimo, tamanho_maximo]
        if (tamanho >= tamanho_minimo_ &&
            tamanho <= tamanho_maximo_) {

            // No modo Gensim, tokens começando com '_' são especiais (ex: _BEGIN_)
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

    // Loop principal: itera sobre cada caractere Unicode (codepoint)
    for (int32_t i = 0; i < unicode.length();) {
        UChar32 c = unicode.char32At(i);
        i += U16_LENGTH(c);

        // Define o que é um caractere válido baseado no modo
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

        // Modo não-Gensim: trata conectores (-, ', ., etc.) dentro de palavras
        if (!modoGensim &&
            ehConector(c) &&
            !token.isEmpty() &&
            possui_letra) {

            const bool existeProximo = i < unicode.length();
            const UChar32 proximo = existeProximo
                ? unicode.char32At(i)
                : -1;

            // Mantém o conector se o próximo caractere for válido (ex: "bem-estar")
            if (existeProximo && ehCaracterePalavra(proximo)) {
                token.append(c);
                ++tamanho;
                continue;
            }
        }

        // Modo não-Gensim: sufixos técnicos (+, #) como em "C++", "C#"
        if (!modoGensim &&
            ehSufixoTecnico(c) &&
            !token.isEmpty() &&
            possui_letra) {
        token.append(c);
        ++tamanho;
        continue;
    }

    // Caracter não reconhecido: finaliza o token atual
    finalizarToken();
}

finalizarToken();
return tokens;
}

// Tokeniza um lote de textos (batch processing). Útil para processar corpus inteiros.
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
