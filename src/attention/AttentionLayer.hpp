#pragma once

#include <Eigen/Dense>
#include <vector>

/**
 * Função auxiliar para o Softmax (estável) - versão compatível com Eigen
 * 
 * @param scores Matriz de scores (seq_len x seq_len)
 * @return       Matriz de probabilidades (seq_len x seq_len)
 */
Eigen::MatrixXd softmax_estavel(const Eigen::MatrixXd& scores);

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

/**
 * Backward do single-head attention.
 * 
 * @param Q     Queries usadas no forward (seq_len x d_k)
 * @param K     Keys usadas no forward (seq_len x d_k)
 * @param V     Values usadas no forward (seq_len x d_v)
 * @param A     Pesos de atenção salvos (seq_len x seq_len)
 * @param dO    Gradiente da saída (seq_len x d_v)
 * @param dQ    Gradiente de Q (seq_len x d_k)  [saída]
 * @param dK    Gradiente de K (seq_len x d_k)  [saída]
 * @param dV    Gradiente de V (seq_len x d_v)  [saída]
 */
void single_head_attention_backward(
    const Eigen::MatrixXd& Q,
    const Eigen::MatrixXd& K,
    const Eigen::MatrixXd& V,
    const Eigen::MatrixXd& A,
    const Eigen::MatrixXd& dO,
    Eigen::MatrixXd& dQ,
    Eigen::MatrixXd& dK,
    Eigen::MatrixXd& dV
);

// Função original (mantida para compatibilidade)
Eigen::MatrixXd self_attention(
    const Eigen::MatrixXd& input,
    const Eigen::MatrixXd& Wq,
    const Eigen::MatrixXd& Wk,
    const Eigen::MatrixXd& Wv
);
