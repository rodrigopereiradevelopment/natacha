#include "LayerNorm.hpp"
#include <cmath>

using namespace Eigen;

LayerNorm::LayerNorm(int d_model, double eps) : epsilon(eps) {
    gamma = VectorXd::Ones(d_model);
    beta = VectorXd::Zero(d_model);
}

MatrixXd LayerNorm::forward(const MatrixXd& input) {
    MatrixXd normalized(input.rows(), input.cols());
    xhat_cache.resize(input.rows(), input.cols());
    inv_std_cache.resize(input.rows());

    for (int i = 0; i < input.rows(); ++i) {
        double mean = input.row(i).mean();
        double var = (input.row(i).array() - mean).square().sum() / input.cols();
        double std_dev = std::sqrt(var + epsilon);
        inv_std_cache(i) = 1.0 / std_dev;

        for (int j = 0; j < input.cols(); ++j) {
            double xhat = (input(i, j) - mean) / std_dev;
            xhat_cache(i, j) = xhat;
            normalized(i, j) = xhat * gamma(j) + beta(j);
        }
    }
    return normalized;
}

MatrixXd LayerNorm::backward(const MatrixXd& dY) {
    const int B = dY.rows();
    const int N = dY.cols();

    // Gradientes de beta e gamma (vectors de tamanho d)
    VectorXd dbeta = dY.colwise().sum();
    VectorXd dgamma = (dY.array() * xhat_cache.array()).colwise().sum();

    // dxhat = dY * gamma^T (broadcast coluna)
    MatrixXd dxhat = dY.array().rowwise() * gamma.transpose().array();

    // Somas por linha para o termo de correção
    VectorXd sum_dxhat = dxhat.rowwise().sum();
    VectorXd sum_dxhat_xhat = (dxhat.array() * xhat_cache.array()).rowwise().sum();

    // dx = dxhat - mean(dxhat) - xhat * mean(dxhat * xhat)
    MatrixXd dx = dxhat;
    dx.colwise() -= sum_dxhat / (double)N;
    for (int i = 0; i < B; ++i) {
        for (int j = 0; j < N; ++j) {
            dx(i, j) -= xhat_cache(i, j) * sum_dxhat_xhat(i) / (double)N;
        }
    }
    dx = dx.array().colwise() * inv_std_cache.array();

    return dx;
}
