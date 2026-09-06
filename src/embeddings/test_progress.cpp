#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

vector<string> tokenizar(const string& texto) {
    vector<string> tokens;
    stringstream ss(texto);
    string palavra;
    while (ss >> palavra) {
        for (char& c : palavra) c = tolower(c);
        string limpa;
        for (char c : palavra)
            if (isalnum(c)) limpa += c;
        if (!limpa.empty())
            tokens.push_back(limpa);
    }
    return tokens;
}

float cosseno(const vector<float>& a, const vector<float>& b) {
    float produto = 0.0f, normaA = 0.0f, normaB = 0.0f;
    for (size_t i = 0; i < a.size(); i++) {
        produto += a[i] * b[i];
        normaA  += a[i] * a[i];
        normaB  += b[i] * b[i];
    }
    if (normaA == 0.0f || normaB == 0.0f) return 0.0f;
    return produto / (sqrt(normaA) * sqrt(normaB));
}

string lerCorpus(const string& caminho) {
    ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        cerr << "ERRO: Nao conseguiu abrir '" << caminho << "'" << endl;
        return "";
    }
    string conteudo, linha;
    while (getline(arquivo, linha))
        conteudo += linha + " ";
    arquivo.close();
    return conteudo;
}

class Vocabulario {
public:
    unordered_map<string, int> palavraParaId;
    vector<string> idParaPalavra;
    int tamanho = 0;

    void construir(const vector<string>& tokens) {
        for (const string& token : tokens) {
            if (palavraParaId.find(token) == palavraParaId.end()) {
                palavraParaId[token] = tamanho;
                idParaPalavra.push_back(token);
                tamanho++;
            }
        }
    }

    int id(const string& palavra) const {
        auto it = palavraParaId.find(palavra);
        if (it != palavraParaId.end()) return it->second;
        return -1;
    }

    string palavra(int id) const {
        if (id >= 0 && id < (int)idParaPalavra.size()) return idParaPalavra[id];
        return "<OOV>";
    }
};

class EmbeddingTable {
public:
    int vocabSize;
    int dimensao;
    vector<vector<float>> vetores;

    EmbeddingTable(int vocab = 0, int dim = 0, unsigned int semente = 42) {
        vocabSize = vocab;
        dimensao  = dim;
        if (vocab > 0 && dim > 0) {
            srand(semente);
            vetores.resize(vocabSize, vector<float>(dimensao));
            float escala = sqrt(1.0f / dimensao);
            for (int i = 0; i < vocabSize; i++)
                for (int j = 0; j < dimensao; j++)
                    vetores[i][j] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * escala;
        }
    }

    vector<float> get(int palavraId) const {
        return vetores[palavraId];
    }

    void update(int palavraId, const vector<float>& gradiente, float taxa) {
        for (int j = 0; j < dimensao; j++)
            vetores[palavraId][j] -= taxa * gradiente[j];
    }

    void salvar(const string& caminho, const Vocabulario& vocab) const {
        json j;
        j["vocabSize"] = vocabSize;
        j["dimensao"]  = dimensao;

        json embeddingsArray = json::array();
        for (int i = 0; i < vocabSize; i++) {
            json item;
            item["palavra"] = vocab.palavra(i);
            item["vetor"]   = vetores[i];
            embeddingsArray.push_back(item);
        }
        j["embeddings"] = embeddingsArray;

        ofstream arquivo(caminho);
        if (!arquivo.is_open()) {
            cerr << "ERRO: nao conseguiu salvar em '" << caminho << "'" << endl;
            return;
        }
        arquivo << j.dump(2);
        arquivo.close();
        cout << "  Embeddings salvos em: " << caminho << endl;
    }
};

class Word2Vec {
public:
    EmbeddingTable* embeddings;
    int dimensao;
    int vocabSize;
    float taxa = 0.001f;
    vector<vector<float>> pesosSaida;

    Word2Vec(int vocab, int dim, unsigned int semente = 42) {
        vocabSize  = vocab;
        dimensao   = dim;
        embeddings = new EmbeddingTable(vocab, dim, semente);
        srand(semente + 1);
        pesosSaida.resize(vocabSize, vector<float>(dimensao));
        float escala = sqrt(1.0f / dimensao);
        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                pesosSaida[i][j] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * escala;
    }

    ~Word2Vec() {
        delete embeddings;
    }

