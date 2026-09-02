// ============================================================================
//  MLP PROFUNDO + Leaky ReLU + Softmax — NATACHA FASE 2
//
//  Correcoes v4:
//    1. Semente manual por rede (evita srand(time(0)) igual pra todas)
//    2. Clip ajustado: 1.0 no Softmax, 5.0 no resto
//    3. Dataset Teste 2 linearmente separavel
//    4. Inicializacao Box-Muller (normal) com He/Xavier
//
//  Dependencia: nlohmann/json (json.hpp no mesmo diretorio)
// ============================================================================

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// ----------------------------------------------------------------------------
// ENUM DE ATIVACAO
// ----------------------------------------------------------------------------
enum class Ativacao { SIGMOID, LEAKY_RELU, SOFTMAX };

string ativacaoToString(Ativacao a) {
    if (a == Ativacao::LEAKY_RELU) return "leaky_relu";
    if (a == Ativacao::SOFTMAX)    return "softmax";
    return "sigmoid";
}

Ativacao stringToAtivacao(const string& s) {
    if (s == "leaky_relu") return Ativacao::LEAKY_RELU;
    if (s == "softmax")    return Ativacao::SOFTMAX;
    return Ativacao::SIGMOID;
}

// ----------------------------------------------------------------------------
// FUNCOES MATEMATICAS
// ----------------------------------------------------------------------------
float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

float sigmoidDerivada(float s) {
    return s * (1.0f - s);
}

float leakyRelu(float x) {
    return x > 0.0f ? x : 0.01f * x;
}

float leakyReluDerivada(float x) {
    return x > 0.0f ? 1.0f : 0.01f;
}

float clip(float v, float limite = 5.0f) {
    if (v >  limite) return  limite;
    if (v < -limite) return -limite;
    return v;
}

// Softmax com protecao contra overflow
vector<float> softmax(const vector<float>& x) {
    float maxVal = *max_element(x.begin(), x.end());
    vector<float> result(x.size());
    float somaExp = 0.0f;
    for (size_t i = 0; i < x.size(); i++) {
        result[i] = exp(x[i] - maxVal);
        somaExp += result[i];
    }
    for (size_t i = 0; i < x.size(); i++)
        result[i] /= somaExp;
    return result;
}

// ----------------------------------------------------------------------------
// CAMADA
// ----------------------------------------------------------------------------
struct Camada {
    int n_entradas;
    int n_neuronios;
    Ativacao ativacao;
    vector<vector<float>> pesos;
    vector<float> bias;
    vector<float> soma;
    vector<float> ativ;

    Camada(int entradas = 0, int neuronios = 0, Ativacao tipoAtiv = Ativacao::SIGMOID) {
        n_entradas  = entradas;
        n_neuronios = neuronios;
        ativacao    = tipoAtiv;
        if (entradas > 0 && neuronios > 0) {
            pesos.resize(n_neuronios, vector<float>(n_entradas));
            bias.resize(n_neuronios, 0.0f);
            soma.resize(n_neuronios);
            ativ.resize(n_neuronios);
        }
    }

    void inicializarAleatorio() {
        float escala;
        if (ativacao == Ativacao::LEAKY_RELU)
            escala = sqrt(2.0f / n_entradas);
        else
            escala = sqrt(1.0f / n_entradas) * 0.5f;

        for (int i = 0; i < n_neuronios; i++) {
            for (int j = 0; j < n_entradas; j++) {
                float u1 = ((float)rand() + 1.0f) / ((float)RAND_MAX + 2.0f);
                float u2 = ((float)rand() + 1.0f) / ((float)RAND_MAX + 2.0f);
                float normal = sqrt(-2.0f * log(u1)) * cos(2.0f * M_PI * u2);
                if ((i + j) % 2 == 0)
                    pesos[i][j] =  fabs(normal) * escala;
                else
                    pesos[i][j] = -fabs(normal) * escala;
            }
            bias[i] = 0.01f;
        }
    }

    void aplicarAtivacao() {
        if (ativacao == Ativacao::SIGMOID) {
            for (int i = 0; i < n_neuronios; i++)
                ativ[i] = sigmoid(soma[i]);
        } else if (ativacao == Ativacao::LEAKY_RELU) {
            for (int i = 0; i < n_neuronios; i++)
                ativ[i] = leakyRelu(soma[i]);
        }
    }

