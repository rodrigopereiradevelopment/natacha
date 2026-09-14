// inspect_embeddings.cpp — Inspecionar embeddings treinados
// Compilar: g++ -O3 -std=c++17 -o inspect_embeddings inspect_embeddings.cpp -lm
//
// Uso:
//   ./inspect_embeddings felix
//   ./inspect_embeddings natacha --top 20
//   ./inspect_embeddings --listar

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <cstdint>

using namespace std;

struct Embedding {
    string palavra;
    vector<float> vetor;
};

float dot(const vector<float>& a, const vector<float>& b) {
    float s = 0;
    for (size_t i = 0; i < a.size(); i++) s += a[i] * b[i];
    return s;
}

float norma(const vector<float>& v) {
    float s = 0;
    for (float x : v) s += x * x;
    return sqrt(s);
}

float cosseno(const vector<float>& a, const vector<float>& b) {
    float na = norma(a);
    float nb = norma(b);
    if (na < 1e-8f || nb < 1e-8f) return 0;
    return dot(a, b) / (na * nb);
}

string normalizar(const string& s) {
    string r;
    for (unsigned char c : s) {
        if (c < 0x80) r += tolower(c);
        // Simplificado - aceita ASCII
    }
    return r;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso:" << endl;
        cout << "  ./inspect_embeddings <palavra> [--top N]" << endl;
        cout << "  ./inspect_embeddings --listar" << endl;
        cout << "  ./inspect_embeddings --todas --top 5" << endl;
        return 0;
    }

    string arquivo = "dados/embeddings/vetores_hibrido.bin";
    string palavraBusca = "";
    int topN = 10;
    bool listarTodas = false;
    bool listarVocab = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--top" && i+1 < argc) topN = stoi(argv[++i]);
        else if (arg == "--todas") listarTodas = true;
        else if (arg == "--listar") listarVocab = true;
        else if (arg == "--arquivo" && i+1 < argc) arquivo = argv[++i];
        else palavraBusca = arg;
    }

    // Ler binário
    ifstream in(arquivo, ios::binary);
    if (!in.is_open()) {
        cerr << "Erro: nao encontrei " << arquivo << endl;
        return 1;
    }

    int dimensao;
    in.read((char*)&dimensao, sizeof(int));

    vector<Embedding> embeddings;
    while (in.peek() != EOF) {
        uint32_t tam;
        in.read((char*)&tam, sizeof(uint32_t));
        if (in.fail()) break;

        string palavra(tam, ' ');
        in.read(&palavra[0], tam);

        vector<float> vetor(dimensao);
        in.read((char*)vetor.data(), dimensao * sizeof(float));

        embeddings.push_back({palavra, vetor});
    }
    in.close();

    cout << "═══════════════════════════════════════════════════" << endl;
    cout << "  INSPECT EMBEDDINGS" << endl;
    cout << "═══════════════════════════════════════════════════" << endl;
    cout << "  Arquivo:    " << arquivo << endl;
    cout << "  Palavras:   " << embeddings.size() << endl;
    cout << "  Dimensao:   " << dimensao << endl;
    cout << "═══════════════════════════════════════════════════" << endl;

    // Listar vocabulário
    if (listarVocab) {
        cout << "\nVocabulario (" << embeddings.size() << " palavras):" << endl;
        for (size_t i = 0; i < embeddings.size(); i++) {
            cout << "  " << setw(4) << i << " " << embeddings[i].palavra << endl;
            if ((i+1) % 20 == 0 && i+1 < embeddings.size()) {
                cout << "  ... [enter para continuar]" << flush;
                // cin.get();  // Opcional: pausar
            }
        }
        return 0;
    }

    if (palavraBusca.empty() && !listarTodas) {
        cout << "\nForneça uma palavra para buscar." << endl;
        return 0;
    }

    // Listar todas as similaridades
    if (listarTodas) {
        cout << "\nTop " << topN << " mais similares para TODAS as palavras:" << endl;
        cout << "(Limitado a palavras com >5 ocorrencias)" << endl;

        for (auto& emb : embeddings) {
            vector<pair<string, float>> sims;
            for (auto& other : embeddings) {
                if (other.palavra == emb.palavra) continue;
                sims.push_back({other.palavra, cosseno(emb.vetor, other.vetor)});
            }
            sort(sims.begin(), sims.end(), [](auto& a, auto& b){ return a.second > b.second; });

            cout << "\n" << emb.palavra << ":" << endl;
            for (int i = 0; i < min(topN, (int)sims.size()); i++) {
                cout << "  " << setw(15) << left << sims[i].first
                     << " " << fixed << setprecision(4) << sims[i].second << endl;
            }
        }
        return 0;
    }

    // Buscar palavra específica
    // Aceitar múltiplas palavras
    vector<string> palavras;
    palavras.push_back(palavraBusca);
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg != "--top" && arg != "--todas" && arg != "--listar" && arg != "--arquivo") {
            if (i+1 < argc && argv[i+1][0] != '-') {
                // Pular valor do --top
            } else if (arg != palavraBusca) {
                palavras.push_back(arg);
            }
        }
    }

    for (auto& busca : palavras) {
        // Encontrar palavra
        int idxEncontrado = -1;
        for (size_t i = 0; i < embeddings.size(); i++) {
            if (embeddings[i].palavra == busca || normalizar(embeddings[i].palavra) == normalizar(busca)) {
                idxEncontrado = i;
                break;
            }
        }

        if (idxEncontrado == -1) {
            cout << "\n  \033[33m?\033[0m '" << busca << "' nao encontrada" << endl;
            continue;
        }

        auto& emb = embeddings[idxEncontrado];
        float n = norma(emb.vetor);

        cout << "\n┌─────────────────────────────────────────────────┐" << endl;
        cout << "│ Palavra: " << setw(40) << left << emb.palavra << "│" << endl;
        cout << "│ Dimensao: " << setw(39) << dimensao << "│" << endl;
        cout << "│ Norma:    " << setw(39) << fixed << setprecision(4) << n << "│" << endl;
        cout << "└─────────────────────────────────────────────────┘" << endl;

        // Calcular similaridades
        vector<pair<string, float>> sims;
        for (auto& other : embeddings) {
            if (other.palavra == emb.palavra) continue;
            sims.push_back({other.palavra, cosseno(emb.vetor, other.vetor)});
        }
        sort(sims.begin(), sims.end(), [](auto& a, auto& b){ return a.second > b.second; });

        cout << "\nMais similares (" << topN << "):" << endl;
        for (int i = 0; i < min(topN, (int)sims.size()); i++) {
            int barras = 20;
            int preenchido = (int)(max(0.0f, sims[i].second) * barras);
            cout << "  " << setw(15) << left << sims[i].first
                 << " " << fixed << setprecision(4) << sims[i].second
                 << " \033[90m";
            for (int b = 0; b < barras; b++)
                cout << (b < preenchido ? "\033[32m▪\033[0m\033[90m" : "·");
            cout << "\033[0m" << endl;
        }

        // Mostrar primeiras dimensões (debug)
        cout << "\nVetor (primeiras 10 dims):" << endl;
        cout << "  [";
        for (int d = 0; d < min(10, dimensao); d++) {
            if (d > 0) cout << ", ";
            cout << fixed << setprecision(3) << emb.vetor[d];
        }
        cout << ", ...]" << endl;
    }

    return 0;
}
