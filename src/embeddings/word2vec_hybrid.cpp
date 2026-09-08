// word2vec_hybrid.cpp — Word2Vec com embeddings pré-treinados (fastText)
// Compilar: g++ -O3 -std=c++17 -o word2vec_hybrid word2vec_hybrid.cpp -lm
//
// Modos:
//   1. Treino do zero (sem pré-treinado)
//   2. Híbrido: carrega fastText + fine-tuna com corpus da Natacha
//
// Downloads:
//   fastText pt-BR: https://dl.fbaipublicfiles.com/fasttext/vectors-crawl/cc.pt.300.vec.gz
//   (descompactar: gunzip cc.pt.300.vec.gz)

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
    float taxaFineTune;  // Taxa menor para embeddings pré-treinados
    mt19937& rng;
    vector<vector<float>> W1;  // embeddings de entrada
    vector<vector<float>> W2;  // pesos de saida

    // Adam optimizer
    vector<vector<float>> mW1, vW1, mW2, vW2;
    float beta1, beta2, eps;
    int passo;

    // Mapeamento de palavras pré-treinadas
    unordered_map<string, vector<float>> pretrained;
    int pretrainedCount;
    int randomCount;

    float sigmoid(float x) {
        if (x > 6.0f) return 1.0f;
        if (x < -6.0f) return 0.0f;
        return 1.0f / (1.0f + expf(-x));
    }

