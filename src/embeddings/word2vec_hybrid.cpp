// word2vec_hybrid.cpp — Word2Vec Negative Sampling Híbrido com fastText (C++17)
// Compilar: g++ -O3 -std=c++17 -o word2vec_hybrid word2vec_hybrid.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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
    vector<vector<float>> W1;  // embeddings de entrada
    vector<vector<float>> W2;  // pesos de saida
    vector<bool> isPretrained; // marca se a palavra veio do fastText

    // Adam optimizer
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
          taxaAtual(taxa), rng(random), passo(0),
          beta1(0.9f), beta2(0.999f), eps(1e-8f) {

        float escala = sqrt(1.0f / dimensao);

        W1.resize(vocabTam, vector<float>(dimensao));
        W2.resize(vocabTam, vector<float>(dimensao));
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

    void carregarPreTreinados(const string& caminho, const unordered_map<string, int>& vocab) {
        ifstream file(caminho);
        if (!file.is_open()) {
            cout << "  \033[33mAviso: arquivo pre-treinado " << caminho << " nao encontrado. Treinando do zero.\033[0m" << endl;
            return;
        }

        string linha;
        int carregados = 0;
        int numPalavras, dims;

        if (getline(file, linha)) {
            stringstream ss(linha);
            ss >> numPalavras >> dims;
        }

        while (getline(file, linha)) {
            stringstream ss(linha);
            string palavra;
            ss >> palavra;

            if (vocab.find(palavra) != vocab.end()) {
                int idx = vocab.at(palavra);
                for (int d = 0; d < min(dimensao, dims); d++) {
                    ss >> W1[idx][d];
                }
                isPretrained[idx] = true;
                carregados++;
            }
        }
        file.close();
        cout << "  \033[32m✓ Carregados " << carregados << " vetores pre-treinados do fastText!\033[0m" << endl;
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

    float treinarPar(int idxAlvo, int idxCtx, uniform_int_distribution<int>& distVocab, bool congelar) {
        passo++; // Exatamente 1 incremento por par de treino

        vector<float>& h = W1[idxAlvo];
        float perda = 0.0f;
        vector<float> gradIn(dimensao, 0.0f);

        // Positiva: Gradiente da perda = sigPos - 1.0f
        float dotPos = dot(W2[idxCtx], h);
        float sigPos = sigmoid(dotPos);
        float gradPos = sigPos - 1.0f; 
        perda -= logf(sigPos + 1e-9f);

        vector<float> w2CtxOriginal(W2[idxCtx].begin(), W2[idxCtx].end());

        for (int d = 0; d < dimensao; d++) {
            gradIn[d] += gradPos * w2CtxOriginal[d];
        }

        // Negativas
        vector<vector<float>> w2NegOriginal;
        vector<int> indicesNegativos;

        for (int n = 0; n < negativos; n++) {
            int idxNeg;
            do { idxNeg = distVocab(rng); } while (idxNeg == idxCtx || idxNeg == idxAlvo);
            indicesNegativos.push_back(idxNeg);

            float dotNeg = dot(W2[idxNeg], h);
            float sigNeg = sigmoid(dotNeg);
            float gradNeg = sigNeg; // Gradiente da perda no negativo
            perda -= logf(1.0f - sigNeg + 1e-9f);

            w2NegOriginal.push_back(W2[idxNeg]);

            for (int d = 0; d < dimensao; d++) {
                gradIn[d] += gradNeg * w2NegOriginal[n][d];
            }
        }

        // Atualizar W2 do contexto positivo
        for (int d = 0; d < dimensao; d++) {
            adamUpdate(W2[idxCtx], mW2[idxCtx], vW2[idxCtx], gradPos * h[d], d);
        }

        // Atualizar W2 dos negativos com os MESMOS índices do cálculo
        for (int n = 0; n < negativos; n++) {
            int idxNeg = indicesNegativos[n];
            float dotNeg = dot(w2NegOriginal[n], h);
            float sigNeg = sigmoid(dotNeg);
            float gradNeg = sigNeg;

            for (int d = 0; d < dimensao; d++) {
                adamUpdate(W2[idxNeg], mW2[idxNeg], vW2[idxNeg], gradNeg * h[d], d);
            }
        }

        // Atualizar W1 (somente se não estiver congelado ou se for palavra nova)
        if (!congelar || !isPretrained[idxAlvo]) {
            for (int d = 0; d < dimensao; d++) {
                adamUpdate(h, mW1[idxAlvo], vW1[idxAlvo], gradIn[d], d);
            }
        }

        return perda;
    }

    void treinar(const vector<ParTreino>& pares, int epocas, bool usarDecay, bool congelar) {
        int totalPares = pares.size();
        uniform_int_distribution<int> distVocab(0, W1.size() - 1);

        auto inicio = chrono::high_resolution_clock::now();
        vector<pair<int,float>> historico;

        for (int e = 0; e < epocas; e++) {
            if (usarDecay)
                taxaAtual = max(taxaMinima, taxaBase * (1.0f - (float)e / epocas));
            float perdaEpoca = 0;

            for (int p = 0; p < totalPares; p++)
                perdaEpoca += treinarPar(pares[p].alvo, pares[p].contexto, distVocab, congelar);

            perdaEpoca /= totalPares;

            if ((e + 1) % 10 == 0 || e == epocas - 1) {
                auto agora = chrono::high_resolution_clock::now();
                float segundos = chrono::duration<float>(agora - inicio).count();
                float paresPorSeg = (float)((e + 1) * totalPares) / max(segundos, 0.01f);

                int barras = 20;
                float progresso = (float)(e + 1) / epocas;
                int preenchido = (int)(progresso * barras);

                cout << "\r\33[2K  Epoca " << setw(5) << (e+1) << "/" << epocas << " ";
                for (int b = 0; b < barras; b++)
                    cout << (b < preenchido ? "\033[32m█\033[0m" : "\033[90m░\033[0m");
                cout << " Perda: " << fixed << setprecision(4) << perdaEpoca
                     << " | Taxa: " << scientific << setprecision(2) << taxaAtual
                     << " | " << (int)paresPorSeg << " pares/s\33[0m" << flush;

                historico.push_back({e+1, perdaEpoca});
            }
        }

        auto fim = chrono::high_resolution_clock::now();
        float segundos = chrono::duration<float>(fim - inicio).count();
        cout << "\n\033[32m✓\033[0m Treino concluido em " << fixed << setprecision(1)
             << segundos << "s" << endl;
    }

    float similaridade(const vector<float>& a, const vector<float>& b) {
        float na = norma(a);
        float nb = norma(b);
        if (na < 1e-8f || nb < 1e-8f) return 0;
        return dot(a, b) / (na * nb);
    }

    void testarPar(const string& w1, const string& w2,
                   const unordered_map<string, int>& vocab,
                   const string& descricao) {
        if (vocab.find(w1) == vocab.end() || vocab.find(w2) == vocab.end()) {
            cout << "  \033[33m?\033[0m " << w1 << " ou " << w2 << " nao encontrado" << endl;
            return;
        }
        vector<float> v1 = pegarVetor(vocab.at(w1));
        vector<float> v2 = pegarVetor(vocab.at(w2));
        float sim = similaridade(v1, v2);

        cout << "  " << w1 << " <-> " << w2 << ": " << fixed << setprecision(4) << sim
             << " (esperado: " << descricao << ")";

        if (sim > 0.5f) cout << " \033[32m\033[1m EXCELENTE\033[0m";
        else if (sim > 0.3f) cout << " \033[32m BOM\033[0m";
        else if (sim > 0.15f) cout << " \033[33m RAZOAVEL\033[0m";
        else if (sim > 0.0f) cout << " \033[31m FRACO\033[0m";
        else cout << " \033[31m\033[1m NEGATIVO\033[0m";

        int barras = 15;
        int preenchido = (int)(max(0.0f, min(1.0f, sim)) * barras);
        cout << " \033[90m";
        for (int i = 0; i < barras; i++)
            cout << (i < preenchido ? "\033[32m▪\033[0m\033[90m" : "·");
        cout << "\033[0m" << endl;
    }

    void salvarJSON(const string& caminho, const unordered_map<string, int>& vocab) {
        ofstream out(caminho);
        if (!out.is_open()) return;

        out << "{\n";
        size_t contador = 0;
        for (auto& [palavra, idx] : vocab) {
            vector<float> v = pegarVetor(idx);
            out << "  \"" << palavra << "\": [";
            for (size_t d = 0; d < v.size(); d++) {
                out << fixed << setprecision(6) << v[d];
                if (d + 1 < v.size()) out << ", ";
            }
            out << "]";
            if (++contador < vocab.size()) out << ",";
            out << "\n";
        }
        out << "}\n";
        out.close();
        cout << "\n\033[32m✓\033[0m Embeddings salvos com sucesso em: " << caminho << endl;
    }

    void relatorio(const unordered_map<string, int>& vocab,
                   const unordered_map<string, int>& frequencias) {

        cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
        cout << "║         RELATORIO DE SIMILARIDADES (HIBRIDO)        ║" << endl;
        cout << "╚═══════════════════════════════════════════════════════╝" << endl;

        cout << "\n\033[1mPares de teste:\033[0m" << endl;
        testarPar("cpu", "cozinha", vocab, "hardware e comodo");
        testarPar("felix", "teclado", vocab, "comportamento do felix");
        testarPar("natacha", "robo", vocab, "deve ser alto");
        testarPar("cozinha", "gato", vocab, "felix frequenta");
        testarPar("felix", "natacha", vocab, "moro junto");
        testarPar("cpu", "gpu", vocab, "hardware");

        cout << "\n\033[1mTop 5 mais similares a cada palavra-chave:\033[0m" << endl;
        vector<string> alvos = {"felix", "natacha", "cozinha", "robo", "cpu", "teclado"};
        for (const string& alvo : alvos) {
            if (vocab.find(alvo) == vocab.end()) continue;
            vector<float> vAlvo = pegarVetor(vocab.at(alvo));
            vector<pair<string,float>> sims;
            for (auto& [pal, idx] : vocab) {
                if (pal == alvo) continue;
                vector<float> v = pegarVetor(idx);
                sims.push_back({pal, similaridade(vAlvo, v)});
            }
            sort(sims.begin(), sims.end(), [](auto& a, auto& b){ return a.second > b.second; });

            cout << "\n  " << alvo << ":" << endl;
            for (int i = 0; i < min(5, (int)sims.size()); i++) {
                int barras = 20;
                int preenchido = (int)(max(0.0f, sims[i].second) * barras);
                cout << "    " << setw(12) << left << sims[i].first
                     << " " << fixed << setprecision(4) << sims[i].second
                     << " \033[90m";
                for (int b = 0; b < barras; b++)
                    cout << (b < preenchido ? "\033[32m▪\033[0m\033[90m" : "·");
                cout << "\033[0m" << endl;
            }
        }
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
    string arquivoCorpus = "dados/embeddings/corpus.txt";
    string arquivoPreTreinado = "dados/embeddings/cc.pt.300.vec";
    string arquivoSaidaJSON = "dados/embeddings/embeddings.json";
    int epocas = 50;
    int dimensao = 300; // ATUALIZADO: Default 300 para bater com o fastText
    int janela = 5;
    float taxaInicial = 0.0005f;
    int negativos = 2;
    bool usarDecay = false;
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
        else if (arg == "--decay") usarDecay = true;
        else if (arg == "--freeze") congelar = true;
        else if (arg == "--ajuda" || arg == "-h") {
            cout << "Uso: ./word2vec_hybrid [opcoes]" << endl;
            cout << "  --arquivo <caminho>        Corpus (default: dados/embeddings/corpus.txt)" << endl;
            cout << "  --pre-treinado <caminho>   Vetor fastText (default: dados/embeddings/cc.pt.300.vec)" << endl;
            cout << "  --epocas <N>               Epocas (default: 50)" << endl;
            cout << "  --dim <N>                  Dimensao (default: 300)" << endl;
            cout << "  --freeze                   Congela os vetores do fastText durante o treino" << endl;
            return 0;
        }
    }

    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║     WORD2VEC NEGATIVE SAMPLING (Híbrido)          ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝" << endl;
    cout << "  Arquivo:        " << arquivoCorpus << endl;
    cout << "  Pre-Treinado:   " << arquivoPreTreinado << endl;
    cout << "  Dimensao:       " << dimensao << endl;
    cout << "  Congelar FastText: " << (congelar ? "SIM" : "NAO") << endl;

    ifstream arquivo(arquivoCorpus);
    if (!arquivo.is_open()) {
        cerr << "\033[31mErro: nao encontrei " << arquivoCorpus << "\033[0m" << endl;
        return 1;
    }
    string linha, texto;
    while (getline(arquivo, linha)) texto += linha + " ";
    arquivo.close();

    vector<string> tokens = tokenizar(texto);
    unordered_map<string, int> freq;
    for (auto& t : tokens) freq[t]++;

    int freqMinima = 3;
    int freqMaxima = 1000;

    unordered_map<string, int> vocab;
    int idx = 0;
    for (auto& [palavra, f] : freq) {
        if (f >= freqMinima && f <= freqMaxima) {
            vocab[palavra] = idx++;
        }
    }

    mt19937 rng(42);

    vector<ParTreino> pares;
    uniform_int_distribution<int> distJanela(1, janela);
    for (size_t i = 0; i < tokens.size(); i++) {
        if (vocab.find(tokens[i]) == vocab.end()) continue;
        int alvo = vocab[tokens[i]];
        int janelaAtual = distJanela(rng);
        for (int j = max(0, (int)i - janelaAtual); j <= min((int)tokens.size()-1, (int)i + janelaAtual); j++) {
            if (i == (size_t)j) continue;
            if (vocab.find(tokens[j]) == vocab.end()) continue;
            pares.push_back({alvo, vocab[tokens[j]]});
        }
    }

    Word2VecHybrid modelo(vocab.size(), dimensao, negativos, taxaInicial, rng);
    
    cout << "\nLendo vetores pre-treinados..." << endl;
    modelo.carregarPreTreinados(arquivoPreTreinado, vocab);

    cout << "\n═══ INICIANDO TREINO HÍBRIDO ═══" << endl;
    modelo.treinar(pares, epocas, usarDecay, congelar);
    modelo.relatorio(vocab, freq);
    modelo.salvarJSON(arquivoSaidaJSON, vocab);

    return 0;
}
