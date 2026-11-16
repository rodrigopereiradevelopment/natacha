#pragma once

#include <Eigen/Dense>

class LayerNorm {
public:
    double epsilon;
    Eigen::VectorXd gamma;
    Eigen::VectorXd beta;

    LayerNorm(int d_model, double eps = 1e-5);
    Eigen::MatrixXd forward(const Eigen::MatrixXd& input) const;
};
