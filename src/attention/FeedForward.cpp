#include "FeedForward.hpp"
#include <cmath>

using namespace Eigen;

FeedForward::FeedForward(int d_model, int d_ff) {
    W1 = MatrixXd::Random(d_model, d_ff);
    b1 = VectorXd::Random(d_ff);
    W2 = MatrixXd::Random(d_ff, d_model);
    b2 = VectorXd::Random(d_model);
}

MatrixXd FeedForward::forward(const MatrixXd& input) const {
    // Camada 1 + ReLU
    MatrixXd hidden = input * W1;
    hidden.rowwise() += b1.transpose();
    hidden = hidden.cwiseMax(0.0); // ReLU

    // Camada 2
    MatrixXd output = hidden * W2;
    output.rowwise() += b2.transpose();
    return output;
}
