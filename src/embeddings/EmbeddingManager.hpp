#ifndef EMBEDDING_MANAGER_HPP
#define EMBEDDING_MANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cstddef> // Necessário para size_t

class EmbeddingManager {
private:
    std::unordered_map<std::string, std::vector<float>> vetores;
    std::unordered_map<std::string, int> palavra_para_id;
    std::vector<std::string> id_para_palavra;
    size_t dimensao = 0;
    size_t total = 0;

public:
    bool carregarEmbeddings(const std::string& caminho);
    std::vector<float> obterVetor(const std::string& palavra) const;
    bool contem(const std::string& palavra) const {
    return vetores.find(palavra) != vetores.end();
}
    int obterId(const std::string& palavra) const;
    std::string obterPalavra(int id) const;
    size_t getVocabSize() const { return total; }
};

#endif // EMBEDDING_MANAGER_HPP