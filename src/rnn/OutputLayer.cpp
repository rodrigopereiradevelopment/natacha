#include "OutputLayer.hpp"
#include <algorithm>
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

OutputLayer::OutputLayer(size_t hidden_dim, size_t vocab_size)
    : hidden_dim(hidden_dim), vocab_size(vocab_size) {
    
    size_t w_size = vocab_size * hidden_dim;
    W_out.resize(w_size);
    b_out.resize(vocab_size, 0.0f);
    dW_out.resize(w_size, 0.0f);
    db_out.resize(vocab_size, 0.0f);
    
    // Inicializa buffers
    ultimo_h.resize(hidden_dim);
    ultimas_probs.resize(vocab_size);
    logits.resize(vocab_size);

    std::default_random_engine generator(42);
    float desvio_padrao = std::sqrt(2.0f / (hidden_dim + vocab_size));
    std::normal_distribution<float> distribution(0.0f, desvio_padrao);
    
    for (auto& w : W_out) w = distribution(generator);
}

void OutputLayer::forward(const std::vector<float>& h) {
    ultimo_h = h; 
    float max_logit = -1e9f;

    for (size_t i = 0; i < vocab_size; ++i) {
        float logit = b_out[i];
        for (size_t j = 0; j < hidden_dim; ++j) {
            logit += W_out[i * hidden_dim + j] * h[j];
        }
        logits[i] = logit;
        if (logit > max_logit) max_logit = logit;
    }

    float soma_exp = 0.0f;
    for (size_t i = 0; i < vocab_size; ++i) {
        ultimas_probs[i] = std::exp(logits[i] - max_logit);
        soma_exp += ultimas_probs[i];
    }
    for (size_t i = 0; i < vocab_size; ++i) {
        ultimas_probs[i] /= soma_exp;
    }
}

std::vector<float> OutputLayer::backward(int palavra_alvo_id) {
    std::vector<float> dh(hidden_dim, 0.0f);
    
    // O gradiente inicial é (p - 1) para a classe correta, e (p) para as outras
    for (size_t i = 0; i < vocab_size; ++i) {
        float erro = ultimas_probs[i];
        if (i == (size_t)palavra_alvo_id) erro -= 1.0f;
        if (std::abs(erro) < 1e-6f) continue; 

        db_out[i] += erro;
        for (size_t j = 0; j < hidden_dim; ++j) {
            dW_out[i * hidden_dim + j] += erro * ultimo_h[j];
            dh[j] += erro * W_out[i * hidden_dim + j];
        }
    }

    return dh;
}

void OutputLayer::atualizarPesos(float taxa_aprendizado) {
    for (size_t i = 0; i < W_out.size(); ++i) {
        W_out[i] -= taxa_aprendizado * dW_out[i];
        dW_out[i] = 0.0f;
    }
    for (size_t i = 0; i < b_out.size(); ++i) {
        b_out[i] -= taxa_aprendizado * db_out[i];
        db_out[i] = 0.0f;
    }
}

// getProbabilidade, salvar e carregar permanecem os mesmos...

float OutputLayer::getProbabilidade(int palavra_alvo_id) const {
    return ultimas_probs[palavra_alvo_id];
}

void OutputLayer::salvar(const std::string& caminho) const {
    json j;
    j["hidden_dim"] = hidden_dim;
    j["vocab_size"] = vocab_size;
    j["W_out"] = W_out;
    j["b_out"] = b_out;

    std::ofstream arquivo(caminho);
    if (arquivo.is_open()) {
        arquivo << j.dump(2);
        arquivo.close();
        std::cout << "  OutputLayer salva em: " << caminho << std::endl;
    }
}

bool OutputLayer::carregar(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo.is_open()) return false;

    json j;
    try {
        arquivo >> j;
    } catch (...) {
        return false;
    }
    arquivo.close();

    hidden_dim = j["hidden_dim"];
    vocab_size = j["vocab_size"];
    W_out = j["W_out"].get<std::vector<float>>();
    b_out = j["b_out"].get<std::vector<float>>();

    return true;
}