// ============================================================================
//  EMBEDDINGS — NATACHA FASE 3
//
//  Objetivo: transformar PALAVRAS em NUMEROS que a rede entende.
//
//  Passos:
//    1. Tokenizacao: "o gato dorme" -> ["o", "gato", "dorme"]
//    2. Vocabulario: lista unica de palavras -> {"o":0, "gato":1, "dorme":2, ...}
//    3. One-hot: cada palavra -> vetor gigante com 1 na posicao dela
//    4. Embedding: vetor DENso (pequeno, aprendido) que captura significado
//
//  Word2Vec Skip-gram:
//    - Entrada: palavra central
//    - Saida: palavras do contexto (vizinhas)
//    - A rede aprende a prever "o que aparece perto" -> embeddings semanticos
//
//  Exemplo: "o gato dorme no sofa"
//    Janela = 2 (2 palavras pra cada lado)
//    Para "dorme" (central):
//      Contexto: ["o", "gato", "no", "sofa"]
//    A rede aprende que "gato" e "cachorro" tem contextos parecidos,
//    entao seus embeddings ficam proximos no espaco vetorial.
// ============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

// ----------------------------------------------------------------------------
// UTILITARIOS
// ----------------------------------------------------------------------------

// Quebra uma frase em tokens (palavras simples, lowercase)
vector<string> tokenizar(const string& texto) {
    vector<string> tokens;
    stringstream ss(texto);
    string palavra;
    while (ss >> palavra) {
        // lowercase simples
        for (char& c : palavra) c = tolower(c);
        tokens.push_back(palavra);
    }
    return tokens;
}

// Calcula similaridade de cosseno entre dois vetores
// 1.0 = identicos, 0.0 = ortogonais, -1.0 = opostos
float cosseno(const vector<float>& a, const vector<float>& b) {
    float produto = 0.0f, normaA = 0.0f, normaB = 0.0f;
    for (size_t i = 0; i < a.size(); i++) {
        produto += a[i] * b[i];
        normaA += a[i] * a[i];
        normaB += b[i] * b[i];
    }
    if (normaA == 0.0f || normaB == 0.0f) return 0.0f;
    return produto / (sqrt(normaA) * sqrt(normaB));
}

// ----------------------------------------------------------------------------
// VOCABULARIO
// ----------------------------------------------------------------------------
// Mapeia cada palavra unica para um indice numerico
// Ex: {"o":0, "gato":1, "dorme":2, "no":3, "sofa":4}
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
        return -1; // palavra desconhecida (OOV)
    }

    string palavra(int id) const {
        if (id >= 0 && id < (int)idParaPalavra.size()) return idParaPalavra[id];
        return "<OOV>";
    }
};

// ----------------------------------------------------------------------------
// EMBEDDING TABLE (Lookup Table)
// ----------------------------------------------------------------------------
// Cada palavra do vocabulario tem um vetor denso de 'dimensao' numeros.
// Esses vetores SAO APRENDIDOS durante o treino (Word2Vec).
//
// Formato: embeddings[palavra_id] = vetor de tamanho 'dimensao'
// ----------------------------------------------------------------------------
class EmbeddingTable {
public:
    int vocabSize;
    int dimensao;
    vector<vector<float>> vetores;  // vetores[palavra_id] = embedding

    EmbeddingTable(int vocab, int dim, unsigned int semente = 42) {
        vocabSize = vocab;
        dimensao = dim;
        srand(semente);
        vetores.resize(vocabSize, vector<float>(dimensao));
        // Inicializacao aleatoria pequena (Xavier-like)
        float escala = sqrt(1.0f / dimensao);
        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                vetores[i][j] = ((float)rand() / (float) RAND_MAX * 2.0f - 1.0f) * escala;
    }

    // Retorna o embedding de uma palavra
    vector<float> get(int palavraId) const {
        return vetores[palavraId];
    }

    // Atualiza o embedding de uma palavra (usado no treino)
    void update(int palavraId, const vector<float>& gradiente, float taxa) {
        for (int j = 0; j < dimensao; j++)
            vetores[palavraId][j] -= taxa * gradiente[j];
    }
};

// ----------------------------------------------------------------------------
// WORD2VEC SKIP-GRAM (simplificado)
// ----------------------------------------------------------------------------
// Arquitetura:
//   Input: one-hot da palavra central -> Embedding lookup -> Hidden
//   Output: softmax sobre vocabulario (prever cada palavra do contexto)
//
// Na pratica, usamos Negative Sampling (mais rapido), mas comecamos com
// o basico: prever o contexto inteiro.
// ----------------------------------------------------------------------------
class Word2Vec {
public:
    EmbeddingTable* embeddings;  // tabela de embeddings (sera aprendida)
    int dimensao;
    int vocabSize;
    float taxa = 0.01f;

    // Pesos da camada de saida (projecao do embedding de volta pro vocabulario)
    vector<vector<float>> pesosSaida;  // pesosSaida[vocab][dimensao]

    Word2Vec(int vocab, int dim, unsigned int semente = 42) {
        vocabSize = vocab;
        dimensao = dim;
        embeddings = new EmbeddingTable(vocab, dim, semente);
        srand(semente);

        // Inicializa pesos de saida
        pesosSaida.resize(vocabSize, vector<float>(dimensao));
        float escala = sqrt(1.0f / dimensao);
        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                pesosSaida[i][j] = ((float)rand() / (float) RAND_MAX * 2.0f - 1.0f) * escala;
    }

    ~Word2Vec() {
        delete embeddings;
    }

