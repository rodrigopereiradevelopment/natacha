// word2vec_neg_full.cpp — Word2Vec Negative Sampling (C++ puro)
// Compilar: g++ -O3 -std=c++17 -o word2vec_neg word2vec_neg_full.cpp

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

class Word2VecNeg {
private:
    int dimensao;
    int negativos;
    float taxaBase;
    float taxaMinima;
    float taxaAtual;
    mt19937& rng;
    vector<vector<float>> W1;  // embeddings de entrada
    vector<vector<float>> W2;  // pesos de saida

    // Adam optimizer
    vector<vector<float>> mW1, vW1, mW2, vW2; // momentos
    float beta1, beta2, eps;
    int passo;

    float sigmoid(float x) {
        if (x > 6.0f) return 1.0f;
        if (x < -6.0f) return 0.0f;
        return 1.0f / (1.0f + expf(-x));
    }

public:
    Word2VecNeg(int vocabTam, int dim, int neg, float taxa, mt19937& random)
        : dimensao(dim), negativos(neg), taxaBase(taxa), taxaMinima(0.0001f),
          taxaAtual(taxa), rng(random), passo(0),
          beta1(0.9f), beta2(0.999f), eps(1e-8f) {

        float escala = sqrt(1.0f / dimensao);

        W1.resize(vocabTam, vector<float>(dimensao));
        W2.resize(vocabTam, vector<float>(dimensao));

        for (auto& v : W1)
            for (auto& x : v)
                x = ((float)rng() / (float)rng.max() * 2.0f - 1.0f) * escala;
        for (auto& v : W2)
            for (auto& x : v)
                x = ((float)rng() / (float)rng.max() * 2.0f - 1.0f) * escala;

        // Inicializar momentos do Adam
        mW1.assign(vocabTam, vector<float>(dimensao, 0.0f));
        vW1.assign(vocabTam, vector<float>(dimensao, 0.0f));
        mW2.assign(vocabTam, vector<float>(dimensao, 0.0f));
        vW2.assign(vocabTam, vector<float>(dimensao, 0.0f));
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

    // Adam update para um vetor
    void adamUpdate(vector<float>& peso, vector<float>& momento, vector<float>& vel,
                    float grad, int idx, int d) {
        momento[d] = beta1 * momento[d] + (1.0f - beta1) * grad;
        vel[d] = beta2 * vel[d] + (1.0f - beta2) * grad * grad;
        float mCorr = momento[d] / (1.0f - powf(beta1, passo));
        float vCorr = vel[d] / (1.0f - powf(beta2, passo));
        peso[d] -= taxaAtual * mCorr / (sqrtf(vCorr) + eps);
    }

    // Treina um par (alvo, contexto) — retorna perda
    float treinarPar(int idxAlvo, int idxCtx, uniform_int_distribution<int>& distVocab) {
        vector<float>& h = W1[idxAlvo];
        float perda = 0.0f;
        vector<float> gradIn(dimensao, 0.0f);

        // Positiva: maximizar sig(w_ctx · h)
        float dotPos = dot(W2[idxCtx], h);
        float sigPos = sigmoid(dotPos);
        float gPos = (1.0f - sigPos);  // sem taxa aqui, Adam cuida
        perda -= logf(sigPos + 1e-9f);

        // Salvar valores originais de W2 antes de atualizar
        vector<float> w2CtxOriginal(W2[idxCtx].begin(), W2[idxCtx].end());

        for (int d = 0; d < dimensao; d++) {
            gradIn[d] += gPos * w2CtxOriginal[d];
        }

        // Negativas: minimizar sig(w_neg · h)
        vector<vector<float>> w2NegOriginal;
        for (int n = 0; n < negativos; n++) {
            int idxNeg;
            do { idxNeg = distVocab(rng); } while (idxNeg == idxCtx);

            float dotNeg = dot(W2[idxNeg], h);
            float sigNeg = sigmoid(dotNeg);
            float gNeg = -sigNeg;  // sem taxa aqui, Adam cuida
            perda -= logf(1.0f - sigNeg + 1e-9f);

            w2NegOriginal.push_back(vector<float>(W2[idxNeg].begin(), W2[idxNeg].end()));

            for (int d = 0; d < dimensao; d++) {
                gradIn[d] += gNeg * w2NegOriginal[n][d];
            }
        }

        // Atualizar W2 do contexto com Adam
        passo++;
        for (int d = 0; d < dimensao; d++) {
            adamUpdate(W2[idxCtx], mW2[idxCtx], vW2[idxCtx], gPos * h[d], idxCtx, d);
        }

        // Atualizar W2 dos negativos com Adam
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

        // Atualizar embedding de entrada com Adam
        for (int d = 0; d < dimensao; d++) {
            adamUpdate(h, mW1[idxAlvo], vW1[idxAlvo], -gradIn[d], idxAlvo, d);
        }

        return perda;
    }

    void treinar(const vector<ParTreino>& pares, int epocas, bool usarDecay) {
        int totalPares = pares.size();
        uniform_int_distribution<int> distVocab(0, W1.size() - 1);

        auto inicio = chrono::high_resolution_clock::now();
        vector<pair<int,float>> historico;

        for (int e = 0; e < epocas; e++) {
            if (usarDecay)
                taxaAtual = max(taxaMinima, taxaBase * (1.0f - (float)e / epocas));
            float perdaEpoca = 0;

            for (int p = 0; p < totalPares; p++)
                perdaEpoca += treinarPar(pares[p].alvo, pares[p].contexto, distVocab);

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

// Remove acentos e normaliza UTF-8 (substitui por ASCII)
string normalizar(const string& s) {
    string r;
    for (unsigned char c : s) {
        if (c < 0x80) {
            r += c;
        } else if (c == 0xC3) {
            // Início de caractere acentuado UTF-8 (2 bytes)
            // Próximo byte define qual acento
            // Vamos apenas marcar para remoção
        }
        // Ignora bytes 0x80-0xBF (continuação UTF-8)
    }
    return r;
}

vector<string> tokenizar(const string& texto) {
    vector<string> tokens;
    string atual;

    // Mapa de acentuados para não-acentuados (UTF-8 -> ASCII)
    auto normalizeUtf8 = [](unsigned char c) -> char {
        // Para simplificar, convertemos tudo para minúsculo sem acento
        // Isso funciona para o corpus da Natacha
        return c;
    };

    for (size_t i = 0; i < texto.size(); i++) {
        unsigned char c = texto[i];

        // Byte ASCII normal
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
        // Início de sequência UTF-8 (2-4 bytes)
        else if (c >= 0xC0 && c < 0xE0) {
            // 2 bytes: á, é, í, ó, ú, ñ, etc
            if (i + 1 < texto.size()) {
                unsigned char c2 = texto[i + 1];
                // Mapear acentuados para ASCII
                char mapped = 0;
                if (c == 0xC3) {
                    switch (c2) {
                        case 0xA1: mapped = 'a'; break; // á
                        case 0xA9: mapped = 'e'; break; // é
                        case 0xAD: mapped = 'i'; break; // í
                        case 0xB3: mapped = 'o'; break; // ó
                        case 0xBA: mapped = 'u'; break; // ú
                        case 0xA3: mapped = 'a'; break; // ã
                        case 0xA7: mapped = 'c'; break; // ç
                        case 0xB1: mapped = 'n'; break; // ñ
                        case 0xA0: mapped = 'a'; break; // à
                        case 0xA2: mapped = 'a'; break; // â
                        case 0xAA: mapped = 'a'; break; // ä
                        case 0xA4: mapped = 'e'; break; // ë
                        case 0xA6: mapped = 'e'; break; // ê
                        case 0xAC: mapped = 'i'; break; // ì
                        case 0xAE: mapped = 'i'; break; // î
                        case 0xAF: mapped = 'i'; break; // ï
                        case 0xB2: mapped = 'o'; break; // ò
                        case 0xB4: mapped = 'o'; break; // ô
                        case 0xB6: mapped = 'o'; break; // õ
                        case 0xBC: mapped = 'u'; break; // ù
                        case 0xBB: mapped = 'u'; break; // û
                        case 0xBF: mapped = 'u'; break; // ü
                        default: mapped = c2; break;
                    }
                }
                if (mapped) {
                    atual += mapped;
                } else {
                    // Não mapeado, usar original
                    atual += (char)c;
                    atual += (char)c2;
                }
                i++; // Pular próximo byte
            }
        }
        // 3 bytes (raro em português)
        else if (c >= 0xE0 && c < 0xF0) {
            i += 2; // Pular 2 bytes
        }
        // 4 bytes (emoji, etc)
        else if (c >= 0xF0) {
            i += 3; // Pular 3 bytes
        }
    }

    if (!atual.empty() && atual.size() > 1)
        tokens.push_back(atual);

    return tokens;
}

int main(int argc, char* argv[]) {
    string arquivoCorpus = "dados/embeddings/corpus.txt";
    int epocas = 2000;
    int dimensao = 16;
    int janela = 5;
    float taxaInicial = 0.001f;  // Adam funciona melhor com taxas menores
    int negativos = 5;
    bool usarDecay = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--arquivo" && i+1 < argc) arquivoCorpus = argv[++i];
        else if (arg == "--epocas" && i+1 < argc) epocas = stoi(argv[++i]);
        else if (arg == "--dim" && i+1 < argc) dimensao = stoi(argv[++i]);
        else if (arg == "--janela" && i+1 < argc) janela = stoi(argv[++i]);
        else if (arg == "--taxa" && i+1 < argc) taxaInicial = stof(argv[++i]);
        else if (arg == "--neg" && i+1 < argc) negativos = stoi(argv[++i]);
        else if (arg == "--decay") usarDecay = true;
        else if (arg == "--ajuda" || arg == "-h") {
            cout << "Uso: ./word2vec_neg [opcoes]" << endl;
            cout << "  --arquivo <caminho>   Corpus (default: dados/embeddings/corpus.txt)" << endl;
            cout << "  --epocas <N>          Epocas (default: 5000)" << endl;
            cout << "  --dim <N>             Dimensao (default: 64)" << endl;
            cout << "  --janela <N>          Janela de contexto (default: 5)" << endl;
            cout << "  --taxa <F>            Taxa inicial (default: 0.05)" << endl;
            cout << "  --neg <N>             Negativos (default: 5)" << endl;
            cout << "  --decay               Usar decay na taxa (default: fixa)" << endl;
            return 0;
        }
    }

    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║     WORD2VEC NEGATIVE SAMPLING (v6 - Adam)         ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝" << endl;
    cout << "  Arquivo:    " << arquivoCorpus << endl;
    cout << "  Dimensao:   " << dimensao << endl;
    cout << "  Negativos:  " << negativos << endl;
    cout << "  Janela:     " << janela << endl;
    cout << "  Taxa:       " << taxaInicial << (usarDecay ? " -> 0.0001 (decrescente)" : " (fixa Adam)") << endl;
    cout << "  Epocas:     " << epocas << endl;
    cout << "  Optimizer:  Adam (beta1=0.9, beta2=0.999)" << endl;

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

    mt19937 rng(42);  // Mover RNG antes da geração de pares

    cout << "Gerando pares de treino (janela dinamica)..." << endl;
    vector<ParTreino> pares;
    uniform_int_distribution<int> distJanela(1, janela);
    for (size_t i = 0; i < tokens.size(); i++) {
        int alvo = vocab[tokens[i]];
        int janelaAtual = distJanela(rng);  // Janela aleatoria entre 1 e janela
        for (int j = max(0, (int)i - janelaAtual); j <= min((int)tokens.size()-1, (int)i + janelaAtual); j++) {
            if (i == (size_t)j) continue;
            pares.push_back({alvo, vocab[tokens[j]]});
        }
    }
    cout << "Pares gerados: " << pares.size() << endl;

    cout << "\n═══ INICIANDO TREINO ═══" << endl;

    Word2VecNeg modelo(vocab.size(), dimensao, negativos, taxaInicial, rng);
    modelo.treinar(pares, epocas, usarDecay);

    cout << "\n═══ SALVANDO ═══" << endl;
    string saida = "dados/embeddings/vetores_word2vec.bin";
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
    cout << "║  Proximos passos:                                    ║" << endl;
    cout << "║  1. ./natacha_embedder                               ║" << endl;
    cout << "║  2. ./word2vec_similaridade                          ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝\n" << endl;

    return 0;
}
