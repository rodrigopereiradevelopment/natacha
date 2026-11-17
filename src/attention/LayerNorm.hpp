#pragma once

#include <Eigen/Dense>

class LayerNorm {
public:
    double epsilon;
    Eigen::VectorXd gamma;
    Eigen::VectorXd beta;

    // Gradientes (preenchidos durante o backward)
    Eigen::VectorXd dgamma;
    Eigen::VectorXd dbeta;

    // Caches salvos durante o forward para uso no backward
    Eigen::MatrixXd xhat_cache;   // x_hat normalizado (para backward)
    Eigen::VectorXd inv_std_cache;// 1/sqrt(var + epsilon) por linha

    LayerNorm(int d_model, double eps = 1e-5);
    Eigen::MatrixXd forward(const Eigen::MatrixXd& input);
    Eigen::MatrixXd backward(const Eigen::MatrixXd& dY);
};
