// ============================================================================
//  WORD2VEC SIMILARIDADE — Análise estatística de embeddings
// ============================================================================
//  Carrega embeddings treinados e roda análises completas:
//  - Similaridade entre pares específicos
//  - Top-N para palavras-chave
//  - Estatísticas globais (média, desvio, distribuição)
//  - Detecção de anomalias (palavras isoladas)
//
//  Compilar: g++ -O3 -std=c++17 -o word2vec_similaridade word2vec_similaridade.cpp
//  Uso:      ./word2vec_similaridade [--arquivo <caminho>] [--top N]
// ============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <cstdint>

using namespace std;

// ============================================================================
//  ESTRUTURA EMBEDDING
// ============================================================================
struct Embedding {
    string palavra;
    vector<float> vetor;
};

// ============================================================================
//  CARREGAR EMBEDDINGS DO ARQUIVO BINÁRIO
// ============================================================================
//  Mesmo formato do word2vec_neg:
//  - 4 bytes: dimensão (int)
//  - Para cada palavra:
//    - 4 bytes: tamanho da palavra (uint32_t)
//    - N bytes: nome da palavra (string)
//    - dim*4 bytes: vetor de floats
// ============================================================================
unordered_map<string, int> carregarEmbeddings(
    const string& caminho,
    vector<Embedding>& embeddings,
    int& dimensao
) {
    unordered_map<string, int> indice;

    ifstream arquivo(caminho, ios::binary);
    if (!arquivo.is_open()) {
        cerr << "Erro: nao encontrei " << caminho << endl;
        return indice;
    }

    arquivo.read((char*)&dimensao, sizeof(int));

    while (arquivo.peek() != EOF) {
        uint32_t tamPalavra;
        arquivo.read((char*)&tamPalavra, sizeof(uint32_t));
        if (arquivo.fail()) break;

        string palavra(tamPalavra, '\0');
        arquivo.read(&palavra[0], tamPalavra);

        vector<float> vetor(dimensao);
        arquivo.read((char*)vetor.data(), dimensao * sizeof(float));

        indice[palavra] = embeddings.size();
        embeddings.push_back({palavra, vetor});
    }

    arquivo.close();
    return indice;
}

// ============================================================================
//  CÁLCULO DE COSSENO (SIMILARIDADE)
// ============================================================================
float cosseno(const vector<float>& a, const vector<float>& b) {
    float produto = 0, normaA = 0, normaB = 0;
    for (size_t i = 0; i < a.size(); i++) {
        produto += a[i] * b[i];
        normaA  += a[i] * a[i];
        normaB  += b[i] * b[i];
    }
    if (normaA == 0 || normaB == 0) return 0;
    return produto / (sqrt(normaA) * sqrt(normaB));
}

// ============================================================================
//  TOP-N SIMILARES DE UMA PALAVRA
// ============================================================================
vector<pair<float, string>> topSimilares(
    const string& alvo,
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings,
    int n = 10
) {
    vector<pair<float, string>> resultados;
    auto it = indice.find(alvo);
    if (it == indice.end()) return resultados;

    int idxAlvo = it->second;
    const vector<float>& vAlvo = embeddings[idxAlvo].vetor;

    for (size_t i = 0; i < embeddings.size(); i++) {
        if ((int)i == idxAlvo) continue;
        float sim = cosseno(vAlvo, embeddings[i].vetor);
        resultados.push_back({sim, embeddings[i].palavra});
    }

    sort(resultados.rbegin(), resultados.rend());
    if ((int)resultados.size() > n) resultados.resize(n);
    return resultados;
}

// ============================================================================
//  SIMILARIDADE ENTRE DOIS PARES
// ============================================================================
float similaridadePar(
    const string& a, const string& b,
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings
) {
    auto itA = indice.find(a);
    auto itB = indice.find(b);
    if (itA == indice.end() || itB == indice.end()) return NAN;
    return cosseno(embeddings[itA->second].vetor, embeddings[itB->second].vetor);
}

