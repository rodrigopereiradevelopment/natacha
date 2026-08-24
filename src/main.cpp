#include "natacha/neuronio.hpp"

#include <iostream>
#include <iomanip>
#include <vector>

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "  NATACHA — Primeiro Neuronio (OR Logico)" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;

    // Dataset: OR logico
    // Entrada [x1, x2] -> Saida esperada
    std::vector<std::vector<float>> entradas = {
        {0.0f, 0.0f},  // 0 OR 0 = 0
        {0.0f, 1.0f},  // 0 OR 1 = 1
        {1.0f, 0.0f},  // 1 OR 0 = 1
        {1.0f, 1.0f}   // 1 OR 1 = 1
    };
    std::vector<float> saidas = {0.0f, 1.0f, 1.0f, 1.0f};

    std::cout << "Dataset de treino (OR logico):" << std::endl;
    std::cout << "  [0, 0] -> 0" << std::endl;
    std::cout << "  [0, 1] -> 1" << std::endl;
    std::cout << "  [1, 0] -> 1" << std::endl;
    std::cout << "  [1, 1] -> 1" << std::endl;
    std::cout << std::endl;

    // Cria o neuronio com 2 entradas
    natacha::Neuronio neuronio(2);

    std::cout << "Pesos iniciais: ";
    for (float p : neuronio.getPesos()) {
        std::cout << std::fixed << std::setprecision(4) << p << " ";
    }
    std::cout << "| Bias: " << neuronio.getBias() << std::endl;
    std::cout << std::endl;

    // Treina por 1000 epocas
    neuronio.treinar(entradas, saidas, 1000);

    // Testa o neuronio treinado
    std::cout << "=== TESTE FINAL ===" << std::endl;
    std::cout << std::fixed << std::setprecision(4);

    for (size_t i = 0; i < entradas.size(); ++i) {
        float resultado = neuronio.forward(entradas[i]);
        int arredondado = (resultado >= 0.5f) ? 1 : 0;
        
        std::cout << "Entrada [" 
                  << static_cast<int>(entradas[i][0]) << ", "
                  << static_cast<int>(entradas[i][1]) << "]"
                  << " -> Saida: " << resultado
                  << " -> Arredondado: " << arredondado
                  << " -> Esperado: " << static_cast<int>(saidas[i])
                  << (arredondado == static_cast<int>(saidas[i]) ? " OK" : " ERRO")
                  << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Pesos finais: ";
    for (float p : neuronio.getPesos()) {
        std::cout << p << " ";
    }
    std::cout << "| Bias: " << neuronio.getBias() << std::endl;
    std::cout << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "  Natacha aprendeu OR! Proximo: XOR (MLP)" << std::endl;
    std::cout << "============================================" << std::endl;

    return 0;
}
