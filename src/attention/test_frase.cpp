#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <Eigen/Dense>
#include "MultiHeadAttention.hpp"
#include "../embeddings/EmbeddingManager.hpp" // Ajuste o caminho se necessário

using namespace Eigen;
using namespace std;

int main() {
    cout << "=== TESTE MULTI-HEAD COM FRASE (FASE 5) ===" << endl;

    // 1. Carregar os embeddings do v39
    // (Simulando o EmbeddingManager - se você já tem o código, use ele)
    // Vou fazer um mock simples para o teste:
    unordered_map<string, VectorXd> vocab;
    
    // Carrega o JSON (simplificado - adapte para o seu EmbeddingManager)
    // ... (aqui você usaria o EmbeddingManager real)
    
    // Para o teste, vou criar vetores aleatórios de 64 dims para as palavras
    vector<string> frase = {"felix", "miou", "na", "cozinha"};
    int seq_len = frase.size();
    int d_model = 64;

    MatrixXd input(seq_len, d_model);
    for (int i = 0; i < seq_len; ++i) {
        // Substitua isso pelo embeddings reais do v39
        input.row(i) = VectorXd::Random(d_model);
        cout << "Token " << i << " (" << frase[i] << ") carregado." << endl;
    }

    // 2. Criar o Multi-Head Attention
    MultiHeadAttention mha(d_model, 4); // 4 cabeças de 16

    // 3. Forward Pass
    MatrixXd output = mha.forward(input);

    // 4. Mostrar o resultado
    cout << "\nInput shape: " << input.rows() << "x" << input.cols() << endl;
    cout << "Output shape: " << output.rows() << "x" << output.cols() << endl;
    
    cout << "\nPrimeiras 5 colunas do Output (por token):" << endl;
    for (int i = 0; i < seq_len; ++i) {
        cout << "Token " << i << " (" << frase[i] << "): ";
        for (int j = 0; j < 5; ++j) {
            cout << output(i, j) << " ";
        }
        cout << "..." << endl;
    }

    cout << "\n[OK] Multi-Head processou a frase!" << endl;

    return 0;
}