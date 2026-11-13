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
#include <iomanip>
#include <vector>
#include <fstream>
#include <random>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Gerador compartilhado para inicializacao reproduzivel
// Usamos std::mt19937 com semente 42 para que a inicializacao de pesos seja
// deterministica — essencial para reprodutibilidade de resultados no TCC.
// Nao usamos std::rand() porque e previsível e nao adequado para redes
// neurais (sonarqube CWE-338).
namespace {
    std::mt19937 gerador(42);
    std::uniform_real_distribution<float> distribuicaoPeso(-1.0f, 1.0f);
}

// ----------------------------------------------------------------------------
// FUNCOES MATEMATICAS
// ----------------------------------------------------------------------------

// Sigmoid: 1 / (1 + e^(-x))
// Mapeia qualquer valor real para (0, 1). Funcao de ativacao diferenciavel,
// obrigatoria para treinamento por gradiente descendente.
float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

// Derivada da sigmoid: sigmoid(x) * (1 - sigmoid(x))
// Como ja temos o resultado da sigmoid (passado como parametro 's'),
// calculamos a derivada sem executar exp() novamente — otimizacao
// computacional importante porque exp() e cara.
float sigmoidDerivada(float s) {
    return s * (1.0f - s);
}

// Correcao: distribuicao uniforme em [-1, 1] via <random>
// Inicializa pesos com valores pequenos e aleatorios. O intervalo [-1, 1]
// evita saturar a sigmoid (que ficaria quase plana em valores extremos),
// permitindo que o gradiente flua durante o treinamento.
float pesoAleatorio() {
    return distribuicaoPeso(gerador);
}

// ----------------------------------------------------------------------------
// CAMADA
// ----------------------------------------------------------------------------
struct Camada {
    int n_entradas;
    int n_neuronios;
    vector<vector<float>> pesos;   // pesos[i][j] = peso do neuronio i para entrada j
    vector<float> bias;            // bias[i] = bias do neuronio i
    vector<float> soma;            // soma[i] = valor bruto antes da ativacao
    vector<float> ativacao;        // ativacao[i] = saida do neuronio i (após sigmoid)

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
    // Cada peso e sorteado de uniform([-1, 1]). O bias tambem e inicializado
    // aleatoriamente para quebrar simetria entre os neurônios da mesma camada.
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
    float taxa = 0.3f;  // Taxa de aprendizado (quao agressivo é o ajuste dos pesos)

    // ========================================================================
    // CONSTRUTOR
    // ========================================================================
    // Recebe uma arquitetura como vetor de inteiros, ex: {2, 2, 1} significa:
    // camada de entrada com 2 neuronios, camada oculta com 2, camada de saida com 1.
    // Cada par consecutivo define uma camada de pesos entre elas.
    MLP(const vector<int>& arquitetura = {}, unsigned int semente = 42) {
        if (!arquitetura.empty()) {
            gerador.seed(semente);
            for (size_t i = 0; i < arquitetura.size() - 1; i++) {
                camadas.emplace_back(arquitetura[i], arquitetura[i + 1]);
                camadas.back().inicializarAleatorio();
            }
        }
    }

