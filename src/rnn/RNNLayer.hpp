#ifndef RNN_LAYER_HPP
#define RNN_LAYER_HPP

#include "LSTMCell.hpp"
#include <vector>

class RNNLayer {
private:
    LSTMCell cell;

public:
    RNNLayer(size_t input_dim = 64, size_t hidden_dim = 64);

    // Processa uma sequência inteira de vetores (embeddings de uma frase)
    // Retorna a sequência de estados ocultos para cada token
    std::vector<std::vector<float>> processarSequencia(const std::vector<std::vector<float>>& sequencia_embeddings);
};

#endif // RNN_LAYER_HPP
