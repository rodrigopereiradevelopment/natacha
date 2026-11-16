#include <iostream>
#include <Eigen/Dense>
#include "src/attention/LayerNorm.hpp"
#include "src/attention/FeedForward.hpp"

using namespace Eigen;
using namespace std;

int main() {
    cout << "=== TESTE ISOLADO: LAYER NORM + FEED FORWARD ===" << endl;

    int B = 3;  // batch (3 tokens)
    int d = 64; // d_model

    MatrixXd x = MatrixXd::Random(B, d);

    LayerNorm ln(d);
    MatrixXd ln_out = ln.forward(x);

    cout << "Input shape: " << x.rows() << "x" << x.cols() << endl;
    cout << "LayerNorm output shape: " << ln_out.rows() << "x" << ln_out.cols() << endl;

    FeedForward ff(d, d * 4); // d_ff = 256
    MatrixXd ff_out = ff.forward(ln_out);

    cout << "FeedForward output shape: " << ff_out.rows() << "x" << ff_out.cols() << endl;

    if (ln_out.rows() == B && ln_out.cols() == d && ff_out.rows() == B && ff_out.cols() == d) {
        cout << "✅ TUDO CERTO! Shapes mantidos." << endl;
    } else {
        cout << "❌ ERRO: Shapes alterados." << endl;
    }
    return 0;
}
