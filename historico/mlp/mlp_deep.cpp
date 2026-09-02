// ============================================================================
//  MLP PROFUNDO — NATACHA FASE 2 (REFATORADO)
//
//  Antes: MLP com 1 camada oculta + 1 camada de saida (hardcoded na logica)
//  Agora:  MLP com N camadas ocultas + 1 camada de saida (totalmente generico)
//
//  A arquitetura eh definida por um vetor de inteiros:
//      {2, 4, 2, 1}  significa:
//          2 entradas -> 4 neuronios (oculta 1) -> 2 neuronios (oculta 2) -> 1 saida
//
//  Cada camada eh independente. A saida da camada[i] vira entrada da camada[i+1].
//  A ultima camada do vetor eh a camada de SAIDA.
//
//  Isso prepara o terreno para:
//      - Redes profundas (Deep Learning)
//      - Dropout (futuro)
//      - Batch Normalization (futuro)
//      - Transformer (muito futuro)
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
// CAMADA — Uma camada da rede (oculta OU saida)
// ----------------------------------------------------------------------------
// Cada camada tem:
//   - n_entradas: quantos numeros ela recebe da camada anterior
//   - n_neuronios: quantos neuronios ela tem
//   - pesos[neuronio][entrada]: cada neuronio tem um peso pra cada entrada
//   - bias[neuronio]: cada neuronio tem um bias
//   - soma[neuronio]: resultado da soma ponderada (antes da sigmoid)
//   - ativacao[neuronio]: resultado da sigmoid (o que a camada "devolve")
//
struct Camada {
    int n_entradas;
    int n_neuronios;
    vector<vector<float>> pesos;   // pesos[neuronio][entrada]
    vector<float> bias;
    vector<float> soma;            // cache: soma ponderada (pre-sigmoid)
    vector<float> ativacao;        // cache: resultado da sigmoid (pos-sigmoid)

    // Construtor: inicializa pesos e biases aleatoriamente
    Camada(int entradas, int neuronios) {
        n_entradas = entradas;
        n_neuronios = neuronios;

        pesos.resize(n_neuronios, vector<float>(n_entradas));
        bias.resize(n_neuronios);
        soma.resize(n_neuronios);
        ativacao.resize(n_neuronios);

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
    vector<Camada> camadas;  // camadas[0] = primeira oculta, camadas.back() = saida
    float taxa = 0.3f;

    // ========================================================================
    // CONSTRUTOR
    // ========================================================================
    // Recebe um vetor com a arquitetura completa.
    // Exemplo: arquitetura = {2, 4, 2, 1}
    //   -> 2 entradas (nao eh uma camada, so define o tamanho da entrada)
    //   -> Camada 0: 4 neuronios (recebe 2 entradas)
    //   -> Camada 1: 2 neuronios (recebe 4 entradas da camada anterior)
    //   -> Camada 2: 1 neuronio  (recebe 2 entradas da camada anterior) = SAIDA
    //
    MLP(const vector<int>& arquitetura, unsigned int semente = 42) {
        srand(semente);

        // Cria cada camada. A camada[i] recebe 'arquitetura[i]' entradas
        // e tem 'arquitetura[i+1]' neuronios.
        for (size_t i = 0; i < arquitetura.size() - 1; i++) {
            int entradas = arquitetura[i];
            int neuronios = arquitetura[i + 1];
            camadas.emplace_back(entradas, neuronios);
        }
    }

    // ========================================================================
    // FORWARD
    // ========================================================================
    // A informacao flui da ESQUERDA para a DIREITA, camada por camada.
    //
    //   entrada -> [camada 0] -> [camada 1] -> ... -> [camada N] -> saida
    //
    // Cada camada faz:
    //   1. soma = bias + sum(entrada * peso)
    //   2. ativacao = sigmoid(soma)
    //   3. ativacao vira entrada da proxima camada
    //
    // Guardamos 'soma' e 'ativacao' de TODAS as camadas (precisamos no treino).
    //
    vector<float> forward(const vector<float>& entrada) {
        vector<float> atual = entrada;  // comeca com a entrada do usuario

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

            atual = proxima;  // saida desta camada = entrada da proxima
        }

        return atual;  // saida da ultima camada
    }

