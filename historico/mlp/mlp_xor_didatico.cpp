// ============================================================================
//  MLP DIDATICO — APRENDENDO XOR
//  Natacha - Fase 2
//
//  Objetivo: entender POR QUE precisamos de uma CAMADA OCULTA.
//
//  O PROBLEMA DO XOR:
//      0 XOR 0 = 0
//      0 XOR 1 = 1
//      1 XOR 0 = 1
//      1 XOR 1 = 0
//
//  Um unico neuronio (reta) NAO consegue separar os 1s dos 0s.
//  Precisamos de DUAS retas (dois neuronios na camada oculta)
//  que trabalham juntas para "dobrar" o espaco.
//
//  ARQUITETURA:
//
//      ENTRADA          CAMADA OCULTA         SAIDA
//       x1  x2      h1          h2             y
//        |   |       |            |            |
//        |   |-------|------------|            |
//        |   |       |            |            |
//        |   |    [sigmoid]   [sigmoid]        |
//        |   |       |            |            |
//        |   |       |------------|------------|
//        |   |                    |            |
//        |   |--------------------|            |
//        |                                [sigmoid]
//        |------------------------------------|
//
//  Cada seta = um peso. Cada neuronio = bias + soma + ativacao.
// ============================================================================

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <iomanip>

using namespace std;

// ----------------------------------------------------------------------------
// FUNCOES MATEMATICAS
// ----------------------------------------------------------------------------

// SIGMOID: transforma qualquer numero em algo entre 0 e 1
// Formato de "S" suave. Quanto mais positivo, mais proximo de 1.
// Quanto mais negativo, mais proximo de 0.
float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

// DERIVADA DA SIGMOID: usada no treino para saber "quanto ajustar"
// Truque: se 's' ja eh o resultado da sigmoid, a derivada eh s * (1 - s)
float sigmoidDerivada(float s) {
    return s * (1.0f - s);
}

// Peso aleatorio entre -1 e +1
float pesoAleatorio() {
    return (float)rand() / RAND_MAX * 2.0f - 1.0f;
}

// ----------------------------------------------------------------------------
// MLP PARA XOR
// ----------------------------------------------------------------------------
class MLP_XOR {
public:
    // ===================== PESOS =====================
    //
    // Camada OCULTA: 2 neuronios (h1 e h2), cada um recebe 2 entradas
    float w_h1_x1, w_h1_x2;   // pesos do h1
    float w_h2_x1, w_h2_x2;   // pesos do h2
    float b_h1, b_h2;          // biases dos ocultos
    //
    // Camada de SAIDA: 1 neuronio (y), recebe 2 entradas (h1 e h2)
    float w_y_h1, w_y_h2;     // pesos da saida
    float b_y;                 // bias da saida
    //
    float taxa = 0.5f;         // velocidade de aprendizado

    // Construtor: inicializa tudo aleatorio
    MLP_XOR() {
        srand(42);  // semente fixa = resultados reproduziveis

        w_h1_x1 = pesoAleatorio(); w_h1_x2 = pesoAleatorio();
        w_h2_x1 = pesoAleatorio(); w_h2_x2 = pesoAleatorio();
        b_h1 = pesoAleatorio();    b_h2 = pesoAleatorio();

        w_y_h1 = pesoAleatorio();  w_y_h2 = pesoAleatorio();
        b_y = pesoAleatorio();
    }

