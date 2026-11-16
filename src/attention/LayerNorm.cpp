#include "LayerNorm.hpp"
#include <cmath>

using namespace Eigen;

LayerNorm::LayerNorm(int d_model, double eps) : epsilon(eps) {
    gamma = VectorXd::Ones(d_model);
    beta = VectorXd::Zero(d_model);
}

MatrixXd LayerNorm::forward(const MatrixXd& input) const {
    MatrixXd normalized(input.rows(), input.cols());
    for (int i = 0; i < input.rows(); ++i) {
        double mean = input.row(i).mean();
        double var = (input.row(i).array() - mean).square().sum() / input.cols();
        double std_dev = std::sqrt(var + epsilon);

        for (int j = 0; j < input.cols(); ++j) {
            normalized(i, j) = (input(i, j) - mean) / std_dev * gamma(j) + beta(j);
        }
    }
    return normalized;
}
