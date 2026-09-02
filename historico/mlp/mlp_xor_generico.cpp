// ============================================================================
//  MLP GENERICO — NATACHA FASE 2
//
//  Antes: MLP_XOR hardcoded (2 entradas, 2 ocultos, 1 saida)
//  Agora:  MLP configuravel (N entradas, M ocultos, K saidas)
//
//  A ideia eh a MESMA, so que agora usamos vetores e loops
//  em vez de variaveis com nome (w_h1_x1, w_h2_x2...).
//
//  Cada neuronio ainda faz: soma = bias + sum(entrada * peso)
//  Cada camada ainda passa pela sigmoid.
//  O backpropagation ainda propaga o erro de tras pra frente.
//
//  A diferenca: agora a rede NAO sabe que eh um XOR.
//  Ela so sabe que recebe vetores, processa, e aprende.
// ============================================================================

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <vector>

using namespace std;

// ----------------------------------------------------------------------------
// FUNCOES MATEMATICAS
// ----------------------------------------------------------------------------

float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

// Se 's' ja eh o resultado da sigmoid, a derivada eh s * (1 - s)
float sigmoidDerivada(float s) {
    return s * (1.0f - s);
}

float pesoAleatorio() {
    return (float)rand() / RAND_MAX * 2.0f - 1.0f;
}

// ----------------------------------------------------------------------------
// MLP GENERICO
// ----------------------------------------------------------------------------
class MLP {
public:
    int n_entradas;
    int n_ocultos;
    int n_saidas;
    float taxa = 0.3f;   // velocidade de aprendizado (ajustavel)

    // pesos_oculta[neuronio_oculto][entrada]
    // Cada neuronio da camada oculta tem um peso pra CADA entrada
    vector<vector<float>> pesos_oculta;
    vector<float> bias_oculta;

    // pesos_saida[neuronio_saida][neuronio_oculto]
    // Cada neuronio da camada de saida tem um peso pra CADA oculto
    vector<vector<float>> pesos_saida;
    vector<float> bias_saida;

    // Guardamos as ativacoes da ultima forward (precisamos no treino)
    // Esses vetores sao preenchidos em forward() e lidos em treinar()
    vector<float> ultima_oculta;
    vector<float> ultima_saida;

    // ========================================================================
    // CONSTRUTOR
    // ========================================================================
    MLP(int entradas, int ocultos, int saidas, unsigned int semente = 42) {
        srand(semente);  // semente fixa = resultados reproduziveis

        n_entradas = entradas;
        n_ocultos  = ocultos;
        n_saidas   = saidas;

        // --- Camada OCULTA ---
        // Cada um dos 'ocultos' neuronios tem 'entradas' pesos + 1 bias
        pesos_oculta.resize(n_ocultos, vector<float>(n_entradas));
        bias_oculta.resize(n_ocultos);
        for (int i = 0; i < n_ocultos; i++) {
            for (int j = 0; j < n_entradas; j++)
                pesos_oculta[i][j] = pesoAleatorio();
            bias_oculta[i] = pesoAleatorio();
        }

        // --- Camada de SAIDA ---
        // Cada um dos 'saidas' neuronios tem 'ocultos' pesos + 1 bias
        pesos_saida.resize(n_saidas, vector<float>(n_ocultos));
        bias_saida.resize(n_saidas);
        for (int i = 0; i < n_saidas; i++) {
            for (int j = 0; j < n_ocultos; j++)
                pesos_saida[i][j] = pesoAleatorio();
            bias_saida[i] = pesoAleatorio();
        }

        // Vetores de cache para o treino
        ultima_oculta.resize(n_ocultos);
        ultima_saida.resize(n_saidas);
    }

    // ========================================================================
    // FORWARD
    // ========================================================================
    // Recebe um vetor de entradas e devolve um vetor de saidas.
    // Guarda as ativacoes internas (oculta e saida) pra usar no backprop.
    //
    // Fluxo:
    //   entrada -> [oculta: soma + sigmoid] -> [saida: soma + sigmoid] -> saida
    //
    vector<float> forward(const vector<float>& entrada) {
        // ---- Camada OCULTA ----
        for (int i = 0; i < n_ocultos; i++) {
            float soma = bias_oculta[i];
            for (int j = 0; j < n_entradas; j++)
                soma += entrada[j] * pesos_oculta[i][j];
            ultima_oculta[i] = sigmoid(soma);
        }

        // ---- Camada de SAIDA ----
        for (int i = 0; i < n_saidas; i++) {
            float soma = bias_saida[i];
            for (int j = 0; j < n_ocultos; j++)
                soma += ultima_oculta[j] * pesos_saida[i][j];
            ultima_saida[i] = sigmoid(soma);
        }

        return ultima_saida;
    }