// ============================================================================
//  ANÁLISE COMPLETA DE UMA PALAVRA-CHAVE
// ============================================================================
void analisarPalavra(
    const string& alvo,
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings,
    int topN
) {
    auto it = indice.find(alvo);
    if (it == indice.end()) {
        cout << "  \033[33m[" << alvo << " nao encontrado]\033[0m" << endl;
        return;
    }

    cout << "\n  \033[1;36m>> " << alvo << "\033[0m" << endl;

    auto similares = topSimilares(alvo, indice, embeddings, topN);

    // Barra visual para cada resultado
    for (size_t i = 0; i < similares.size(); i++) {
        float sim = similares[i].first;
        const string& pal = similares[i].second;

        // Cor baseada na similaridade
        string cor = (sim > 0.5f) ? "\033[32m" :
                     (sim > 0.3f) ? "\033[33m" :
                     (sim > 0.15f) ? "\033[90m" : "\033[31m";

        // Barra de 30 chars
        int barras = (int)(max(0.0f, sim) * 30);
        string barra(barras, '#');
        string resto(30 - barras, '.');

        cout << "    " << setw(2) << (i+1) << ". " << setw(15) << left << pal
             << " " << cor << fixed << setprecision(4) << sim << "\033[0m "
             << barra << resto << endl;
    }
}

// ============================================================================
//  TESTAR PAR DE PALAVRAS (COM EXPECTATIVA)
// ============================================================================
void testarPar(
    const string& a, const string& b,
    const string& descricao,
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings
) {
    float sim = similaridadePar(a, b, indice, embeddings);

    if (isnan(sim)) {
        cout << "  \033[33m[" << a << " ou " << b << " nao encontrado]\033[0m" << endl;
        return;
    }

    string cor = (sim > 0.5f) ? "\033[32m\033[1m EXCELENTE" :
                 (sim > 0.3f) ? "\033[32m BOM" :
                 (sim > 0.15f) ? "\033[33m RAZOAVEL" :
                 (sim > 0.0f) ? "\033[31m FRACO" :
                 "\033[31m\033[1m NEGATIVO";

    int barras = (int)(max(0.0f, sim) * 20);
    string barra(barras, '#');
    string resto(20 - barras, '.');

    cout << "  " << setw(12) << left << a << " <-> " << setw(12) << left << b
         << " " << fixed << setprecision(4) << sim
         << " (" << descricao << ")"
         << " \033[90m[" << barra << resto << "]\033[0m"
         << cor << "\033[0m" << endl;
}

// ============================================================================
//  ESTATÍSTICAS GLOBAIS DOS EMBEDDINGS
// ============================================================================
void estatisticasGlobais(
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings
) {
    cout << "\n\033[1m═══════════════════════════════════════════════\033[0m" << endl;
    cout << "\033[1m         ESTATÍSTICAS GLOBAIS                  \033[0m" << endl;
    cout << "\033[1m═══════════════════════════════════════════════\033[0m" << endl;

    // Amostra aleatória de pares para estimar distribuição
    vector<float> amostra;
    amostra.reserve(10000);

    vector<int> indices(embeddings.size());
    iota(indices.begin(), indices.end(), 0);

    // Pega 5000 pares aleatórios (rápido e representativo)
    for (int i = 0; i < 5000 && i < (int)embeddings.size(); i++) {
        int a = rand() % embeddings.size();
        int b = rand() % embeddings.size();
        if (a == b) continue;
        float sim = cosseno(embeddings[a].vetor, embeddings[b].vetor);
        amostra.push_back(sim);
    }

    if (amostra.empty()) return;

    sort(amostra.begin(), amostra.end());

    float soma = accumulate(amostra.begin(), amostra.end(), 0.0f);
    float media = soma / amostra.size();

    // Variância
    float var = 0;
    for (float x : amostra) var += (x - media) * (x - media);
    var /= amostra.size();
    float desvio = sqrt(var);

    // Percentis
    auto pct = [&](float p) {
        return amostra[(int)(p * amostra.size())];
    };

    cout << "  Total palavras:     " << embeddings.size() << endl;
    cout << "  Dimensão:           " << embeddings[0].vetor.size() << endl;
    cout << "  Amostra pares:      " << amostra.size() << endl;
    cout << "  Similaridade média: " << fixed << setprecision(4) << media << endl;
    cout << "  Desvio padrão:      " << fixed << setprecision(4) << desvio << endl;
    cout << "  Min:                " << fixed << setprecision(4) << amostra.front() << endl;
    cout << "  P25:                " << fixed << setprecision(4) << pct(0.25) << endl;
    cout << "  P50 (mediana):      " << fixed << setprecision(4) << pct(0.50) << endl;
    cout << "  P75:                " << fixed << setprecision(4) << pct(0.75) << endl;
    cout << "  P90:                " << fixed << setprecision(4) << pct(0.90) << endl;
    cout << "  P95:                " << fixed << setprecision(4) << pct(0.95) << endl;
    cout << "  Max:                " << fixed << setprecision(4) << amostra.back() << endl;

    // Contagem por faixa
    int neg = 0, fraco = 0, razoavel = 0, bom = 0, exce = 0;
    for (float x : amostra) {
        if (x < 0) neg++;
        else if (x < 0.15f) fraco++;
        else if (x < 0.3f) razoavel++;
        else if (x < 0.5f) bom++;
        else exce++;
    }
    cout << "\n  Distribuição:" << endl;
    cout << "    Negativo (<0):    " << setw(5) << neg << " (" << (neg*100.0/amostra.size()) << "%)" << endl;
    cout << "    Fraco (0-0.15):   " << setw(5) << fraco << " (" << (fraco*100.0/amostra.size()) << "%)" << endl;
    cout << "    Razoável (0.15-0.3): " << setw(5) << razoavel << " (" << (razoavel*100.0/amostra.size()) << "%)" << endl;
    cout << "    Bom (0.3-0.5):    " << setw(5) << bom << " (" << (bom*100.0/amostra.size()) << "%)" << endl;
    cout << "    Excelente (>0.5): " << setw(5) << exce << " (" << (exce*100.0/amostra.size()) << "%)" << endl;
}

