// ============================================================================
//  NATACHA EMBEDDER — Ferramenta interativa de embeddings
// ============================================================================
//  Carrega os embeddings treinados e permite conversar com a Natacha
//  usando similaridade de vetores.
//
//  Compilar: g++ -O3 -std=c++17 -o natacha_embedder natacha_embedder.cpp
//  Uso:      ./natacha_embedder [--arquivo <caminho>]
// ============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cstdint>

using namespace std;

// ============================================================================
//  ESTRUTURAS DE DADOS
// ============================================================================

// Armazena um vetor embedding de uma palavra
struct Embedding {
    string palavra;
    vector<float> vetor;
};

// ============================================================================
//  CARREGAR EMBEDDINGS DO ARQUIVO BINÁRIO
// ============================================================================
//  Formato do arquivo .bin:
//  - 4 bytes: dimensão (int)
//  - Para cada palavra:
//    - 4 bytes: tamanho da palavra (uint32_t)
//    - N bytes: nome da palavra (string)
//    - dim*4 bytes: vetor de floats
//
//  Retorna: mapa {palavra → índice no vetor embeddings}
// ============================================================================

unordered_map<string, int> carregarEmbeddings(
    const string& caminho,
    vector<Embedding>& embeddings,
    int& dimensao
) {
    unordered_map<string, int> indice;

    ifstream arquivo(caminho, ios::binary);
    if (!arquivo.is_open()) {
        cerr << "Erro: nao encontrei " << caminho << endl;
        return indice;
    }

    // Lê a dimensão dos vetores (20, 64, etc)
    arquivo.read((char*)&dimensao, sizeof(int));

    // Lê cada palavra e seu vetor
    while (arquivo.peek() != EOF) {
        // Lê tamanho do nome da palavra
        uint32_t tamPalavra;
        arquivo.read((char*)&tamPalavra, sizeof(uint32_t));
        if (arquivo.fail()) break;

        // Lê o nome da palavra
        string palavra(tamPalavra, '\0');
        arquivo.read(&palavra[0], tamPalavra);

        // Lê o vetor de floats (dimensão valores)
        vector<float> vetor(dimensao);
        arquivo.read((char*)vetor.data(), dimensao * sizeof(float));

        // Armazena e cria índice
        indice[palavra] = embeddings.size();
        embeddings.push_back({palavra, vetor});
    }

    arquivo.close();
    return indice;
}

// ============================================================================
//  CÁLCULO DE SIMILARIDADE (COSENO)
// ============================================================================
//  Mede o quão "próximos" dois vetores estão no espaço.
//  Retorna valor entre -1 e 1:
//   - 1.0 = idênticos
//   - 0.0 = ortogonais (sem relação)
//  - -1.0 = opostos
//
//  Fórmula: cos(a,b) = (a·b) / (||a|| * ||b||)
// ============================================================================

float cosseno(const vector<float>& a, const vector<float>& b) {
    float produto = 0, normaA = 0, normaB = 0;
    for (size_t i = 0; i < a.size(); i++) {
        produto += a[i] * b[i];
        normaA  += a[i] * a[i];
        normaB  += b[i] * b[i];
    }
    if (normaA == 0 || normaB == 0) return 0;
    return produto / (sqrt(normaA) * sqrt(normaB));
}

// ============================================================================
//  ENCONTRAR PALAVRAS MAIS SIMILARES
// ============================================================================
//  Dada uma palavra-alvo, retorna as N palavras mais parecidas
//  usando similaridade cosseno.
// ============================================================================

vector<pair<float, string>> topSimilares(
    const string& alvo,
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings,
    int n = 5
) {
    vector<pair<float, string>> resultados;

    // Verifica se a palavra existe no vocabulário
    auto it = indice.find(alvo);
    if (it == indice.end()) return resultados;

    int idxAlvo = it->second;
    const vector<float>& vAlvo = embeddings[idxAlvo].vetor;

    // Calcula similaridade com todas as outras palavras
    for (size_t i = 0; i < embeddings.size(); i++) {
        if ((int)i == idxAlvo) continue; // pula a própria palavra
        float sim = cosseno(vAlvo, embeddings[i].vetor);
        resultados.push_back({sim, embeddings[i].palavra});
    }

    // Ordena do mais similar pro menos similar
    sort(resultados.rbegin(), resultados.rend());

    // Retorna apenas os N primeiros
    if ((int)resultados.size() > n)
        resultados.resize(n);
    return resultados;
}

// ============================================================================
//  PROCESSAR FRASE DO USUÁRIO
// ============================================================================
//  Recebe uma frase como "onde fica a cozinha"
//  Tokeniza e busca similaridade com cada palavra do vocabulário
// ============================================================================