    // ===================== FORWARD =====================
    // A informacao flui da ESQUERDA para a DIREITA.
    //
    // Passo 1: entradas (x1, x2) chegam
    // Passo 2: cada neuronio oculto faz: soma = x1*w1 + x2*w2 + bias
    // Passo 3: aplicamos sigmoid na soma -> saida do neuronio
    // Passo 4: a saida dos ocultos vira entrada do neuronio de saida
    // Passo 5: neuronio de saida faz a mesma coisa
    // Passo 6: retornamos o resultado final
    //
    // Tambem retornamos h1 e h2 por referencia (precisamos deles no treino)
    float forward(float x1, float x2, float& h1_out, float& h2_out) {

        // ---- Camada OCULTA ----
        // h1 recebe x1 e x2, soma tudo com bias, passa na sigmoid
        float soma_h1 = x1 * w_h1_x1 + x2 * w_h1_x2 + b_h1;
        h1_out = sigmoid(soma_h1);

        // h2 faz a mesma coisa, com PESOS DIFERENTES
        float soma_h2 = x1 * w_h2_x1 + x2 * w_h2_x2 + b_h2;
        h2_out = sigmoid(soma_h2);

        // ---- Camada de SAIDA ----
        // y recebe h1 e h2 (NAO recebe x1 e x2 diretamente!)
        float soma_y = h1_out * w_y_h1 + h2_out * w_y_h2 + b_y;
        float y = sigmoid(soma_y);

        return y;
    }

    // ===================== TREINAR (BACKPROPAGATION) =====================
    //
    // A magica acontece aqui. O erro comeca na SAIDA e "volta" pela rede,
    // ajustando cada peso proporcionalmente a sua culpa no erro.
    //
    // Conceito: DELTA = erro * derivada_da_ativacao
    //
    // Se a saida errou MUITO e a sigmoid estava na "parte ingreme",
    // o delta eh grande -> ajustamos MUITO.
    // Se a saida errou POUCO ou a sigmoid estava "plana",
    // o delta eh pequeno -> ajustamos POUCO.
    //
    void treinar(float x1, float x2, float esperado) {

        float h1, h2;

        // ---- PASSO 1: FORWARD ----
        // Calculamos a saida atual da rede
        float y = forward(x1, x2, h1, h2);

        // ---- PASSO 2: ERRO NA SAIDA ----
        // Quanto a saida final errou?
        float erro_y = esperado - y;

        // ---- PASSO 3: DELTA DA CAMADA DE SAIDA ----
        // "Quanto o neuronio de saida precisa mudar?"
        // erro_y = direcao (mais ou menos)
        // sigmoidDerivada(y) = forca (quanto a sigmoid permite mudar)
        float delta_y = erro_y * sigmoidDerivada(y);

        // ---- PASSO 4: DELTAS DA CAMADA OCULTA ----
        // O erro da saida "propaga" para tras.
        // Se w_y_h1 eh grande, h1 tem mais "influencia" no erro.
        // Entao h1 precisa mudar mais.
        float delta_h1 = delta_y * w_y_h1 * sigmoidDerivada(h1);
        float delta_h2 = delta_y * w_y_h2 * sigmoidDerivada(h2);

        // ---- PASSO 5: AJUSTA PESOS DA SAIDA ----
        // Novo peso = peso antigo + (taxa * delta * entrada_que_veio)
        //
        // Se delta_y eh positivo (precisamos AUMENTAR a saida),
        // e h1 era grande, aumentamos w_y_h1.
        // Se h1 era pequeno, aumentamos menos.
        w_y_h1 += taxa * delta_y * h1;
        w_y_h2 += taxa * delta_y * h2;
        b_y    += taxa * delta_y;       // bias tem entrada = 1

        // ---- PASSO 6: AJUSTA PESOS DA CAMADA OCULTA ----
        // Mesma logica, mas agora as "entradas" sao x1 e x2 (originais)
        w_h1_x1 += taxa * delta_h1 * x1;
        w_h1_x2 += taxa * delta_h1 * x2;
        b_h1    += taxa * delta_h1;

        w_h2_x1 += taxa * delta_h2 * x1;
        w_h2_x2 += taxa * delta_h2 * x2;
        b_h2    += taxa * delta_h2;
    }

