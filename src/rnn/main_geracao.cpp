#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <sstream> // Necessário para o stringstream
#include "json.hpp"
#include "../embeddings/EmbeddingManager.hpp"
#include "RNNLayer.hpp"
#include "OutputLayer.hpp"

using json = nlohmann::json;

int amostrarPalavra(const std::vector<float>& probs, std::mt19937& gen, float temperature = 0.7f) {
    std::vector<float> ajustadas(probs.size());
    for (size_t i = 0; i < probs.size(); ++i) {
        ajustadas[i] = std::pow(probs[i], 1.0f / temperature);
    }
    std::discrete_distribution<int> dist(ajustadas.begin(), ajustadas.end());
    return dist(gen);
}

int main(int argc, char* argv[]) {
    std::string prompt = (argc > 1) ? argv[1] : "felix miou na cozinha";
    int seed = (argc > 2) ? std::atoi(argv[2]) : 42;

    std::cout << "=== NATACHA ENGINE - FASE 4 (GERACAO COM CONTEXTO) ===" << std::endl;

    EmbeddingManager embManager;
    if (!embManager.carregarEmbeddings("dados/embeddings/word2vec_v39.json")) return 1;

    std::ifstream arqVocab("dados/embeddings/vocab_local.json");
    if (!arqVocab.is_open()) {
        std::cerr << "Erro: vocab_local.json nao encontrado." << std::endl;
        return 1;
    }
    json j_vocab;
    arqVocab >> j_vocab;
    std::vector<std::string> idParaPalavra = j_vocab["id_para_palavra"].get<std::vector<std::string>>();
    int vocab_local_size = idParaPalavra.size();

    size_t dim = 64;
    RNNLayer rnn(dim, dim);
    OutputLayer out_layer(dim, vocab_local_size);

    if (!rnn.getCell().carregar("dados/embeddings/lstm_pesos.json")) return 1;
    if (!out_layer.carregar("dados/embeddings/output_pesos.json")) return 1;

    std::cout << "Prompt: " << prompt << " | Seed: " << seed << std::endl;
    std::cout << "Gerando: " << prompt;

    std::vector<float> h_atual(dim, 0.0f);
    std::vector<float> c_atual(dim, 0.0f);
    std::mt19937 gen(seed);

    // 1. AQUECIMENTO: Passa cada palavra do prompt pela LSTM
    std::stringstream ss(prompt);
    std::string palavra;
    std::string ultima_palavra = "";

    while (ss >> palavra) {
        std::vector<float> vec = embManager.obterVetor(palavra);
        if (!vec.empty()) {
            LSTMState estado = rnn.getCell().forward(vec, h_atual, c_atual);
            h_atual = estado.h;
            c_atual = estado.c;
            ultima_palavra = palavra;
        }
    }

    if (ultima_palavra.empty()) {
        std::cerr << "\nNenhuma palavra do prompt tem embedding. Parando." << std::endl;
        return 1;
    }

    // 2. GERAÇÃO: A partir do estado "aquecido", prevê as próximas palavras
    std::string palavra_atual = ultima_palavra;

    for (int passo = 0; passo < 20; ++passo) {
        std::vector<float> vec = embManager.obterVetor(palavra_atual);
        if (vec.empty()) break;

        LSTMState estado = rnn.getCell().forward(vec, h_atual, c_atual);
        h_atual = estado.h;
        c_atual = estado.c;

        out_layer.forward(h_atual);
        const std::vector<float>& probs = out_layer.getProbabilidades();

        // Filtra palavras que não têm embedding
        std::vector<float> probs_validas = probs;
        float soma = 0.0f;
        for (size_t i = 0; i < probs_validas.size(); ++i) {
            if (!embManager.contem(idParaPalavra[i])) {
                probs_validas[i] = 0.0f;
            }
            soma += probs_validas[i];
        }

        if (soma <= 0.0f) break;

        int proximo_id = amostrarPalavra(probs_validas, gen, 0.7f);
        if (proximo_id < 0 || static_cast<size_t>(proximo_id) >= idParaPalavra.size()) break;

        std::string proxima_palavra = idParaPalavra[proximo_id];
        std::cout << " " << proxima_palavra;
        palavra_atual = proxima_palavra;
    }

    std::cout << "\nFim da geracao." << std::endl;
    return 0;
}