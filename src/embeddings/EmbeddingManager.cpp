#include "EmbeddingManager.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <system_error>
#include <utility>

using json = nlohmann::json;

namespace {

// Identificador do cache binário. Se mudar o formato, mude a versão
// (o cache antigo será ignorado e regenerado a partir do JSON).
constexpr char MAGIC[5] = {'N', 'E', 'M', 'B', '1'};

/**
 * Retorna true se o cache existe e foi modificado depois do JSON.
 * Um cache mais velho que o JSON está obsoleto e deve ser regenerado.
 */
bool cache_valido(const std::string& cache, const std::string& origem) {
    namespace fs = std::filesystem;
    std::error_code ec;

    if (!fs::exists(cache, ec) || ec) return false;
    if (!fs::exists(origem, ec) || ec) return false;

    auto t_cache = fs::last_write_time(cache, ec);
    if (ec) return false;
    auto t_origem = fs::last_write_time(origem, ec);
    if (ec) return false;

    return t_cache >= t_origem;
}

}  // namespace

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
    // Atalho: se existe um cache binário atualizado, usa ele.
    // Ler ~96 MB de JSON com o nlohmann leva ~12 s; o binário leva <100 ms.
    const std::string caminho_cache = caminho + ".bin";
    if (cache_valido(caminho_cache, caminho)) {
        if (carregarCacheBinario(caminho_cache)) {
            std::cout << "  (cache binario) " << total << " palavras, "
                      << dimensao << " dimensoes" << std::endl;
            return true;
        }
        std::cerr << "AVISO: cache binario invalido; recarregando do JSON." << std::endl;
    }

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

    // Grava o cache binário para a próxima inicialização ser instantânea.
    if (!salvarCacheBinario(caminho_cache)) {
        // Falha ao gravar cache não é fatal: o JSON continua funcionando.
        std::cerr << "AVISO: nao foi possivel gravar o cache binario." << std::endl;
    }

    return true;
}

// Formato do cache binário (little-endian, gerado e lido na mesma máquina):
//   magic[5] = "NEMB1"
//   uint32 dimensao
//   uint32 total
//   repetido total vezes:
//     uint32 tamanho_da_palavra
//     bytes  da_palavra (UTF-8)
//     float  vetor[dimensao]
bool EmbeddingManager::salvarCacheBinario(const std::string& caminho) const {
    std::ofstream saida(caminho, std::ios::binary);
    if (!saida.is_open()) return false;

    uint32_t dim32 = static_cast<uint32_t>(dimensao);
    uint32_t total32 = static_cast<uint32_t>(total);

    saida.write(MAGIC, sizeof(MAGIC));
    saida.write(reinterpret_cast<const char*>(&dim32), sizeof(dim32));
    saida.write(reinterpret_cast<const char*>(&total32), sizeof(total32));

    for (const std::string& palavra : id_para_palavra) {
        auto it = vetores.find(palavra);
        if (it == vetores.end()) continue;

        uint32_t tam = static_cast<uint32_t>(palavra.size());
        saida.write(reinterpret_cast<const char*>(&tam), sizeof(tam));
        saida.write(palavra.data(), tam);

        const std::vector<float>& v = it->second;
        saida.write(reinterpret_cast<const char*>(v.data()),
                    static_cast<std::streamsize>(v.size() * sizeof(float)));
    }

    return saida.good();
}

bool EmbeddingManager::carregarCacheBinario(const std::string& caminho) {
    std::ifstream entrada(caminho, std::ios::binary);
    if (!entrada.is_open()) return false;

    char magic[sizeof(MAGIC)];
    entrada.read(magic, sizeof(magic));
    if (!entrada.good() || std::memcmp(magic, MAGIC, sizeof(MAGIC)) != 0) {
        return false;
    }

    uint32_t dim32 = 0, total32 = 0;
    entrada.read(reinterpret_cast<char*>(&dim32), sizeof(dim32));
    entrada.read(reinterpret_cast<char*>(&total32), sizeof(total32));
    if (!entrada.good()) return false;

    vetores.clear();
    palavra_para_id.clear();
    id_para_palavra.clear();
    id_para_palavra.reserve(total32);

    for (uint32_t i = 0; i < total32; ++i) {
        uint32_t tam = 0;
        entrada.read(reinterpret_cast<char*>(&tam), sizeof(tam));
        if (!entrada.good() || tam > 4096) return false;

        std::string palavra(tam, '\0');
        entrada.read(palavra.data(), tam);
        if (!entrada.good()) return false;

        std::vector<float> v(dim32);
        entrada.read(reinterpret_cast<char*>(v.data()),
                     static_cast<std::streamsize>(dim32 * sizeof(float)));
        if (!entrada.good()) return false;

        vetores[palavra] = std::move(v);
        palavra_para_id[palavra] = static_cast<int>(i);
        id_para_palavra.push_back(std::move(palavra));
    }

    dimensao = dim32;
    total = total32;
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
