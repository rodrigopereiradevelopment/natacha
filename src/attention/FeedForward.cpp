#include "FeedForward.hpp"
#include <cmath>

using namespace Eigen;

FeedForward::FeedForward(int d_model, int d_ff) {
    W1 = MatrixXd::Random(d_model, d_ff);
    b1 = VectorXd::Random(d_ff);
    W2 = MatrixXd::Random(d_ff, d_model);
    b2 = VectorXd::Random(d_model);
}

MatrixXd FeedForward::forward(const MatrixXd& input) {
    // Salva entrada para o backward
    x_cache = input;

    // Camada 1 + ReLU
    MatrixXd hidden = input * W1;
    hidden.rowwise() += b1.transpose();
    h_cache = hidden;  // salva antes da ReLU
    hidden = hidden.cwiseMax(0.0); // ReLU
    a_cache = hidden;  // salva após a ReLU

    // Camada 2
    MatrixXd output = hidden * W2;
    output.rowwise() += b2.transpose();
    return output;
}

MatrixXd FeedForward::backward(const MatrixXd& dY) {
    const int B = dY.rows();
    const int d_ff = b1.size();
    const int d_model = W2.cols();

    // Gradientes de W2 e b2 (camada de saída)
    // dW2 = a_cache^T * dY   (d_ff x d_model)
    // db2 = soma_por_linha(dY)
    MatrixXd dW2 = a_cache.transpose() * dY;
    VectorXd db2 = dY.colwise().sum();

    // da = dY * W2^T   (gradiente que volta pela camada 2)
    MatrixXd da = dY * W2.transpose();

    // dh = da * (h_cache > 0)  (derivada do ReLU)
    MatrixXd dh = da.array() * (h_cache.array() > 0).cast<double>();

    // Gradientes de W1 e b1 (camada de entrada)
    // dW1 = x_cache^T * dh   (d_model x d_ff)
    // db1 = soma_por_linha(dh)
    MatrixXd dW1 = x_cache.transpose() * dh;
    VectorXd db1 = dh.colwise().sum();

    // dX = dh * W1^T   (gradiente que volta para a entrada)
    MatrixXd dX = dh * W1.transpose();

    // Armazena gradientes nos membros para acesso externo
    this->dW1 = dW1;
    this->dW2 = dW2;
    this->db1 = db1;
    this->db2 = db2;

    return dX;
}
