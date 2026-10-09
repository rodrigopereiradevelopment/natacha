#ifndef LSTM_CELL_HPP
#define LSTM_CELL_HPP

#include <vector>
#include <cmath>
#include <random>

// Estrutura que segura os dois estados da LSTM a cada passo t
struct LSTMState {
    std::vector<float> h; // Estado Oculto (Saída / Memória de curto prazo)
    std::vector<float> c; // Estado da Célula (Memória de longo prazo)
};

class LSTMCell {
private:
    size_t input_dim=64; // Tamanho do vetor de embedding (ex: 32)
    size_t hidden_dim=64; // Tamanho do estado oculto (ex: 32)

    // Pesos para as 4 portas: Forget (f), Input (i), Candidate (c), Output (o)
    // Matrizes armazenadas em vetor 1D plano para otimização de cache da CPU
    std::vector<float> Wf, Wi, Wc, Wo; // Pesos da entrada x_t (hidden_dim x input_dim)
    std::vector<float> Uf, Ui, Uc, Uo; // Pesos do estado anterior h_{t-1} (hidden_dim x hidden_dim)
    std::vector<float> bf, bi, bc, bo; // Biases (hidden_dim)

    // Funções auxiliares de ativação
    float sigmoid(float x) const {
        return 1.0f / (1.0f + std::exp(-x));
    }

    void inicializarPesos();

public:
    LSTMCell(size_t input_dim = 64, size_t hidden_dim = 64);

    // Passo Forward: calcula (h_t, c_t) a partir de x_t, h_{t-1} e c_{t-1}
    LSTMState forward(const std::vector<float>& x_t, 
                      const std::vector<float>& h_prev, 
                      const std::vector<float>& c_prev);

    size_t getInputDim() const { return input_dim; }
    size_t getHiddenDim() const { return hidden_dim; }
};

#endif // LSTM_CELL_HPP
