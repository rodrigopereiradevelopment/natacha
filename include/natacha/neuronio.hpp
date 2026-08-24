#pragma once

#include <vector>

namespace natacha {

class Neuronio {
public:
    Neuronio(int entradas);
    double prever(const std::vector<double>& entradas);
    void treinar(const std::vector<double>& entradas, double saida_esperada, double taxa_aprendizado);

private:
    std::vector<double> pesos_;
    double vies_;

    double sigmoid(double x);
};

} // namespace natacha