void processarFrase(
    const string& frase,
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings
) {
    // Tokeniza a frase (simplificado: separa por espaços)
    stringstream ss(frase);
    string palavra;
    vector<string> tokens;

    while (ss >> palavra) {
        // Converte para minúsculas
        transform(palavra.begin(), palavra.end(), palavra.begin(), ::tolower);
        // Remove pontuação
        palavra.erase(remove_if(palavra.begin(), palavra.end(), ::ispunct), palavra.end());
        if (!palavra.empty()) tokens.push_back(palavra);
    }

    if (tokens.empty()) {
        cout << "  Nao entendi. Tente outra frase." << endl;
        return;
    }

    // Para cada token, mostra as 3 palavras mais similares
    for (const string& token : tokens) {
        if (indice.find(token) == indice.end()) continue;

        auto similares = topSimilares(token, indice, embeddings, 3);
        if (similares.empty()) continue;

        cout << "  " << token << " → ";
        for (size_t i = 0; i < similares.size(); i++) {
            if (i > 0) cout << ", ";
            // Mostra com cor baseado na similaridade
            if (similares[i].first > 0.5f)
                cout << "\033[32m" << similares[i].second << "\033[0m";
            else if (similares[i].first > 0.3f)
                cout << "\033[33m" << similares[i].second << "\033[0m";
            else
                cout << "\033[90m" << similares[i].second << "\033[0m";
        }
        cout << endl;
    }
}

// ============================================================================
//  MODO INTERATIVO
// ============================================================================
//  Loop principal: recebe input do usuário e responde
// ============================================================================

void modoInterativo(
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings
) {
    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║         NATACHA EMBEDDER — Modo Interativo          ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝" << endl;
    cout << "  Comandos:" << endl;
    cout << "    Digite uma frase  → busca similaridade" << endl;
    cout << "    /top <palavra>    → top 5 da palavra" << endl;
    cout << "    /vocab            → listar vocabulário" << endl;
    cout << "    /sair             → encerrar" << endl;
    cout << "\n  Vocabulário: " << indice.size() << " palavras" << endl;
    cout << "  Dimensão: " << embeddings[0].vetor.size() << " floats\n" << endl;

    string entrada;
    while (true) {
        cout << "\033[1;36mnatacha> \033[0m";
        getline(cin, entrada);

        if (entrada.empty()) continue;
        if (entrada == "/sair" || entrada == "/exit") {
            cout << "Tchau! 👋" << endl;
            break;
        }

        if (entrada.substr(0, 4) == "/top") {
            // Comando /top: mostra top 5 de uma palavra
            string palavra = entrada.substr(5);
            transform(palavra.begin(), palavra.end(), palavra.begin(), ::tolower);
            auto similares = topSimilares(palavra, indice, embeddings, 5);
            if (similares.empty()) {
                cout << "  \"" << palavra << "\" nao encontrado no vocabulario." << endl;
            } else {
                cout << "  Top 5 de \"" << palavra << "\":" << endl;
                for (size_t i = 0; i < similares.size(); i++) {
                    int barras = (int)(max(0.0f, similares[i].first) * 20);
                    cout << "    " << (i+1) << ". " << setw(12) << left << similares[i].second
                         << " " << fixed << setprecision(3) << similares[i].first << " ";
                    for (int b = 0; b < barras; b++) cout << "█";
                    cout << endl;
                }
            }
        } else if (entrada == "/vocab") {
            // Comando /vocab: mostra todas as palavras
            cout << "  Vocabulário (" << indice.size() << " palavras):" << endl;
            int count = 0;
            for (auto& [pal, idx] : indice) {
                cout << "  " << setw(15) << left << pal;
                if (++count % 5 == 0) cout << endl;
            }
            if (count % 5 != 0) cout << endl;
        } else {
            // Frase normal: busca similaridade
            processarFrase(entrada, indice, embeddings);
        }
    }
}

// ============================================================================
//  MAIN
// ============================================================================

int main(int argc, char* argv[]) {
    string caminhoBin = "dados/embeddings/vetores_word2vec.bin";

    // Parse de argumentos de linha de comando
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--arquivo" && i+1 < argc)
            caminhoBin = argv[++i];
        else if (arg == "--ajuda" || arg == "-h") {
            cout << "Uso: ./natacha_embedder [--arquivo <caminho>]" << endl;
            return 0;
        }
    }

    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║           NATACHA EMBEDDER — Carregando...          ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝" << endl;

    // Carrega embeddings do arquivo binário
    vector<Embedding> embeddings;
    int dimensao;
    auto indice = carregarEmbeddings(caminhoBin, embeddings, dimensao);

    if (indice.empty()) {
        cerr << "Erro: nenhum embedding carregado." << endl;
        return 1;
    }

    cout << "  Arquivo: " << caminhoBin << endl;
    cout << "  Palavras: " << indice.size() << endl;
    cout << "  Dimensão: " << dimensao << " floats" << endl;

    // Entra no modo interativo
    modoInterativo(indice, embeddings);

    return 0;
}
