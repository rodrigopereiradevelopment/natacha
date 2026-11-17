#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "src/attention/LayerNorm.hpp"

using namespace Eigen;
using namespace std;

double max_abs(const MatrixXd& A) { return A.cwiseAbs().maxCoeff(); }

int main() {
    const int B = 4, d = 8;
    LayerNorm ln(d);

    // Randomiza gamma e beta pra não ficar trivial
    ln.gamma = VectorXd::Random(d).array() * 0.5 + 1.0;
    ln.beta  = VectorXd::Random(d) * 0.1;

    MatrixXd x  = MatrixXd::Random(B, d);
    MatrixXd dY = MatrixXd::Random(B, d);
    double eps = 1e-6;

    // 1. Forward e Backward Analítico
    ln.forward(x);
    MatrixXd dx_analitico = ln.backward(dY);

    // 2. Backward Numérico
    MatrixXd dx_numerico(B, d);
    for (int i = 0; i < B; ++i) {
        for (int j = 0; j < d; ++j) {
            MatrixXd xp = x, xm = x;
            xp(i,j) += eps; xm(i,j) -= eps;
            double lp = (ln.forward(xp).array() * dY.array()).sum();
            double lm = (ln.forward(xm).array() * dY.array()).sum();
            dx_numerico(i,j) = (lp - lm) / (2.0 * eps);
        }
    }

    double erro = max_abs(dx_analitico - dx_numerico);
    cout << "Erro máximo (dx): " << erro << endl;

    if (erro < 1e-6) cout << "✅ Gradiente do LayerNorm BATEU!" << endl;
    else cout << "❌ Gradiente errado, revisar fórmula." << endl;

    return 0;
}
