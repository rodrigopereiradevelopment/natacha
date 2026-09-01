// ============================================================================
//  MLP PROFUNDO + SERIALIZACAO JSON — NATACHA FASE 2
//
//  Novidades desta versao:
//    1. N camadas ocultas (ja tinha)
//    2. Salvar e carregar pesos em JSON (NOVO)
//    3. Correcao do warning RAND_MAX no clang
//
//  Por que JSON?
//    - A Natacha pode "dormir" (fechar o programa) e "acordar" sabendo.
//    - Pesos salvos podem ser inspecionados, versionados, transferidos.
//    - Formato humano-legivel (diferente de binario cru).
//
//  Dependencia: nlohmann/json (single-header, so incluir json.hpp)
// ============================================================================

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <fstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// ----------------------------------------------------------------------------
// FUNCOES MATEMATICAS
// ----------------------------------------------------------------------------

float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

float sigmoidDerivada(float s) {
    return s * (1.0f - s);
}

// Correcao: RAND_MAX eh int, entao convertemos ANTES da divisao
float pesoAleatorio() {
    return (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
}

// ----------------------------------------------------------------------------
// CAMADA
// ----------------------------------------------------------------------------
struct Camada {
    int n_entradas;
    int n_neuronios;
    vector<vector<float>> pesos;
    vector<float> bias;
    vector<float> soma;
    vector<float> ativacao;

    Camada(int entradas = 0, int neuronios = 0) {
        n_entradas = entradas;
        n_neuronios = neuronios;
        if (entradas > 0 && neuronios > 0) {
            pesos.resize(n_neuronios, vector<float>(n_entradas));
            bias.resize(n_neuronios);
            soma.resize(n_neuronios);
            ativacao.resize(n_neuronios);
        }
    }

    // Inicializa pesos aleatorios (usado no construtor da MLP)
    void inicializarAleatorio() {
        for (int i = 0; i < n_neuronios; i++) {
            for (int j = 0; j < n_entradas; j++)
                pesos[i][j] = pesoAleatorio();
            bias[i] = pesoAleatorio();
        }
    }
};

// ----------------------------------------------------------------------------
// MLP PROFUNDO
// ----------------------------------------------------------------------------
class MLP {
public:
    vector<Camada> camadas;
    float taxa = 0.3f;

    // ========================================================================
    // CONSTRUTOR
    // ========================================================================
    MLP(const vector<int>& arquitetura = {}, unsigned int semente = 42) {
        if (!arquitetura.empty()) {
            srand(semente);
            for (size_t i = 0; i < arquitetura.size() - 1; i++) {
                camadas.emplace_back(arquitetura[i], arquitetura[i + 1]);
                camadas.back().inicializarAleatorio();
            }
        }
    }

    // ========================================================================
    // FORWARD
    // ========================================================================
    vector<float> forward(const vector<float>& entrada) {
        vector<float> atual = entrada;
        for (size_t c = 0; c < camadas.size(); c++) {
            Camada& cam = camadas[c];
            vector<float> proxima(cam.n_neuronios);
            for (int i = 0; i < cam.n_neuronios; i++) {
                cam.soma[i] = cam.bias[i];
                for (int j = 0; j < cam.n_entradas; j++)
                    cam.soma[i] += atual[j] * cam.pesos[i][j];
                cam.ativacao[i] = sigmoid(cam.soma[i]);
                proxima[i] = cam.ativacao[i];
            }
            atual = proxima;
        }
        return atual;
    }

    // ========================================================================
    // BACKPROPAGATION
    // ========================================================================
    void treinar(const vector<float>& entrada, const vector<float>& esperado) {
        forward(entrada);
        int n_camadas = (int)camadas.size();
        vector<vector<float>> deltas(n_camadas);

        // Delta da ultima camada
        Camada& ultima = camadas.back();
        deltas[n_camadas - 1].resize(ultima.n_neuronios);
        for (int i = 0; i < ultima.n_neuronios; i++) {
            float erro = esperado[i] - ultima.ativacao[i];
            deltas[n_camadas - 1][i] = erro * sigmoidDerivada(ultima.ativacao[i]);
        }

        // Deltas das camadas ocultas (de tras pra frente)
        for (int c = n_camadas - 2; c >= 0; c--) {
            Camada& cam_atual = camadas[c];
            Camada& cam_prox = camadas[c + 1];
            deltas[c].resize(cam_atual.n_neuronios);
            for (int j = 0; j < cam_atual.n_neuronios; j++) {
                float soma = 0.0f;
                for (int k = 0; k < cam_prox.n_neuronios; k++)
                    soma += deltas[c + 1][k] * cam_prox.pesos[k][j];
                deltas[c][j] = soma * sigmoidDerivada(cam_atual.ativacao[j]);
            }
        }

        // Ajusta pesos
        for (int c = 0; c < n_camadas; c++) {
            Camada& cam = camadas[c];
            vector<float> entrada_da_camada = (c == 0) ? entrada : camadas[c - 1].ativacao;
            for (int i = 0; i < cam.n_neuronios; i++) {
                for (int j = 0; j < cam.n_entradas; j++)
                    cam.pesos[i][j] += taxa * deltas[c][i] * entrada_da_camada[j];
                cam.bias[i] += taxa * deltas[c][i];
            }
        }
    }

    // ========================================================================
    // SALVAR pesos em JSON
    // ========================================================================
    // Formato:
    // {
    //   "taxa": 0.3,
    //   "arquitetura": [2, 2, 1],
    //   "camadas": [
    //     {
    //       "n_entradas": 2,
    //       "n_neuronios": 2,
    //       "pesos": [[w00, w01], [w10, w11]],
    //       "bias": [b0, b1]
    //     },
    //     ...
    //   ]
    // }
    //
    void salvar(const string& caminho) const {
        json j;
        j["taxa"] = taxa;

        // Arquitetura: [entradas, ocultos..., saidas]
        vector<int> arquitetura;
        if (!camadas.empty()) {
            arquitetura.push_back(camadas[0].n_entradas);
            for (const auto& cam : camadas)
                arquitetura.push_back(cam.n_neuronios);
        }
        j["arquitetura"] = arquitetura;

        // Camadas
        j["camadas"] = json::array();
        for (const auto& cam : camadas) {
            json cam_json;
            cam_json["n_entradas"] = cam.n_entradas;
            cam_json["n_neuronios"] = cam.n_neuronios;
            cam_json["pesos"] = cam.pesos;
            cam_json["bias"] = cam.bias;
            j["camadas"].push_back(cam_json);
        }

        ofstream arquivo(caminho);
        if (!arquivo.is_open()) {
            cerr << "ERRO: nao conseguiu abrir '" << caminho << "' para escrita." << endl;
            return;
        }
        arquivo << j.dump(2);  // indentacao = 2 espacos
        arquivo.close();
        cout << "  Pesos salvos em: " << caminho << endl;
    }

    // ========================================================================
    // CARREGAR pesos de JSON
    // ========================================================================
    // Cria a rede a partir do JSON (sobrescreve qualquer estado anterior).
    //
    void carregar(const string& caminho) {
        ifstream arquivo(caminho);
        if (!arquivo.is_open()) {
            cerr << "ERRO: nao conseguiu abrir '" << caminho << "' para leitura." << endl;
            return;
        }

        json j;
        arquivo >> j;
        arquivo.close();

        taxa = j.value("taxa", 0.3f);

        // Reconstroi as camadas
        camadas.clear();
        for (const auto& cam_json : j["camadas"]) {
            int entradas = cam_json["n_entradas"];
            int neuronios = cam_json["n_neuronios"];
            Camada cam(entradas, neuronios);
            cam.pesos = cam_json["pesos"].get<vector<vector<float>>>();
            cam.bias = cam_json["bias"].get<vector<float>>();
            camadas.push_back(cam);
        }

        cout << "  Pesos carregados de: " << caminho << endl;
        cout << "  Arquitetura reconstruida: " << descricaoArquitetura() << endl;
    }

    // ========================================================================
    // MOSTRAR PESOS
    // ========================================================================
    void mostrarPesos() {
        cout << fixed << setprecision(4);
        for (size_t c = 0; c < camadas.size(); c++) {
            Camada& cam = camadas[c];
            string tipo = (c == camadas.size() - 1) ? "SAIDA" : "OCULTA " + to_string(c);
            cout << "  --- Camada " << c << " [" << tipo << "] ("
                 << cam.n_neuronios << " neuronios) ---" << endl;
            for (int i = 0; i < cam.n_neuronios; i++) {
                cout << "    n" << i << ": ";
                for (int j = 0; j < cam.n_entradas; j++)
                    cout << "w" << j << "=" << cam.pesos[i][j] << " ";
                cout << "bias=" << cam.bias[i] << endl;
            }
        }
    }

    string descricaoArquitetura() {
        if (camadas.empty()) return "vazia";
        string desc = to_string(camadas[0].n_entradas);
        for (const auto& cam : camadas)
            desc += "->" + to_string(cam.n_neuronios);
        return desc;
    }
};

// ----------------------------------------------------------------------------
// DATASET XOR
// ----------------------------------------------------------------------------
vector<vector<float>> entradasXOR = {
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}
};
vector<vector<float>> saidasXOR = {
    {0.0f},
    {1.0f},
    {1.0f},
    {0.0f}
};

