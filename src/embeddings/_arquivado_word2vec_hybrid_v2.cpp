// word2vec_hybrid_v2.cpp — Híbrido FastText + Skip-gram com correções
// Compilar: clang++ -O3 -std=c++17 -o word2vec_hybrid word2vec_hybrid_v2.cpp
// Rodar:    ./word2vec_hybrid --arquivo ../../dados/embeddings/corpus.txt \
//                              --pre-treinado ../../dados/embeddings/cc.pt.300.vec \
//                              --json ../../dados/embeddings/natacha_embeddings.json \
//                              --epocas 50 --decay

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <sstream>

using namespace std;

struct ParTreino { int alvo; int contexto; };

class Word2VecHybrid {
private:
    int dimensao;
    int negativos;
    float taxaBase;
    float taxaMinima;
    float taxaAtual;
    mt19937& rng;
    vector<vector<float>> W1;
    vector<vector<float>> W2;
    vector<bool> isPretrained;

    // Distribuicao para negative sampling (freq^0.75)
    vector<float> negCum;  // cumulative distribution
    float negTotal;

    // Adam
    vector<vector<float>> mW1, vW1, mW2, vW2;
    float beta1, beta2, eps;
    int passo;

    float sigmoid(float x) {
        if (x > 6.0f) return 1.0f;
        if (x < -6.0f) return 0.0f;
        return 1.0f / (1.0f + expf(-x));
    }

public:
    Word2VecHybrid(int vocabTam, int dim, int neg, float taxa, mt19937& random)
        : dimensao(dim), negativos(neg), taxaBase(taxa), taxaMinima(0.0001f),
          taxaAtual(taxa), rng(random), passo(0), negTotal(0.0f),
          beta1(0.9f), beta2(0.999f), eps(1e-8f) {

        // [FIX 2] Escala menor pra dims pequenas
        float escala = 0.5f / (float)dimensao;

        W1.assign(vocabTam, vector<float>(dimensao));
        W2.assign(vocabTam, vector<float>(dimensao));
        isPretrained.assign(vocabTam, false);

        for (auto& v : W1)
            for (auto& x : v)
                x = ((float)rng() / (float)rng.max() * 2.0f - 1.0f) * escala;
        for (auto& v : W2)
            for (auto& x : v)
                x = ((float)rng() / (float)rng.max() * 2.0f - 1.0f) * escala;

        mW1.assign(vocabTam, vector<float>(dimensao, 0.0f));
        vW1.assign(vocabTam, vector<float>(dimensao, 0.0f));
        mW2.assign(vocabTam, vector<float>(dimensao, 0.0f));
        vW2.assign(vocabTam, vector<float>(dimensao, 0.0f));
    }

    // [FIX 6] Constroi tabela de negativos baseada em freq^0.75
    void construirTabelaNegativos(const unordered_map<string,int>& vocab,
                                   const unordered_map<string,int>& freq,
                                   const vector<string>& idxToWord) {
        negCum.assign(vocab.size(), 0.0f);
        float soma = 0.0f;
        for (size_t i = 0; i < idxToWord.size(); i++) {
            int f = 1;
            auto it = freq.find(idxToWord[i]);
            if (it != freq.end()) f = it->second;
            float peso = powf((float)f, 0.75f);
            soma += peso;
            negCum[i] = soma;
        }
        negTotal = soma;
        // Normaliza cumulativa
        for (auto& x : negCum) x /= negTotal;
        negTotal = 1.0f;
    }

