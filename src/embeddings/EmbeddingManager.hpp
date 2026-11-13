#ifndef EMBEDDING_MANAGER_HPP
#define EMBEDDING_MANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cstddef> // Necessário para size_t

/**
 * Gerenciador de embeddings (vetores densos de palavras) para a Natacha.
 * 
 * Carrega, armazena e fornece acesso rápido a vetores de palavras
 * treinados (Word2Vec, GloVe, etc.) em formato JSON.
 * 
 * Estrutura de dados interna:
 * - vetores: mapa palavra -> vetor denso (float[]) para busca O(1).
 * - palavra_para_id: palavra -> índice inteiro (para camadas neurais).
 * - id_para_palavra: índice -> palavra (para geração de texto).
 * - dimensao: dimensionalidade dos vetores (ex: 64).
 * - total: quantidade de palavras carregadas.
 * 
 * Formato JSON esperado:
 * {
 *   "dimensao": 64,
 *   "embeddings": [
 *     {"palavra": "natacha", "vetor": [0.1, -0.2, ...]},
 *     ...
 *   ]
 * }
 */
class EmbeddingManager {
private:
    std::unordered_map<std::string, std::vector<float>> vetores;
    std::unordered_map<std::string, int> palavra_para_id;
    std::vector<std::string> id_para_palavra;
    size_t dimensao = 0;
    size_t total = 0;

public:
    /**
     * Carrega embeddings de um arquivo JSON.
     * 
     * @param caminho Caminho para o arquivo .json com os embeddings.
     * @return true se carregou com sucesso, false caso contrário.
     * 
     * O JSON deve ter a estrutura:
     * {
     *   "dimensao": 64,
     *   "embeddings": [
     *     {"palavra": "natacha", "vetor": [0.1, -0.2, ...]},
     *     ...
     *   ]
     * }
     */
    bool carregarEmbeddings(const std::string& caminho);

    /**
     * Obtém o vetor denso de uma palavra.
     * 
     * @param palavra Palavra a buscar (deve estar exatamente como no vocabulário).
     * @return Vetor de floats se encontrado, vetor vazio caso contrário.
     * Complexidade: O(1) médio (unordered_map).
     */
    std::vector<float> obterVetor(const std::string& palavra) const;

    /**
     * Verifica se uma palavra existe no vocabulário carregado.
     * 
     * @param palavra Palavra a verificar.
     * @return true se existe, false caso contrário.
     */
    bool contem(const std::string& palavra) const {
        return vetores.find(palavra) != vetores.end();
    }

    /**
     * Obtém o ID inteiro (índice) de uma palavra no vocabulário.
     * Útil para converter palavras em índices para camadas neurais.
     * 
     * @param palavra Palavra a buscar.
     * @return ID (>=0) se encontrado, -1 caso contrário.
     */
    int obterId(const std::string& palavra) const;

    /**
     * Obtém a palavra correspondente a um ID.
     * Útil para reconstruir texto a partir de IDs gerados pelo modelo.
     * 
     * @param id ID da palavra (0 <= id < vocab_size).
     * @return Palavra correspondente, ou "<OOV>" se ID inválido.
     */
    std::string obterPalavra(int id) const;

    /**
     * Retorna o tamanho do vocabulário carregado.
     */
    size_t getVocabSize() const { return total; }
};

#endif // EMBEDDING_MANAGER_HPP