// ----------------------------------------------------------------------------
// FUNCAO AUXILIAR: treina e testa
// ----------------------------------------------------------------------------
int testarXOR(MLP& rede, bool mostrarProgresso = true, int epocas = 20000) {
    if (mostrarProgresso) {
        cout << "--- Treinamento (" << epocas << " epocas) ---" << endl;
    }

    for (int e = 0; e < epocas; e++) {
        float erroTotal = 0.0f;
        for (int i = 0; i < 4; i++) {
            rede.treinar(entradasXOR[i], saidasXOR[i]);
            vector<float> pred = rede.forward(entradasXOR[i]);
            for (size_t j = 0; j < pred.size(); j++) {
                float erro = saidasXOR[i][j] - pred[j];
                erroTotal += erro * erro;
            }
        }
        if (mostrarProgresso && e % 2000 == 0) {
            cout << "  Epoca " << setw(5) << e << " | MSE: " << erroTotal << endl;
        }
    }

    // Teste final
    if (mostrarProgresso) {
        cout << "--- Teste final ---" << endl;
    }
    int acertos = 0;
    for (int i = 0; i < 4; i++) {
        vector<float> y = rede.forward(entradasXOR[i]);
        int arredondado = (y[0] >= 0.5f) ? 1 : 0;
        bool acertou = (arredondado == (int)saidasXOR[i][0]);
        if (acertou) acertos++;
        if (mostrarProgresso) {
            cout << "  [" << (int)entradasXOR[i][0] << "," << (int)entradasXOR[i][1] << "]"
                 << " -> " << fixed << setprecision(4) << y[0]
                 << " -> " << arredondado
                 << (acertou ? "  ACERTOU" : "  ERROU") << endl;
        }
    }
    return acertos;
}

// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------
int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - MLP PROFUNDO + SERIALIZACAO JSON" << endl;
    cout << "============================================================" << endl;

    // ================================================================
    // PARTE 1: Treinar do zero e salvar
    // ================================================================
    cout << endl;
    cout << "--- PARTE 1: Treinando do zero ---" << endl;
    cout << "Arquitetura: 2->2->1 (classico)" << endl;
    cout << endl;

    MLP natacha({2, 2, 1});
    natacha.mostrarPesos();
    cout << endl;

    int acertos = testarXOR(natacha, true, 20000);
    cout << "Resultado: " << acertos << "/4 acertos" << endl;

    if (acertos == 4) {
        cout << endl;
        cout << "--- Salvando pesos ---" << endl;
        natacha.salvar("natacha_pesos.json");
    } else {
        cout << "Treinamento falhou. Nao vou salvar pesos ruins." << endl;
        return 1;
    }

    // ================================================================
    // PARTE 2: Criar nova rede VAZIA e carregar pesos salvos
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
    cout << "--- PARTE 2: Natacha 'acorda' com pesos salvos ---" << endl;
    cout << "============================================================" << endl;
    cout << endl;

    // Cria uma rede NOVA, sem arquitetura definida
    MLP natachaNova;
    cout << "Rede nova criada (vazia)." << endl;

    // Carrega os pesos do arquivo JSON
    natachaNova.carregar("natacha_pesos.json");
    cout << "Taxa carregada: " << natachaNova.taxa << endl;
    cout << endl;

    natachaNova.mostrarPesos();
    cout << endl;

    // Testa SEM treinar — ela ja deve saber!
    cout << "--- Teste SEM treinar (usando pesos carregados) ---" << endl;
    int acertosCarregada = 0;
    for (int i = 0; i < 4; i++) {
        vector<float> y = natachaNova.forward(entradasXOR[i]);
        int arredondado = (y[0] >= 0.5f) ? 1 : 0;
        bool acertou = (arredondado == (int)saidasXOR[i][0]);
        if (acertou) acertosCarregada++;
        cout << "  [" << (int)entradasXOR[i][0] << "," << (int)entradasXOR[i][1] << "]"
             << " -> " << fixed << setprecision(4) << y[0]
             << " -> " << arredondado
             << (acertou ? "  ACERTOU" : "  ERROU") << endl;
    }

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Natacha original:   " << acertos << "/4 acertos" << endl;
    cout << "  Natacha 'acordada': " << acertosCarregada << "/4 acertos" << endl;
    if (acertosCarregada == 4)
        cout << "  A Natacha lembrou de tudo! A memoria persiste." << endl;
    else
        cout << "  ALGO ERRADO: os pesos nao foram carregados corretamente." << endl;
    cout << "============================================================" << endl;

    return 0;
}
