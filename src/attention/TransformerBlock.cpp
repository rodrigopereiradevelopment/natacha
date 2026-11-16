#include "TransformerBlock.hpp"

TransformerBlock::TransformerBlock(int d_model, int num_heads, int d_ff)
    : attn(d_model, num_heads),
      ln1(d_model),
      ln2(d_model),
      ff(d_model, d_ff) {
    this->d_model = d_model;
    this->num_heads = num_heads;
    this->d_ff = d_ff;
}

Eigen::MatrixXd TransformerBlock::forward(const Eigen::MatrixXd& x) {
    // Guarda entrada para o backward (preparação para Fase 6+)
    x_cache = x;

    // 1. ATENÇÃO COM PRE-NORM E CONEXÃO RESIDUAL
    //   ln1_out = LayerNorm1(x)
    //   attn_out = MultiHeadAttention(ln1_out)
    //   res1 = x + attn_out  (conexão residual)
    ln1_out_cache = ln1.forward(x);
    attn_out_cache = attn.forward(ln1_out_cache);
    res1_cache = x + attn_out_cache;

    // 2. FEED-FORWARD COM PRE-NORM E CONEXÃO RESIDUAL
    //   ln2_out = LayerNorm2(res1)
    //   ff_out = FeedForward(ln2_out)
    ln2_out_cache = ln2.forward(res1_cache);
    Eigen::MatrixXd ff_out = ff.forward(ln2_out_cache);

    // 3. SAÍDA FINAL
    //   out = res1 + ff_out
    Eigen::MatrixXd out = res1_cache + ff_out;
    return out;
}