    // ========================================================================
    // TREINAR (BACKPROPAGATION)
    // ========================================================================
    // Recebe um vetor de entrada e um vetor de respostas esperadas.
    // Faz forward, calcula o erro, e ajusta TODOS os pesos.
    //
    // A logica eh a mesma do XOR, so que agora com loops:
    //
    //   1. Forward -> guarda ativacoes
    //   2. Erro na saida = esperado - obtido
    //   3. Delta da saida = erro * sigmoid'(saida)
    //   4. Delta da oculta = (soma dos deltas_saida * pesos que ligam nele) * sigmoid'(oculta)
    //   5. Ajusta pesos da saida
    //   6. Ajusta pesos da oculta
    //
    void treinar(const vector<float>& entrada, const vector<float>& esperado) {
        // ---- PASSO 1: FORWARD ----
        // Ja preenche ultima_oculta e ultima_saida
        forward(entrada);

        // ---- PASSO 2 & 3: DELTAS DA CAMADA DE SAIDA ----
        // delta_saida[i] = (esperado[i] - saida[i]) * sigmoid'(saida[i])
        vector<float> delta_saida(n_saidas);
        for (int i = 0; i < n_saidas; i++) {
            float erro = esperado[i] - ultima_saida[i];
            delta_saida[i] = erro * sigmoidDerivada(ultima_saida[i]);
        }

        // ---- PASSO 4: DELTAS DA CAMADA OCULTA ----
        // Cada neuronio oculto 'j' recebe uma parcela do erro de CADA saida 'i'.
        // Quanto maior o peso que liga oculto[j] -> saida[i], mais culpa ele tem.
        //
        // delta_oculta[j] = (sum_i delta_saida[i] * pesos_saida[i][j]) * sigmoid'(oculta[j])
        //
        vector<float> delta_oculta(n_ocultos);
        for (int j = 0; j < n_ocultos; j++) {
            float soma = 0.0f;
            for (int i = 0; i < n_saidas; i++)
                soma += delta_saida[i] * pesos_saida[i][j];
            delta_oculta[j] = soma * sigmoidDerivada(ultima_oculta[j]);
        }

        // ---- PASSO 5: AJUSTA PESOS DA SAIDA ----
        // peso_saida[i][j] += taxa * delta_saida[i] * ativacao_oculta[j]
        for (int i = 0; i < n_saidas; i++) {
            for (int j = 0; j < n_ocultos; j++)
                pesos_saida[i][j] += taxa * delta_saida[i] * ultima_oculta[j];
            bias_saida[i] += taxa * delta_saida[i];  // bias tem entrada = 1
        }

        // ---- PASSO 6: AJUSTA PESOS DA CAMADA OCULTA ----
        // peso_oculta[j][k] += taxa * delta_oculta[j] * entrada[k]
        for (int j = 0; j < n_ocultos; j++) {
            for (int k = 0; k < n_entradas; k++)
                pesos_oculta[j][k] += taxa * delta_oculta[j] * entrada[k];
            bias_oculta[j] += taxa * delta_oculta[j];  // bias tem entrada = 1
        }
    }

    // ========================================================================
    // MOSTRAR PESOS (debug/curiosidade)
    // ========================================================================
    void mostrarPesos() {
        cout << fixed << setprecision(4);
        cout << "  --- Camada OCULTA (" << n_ocultos << " neuronios) ---" << endl;
        for (int i = 0; i < n_ocultos; i++) {
            cout << "    h" << i << ": ";
            for (int j = 0; j < n_entradas; j++)
                cout << "w" << j << "=" << pesos_oculta[i][j] << " ";
            cout << "bias=" << bias_oculta[i] << endl;
        }
        cout << "  --- Camada de SAIDA (" << n_saidas << " neuronios) ---" << endl;
        for (int i = 0; i < n_saidas; i++) {
            cout << "    y" << i << ": ";
            for (int j = 0; j < n_ocultos; j++)
                cout << "wh" << j << "=" << pesos_saida[i][j] << " ";
            cout << "bias=" << bias_saida[i] << endl;
        }
    }
};

