#pragma once

#include <Eigen/Dense>
#include "MultiHeadAttention.hpp"
#include "LayerNorm.hpp"
#include "FeedForward.hpp"

/**
 * Transformer Block (Pre-Norm) — Fase 5 do Projeto Natacha.
 * 
 * Implementa um bloco completo do Transformer com:
 *   1. Multi-Head Attention + Conexão Residual + LayerNorm (Pre-Norm)
 *   2. Feed-Forward + Conexão Residual + LayerNorm (Pre-Norm)
 * 
 * O esquema Pre-Norm aplica a LayerNorm ANTES de cada subcamada,
 * o que ajuda na estabilidade de treinamento em redes profundas.
 */
class TransformerBlock {
public:
    int d_model;      // Dimensão do modelo (ex: 64)
    int num_heads;    // Número de cabeças de atenção (ex: 4)
    int d_ff;         // Dimensão interna do Feed-Forward (ex: 256)

    // Componentes do bloco
    MultiHeadAttention attn;
    LayerNorm ln1;
    LayerNorm ln2;
    FeedForward ff;

    // Caches para o backward (usaremos depois)
    Eigen::MatrixXd x_cache;
    Eigen::MatrixXd ln1_out_cache;
    Eigen::MatrixXd attn_out_cache;
    Eigen::MatrixXd res1_cache;
    Eigen::MatrixXd ln2_out_cache;

    /**
     * Construtor. Inicializa todos os componentes com os parâmetros dados.
     * 
     * @param d_model   Dimensão do modelo (ex: 64)
     * @param num_heads Número de cabeças de atenção (ex: 4)
     * @param d_ff      Dimensão oculta do Feed-Forward (ex: 256)
     */
    TransformerBlock(int d_model, int num_heads, int d_ff);

    /**
     * Forward pass do bloco Transformer.
     * 
     * Fluxo Pre-Norm:
     *   x -> LayerNorm1 -> MultiHeadAttention -> Soma Residual -> res1
     *   res1 -> LayerNorm2 -> FeedForward -> Soma Residual -> saida
     * 
     * @param x Entrada (seq_len x d_model)
     * @return  Saída (seq_len x d_model)
     */
    Eigen::MatrixXd forward(const Eigen::MatrixXd& x);

    /**
     * Backward pass do bloco Transformer (Pre-Norm).
     * 
     * Fluxo:
     *   dY -> FF.backward -> LN2.backward -> d_res1
     *   d_res1 -> MHA.backward -> LN1.backward -> dX
     *   Com conexões residuais: dX = d_res1 + d_ln1_in
     * 
     * @param dY Gradiente da saída (seq_len x d_model)
     * @return   Gradiente da entrada (seq_len x d_model)
     */
    Eigen::MatrixXd backward(const Eigen::MatrixXd& dY);
};
