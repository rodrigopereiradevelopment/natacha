#include "natacha/neuronio.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void test_prever() {
    natacha::Neuronio n(2);
    std::vector<double> entradas = {1.0, 0.0};
    double resultado = n.prever(entradas);
    assert(resultado >= 0.0 && resultado <= 1.0);
    std::cout << "test_prever: OK\n";
}

void test_treinar() {
    natacha::Neuronio n(2);

    std::vector<std::vector<double>> entradas = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    std::vector<double> saidas = {0, 1, 1, 0};

    for (int epoca = 0; epoca < 10000; ++epoca) {
        for (size_t i = 0; i < entradas.size(); ++i) {
            n.treinar(entradas[i], saidas[i], 0.5);
        }
    }

    for (size_t i = 0; i < entradas.size(); ++i) {
        double r = n.prever(entradas[i]);
        double esperado = saidas[i];
        assert(std::abs(r - esperado) < 0.3);
    }
    std::cout << "test_treinar: OK\n";
}

int main() {
    test_prever();
    test_treinar();
    std::cout << "Todos os testes passaram!\n";
    return 0;
}