// ----------------------------------------------------------------------------
// FUNCAO AUXILIAR: treina e testa uma rede com o dataset XOR
// ----------------------------------------------------------------------------
void testarXOR(const string& titulo, MLP& rede, int epocas) {
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  " << titulo << endl;
    cout << "  Arquitetura: " << rede.n_entradas << "->" << rede.n_ocultos << "->" << rede.n_saidas << endl;
    cout << "============================================================" << endl;
    cout << endl;

    // Dataset XOR
    vector<vector<float>> entradas = {
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f}
    };
    vector<vector<float>> saidas = {
        {0.0f},
        {1.0f},
        {1.0f},
        {0.0f}
    };

    cout << "--- Pesos INICIAIS ---" << endl;
    rede.mostrarPesos();
    cout << endl;

    // Testa ANTES de treinar
    cout << "--- Teste ANTES do treino ---" << endl;
    for (int i = 0; i < 4; i++) {
        vector<float> y = rede.forward(entradas[i]);
        cout << "  [" << (int)entradas[i][0] << "," << (int)entradas[i][1]
             << "] -> " << y[0] << " (esperado: " << (int)saidas[i][0] << ")" << endl;
    }
    cout << endl;

    // ===================== TREINAMENTO =====================
    cout << "--- TREINAMENTO (" << epocas << " epocas, taxa=" << rede.taxa << ") ---" << endl;

    for (int e = 0; e < epocas; e++) {
        float erroTotal = 0.0f;

        for (int i = 0; i < 4; i++) {
            rede.treinar(entradas[i], saidas[i]);

            // Calcula erro pra acompanhar progresso
            vector<float> pred = rede.forward(entradas[i]);
            for (size_t j = 0; j < pred.size(); j++) {
                float erro = saidas[i][j] - pred[j];
                erroTotal += erro * erro;
            }
        }

        if (e % 2000 == 0) {
            cout << "  Epoca " << setw(5) << e << " | Erro total (MSE): " << erroTotal << endl;
        }
    }
    cout << endl;

    cout << "--- Pesos FINAIS ---" << endl;
    rede.mostrarPesos();
    cout << endl;

    // ===================== TESTE FINAL =====================
    cout << "--- Teste DEPOIS do treino ---" << endl;
    cout << fixed << setprecision(4);

    int acertos = 0;
    for (int i = 0; i < 4; i++) {
        vector<float> y = rede.forward(entradas[i]);
        int arredondado = (y[0] >= 0.5f) ? 1 : 0;
        bool acertou = (arredondado == (int)saidas[i][0]);
        if (acertou) acertos++;

        cout << "  [" << (int)entradas[i][0] << "," << (int)entradas[i][1] << "]"
             << " -> " << y[0]
             << " -> arredondado: " << arredondado
             << " (esperado: " << (int)saidas[i][0] << ")"
             << (acertou ? "  ACERTOU" : "  ERROU")
             << endl;
    }

    cout << endl;
    cout << "  Resultado: " << acertos << "/4 acertos" << endl;
    if (acertos == 4)
        cout << "  A Natacha aprendeu XOR!" << endl;
    else
        cout << "  Treine mais epocas ou ajuste a taxa de aprendizado." << endl;
}

// ----------------------------------------------------------------------------
// MAIN — Testa DUAS arquiteturas diferentes pra provar que eh generico
// ----------------------------------------------------------------------------
int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - MLP GENERICO" << endl;
    cout << "============================================================" << endl;

    // Teste 1: arquitetura classica do XOR (2-2-1)
    MLP rede1(2, 2, 1);
    testarXOR("Teste 1: 2 entradas -> 2 ocultos -> 1 saida", rede1, 20000);

    // Teste 2: arquitetura com MAIS neuronios ocultos (2-4-1)
    // A rede eh mais "poderosa" — deve aprender mais rapido ou com menos epocas
    MLP rede2(2, 4, 1);
    testarXOR("Teste 2: 2 entradas -> 4 ocultos -> 1 saida", rede2, 20000);

    // Teste 3: arquitetura com MENOS neuronios ocultos (2-1-1)
    // Isso NAO deve aprender XOR (um unico neuronio nao resolve XOR)
    // Mas a rede generica TENTA anyway — eh um bom teste de limitacao
    MLP rede3(2, 1, 1);
    testarXOR("Teste 3: 2 entradas -> 1 oculto -> 1 saida (deve FALHAR)", rede3, 20000);

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Fim dos testes. Proximo passo: adicionar mais camadas!" << endl;
    cout << "============================================================" << endl;

    return 0;
}
