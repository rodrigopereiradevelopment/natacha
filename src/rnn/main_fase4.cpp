#include <iostream>
#include <vector>
#include <string>
#include "../embeddings/EmbeddingManager.hpp"
#include "rnn/RNNLayer.hpp"

int main() {
    std::cout << "=== NATACHA ENGINE - TESTE FASE 4 (RNN/LSTM) ===" << std::endl;

    // 1. Carrega os Embeddings treinados com 64d gerados no último treino
    EmbeddingManager embManager;
    if (!embManager.carregarEmbeddings("dados/embeddings/natacha_embeddings.json")) {
        std::cerr << "Falha ao carregar embeddings." << std::endl;
        return 1;
    }

    // 2. Frase de teste
    std::vector<std::string> tokens = {"felix", "observa", "logs", "natacha"};
    std::vector<std::vector<float>> sequencia_vetores;

    size_t dim_detectada = 64; // Padrão novo de 64dim

    std::cout << "\nConvertendo frase em embeddings:" << std::endl;
    for (const auto& token : tokens) {
        std::vector<float> vec = embManager.obterVetor(token);
        if (!vec.empty()) {
            dim_detectada = vec.size(); 
            std::cout << " [+] Token: '" << token << "' -> Vetor dim: " << dim_detectada << std::endl;
            sequencia_vetores.push_back(vec);
        } else {
            std::cout << " [!] Token nao encontrado: " << token << " (vetor zero)" << std::endl;
            sequencia_vetores.push_back(std::vector<float>(dim_detectada, 0.0f));
        }
    }

    // 3. Inicializa a RNN/LSTM com a dimensão real dos vetores (64d)
    std::cout << "\nInicializando RNN/LSTM com " << dim_detectada << " dimensoes..." << std::endl;
    RNNLayer rnn(dim_detectada, dim_detectada);
    
    auto saidas_ocultas = rnn.processarSequencia(sequencia_vetores);

    std::cout << "\n--- Resultado da Passagem Temporal ---" << std::endl;
    for (size_t t = 0; t < saidas_ocultas.size(); ++t) {
        std::cout << "Passo t=" << t << " (" << tokens[t] << ") | Estado Oculto (dim " << saidas_ocultas[t].size() << ") [0..3]: ";
        for (int i = 0; i < 4; ++i) {
            std::cout << saidas_ocultas[t][i] << " ";
        }
        std::cout << "..." << std::endl;
    }

    std::cout << "\n✓ Fase 4 ajustada para " << dim_detectada << "d e executada com sucesso!" << std::endl;
    return 0;
}