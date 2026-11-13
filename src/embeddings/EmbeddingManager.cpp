#include "EmbeddingManager.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstddef>

using json = nlohmann::json;

// Carrega embeddings treinados de um arquivo JSON.
// O formato esperado é:
//   {
//     "dimensao": 64,
//     "embeddings": [
//       {"palavra": "natacha", "vetor": [0.1, -0.2, ...]},
//       ...
//     ]
//   }
//
// Isso permite que a Natacha carregue vetores de palavras treinados
// (Word2Vec) sem precisar retreinar do zero toda vez que inicia.
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

    // Lê a dimensão dos vetores (ex: 64 para Word2Vec v38/v39)
    if (j.contains("dimensao")) {
        dimensao = j["dimensao"].get<std::size_t>();
    }

    // Valida que o JSON tem o campo "embeddings" e que é um array
    if (!j.contains("embeddings") || !j["embeddings"].is_array()) {
        std::cerr << "ERRO: JSON sem campo 'embeddings' ou nao e array" << std::endl;
        return false;
    }

    // Limpa qualquer estado anterior antes de carregar
    vetores.clear();
    palavra_para_id.clear();
    id_para_palavra.clear();
    total = 0;
    int id_atual = 0;

    // Itera sobre cada entrada do array "embeddings"
    // Cada entrada tem "palavra" (string) e "vetor" (array de floats)
    for (const auto& item : j["embeddings"]) {
        if (!item.contains("palavra") || !item.contains("vetor")) continue;
        std::string palavra = item["palavra"].get<std::string>();
        std::vector<float> vetor = item["vetor"].get<std::vector<float>>();

        // Armazena o vetor indexado pela palavra (mapa palavra -> vetor)
        vetores[palavra] = vetor;
        // Mapeia palavra para um ID inteiro (índice sequencial)
        palavra_para_id[palavra] = id_atual;
        // Mapeia ID inteiro de volta para a palavra (para reconstrução)
        id_para_palavra.push_back(palavra);
        id_atual++;
        total++;
    }

    // Se a dimensão não estava no JSON, infere a partir do primeiro vetor
    if (dimensao == 0 && !vetores.empty()) {
        dimensao = vetores.begin()->second.size();
    }

    std::cout << "  Embeddings carregados: " << total
              << " palavras, " << dimensao << " dimensoes" << std::endl;

    return true;
}

// Retorna o vetor de uma palavra, ou vetor vazio se a palavra não existe no vocabulário.
// Complexidade: O(1) médio graças ao unordered_map.
std::vector<float> EmbeddingManager::obterVetor(const std::string& palavra) const {
    auto it = vetores.find(palavra);
    if (it == vetores.end()) {
        return {};
    }
    return it->second;
}

// Retorna o ID inteiro associado a uma palavra, ou -1 se não encontrada.
// Útil para converter palavras em índices numéricos para camadas neurais.
int EmbeddingManager::obterId(const std::string& palavra) const {
    auto it = palavra_para_id.find(palavra);
    if (it != palavra_para_id.end()) {
        return it->second;
    }
    return -1;
}

// Retorna a palavra associada a um ID inteiro, ou "<OOV>" se o ID for inválido.
// OOV = Out Of Vocabulary (palavra fora do vocabulário conhecido).
std::string EmbeddingManager::obterPalavra(int id) const {
    if (id >= 0 && id < (int)id_para_palavra.size()) {
        return id_para_palavra[id];
    }
    return "<OOV>";
}
