// ============================================================================
// NATACHA — WORD2VEC SKIP-GRAM COM NEGATIVE SAMPLING
// Versão corrigida e otimizada
// ============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <random>
#include <cctype>
#include <chrono>

#include "json.hpp"

using namespace std;
using json = nlohmann::json;


// ============================================================================
// CONFIGURAÇÕES
// ============================================================================

constexpr float EPSILON = 1e-8f;


// ============================================================================
// UTILITÁRIOS
// ============================================================================

string normalizarToken(const string& palavra) {

    string resultado;

    for (char c : palavra) {

        unsigned char uc = static_cast<unsigned char>(c);

        if (isalnum(uc) || c == '+' || c == '#' || c == '-' || c == '_') {
            resultado += static_cast<char>(tolower(uc));
        }
    }

    return resultado;
}


// ----------------------------------------------------------------------------
// TOKENIZAÇÃO
// ----------------------------------------------------------------------------

vector<string> tokenizar(const string& texto) {

    vector<string> tokens;

    stringstream ss(texto);
    string palavra;

    while (ss >> palavra) {

        string token = normalizarToken(palavra);

        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}


// ----------------------------------------------------------------------------
// COSSENO
// ----------------------------------------------------------------------------

float cosseno(const vector<float>& a, const vector<float>& b) {

    if (a.size() != b.size() || a.empty())
        return 0.0f;

    float produto = 0.0f;
    float normaA = 0.0f;
    float normaB = 0.0f;

    for (size_t i = 0; i < a.size(); i++) {

        produto += a[i] * b[i];
        normaA += a[i] * a[i];
        normaB += b[i] * b[i];
    }

    if (normaA < EPSILON || normaB < EPSILON)
        return 0.0f;

    return produto / (sqrt(normaA) * sqrt(normaB));
}


// ----------------------------------------------------------------------------
// SIGMOIDE ESTÁVEL
// ----------------------------------------------------------------------------

float sigmoid(float x) {

    if (x < -20.0f)
        return 0.0f;

    if (x > 20.0f)
        return 1.0f;

    return 1.0f / (1.0f + exp(-x));
}


// ----------------------------------------------------------------------------
// LEITURA DO CORPUS
// ----------------------------------------------------------------------------

string lerCorpus(const string& caminho) {

    ifstream arquivo(caminho);

    if (!arquivo.is_open()) {

        cerr << "ERRO: Nao conseguiu abrir '" << caminho << "'" << endl;

        return "";
    }

    string conteudo;
    string linha;

    while (getline(arquivo, linha)) {

        conteudo += linha;
        conteudo += " ";
    }

    arquivo.close();

    return conteudo;
}


// ============================================================================
// VOCABULÁRIO
// ============================================================================

class Vocabulario {

public:

    unordered_map<string, int> palavraParaId;
    vector<string> idParaPalavra;
    vector<int> frequencias;

    int tamanho = 0;


    void construir(const vector<string>& tokens) {

        unordered_map<string, int> freq;

        for (const string& token : tokens) {
            freq[token]++;
        }


        vector<pair<int, string>> ordenado;

        for (const auto& [palavra, frequencia] : freq) {

            ordenado.push_back({
                frequencia,
                palavra
            });
        }


        sort(
            ordenado.begin(),
            ordenado.end(),
            [](const auto& a, const auto& b) {

                if (a.first != b.first)
                    return a.first > b.first;

                return a.second < b.second;
            }
        );


        for (const auto& [frequencia, palavra] : ordenado) {

            palavraParaId[palavra] = tamanho;

            idParaPalavra.push_back(palavra);

            frequencias.push_back(frequencia);

            tamanho++;
        }
    }


    int id(const string& palavra) const {

        auto it = palavraParaId.find(palavra);

        if (it != palavraParaId.end())
            return it->second;

        return -1;
    }


    string palavra(int id) const {

        if (id >= 0 && id < (int)idParaPalavra.size())
            return idParaPalavra[id];

        return "<OOV>";
    }
};


// ============================================================================
// EMBEDDING TABLE
// ============================================================================

class EmbeddingTable {

public:

    int vocabSize;
    int dimensao;

    vector<vector<float>> vetores;


    EmbeddingTable(
        int vocab = 0,
        int dim = 0,
        unsigned int semente = 42
    ) {

        vocabSize = vocab;
        dimensao = dim;

        if (vocab <= 0 || dim <= 0)
            return;


        mt19937 gerador(semente);

        uniform_real_distribution<float> distribuicao(-1.0f, 1.0f);

        vetores.resize(
            vocabSize,
            vector<float>(dimensao)
        );


        float escala = 0.5f / sqrt(static_cast<float>(dimensao));


        for (int i = 0; i < vocabSize; i++) {

            for (int j = 0; j < dimensao; j++) {

                vetores[i][j] =
                    distribuicao(gerador) * escala;
            }
        }
    }


    const vector<float>& get(int palavraId) const {

        return vetores[palavraId];
    }


    vector<float>& getMutavel(int palavraId) {

        return vetores[palavraId];
    }


    void update(
        int palavraId,
        const vector<float>& gradiente,
        float taxa
    ) {

        for (int j = 0; j < dimensao; j++) {

            vetores[palavraId][j] -=
                taxa * gradiente[j];
        }
    }


    void salvar(
        const string& caminho,
        const Vocabulario& vocab
    ) const {

        json j;

        j["vocabSize"] = vocabSize;
        j["dimensao"] = dimensao;


        json embeddingsArray = json::array();


        for (int i = 0; i < vocabSize; i++) {

            json item;

            item["palavra"] = vocab.palavra(i);
            item["vetor"] = vetores[i];

            embeddingsArray.push_back(item);
        }


        j["embeddings"] = embeddingsArray;


        ofstream arquivo(caminho);

        if (!arquivo.is_open()) {

            cerr << "ERRO: nao conseguiu salvar em '"
                 << caminho << "'" << endl;

            return;
        }


        arquivo << j.dump(2);

        arquivo.close();


        cout << "  Embeddings salvos em: "
             << caminho << endl;
    }
};


// ============================================================================
// WORD2VEC
// ============================================================================

class Word2Vec {

public:

    EmbeddingTable* embeddings;

    vector<vector<float>> pesosSaida;

    int dimensao;
    int vocabSize;

    float taxa;

    int negativos;

    vector<float> tabelaUnigram;

    mt19937 gerador;


    Word2Vec(
        int vocab,
        int dim,
        const vector<int>& frequencias,
        int k = 5,
        unsigned int semente = 42
    )
        : gerador(semente)
    {

        vocabSize = vocab;
        dimensao = dim;
        negativos = k;

        taxa = 0.025f;


        embeddings =
            new EmbeddingTable(
                vocab,
                dim,
                semente
            );


        pesosSaida.resize(
            vocabSize,
            vector<float>(dimensao)
        );


        uniform_real_distribution<float> distribuicao(-1.0f, 1.0f);

        float escala = 0.5f / sqrt(static_cast<float>(dimensao));


        for (int i = 0; i < vocabSize; i++) {

            for (int j = 0; j < dimensao; j++) {

                pesosSaida[i][j] =
                    distribuicao(gerador) * escala;
            }
        }


        construirTabelaUnigram(frequencias);
    }


    ~Word2Vec() {

        delete embeddings;
    }


    // ------------------------------------------------------------------------
    // TABELA UNIGRAM
    // ------------------------------------------------------------------------

    void construirTabelaUnigram(
        const vector<int>& frequencias
    ) {

        float soma = 0.0f;


        for (int f : frequencias) {

            soma += pow(static_cast<float>(f), 0.75f);
        }


        tabelaUnigram.resize(vocabSize);


        float acumulado = 0.0f;


        for (int i = 0; i < vocabSize; i++) {

            acumulado +=
                pow(static_cast<float>(frequencias[i]), 0.75f)
                / soma;

            tabelaUnigram[i] = acumulado;
        }


        tabelaUnigram.back() = 1.0f;
    }


    // ------------------------------------------------------------------------
    // AMOSTRAGEM NEGATIVA
    // ------------------------------------------------------------------------

    int amostrarNegativo(int contexto) {

        uniform_real_distribution<float> distribuicao(0.0f, 1.0f);

        while (true) {

            float r = distribuicao(gerador);

            auto it = lower_bound(
                tabelaUnigram.begin(),
                tabelaUnigram.end(),
                r
            );


            int idx = distance(
                tabelaUnigram.begin(),
                it
            );


            if (idx >= vocabSize)
                idx = vocabSize - 1;


            if (idx != contexto)
                return idx;
        }
    }


    // ------------------------------------------------------------------------
    // TREINAMENTO DE UM PAR
    // ------------------------------------------------------------------------

    float treinar(int central, int contexto) {

        const vector<float> h =
            embeddings->get(central);


        vector<float> gradEntrada(
            dimensao,
            0.0f
        );


        float perda = 0.0f;


        // ================================================================
        // POSITIVO
        // ================================================================

        float dot = 0.0f;


        for (int j = 0; j < dimensao; j++) {

            dot += pesosSaida[contexto][j] * h[j];
        }


        float sig = sigmoid(dot);


        perda -= log(max(sig, EPSILON));


        // Derivada da BCE:
        // sigmoid(dot) - 1
        float erro = sig - 1.0f;


        vector<float> gradSaidaPositivo(dimensao);


        for (int j = 0; j < dimensao; j++) {

            // Guardamos os pesos antigos para calcular
            // corretamente o gradiente da entrada.

            gradSaidaPositivo[j] =
                erro * h[j];

            gradEntrada[j] +=
                erro * pesosSaida[contexto][j];
        }


        // Atualiza pesos de saída positivos

        for (int j = 0; j < dimensao; j++) {

            pesosSaida[contexto][j] -=
                taxa * gradSaidaPositivo[j];
        }


        // ================================================================
        // NEGATIVOS
        // ================================================================

        for (int n = 0; n < negativos; n++) {

            int neg = amostrarNegativo(contexto);


            dot = 0.0f;


            for (int j = 0; j < dimensao; j++) {

                dot += pesosSaida[neg][j] * h[j];
            }


            sig = sigmoid(dot);


            perda -= log(max(1.0f - sig, EPSILON));


            // Para classe negativa:
            // derivada = sigmoid(dot)

            erro = sig;


            vector<float> gradSaidaNegativo(dimensao);


            for (int j = 0; j < dimensao; j++) {

                gradSaidaNegativo[j] =
                    erro * h[j];


                gradEntrada[j] +=
                    erro * pesosSaida[neg][j];
            }


            // Atualiza pesos negativos

            for (int j = 0; j < dimensao; j++) {

                pesosSaida[neg][j] -=
                    taxa * gradSaidaNegativo[j];
            }
        }


        // ================================================================
        // ATUALIZA EMBEDDING CENTRAL
        // ================================================================

        embeddings->update(
            central,
            gradEntrada,
            taxa
        );


        return perda;
    }


    // ------------------------------------------------------------------------
    // SIMILARES
    // ------------------------------------------------------------------------

    void topSimilares(
        const string& alvo,
        const Vocabulario& vocab,
        int n = 5
    ) {

        int idAlvo = vocab.id(alvo);


        if (idAlvo == -1) {

            cout << "  \"" << alvo
                 << "\" nao esta no vocabulario."
                 << endl;

            return;
        }


        vector<float> embAlvo =
            embeddings->get(idAlvo);


        vector<pair<float, string>> sims;


        for (int i = 0; i < vocabSize; i++) {

            if (i == idAlvo)
                continue;


            float sim =
                cosseno(
                    embAlvo,
                    embeddings->get(i)
                );


            sims.push_back({
                sim,
                vocab.palavra(i)
            });
        }


        sort(
            sims.rbegin(),
            sims.rend()
        );


        cout << "  Top " << n
             << " mais similares a \""
             << alvo << "\":"
             << endl;


        for (int i = 0; i < min(n, (int)sims.size()); i++) {

            cout << "    "
                 << setw(15)
                 << left
                 << sims[i].second

                 << " sim="
                 << fixed
                 << setprecision(4)
                 << sims[i].first
                 << endl;
        }
    }
};


// ============================================================================
// GERAÇÃO DE PARES
// ============================================================================

vector<pair<int, int>> gerarPares(
    const vector<string>& tokens,
    const Vocabulario& vocab,
    int janela = 2
) {

    vector<pair<int, int>> pares;

    int n = static_cast<int>(tokens.size());


    for (int i = 0; i < n; i++) {

        int central =
            vocab.id(tokens[i]);


        if (central == -1)
            continue;


        for (
            int offset = -janela;
            offset <= janela;
            offset++
        ) {

            if (offset == 0)
                continue;


            int j = i + offset;


            if (j < 0 || j >= n)
                continue;


            int contexto =
                vocab.id(tokens[j]);


            if (contexto == -1)
                continue;


            pares.push_back({
                central,
                contexto
            });
        }
    }


    return pares;
}


// ============================================================================
// SUBSAMPLING
// ============================================================================

vector<string> aplicarSubsampling(
    const vector<string>& tokens,
    float t = 1e-3f,
    unsigned int semente = 42
) {

    unordered_map<string, int> freq;


    for (const string& token : tokens) {
        freq[token]++;
    }


    int total = static_cast<int>(tokens.size());


    vector<string> resultado;

    resultado.reserve(tokens.size());


    mt19937 gerador(semente);

    uniform_real_distribution<float> distribuicao(0.0f, 1.0f);


    for (const string& token : tokens) {

        float f =
            static_cast<float>(freq[token])
            / static_cast<float>(total);


        float probRemover =
            1.0f - sqrt(t / f);


        if (probRemover < 0.0f)
            probRemover = 0.0f;


        float r = distribuicao(gerador);


        if (r > probRemover) {

            resultado.push_back(token);
        }
    }


    cout << "  Subsampling: "
         << tokens.size()
         << " -> "
         << resultado.size()
         << " tokens"
         << endl;


    return resultado;
}


// ============================================================================
// MAIN
// ============================================================================

int main() {

    cout << "============================================================"
         << endl;

    cout << "  NATACHA - Word2Vec Skip-Gram Negative Sampling"
         << endl;

    cout << "============================================================"
         << endl;


    // Caminho relativo a partir da pasta build/src/embeddings
    string caminhoCorpus =
        "../../dados/embeddings/corpus.txt";


    string corpus =
        lerCorpus(caminhoCorpus);


    if (corpus.empty())
        return 1;


    cout << endl
         << "Corpus: "
         << corpus.size()
         << " caracteres";


    vector<string> tokens =
        tokenizar(corpus);


    cout << ", "
         << tokens.size()
         << " tokens"
         << endl;


    // ================================================================
    // SUBSAMPLING
    // ================================================================

    tokens =
        aplicarSubsampling(
            tokens,
            1e-3f,
            42
        );


    if (tokens.size() < 20) {

        cerr << "ERRO: Corpus muito pequeno apos subsampling."
             << endl;

        return 1;
    }


    // ================================================================
    // VOCABULARIO
    // ================================================================

    Vocabulario vocab;

    vocab.construir(tokens);


    cout << "Vocabulario: "
         << vocab.tamanho
         << " palavras"
         << endl;


    // ================================================================
    // CONFIGURAÇÕES DO MODELO
    // ================================================================

    int dim = 64;
    int janela = 5;
    int epocas = 500;
    int negativos = 5;


    cout << endl
         << "Configuracao:"
         << endl;

    cout << "  Dimensao: " << dim << endl;
    cout << "  Janela: " << janela << endl;
    cout << "  Epocas: " << epocas << endl;
    cout << "  Negativos: " << negativos << endl;


    // ================================================================
    // GERA PARES
    // ================================================================

    auto pares =
        gerarPares(
            tokens,
            vocab,
            janela
        );


    if (pares.empty()) {

        cerr << "ERRO: Nenhum par de treinamento gerado."
             << endl;

        return 1;
    }


    cout << "Pares por epoca: "
         << pares.size()
         << endl;


    cout << "Operacoes estimadas: "
         << static_cast<long long>(pares.size())
            * (1 + negativos)
            * dim
            * 4
         << endl;


    // ================================================================
    // MODELO
    // ================================================================

    Word2Vec modelo(
        vocab.tamanho,
        dim,
        vocab.frequencias,
        negativos,
        42
    );


    // ================================================================
    // TREINAMENTO
    // ================================================================

    float alphaInicial = 0.025f;
    float alphaMinimo = 0.0001f;


    float melhorPerda = numeric_limits<float>::max();

    int semMelhora = 0;

    int paciencia = 25;


    mt19937 embaralhador(42);


    auto inicio =
        chrono::high_resolution_clock::now();


    cout << endl
         << "--- INICIANDO TREINAMENTO ---"
         << endl;


    for (int epoca = 0; epoca < epocas; epoca++) {

        float perdaTotal = 0.0f;


        // ============================================================
        // DECAY DA TAXA DE APRENDIZADO
        // ============================================================

        float progresso =
            static_cast<float>(epoca)
            / static_cast<float>(epocas);


        float alpha =
            alphaInicial
            * (1.0f - progresso);


        if (alpha < alphaMinimo)
            alpha = alphaMinimo;


        modelo.taxa = alpha;


        // ============================================================
        // EMBARALHA PARES
        // ============================================================

        shuffle(
            pares.begin(),
            pares.end(),
            embaralhador
        );


        // ============================================================
        // TREINA
        // ============================================================

        for (const auto& par : pares) {

            perdaTotal +=
                modelo.treinar(
                    par.first,
                    par.second
                );
        }


        float perdaMedia =
            perdaTotal
            / static_cast<float>(pares.size());


        // ============================================================
        // TEMPO
        // ============================================================

        auto agora =
            chrono::high_resolution_clock::now();


        float tempo =
            chrono::duration<float>(
                agora - inicio
            ).count();


        // ============================================================
        // LOG
        // ============================================================

        if (
            epoca == 0 ||
            (epoca + 1) % 25 == 0 ||
            epoca == epocas - 1
        ) {

            cout << "  Epoca "
                 << setw(4)
                 << epoca + 1
                 << "/"
                 << epocas

                 << " | Perda: "
                 << fixed
                 << setprecision(4)
                 << perdaMedia

                 << " | Alpha: "
                 << setprecision(6)
                 << alpha

                 << " | Tempo: "
                 << setprecision(1)
                 << tempo
                 << "s"

                 << endl;
        }


        // ============================================================
        // EARLY STOPPING
        // ============================================================

        if (perdaMedia < melhorPerda - 0.00001f) {

            melhorPerda = perdaMedia;
            semMelhora = 0;

        } else {

            semMelhora++;
        }


        if (semMelhora >= paciencia) {

            cout << endl
                 << "Early stopping na epoca "
                 << epoca + 1
                 << endl;

            break;
        }
    }


    auto fim =
        chrono::high_resolution_clock::now();


    float tempoTotal =
        chrono::duration<float>(
            fim - inicio
        ).count();


    cout << endl
         << "============================================================"
         << endl;


    cout << "Treinamento concluido!"
         << endl;


    cout << "Tempo total: "
         << fixed
         << setprecision(1)
         << tempoTotal
         << " segundos"
         << endl;


    cout << "Melhor perda: "
         << fixed
         << setprecision(6)
         << melhorPerda
         << endl;


    // ================================================================
    // SALVAMENTO
    // ================================================================

    cout << endl
         << "--- SALVANDO EMBEDDINGS ---"
         << endl;


    modelo.embeddings->salvar(
        "../../dados/embeddings/natacha_embeddings.json",
        vocab
    );


    // ================================================================
    // TESTES DE SIMILARIDADE
    // ================================================================

    cout << endl
         << "============================================================"
         << endl;

    cout << "             TESTES DE SIMILARIDADE"
         << endl;

    cout << "============================================================"
         << endl;


    auto testar = [&](const string& a, const string& b) {

        int idA = vocab.id(a);
        int idB = vocab.id(b);


        if (idA == -1 || idB == -1) {

            cout << "  "
                 << a
                 << " <-> "
                 << b
                 << " = palavra nao encontrada"
                 << endl;

            return;
        }


        float sim =
            cosseno(
                modelo.embeddings->get(idA),
                modelo.embeddings->get(idB)
            );


        cout << "  "
             << setw(12)
             << left
             << a

             << " <-> "

             << setw(12)
             << left
             << b

             << " = "

             << fixed
             << setprecision(4)
             << sim
             << endl;
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


    // ================================================================
    // TOP SIMILARES
    // ================================================================

    cout << endl
         << "============================================================"
         << endl;

    cout << "                TOP SIMILARES"
         << endl;

    cout << "============================================================"
         << endl;


    modelo.topSimilares("natacha", vocab, 10);

    cout << endl;

    modelo.topSimilares("felix", vocab, 10);

    cout << endl;

    modelo.topSimilares("cpu", vocab, 10);

    cout << endl;

    modelo.topSimilares("cafe", vocab, 10);

    cout << endl;

    modelo.topSimilares("aprende", vocab, 10);


    cout << endl
         << "============================================================"
         << endl;

    cout << "Natacha treinou embeddings Word2Vec."
         << endl;

    cout << "Vocabulario: "
         << vocab.tamanho
         << " palavras"
         << endl;

    cout << "Dimensao: "
         << dim
         << endl;

    cout << "============================================================"
         << endl;


    return 0;
}