    // Mostra os pesos atuais (para curiosidade)
    void mostrarPesos() {
        cout << fixed << setprecision(4);
        cout << "  Pesos Ocultos:" << endl;
        cout << "    h1: w1=" << w_h1_x1 << " w2=" << w_h1_x2 << " bias=" << b_h1 << endl;
        cout << "    h2: w1=" << w_h2_x1 << " w2=" << w_h2_x2 << " bias=" << b_h2 << endl;
        cout << "  Pesos Saida:" << endl;
        cout << "    y:  wh1=" << w_y_h1 << " wh2=" << w_y_h2 << " bias=" << b_y << endl;
    }
};

// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------
int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - MLP DIDATICO (XOR)" << endl;
    cout << "============================================================" << endl;
    cout << endl;

    // Dataset XOR
    float entradas[4][2] = {{0,0}, {0,1}, {1,0}, {1,1}};
    float saidas[4] = {0, 1, 1, 0};

    cout << "Dataset XOR (o problema que um neuronio sozinho NAO resolve):" << endl;
    cout << "  0 XOR 0 = 0" << endl;
    cout << "  0 XOR 1 = 1" << endl;
    cout << "  1 XOR 0 = 1" << endl;
    cout << "  1 XOR 1 = 0" << endl;
    cout << endl;
    cout << "Arquitetura: 2 entradas -> 2 neuronios ocultos -> 1 saida" << endl;
    cout << endl;

    MLP_XOR rede;

    cout << "--- Pesos INICIAIS (aleatorios) ---" << endl;
    rede.mostrarPesos();
    cout << endl;

    // Testa ANTES de treinar (vai errar tudo)
    cout << "--- Teste ANTES do treino ---" << endl;
    for (int i = 0; i < 4; i++) {
        float h1, h2;
        float y = rede.forward(entradas[i][0], entradas[i][1], h1, h2);
        cout << "  [" << (int)entradas[i][0] << "," << (int)entradas[i][1]
             << "] -> " << y << " (esperado: " << (int)saidas[i] << ")" << endl;
    }
    cout << endl;

    // ===================== TREINAMENTO =====================
    int epocas = 10000;
    cout << "--- TREINAMENTO (" << epocas << " epocas) ---" << endl;

    for (int e = 0; e < epocas; e++) {
        float erroTotal = 0;

        for (int i = 0; i < 4; i++) {
            rede.treinar(entradas[i][0], entradas[i][1], saidas[i]);

            // Calcula erro para acompanhar progresso
            float h1, h2;
            float pred = rede.forward(entradas[i][0], entradas[i][1], h1, h2);
            float erro = saidas[i] - pred;
            erroTotal += erro * erro;  // MSE
        }

        // Mostra progresso a cada 1000 epocas
        if (e % 1000 == 0) {
            cout << "  Epoca " << setw(5) << e << " | Erro total: " << erroTotal << endl;
        }
    }
    cout << endl;

    cout << "--- Pesos FINAIS (apos treino) ---" << endl;
    rede.mostrarPesos();
    cout << endl;

    // ===================== TESTE FINAL =====================
    cout << "--- Teste DEPOIS do treino ---" << endl;
    cout << fixed << setprecision(4);

    int acertos = 0;
    for (int i = 0; i < 4; i++) {
        float h1, h2;
        float y = rede.forward(entradas[i][0], entradas[i][1], h1, h2);
        int arredondado = (y >= 0.5f) ? 1 : 0;

        bool acertou = (arredondado == (int)saidas[i]);
        if (acertou) acertos++;

        cout << "  [" << (int)entradas[i][0] << "," << (int)entradas[i][1] << "]"
             << " -> " << y
             << " -> arredondado: " << arredondado
             << " (esperado: " << (int)saidas[i] << ")"
             << (acertou ? "  ACERTOU" : "  ERROU")
             << endl;
    }

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Resultado: " << acertos << "/4 acertos" << endl;
    if (acertos == 4)
        cout << "  A Natacha aprendeu XOR! Proximo: modularizar em classes" << endl;
    else
        cout << "  Treine mais epocas ou ajuste a taxa de aprendizado." << endl;
    cout << "============================================================" << endl;

    return 0;
}
