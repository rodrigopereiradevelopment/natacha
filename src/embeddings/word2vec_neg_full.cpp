// ============================================================================
//  WORD2VEC COM NEGATIVE SAMPLING — VERSAO COMPLETA
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
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

vector<string> tokenizar(const string& texto) {
    vector<string> tokens;
    stringstream ss(texto);
    string palavra;
    while (ss >> palavra) {
        // Preserva c++, c#, etc.
        string lower = palavra;
        for (char& c : lower) c = tolower(c);
        
        if (lower == "c++" || lower == "c#") {
            tokens.push_back(lower);
            continue;
        }
        
        string limpa;
        for (char c : lower)
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
    vector<int> frequencias;
    int tamanho = 0;

    void construir(const vector<string>& tokens) {
        unordered_map<string, int> freq;
        for (const string& token : tokens) freq[token]++;
        
        vector<pair<int, string>> ordenado;
        for (auto& [palavra, f] : freq) ordenado.push_back({f, palavra});
        sort(ordenado.rbegin(), ordenado.rend());
        
        for (auto& [f, palavra] : ordenado) {
            palavraParaId[palavra] = tamanho;
            idParaPalavra.push_back(palavra);
            frequencias.push_back(f);
            tamanho++;
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

    vector<float> get(int palavraId) const { return vetores[palavraId]; }

    void update(int palavraId, const vector<float>& gradiente, float taxa) {
        for (int j = 0; j < dimensao; j++)
            vetores[palavraId][j] -= taxa * gradiente[j];
    }

    void salvar(const string& caminho, const Vocabulario& vocab) const {
        json j;
        j["vocabSize"] = vocabSize;
        j["dimensao"]  = dimensao;
        json arr = json::array();
        for (int i = 0; i < vocabSize; i++) {
            arr.push_back({{"palavra", vocab.palavra(i)}, {"vetor", vetores[i]}});
        }
        j["embeddings"] = arr;
        ofstream arq(caminho);
        arq << j.dump(2);
        arq.close();
        cout << "  Salvo em: " << caminho << endl;
    }
};

class Word2Vec {
public:
    EmbeddingTable* embeddings;
    vector<vector<float>> pesosSaida;
    int dimensao, vocabSize, negativos;
    float taxa;
    vector<float> tabelaUnigram;

    Word2Vec(int vocab, int dim, const vector<int>& freq, int k = 5, unsigned int semente = 42) {
        vocabSize = vocab; dimensao = dim; negativos = k; taxa = 0.01f;
        embeddings = new EmbeddingTable(vocab, dim, semente);
        srand(semente + 1);
        pesosSaida.resize(vocabSize, vector<float>(dimensao));
        float escala = sqrt(1.0f / dimensao);
        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                pesosSaida[i][j] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * escala;

        float soma = 0;
        for (int f : freq) soma += pow(f, 0.75f);
        tabelaUnigram.resize(vocabSize);
        float acc = 0;
        for (int i = 0; i < vocabSize; i++) {
            acc += pow(freq[i], 0.75f) / soma;
            tabelaUnigram[i] = acc;
        }
    }

    ~Word2Vec() { delete embeddings; }

    int amostrarNegativo() {
        float r = (float)rand() / (float)RAND_MAX;
        auto it = lower_bound(tabelaUnigram.begin(), tabelaUnigram.end(), r);
        return min((int)distance(tabelaUnigram.begin(), it), vocabSize - 1);
    }

    float treinar(int central, int contexto) {
        vector<float> h = embeddings->get(central);
        float perda = 0.0f;
        vector<float> gradIn(dimensao, 0.0f);

        // Positiva
        float dot = 0;
        for (int j = 0; j < dimensao; j++) dot += pesosSaida[contexto][j] * h[j];
        float sig = 1.0f / (1.0f + exp(-dot));
        float g = (1.0f - sig) * taxa;
        perda -= log(sig + 1e-9f);
        for (int j = 0; j < dimensao; j++) {
            pesosSaida[contexto][j] += g * h[j];
            gradIn[j] += g * pesosSaida[contexto][j];
        }

        // Negativas
        for (int n = 0; n < negativos; n++) {
            int neg = amostrarNegativo();
            if (neg == contexto) continue;
            dot = 0;
            for (int j = 0; j < dimensao; j++) dot += pesosSaida[neg][j] * h[j];
            sig = 1.0f / (1.0f + exp(-dot));
            g = -sig * taxa;
            perda -= log(1.0f - sig + 1e-9f);
            for (int j = 0; j < dimensao; j++) {
                pesosSaida[neg][j] += g * h[j];
                gradIn[j] += g * pesosSaida[neg][j];
            }
        }

        embeddings->update(central, gradIn, 1.0f);
        return perda;
    }

    void topSimilares(const string& alvo, const Vocabulario& vocab, int n = 5) {
        int idAlvo = vocab.id(alvo);
        if (idAlvo == -1) { cout << "  \"" << alvo << "\" N/A" << endl; return; }
        vector<float> emb = embeddings->get(idAlvo);
        vector<pair<float, string>> sims;
        for (int i = 0; i < vocabSize; i++) {
            if (i == idAlvo) continue;
            sims.push_back({cosseno(emb, embeddings->get(i)), vocab.palavra(i)});
        }
        sort(sims.rbegin(), sims.rend());
        cout << "  " << alvo << ": ";
        for (int i = 0; i < min(n, (int)sims.size()); i++)
            cout << sims[i].second << "(" << fixed << setprecision(2) << sims[i].first << ") ";
        cout << endl;
    }
};

vector<pair<int, int>> gerarPares(const vector<string>& tokens, const Vocabulario& vocab, int janela = 2) {
    vector<pair<int, int>> pares;
    int n = tokens.size();
    for (int i = 0; i < n; i++) {
        int c = vocab.id(tokens[i]);
        for (int o = -janela; o <= janela; o++) {
            if (o == 0) continue;
            int j = i + o;
            if (j >= 0 && j < n) pares.push_back({c, vocab.id(tokens[j])});
        }
    }
    return pares;
}

int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - Word2Vec Negative Sampling (otimizado)" << endl;
    cout << "============================================================" << endl;

    string corpus = lerCorpus("../../dados/embeddings/corpus.txt");
    if (corpus.empty()) return 1;

    vector<string> tokens = tokenizar(corpus);
    Vocabulario vocab;
    vocab.construir(tokens);
    
    auto pares = gerarPares(tokens, vocab, 2);
    
    cout << "Corpus: " << tokens.size() << " tokens | Vocab: " << vocab.tamanho 
         << " | Pares/epoca: " << pares.size() << endl;

    int epocas = 5000;
    Word2Vec modelo(vocab.tamanho, 15, vocab.frequencias, 5, 42);

    cout << endl << "Treinando " << epocas << " epocas..." << endl << endl;

    float melhorPerda = 1e9f;
    int semMelhora = 0;
    clock_t inicio = clock();

    for (int e = 0; e < epocas; e++) {
        float perdaTotal = 0.0f;
        for (auto& par : pares) perdaTotal += modelo.treinar(par.first, par.second);
        float perdaMedia = perdaTotal / pares.size();
        
        if (e % 200 == 0) {
            float elapsed = (float)(clock() - inicio) / CLOCKS_PER_SEC;
            cout << "  Epoca " << setw(5) << e << " | Perda: " << fixed << setprecision(4) << perdaMedia
                 << " | " << fixed << setprecision(1) << elapsed << "s" << endl;
        }
        
        if (perdaMedia < melhorPerda - 0.0001f) { melhorPerda = perdaMedia; semMelhora = 0; }
        else if (++semMelhora >= 2000) { cout << "  Early stop epoca " << e << endl; break; }
    }

    float tempoTotal = (float)(clock() - inicio) / CLOCKS_PER_SEC;
    cout << endl << "Concluido em " << fixed << setprecision(1) << tempoTotal << "s | Melhor perda: " << melhorPerda << endl;

    modelo.embeddings->salvar("../../dados/embeddings/natacha_embeddings.json", vocab);

    cout << endl << "=== SIMILARIDADES ===" << endl << endl;
    modelo.topSimilares("natacha", vocab, 8);
    modelo.topSimilares("felix", vocab, 8);
    modelo.topSimilares("cpu", vocab, 8);
    modelo.topSimilares("quarto", vocab, 5);
    modelo.topSimilares("banho", vocab, 5);
    modelo.topSimilares("aprende", vocab, 5);
    modelo.topSimilares("c++", vocab, 5);
    modelo.topSimilares("rodrigo", vocab, 5);

    cout << endl << "============================================================" << endl;
    cout << "  FIM - " << epocas << " epocas em " << fixed << setprecision(1) << tempoTotal << "s" << endl;
    cout << "============================================================" << endl;
    return 0;
}
