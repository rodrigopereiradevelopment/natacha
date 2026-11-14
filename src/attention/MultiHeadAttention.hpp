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
 * Fluxo:
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
};
