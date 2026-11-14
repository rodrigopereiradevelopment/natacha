#pragma once

#include <Eigen/Dense>

/**
 * Single-Head Attention: recebe Q, K, V já projetados
 * e aplica máscara causal + softmax estável.
 * 
 * @param Q Matriz de queries (seq_len x d_k)
 * @param K Matriz de keys   (seq_len x d_k)
 * @param V Matriz de values (seq_len x d_v)
 * @return  Matriz de saída (seq_len x d_v)
 */
Eigen::MatrixXd single_head_attention(
    const Eigen::MatrixXd& Q,
    const Eigen::MatrixXd& K,
    const Eigen::MatrixXd& V
);

// Função original (mantida para compatibilidade)
Eigen::MatrixXd self_attention(
    const Eigen::MatrixXd& input,
    const Eigen::MatrixXd& Wq,
    const Eigen::MatrixXd& Wk,
    const Eigen::MatrixXd& Wv
);
