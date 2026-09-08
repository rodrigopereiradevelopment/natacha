// ============================================================================
//  WORD2VEC COM NEGATIVE SAMPLING — OTIMIZADO
//  
//  Softmax full: 1055 x 1055 = 1.1M ops/par x 14778 pares = ~16B ops/epoca
//  Negative sampling (k=5): 6 x 15 = 90 ops/par x 14778 pares = ~1.3M ops/epoca
//  Reducao: ~12000x mais rapido!
// ============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    vector<int> frequencias;
    int tamanho = 0;

    void construir(const vector<string>& tokens) {
        // Conta frequencias
        unordered_map<string, int> freq;
        for (const string& token : tokens) {
            freq[token]++;
        }
        
        // Ordena por frequencia (desc) e atribui IDs
        vector<pair<int, string>> ordenado;
        for (auto& [palavra, f] : freq) {
            ordenado.push_back({f, palavra});
        }
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

// ----------------------------------------------------------------------------
// EMBEDDING TABLE
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
            float escala = sqrt(0.5f / dimensao);
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
// WORD2VEC COM NEGATIVE SAMPLING
// ----------------------------------------------------------------------------
class Word2Vec {
public:
    EmbeddingTable* embeddings;
    vector<vector<float>> pesosSaida;
    int dimensao;
    int vocabSize;
    float taxa;
    int negativos;
    vector<float> tabelaUnigram; // Para sampling negativo

    Word2Vec(int vocab, int dim, const vector<int>& frequencias, int k = 5, unsigned int semente = 42) {
        vocabSize  = vocab;
        dimensao   = dim;
        negativos  = k;
        taxa       = 0.005f;
        
        embeddings = new EmbeddingTable(vocab, dim, semente);
        srand(semente + 1);
        pesosSaida.resize(vocabSize, vector<float>(dimensao));
        float escala = sqrt(0.5f / dimensao);
        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                pesosSaida[i][j] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * escala;

        // Constroi tabela de unigram para negative sampling
        construirTabelaUnigram(frequencias);
    }

    ~Word2Vec() {
        delete embeddings;
    }

    void construirTabelaUnigram(const vector<int>& frequencias) {
        float soma = 0;
        for (int f : frequencias) soma += pow(f, 0.75f);
        
        tabelaUnigram.resize(vocabSize);
        float acumulado = 0;
        for (int i = 0; i < vocabSize; i++) {
            acumulado += pow(frequencias[i], 0.75f) / soma;
            tabelaUnigram[i] = acumulado;
        }
    }

    int amostrarNegativo() {
        float r = (float)rand() / (float)RAND_MAX;
        auto it = lower_bound(tabelaUnigram.begin(), tabelaUnigram.end(), r);
        int idx = distance(tabelaUnigram.begin(), it);
        return min(idx, vocabSize - 1);
    }

    // Treina um par (central, contexto) com negative sampling
    float treinar(int central, int contexto) {
        vector<float> h = embeddings->get(central);
        
        float perda = 0.0f;
        
        // Treina com a palavra positiva (contexto = 1)
        vector<float> gradEntrada(dimensao, 0.0f);
        
        // Forward para positiva
        float dot = 0.0f;
        for (int j = 0; j < dimensao; j++)
            dot += pesosSaida[contexto][j] * h[j];
        float sig = 1.0f / (1.0f + exp(-dot));
        
        float grad = (1.0f - sig) * taxa;
        perda -= log(sig + 1e-9f);
        
        // Atualiza pesos de saida (positiva)
        for (int j = 0; j < dimensao; j++) {
            pesosSaida[contexto][j] += grad * h[j];
            gradEntrada[j] += grad * pesosSaida[contexto][j];
        }
        
        // Treina com palavras negativas (target = 0)
        for (int n = 0; n < negativos; n++) {
            int neg = amostrarNegativo();
            if (neg == contexto) continue;
            
            dot = 0.0f;
            for (int j = 0; j < dimensao; j++)
                dot += pesosSaida[neg][j] * h[j];
            sig = 1.0f / (1.0f + exp(-dot));
            
            grad = -sig * taxa;
            perda -= log(1.0f - sig + 1e-9f);
            
            // Atualiza pesos de saida (negativa)
            for (int j = 0; j < dimensao; j++) {
                pesosSaida[neg][j] += grad * h[j];
                gradEntrada[j] += grad * pesosSaida[neg][j];
            }
        }
        
        // Atualiza embedding de entrada
        embeddings->update(central, gradEntrada, 1.0f);
        
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
// GERA PARES
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
// ============================================================
//  SUBSAMPLING — remove palavras muito frequentes
// ============================================================
vector<string> aplicarSubsampling(const vector<string>& tokens, float t = 1e-4f) {
    unordered_map<string, int> freq;
    for (const string& token : tokens) freq[token]++;

    int total = tokens.size();
    vector<string> resultado;
    srand(42);  // semente fixa

    for (const string& token : tokens) {
        float f = (float)freq[token] / total;
        float prob = 1.0f - sqrt(t / f);
        if (prob < 0) prob = 0;

        float r = (float)rand() / (float)RAND_MAX;
        if (r > prob) {
            resultado.push_back(token);
        }
    }

    cout << "  Subsampling: " << tokens.size() << " → " << resultado.size() << " tokens" << endl;
    return resultado;
}
// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------
int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - Word2Vec com Negative Sampling (otimizado)" << endl;
    cout << "============================================================" << endl;

    string corpus = lerCorpus("../../dados/embeddings/corpus.txt");
    if (corpus.empty()) return 1;

    cout << endl << "Corpus: " << corpus.size() << " chars, ";
    vector<string> tokens = tokenizar(corpus);
    cout << tokens.size() << " tokens" << endl;

    // 🔥 SUBSAMPLING — remove palavras muito frequentes
    tokens = aplicarSubsampling(tokens, 1e-4f);

    Vocabulario vocab;
    vocab.construir(tokens);
    cout << "Vocabulario: " << vocab.tamanho << " palavras" << endl;

    int dim = 64, janela = 5, epocas = 10000, negativos = 5;

    auto pares = gerarPares(tokens, vocab, janela);
    cout << "Pares por epoca: " << pares.size() << endl;
    cout << "Negativos por par: " << negativos << endl;
    cout << "Ops por epoca (estimado): " << pares.size() * (1 + negativos) * dim * 4 << endl;

    cout << endl << "--- Treinando " << epocas << " epocas ---" << endl;
    Word2Vec modelo(vocab.tamanho, dim, vocab.frequencias, negativos, 42);

    // 🔥 DECAY CONTÍNUO DA TAXA DE APRENDIZADO
    float alpha_inicial = 0.025f;
    float alpha_min = alpha_inicial * 0.0001f;  // 0.0000025
    int total_tokens_processados = 0;
    int total_pares = pares.size();

    float melhorPerda = 1e9f;
    int semMelhora = 0;
    clock_t inicio = clock();

    for (int e = 0; e < epocas; e++) {
        float perdaTotal = 0.0f;
        
        for (auto& par : pares) {
            // Taxa decrescente contínua
            float alpha = alpha_inicial * (1.0f - (float)total_tokens_processados /
                          (float)(epocas * total_pares));
            if (alpha < alpha_min) alpha = alpha_min;
            modelo.taxa = alpha;

            perdaTotal += modelo.treinar(par.first, par.second);
            total_tokens_processados++;
        }
        
        float perdaMedia = perdaTotal / pares.size();
        
        if (e % 100 == 0) {
            float elapsed = (float)(clock() - inicio) / CLOCKS_PER_SEC;
            cout << "  Epoca " << setw(5) << e 
                 << " | Perda: " << fixed << setprecision(4) << perdaMedia
                 << " | Tempo: " << fixed << setprecision(1) << elapsed << "s" << endl;
        }
        
        if (perdaMedia < melhorPerda - 0.0001f) {
            melhorPerda = perdaMedia;
            semMelhora = 0;
        } else if (++semMelhora >= 5000) {
            cout << "  Early stopping na epoca " << e << endl;
            break;
        }
    }

    float tempoTotal = (float)(clock() - inicio) / CLOCKS_PER_SEC;
    cout << endl << "Treinamento concluido em " << fixed << setprecision(1) << tempoTotal << "s" << endl;
    cout << "Melhor perda: " << melhorPerda << endl;

    cout << endl << "--- Salvando embeddings ---" << endl;
    modelo.embeddings->salvar("../../dados/embeddings/natacha_embeddings.json", vocab);

    cout << endl << "=== TESTE DE SIMILARIDADE ===" << endl << endl;

    auto testar = [&](const string& a, const string& b) {
        int idA = vocab.id(a), idB = vocab.id(b);
        if (idA == -1 || idB == -1) return;
        float sim = cosseno(modelo.embeddings->get(idA), modelo.embeddings->get(idB));
        cout << "  " << setw(12) << left << a << " <-> " << setw(12) << b << " = " << fixed << setprecision(4) << sim << endl;
    };

    testar("natacha", "felix");
    testar("natacha", "rodrigo");
    testar("cozinha", "cpu");
    testar("quarto", "ram");
    testar("banho", "privado");
    testar("c++", "codigo");
    testar("mlp", "neuronio");
    testar("embeddings", "palavras");
    testar("cafe", "processamento");
    testar("felix", "gato");

    cout << endl << "=== TOP SIMILARES ===" << endl << endl;

    modelo.topSimilares("natacha", vocab, 8);
    cout << endl;
    modelo.topSimilares("felix", vocab, 8);
    cout << endl;
    modelo.topSimilares("cpu", vocab, 8);
    cout << endl;
    modelo.topSimilares("cafe", vocab, 8);
    cout << endl;
    modelo.topSimilares("aprende", vocab, 8);

    cout << endl << "============================================================" << endl;
    cout << "  Natacha treinou " << epocas << " epocas com Negative Sampling" << endl;
    cout << "  Vocabulario: " << vocab.tamanho << " palavras" << endl;
    cout << "  Tempo total: " << fixed << setprecision(1) << tempoTotal << "s" << endl;
    cout << "============================================================" << endl;

    return 0;
}