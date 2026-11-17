#pragma once

#include <Eigen/Dense>
#include <vector>
#include <stdexcept>
#include <cmath>

#include "AttentionLayer.hpp"

/**
 * Multi-Head Attention (MHA) para o Projeto Natacha - Fase 5.
 * 
 * Implementa atenção multi-cabeça conforme descrito em "Attention Is All You Need".
 * 
 * Fluxo forward:
 *   1. Projeta input (seq_len x d_model) para Q_global, K_global, V_global
 *      usando matrizes (d_model x d_model).
 *   2. Para cada cabeça h em [0, num_heads):
 *      a. Fatia Q_global, K_global, V_global nas colunas [h*d_k, (h+1)*d_k)
 *      b. Chama single_head_attention(Q_head, K_head, V_head)
 *   3. Concatena todas as cabeças horizontalmente (seq_len x d_model)
 *   4. Aplica projeção final Wo (d_model x d_model)
 * 
 * Exemplo: d_model=64, num_heads=4 => d_k=16 por cabeça.
 */
class MultiHeadAttention {
public:
    int d_model;    // Dimensão total (ex: 64)
    int num_heads;  // Número de cabeças (ex: 4)
    int d_k;        // Dimensão por cabeça = d_model / num_heads (ex: 16)

    // Matrizes de projeção (empilhadas)
    Eigen::MatrixXd Wq;  // (d_model x d_model)
    Eigen::MatrixXd Wk;  // (d_model x d_model)
    Eigen::MatrixXd Wv;  // (d_model x d_model)
    Eigen::MatrixXd Wo;  // (d_model x d_model)

    // Caches para o backward
    Eigen::MatrixXd input_cache;        // (seq_len x d_model) - entrada original
    Eigen::MatrixXd Q_global_cache;   // (seq_len x d_model)
    Eigen::MatrixXd K_global_cache;   // (seq_len x d_model)
    Eigen::MatrixXd V_global_cache;   // (seq_len x d_model)
    std::vector<Eigen::MatrixXd> Q_head_cache;   // [num_heads] cada (seq_len x d_k)
    std::vector<Eigen::MatrixXd> K_head_cache;   // [num_heads] cada (seq_len x d_k)
    std::vector<Eigen::MatrixXd> V_head_cache;   // [num_heads] cada (seq_len x d_k)
    std::vector<Eigen::MatrixXd> A_head_cache;   // [num_heads] cada (seq_len x seq_len)
    Eigen::MatrixXd concat_cache;           // (seq_len x d_model)
    std::vector<Eigen::MatrixXd> heads_output_cache; // [num_heads] cada (seq_len x d_k)

    // Gradientes (preenchidos durante o backward)
    Eigen::MatrixXd dWq;   // (d_model x d_model)
    Eigen::MatrixXd dWk;   // (d_model x d_model)
    Eigen::MatrixXd dWv;   // (d_model x d_model)
    Eigen::MatrixXd dWo;   // (d_model x d_model)

    /**
     * Construtor. Inicializa as matrizes de peso aleatoriamente.
     * 
     * @param d_model   Dimensão do modelo (ex: 64)
     * @param num_heads Número de cabeças (ex: 4)
     */
    MultiHeadAttention(int d_model, int num_heads);

    /**
     * Forward pass da Multi-Head Attention.
     * 
     * @param input Entrada (seq_len x d_model)
     * @return      Saída (seq_len x d_model)
     */
    Eigen::MatrixXd forward(const Eigen::MatrixXd& input);

    /**
     * Backward pass da Multi-Head Attention.
     * 
     * @param dO Gradiente da saída (seq_len x d_model)
     * @return   Gradiente da entrada (seq_len x d_model)
     */
    Eigen::MatrixXd backward(const Eigen::MatrixXd& dO);
};
