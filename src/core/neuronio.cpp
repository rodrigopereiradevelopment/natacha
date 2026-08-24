#include "natacha/neuronio.hpp"
#include <cmath>
#include <random>

namespace natacha {

Neuronio::Neuronio(int entradas) : vies_(0.0) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-1.0, 1.0);

    pesos_.resize(entradas);
    for (auto& p : pesos_) {
        p = dist(gen);
    }
}

double Neuronio::prever(const std::vector<double>& entradas) {
    double soma = vies_;
    for (size_t i = 0; i < entradas.size(); ++i) {
        soma += entradas[i] * pesos_[i];
    }
    return sigmoid(soma);
}

void Neuronio::treinar(const std::vector<double>& entradas, double saida_esperada, double taxa_aprendizado) {
    double saida = prever(entradas);
    double erro = saida_esperada - saida;
    double delta = erro * saida * (1.0 - saida);

    for (size_t i = 0; i < pesos_.size(); ++i) {
        pesos_[i] += taxa_aprendizado * delta * entradas[i];
    }
    vies_ += taxa_aprendizado * delta;
}

double Neuronio::sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

} // namespace natacha
