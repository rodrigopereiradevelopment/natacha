#include "natacha/neuronio.hpp"

#include <cmath>
#include <iostream>
#include <random>

namespace natacha {

// Construtor: inicializa pesos e bias com valores aleatorios pequenos
// Usamos semente fixa (42) para reprodutibilidade — essencial para debug e para
// a banca do TCC conseguir reproduzir os mesmos resultados.
// Nao usamos std::rand()/srand() porque sao previsiveis e nao sao adequados
// para inicializacao de redes neurais (veja sonarqube CWE-338).
// std::mt19937 com std::uniform_real_distribution(-0.5f, 0.5f) distribui os
// pesos iniciais de forma uniforme em [-0.5, +0.5], evitando saturar a sigmoid.
Neuronio::Neuronio(int numEntradas) {
    // Semente fixa para resultados reproduziveis
    std::mt19937 gerador(42);
    std::uniform_real_distribution<float> distribuicao(-0.5f, 0.5f);

    for (int i = 0; i < numEntradas; ++i) {
        // Valor entre -0.5 e +0.5
        pesos_.push_back(distribuicao(gerador));
    }
    bias_ = distribuicao(gerador);
}

// Forward pass: entrada * pesos + bias -> sigmoid
// Aqui computamos: saida = sigmoid(bias + sum(entrada[i] * pesos[i]))
// Cada entrada contribui com seu peso correspondente, e o bias desloca a
// curva da sigmoid para esquerda/direita.
float Neuronio::forward(const std::vector<float>& entradas) const {
    float soma = bias_;
    for (size_t i = 0; i < entradas.size(); ++i) {
        soma += entradas[i] * pesos_[i];
    }
    return sigmoid(soma);
}

// Treinamento: gradiente descendente (Delta Rule do Perceptron)
// Para cada epoca, passamos por todo o dataset e ajustamos os pesos na
// direcao que reduz o erro. A regra de atualizacao e:
//   peso_i = peso_i + taxa * erro * derivada(sigmoid) * entrada[i]
// O termo derivada(sigmoid) vem da regra da cadeia:
//   d(erro)/d(peso_i) = d(erro)/d(saida) * d(saida)/d(soma) * d(soma)/d(peso_i)
//                    = -erro * sigmoid'(soma) * entrada[i]
// O sinal menos se cancela porque definimos "erro" como (saidaEsperada - saidaPredita).
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
            // 1. Forward: calcula saida do neuronio para esta entrada
            float saidaPredita = forward(entradas[i]);

            // 2. Calcula erro: quanto a predicao esta longe do esperado
            float erro = saidas[i] - saidaPredita;
            erroTotal += erro * erro;  // MSE parcial (erro quadratico medio)

            // 3. Backpropagation: ajusta pesos e bias usando a regra do delta
            // derivada = erro * sigmoid'(saida)  (regra da cadeia)
            // Isso nos diz em qual direcao mexer cada peso para reduzir o erro.
            float derivada = erro * sigmoidDerivada(saidaPredita);

            for (size_t j = 0; j < pesos_.size(); ++j) {
                // Atualizacao do peso: move na direcao oposta ao gradiente do erro
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
// Funcao de ativacao que mapeia qualquer valor real para (0, 1).
// Usada porque e diferenciavel em todo ponto, o que e obrigatorio para
// treinamento por gradiente descendente.
float Neuronio::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

// Derivada da sigmoid: sigmoid(x) * (1 - sigmoid(x))
// Como ja temos o resultado da sigmoid (chamado de 'x' aqui por simplicidade),
// podemos calcular a derivada sem executar exp() novamente:
//   sigmoid'(x) = sigmoid(x) * (1 - sigmoid(x))
// Isso e uma otimizacao importante porque exp() e cara computacionalmente.
float Neuronio::sigmoidDerivada(float x) {
    return x * (1.0f - x);
}

} // namespace natacha
