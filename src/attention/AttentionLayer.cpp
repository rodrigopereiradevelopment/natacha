#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <vector>

using namespace Eigen;
using namespace std;

// Função auxiliar para o Softmax (estável)
MatrixXd softmax_estavel(const MatrixXd& scores) {
    MatrixXd exp_scores(scores.rows(), scores.cols());
    for (int i = 0; i < scores.rows(); ++i) {
        double max_val = scores.row(i).maxCoeff();
        for (int j = 0; j < scores.cols(); ++j) {
            exp_scores(i, j) = std::exp(scores(i, j) - max_val);
        }
        double soma = exp_scores.row(i).sum();
        for (int j = 0; j < scores.cols(); ++j) {
            exp_scores(i, j) /= soma;
        }
    }
    return exp_scores;
}

// Forward Pass
struct Cache {
    MatrixXd Q, K, V, S, A;
    MatrixXd input;
};

MatrixXd forward(const MatrixXd& input, const MatrixXd& Wq, const MatrixXd& Wk, const MatrixXd& Wv, Cache& cache) {
    cache.input = input;
    cache.Q = input * Wq;
    cache.K = input * Wk;
    cache.V = input * Wv;

    int seq_len = input.rows();
    int d_k = cache.K.cols();

    cache.S = (cache.Q * cache.K.transpose()) / std::sqrt(static_cast<double>(d_k));

    // Máscara Causal
    for (int i = 0; i < seq_len; ++i) {
        for (int j = i + 1; j < seq_len; ++j) {
            cache.S(i, j) = -1e9;
        }
    }

    cache.A = softmax_estavel(cache.S);
    return cache.A * cache.V;
}

// Backward Pass
void backward(const MatrixXd& dO, Cache& cache, 
              MatrixXd& dWq, MatrixXd& dWk, MatrixXd& dWv) {
    // 1. Gradiente de V
    MatrixXd dV = cache.A.transpose() * dO;

    // 2. Gradiente de A
    MatrixXd dA = dO * cache.V.transpose();

    // 3. Gradiente do Softmax (dS)
    MatrixXd dS(cache.A.rows(), cache.A.cols());
    for (int i = 0; i < cache.A.rows(); ++i) {
        for (int j = 0; j < cache.A.cols(); ++j) {
            double soma = 0.0;
            for (int k = 0; k < cache.A.cols(); ++k) {
                soma += dA(i, k) * cache.A(i, k);
            }
            dS(i, j) = cache.A(i, j) * (dA(i, j) - soma);
        }
    }

    // 4. Gradiente de S (ajuste da escala)
    int d_k = cache.K.cols();
    dS = dS / std::sqrt(static_cast<double>(d_k));

    // 5. Gradientes de Q e K
    MatrixXd dQ = dS * cache.K;
    MatrixXd dK = dS.transpose() * cache.Q;

    // 6. Gradientes dos Pesos
    dWq = cache.input.transpose() * dQ;
    dWk = cache.input.transpose() * dK;
    dWv = cache.input.transpose() * dV;
}

// Teste Numérico de Gradiente
void testar_gradiente() {
    cout << "=== TESTE DE GRADIENTE NUMERICO ===" << endl;
    
    // 1. Setup
    MatrixXd input(3, 4);
    input << 1.0, 0.5, 0.1, 0.0,
             0.2, 1.0, 0.3, 0.8,
             0.0, 0.1, 1.0, 0.5;
    
    MatrixXd Wq = MatrixXd::Random(4, 4);
    MatrixXd Wk = MatrixXd::Random(4, 4);
    MatrixXd Wv = MatrixXd::Random(4, 4);

    // 2. Forward e Backward
    Cache cache;
    MatrixXd O = forward(input, Wq, Wk, Wv, cache);

    // Suponha uma perda fictícia: L = sum(O)
    MatrixXd dO = MatrixXd::Ones(O.rows(), O.cols());

    MatrixXd dWq(4, 4), dWk(4, 4), dWv(4, 4);
    backward(dO, cache, dWq, dWk, dWv);

    // 3. Teste Numérico (perturba um peso de Wq)
    double epsilon = 1e-5;
    double erro_max = 0.0;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            // Salva o valor original
            double original = Wq(i, j);

            // Perturba +epsilon
            Wq(i, j) = original + epsilon;
            Cache cache_plus;
            MatrixXd O_plus = forward(input, Wq, Wk, Wv, cache_plus);
            double L_plus = O_plus.sum();

            // Perturba -epsilon
            Wq(i, j) = original - epsilon;
            Cache cache_minus;
            MatrixXd O_minus = forward(input, Wq, Wk, Wv, cache_minus);
            double L_minus = O_minus.sum();

            // Restaura
            Wq(i, j) = original;

            // Gradiente numérico
            double grad_num = (L_plus - L_minus) / (2.0 * epsilon);
            double grad_analitico = dWq(i, j);

            double erro = std::abs(grad_num - grad_analitico);
            if (erro > erro_max) erro_max = erro;

            cout << "Wq(" << i << "," << j << "): Num=" << grad_num 
                 << " Analitico=" << grad_analitico 
                 << " Erro=" << erro << endl;
        }
    }

    cout << "\nErro maximo: " << erro_max << endl;
    if (erro_max < 1e-4) {
        cout << "[OK] Gradiente validado!" << endl;
    } else {
        cout << "[ERRO] Gradiente incorreto." << endl;
    }
}

int main() {
    testar_gradiente();
    return 0;
}