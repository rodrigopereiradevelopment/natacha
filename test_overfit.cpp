#include <iostream>
#include <Eigen/Dense>
#include "src/attention/TransformerBlock.hpp"

using namespace Eigen;
using namespace std;

int main() {
    cout << "=== TESTE OVERFIT TRANSFORMER BLOCK (FASE 5) ===" << endl;

    int B = 3;           // seq_len (3 tokens)
    int d_model = 16;    // dimensão pequena para teste rápido
    int num_heads = 4;   // cabeças de atenção
    int d_ff = 32;       // feed forward

    // Semente fixa para reprodutibilidade
    srand(42);

    // Cria o bloco Transformer
    TransformerBlock block(d_model, num_heads, d_ff);

    // Input aleatório (3 tokens x 64 dims)
    MatrixXd x = MatrixXd::Random(B, d_model);

    // Target aleatório (o que queremos que o bloco aprenda a produzir)
    MatrixXd target = MatrixXd::Random(B, d_model);

    double learning_rate = 0.01;
    int epocas = 500;

    cout << "Iniciando treino com " << epocas << " épocas..." << endl;
    cout << "d_model=" << d_model << ", num_heads=" << num_heads 
         << ", d_ff=" << d_ff << endl;

    double perda_inicial = 0.0;
    double perda_final = 0.0;

    for (int e = 0; e < epocas; ++e) {
        // Forward
        MatrixXd out = block.forward(x);

        // Perda: MSE entre output e target
        MatrixXd diff = out - target;
        double perda = (diff.array().square().sum()) / (B * d_model);

        if (e == 0) perda_inicial = perda;
        if (e == epocas - 1) perda_final = perda;

        // Backward: gradiente da perda MSE w.r.t. output
        // d(out)/d(out) = 2 * (out - target) / (B * d_model)
        MatrixXd dY = (2.0 * diff) / (B * d_model);

        // Backward pelo bloco
        MatrixXd dX = block.backward(dY);

        // Atualiza pesos de cada componente (SGD simples)
        // Atualiza Wq, Wk, Wv, Wo do MHA
        block.attn.Wq -= learning_rate * block.attn.dWq;
        block.attn.Wk -= learning_rate * block.attn.dWk;
        block.attn.Wv -= learning_rate * block.attn.dWv;
        block.attn.Wo -= learning_rate * block.attn.dWo;

        // Atualiza W1, W2, b1, b2 do FeedForward
        block.ff.W1 -= learning_rate * block.ff.dW1;
        block.ff.W2 -= learning_rate * block.ff.dW2;
        block.ff.b1 -= learning_rate * block.ff.db1;
        block.ff.b2 -= learning_rate * block.ff.db2;

        // Atualiza gamma, beta do LayerNorm 1
        block.ln1.gamma -= learning_rate * block.ln1.dgamma;
        block.ln1.beta -= learning_rate * block.ln1.dbeta;

        // Atualiza gamma, beta do LayerNorm 2
        block.ln2.gamma -= learning_rate * block.ln2.dgamma;
        block.ln2.beta -= learning_rate * block.ln2.dbeta;

        if (e % 100 == 0 || e == epocas - 1) {
            cout << "Época " << e << " | Perda: " << perda << endl;
        }
    }

    cout << "\n=== RESULTADO ===" << endl;
    cout << "Perda inicial: " << perda_inicial << endl;
    cout << "Perda final:   " << perda_final << endl;
    cout << "Redução:       " << (100.0 * (perda_inicial - perda_final) / perda_inicial) << "%" << endl;

    if (perda_final < perda_inicial * 0.1) {
        cout << "✅ OVERFIT FUNCIONOU! O bloco aprendeu a mapear entrada para target." << endl;
    } else {
        cout << "⚠️  Perda não caiu o suficiente. Pode precisar de mais épocas ou LR diferente." << endl;
    }

    return 0;
}