// ============================================================================
//  PALAVRAS MAIS ISOLADAS (BAIXA SIMILARIDADE MÁXIMA)
// ============================================================================
void palavrasIsoladas(
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings,
    int n = 10
) {
    cout << "\n\033[1m═══════════════════════════════════════════════\033[0m" << endl;
    cout << "\033[1m      PALAVRAS MAIS ISOLADAS (outliers)       \033[0m" << endl;
    cout << "\033[1m═══════════════════════════════════════════════\033[0m" << endl;

    vector<pair<float, string>> isoladas;

    for (size_t i = 0; i < embeddings.size(); i++) {
        float maxSim = -1;
        // Verifica contra 200 palavras aleatórias
        for (int k = 0; k < 200; k++) {
            int j = rand() % embeddings.size();
            if ((int)i == j) continue;
            float sim = cosseno(embeddings[i].vetor, embeddings[j].vetor);
            if (sim > maxSim) maxSim = sim;
        }
        isoladas.push_back({maxSim, embeddings[i].palavra});
    }

    sort(isoladas.begin(), isoladas.end());

    cout << "  Top " << n << " palavras com menor similaridade máxima:" << endl;
    for (int i = 0; i < min(n, (int)isoladas.size()); i++) {
        cout << "    " << setw(2) << (i+1) << ". " << setw(15) << left << isoladas[i].second
             << " max_sim=" << fixed << setprecision(4) << isoladas[i].first << endl;
    }
}

// ============================================================================
//  MATRIZ DE SIMILARIDADE ENTRE CONJUNTO DE PALAVRAS
// ============================================================================
void matrizSimilaridade(
    const vector<string>& palavras,
    const unordered_map<string, int>& indice,
    const vector<Embedding>& embeddings
) {
    cout << "\n\033[1m═══════════════════════════════════════════════\033[0m" << endl;
    cout << "\033[1m         MATRIZ DE SIMILARIDADE               \033[0m" << endl;
    cout << "\033[1m═══════════════════════════════════════════════\033[0m" << endl;

    // Cabeçalho
    cout << "         ";
    for (const auto& p : palavras) {
        cout << setw(10) << right << p.substr(0, 8);
    }
    cout << endl;

    // Linhas
    for (const auto& pa : palavras) {
        auto itA = indice.find(pa);
        if (itA == indice.end()) continue;

        cout << setw(10) << right << pa.substr(0, 8);

        for (const auto& pb : palavras) {
            auto itB = indice.find(pb);
            if (itB == indice.end()) {
                cout << "     N/A ";
                continue;
            }
            float sim = cosseno(embeddings[itA->second].vetor, embeddings[itB->second].vetor);
            string cor = (sim > 0.5f) ? "\033[32m" :
                         (sim > 0.3f) ? "\033[33m" :
                         (sim > 0.15f) ? "\033[90m" : "\033[31m";
            cout << " " << cor << setw(7) << fixed << setprecision(3) << sim << "\033[0m ";
        }
        cout << endl;
    }
}

