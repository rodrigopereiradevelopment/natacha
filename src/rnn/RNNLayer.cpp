#include "RNNLayer.hpp"
#include <algorithm>

RNNLayer::RNNLayer(size_t input_dim, size_t hidden_dim) 
    : cell(input_dim, hidden_dim) {}

// Processa uma sequência de embeddings através da célula LSTM, um passo de
// tempo por vez. Guarda o histórico de entradas (x) e estados (h, c) de cada
// passo porque o BPTT (backpropagation through time) precisa deles para
// calcular os gradientes quando voltamos para trás na sequência.
//
// Retorna as saídas ocultas (h) de cada passo de tempo.
std::vector<std::vector<float>> RNNLayer::processarSequencia(const std::vector<std::vector<float>>& sequencia_embeddings) {
    // Limpa históricos de execuções anteriores
    historico_x.clear();
    historico_h.clear();
    historico_c.clear();

    size_t tamanho = sequencia_embeddings.size();
    // Reserve espaço para evitar realocações durante o loop
    historico_x.reserve(tamanho);
    historico_h.reserve(tamanho);
    historico_c.reserve(tamanho);

    // Declara saidas_h UMA VEZ SÓ e reserva espaço (otimização de memória)
    std::vector<std::vector<float>> saidas_h;
    saidas_h.reserve(tamanho);

    size_t h_dim = cell.getHiddenDim();
    std::vector<float> h_atual(h_dim, 0.0f);  // estado oculto inicial (zeros)
    std::vector<float> c_atual(h_dim, 0.0f);  // estado celular inicial (zeros)

    // Forward pass: para cada entrada x_t da sequência, calcula o novo estado
    for (const auto& x_t : sequencia_embeddings) {
        // Guarda o estado ANTES de processar este passo (precisamos para o BPTT)
        historico_x.push_back(x_t);
        historico_h.push_back(h_atual);
        historico_c.push_back(c_atual);

        // Avança um passo na LSTM
        LSTMState novo_estado = cell.forward(x_t, h_atual, c_atual);
        h_atual = novo_estado.h;
        c_atual = novo_estado.c;
        
        saidas_h.push_back(h_atual);
    }

    return saidas_h;
}

// Executa o Backpropagation Through Time (BPTT) na camada inteira.
//
// O BPTT desfaz o forward pass passo a passo, de trás para frente (do último
// token para o primeiro), aplicando a regra da cadeia através do tempo.
//
// Parâmetros:
//   dh_saidas : gradiente do erro em relação à saída (h) de cada passo de tempo.
//               Se dh_saidas for menor que o histórico, os passos faltantes
//               recebem gradiente zero.
void RNNLayer::backward(const std::vector<std::vector<float>>& dh_saidas) {
    // O BPTT é uma passagem de trás para frente (do último token para o primeiro)
    // Por isso usamos um loop decrescente (de T-1 até 0)
    
    size_t h_dim = cell.getHiddenDim();
    
    // Gradientes que vêm "do futuro" (passo t+1). Inicializados em zero
    // porque não há nada depois do último passo de tempo.
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
        // Isso calcula os gradientes das matrizes W, U, b para este passo
        std::vector<float> dh_prev_out, dc_prev_out;
        
        cell.backward(
            historico_x[t],   // Entrada original do passo t
            historico_h[t],   // Estado oculto ANTERIOR (t-1)
            historico_c[t],   // Estado da célula ANTERIOR (t-1)
            dh_t,             // Erro que vem do futuro (incluindo dh_saidas[t])
            dc_next,          // Erro do estado da célula
            dh_prev_out,      // Saída: erro para o passo t-1 (h)
            dc_prev_out       // Saída: erro para o passo t-1 (c)
        );

        // 3. Atualiza os gradientes "do futuro" para o próximo loop (t-1)
        // O erro que sai desta célula vira o erro de entrada para o passo anterior
        dh_next = dh_prev_out;
        dc_next = dc_prev_out;
    }
}

// Depois que o BPTT terminou e todos os gradientes foram acumulados na célula,
// nós mandamos a célula atualizar seus pesos (descida do gradiente).
void RNNLayer::atualizarPesos(float taxa_aprendizado) {
    cell.atualizarPesos(taxa_aprendizado);
}
