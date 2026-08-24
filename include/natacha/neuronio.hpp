#pragma once

#include <vector>
#include <string>

namespace natacha {

/**
 * Perceptron simples — o primeiro tijolo da Natacha.
 * 
 * Um neuronio artificial com:
 *   - pesos (importancia de cada entrada)
 *   - bias (preferencia inicial)
 *   - funcao de ativacao sigmoid
 * 
 * Dataset de treino: OR logico
 *   [0, 0] -> 0
 *   [0, 1] -> 1
 *   [1, 0] -> 1
 *   [1, 1] -> 1
 */
class Neuronio {
public:
    /**
     * Cria um neuronio com N entradas.
     * Pesos e bias comecam aleatorios (pequenos).
     */
    explicit Neuronio(int numEntradas);

    /**
     * Calcula a saida para um conjunto de entradas.
     * Formula: sigmoid( (x1*w1 + x2*w2 + ... + xn*wn) + bias )
     */
    float forward(const std::vector<float>& entradas) const;

    /**
     * Treina o neuronio com um dataset.
     * 
     * @param entradas  Matriz de entradas (cada linha = uma amostra)
     * @param saidas    Vetor de saidas esperadas
     * @param epocas    Quantas vezes passa pelo dataset
     */
    void treinar(
        const std::vector<std::vector<float>>& entradas,
        const std::vector<float>& saidas,
        int epocas
    );

    /**
     * Retorna os pesos atuais (para debug/curiosidade).
     */
    std::vector<float> getPesos() const;
    float getBias() const;

private:
    std::vector<float> pesos_;   // um peso para cada entrada
    float bias_;                   // preferencia inicial
    float taxaAprendizado_ = 0.5f; // quao rapido aprende

    /**
     * Funcao sigmoid: mapeia qualquer valor para entre 0 e 1.
     * Formato de "S" suave — ideal para decisoes probabilisticas.
     */
    static float sigmoid(float x);

    /**
     * Derivada da sigmoid — usada no treinamento para
     * saber "quanto ajustar" cada peso.
     */
    static float sigmoidDerivada(float x);
};

} // namespace natacha
