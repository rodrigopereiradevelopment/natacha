#ifndef LSTM_CELL_HPP
#define LSTM_CELL_HPP

#include <vector>
#include <cmath>
#include <random>
#include <string> // NOVO

struct LSTMState {
    std::vector<float> h;
    std::vector<float> c;
};

class LSTMCell {
private:
    size_t input_dim = 64;
    size_t hidden_dim = 64;

    std::vector<float> Wf, Wi, Wc, Wo;
    std::vector<float> Uf, Ui, Uc, Uo;
    std::vector<float> bf, bi, bc, bo;

    std::vector<float> dWf, dWi, dWc, dWo;
    std::vector<float> dUf, dUi, dUc, dUo;
    std::vector<float> dbf, dbi, dbc, dbo;

    float sigmoid(float x) const {
        return 1.0f / (1.0f + std::exp(-x));
    }

    void inicializarPesos();

public:
    LSTMCell(size_t input_dim = 64, size_t hidden_dim = 64);

    LSTMState forward(const std::vector<float>& x_t, 
                      const std::vector<float>& h_prev, 
                      const std::vector<float>& c_prev);

    std::vector<float> backward(
        const std::vector<float>& x_t,
        const std::vector<float>& h_prev,
        const std::vector<float>& c_prev,
        const std::vector<float>& dh_next,
        const std::vector<float>& dc_next,
        std::vector<float>& dh_prev_out,
        std::vector<float>& dc_prev_out
    );

    void atualizarPesos(float taxa_aprendizado);

    // NOVO: Salvar e Carregar
    void salvar(const std::string& caminho) const;
    bool carregar(const std::string& caminho);

    size_t getInputDim() const { return input_dim; }
    size_t getHiddenDim() const { return hidden_dim; }
};

#endif // LSTM_CELL_HPP