    float derivada(int i) const {
        if (ativacao == Ativacao::SIGMOID)
            return sigmoidDerivada(ativ[i]);
        if (ativacao == Ativacao::LEAKY_RELU)
            return leakyReluDerivada(soma[i]);
        return 0.0f;
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
    // CONSTRUTOR — agora recebe semente manual!
    // ========================================================================
    MLP(const vector<int>& arquitetura = {},
        const vector<Ativacao>& ativacoes = {},
        unsigned int semente = 0) {
        if (!arquitetura.empty() && arquitetura.size() >= 2) {
            // Se semente == 0, usa time(0) como fallback
            srand(semente > 0 ? semente : (unsigned int)time(0));
            int n_camadas = (int)arquitetura.size() - 1;
            for (int i = 0; i < n_camadas; i++) {
                Ativacao ativ = (i < (int)ativacoes.size())
                                ? ativacoes[i]
                                : Ativacao::SIGMOID;
                camadas.emplace_back(arquitetura[i], arquitetura[i + 1], ativ);
                camadas.back().inicializarAleatorio();
            }
        }
    }

    // ===================================================================
    // FORWARD
    // ===================================================================
    vector<float> forward(const vector<float>& entrada) {
        vector<float> atual = entrada;
        for (size_t c = 0; c < camadas.size(); c++) {
            Camada& cam = camadas[c];
            for (int i = 0; i < cam.n_neuronios; i++) {
                cam.soma[i] = cam.bias[i];
                for (int j = 0; j < cam.n_entradas; j++)
                    cam.soma[i] += atual[j] * cam.pesos[i][j];
            }
            if (cam.ativacao == Ativacao::SOFTMAX)
                cam.ativ = softmax(cam.soma);
            else
                cam.aplicarAtivacao();
            atual = cam.ativ;
        }
        return atual;
    }

    // ===================================================================
    // BACKPROPAGATION
    // ===================================================================
    void treinar(const vector<float>& entrada, const vector<float>& esperado) {
        forward(entrada);
        int n_camadas = (int)camadas.size();
        vector<vector<float>> deltas(n_camadas);

        Camada& ultima = camadas.back();
        deltas[n_camadas - 1].resize(ultima.n_neuronios);

        if (ultima.ativacao == Ativacao::SOFTMAX) {
            for (int i = 0; i < ultima.n_neuronios; i++)
                deltas[n_camadas - 1][i] = ultima.ativ[i] - esperado[i];
        } else {
            for (int i = 0; i < ultima.n_neuronios; i++) {
                float erro = esperado[i] - ultima.ativ[i];
                deltas[n_camadas - 1][i] = clip(erro * ultima.derivada(i), 5.0f);
            }
        }

        for (int c = n_camadas - 2; c >= 0; c--) {
            Camada& cam_atual = camadas[c];
            Camada& cam_prox  = camadas[c + 1];
            deltas[c].resize(cam_atual.n_neuronios);
            for (int j = 0; j < cam_atual.n_neuronios; j++) {
                float soma = 0.0f;
                for (int k = 0; k < cam_prox.n_neuronios; k++)
                    soma += deltas[c + 1][k] * cam_prox.pesos[k][j];
                deltas[c][j] = clip(soma * cam_atual.derivada(j), 5.0f);
            }
        }

        for (int c = 0; c < n_camadas; c++) {
            Camada& cam = camadas[c];
            vector<float> entrada_da_camada = (c == 0) ? entrada : camadas[c - 1].ativ;
            for (int i = 0; i < cam.n_neuronios; i++) {
                for (int j = 0; j < cam.n_entradas; j++)
                    cam.pesos[i][j] += taxa * deltas[c][i] * entrada_da_camada[j];
                cam.bias[i] += taxa * deltas[c][i];
            }
        }
    }

    // ===================================================================
    // SALVAR JSON
    // ===================================================================
    void salvar(const string& caminho) const {
        json j;
        j["taxa"] = taxa;
        vector<int> arquitetura;
        if (!camadas.empty()) {
            arquitetura.push_back(camadas[0].n_entradas);
            for (const auto& cam : camadas)
                arquitetura.push_back(cam.n_neuronios);
        }
        j["arquitetura"] = arquitetura;
        j["camadas"] = json::array();
        for (const auto& cam : camadas) {
            json cam_json;
            cam_json["n_entradas"]  = cam.n_entradas;
            cam_json["n_neuronios"] = cam.n_neuronios;
            cam_json["ativacao"]    = ativacaoToString(cam.ativacao);
            cam_json["pesos"]       = cam.pesos;
            cam_json["bias"]        = cam.bias;
            j["camadas"].push_back(cam_json);
        }
        ofstream arquivo(caminho);
        if (!arquivo.is_open()) {
            cerr << "ERRO: nao conseguiu abrir '" << caminho << "' para escrita." << endl;
            return;
        }
        arquivo << j.dump(2);
        arquivo.close();
        cout << "  Pesos salvos em: " << caminho << endl;
    }

    // ===================================================================
    // CARREGAR JSON
    // ===================================================================
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
        camadas.clear();
        for (const auto& cam_json : j["camadas"]) {
            int entradas  = cam_json["n_entradas"];
            int neuronios = cam_json["n_neuronios"];
            Ativacao ativ = stringToAtivacao(cam_json.value("ativacao", "sigmoid"));
            Camada cam(entradas, neuronios, ativ);
            cam.pesos = cam_json["pesos"].get<vector<vector<float>>>();
            cam.bias  = cam_json["bias"].get<vector<float>>();
            camadas.push_back(cam);
        }
        cout << "  Pesos carregados de: " << caminho << endl;
        cout << "  Arquitetura: " << descricaoArquitetura() << endl;
    }

