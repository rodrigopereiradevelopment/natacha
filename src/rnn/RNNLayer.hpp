#ifndef RNN_LAYER_HPP
#define RNN_LAYER_HPP

#include "LSTMCell.hpp"
#include <vector>

class RNNLayer {
private:
    LSTMCell cell;

    // Históricos do Forward Pass
    // Precisamos guardar as entradas (x) e os estados (h, c) de cada passo
    // porque o BPTT precisa deles para calcular os gradientes
    std::vector<std::vector<float>> historico_x;
    std::vector<std::vector<float>> historico_h;
    std::vector<std::vector<float>> historico_c;

public:
    RNNLayer(size_t input_dim = 64, size_t hidden_dim = 64);

    // Processa a sequência e GUARDA o histórico para o backward
    std::vector<std::vector<float>> processarSequencia(const std::vector<std::vector<float>>& sequencia_embeddings);

    // Executa o Backpropagation Through Time (BPTT) na camada inteira
    // dh_saidas: O gradiente do erro em relação à saída (h) de cada passo
    void backward(const std::vector<std::vector<float>>& dh_saidas);

    // Atualiza os pesos da célula usando os gradientes acumulados
    void atualizarPesos(float taxa_aprendizado);
    
    // Getter para a célula (opcional, mas útil para debug)
    LSTMCell& getCell() { return cell; }
};

#endif // RNN_LAYER_HPP