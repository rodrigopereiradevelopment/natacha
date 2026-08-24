#include "natacha/neuronio.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace natacha {

// Construtor: inicializa pesos e bias com valores aleatorios pequenos
Neuronio::Neuronio(int numEntradas) {
    // Semente fixa para resultados reproduziveis (depois trocamos)
    std::srand(42);
    
    for (int i = 0; i < numEntradas; ++i) {
        // Valor entre -0.5 e +0.5
        pesos_.push_back(static_cast<float>(std::rand()) / RAND_MAX - 0.5f);
    }
    bias_ = static_cast<float>(std::rand()) / RAND_MAX - 0.5f;
}

// Forward pass: entrada * pesos + bias -> sigmoid
float Neuronio::forward(const std::vector<float>& entradas) const {
    float soma = bias_;
    for (size_t i = 0; i < entradas.size(); ++i) {
        soma += entradas[i] * pesos_[i];
    }
    return sigmoid(soma);
}

// Treinamento: gradiente descendente
void Neuronio::treinar(
    const std::vector<std::vector<float>>& entradas,
    const std::vector<float>& saidas,
    int epocas
) {
    std::cout << "=== TREINAMENTO DO NEURONIO ===" << std::endl;
    std::cout << "Epocas: " << epocas << std::endl;
    std::cout << "Taxa de aprendizado: " << taxaAprendizado_ << std::endl;
    std::cout << "--------------------------------" << std::endl;

    for (int epoca = 0; epoca < epocas; ++epoca) {
        float erroTotal = 0.0f;

        for (size_t i = 0; i < entradas.size(); ++i) {
            // 1. Forward: calcula saida
            float saidaPredita = forward(entradas[i]);

            // 2. Calcula erro
            float erro = saidas[i] - saidaPredita;
            erroTotal += erro * erro;  // MSE parcial

            // 3. Backpropagation: ajusta pesos e bias
            // Derivada do erro em relacao a cada peso
            float derivada = erro * sigmoidDerivada(saidaPredita);

            for (size_t j = 0; j < pesos_.size(); ++j) {
                pesos_[j] += taxaAprendizado_ * derivada * entradas[i][j];
            }
            bias_ += taxaAprendizado_ * derivada;
        }

        // Mostra progresso a cada 100 epocas
        if (epoca % 100 == 0 || epoca == epocas - 1) {
            std::cout << "Epoca " << epoca 
                      << " | Erro: " << erroTotal 
                      << std::endl;
        }
    }

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Treinamento finalizado!" << std::endl;
    std::cout << std::endl;
}

// Getters
std::vector<float> Neuronio::getPesos() const {
    return pesos_;
}

float Neuronio::getBias() const {
    return bias_;
}

// Sigmoid: 1 / (1 + e^(-x))
float Neuronio::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

// Derivada da sigmoid: sigmoid(x) * (1 - sigmoid(x))
// Mas como ja temos a saida da sigmoid, usamos: saida * (1 - saida)
float Neuronio::sigmoidDerivada(float x) {
    return x * (1.0f - x);
}

} // namespace natacha