    int amostrarNegativo() {
        float r = (float)rng() / (float)rng.max();
        // binary search
        int lo = 0, hi = (int)negCum.size() - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (negCum[mid] < r) lo = mid + 1;
            else hi = mid;
        }
        return lo;
    }

    // [FIX 5] Copia W1 pra W2 apos carregar pretrained
    void carregarPreTreinados(const string& caminho, const unordered_map<string, int>& vocab) {
        ifstream file(caminho);
        if (!file.is_open()) {
            cout << "  \033[33mAviso: " << caminho << " nao encontrado. Treinando do zero.\033[0m" << endl;
            return;
        }

        string linha;
        int carregados = 0;
        int numPalavras = 0, dims = 0;

        if (getline(file, linha)) {
            stringstream ss(linha);
            ss >> numPalavras >> dims;
            cout << "  fastText: " << numPalavras << " palavras, " << dims << " dims" << endl;
        }

        int dimsLer = min(dimensao, dims);
        while (getline(file, linha)) {
            // Pega so a primeira palavra antes de fazer parse completo
            size_t espaco = linha.find(' ');
            if (espaco == string::npos) continue;
            string palavra = linha.substr(0, espaco);

            auto it = vocab.find(palavra);
            if (it != vocab.end()) {
                int idx = it->second;
                stringstream ss(linha.substr(espaco + 1));
                for (int d = 0; d < dimsLer; d++) {
                    ss >> W1[idx][d];
                    W2[idx][d] = W1[idx][d];  // [FIX 5] espelha em W2
                }
                isPretrained[idx] = true;
                carregados++;
            }
        }
        file.close();
        cout << "  \033[32m✓ " << carregados << " vetores pre-treinados carregados\033[0m" << endl;
    }

    void setTaxa(float novaTaxa) { taxaAtual = novaTaxa; }

    float dot(const vector<float>& a, const vector<float>& b) {
        float s = 0;
        for (size_t i = 0; i < a.size(); i++) s += a[i] * b[i];
        return s;
    }

    vector<float> pegarVetor(int idx) {
        vector<float> v(dimensao);
        for (int d = 0; d < dimensao; d++)
            v[d] = W1[idx][d] + W2[idx][d];
        return v;
    }

    float norma(const vector<float>& v) {
        float soma = 0;
        for (float x : v) soma += x * x;
        return sqrt(soma);
    }

    void adamUpdate(vector<float>& peso, vector<float>& momento, vector<float>& vel,
                    float grad, int d) {
        int t = max(1, passo);
        momento[d] = beta1 * momento[d] + (1.0f - beta1) * grad;
        vel[d] = beta2 * vel[d] + (1.0f - beta2) * grad * grad;
        float mCorr = momento[d] / (1.0f - powf(beta1, t));
        float vCorr = vel[d] / (1.0f - powf(beta2, t));
        peso[d] -= taxaAtual * mCorr / (sqrtf(vCorr) + eps);
    }

    float treinarPar(int idxAlvo, int idxCtx, bool congelar) {
        passo++;

        vector<float>& h = W1[idxAlvo];
        float perda = 0.0f;
        vector<float> gradIn(dimensao, 0.0f);

        // Positiva
        float dotPos = dot(W2[idxCtx], h);
        float sigPos = sigmoid(dotPos);
        float gradPos = sigPos - 1.0f;
        perda -= logf(sigPos + 1e-9f);

        vector<float> w2CtxOriginal = W2[idxCtx];
        for (int d = 0; d < dimensao; d++)
            gradIn[d] += gradPos * w2CtxOriginal[d];

        // Negativas — [FIX 6] amostragem por frequencia
        vector<vector<float>> w2NegOriginal;
        vector<int> indicesNegativos;
        for (int n = 0; n < negativos; n++) {
            int idxNeg;
            do { idxNeg = amostrarNegativo(); }
            while (idxNeg == idxCtx || idxNeg == idxAlvo);

            indicesNegativos.push_back(idxNeg);
            float dotNeg = dot(W2[idxNeg], h);
            float sigNeg = sigmoid(dotNeg);
            float gradNeg = sigNeg;
            perda -= logf(1.0f - sigNeg + 1e-9f);

            w2NegOriginal.push_back(W2[idxNeg]);
            for (int d = 0; d < dimensao; d++)
                gradIn[d] += gradNeg * w2NegOriginal[n][d];
        }

        // Atualiza W2 positivo
        for (int d = 0; d < dimensao; d++)
            adamUpdate(W2[idxCtx], mW2[idxCtx], vW2[idxCtx], gradPos * h[d], d);

        // Atualiza W2 negativos
        for (int n = 0; n < negativos; n++) {
            int idxNeg = indicesNegativos[n];
            float sigNeg = sigmoid(dot(w2NegOriginal[n], h));
            float gradNeg = sigNeg;
            for (int d = 0; d < dimensao; d++)
                adamUpdate(W2[idxNeg], mW2[idxNeg], vW2[idxNeg], gradNeg * h[d], d);
        }

        // W1 (congelar so se for pretrained E congelar=true)
        if (!congelar || !isPretrained[idxAlvo]) {
            for (int d = 0; d < dimensao; d++)
                adamUpdate(h, mW1[idxAlvo], vW1[idxAlvo], gradIn[d], d);
        }

        return perda;
    }

    void treinar(const vector<ParTreino>& pares, int epocas, bool usarDecay, bool congelar) {
        int totalPares = pares.size();
        auto inicio = chrono::high_resolution_clock::now();

        for (int e = 0; e < epocas; e++) {
            if (usarDecay)
                taxaAtual = max(taxaMinima, taxaBase * (1.0f - (float)e / epocas));

            float perdaEpoca = 0;
            for (int p = 0; p < totalPares; p++)
                perdaEpoca += treinarPar(pares[p].alvo, pares[p].contexto, congelar);
            perdaEpoca /= totalPares;

            if ((e + 1) % 5 == 0 || e == epocas - 1) {
                auto agora = chrono::high_resolution_clock::now();
                float segundos = chrono::duration<float>(agora - inicio).count();
                cout << "  Epoca " << setw(4) << (e+1) << "/" << epocas
                     << " | Perda: " << fixed << setprecision(4) << perdaEpoca
                     << " | Alpha: " << scientific << setprecision(6) << taxaAtual
                     << " | Tempo: " << fixed << setprecision(1) << segundos << "s" << endl;
            }
        }

        auto fim = chrono::high_resolution_clock::now();
        float segundos = chrono::duration<float>(fim - inicio).count();
        cout << "\n\033[32m✓\033[0m Treino concluido em " << fixed << setprecision(1)
             << segundos << "s" << endl;
    }

    float similaridade(const vector<float>& a, const vector<float>& b) {
        float na = norma(a), nb = norma(b);
        if (na < 1e-8f || nb < 1e-8f) return 0;
        return dot(a, b) / (na * nb);
    }

    // [FIX 7] JSON no formato do word2vec_neg
    void salvarJSON(const string& caminho, const vector<string>& idxToWord) {
        ofstream out(caminho);
        if (!out.is_open()) {
            cerr << "Erro ao abrir " << caminho << endl;
            return;
        }

        out << "{\n";
        out << "  \"dimensao\": " << dimensao << ",\n";
        out << "  \"embeddings\": [\n";

        for (size_t i = 0; i < idxToWord.size(); i++) {
            vector<float> v = pegarVetor((int)i);
            out << "    {\"palavra\": \"" << idxToWord[i] << "\", \"vetor\": [";
            for (int d = 0; d < dimensao; d++) {
                out << fixed << setprecision(6) << v[d];
                if (d + 1 < dimensao) out << ", ";
            }
            out << "]}";
            if (i + 1 < idxToWord.size()) out << ",";
            out << "\n";
        }

        out << "  ],\n";
        out << "  \"vocabSize\": " << idxToWord.size() << "\n";
        out << "}\n";
        out.close();
        cout << "\033[32m✓\033[0m JSON salvo em " << caminho << endl;
    }

    void testarPar(const string& w1, const string& w2,
                   const unordered_map<string, int>& vocab) {
        if (vocab.find(w1) == vocab.end() || vocab.find(w2) == vocab.end()) {
            cout << "  ? " << w1 << " <-> " << w2 << " (ausente)" << endl;
            return;
        }
        float sim = similaridade(pegarVetor(vocab.at(w1)), pegarVetor(vocab.at(w2)));
        cout << "  " << w1 << " <-> " << w2 << " = " << fixed << setprecision(4) << sim << endl;
    }
};