    // ===================================================================
    // UTILITARIOS
    // ===================================================================
    void mostrarPesos() {
        cout << fixed << setprecision(4);
        for (size_t c = 0; c < camadas.size(); c++) {
            Camada& cam  = camadas[c];
            string  tipo = (c == camadas.size() - 1) ? "SAIDA" : "OCULTA " + to_string(c);
            cout << "  --- Camada " << c << " [" << tipo << "] ("
                 << cam.n_neuronios << " neuronios, "
                 << ativacaoToString(cam.ativacao) << ") ---" << endl;
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
// FUNCAO AUXILIAR: treina e testa
// ----------------------------------------------------------------------------
int treinarETestar(MLP& rede,
                   const vector<vector<float>>& entradas,
                   const vector<vector<float>>& saidas,
                   int epocas,
                   bool mostrarProgresso = true,
                   bool classificacaoBinaria = true) {
    if (mostrarProgresso)
        cout << "--- Treinamento (" << epocas << " epocas, taxa=" << rede.taxa << ") ---" << endl;

    int n_amostras = (int)entradas.size();

    // Early stopping
    float melhorMSE = 999999.0f;
    int semMelhora  = 0;
    int paciencia   = 500;

    for (int e = 0; e < epocas; e++) {
        float erroTotal = 0.0f;
        for (int i = 0; i < n_amostras; i++) {
            rede.treinar(entradas[i], saidas[i]);
            vector<float> pred = rede.forward(entradas[i]);
            for (size_t j = 0; j < pred.size(); j++) {
                float erro = saidas[i][j] - pred[j];
                erroTotal += erro * erro;
            }
        }
        if (mostrarProgresso && e % 2000 == 0)
            cout << "  Epoca " << setw(5) << e << " | MSE: " << erroTotal << endl;

        // Verifica se melhorou
        if (erroTotal < melhorMSE - 0.0001f) {
            melhorMSE  = erroTotal;
            semMelhora = 0;
        } else {
            semMelhora++;
            if (semMelhora >= paciencia) {
                if (mostrarProgresso)
                    cout << "  Early stopping na epoca " << e << endl;
                break;
            }
        }
    }

    // --- Teste final --- (esse bloco fica igual, nao muda nada)
    if (mostrarProgresso)
        cout << "--- Teste final ---" << endl;

    int acertos = 0;
    for (int i = 0; i < n_amostras; i++) {
        vector<float> y = rede.forward(entradas[i]);
        bool acertou = false;

        if (classificacaoBinaria) {
            int pred = (y[0] >= 0.5f) ? 1 : 0;
            int esp  = (int)saidas[i][0];
            acertou  = (pred == esp);
            if (mostrarProgresso)
                cout << "  [" << (int)entradas[i][0] << "," << (int)entradas[i][1] << "]"
                     << " -> " << fixed << setprecision(4) << y[0]
                     << " -> " << pred << " (esperado: " << esp << ")"
                     << (acertou ? "  ACERTOU" : "  ERROU") << endl;
        } else {
            int pred = (int)(max_element(y.begin(), y.end()) - y.begin());
            int esp  = (int)(max_element(saidas[i].begin(), saidas[i].end()) - saidas[i].begin());
            acertou  = (pred == esp);
            if (mostrarProgresso) {
                cout << "  [" << entradas[i][0] << "," << entradas[i][1] << "] -> [";
                for (size_t k = 0; k < y.size(); k++) {
                    cout << fixed << setprecision(3) << y[k];
                    if (k < y.size() - 1) cout << ", ";
                }
                cout << "] -> classe " << pred << " (esperada: " << esp << ")"
                     << (acertou ? "  ACERTOU" : "  ERROU") << endl;
            }
        }
        if (acertou) acertos++;
    }
    return acertos;
}
// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------
int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - MLP com Leaky ReLU + Softmax (v4)" << endl;
    cout << "============================================================" << endl;

    vector<vector<float>> xor_entradas = {
        {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}
    };
    vector<vector<float>> xor_saidas = {
        {0.0f}, {1.0f}, {1.0f}, {0.0f}
    };

    // ================================================================
    // TESTE 1: XOR com Leaky ReLU (semente 42)
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  TESTE 1: XOR com Leaky ReLU" << endl;
    cout << "  Arquitetura: 2 -> 4 (Leaky ReLU) -> 1 (Sigmoid)" << endl;
    cout << "  Semente: 42" << endl;
    cout << "============================================================" << endl;
    cout << endl;

    MLP rede1({2, 4, 1}, {Ativacao::LEAKY_RELU, Ativacao::SIGMOID}, 42);
    rede1.taxa = 0.3f;
    rede1.mostrarPesos();
    cout << endl;

    int acertos1 = treinarETestar(rede1, xor_entradas, xor_saidas, 20000, true, true);
    cout << "Resultado: " << acertos1 << "/4 acertos" << endl;
    if (acertos1 == 4)
        rede1.salvar("natacha_leaky.json");

    // ================================================================
    // TESTE 2: 3 classes (semente 137 — DIFERENTE da rede1!)
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  TESTE 2: 3 classes (regioes claras)" << endl;
    cout << "  Classe 0: x baixo, y baixo" << endl;
    cout << "  Classe 1: x alto,  y baixo" << endl;
    cout << "  Classe 2: y alto   (qualquer x)" << endl;
    cout << "  Arquitetura: 2 -> 8 (Leaky ReLU) -> 3 (Softmax)" << endl;
    cout << "  Semente: 137" << endl;
    cout << "============================================================" << endl;
    cout << endl;

    vector<vector<float>> mc_entradas = {
        {0.1f, 0.1f}, {0.2f, 0.2f}, {0.3f, 0.1f}, {0.1f, 0.3f},
        {0.9f, 0.1f}, {0.8f, 0.2f}, {0.7f, 0.1f}, {0.9f, 0.3f},
        {0.1f, 0.9f}, {0.5f, 0.8f}, {0.9f, 0.7f}, {0.3f, 0.9f},
    };
    vector<vector<float>> mc_saidas = {
        {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0},
        {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0},
        {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1},
    };

    MLP rede2({2, 8, 3}, {Ativacao::LEAKY_RELU, Ativacao::SOFTMAX}, 137);
    rede2.taxa = 0.05f;
    rede2.mostrarPesos();
    cout << endl;

    int acertos2 = treinarETestar(rede2, mc_entradas, mc_saidas, 30000, true, false);
    cout << "Resultado: " << acertos2 << "/" << mc_entradas.size() << " acertos" << endl;
    if (acertos2 == (int)mc_entradas.size())
        rede2.salvar("natacha_softmax.json");

    // ================================================================
    // TESTE 3: XOR profundo (semente 999 — DIFERENTE das outras!)
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  TESTE 3: XOR profundo (2 camadas ocultas)" << endl;
    cout << "  Arquitetura: 2 -> 4 (Leaky ReLU) -> 2 (Leaky ReLU) -> 1 (Sigmoid)" << endl;
    cout << "  Semente: 999" << endl;
    cout << "============================================================" << endl;
    cout << endl;

    MLP rede3({2, 4, 2, 1}, {Ativacao::LEAKY_RELU, Ativacao::LEAKY_RELU, Ativacao::SIGMOID}, 999);
    rede3.taxa = 0.1f;
    rede3.mostrarPesos();
    cout << endl;

    int acertos3 = treinarETestar(rede3, xor_entradas, xor_saidas, 40000, true, true);
    cout << "Resultado: " << acertos3 << "/4 acertos" << endl;
    if (acertos3 == 4)
        rede3.salvar("natacha_profundo.json");

    // ================================================================
    // RESUMO
    // ================================================================
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  RESUMO DOS TESTES" << endl;
    cout << "============================================================" << endl;
    cout << "  Teste 1 (XOR + Leaky ReLU):  " << acertos1 << "/4 acertos" << endl;
    cout << "  Teste 2 (3 classes Softmax): " << acertos2 << "/" << mc_entradas.size() << " acertos" << endl;
    cout << "  Teste 3 (XOR profundo):      " << acertos3 << "/4 acertos" << endl;
    cout << endl;
    if (acertos1 == 4)
        cout << "  A Natacha domina XOR com Leaky ReLU!" << endl;
    if (acertos2 == (int)mc_saidas.size())
        cout << "  A Natacha classifica 3 classes com Softmax!" << endl;
    if (acertos3 == 4)
        cout << "  A Natacha domina redes profundas!" << endl;
    cout << "============================================================" << endl;

    return 0;
}
