#include "LSTMCell.hpp"
#include <stdexcept>
#include <cmath>

LSTMCell::LSTMCell(size_t input_dim, size_t hidden_dim)
    : input_dim(input_dim), hidden_dim(hidden_dim) {
    
    size_t w_size = hidden_dim * input_dim;
    size_t u_size = hidden_dim * hidden_dim;

    Wf.resize(w_size); Wi.resize(w_size); Wc.resize(w_size); Wo.resize(w_size);
    Uf.resize(u_size); Ui.resize(u_size); Uc.resize(u_size); Uo.resize(u_size);
    bf.resize(hidden_dim, 0.0f); bi.resize(hidden_dim, 0.0f); 
    bc.resize(hidden_dim, 0.0f); bo.resize(hidden_dim, 0.0f);

    inicializarPesos();
}

void LSTMCell::inicializarPesos() {
    std::default_random_engine generator(42); // Semente fixa para testes reproduzíveis
    std::normal_distribution<float> distribution(0.0f, 0.1f);

    auto preencher = [&](std::vector<float>& vec) {
        for (auto& val : vec) val = distribution(generator);
    };

    preencher(Wf); preencher(Wi); preencher(Wc); preencher(Wo);
    preencher(Uf); preencher(Ui); preencher(Uc); preencher(Uo);
}

LSTMState LSTMCell::forward(const std::vector<float>& x_t, 
                          const std::vector<float>& h_prev, 
                          const std::vector<float>& c_prev) {

    std::vector<float> h_next(hidden_dim, 0.0f);
    std::vector<float> c_next(hidden_dim, 0.0f);

    for (size_t i = 0; i < hidden_dim; ++i) {
        float f_gate_in = bf[i];
        float i_gate_in = bi[i];
        float c_tilde_in = bc[i];
        float o_gate_in = bo[i];

        // Multiplicação Matriz-Vetor: W * x_t
        for (size_t j = 0; j < input_dim; ++j) {
            float x_j = (j < x_t.size()) ? x_t[j] : 0.0f;
            f_gate_in += Wf[i * input_dim + j] * x_j;
            i_gate_in += Wi[i * input_dim + j] * x_j;
            c_tilde_in += Wc[i * input_dim + j] * x_j;
            o_gate_in += Wo[i * input_dim + j] * x_j;
        }

        // Multiplicação Matriz-Vetor: U * h_prev
        for (size_t j = 0; j < hidden_dim; ++j) {
            float h_j = (j < h_prev.size()) ? h_prev[j] : 0.0f;
            f_gate_in += Uf[i * hidden_dim + j] * h_j;
            i_gate_in += Ui[i * hidden_dim + j] * h_j;
            c_tilde_in += Uc[i * hidden_dim + j] * h_j;
            o_gate_in += Uo[i * hidden_dim + j] * h_j;
        }

        // Ativações das Portas
        float f_t = sigmoid(f_gate_in);
        float i_t = sigmoid(i_gate_in);
        float c_tilde = std::tanh(c_tilde_in);
        float o_t = sigmoid(o_gate_in);

        // Atualização do Estado da Célula e Estado Oculto
        float c_p = (i < c_prev.size()) ? c_prev[i] : 0.0f;
        c_next[i] = (f_t * c_p) + (i_t * c_tilde);
        h_next[i] = o_t * std::tanh(c_next[i]);
    }

    return {h_next, c_next};
}
