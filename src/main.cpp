#include "natacha/neuronio.hpp"
#include <iostream>
#include <vector>

int main() {
    natacha::Neuronio n(2);

    std::vector<std::vector<double>> entradas = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    std::vector<double> saidas = {0, 1, 1, 0};

    for (int epoca = 0; epoca < 10000; ++epoca) {
        for (size_t i = 0; i < entradas.size(); ++i) {
            n.treinar(entradas[i], saidas[i], 0.5);
        }
    }

    std::cout << "Resultados XOR:\n";
    for (size_t i = 0; i < entradas.size(); ++i) {
        double r = n.prever(entradas[i]);
        std::cout << entradas[i][0] << " XOR " << entradas[i][1] << " = " << r << "\n";
    }

    return 0;
}
