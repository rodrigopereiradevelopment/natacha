#include "MultiHeadAttention.hpp"
#include <iostream>

MultiHeadAttention::MultiHeadAttention(int d_model, int num_heads)
    : d_model(d_model), num_heads(num_heads) {
    if (d_model % num_heads != 0) {
        throw std::invalid_argument(
            "d_model precisa ser divisível por num_heads");
    }
    d_k = d_model / num_heads;

    // Inicializa matrizes de peso aleatoriamente (para teste)
    // Em produção, usar Xavier/Glorot initialization
    Wq = Eigen::MatrixXd::Random(d_model, d_model);
    Wk = Eigen::MatrixXd::Random(d_model, d_model);
    Wv = Eigen::MatrixXd::Random(d_model, d_model);
    Wo = Eigen::MatrixXd::Random(d_model, d_model);
}

Eigen::MatrixXd MultiHeadAttention::forward(const Eigen::MatrixXd& input) {
    int seq_len = input.rows();  // Número de tokens (ex: 3)

    // 1. Projeta a entrada para Q_global, K_global, V_global
    // input: (seq_len x d_model) * Wq: (d_model x d_model) => Q_global: (seq_len x d_model)
    Eigen::MatrixXd Q_global = input * Wq;
    Eigen::MatrixXd K_global = input * Wk;
    Eigen::MatrixXd V_global = input * Wv;

    // Vetor para guardar a saída de cada cabeça
    std::vector<Eigen::MatrixXd> heads_output;
    heads_output.reserve(num_heads);

    // 2. Para cada cabeça, fatia as dimensões e chama atenção simples
    for (int h = 0; h < num_heads; ++h) {
        // Pega o bloco de colunas da cabeça h
        int start_col = h * d_k;

        Eigen::MatrixXd Q_head = Q_global.block(0, start_col, seq_len, d_k);
        Eigen::MatrixXd K_head = K_global.block(0, start_col, seq_len, d_k);
        Eigen::MatrixXd V_head = V_global.block(0, start_col, seq_len, d_k);

        // Chama a função de atenção simples (já validada no teste anterior)
        Eigen::MatrixXd head_out = single_head_attention(Q_head, K_head, V_head);
        heads_output.push_back(head_out);
    }

    // 3. Concatena todas as cabeças horizontalmente
    // Resultado: (seq_len x d_model)
    Eigen::MatrixXd concat(seq_len, d_model);
    for (int h = 0; h < num_heads; ++h) {
        concat.block(0, h * d_k, seq_len, d_k) = heads_output[h];
    }

    // 4. Projeção final (Wo): concat x Wo => (seq_len x d_model)
    return concat * Wo;
}