    // Forward: dado uma palavra central, retorna distribuicao sobre vocabulario
    vector<float> forward(int palavraCentral) {
        vector<float> h = embeddings->get(palavraCentral);  // embedding da palavra

        // Camada de saida: score = pesosSaida * h
        vector<float> scores(vocabSize);
        for (int i = 0; i < vocabSize; i++) {
            scores[i] = 0.0f;
            for (int j = 0; j < dimensao; j++)
                scores[i] += pesosSaida[i][j] * h[j];
        }

        // Softmax
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

    // Treina um par (palavra central, palavra do contexto esperada)
    void treinar(int central, int contexto) {
        vector<float> h = embeddings->get(central);
        vector<float> probs = forward(central);

        // Erro na saida: probabilidade predita - one-hot do contexto
        vector<float> erro(vocabSize);
        for (int i = 0; i < vocabSize; i++)
            erro[i] = probs[i] - (i == contexto ? 1.0f : 0.0f);

        // Gradiente dos pesos de saida
        for (int i = 0; i < vocabSize; i++)
            for (int j = 0; j < dimensao; j++)
                pesosSaida[i][j] -= taxa * erro[i] * h[j];

        // Gradiente do embedding da palavra central
        vector<float> gradEmbedding(dimensao, 0.0f);
        for (int j = 0; j < dimensao; j++)
            for (int i = 0; i < vocabSize; i++)
                gradEmbedding[j] += erro[i] * pesosSaida[i][j];

        embeddings->update(central, gradEmbedding, taxa);
    }
};

// ----------------------------------------------------------------------------
// FUNCAO: gera pares (central, contexto) a partir de um corpus
// ----------------------------------------------------------------------------
vector<pair<int, int>> gerarPares(const vector<string>& tokens,
                                   const Vocabulario& vocab,
                                   int janela = 2) {
    vector<pair<int, int>> pares;
    int n = tokens.size();
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
    cout << "  NATACHA - Fase 3: Embeddings (Word2Vec Skip-gram)" << endl;
    cout << "============================================================" << endl;

    // ================================================================
    // CORPUS: texto simples pra aprender
    // ================================================================
    string corpus =
        "o gato dorme no sofa "
        "o cachorro dorme no chao "
        "o gato come o peixe "
        "o cachorro come a racao "
        "o peixe nada no aquario "
        "o gato pula no sofa "
        "o cachorro corre no parque "
        "o peixe dorme no aquario";

    cout << endl << "Corpus:" << endl;
    cout << "  \"" << corpus << "\"" << endl;

    // Tokeniza
    vector<string> tokens = tokenizar(corpus);
    cout << endl << "Tokens (" << tokens.size() << "): ";
    for (const string& t : tokens) cout << t << " ";
    cout << endl;

    // Constroi vocabulario
    Vocabulario vocab;
    vocab.construir(tokens);
    cout << endl << "Vocabulario (" << vocab.tamanho << " palavras):" << endl;
    for (int i = 0; i < vocab.tamanho; i++)
        cout << "  " << i << ": " << vocab.palavra(i) << endl;

    // ================================================================
    // TREINAMENTO Word2Vec
    // ================================================================
    int dimensaoEmbedding = 10;  // tamanho do vetor de cada palavra
    int janela = 2;              // palavras pra cada lado
    int epocas = 5000;

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Treinando Word2Vec" << endl;
    cout << "  Dimensao do embedding: " << dimensaoEmbedding << endl;
    cout << "  Janela de contexto: " << janela << endl;
    cout << "  Epocas: " << epocas << endl;
    cout << "============================================================" << endl;

    Word2Vec modelo(vocab.tamanho, dimensaoEmbedding, 42);

    vector<pair<int, int>> pares = gerarPares(tokens, vocab, janela);
    cout << endl << "Pares (central, contexto) gerados: " << pares.size() << endl;

    for (int e = 0; e < epocas; e++) {
        for (auto& par : pares)
            modelo.treinar(par.first, par.second);
        if (e % 1000 == 0)
            cout << "  Epoca " << e << " completa" << endl;
    }

    // ================================================================
    // TESTE: Similaridade entre palavras
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Similaridade de palavras (cosseno)" << endl;
    cout << "============================================================" << endl;

    auto testarSimilaridade = [&](const string& a, const string& b) {
        int idA = vocab.id(a);
        int idB = vocab.id(b);
        if (idA == -1 || idB == -1) {
            cout << "  \"" << a << "\" ou \"" << b << "\" nao esta no vocabulario" << endl;
            return;
        }
        vector<float> embA = modelo.embeddings->get(idA);
        vector<float> embB = modelo.embeddings->get(idB);
        float sim = cosseno(embA, embB);
        cout << fixed << setprecision(4);
        cout << "  cosseno(\"" << a << "\", \"" << b << "\") = " << sim << endl;
    };

    cout << endl << "Animais (deveriam ser parecidos):" << endl;
    testarSimilaridade("gato", "cachorro");
    testarSimilaridade("gato", "peixe");
    testarSimilaridade("cachorro", "peixe");

    cout << endl << "Lugares (deveriam ser parecidos):" << endl;
    testarSimilaridade("sofa", "chao");
    testarSimilaridade("sofa", "aquario");

    cout << endl << "Acao vs lugar (deveriam ser diferentes):" << endl;
    testarSimilaridade("dorme", "sofa");
    testarSimilaridade("come", "peixe");

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Proximo: Fase 4 - RNN (Redes Recorrentes)" << endl;
    cout << "============================================================" << endl;

    return 0;
}