public:
    Word2VecHybrid(int vocabTam, int dim, int neg, float taxa, mt19937& random)
        : dimensao(dim), negativos(neg), taxaBase(taxa), taxaMinima(0.0001f),
          taxaAtual(taxa), taxaFineTune(taxa * 0.1f),  // Fine-tune com taxa 10x menor
          rng(random), passo(0), pretrainedCount(0), randomCount(0),
          beta1(0.9f), beta2(0.999f), eps(1e-8f) {

        float escala = sqrt(1.0f / dimensao);

        W1.resize(vocabTam, vector<float>(dimensao));
        W2.resize(vocabTam, vector<float>(dimensao));

        // Inicialização aleatória (será substituída se houver pré-treinado)
        for (auto& v : W1)
            for (auto& x : v)
                x = ((float)rng() / (float)rng.max() * 2.0f - 1.0f) * escala;
        for (auto& v : W2)
            for (auto& x : v)
                x = ((float)rng() / (float)rng.max() * 2.0f - 1.0f) * escala;

        // Adam moments
        mW1.assign(vocabTam, vector<float>(dimensao, 0.0f));
        vW1.assign(vocabTam, vector<float>(dimensao, 0.0f));
        mW2.assign(vocabTam, vector<float>(dimensao, 0.0f));
        vW2.assign(vocabTam, vector<float>(dimensao, 0.0f));
    }

    // Carregar embeddings pré-treinados (fastText format)
    bool carregarPreTreinados(const string& arquivo,
                              const unordered_map<string, int>& vocab,
                              vector<bool>& isPretrained) {
        cout << "Carregando embeddings pre-treinados: " << arquivo << "..." << endl;

        ifstream file(arquivo);
        if (!file.is_open()) {
            cerr << "\033[31mErro: nao encontrei " << arquivo << "\033[0m" << endl;
            cerr << "Baixe o fastText pt-BR:" << endl;
            cerr << "  wget https://dl.fbaipublicfiles.com/fasttext/vectors-crawl/cc.pt.300.vec.gz" << endl;
            cerr << "  gunzip cc.pt.300.vec.gz" << endl;
            return false;
        }

        string line;
        getline(file, line);  // Pular cabeçalho (num_words dim)

        // Verificar dimensão
        istringstream iss(line);
        int numWords, dims;
        iss >> numWords >> dims;

        if (dims != dimensao) {
            cerr << "\033[33mAviso: fastText tem " << dims << " dims, modelo tem " << dimensao << "\033[0m" << endl;
            cerr << "Usando apenas as primeiras " << dimensao << " dimensões" << endl;
        }

        cout << "  fastText: " << numWords << " palavras, " << dims << " dims" << endl;

        int carregados = 0;
        int ignorados = 0;

        while (getline(file, line)) {
            istringstream ss(line);
            string palavra;
            ss >> palavra;

            // Normalizar palavra (lowercase, sem acentos)
            string normalizada = normalizarPalavra(palavra);

            // Verificar se está no nosso vocabulário
            if (vocab.find(normalizada) != vocab.end()) {
                int idx = vocab.at(normalizada);
                vector<float> vec(dims);
                for (int i = 0; i < dims; i++) {
                    ss >> vec[i];
                }

                // Copiar para W1 (com ou sem redução de dimensionalidade)
                for (int d = 0; d < min(dimensao, dims); d++) {
                    W1[idx][d] = vec[d];
                }
                isPretrained[idx] = true;  // Marcar como pré-treinado
                carregados++;
            } else {
                ignorados++;
            }

            if ((carregados + ignorados) % 100000 == 0) {
                cout << "\r  Processando: " << (carregados + ignorados) << " palavras..." << flush;
            }
        }

        file.close();

        cout << "\r\033[2K" << flush;
        cout << "  \033[32m✓\033[0m Carregados: " << carregados << " embeddings" << endl;
        cout << "  Ignorados: " << ignorados << " (fora do vocabulario)" << endl;

        pretrainedCount = carregados;
        randomCount = vocab.size() - carregados;

        return carregados > 0;
    }

    // Normalizar palavra (lowercase, remover acentos)
    string normalizarPalavra(const string& s) {
        string r;
        for (size_t i = 0; i < s.size(); i++) {
            unsigned char c = s[i];
            if (c < 0x80) {
                r += tolower(c);
            } else if (c >= 0xC0 && c < 0xE0 && i + 1 < s.size()) {
                // 2-byte UTF-8
                unsigned char c2 = s[i + 1];
                char mapped = 0;
                if (c == 0xC3) {
                    switch (c2) {
                        case 0xA1: mapped = 'a'; break;
                        case 0xA9: mapped = 'e'; break;
                        case 0xAD: mapped = 'i'; break;
                        case 0xB3: mapped = 'o'; break;
                        case 0xBA: mapped = 'u'; break;
                        case 0xA3: mapped = 'a'; break;
                        case 0xA7: mapped = 'c'; break;
                        case 0xB1: mapped = 'n'; break;
                        case 0xA0: mapped = 'a'; break;
                        case 0xA2: mapped = 'a'; break;
                        case 0xA6: mapped = 'e'; break;
                        case 0xB4: mapped = 'o'; break;
                        case 0xB6: mapped = 'o'; break;
                        default: mapped = c2; break;
                    }
                }
                if (mapped) {
                    r += mapped;
                    i++; // Skip next byte
                }
            }
        }
        return r;
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

    // Adam update
    void adamUpdate(vector<float>& peso, vector<float>& momento, vector<float>& vel,
                    float grad, int idx, int d) {
        momento[d] = beta1 * momento[d] + (1.0f - beta1) * grad;
        vel[d] = beta2 * vel[d] + (1.0f - beta2) * grad * grad;
        float mCorr = momento[d] / (1.0f - powf(beta1, passo));
        float vCorr = vel[d] / (1.0f - powf(beta2, passo));
        peso[d] -= taxaAtual * mCorr / (sqrtf(vCorr) + eps);
    }

    // Treina um par
    float treinarPar(int idxAlvo, int idxCtx, uniform_int_distribution<int>& distVocab,
                     bool congelar, const vector<bool>& isPretrained) {
        vector<float>& h = W1[idxAlvo];
        float perda = 0.0f;
        vector<float> gradIn(dimensao, 0.0f);

        // Positiva
        float dotPos = dot(W2[idxCtx], h);
        float sigPos = sigmoid(dotPos);
        float gPos = (1.0f - sigPos);
        perda -= logf(sigPos + 1e-9f);

        vector<float> w2CtxOriginal(W2[idxCtx].begin(), W2[idxCtx].end());

        for (int d = 0; d < dimensao; d++) {
            gradIn[d] += gPos * w2CtxOriginal[d];
        }

        // Negativas
        vector<vector<float>> w2NegOriginal;
        for (int n = 0; n < negativos; n++) {
            int idxNeg;
            do { idxNeg = distVocab(rng); } while (idxNeg == idxCtx);

            float dotNeg = dot(W2[idxNeg], h);
            float sigNeg = sigmoid(dotNeg);
            float gNeg = -sigNeg;
            perda -= logf(1.0f - sigNeg + 1e-9f);

            w2NegOriginal.push_back(vector<float>(W2[idxNeg].begin(), W2[idxNeg].end()));

            for (int d = 0; d < dimensao; d++) {
                gradIn[d] += gNeg * w2NegOriginal[n][d];
            }
        }

        // Atualizar W2 do contexto
        passo++;
        for (int d = 0; d < dimensao; d++) {
            adamUpdate(W2[idxCtx], mW2[idxCtx], vW2[idxCtx], gPos * h[d], idxCtx, d);
        }

        // Atualizar W2 dos negativos
        for (int n = 0; n < negativos; n++) {
            int idxNeg;
            do { idxNeg = distVocab(rng); } while (idxNeg == idxCtx);

            float dotNeg = dot(w2NegOriginal[n], h);
            float sigNeg = sigmoid(dotNeg);
            float gNeg = -sigNeg;

            passo++;
            for (int d = 0; d < dimensao; d++) {
                adamUpdate(W2[idxNeg], mW2[idxNeg], vW2[idxNeg], gNeg * h[d], idxNeg, d);
            }
        }

        // Atualizar embedding de entrada (apenas se não congelado ou não pré-treinado)
        if (!congelar || !isPretrained[idxAlvo]) {
            for (int d = 0; d < dimensao; d++) {
                adamUpdate(h, mW1[idxAlvo], vW1[idxAlvo], -gradIn[d], idxAlvo, d);
            }
        }

        return perda;
    }

    void treinar(const vector<ParTreino>& pares, int epocas, bool usarDecay,
                 bool congelar, const vector<bool>& isPretrained) {
        int totalPares = pares.size();
        uniform_int_distribution<int> distVocab(0, W1.size() - 1);

        auto inicio = chrono::high_resolution_clock::now();
        vector<pair<int,float>> historico;

        cout << "\n═══ INICIANDO TREINO ═══" << endl;
        if (pretrainedCount > 0) {
            cout << "  Modo: HIBRIDO (pré-treinado + fine-tune)" << endl;
            cout << "  Embeddings pré-treinados: " << pretrainedCount << endl;
            cout << "  Embeddings aleatórios: " << randomCount << endl;
        } else {
            cout << "  Modo: TREINO DO ZERO" << endl;
        }
        cout << endl;

        for (int e = 0; e < epocas; e++) {
            if (usarDecay)
                taxaAtual = max(taxaMinima, taxaBase * (1.0f - (float)e / epocas));

            float perdaEpoca = 0;

            for (int p = 0; p < totalPares; p++)
                perdaEpoca += treinarPar(pares[p].alvo, pares[p].contexto, distVocab,
                                        congelar, isPretrained);

            perdaEpoca /= totalPares;

            if ((e + 1) % 500 == 0) {
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

        // Curva de perda
        cout << "\n═══ Curva de Perda ═══" << endl;
        float perdaMin = 1e9, perdaMax = 0;
        for (auto& [ep, p] : historico) {
            if (p < perdaMin) perdaMin = p;
            if (p > perdaMax) perdaMax = p;
        }
        for (auto& [ep, p] : historico) {
            int tamanho = (perdaMax - perdaMin > 0.001f)
                ? (int)((p - perdaMin) / (perdaMax - perdaMin) * 50)
                : 0;
            cout << setw(5) << ep << " | ";
            for (int i = 0; i < tamanho; i++) cout << "\033[31m#\033[0m";
            cout << " " << fixed << setprecision(4) << p << endl;
        }
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

    void relatorio(const unordered_map<string, int>& vocab,
                   const unordered_map<string, int>& frequencias) {

        cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
        cout << "║         RELATORIO DE SIMILARIDADES                   ║" << endl;
        cout << "╚═══════════════════════════════════════════════════════╝" << endl;

        cout << "\n\033[1mTop 10 palavras mais frequentes:\033[0m" << endl;
        vector<pair<string,int>> freq(frequencias.begin(), frequencias.end());
        sort(freq.begin(), freq.end(), [](auto& a, auto& b){ return a.second > b.second; });
        for (int i = 0; i < min(10, (int)freq.size()); i++)
            cout << "  " << setw(2) << (i+1) << ". " << setw(12) << left << freq[i].first
                 << " (" << freq[i].second << " ocorrencias)" << endl;

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

string paraMinusculo(const string& s) {
    string r = s;
    transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

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
                if (!atual.empty() && atual.size() > 1)
                    tokens.push_back(atual);
                atual.clear();
            }
        }
        else if (c >= 0xC0 && c < 0xE0) {
            if (i + 1 < texto.size()) {
                unsigned char c2 = texto[i + 1];
                char mapped = 0;
                if (c == 0xC3) {
                    switch (c2) {
                        case 0xA1: mapped = 'a'; break;
                        case 0xA9: mapped = 'e'; break;
                        case 0xAD: mapped = 'i'; break;
                        case 0xB3: mapped = 'o'; break;
                        case 0xBA: mapped = 'u'; break;
                        case 0xA3: mapped = 'a'; break;
                        case 0xA7: mapped = 'c'; break;
                        case 0xB1: mapped = 'n'; break;
                        case 0xA0: mapped = 'a'; break;
                        case 0xA2: mapped = 'a'; break;
                        case 0xA6: mapped = 'e'; break;
                        case 0xB4: mapped = 'o'; break;
                        case 0xB6: mapped = 'o'; break;
                        default: mapped = c2; break;
                    }
                }
                if (mapped) {
                    atual += mapped;
                } else {
                    atual += (char)c;
                    atual += (char)c2;
                }
                i++;
            }
        }
        else if (c >= 0xE0 && c < 0xF0) {
            i += 2;
        }
        else if (c >= 0xF0) {
            i += 3;
        }
    }

    if (!atual.empty() && atual.size() > 1)
        tokens.push_back(atual);

    return tokens;
}

int main(int argc, char* argv[]) {
    string arquivoCorpus = "dados/embeddings/corpus.txt";
    string arquivoPreTreinado = "";
    int epocas = 5000;
    int dimensao = 100;  // Equilíbrio entre velocidade e qualidade
    int janela = 5;
    float taxaInicial = 0.001f;
    int negativos = 5;
    bool usarDecay = false;
    bool congelarPreTreinados = false;  // Congelar embeddings pré-treinados

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--arquivo" && i+1 < argc) arquivoCorpus = argv[++i];
        else if (arg == "--pre-treinado" && i+1 < argc) arquivoPreTreinado = argv[++i];
        else if (arg == "--epocas" && i+1 < argc) epocas = stoi(argv[++i]);
        else if (arg == "--dim" && i+1 < argc) dimensao = stoi(argv[++i]);
        else if (arg == "--janela" && i+1 < argc) janela = stoi(argv[++i]);
        else if (arg == "--taxa" && i+1 < argc) taxaInicial = stof(argv[++i]);
        else if (arg == "--neg" && i+1 < argc) negativos = stoi(argv[++i]);
        else if (arg == "--decay") usarDecay = true;
        else if (arg == "--freeze") congelarPreTreinados = true;
        else if (arg == "--ajuda" || arg == "-h") {
            cout << "Uso: ./word2vec_hybrid [opcoes]" << endl;
            cout << "  --arquivo <caminho>      Corpus (default: dados/embeddings/corpus.txt)" << endl;
            cout << "  --pre-treinado <caminho> Embeddings fastText (opcional)" << endl;
            cout << "  --epocas <N>             Epocas (default: 5000)" << endl;
            cout << "  --dim <N>                Dimensao (default: 300)" << endl;
            cout << "  --janela <N>             Janela de contexto (default: 5)" << endl;
            cout << "  --taxa <F>               Taxa inicial (default: 0.001)" << endl;
            cout << "  --neg <N>                Negativos (default: 5)" << endl;
            cout << "  --decay                  Usar decay na taxa" << endl;
            cout << "  --freeze                 Congelar embeddings pre-treinados" << endl;
            return 0;
        }
    }

    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║     WORD2VEC HIBRIDO (v1 - fastText + Natacha)      ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝" << endl;
    cout << "  Arquivo:       " << arquivoCorpus << endl;
    cout << "  Pre-treinado:  " << (arquivoPreTreinado.empty() ? "nenhum" : arquivoPreTreinado) << endl;
    cout << "  Dimensao:      " << dimensao << endl;
    cout << "  Negativos:     " << negativos << endl;
    cout << "  Janela:        " << janela << endl;
    cout << "  Taxa:          " << taxaInicial << (usarDecay ? " -> 0.0001 (decrescente)" : " (fixa)") << endl;
    cout << "  Epocas:        " << epocas << endl;
    cout << "  Optimizer:     Adam (beta1=0.9, beta2=0.999)" << endl;
    cout << "  Freeze:        " << (congelarPreTreinados ? "SIM" : "NAO") << endl;

    cout << "\nLendo corpus..." << endl;
    ifstream arquivo(arquivoCorpus);
    if (!arquivo.is_open()) {
        cerr << "\033[31mErro: nao encontrei " << arquivoCorpus << "\033[0m" << endl;
        return 1;
    }
    string linha, texto;
    while (getline(arquivo, linha)) texto += linha + " ";
    arquivo.close();
    cout << "Lido: " << texto.size() << " caracteres" << endl;

    cout << "Tokenizando..." << endl;
    vector<string> tokens = tokenizar(texto);
    cout << "Tokens: " << tokens.size() << endl;

    if (tokens.size() < 10) {
        cerr << "\033[31mCorpus muito pequeno\033[0m" << endl;
        return 1;
    }

    cout << "Construindo vocabulario..." << endl;
    unordered_map<string, int> freq;
    for (auto& t : tokens) freq[t]++;

    unordered_map<string, int> vocab;
    unordered_map<int, string> idxParaPalavra;
    int idx = 0;
    for (auto& [palavra, f] : freq) {
        vocab[palavra] = idx;
        idxParaPalavra[idx] = palavra;
        idx++;
    }
    cout << "Vocabulario: " << vocab.size() << " palavras" << endl;

    cout << "Gerando pares de treino..." << endl;
    vector<ParTreino> pares;
    for (size_t i = 0; i < tokens.size(); i++) {
        int alvo = vocab[tokens[i]];
        for (int j = max(0, (int)i - janela); j <= min((int)tokens.size()-1, (int)i + janela); j++) {
            if (i == (size_t)j) continue;
            pares.push_back({alvo, vocab[tokens[j]]});
        }
    }
    cout << "Pares gerados: " << pares.size() << endl;

    mt19937 rng(42);

    Word2VecHybrid modelo(vocab.size(), dimensao, negativos, taxaInicial, rng);

    // Carregar pré-treinados se especificado
    vector<bool> isPretrained(vocab.size(), false);
    if (!arquivoPreTreinado.empty()) {
        modelo.carregarPreTreinados(arquivoPreTreinado, vocab, isPretrained);
    }

    modelo.treinar(pares, epocas, usarDecay, congelarPreTreinados, isPretrained);

    cout << "\n═══ SALVANDO ═══" << endl;
    string saida = "dados/embeddings/vetores_hibrido.bin";
    ofstream out(saida, ios::binary);
    out.write((char*)&dimensao, sizeof(int));
    for (auto& [palavra, palavraIdx] : vocab) {
        uint32_t tam = palavra.size();
        out.write((char*)&tam, sizeof(uint32_t));
        out.write(palavra.c_str(), tam);
        vector<float> v = modelo.pegarVetor(palavraIdx);
        out.write((char*)v.data(), dimensao * sizeof(float));
    }
    out.close();
    cout << "\033[32m✓\033[0m Vetores salvos em " << saida << endl;

    modelo.relatorio(vocab, freq);

    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║              TREINO CONCLUIDO                        ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝\n" << endl;

    return 0;
}
