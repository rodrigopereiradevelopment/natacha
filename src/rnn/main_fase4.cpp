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

// Tokeniza um texto simples separando por espaços em branco.
// Cada token separado por espaço vira uma palavra do corpus.
std::vector<std::string> tokenizarTexto(const std::string& texto) {
    std::vector<std::string> tokens;
    std::stringstream ss(texto);
    std::string palavra;
    while (ss >> palavra) tokens.push_back(palavra);
    return tokens;
}

// Aplica gradient clipping (corte de gradiente) para evitar o problema do
// "gradiente explodindo" em redes neurais recorrentes.
//
// Calcula a norma L2 do vetor de gradientes. Se a norma exceder o limite,
// escala todos os gradientes por (limite / norma) para que a nova norma
// seja exatamente o limite. Isso evita atualizações de peso muito grandes
// que poderiam destruir o treinamento.
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
    // Configura OpenMP para usar 3 threads (paralelismo no processamento do corpus)
    omp_set_num_threads(3);
    std::cout << "=== NATACHA ENGINE - FASE 4 (OTIMIZADO) ===" << std::endl;

    // Carrega os embeddings Word2Vec previamente treinados (v38)
    // Cada palavra do vocabulário mapeia para um vetor denso de 64 dimensões.
    EmbeddingManager embManager;
    if (!embManager.carregarEmbeddings("dados/embeddings/word2vec_v38.json")) return 1;

    // Lê o corpus de texto completo para treinamento da LSTM
    std::ifstream arquivoTexto("dados/embeddings/corpus_natacha_220k.txt");
    if (!arquivoTexto.is_open()) {
        std::cerr << "Erro: corpus_natacha_220k.txt nao encontrado." << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << arquivoTexto.rdbuf();
    std::string texto = buffer.str();
    arquivoTexto.close();

    // Tokeniza o texto em palavras individuais
    auto tokens = tokenizarTexto(texto);
    std::cout << "Tokens no corpus: " << tokens.size() << std::endl;

    // 1. Constrói o vocabulário local a partir do corpus
    // Conta a frequência de cada palavra (após limpeza alfanumérica)
    std::unordered_map<std::string, int> freq;
    for (const std::string& tok : tokens) {
        std::string limpa = tok;
        limpa.erase(std::remove_if(limpa.begin(), limpa.end(), 
            [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), limpa.end());
        if (!limpa.empty()) freq[limpa]++;
    }

    // Filtra palavras com frequência >= 3 (remove palavras raras/ruído)
    // e atribui um ID inteiro sequencial a cada palavra válida
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
    std::cout << "Tamanho do Vocab Local (filtrado): " << vocabLocalSize << std::endl;

    // Dimensão dos embeddings (deve bater com o Word2Vec treinado, ex: 64)
    size_t dim = 64;
    size_t total_tokens = tokens.size();

    // 2. Prepara os dados de treinamento em paralelo (OpenMP)
    // Para cada par de palavras consecutivas (x_t, x_{t+1}):
    //   - entrada[i] = vetor da palavra atual (embedding de 64 dims)
    //   - alvos[i]   = ID da próxima palavra no vocabulário local
    //
    // Paralelizamos com OpenMP porque a leitura de embeddings é independente
    // para cada token.
    std::vector<std::vector<float>> entradas(total_tokens);
    std::vector<int> alvos(total_tokens, -1);

    #pragma omp parallel for
    for (size_t i = 0; i < total_tokens - 1; ++i) {
        std::string atual = tokens[i];
        atual.erase(std::remove_if(atual.begin(), atual.end(), 
            [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), atual.end());
        
        std::string prox = tokens[i+1];
        prox.erase(std::remove_if(prox.begin(), prox.end(), 
            [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), prox.end());

        auto it = vocabLocal.find(prox);
        if (!atual.empty() && it != vocabLocal.end()) {
            auto vec = embManager.obterVetor(atual);
            if (!vec.empty()) {
                entradas[i] = std::move(vec);
                alvos[i] = it->second;
            }
        }
    }

    // Filtra apenas os pares válidos (ambas as palavras existem no vocabulário)
    std::vector<std::vector<float>> entradas_ok;
    std::vector<int> alvos_ok;
    for (size_t i = 0; i < total_tokens - 1; ++i) {
        if (alvos[i] != -1) {
            entradas_ok.push_back(std::move(entradas[i]));
            alvos_ok.push_back(alvos[i]);
        }
    }
    
    std::cout << "Pares validos: " << entradas_ok.size() << std::endl;

    // 3. Inicializa as Camadas da rede
    // RNNLayer: LSTM com input_dim = dim, hidden_dim = dim
    // OutputLayer: mapeia hidden_dim para vocabLocalSize (softmax sobre o vocabulário)
    RNNLayer rnn(dim, dim);
    OutputLayer out_layer(dim, vocabLocalSize);

    // CONFIGURAÇÃO DE LEARNING RATE (DECAY)
    // Começa com learning rate maior e diminui linearmente ao longo das épocas.
    // Isso ajuda a convergir rápido no início e afinar o modelo no final.
    float learningRateInicial = 0.003f;
    float learningRateFinal = 0.0003f;
    int epocas = 50;
    int tamanho_bloco = 30;  // quantos pares processamos por atualização de peso

    std::cout << "Iniciando treino..." << std::endl;

    for (int epoca = 0; epoca < epocas; ++epoca) {
        // Decay linear do Learning Rate: começa em learningRateInicial, termina em learningRateFinal
        float progresso = (epocas > 1) 
        ? (float)epoca / (float)(epocas - 1) 
        : 0.0f;
        float learningRate = learningRateInicial * (1.0f - progresso) + learningRateFinal * progresso;

        float perda_total = 0.0f;
        int total_pares = 0;

        // Processa o corpus em blocos para não carregar tudo na memória de uma vez
        for (size_t inicio = 0; inicio < entradas_ok.size(); inicio += tamanho_bloco) {
            size_t fim = std::min(inicio + tamanho_bloco, entradas_ok.size());
            
            // NOTA: As cópias de bloco_entradas e bloco_alvos ainda existem. 
            // É o próximo gargalo a resolver, mas vamos rodar o benchmark primeiro.
            std::vector<std::vector<float>> bloco_entradas(entradas_ok.begin() + inicio, entradas_ok.begin() + fim);
            std::vector<int> bloco_alvos(alvos_ok.begin() + inicio, alvos_ok.begin() + fim);

            // FORWARD PASS: processa o bloco através da LSTM
            auto saidas_h = rnn.processarSequencia(bloco_entradas);
            std::vector<std::vector<float>> dh_saidas;

            // Para cada passo do bloco, calcula a saída da OutputLayer e o erro
            for (size_t t = 0; t < saidas_h.size(); ++t) {
                out_layer.forward(saidas_h[t]);  // softmax sobre o vocabulário
                float prob = out_layer.getProbabilidade(bloco_alvos[t]);
                // Perda de entropia cruzada: -log(p_alvo)
                perda_total -= std::log(prob + 1e-8f);
                total_pares++;

                // BACKWARD da OutputLayer: calcula dh (gradiente para h)
                auto dh_t = out_layer.backward(bloco_alvos[t]);
                // Gradient clipping para evitar gradiente explodindo
                clipGradients(dh_t, 5.0f);
                dh_saidas.push_back(dh_t);
            }

            // BACKWARD da RNN (BPTT): propaga o erro através do tempo
            rnn.backward(dh_saidas);
            
            // Atualiza os pesos usando o learning rate com decay
            rnn.atualizarPesos(learningRate);
            out_layer.atualizarPesos(learningRate);
        }

        // Calcula estatísticas da época
        float perda_media = perda_total / total_pares;
        // Perplexidade = exp(perda_media): métrica padrão para modelos de linguagem.
        // Quanto menor, melhor. Perplexidade = 1 significa predição perfeita.
        float perplexidade = std::exp(perda_media);
        std::cout << "Epoca " << epoca << " | LR: " << learningRate 
                  << " | Perda: " << perda_media 
                  << " | Perplexidade: " << perplexidade << std::endl;

        // Salva checkpoint a cada 5 épocas para não perder o progresso
        if ((epoca + 1) % 5 == 0) {
            std::cout << "  Salvando checkpoint..." << std::endl;
            rnn.getCell().salvar("dados/embeddings/lstm_pesos_checkpoint.json");
            out_layer.salvar("dados/embeddings/output_pesos_checkpoint.json");
        }
    }

    // Salvamento final dos pesos e vocabulário
    std::cout << "Salvando pesos finais..." << std::endl;
    rnn.getCell().salvar("dados/embeddings/lstm_pesos.json");
    out_layer.salvar("dados/embeddings/output_pesos.json");
    
    // Salva o mapeamento ID -> palavra para uso posterior na geração de texto
    json j_vocab;
    j_vocab["id_para_palavra"] = idParaPalavra;
    std::ofstream arqVocab("dados/embeddings/vocab_local.json");
    arqVocab << j_vocab.dump();
    arqVocab.close();
    
    std::cout << "Pesos e Vocab Local salvos com sucesso!" << std::endl;
    return 0;
}
