#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace natacha {

/**
 * Modos de tokenização disponíveis:
 * - GENSIM_COMPAT: compatível com o formato do Word2Vec/Gensim (tokens começando
 *   com '_' são especiais, como _BEGIN_, _END_).
 * - NATACHA: modo padrão da Natacha, mais flexível com conectores e sufixos.
 */
enum class ModoTokenizer {
    GENSIM_COMPAT,
    NATACHA
};

/**
 * Configuração do tokenizer.
 * 
 * @param modo              Modo de tokenização (padrão: NATACHA).
 * @param converter_minusculas   Converte tudo para minúsculas (padrão: true).
 * @param remover_acentos        Remove acentos via normalização NFD + remoção
 *                               de marcas de não-spacing (padrão: false).
 * @param tamanho_minimo         Tamanho mínimo do token (nullopt = padrão do modo).
 * @param tamanho_maximo         Tamanho máximo do token (nullopt = padrão do modo).
 * 
 * No modo GENSIM_COMPAT: min=2, max=15, remove acentos=false.
 * No modo NATACHA: min=1, max=ilimitado, remove acentos=false.
 */
struct ConfigTokenizer {
    ModoTokenizer modo = ModoTokenizer::NATACHA;

    bool converter_minusculas = true;
    bool remover_acentos = false;

    // nullopt = utilizar o limite padrão do modo.
    std::optional<std::size_t> tamanho_minimo;
    std::optional<std::size_t> tamanho_maximo;
};

/**
 * Tokenizer Unicode-aware para língua portuguesa e código técnico.
 * 
 * Recursos principais:
 * - Suporte completo a Unicode via ICU (normalização NFD, remoção de acentos).
 * - Dois modos: GENSIM_COMPAT (compatível com Word2Vec) e NATACHA (flexível).
 * - Suporte a conectores internos (-, ', .) e sufixos técnicos (+, #) no modo NATACHA.
 * - Filtro por tamanho mínimo/máximo de token configurável.
 * - Suporte a batch processing (tokenizarLote).
 * 
 * Exemplo de uso:
 *   ConfigTokenizer cfg;
 *   cfg.modo = ModoTokenizer::NATACHA;
 *   cfg.converter_minusculas = true;
 *   Tokenizer tokenizer(cfg);
 *   auto tokens = tokenizer.tokenizar("Olá, mundo! C++ é legal.");
 */
class Tokenizer {
public:
    /**
     * Constrói o tokenizer com a configuração dada.
     * Calcula os tamanhos mínimo/máximo baseados no modo e configuração.
     * 
     * @param config Configuração do tokenizer (padrão: modo NATACHA, minúsculas on).
     */
    explicit Tokenizer(const ConfigTokenizer& config = {});

    /**
     * Normaliza um texto aplicando as configurações:
     * - Normalização Unicode NFD + remoção de marcas de não-spacing (se remover_acentos).
     * - Conversão para minúsculas (se converter_minusculas).
     * 
     * @param texto Texto UTF-8 a normalizar.
     * @return Texto normalizado em UTF-8.
     */
    std::string normalizar(const std::string& texto) const;

    /**
     * Tokeniza um texto em uma lista de palavras/tokens.
     * 
     * Fluxo:
     * 1. Normaliza o texto.
     * 2. Itera codepoints Unicode, agrupando caracteres válidos em tokens.
     * 2. No modo NATACHA: mantém conectores (-, ', .) entre letras
     *    e sufixos técnicos (+, #) como parte do token.
     * 3. Finaliza tokens ao encontrar separadores.
     * 4. Filtra por tamanho [tamanho_minimo, tamanho_maximo].
     * 
     * @param texto Texto UTF-8 a tokenizar.
     * @return Vetor de tokens (strings UTF-8).
     */
    std::vector<std::string> tokenizar(
        const std::string& texto
    ) const;

    /**
     * Tokeniza um lote de textos (batch processing).
     * Mais eficiente que chamar tokenizar() em loop para corpus grandes.
     * 
     * @param textos Vetor de textos UTF-8.
     * @return Vetor de vetores de tokens (um vetor por texto de entrada).
     */
    std::vector<std::vector<std::string>> tokenizarLote(
        const std::vector<std::string>& textos
    ) const;

private:
    ConfigTokenizer config_;
    std::size_t tamanho_minimo_;
    std::size_t tamanho_maximo_;
};

} // namespace natacha
