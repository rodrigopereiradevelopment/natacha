#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <cstddef>
#include <omp.h>
#include "../embeddings/EmbeddingManager.hpp"
#include "RNNLayer.hpp"
#include "OutputLayer.hpp"
#include "json.hpp"

using json = nlohmann::json;

std::vector<std::string> tokenizarTexto(const std::string& texto) {
    std::vector<std::string> tokens;
    std::stringstream ss(texto);
    std::string palavra;
    while (ss >> palavra) tokens.push_back(palavra);
    return tokens;
}

void clipGradients(std::vector<float>& grad, float limite) {
    float norma = 0.0f;
    for (float g : grad) norma += g * g;
    norma = std::sqrt(norma);
    if (norma > limite) {
        float fator = limite / norma;
        for (float& g : grad) g *= fator;
    }
}

int main() {
    omp_set_num_threads(3);
    std::cout << "=== NATACHA ENGINE - FASE 4 (SEQUENCIAS NARRATIVAS) ===" << std::endl;

    EmbeddingManager embManager;
    if (!embManager.carregarEmbeddings("dados/embeddings/word2vec_v39.json")) return 1;

    // Carrega os dois corpora narrativos
    std::string texto;
    std::vector<std::string> arquivos = {
        "dados/embeddings/corpus_lstm_narrativo.txt",
        "dados/embeddings/corpus_lstm_auxiliar.txt"
    };

    for (const auto& arq : arquivos) {
        std::ifstream f(arq);
        if (!f.is_open()) {
            std::cerr << "Aviso: " << arq << " nao encontrado." << std::endl;
            continue;
        }
        std::stringstream buffer;
        buffer << f.rdbuf();
        texto += buffer.str() + "\n\n";
        f.close();
        std::cout << "Carregado: " << arq << std::endl;
    }

    auto tokens = tokenizarTexto(texto);
    std::cout << "Tokens no corpus: " << tokens.size() << std::endl;

    // 1. Constrói o Vocab Local (palavras com freq >= 3)
    std::unordered_map<std::string, int> freq;
    for (const std::string& tok : tokens) {
        std::string limpa = tok;
        limpa.erase(std::remove_if(limpa.begin(), limpa.end(), 
            [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), limpa.end());
        if (!limpa.empty()) freq[limpa]++;
    }

    std::unordered_map<std::string, int> vocabLocal;
    std::vector<std::string> idParaPalavra;
    for (const std::string& tok : tokens) {
        std::string limpa = tok;
        limpa.erase(std::remove_if(limpa.begin(), limpa.end(), 
            [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), limpa.end());
        if (!limpa.empty() && freq[limpa] >= 3 && vocabLocal.find(limpa) == vocabLocal.end()) {
            vocabLocal[limpa] = idParaPalavra.size();
            idParaPalavra.push_back(limpa);
        }
    }
    int vocabLocalSize = idParaPalavra.size();
    std::cout << "Vocab Local: " << vocabLocalSize << " palavras" << std::endl;

    size_t dim = 64;
    int tamanho_sequencia = 30; // Janela de 30 palavras

    // 2. Cria sequências de 30 palavras consecutivas
    // Cada sequência tem 30 palavras de entrada e 1 palavra alvo (a próxima)
    std::vector<std::vector<std::vector<float>>> sequencias;
    std::vector<int> alvos;

    for (size_t i = 0; i + tamanho_sequencia < tokens.size(); ++i) {
        std::vector<std::vector<float>> seq;
        bool valida = true;

        // Pega 30 palavras consecutivas
        for (int j = 0; j < tamanho_sequencia; ++j) {
            std::string palavra = tokens[i + j];
            palavra.erase(std::remove_if(palavra.begin(), palavra.end(), 
                [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), palavra.end());
            
            if (palavra.empty()) { valida = false; break; }
            auto vec = embManager.obterVetor(palavra);
            if (vec.empty()) { valida = false; break; }
            seq.push_back(std::move(vec));
        }

        if (!valida) continue;

        // Palavra alvo (a próxima após as 30)
        std::string alvo = tokens[i + tamanho_sequencia];
        alvo.erase(std::remove_if(alvo.begin(), alvo.end(), 
            [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), alvo.end());

        auto it = vocabLocal.find(alvo);
        if (it == vocabLocal.end()) continue;

        sequencias.push_back(std::move(seq));
        alvos.push_back(it->second);
    }

    std::cout << "Sequencias validas: " << sequencias.size() << std::endl;

    // 3. Inicializa as Camadas
    RNNLayer rnn(dim, dim);
    OutputLayer out_layer(dim, vocabLocalSize);

    float learningRateInicial = 0.003f;
    float learningRateFinal = 0.0003f;
    int epocas = 20; // Menos épocas porque agora cada exemplo é uma sequência de 30

    std::cout << "Iniciando treino..." << std::endl;

    for (int epoca = 0; epoca < epocas; ++epoca) {
        float progresso = (epocas > 1) 
        ? (float)epoca / (float)(epocas - 1) 
        : 0.0f;
        float learningRate = learningRateInicial * (1.0f - progresso) + learningRateFinal * progresso;

        float perda_total = 0.0f;
        int total_exemplos = 0;

        for (size_t s = 0; s < sequencias.size(); ++s) {
            // Passa a sequência de 30 palavras para a RNN
            auto saidas_h = rnn.processarSequencia(sequencias[s]);

            // A saída do ÚLTIMO passo (h_t) é usada para prever a próxima palavra
            out_layer.forward(saidas_h.back());
            float prob = out_layer.getProbabilidade(alvos[s]);
            perda_total -= std::log(prob + 1e-8f);
            total_exemplos++;

            // Backward da saída
            auto dh_t = out_layer.backward(alvos[s]);
            clipGradients(dh_t, 5.0f);

            // O gradiente só volta para o último passo (dh_t), os outros passos recebem zero
            std::vector<std::vector<float>> dh_saidas(tamanho_sequencia, std::vector<float>(dim, 0.0f));
            dh_saidas.back() = dh_t;

            rnn.backward(dh_saidas);
            rnn.atualizarPesos(learningRate);
            out_layer.atualizarPesos(learningRate);
        }

        float perda_media = perda_total / total_exemplos;
        float perplexidade = std::exp(perda_media);
        std::cout << "Epoca " << epoca << " | LR: " << learningRate 
                  << " | Perda: " << perda_media 
                  << " | Perplexidade: " << perplexidade << std::endl;

        if ((epoca + 1) % 5 == 0) {
            std::cout << "  Salvando checkpoint..." << std::endl;
            rnn.getCell().salvar("dados/embeddings/lstm_pesos_checkpoint.json");
            out_layer.salvar("dados/embeddings/output_pesos_checkpoint.json");
        }
    }

    std::cout << "Salvando pesos finais..." << std::endl;
    rnn.getCell().salvar("dados/embeddings/lstm_pesos.json");
    out_layer.salvar("dados/embeddings/output_pesos.json");
    
    json j_vocab;
    j_vocab["id_para_palavra"] = idParaPalavra;
    std::ofstream arqVocab("dados/embeddings/vocab_local.json");
    arqVocab << j_vocab.dump();
    arqVocab.close();
    
    std::cout << "Pesos e Vocab Local salvos com sucesso!" << std::endl;
    return 0;
}