#include "EmbeddingManager.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool EmbeddingManager::carregarEmbeddings(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        std::cerr << "ERRO: nao consegui abrir '" << caminho << "'" << std::endl;
        return false;
    }

    json j;
    try {
        arquivo >> j;
    } catch (const std::exception& e) {
        std::cerr << "ERRO ao parsear JSON: " << e.what() << std::endl;
        return false;
    }
    arquivo.close();

    // Dimensao
    if (j.contains("dimensao")) {
        dimensao = j["dimensao"].get<size_t>();
    }

    // Embeddings
    if (!j.contains("embeddings") || !j["embeddings"].is_array()) {
        std::cerr << "ERRO: JSON sem campo 'embeddings' ou nao e array" << std::endl;
        return false;
    }

    vetores.clear();
    total = 0;

    for (const auto& item : j["embeddings"]) {
        if (!item.contains("palavra") || !item.contains("vetor")) continue;
        std::string palavra = item["palavra"].get<std::string>();
        std::vector<float> vetor = item["vetor"].get<std::vector<float>>();
        vetores[palavra] = vetor;
        total++;
    }

    // Se dimensao nao estava no JSON, detecta do primeiro vetor
    if (dimensao == 0 && !vetores.empty()) {
        dimensao = vetores.begin()->second.size();
    }

    std::cout << "  Embeddings carregados: " << total
              << " palavras, " << dimensao << " dimensoes" << std::endl;

    return true;
}

std::vector<float> EmbeddingManager::obterVetor(const std::string& palavra) const {
    auto it = vetores.find(palavra);
    if (it == vetores.end()) {
        return {};
    }
    return it->second;
}

bool EmbeddingManager::contem(const std::string& palavra) const {
    return vetores.find(palavra) != vetores.end();
}
