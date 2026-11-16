#include <iostream>
#include <Eigen/Dense>
#include "src/attention/TransformerBlock.hpp"

using namespace Eigen;
using namespace std;

int main() {
    cout << "=== TESTE TRANSFORMER BLOCK (PRE-NORM) ===" << endl;

    int B = 3;           // batch (3 tokens)
    int d_model = 64;    // dimensão
    int num_heads = 4;   // cabeças de atenção
    int d_ff = 256;      // feed forward

    TransformerBlock block(d_model, num_heads, d_ff);

    MatrixXd x = MatrixXd::Random(B, d_model);
    MatrixXd out = block.forward(x);

    cout << "Input shape:  " << x.rows() << "x" << x.cols() << endl;
    cout << "Output shape: " << out.rows() << "x" << out.cols() << endl;

    if (out.rows() == B && out.cols() == d_model) {
        cout << "✅ TransformerBlock OK! Shapes mantidos." << endl;
    } else {
        cout << "❌ ERRO: Shapes alterados." << endl;
        return 1;
    }

    return 0;
}
