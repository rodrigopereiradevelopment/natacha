#pragma once

#include <Eigen/Dense>

class FeedForward {
public:
    Eigen::MatrixXd W1, W2;
    Eigen::VectorXd b1, b2;

    FeedForward(int d_model, int d_ff);
    Eigen::MatrixXd forward(const Eigen::MatrixXd& input) const;
};
