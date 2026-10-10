#ifndef EMBEDDING_MANAGER_HPP
#define EMBEDDING_MANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>

class EmbeddingManager {
private:
    std::unordered_map<std::string, std::vector<float>> vetores;
    size_t dimensao = 0;
    size_t total = 0;

public:
    bool carregarEmbeddings(const std::string& caminho);
    std::vector<float> obterVetor(const std::string& palavra) const;
    bool contem(const std::string& palavra) const;
    size_t getDimensao() const { return dimensao; }
    size_t getTotal() const { return total; }
};

#endif // EMBEDDING_MANAGER_HPP