    float treinar(int central, int contexto) {
        vector<float> h = embeddings->get(central);
        
        // Forward
        vector<float> scores(vocabSize);
        for (int i = 0; i < vocabSize; i++) {
            scores[i] = 0.0f;
            for (int j = 0; j < dimensao; j++)
                scores[i] += pesosSaida[i][j] * h[j];
        }
        float maxVal = *max_element(scores.begin(), scores.end());
        vector<float> probs(vocabSize);
        float soma = 0.0f;
        for (int i = 0; i < vocabSize; i++) {
            probs[i] = exp(scores[i] - maxVal);
            soma += probs[i];
        }
        for (int i = 0; i < vocabSize; i++)
            probs[i] /= soma;
        
        float perda = -log(probs[contexto] + 1e-9f);

        // Backward
        vector<float> erro(vocabSize);
        for (int i = 0; i < vocabSize; i++)
            erro[i] = probs[i] - (i == contexto ? 1.0f : 0.0f);

        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                pesosSaida[i][j] -= taxa * erro[i] * h[j];

        vector<float> gradEmbedding(dimensao, 0.0f);
        for (int j = 0; j < dimensao; j++)
            for (int i = 0; i < vocabSize; i++)
                gradEmbedding[j] += erro[i] * pesosSaida[i][j];

        embeddings->update(central, gradEmbedding, taxa);
        return perda;
    }

    void topSimilares(const string& alvo, const Vocabulario& vocab, int n = 5) {
        int idAlvo = vocab.id(alvo);
        if (idAlvo == -1) {
            cout << "  \"" << alvo << "\" nao esta no vocabulario." << endl;
            return;
        }
        vector<float> embAlvo = embeddings->get(idAlvo);
        vector<pair<float, string>> sims;
        for (int i = 0; i < vocabSize; i++) {
            if (i == idAlvo) continue;
            float sim = cosseno(embAlvo, embeddings->get(i));
            sims.push_back({sim, vocab.palavra(i)});
        }
        sort(sims.rbegin(), sims.rend());
        cout << "  Top " << n << " mais similares a \"" << alvo << "\":" << endl;
        for (int i = 0; i < min(n, (int)sims.size()); i++)
            cout << "    " << setw(12) << left << sims[i].second
                 << " sim=" << fixed << setprecision(4) << sims[i].first << endl;
    }
};

vector<pair<int, int>> gerarPares(const vector<string>& tokens,
                                   const Vocabulario& vocab,
                                   int janela = 2) {
    vector<pair<int, int>> pares;
    int n = (int)tokens.size();
    for (int i = 0; i < n; i++) {
        int central = vocab.id(tokens[i]);
        for (int offset = -janela; offset <= janela; offset++) {
            if (offset == 0) continue;
            int j = i + offset;
            if (j >= 0 && j < n) {
                int contexto = vocab.id(tokens[j]);
                pares.push_back({central, contexto});
            }
        }
    }
    return pares;
}

int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - Teste de Progresso" << endl;
    cout << "============================================================" << endl;

    string corpus = lerCorpus("../../dados/embeddings/corpus.txt");
    if (corpus.empty()) return 1;

    cout << "Corpus: " << corpus.size() << " chars" << endl;
    vector<string> tokens = tokenizar(corpus);
    cout << "Tokens: " << tokens.size() << endl;

    Vocabulario vocab;
    vocab.construir(tokens);
    cout << "Vocabulario: " << vocab.tamanho << " palavras" << endl;

    int dim = 15, janela = 2, epocas = 100;

    cout << "Pares: " << gerarPares(tokens, vocab, janela).size() << endl;
    cout << endl << "Treinando " << epocas << " epocas (imprime a cada 10)..." << endl;

    Word2Vec modelo(vocab.tamanho, dim, 42);
    auto pares = gerarPares(tokens, vocab, janela);

    for (int e = 0; e < epocas; e++) {
        float perdaTotal = 0.0f;
        for (auto& par : pares) perdaTotal += modelo.treinar(par.first, par.second);
        float perdaMedia = perdaTotal / pares.size();
        if (e % 10 == 0)
            cout << "  Epoca " << setw(4) << e << " | Perda: " << fixed << setprecision(4) << perdaMedia << endl;
    }

    cout << endl << "=== TOP SIMILARES (100 epocas) ===" << endl << endl;
    modelo.topSimilares("natacha", vocab, 5);
    cout << endl;
    modelo.topSimilares("felix", vocab, 5);
    cout << endl;
    modelo.topSimilares("cpu", vocab, 5);

    cout << endl << "Salvando..." << endl;
    modelo.embeddings->salvar("../../dados/embeddings/natacha_embeddings.json", vocab);

    cout << "Pronto!" << endl;
    return 0;
}
