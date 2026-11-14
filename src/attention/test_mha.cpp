#include "MultiHeadAttention.hpp"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== TESTE MULTI-HEAD ATTENTION (FASE 5) ===" << std::endl;

    // Simula um input de 3 tokens com d_model = 64
    Eigen::MatrixXd input = Eigen::MatrixXd::Random(3, 64);

    // Cria Multi-Head Attention: 64 dimensões, 4 cabeças de 16 cada
    MultiHeadAttention mha(64, 4);

    // Forward pass
    Eigen::MatrixXd output = mha.forward(input);

    // Mostra dimensões
    std::cout << "Input shape:  " << input.rows() << " x " << input.cols() << std::endl;
    std::cout << "Output shape: " << output.rows() << " x " << output.cols() << std::endl;

    // Verifica se as dimensões estão corretas
    if (output.rows() == input.rows() && output.cols() == input.cols()) {
        std::cout << "[OK] Multi-Head Attention funcionando!" << std::endl;
        std::cout << "   d_model=" << mha.d_model
                  << ", num_heads=" << mha.num_heads
                  << ", d_k=" << mha.d_k << std::endl;
    } else {
        std::cout << "[ERRO] Dimensões incorretas!" << std::endl;
        return 1;
    }

    // Mostra um pouco do output para conferir que não é zero
    std::cout << "\nOutput (primeiras 3 linhas, primeiras 5 colunas):" << std::endl;
    std::cout << output.block(0, 0, 3, 5) << std::endl;

    std::cout << "\n=== TESTE CONCLUÍDO COM SUCESSO ===" << std::endl;
    return 0;
}