// ============================================================================
//  MAIN
// ============================================================================
int main(int argc, char* argv[]) {
    string caminhoBin = "dados/embeddings/vetores_word2vec.bin";
    int topN = 10;

    // Parse argumentos
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--arquivo" && i+1 < argc)
            caminhoBin = argv[++i];
        else if (arg == "--top" && i+1 < argc)
            topN = stoi(argv[++i]);
        else if (arg == "--ajuda" || arg == "-h") {
            cout << "Uso: ./word2vec_similaridade [--arquivo <caminho>] [--top N]" << endl;
            return 0;
        }
    }

    cout << "\n╔═══════════════════════════════════════════════════════╗" << endl;
    cout << "║      WORD2VEC SIMILARIDADE — Análise Completa       ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════╝" << endl;

    // Carrega embeddings
    vector<Embedding> embeddings;
    int dimensao;
    auto indice = carregarEmbeddings(caminhoBin, embeddings, dimensao);

    if (indice.empty()) {
        cerr << "Erro: nenhum embedding carregado." << endl;
        return 1;
    }

    cout << "  Carregado: " << indice.size() << " palavras, dim=" << dimensao << endl;

    // ========================================================================
    //  1. PARÂMETROS DE TESTE (palavras-chave do corpus Natacha)
    // ========================================================================
    vector<string> chaves = {
        "felix", "natacha", "cozinha", "cpu", "robo", "teclado",
        "rodrigo", "casa", "gato", "memoria"
    };

    struct ParTeste { string a, b, desc; };

    vector<ParTeste> paresTeste = {
        {"felix", "natacha", "moro junto"},
        {"felix", "teclado", "comportamento felix"},
        {"felix", "gato", "felix e gato"},
        {"natacha", "robo", "deve ser alto"},
        {"natacha", "construiu", "ela constroi"},
        {"cozinha", "cpu", "hardware e comodo"},
        {"cozinha", "gato", "felix frequenta"},
        {"cpu", "gpu", "hardware"},
        {"memoria", "ram", "hardware"},
        {"rodrigo", "natacha", "criador"}
    };

    // ========================================================================
    //  2. TESTE DE PARES ESPERADOS
    // ========================================================================
    cout << "\n\033[1m═══════════════════════════════════════════════\033[0m" << endl;
    cout << "\033[1m         PARES DE TESTE (esperados)           \033[0m" << endl;
    cout << "\033[1m═══════════════════════════════════════════════\033[0m" << endl;

    for (auto& p : paresTeste) {
        testarPar(p.a, p.b, p.desc, indice, embeddings);
    }

    // ========================================================================
    //  3. TOP-N PARA CADA PALAVRA-CHAVE
    // ========================================================================
    cout << "\n\033[1m═══════════════════════════════════════════════\033[0m" << endl;
    cout << "\033[1m         TOP-" << topN << " PALAVRAS-CHAVE                 \033[0m" << endl;
    cout << "\033[1m═══════════════════════════════════════════════\033[0m" << endl;

    for (const string& chave : chaves) {
        analisarPalavra(chave, indice, embeddings, topN);
    }

    // ========================================================================
    //  4. MATRIZ DE SIMILARIDADE (subconjunto)
    // ========================================================================
    vector<string> matrizSet = {"natacha", "felix", "cpu", "cozinha", "robo", "memoria"};
    matrizSimilaridade(matrizSet, indice, embeddings);

    // ========================================================================
    //  5. ESTATÍSTICAS GLOBAIS
    // ========================================================================
    estatisticasGlobais(indice, embeddings);

    // ========================================================================
    //  6. PALAVRAS ISOLADAS
    // ========================================================================
    palavrasIsoladas(indice, embeddings, 10);

    cout << "\n\033[32m✓\033[0m Análise concluída.\n" << endl;
    return 0;
}