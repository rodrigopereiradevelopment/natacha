#pragma once

#include <Eigen/Dense>

class FeedForward {
public:
    Eigen::MatrixXd W1, W2;
    Eigen::VectorXd b1, b2;

    // Gradientes acumulados no backward (acessíveis após chamar backward)
    Eigen::MatrixXd dW1, dW2;
    Eigen::VectorXd db1, db2;

    // Caches salvos durante o forward para uso no backward
    Eigen::MatrixXd x_cache;   // entrada original (seq_len x d_model)
    Eigen::MatrixXd h_cache;   // saída da camada 1 antes da ReLU (seq_len x d_ff)
    Eigen::MatrixXd a_cache;   // saída da ReLU (seq_len x d_ff)

    FeedForward(int d_model, int d_ff);
    Eigen::MatrixXd forward(const Eigen::MatrixXd& input);
    Eigen::MatrixXd backward(const Eigen::MatrixXd& dY);
};
