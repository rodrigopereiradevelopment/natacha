// ============================================================================
//  WORD2VEC JSON EXPORTER — Exporta embeddings .bin para JSON
// ============================================================================
//  Lê o arquivo binário gerado pelo word2vec_neg e salva em JSON legível
//  para inspeção, debug e integração com outras ferramentas.
//
//  Compilar: g++ -O3 -std=c++17 -o export_json export_json.cpp
//  Uso:      ./export_json [--input <arquivo.bin>] [--output <arquivo.json>]
// ============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <cstdint>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct Embedding {
    string palavra;
    vector<float> vetor;
};

int main(int argc, char* argv[]) {
    string inputBin = "dados/embeddings/vetores_word2vec.bin";
    string outputJson = "dados/embeddings/vetores_word2vec.json";

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--input" && i+1 < argc) inputBin = argv[++i];
        else if (arg == "--output" && i+1 < argc) outputJson = argv[++i];
        else if (arg == "--ajuda" || arg == "-h") {
            cout << "Uso: ./export_json [--input <arquivo.bin>] [--output <arquivo.json>]" << endl;
            return 0;
        }
    }

    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║      WORD2VEC JSON EXPORTER                          ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝" << endl;

    ifstream arquivo(inputBin, ios::binary);
    if (!arquivo.is_open()) {
        cerr << "\033[31mErro: nao encontrei " << inputBin << "\033[0m" << endl;
        return 1;
    }

    int dimensao;
    arquivo.read((char*)&dimensao, sizeof(int));

    vector<Embedding> embeddings;

    while (arquivo.peek() != EOF) {
        uint32_t tamPalavra;
        arquivo.read((char*)&tamPalavra, sizeof(uint32_t));
        if (arquivo.fail()) break;

        string palavra(tamPalavra, '\0');
        arquivo.read(&palavra[0], tamPalavra);

        vector<float> vetor(dimensao);
        arquivo.read((char*)vetor.data(), dimensao * sizeof(float));

        embeddings.push_back({palavra, vetor});
    }

    arquivo.close();

    if (embeddings.empty()) {
        cerr << "\033[31mNenhum embedding carregado\033[0m" << endl;
        return 1;
    }

    cout << "  Carregados: " << embeddings.size() << " palavras" << endl;
    cout << "  Dimensao: " << dimensao << endl;

    // Constrói JSON
    json j;
    j["metadata"] = {
        {"vocab_size", (int)embeddings.size()},
        {"dimension", dimensao},
        {"format", "word2vec_skipgram_negative_sampling"},
        {"corpus", "natacha_corpus_pt_br"}
    };

    json arr = json::array();
    for (auto& emb : embeddings) {
        json item;
        item["word"] = emb.palavra;
        item["vector"] = emb.vetor;
        arr.push_back(item);
    }
    j["embeddings"] = arr;

    // Salva JSON formatado
    ofstream out(outputJson);
    out << j.dump(2);
    out.close();

    cout << "\033[32m✓\033[0m JSON salvo em: " << outputJson << endl;

    // Mostra preview
    cout << "\n  Preview (primeiras 3 palavras):" << endl;
    for (int i = 0; i < min(3, (int)embeddings.size()); i++) {
        cout << "    " << embeddings[i].palavra << ": [";
        for (int d = 0; d < min(5, dimensao); d++) {
            if (d > 0) cout << ", ";
            cout << fixed << setprecision(4) << embeddings[i].vetor[d];
        }
        cout << "... (" << dimensao << " dims)]" << endl;
    }

    cout << "\n\033[32m✓\033[0m Exportacao concluida.\n" << endl;
    return 0;
}