    // ========================================================================
    // TREINAR (BACKPROPAGATION)
    // ========================================================================
    // O erro comeca na ULTIMA camada e "volta" camada por camada.
    //
    // Para cada camada (de tras pra frente):
    //   delta[i] = (soma dos deltas da proxima camada * pesos que ligam nele)
    //              * sigmoid'(ativacao_atual)
    //
    // Depois ajustamos os pesos de cada camada:
    //   peso += taxa * delta * entrada_que_veio
    //
    // A "entrada que veio" pra camada[c] eh a ativacao da camada[c-1]
    // (ou a entrada original do usuario, se c == 0).
    //
    void treinar(const vector<float>& entrada, const vector<float>& esperado) {
        // ---- PASSO 1: FORWARD ----
        // Ja preenche 'soma' e 'ativacao' de todas as camadas
        vector<float> saida = forward(entrada);

        int n_camadas = (int)camadas.size();

        // ---- PASSO 2: DELTAS (de tras pra frente) ----
        // deltas[c] = vetor de deltas da camada 'c'
        vector<vector<float>> deltas(n_camadas);

        // --- Delta da ULTIMA camada (saida) ---
        Camada& ultima = camadas.back();
        deltas[n_camadas - 1].resize(ultima.n_neuronios);
        for (int i = 0; i < ultima.n_neuronios; i++) {
            float erro = esperado[i] - ultima.ativacao[i];
            deltas[n_camadas - 1][i] = erro * sigmoidDerivada(ultima.ativacao[i]);
        }

        // --- Deltas das camadas anteriores (ocultas) ---
        // Para cada camada 'c' (de tras pra frente, exceto a ultima):
        //   delta[c][j] = sum_k( delta[c+1][k] * pesos[c+1][k][j] ) * sigmoid'(ativacao[c][j])
        //
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

        // ---- PASSO 3: AJUSTA PESOS (da primeira ate a ultima camada) ----
        // Para cada camada 'c', a "entrada" que ela recebeu foi:
        //   - Se c == 0: a entrada original do usuario
        //   - Se c > 0:  a ativacao da camada[c-1]
        //
        for (int c = 0; c < n_camadas; c++) {
            Camada& cam = camadas[c];

            // Descobre qual foi a entrada desta camada durante o forward
            vector<float> entrada_da_camada;
            if (c == 0)
                entrada_da_camada = entrada;
            else
                entrada_da_camada = camadas[c - 1].ativacao;

            for (int i = 0; i < cam.n_neuronios; i++) {
                for (int j = 0; j < cam.n_entradas; j++)
                    cam.pesos[i][j] += taxa * deltas[c][i] * entrada_da_camada[j];
                cam.bias[i] += taxa * deltas[c][i];  // bias tem entrada = 1
            }
        }
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

    // ========================================================================
    // DESCRICAO DA ARQUITETURA
    // ========================================================================
    string descricaoArquitetura() {
        string desc = to_string(camadas[0].n_entradas);
        for (size_t c = 0; c < camadas.size(); c++)
            desc += "->" + to_string(camadas[c].n_neuronios);
        return desc;
    }
};

// ----------------------------------------------------------------------------
// FUNCAO AUXILIAR: treina e testa uma rede com o dataset XOR
// ----------------------------------------------------------------------------
void testarXOR(const string& titulo, MLP& rede, int epocas) {
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  " << titulo << endl;
    cout << "  Arquitetura: " << rede.descricaoArquitetura() << endl;
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
// MAIN — Testa varias arquiteturas pra provar que eh generico
// ----------------------------------------------------------------------------
int main() {
    cout << "============================================================" << endl;
    cout << "  NATACHA - MLP PROFUNDO (N camadas ocultas)" << endl;
    cout << "============================================================" << endl;

    // Teste 1: arquitetura classica do XOR (2 entradas -> 2 ocultos -> 1 saida)
    //          Equivalente ao MLP_XOR original. Deve funcionar.
    MLP rede1({2, 2, 1});
    testarXOR("Teste 1: 1 camada oculta (classico)", rede1, 20000);

    // Teste 2: rede PROFUNDA com 2 camadas ocultas (2 -> 4 -> 2 -> 1)
    //          Mais poderosa. Deve aprender tambem.
    MLP rede2({2, 4, 2, 1});
    testarXOR("Teste 2: 2 camadas ocultas (profunda)", rede2, 20000);

    // Teste 3: rede MUITO profunda (2 -> 8 -> 4 -> 2 -> 1)
    //          Overkill pro XOR, mas prova que o codigo aguenta.
    MLP rede3({2, 8, 4, 2, 1});
    testarXOR("Teste 3: 3 camadas ocultas (muito profunda)", rede3, 20000);

    // Teste 4: SEM camada oculta (2 -> 1)
    //          Um unico neuronio linear. NAO resolve XOR.
    //          Prova que a nao-linearidade eh essencial.
    MLP rede4({2, 1});
    testarXOR("Teste 4: 0 camadas ocultas (deve FALHAR)", rede4, 20000);

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Fim dos testes." << endl;
    cout << "  Proximo: Embeddings (Fase 3) ou mais funcionalidades no MLP?" << endl;
    cout << "============================================================" << endl;

    return 0;
}
