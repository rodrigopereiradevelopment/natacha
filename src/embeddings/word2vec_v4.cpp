// ============================================================================
//  EMBEDDINGS — NATACHA FASE 3 (v4: salva/carrega embeddings em JSON)
//
//  Mudancas:
//    1. Le corpus de 'corpus.txt'
//    2. Salva embeddings em 'natacha_embeddings.json'
//    3. Carrega embeddings de JSON (sem re-treinar)
//    4. Usa nlohmann/json (igual ao MLP da Fase 2)
//    5. Prova: treina -> salva -> novo modelo -> carrega -> mesma similaridade
//
//  Dependencia: nlohmann/json (json.hpp no mesmo diretorio ou no include path)
//  Uso:
//    1. Edite corpus.txt
//    2. Compile: clang++ -std=c++17 -O2 -I. -o word2vec_v4 word2vec_v4.cpp -lm
//    3. Execute: ./word2vec_v4
// ============================================================================

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
#include <numeric>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// ----------------------------------------------------------------------------
// UTILITARIOS
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// LE CORPUS DO ARQUIVO
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// VOCABULARIO
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// EMBEDDING TABLE (com salvar/carregar via nlohmann/json)
// ----------------------------------------------------------------------------
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

    // ======================================================================
    // SALVAR com nlohmann/json (limpo e robusto)
    // ======================================================================
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

    // ======================================================================
    // CARREGAR com nlohmann/json (limpo e robusto)
    // ======================================================================
    bool carregar(const string& caminho) {
    ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        cerr << "ERRO: nao conseguiu carregar de '" << caminho << "'" << endl;
        return false;
    }

    json j;
    arquivo >> j;
    arquivo.close();

    vocabSize = j.value("vocabSize", 0);
    dimensao  = j.value("dimensao", 0);
    vetores.clear();

    for (const auto& item : j["embeddings"]) {
        vector<float> vetor = item["vetor"].get<vector<float>>();
        vetores.push_back(vetor);
    }

    cout << "  Embeddings carregados: " << vocabSize
         << " palavras, dimensao " << dimensao << endl;
    return true;
    }
};

// ----------------------------------------------------------------------------
// WORD2VEC SKIP-GRAM
// ----------------------------------------------------------------------------
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

    vector<float> forward(int palavraCentral) {
        vector<float> h = embeddings->get(palavraCentral);
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
        return probs;
    }

    float treinar(int central, int contexto) {
        vector<float> h     = embeddings->get(central);
        vector<float> probs = forward(central);
        float perda = -log(probs[contexto] + 1e-9f);

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

// ----------------------------------------------------------------------------
// GERA PARES (central, contexto)
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------
int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - Fase 3: Embeddings + nlohmann/json (v4)" << endl;
    cout << "============================================================" << endl;

    // ================================================================
    // PARTE 1: Treinar e salvar
    // ================================================================
    string corpus = lerCorpus("corpus.txt");
    if (corpus.empty()) return 1;

    cout << endl << "Corpus lido: " << corpus.size() << " chars, ";
    vector<string> tokens = tokenizar(corpus);
    cout << tokens.size() << " tokens" << endl;

    Vocabulario vocab;
    vocab.construir(tokens);
    cout << "Vocabulario: " << vocab.tamanho << " palavras" << endl;

    int dim = 15, janela = 2, epocas = 10000, paciencia = 2000;

    cout << endl << "--- PARTE 1: Treinando ---" << endl;
    Word2Vec modelo(vocab.tamanho, dim, 42);
    auto pares = gerarPares(tokens, vocab, janela);

    float melhorPerda = 1e9f;
    int semMelhora = 0;
    for (int e = 0; e < epocas; e++) {
        float perdaTotal = 0.0f;
        for (auto& par : pares) perdaTotal += modelo.treinar(par.first, par.second);
        float perdaMedia = perdaTotal / pares.size();
        if (e % 1000 == 0)
            cout << "  Epoca " << e << " | Perda: " << perdaMedia << endl;
        if (perdaMedia < melhorPerda - 0.0001f) {
            melhorPerda = perdaMedia;
            semMelhora = 0;
        } else if (++semMelhora >= paciencia) {
            cout << "  Early stopping na epoca " << e << endl;
            break;
        }
    }

    // Salva embeddings
    modelo.embeddings->salvar("natacha_embeddings.json", vocab);

    // Teste antes de salvar
    cout << endl << "--- Similaridade ANTES de salvar ---" << endl;
    auto testar = [&](const string& a, const string& b) {
        int idA = vocab.id(a), idB = vocab.id(b);
        if (idA == -1 || idB == -1) return;
        float sim = cosseno(modelo.embeddings->get(idA), modelo.embeddings->get(idB));
        cout << "  " << a << "-" << b << " = " << fixed << setprecision(4) << sim << endl;
    };
    testar("natacha", "felix");
    testar("cozinha", "cpu");
    testar("banho", "privado");

    // ================================================================
    // PARTE 2: Carregar e provar que é igual
    // ================================================================
    cout << endl << "--- PARTE 2: Carregando embeddings salvos ---" << endl;
    EmbeddingTable tabelaCarregada;
    if (!tabelaCarregada.carregar("natacha_embeddings.json")) return 1;

    // Compara embedding de "natacha" antes e depois
    int idNatacha = vocab.id("natacha");
    vector<float> embOriginal = modelo.embeddings->get(idNatacha);
    vector<float> embCarregado = tabelaCarregada.get(idNatacha);
    float diff = 0.0f;
    for (int j = 0; j < dim; j++) diff += fabs(embOriginal[j] - embCarregado[j]);
    cout << "  Diferenca total embedding 'natacha': " << diff << endl;
    if (diff < 0.001f)
        cout << "  ✅ Embeddings identicos! Memoria persiste." << endl;
    else
        cout << "  ❌ ALGO ERRADO: embeddings diferentes!" << endl;

    // Teste depois de carregar
    cout << endl << "--- Similaridade DEPOIS de carregar ---" << endl;
    auto testarCarregado = [&](const string& a, const string& b) {
        int idA = vocab.id(a), idB = vocab.id(b);
        if (idA == -1 || idB == -1) return;
        float sim = cosseno(tabelaCarregada.get(idA), tabelaCarregada.get(idB));
        cout << "  " << a << "-" << b << " = " << fixed << setprecision(4) << sim << endl;
    };
    testarCarregado("natacha", "felix");
    testarCarregado("cozinha", "cpu");
    testarCarregado("banho", "privado");

    cout << endl;
    cout << 
      "============================================================" << endl;
    cout << "  Top similares por palavra" << endl;
    cout << "============================================================" << endl;
    cout << endl;

    modelo.topSimilares("natacha", vocab, 5);
    cout << endl;
    modelo.topSimilares("felix",   vocab, 5);
    cout << endl;
    modelo.topSimilares("cpu",     vocab, 5);
    cout << endl;
    modelo.topSimilares("aprende", vocab, 5);

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Fim. Natacha lembra de tudo mesmo depois de 'dormir'." << endl;
    cout << "============================================================" << endl;

    return 0;
}
