#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "src/attention/FeedForward.hpp"

using namespace Eigen;
using namespace std;

double max_abs(const MatrixXd& A) { return A.cwiseAbs().maxCoeff(); }

int main() {
    const int B = 4, d_model = 8, d_ff = 16;
    FeedForward ff(d_model, d_ff);

    // Randomiza pesos para não ficar trivial
    ff.W1 = MatrixXd::Random(d_model, d_ff).array() * 0.5;
    ff.b1 = VectorXd::Random(d_ff) * 0.1;
    ff.W2 = MatrixXd::Random(d_ff, d_model).array() * 0.5;
    ff.b2 = VectorXd::Random(d_model) * 0.1;

    MatrixXd x  = MatrixXd::Random(B, d_model);
    MatrixXd dY = MatrixXd::Random(B, d_model);
    double eps = 1e-6;

    // 1. Forward e Backward Analítico
    ff.forward(x);
    MatrixXd dx_analitico = ff.backward(dY);

    // 2. Backward Numérico
    MatrixXd dx_numerico(B, d_model);
    for (int i = 0; i < B; ++i) {
        for (int j = 0; j < d_model; ++j) {
            MatrixXd xp = x, xm = x;
            xp(i,j) += eps; xm(i,j) -= eps;
            double lp = (ff.forward(xp).array() * dY.array()).sum();
            double lm = (ff.forward(xm).array() * dY.array()).sum();
            dx_numerico(i,j) = (lp - lm) / (2.0 * eps);
        }
    }

    double erro = max_abs(dx_analitico - dx_numerico);
    cout << "Erro máximo (dx): " << erro << endl;

    if (erro < 1e-6) cout << "✅ Gradiente do FeedForward BATEU!" << endl;
    else cout << "❌ Gradiente errado, revisar fórmula." << endl;

    return 0;
}
