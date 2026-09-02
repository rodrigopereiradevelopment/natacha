// ============================================================================
//  EMBEDDINGS — NATACHA FASE 3 (v3: le corpus de arquivo)
//
//  Mudancas:
//    1. Le corpus de 'corpus.txt' (voce edita sem recompilar)
//    2. Correcao RAND_MAX (cast pra float)
//    3. Mostra estatisticas de treino
//    4. Testes de similaridade com palavras reais do corpus
//    5. Top-5 palavras mais similares a uma palavra-alvo
//    6. Early stopping (para quando perda nao melhora)
//
//  Uso:
//    1. Edite corpus.txt com suas frases
//    2. Compile: clang++ -std=c++17 -O2 -o word2vec_v3 word2vec_v3.cpp -lm
//    3. Execute: ./word2vec_v3
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

using namespace std;

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
        cerr << "Crie um arquivo 'corpus.txt' no mesmo diretorio." << endl;
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
// EMBEDDING TABLE
// ----------------------------------------------------------------------------
class EmbeddingTable {
public:
    int vocabSize;
    int dimensao;
    vector<vector<float>> vetores;

    EmbeddingTable(int vocab, int dim, unsigned int semente = 42) {
        vocabSize = vocab;
        dimensao  = dim;
        srand(semente);
        vetores.resize(vocabSize, vector<float>(dimensao));
        float escala = sqrt(1.0f / dimensao);
        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                vetores[i][j] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * escala;
    }

    vector<float> get(int palavraId) const {
        return vetores[palavraId];
    }

    void update(int palavraId, const vector<float>& gradiente, float taxa) {
        for (int j = 0; j < dimensao; j++)
            vetores[palavraId][j] -= taxa * gradiente[j];
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
        srand(semente + 1); // semente diferente pra pesos de saida
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

    // Retorna a perda (cross-entropy) pra monitorar treino
    float treinar(int central, int contexto) {
        vector<float> h     = embeddings->get(central);
        vector<float> probs = forward(central);

        // Perda cross-entropy
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

    // Top-N palavras mais similares a uma palavra-alvo
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
    cout << "  NATACHA - Fase 3: Embeddings (le corpus.txt)" << endl;
    cout << "============================================================" << endl;

    // Le corpus do arquivo
    string corpus = lerCorpus("corpus.txt");
    if (corpus.empty()) return 1;

    cout << endl << "Corpus lido de 'corpus.txt'" << endl;
    cout << "Tamanho: " << corpus.size() << " caracteres" << endl;

    vector<string> tokens = tokenizar(corpus);
    cout << "Tokens: " << tokens.size() << endl;

    Vocabulario vocab;
    vocab.construir(tokens);
    cout << "Vocabulario: " << vocab.tamanho << " palavras unicas" << endl;

    // ================================================================
    // TREINAMENTO com early stopping
    // ================================================================
    int dimensaoEmbedding = 15;
    int janela  = 2;
    int epocas  = 10000;
    int paciencia = 2000;  // early stopping

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Treinando Word2Vec (Skip-Gram)" << endl;
    cout << "  Dimensao: " << dimensaoEmbedding
         << " | Janela: " << janela
         << " | Max epocas: " << epocas << endl;
    cout << "============================================================" << endl;

    Word2Vec modelo(vocab.tamanho, dimensaoEmbedding, 42);

    vector<pair<int, int>> pares = gerarPares(tokens, vocab, janela);
    cout << endl << "Pares (central, contexto): " << pares.size() << endl << endl;

    float melhorPerda = 1e9f;
    int   semMelhora  = 0;

    for (int e = 0; e < epocas; e++) {
        float perdaTotal = 0.0f;
        for (auto& par : pares)
            perdaTotal += modelo.treinar(par.first, par.second);
        float perdaMedia = perdaTotal / pares.size();

        if (e % 1000 == 0)
            cout << "  Epoca " << setw(5) << e
                 << " | Perda media: " << fixed << setprecision(4) << perdaMedia << endl;

        // Early stopping
        if (perdaMedia < melhorPerda - 0.0001f) {
            melhorPerda = perdaMedia;
            semMelhora  = 0;
        } else {
            semMelhora++;
            if (semMelhora >= paciencia) {
                cout << "  Early stopping na epoca " << e << endl;
                break;
            }
        }
    }

    // ================================================================
    // TESTE 1: Similaridade entre pares de palavras
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Similaridade de palavras (cosseno)" << endl;
    cout << "============================================================" << endl;

    auto testar = [&](const string& a, const string& b) {
        int idA = vocab.id(a);
        int idB = vocab.id(b);
        if (idA == -1 || idB == -1) {
            cout << "  \"" << a << "\" ou \"" << b << "\" nao esta no vocabulario" << endl;
            return;
        }
        float sim = cosseno(modelo.embeddings->get(idA), modelo.embeddings->get(idB));
        cout << fixed << setprecision(4);
        cout << "  cosseno(\"" << a << "\", \"" << b << "\") = " << sim << endl;
    };

    cout << endl << "Pares relacionados (esperado: similaridade alta):" << endl;
    testar("natacha", "felix");
    testar("cozinha", "cpu");
    testar("sala",    "gpu");
    testar("quarto",  "ram");
    testar("porao",   "disco");
    testar("banho",   "privado");

    cout << endl << "Pares nao relacionados (esperado: similaridade baixa):" << endl;
    testar("natacha", "robo");
    testar("felix",   "ferramenta");
    testar("natacha", "wrapper");

    // ================================================================
    // TESTE 2: Top-5 mais similares
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
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
    cout << "  Fim. Edite 'corpus.txt' e execute novamente." << endl;
    cout << "============================================================" << endl;

    return 0;
}