vector<string> tokenizar(const string& texto) {
    vector<string> tokens;
    string atual;
    for (size_t i = 0; i < texto.size(); i++) {
        unsigned char c = texto[i];
        if (c < 0x80) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                c == '+' || c == '#' || (c >= '0' && c <= '9')) {
                atual += tolower(c);
            } else {
                if (!atual.empty() && atual.size() > 1) tokens.push_back(atual);
                atual.clear();
            }
        }
    }
    if (!atual.empty() && atual.size() > 1) tokens.push_back(atual);
    return tokens;
}

int main(int argc, char* argv[]) {
    string arquivoCorpus = "../../dados/embeddings/corpus.txt";
    string arquivoPreTreinado = "../../dados/embeddings/cc.pt.300.vec";
    string arquivoSaidaJSON = "../../dados/embeddings/natacha_embeddings.json";

    // [FIX 2/3/4] Defaults ajustados
    int epocas = 50;
    int dimensao = 32;
    int janela = 5;
    float taxaInicial = 0.025f;
    int negativos = 5;
    bool usarDecay = true;  // [FIX 3] decay default ON
    bool congelar = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--arquivo" && i+1 < argc) arquivoCorpus = argv[++i];
        else if (arg == "--pre-treinado" && i+1 < argc) arquivoPreTreinado = argv[++i];
        else if (arg == "--json" && i+1 < argc) arquivoSaidaJSON = argv[++i];
        else if (arg == "--epocas" && i+1 < argc) epocas = stoi(argv[++i]);
        else if (arg == "--dim" && i+1 < argc) dimensao = stoi(argv[++i]);
        else if (arg == "--janela" && i+1 < argc) janela = stoi(argv[++i]);
        else if (arg == "--taxa" && i+1 < argc) taxaInicial = stof(argv[++i]);
        else if (arg == "--neg" && i+1 < argc) negativos = stoi(argv[++i]);
        else if (arg == "--no-decay") usarDecay = false;
        else if (arg == "--freeze") congelar = true;
        else if (arg == "--ajuda" || arg == "-h") {
            cout << "Uso: ./word2vec_hybrid [opcoes]" << endl;
            cout << "  --arquivo <caminho>        Corpus (default: ../../dados/embeddings/corpus.txt)" << endl;
            cout << "  --pre-treinado <caminho>   Vetor fastText (default: ../../dados/embeddings/cc.pt.300.vec)" << endl;
            cout << "  --json <caminho>           Saida JSON" << endl;
            cout << "  --epocas <N>               Epocas (default: 50)" << endl;
            cout << "  --dim <N>                  Dimensao (default: 32)" << endl;
            cout << "  --janela <N>               Janela (default: 5)" << endl;
            cout << "  --taxa <F>                 Taxa base (default: 0.025)" << endl;
            cout << "  --neg <N>                  Negativos (default: 5)" << endl;
            cout << "  --no-decay                 Desliga decay da taxa" << endl;
            cout << "  --freeze                   Congela palavras do fastText" << endl;
            return 0;
        }
    }

    cout << "============================================================\n";
    cout << "  NATACHA - Word2Vec Hibrido (FastText + Skip-gram v2)\n";
    cout << "============================================================\n\n";

    ifstream arquivo(arquivoCorpus);
    if (!arquivo.is_open()) {
        cerr << "Erro: nao encontrei " << arquivoCorpus << endl;
        return 1;
    }
    string linha, texto;
    while (getline(arquivo, linha)) texto += linha + " ";
    arquivo.close();

    vector<string> tokens = tokenizar(texto);
    unordered_map<string, int> freq;
    for (auto& t : tokens) freq[t]++;

    // [FIX 1] Sem freqMaxima
    int freqMinima = 3;

    unordered_map<string, int> vocab;
    vector<string> idxToWord;
    for (auto& [palavra, f] : freq) {
        if (f >= freqMinima) {
            vocab[palavra] = (int)idxToWord.size();
            idxToWord.push_back(palavra);
        }
    }

    cout << "Corpus: " << texto.size() << " caracteres, " << tokens.size() << " tokens\n";
    cout << "Vocabulario: " << vocab.size() << " palavras\n\n";

    mt19937 rng(42);

    vector<ParTreino> pares;
    uniform_int_distribution<int> distJanela(1, janela);
    for (size_t i = 0; i < tokens.size(); i++) {
        if (vocab.find(tokens[i]) == vocab.end()) continue;
        int alvo = vocab[tokens[i]];
        int janelaAtual = distJanela(rng);
        for (int j = max(0, (int)i - janelaAtual);
             j <= min((int)tokens.size()-1, (int)i + janelaAtual); j++) {
            if (i == (size_t)j) continue;
            if (vocab.find(tokens[j]) == vocab.end()) continue;
            pares.push_back({alvo, vocab[tokens[j]]});
        }
    }

    cout << "Configuracao:\n";
    cout << "  Dimensao:    " << dimensao << "\n";
    cout << "  Janela:      " << janela << "\n";
    cout << "  Epocas:      " << epocas << "\n";
    cout << "  Negativos:   " << negativos << "\n";
    cout << "  Taxa base:   " << taxaInicial << "\n";
    cout << "  Decay:       " << (usarDecay ? "ON" : "OFF") << "\n";
    cout << "  Freeze:      " << (congelar ? "ON" : "OFF") << "\n";
    cout << "  Pares/epoca: " << pares.size() << "\n\n";

    Word2VecHybrid modelo((int)vocab.size(), dimensao, negativos, taxaInicial, rng);
    modelo.construirTabelaNegativos(vocab, freq, idxToWord);

    cout << "Lendo vetores pre-treinados...\n";
    modelo.carregarPreTreinados(arquivoPreTreinado, vocab);

    cout << "\n--- INICIANDO TREINAMENTO ---\n";
    modelo.treinar(pares, epocas, usarDecay, congelar);

    cout << "\n=== TESTES DE SIMILARIDADE ===\n";
    modelo.testarPar("felix", "gato", vocab);
    modelo.testarPar("felix", "iagato", vocab);
    modelo.testarPar("iagato", "gato", vocab);
    modelo.testarPar("felix", "padrinho", vocab);
    modelo.testarPar("felix", "natacha", vocab);
    modelo.testarPar("natacha", "rodrigo", vocab);
    modelo.testarPar("natacha", "casa", vocab);
    modelo.testarPar("cafe", "processamento", vocab);
    modelo.testarPar("c++", "codigo", vocab);
    modelo.testarPar("cozinha", "cpu", vocab);
    modelo.testarPar("mlp", "neuronio", vocab);

    modelo.salvarJSON(arquivoSaidaJSON, idxToWord);

    return 0;
}