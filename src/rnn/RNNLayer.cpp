#include "RNNLayer.hpp"

RNNLayer::RNNLayer(size_t input_dim, size_t hidden_dim) 
    : cell(input_dim, hidden_dim) {}

std::vector<std::vector<float>> RNNLayer::processarSequencia(const std::vector<std::vector<float>>& sequencia_embeddings) {
    std::vector<std::vector<float>> historico_h;
    
    size_t h_dim = cell.getHiddenDim();
    std::vector<float> h_atual(h_dim, 0.0f);
    std::vector<float> c_atual(h_dim, 0.0f);

    for (const auto& x_t : sequencia_embeddings) {
        LSTMState novo_estado = cell.forward(x_t, h_atual, c_atual);
        h_atual = novo_estado.h;
        c_atual = novo_estado.c;
        historico_h.push_back(h_atual);
    }

    return historico_h;
}
