#include "EmbeddingManager.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstddef>

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

    if (j.contains("dimensao")) {
        dimensao = j["dimensao"].get<std::size_t>();
    }

    if (!j.contains("embeddings") || !j["embeddings"].is_array()) {
        std::cerr << "ERRO: JSON sem campo 'embeddings' ou nao e array" << std::endl;
        return false;
    }

    vetores.clear();
    palavra_para_id.clear();
    id_para_palavra.clear();
    total = 0;
    int id_atual = 0;

    for (const auto& item : j["embeddings"]) {
        if (!item.contains("palavra") || !item.contains("vetor")) continue;
        std::string palavra = item["palavra"].get<std::string>();
        std::vector<float> vetor = item["vetor"].get<std::vector<float>>();
        
        vetores[palavra] = vetor;
        palavra_para_id[palavra] = id_atual;
        id_para_palavra.push_back(palavra);
        id_atual++;
        total++;
    }

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



int EmbeddingManager::obterId(const std::string& palavra) const {
    auto it = palavra_para_id.find(palavra);
    if (it != palavra_para_id.end()) {
        return it->second;
    }
    return -1;
}

std::string EmbeddingManager::obterPalavra(int id) const {
    if (id >= 0 && id < (int)id_para_palavra.size()) {
        return id_para_palavra[id];
    }
    return "<OOV>";
}