    // ========================================================================
    // FORWARD
    // ========================================================================
    // Propaga a entrada atraves de todas as camadas.
    // Em cada camada, cada neuronio calcula: ativacao = sigmoid(bias + sum(entrada[j] * pesos[j]))
    // A saida de uma camada vira a entrada da proxima.
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
    // Implementa o algoritmo de retropropagação de erro (backprop) para
    // redes neurais com camadas ocultas. A ideia e:
    //  1. Fazer o forward pass para obter as ativações.
    //  2. Calcular o "delta" (erro) da camada de saída usando a derivada da sigmoid.
    //  3. Propagar o erro para trás, camada por camada, usando a regra da cadeia.
    //  4. Atualizar pesos e bias na direção oposta ao gradiente.
    //
    // O delta de um neurônio em uma camada oculta depende dos deltas dos
    // neurônios da camada seguinte aos quais ele se conecta:
    //   delta_c[j] = (sum_k delta_{c+1}[k] * pesos_{c+1}[k][j]) * sigmoid'(ativacao_c[j])
    void treinar(const vector<float>& entrada, const vector<float>& esperado) {
        forward(entrada);
        int n_camadas = (int)camadas.size();
        vector<vector<float>> deltas(n_camadas);

        // Delta da ultima camada (camada de saída)
        // Aqui o erro é direto: (esperado - predito) * sigmoid'(ativacao)
        Camada& ultima = camadas.back();
        deltas[n_camadas - 1].resize(ultima.n_neuronios);
        for (int i = 0; i < ultima.n_neuronios; i++) {
            float erro = esperado[i] - ultima.ativacao[i];
            deltas[n_camadas - 1][i] = erro * sigmoidDerivada(ultima.ativacao[i]);
        }

        // Deltas das camadas ocultas (de trás pra frente)
        // O erro de um neurônio em uma camada oculta depende dos pesos que o
        // conectam à próxima camada e dos deltas dessa próxima camada.
        for (int c = n_camadas - 2; c >= 0; c--) {
            Camada& cam_atual = camadas[c];
            Camada& cam_prox = camadas[c + 1];
            deltas[c].resize(cam_atual.n_neuronios);
            for (int j = 0; j < cam_atual.n_neuronios; j++) {
                float soma = 0.0f;
                // Soma ponderada dos deltas da camada seguinte pelos pesos
                // que conectam este neurônio a cada neurônio da próxima camada
                for (int k = 0; k < cam_prox.n_neuronios; k++)
                    soma += deltas[c + 1][k] * cam_prox.pesos[k][j];
                deltas[c][j] = soma * sigmoidDerivada(cam_atual.ativacao[j]);
            }
        }

        // Ajusta pesos e bias de cada camada
        // Atualização: peso = peso + taxa * delta * entrada_da_camada
        // Isso move os pesos na direção que reduz o erro (descida do gradiente).
        for (int c = 0; c < n_camadas; c++) {
            Camada& cam = camadas[c];
            // A entrada da primeira camada é a entrada da rede; para as demais,
            // é a ativação da camada anterior.
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
    // Isso permite que a Natacha "dorma" (fecha o programa) e "acorde" sabendo
    // os pesos treinados, sem precisar treinar novamente do zero.
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
        arquivo << j.dump(2);  // indentacao = 2 espacos (legível por humanos)
        arquivo.close();
        cout << "  Pesos salvos em: " << caminho << endl;
    }

    // ========================================================================
    // CARREGAR pesos de JSON
    // ========================================================================
    // Cria a rede a partir do JSON (sobrescreve qualquer estado anterior).
    // Usado para carregar pesos previamente treinados sem precisar retreinar.
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

        // Reconstroi as camadas a partir do JSON
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
    // Imprime os pesos e bias de cada camada de forma legível.
    // Útil para depuração e para verificar se o treinamento está funcionando.
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

    // Retorna uma string descrevendo a arquitetura, ex: "2->2->1"
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
// Tabela verdade da porta lógica XOR:
//   0 XOR 0 = 0
//   0 XOR 1 = 1
//   1 XOR 0 = 1
//   1 XOR 1 = 0
// É um problema não-linearmente separável, ou seja, um único perceptron
// não consegue resolvê-lo. Precisamos de pelo menos uma camada oculta.
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
// Treina a rede por N épocas no dataset XOR e depois testa se ela aprendeu.
// Retorna o número de acertos (de 0 a 4).
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

    // Teste final: verifica se a rede aprendeu a mapeação XOR
    if (mostrarProgresso) {
        cout << "--- Teste final ---" << endl;
    }
    int acertos = 0;
    for (int i = 0; i < 4; i++) {
        vector<float> y = rede.forward(entradasXOR[i]);
        // Arredonda a saída contínua da sigmoid para 0 ou 1 (limiar 0.5)
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
    // Demonstra que a serialização JSON funciona: salvamos os pesos treinados
    // e os carregamos em uma instância nova, sem precisar retreinar.
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
