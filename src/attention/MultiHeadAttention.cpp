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
    Wq = Eigen::MatrixXd::Random(d_model, d_model);
    Wk = Eigen::MatrixXd::Random(d_model, d_model);
    Wv = Eigen::MatrixXd::Random(d_model, d_model);
    Wo = Eigen::MatrixXd::Random(d_model, d_model);

    // Inicializa gradientes como zero
    dWq = Eigen::MatrixXd::Zero(d_model, d_model);
    dWk = Eigen::MatrixXd::Zero(d_model, d_model);
    dWv = Eigen::MatrixXd::Zero(d_model, d_model);
    dWo = Eigen::MatrixXd::Zero(d_model, d_model);
}

Eigen::MatrixXd MultiHeadAttention::forward(const Eigen::MatrixXd& input) {
    input_cache = input;
    int seq_len = input.rows();

    // 1. Projeta a entrada para Q_global, K_global, V_global
    Q_global_cache = input * Wq;
    K_global_cache = input * Wk;
    V_global_cache = input * Wv;

    // Vetor para guardar a saída de cada cabeça
    Q_head_cache.clear();
    K_head_cache.clear();
    V_head_cache.clear();
    A_head_cache.clear();
    heads_output_cache.clear();
    heads_output_cache.reserve(num_heads);

    // 2. Para cada cabeça, fatia as dimensões e chama atenção simples
    for (int h = 0; h < num_heads; ++h) {
        int start_col = h * d_k;

        Eigen::MatrixXd Q_head = Q_global_cache.block(0, start_col, seq_len, d_k);
        Eigen::MatrixXd K_head = K_global_cache.block(0, start_col, seq_len, d_k);
        Eigen::MatrixXd V_head = V_global_cache.block(0, start_col, seq_len, d_k);

        Q_head_cache.push_back(Q_head);
        K_head_cache.push_back(K_head);
        V_head_cache.push_back(V_head);

        // Chama a função de atenção simples
        Eigen::MatrixXd head_out = single_head_attention(Q_head, K_head, V_head);
        heads_output_cache.push_back(head_out);

        // Re-calcula A para armazenar no cache (necessário para backward)
        Eigen::MatrixXd scores = (Q_head * K_head.transpose()) / std::sqrt(static_cast<double>(d_k));
        for (int i = 0; i < seq_len; ++i) {
            for (int j = i + 1; j < seq_len; ++j) {
                scores(i, j) = -1e9;
            }
        }
        A_head_cache.push_back(softmax_estavel(scores));
    }

    // 3. Concatena todas as cabeças horizontalmente
    concat_cache.resize(seq_len, d_model);
    for (int h = 0; h < num_heads; ++h) {
        concat_cache.block(0, h * d_k, seq_len, d_k) = heads_output_cache[h];
    }

    // 4. Projeção final (Wo): concat x Wo => (seq_len x d_model)
    return concat_cache * Wo;
}

Eigen::MatrixXd MultiHeadAttention::backward(const Eigen::MatrixXd& dO) {
    int seq_len = input_cache.rows();

    // 1. Gradiente de Wo: dWo = concat^T * dO   (d_model x d_model)
    dWo = concat_cache.transpose() * dO;

    // 2. Gradiente de concat: d_concat = dO * Wo^T   (seq_len x d_model)
    Eigen::MatrixXd d_concat = dO * Wo.transpose();

    // 3. Para cada cabeça, calcula gradientes de Q, K, V
    dWq = Eigen::MatrixXd::Zero(d_model, d_model);
    dWk = Eigen::MatrixXd::Zero(d_model, d_model);
    dWv = Eigen::MatrixXd::Zero(d_model, d_model);

    // Gradiente de input (será retornado)
    Eigen::MatrixXd d_input = Eigen::MatrixXd::Zero(seq_len, d_model);

    for (int h = 0; h < num_heads; ++h) {
        int start_col = h * d_k;

        // Gradiente que entra nessa cabeça (fatia de d_concat)
        Eigen::MatrixXd dQ_head = d_concat.block(0, start_col, seq_len, d_k);

        // Chama backward da atenção simples
        Eigen::MatrixXd dQ_head_out, dK_head_out, dV_head_out;
        single_head_attention_backward(
            Q_head_cache[h],
            K_head_cache[h],
            V_head_cache[h],
            A_head_cache[h],
            dQ_head,
            dQ_head_out,
            dK_head_out,
            dV_head_out
        );

        // Espalha gradientes de volta para Wq, Wk, Wv
        // Q_head = input_cache * Wq_bloco_h  =>  dWq_bloco_h = input_cache^T * dQ_head_out
        Eigen::MatrixXd dWq_bloco = input_cache.transpose() * dQ_head_out;
        Eigen::MatrixXd dWk_bloco = input_cache.transpose() * dK_head_out;
        Eigen::MatrixXd dWv_bloco = input_cache.transpose() * dV_head_out;

        dWq.block(0, start_col, d_model, d_k) = dWq_bloco;
        dWk.block(0, start_col, d_model, d_k) = dWk_bloco;
        dWv.block(0, start_col, d_model, d_k) = dWv_bloco;

        // Gradiente de input: d_input += dQ_head_out * Wq_bloco^T + ...
        Eigen::MatrixXd Wq_bloco = Wq.block(0, start_col, d_model, d_k);
        Eigen::MatrixXd Wk_bloco = Wk.block(0, start_col, d_model, d_k);
        Eigen::MatrixXd Wv_bloco = Wv.block(0, start_col, d_model, d_k);

        d_input += dQ_head_out * Wq_bloco.transpose();
        d_input += dK_head_out * Wk_bloco.transpose();
        d_input += dV_head_out * Wv_bloco.transpose();
    }

    return d_input;
}
