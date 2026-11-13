#include "OutputLayer.hpp"
#include <algorithm>
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// Construtor da camada de saída.
// Mapeia o estado oculto da LSTM (hidden_dim) para o vocabulário (vocab_size).
// Cada palavra do vocabulário tem um peso separado: W_out[palavra_id][hidden_dim].
// Inicialização de Xavier: desvio padrão = sqrt(2 / (hidden_dim + vocab_size)).
OutputLayer::OutputLayer(size_t hidden_dim, size_t vocab_size)
    : hidden_dim(hidden_dim), vocab_size(vocab_size) {
    
    size_t w_size = vocab_size * hidden_dim;
    W_out.resize(w_size);
    b_out.resize(vocab_size, 0.0f);
    dW_out.resize(w_size, 0.0f);
    db_out.resize(vocab_size, 0.0f);
    
    // Buffers reutilizados entre forward/backward para evitar alocações repetidas
    ultimo_h.resize(hidden_dim);
    ultimas_probs.resize(vocab_size);
    logits.resize(vocab_size);

    // Inicialização de Xavier para os pesos de saída
    std::default_random_engine generator(42);
    float desvio_padrao = std::sqrt(2.0f / (hidden_dim + vocab_size));
    std::normal_distribution<float> distribution(0.0f, desvio_padrao);
    
    for (auto& w : W_out) w = distribution(generator);
}

// Forward pass da camada de saída.
// Calcula os logits (pontuações brutas) para cada palavra do vocabulário:
//   logit[i] = b_out[i] + sum_j (W_out[i][j] * h[j])
//
// Depois aplica softmax para converter logits em probabilidades:
//   prob[i] = exp(logit[i] - max_logit) / sum(exp(logit[k] - max_logit))
//
// O subtração de max_logit é uma otimização numérica para evitar overflow
// no exp() quando os logits são grandes (softmax estável numericamente).
void OutputLayer::forward(const std::vector<float>& h) {
    ultimo_h = h; 
    float max_logit = -1e9f;

    // Calcula logits: produto matriz-vetor entre W_out e h
    for (size_t i = 0; i < vocab_size; ++i) {
        float logit = b_out[i];
        for (size_t j = 0; j < hidden_dim; ++j) {
            logit += W_out[i * hidden_dim + j] * h[j];
        }
        logits[i] = logit;
        if (logit > max_logit) max_logit = logit;
    }

    // Softmax estável numericamente
    float soma_exp = 0.0f;
    for (size_t i = 0; i < vocab_size; ++i) {
        ultimas_probs[i] = std::exp(logits[i] - max_logit);
        soma_exp += ultimas_probs[i];
    }
    for (size_t i = 0; i < vocab_size; ++i) {
        ultimas_probs[i] /= soma_exp;
    }
}

// Backpropagation da camada de saída.
//
// Usamos perda de entropia cruzada (cross-entropy) combinada com softmax.
// A vantagem é que o gradiente simplifica para:
//   erro[i] = prob[i] - 1  se i == palavra_alvo
//   erro[i] = prob[i]      caso contrário
//
// Isso vem da derivada da entropia cruzada em relação aos logits do softmax.
// Retorna dh: o gradiente em relação ao estado oculto h (para propagar
// de volta para a camada LSTM via BPTT).
std::vector<float> OutputLayer::backward(int palavra_alvo_id) {
    std::vector<float> dh(hidden_dim, 0.0f);
    
    // O gradiente inicial é (p - 1) para a classe correta, e (p) para as outras
    // Isso é a derivada da entropia cruzada com softmax: d(perda)/d(logit)
    for (size_t i = 0; i < vocab_size; ++i) {
        float erro = ultimas_probs[i];
        if (i == (size_t)palavra_alvo_id) erro -= 1.0f;
        if (std::abs(erro) < 1e-6f) continue;  // Otimização: pula gradientes quase zero
        
        // Acumula gradiente no bias
        db_out[i] += erro;
        // Acumula gradiente na matriz de pesos: dW_out[i][j] += erro * h[j]
        for (size_t j = 0; j < hidden_dim; ++j) {
            dW_out[i * hidden_dim + j] += erro * ultimo_h[j];
            // dh[j] acumula o erro de volta para o estado oculto h
            dh[j] += erro * W_out[i * hidden_dim + j];
        }
    }

    return dh;
}

// Atualiza os pesos da camada de saída usando descida do gradiente:
//   W_out = W_out - taxa * dW_out
//   b_out = b_out - taxa * db_out
// Depois zera os gradientes para o próximo passo.
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

// Retorna a probabilidade prevista para uma palavra específica (após o último forward)
float OutputLayer::getProbabilidade(int palavra_alvo_id) const {
    return ultimas_probs[palavra_alvo_id];
}

// Salva os pesos da camada de saída em JSON
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

// Carrega os pesos da camada de saída de um JSON previamente salvo
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
