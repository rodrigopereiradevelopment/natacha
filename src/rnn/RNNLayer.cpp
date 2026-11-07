#include "RNNLayer.hpp"
#include <algorithm>

RNNLayer::RNNLayer(size_t input_dim, size_t hidden_dim) 
    : cell(input_dim, hidden_dim) {}

std::vector<std::vector<float>> RNNLayer::processarSequencia(const std::vector<std::vector<float>>& sequencia_embeddings) {
    historico_x.clear();
    historico_h.clear();
    historico_c.clear();

    size_t tamanho = sequencia_embeddings.size();
    historico_x.reserve(tamanho);
    historico_h.reserve(tamanho);
    historico_c.reserve(tamanho);

    // Declara saidas_h UMA VEZ SÓ e reserva espaço
    std::vector<std::vector<float>> saidas_h;
    saidas_h.reserve(tamanho);

    size_t h_dim = cell.getHiddenDim();
    std::vector<float> h_atual(h_dim, 0.0f);
    std::vector<float> c_atual(h_dim, 0.0f);

    for (const auto& x_t : sequencia_embeddings) {
        historico_x.push_back(x_t);
        historico_h.push_back(h_atual);
        historico_c.push_back(c_atual);

        LSTMState novo_estado = cell.forward(x_t, h_atual, c_atual);
        h_atual = novo_estado.h;
        c_atual = novo_estado.c;
        
        saidas_h.push_back(h_atual);
    }

    return saidas_h;
}

void RNNLayer::backward(const std::vector<std::vector<float>>& dh_saidas) {
    // O BPTT é uma passagem de trás para frente (do último token para o primeiro)
    // Por isso usamos um loop decrescente (de T-1 até 0)
    
    size_t h_dim = cell.getHiddenDim();
    
    // Gradientes que vêm "do futuro" (passo t+1)
    std::vector<float> dh_next(h_dim, 0.0f);
    std::vector<float> dc_next(h_dim, 0.0f);

    // Loop invertido: começa na última palavra e volta até a primeira
    for (int t = (int)historico_x.size() - 1; t >= 0; --t) {
        
        // 1. Pega o erro que vem da camada de saída no passo t
        // (Se dh_saidas for menor que o histórico, usamos zero)
        std::vector<float> dh_t = dh_next;
        if (t < (int)dh_saidas.size()) {
            for (size_t i = 0; i < h_dim; ++i) {
                dh_t[i] += dh_saidas[t][i];
            }
        }

        // 2. Chama o backward da célula para o passo t
        std::vector<float> dh_prev_out, dc_prev_out;
        
        cell.backward(
            historico_x[t],   // Entrada original do passo t
            historico_h[t],   // Estado oculto ANTERIOR (t-1)
            historico_c[t],   // Estado da célula ANTERIOR (t-1)
            dh_t,             // Erro que vem do futuro
            dc_next,          // Erro do estado da célula
            dh_prev_out,      // Saída: erro para o passo t-1
            dc_prev_out       // Saída: erro para o passo t-1
        );

        // 3. Atualiza os gradientes "do futuro" para o próximo loop (t-1)
        dh_next = dh_prev_out;
        dc_next = dc_prev_out;
    }
}

void RNNLayer::atualizarPesos(float taxa_aprendizado) {
    // Depois que o BPTT terminou e todos os gradientes foram acumulados na célula,
    // nós mandamos a célula atualizar seus pesos
    cell.atualizarPesos(taxa_aprendizado);
}