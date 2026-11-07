#ifndef OUTPUT_LAYER_HPP
#define OUTPUT_LAYER_HPP

#include <vector>
#include <cmath>
#include <random>
#include <string>
#include <cstddef>

class OutputLayer {
private:
    size_t hidden_dim;
    size_t vocab_size;

    std::vector<float> W_out;
    std::vector<float> b_out;
    std::vector<float> dW_out;
    std::vector<float> db_out;

    std::vector<float> ultimo_h;
    std::vector<float> ultimas_probs;
    std::vector<float> logits; // NOVO BUFFER

public:
    OutputLayer(size_t hidden_dim, size_t vocab_size);

    const std::vector<float>& getProbabilidades() const { return ultimas_probs; }
    void forward(const std::vector<float>& h); // MUDOU PARA VOID
    std::vector<float> backward(int palavra_alvo_id);
    void atualizarPesos(float taxa_aprendizado);
    float getProbabilidade(int palavra_alvo_id) const;
    void salvar(const std::string& caminho) const;
    bool carregar(const std::string& caminho);
};
#endif