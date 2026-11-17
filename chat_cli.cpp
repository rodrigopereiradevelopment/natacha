#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <memory>
#include <chrono>
#include <iomanip>
#include <Eigen/Dense>

#include "src/embeddings/EmbeddingManager.hpp"
#include "src/embeddings/json.hpp"
#include "src/rnn/RNNLayer.hpp"
#include "src/rnn/OutputLayer.hpp"
#include "src/attention/TransformerBlock.hpp"

using namespace Eigen;
using namespace std;
using json = nlohmann::json;

/**
 * Natacha CLI Chat — protótipo de terminal (Fases 4/5).
 *
 * Objetivo: dar ao usuário a experiência de conversar com a Natacha
 * rodando 100% localmente, sem nenhuma chamada de API.
 *
 * Dois motores possíveis:
 *
 *   1. MOTOR LSTM (padrão, Fase 4)
 *      Usa os pesos já treinados em:
 *        - dados/embeddings/lstm_pesos.json
 *        - dados/embeddings/output_pesos.json
 *        - dados/embeddings/vocab_local.json
 *      A entrada do usuário "aquece" o estado oculto (h, c) e a LSTM
 *      prevê as próximas palavras, uma a uma (geração autoregressiva).
 *      É o motor que já "fala" alguma coisa.
 *
 *   2. MOTOR TRANSFORMER (demonstração, Fase 5)
 *      Se os pesos da LSTM não estiverem disponíveis, o chat cai para
 *      um TransformerBlock com pesos aleatórios. Nesse modo a saída é
 *      apenas estatística (média/desvio dos valores), servindo para
 *      provar que o encanamento forward funciona ponta a ponta.
 *
 * Comandos do chat:
 *   sair / exit / quit   encerra
 *   ajuda / help         mostra os comandos
 *   limpar / clear       zera o histórico de contexto
 *   debug                liga/desliga a impressão de detalhes técnicos
 */

// ---------------------------------------------------------------------------
// Utilidades de texto
// ---------------------------------------------------------------------------

/**
 * Tokeniza uma linha em palavras.
 *
 * Mantém bytes UTF-8 (>= 0x80) para não destruir acentos, mas converte
 * apenas ASCII para minúsculas. A pontuação é descartada.
 */
vector<string> tokenizar(const string& texto) {
    vector<string> tokens;
    string atual;

    auto finalizar = [&]() {
        if (!atual.empty()) {
            tokens.push_back(atual);
            atual.clear();
        }
    };

    for (unsigned char c : texto) {
        if (c >= 0x80) {
            // Byte de continuação/início de caractere UTF-8: preserva.
            atual += static_cast<char>(c);
        } else if (isalnum(c) || c == '_' || c == '\'') {
            atual += static_cast<char>(tolower(c));
        } else {
            finalizar();
        }
    }
    finalizar();
    return tokens;
}

/** Junta palavras em uma frase legível. */
string juntar(const vector<string>& palavras, size_t inicio = 0) {
    string out;
    for (size_t i = inicio; i < palavras.size(); ++i) {
        if (!out.empty()) out += ' ';
        out += palavras[i];
    }
    return out;
}

// ---------------------------------------------------------------------------
// Carregamento de recursos
// ---------------------------------------------------------------------------

/**
 * Tenta carregar os embeddings em uma lista de caminhos candidatos.
 * Retorna true no primeiro que funcionar.
 */
bool carregar_embeddings(EmbeddingManager& emb, string& caminho_usado) {
    const vector<string> candidatos = {
        "dados/embeddings/word2vec_v39.json",
        "dados/embeddings/word2vec_v38.json",
        "../dados/embeddings/word2vec_v39.json",
        "../dados/embeddings/word2vec_v38.json"
    };

    for (const string& caminho : candidatos) {
        ifstream teste(caminho);
        if (!teste.good()) continue;
        teste.close();

        cout << "Carregando embeddings: " << caminho << " ..." << flush;
        auto t0 = chrono::steady_clock::now();
        if (emb.carregarEmbeddings(caminho)) {
            auto dt = chrono::duration_cast<chrono::milliseconds>(
                          chrono::steady_clock::now() - t0).count();
            cout << " ok (" << dt << " ms)" << endl;
            caminho_usado = caminho;
            return true;
        }
        cout << " falhou." << endl;
    }
    return false;
}

/** Lê o vocabulário local (id_para_palavra) usado pela camada de saída. */
bool carregar_vocab_local(const string& caminho, vector<string>& id_para_palavra) {
    ifstream arq(caminho);
    if (!arq.is_open()) return false;

    try {
        json j;
        arq >> j;
        id_para_palavra = j.at("id_para_palavra").get<vector<string>>();
    } catch (const exception& e) {
        cerr << "ERRO ao ler vocab: " << e.what() << endl;
        return false;
    }
    return !id_para_palavra.empty();
}

/** Procura o primeiro arquivo existente entre os candidatos. */
string primeiro_existente(const vector<string>& caminhos) {
    for (const string& c : caminhos) {
        ifstream f(c);
        if (f.good()) return c;
    }
    return "";
}

// ---------------------------------------------------------------------------
// Motor LSTM: geração autoregressiva
// ---------------------------------------------------------------------------

/**
 * Amostra um índice a partir de probabilidades, aplicando temperatura.
 * Temperatura < 1 deixa a distribuição mais "afiada" (respostas previsíveis);
 * temperatura > 1 deixa mais aleatória.
 */
int amostrar(const vector<float>& probs, mt19937& gen, float temperatura) {
    vector<float> ajustadas(probs.size());
    for (size_t i = 0; i < probs.size(); ++i) {
        ajustadas[i] = pow(probs[i], 1.0f / temperatura);
    }
    discrete_distribution<int> dist(ajustadas.begin(), ajustadas.end());
    return dist(gen);
}

/**
 * Gera uma resposta a partir de um contexto.
 *
 * @param contexto        Palavras que servem de "aquecimento" da LSTM
 *                        (histórico da conversa + mensagem atual).
 * @param emb             Embeddings carregados.
 * @param id_para_palavra Vocabulário da camada de saída.
 * @param rnn             Camada LSTM com pesos treinados.
 * @param out             Camada de saída com pesos treinados.
 * @param gen             Gerador aleatório (seed fixa => reprodutível).
 * @param max_tokens      Máximo de palavras geradas.
 * @param temperatura     Criatividade da amostragem.
 * @param debug           Se true, imprime detalhes técnicos.
 * @return                Texto gerado (pode ser vazio).
 */
string gerar_resposta(const vector<string>& contexto,
                      EmbeddingManager& emb,
                      const vector<string>& id_para_palavra,
                      RNNLayer& rnn,
                      OutputLayer& out,
                      mt19937& gen,
                      int max_tokens,
                      float temperatura,
                      bool debug) {
    const size_t dim = rnn.getCell().getHiddenDim();

    vector<float> h(dim, 0.0f);
    vector<float> c(dim, 0.0f);
    string ultima_palavra;
    int conhecidas = 0;

    // 1. AQUECIMENTO: passa cada palavra conhecida do contexto pela LSTM.
    //    O estado (h, c) final resume o que a rede "lembra" da conversa.
    for (const string& palavra : contexto) {
        vector<float> vec = emb.obterVetor(palavra);
        if (vec.empty() || vec.size() != dim) continue;

        LSTMState estado = rnn.getCell().forward(vec, h, c);
        h = estado.h;
        c = estado.c;
        ultima_palavra = palavra;
        conhecidas++;
    }

    if (ultima_palavra.empty()) {
        return "";  // nenhuma palavra do contexto tem embedding
    }

    if (debug) {
        cout << "   [debug] palavras conhecidas no contexto: " << conhecidas << endl;
    }

    // 2. GERAÇÃO: realimenta cada palavra prevista como entrada do próximo passo.
    vector<string> geradas;
    string palavra_atual = ultima_palavra;
    int repeticao = 0;

    for (int passo = 0; passo < max_tokens; ++passo) {
        vector<float> vec = emb.obterVetor(palavra_atual);
        if (vec.empty() || vec.size() != dim) break;

        LSTMState estado = rnn.getCell().forward(vec, h, c);
        h = estado.h;
        c = estado.c;

        out.forward(h);
        vector<float> probs = out.getProbabilidades();

        // Zera palavras que não têm embedding: gerar uma palavra que a
        // própria Natacha não entende seria um beco sem saída.
        float soma = 0.0f;
        for (size_t i = 0; i < probs.size() && i < id_para_palavra.size(); ++i) {
            if (!emb.contem(id_para_palavra[i])) {
                probs[i] = 0.0f;
            }
            soma += probs[i];
        }
        if (soma <= 0.0f) break;

        int id = amostrar(probs, gen, temperatura);
        if (id < 0 || static_cast<size_t>(id) >= id_para_palavra.size()) break;

        string proxima = id_para_palavra[id];
        geradas.push_back(proxima);

        // Anti-loop simples: se repetir a mesma palavra 3x seguidas, para.
        if (proxima == palavra_atual) {
            if (++repeticao >= 3) break;
        } else {
            repeticao = 0;
        }
        palavra_atual = proxima;
    }

    return juntar(geradas);
}

// ---------------------------------------------------------------------------
// Motor Transformer (modo demonstração)
// ---------------------------------------------------------------------------

/**
 * Fallback quando não há pesos da LSTM: processa o texto pelo
 * TransformerBlock e devolve um resumo estatístico da saída.
 * Não é uma resposta de verdade — é só prova de que o forward roda.
 */
string resposta_demo_transformer(const string& entrada,
                                 EmbeddingManager& emb,
                                 TransformerBlock& bloco,
                                 int d_model) {
    vector<string> tokens = tokenizar(entrada);
    vector<string> validos;

    for (const string& t : tokens) {
        vector<float> v = emb.obterVetor(t);
        if (v.size() == static_cast<size_t>(d_model)) validos.push_back(t);
    }

    if (validos.empty()) {
        return "(demo) Nao encontrei nenhuma palavra conhecida nessa frase.";
    }

    MatrixXd x(validos.size(), d_model);
    for (size_t i = 0; i < validos.size(); ++i) {
        vector<float> v = emb.obterVetor(validos[i]);
        for (int j = 0; j < d_model; ++j) x(i, j) = v[j];
    }

    MatrixXd y = bloco.forward(x);
    double media = y.mean();
    double var = (y.array() - media).square().mean();

    ostringstream oss;
    oss << "(demo Transformer) " << validos.size() << " tokens, "
        << d_model << " dims | media=" << fixed << setprecision(4) << media
        << " desvio=" << sqrt(var);
    return oss.str();
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    cout << "==============================================" << endl;
    cout << "===      NATACHA TERMINAL CHAT            ===" << endl;
    cout << "===      Fases 4/5 - 100% local           ===" << endl;
    cout << "==============================================" << endl;
    cout << endl;
    cout << "Digite 'sair' para encerrar e 'ajuda' para comandos." << endl;
    cout << endl;

    // 1. Embeddings (obrigatórios para qualquer motor)
    EmbeddingManager emb;
    string caminho_emb;
    if (!carregar_embeddings(emb, caminho_emb)) {
        cerr << "ERRO: nenhum arquivo de embeddings encontrado." << endl;
        cerr << "Rode o treino do Word2Vec (Fase 3) ou verifique os caminhos." << endl;
        return 1;
    }

    const int d_model = static_cast<int>(emb.getDimensao());
    cout << "Vocabulario: " << emb.getVocabSize()
         << " palavras, " << d_model << " dimensoes." << endl;
    cout << endl;

    // 2. Pesos da LSTM (motor principal)
    string caminho_vocab = primeiro_existente({
        "dados/embeddings/vocab_local.json",
        "../dados/embeddings/vocab_local.json"});
    string caminho_lstm = primeiro_existente({
        "dados/embeddings/lstm_pesos.json",
        "../dados/embeddings/lstm_pesos.json"});
    string caminho_out = primeiro_existente({
        "dados/embeddings/output_pesos.json",
        "../dados/embeddings/output_pesos.json"});

    vector<string> id_para_palavra;
    unique_ptr<RNNLayer> rnn;
    unique_ptr<OutputLayer> out;
    bool motor_lstm = false;

    if (!caminho_vocab.empty() && !caminho_lstm.empty() && !caminho_out.empty() &&
        carregar_vocab_local(caminho_vocab, id_para_palavra)) {
        rnn = make_unique<RNNLayer>(d_model, d_model);
        out = make_unique<OutputLayer>(d_model, id_para_palavra.size());

        if (rnn->getCell().carregar(caminho_lstm) &&
            out->carregar(caminho_out)) {
            motor_lstm = true;
        }
    }

    // 3. Fallback: Transformer de demonstração
    unique_ptr<TransformerBlock> bloco;
    if (motor_lstm) {
        cout << "Motor: LSTM treinada (Fase 4)" << endl;
        cout << "  vocab_local: " << id_para_palavra.size() << " palavras" << endl;
    } else {
        cout << "Motor: Transformer DEMO (pesos aleatorios, Fase 5)" << endl;
        cout << "  A LSTM treinada nao foi encontrada; a saida sera estatistica." << endl;
        bloco = make_unique<TransformerBlock>(d_model, 4, d_model * 4);
    }
    cout << endl;

    // Estado da conversa
    vector<string> historico;   // palavras recentes (contexto para a LSTM)
    const size_t max_historico = 24;
    mt19937 gen(42);
    const int max_tokens = 20;
    const float temperatura = 0.7f;
    bool debug = false;

    cout << "=== Chat iniciado. ===" << endl << endl;

    string entrada;
    while (true) {
        cout << "Voce: ";
        if (!getline(cin, entrada)) break;  // EOF

        // Remove espacos nas pontas
        entrada.erase(0, entrada.find_first_not_of(" \t\r\n"));
        entrada.erase(entrada.find_last_not_of(" \t\r\n") + 1);

        if (entrada == "sair" || entrada == "exit" || entrada == "quit") {
            cout << "Natacha: Ate mais! Vou ficar de olho no Felix." << endl;
            break;
        }
        if (entrada == "ajuda" || entrada == "help") {
            cout << "Comandos:" << endl;
            cout << "  sair   - encerra o chat" << endl;
            cout << "  limpar - zera o contexto da conversa" << endl;
            cout << "  debug  - liga/desliga detalhes tecnicos" << endl;
            continue;
        }
        if (entrada == "limpar" || entrada == "clear") {
            historico.clear();
            cout << "Natacha: Contexto limpo." << endl;
            continue;
        }
        if (entrada == "debug") {
            debug = !debug;
            cout << "Natacha: debug " << (debug ? "ligado" : "desligado") << "." << endl;
            continue;
        }
        if (entrada.empty()) {
            cout << "Natacha: ..." << endl;
            continue;
        }

        vector<string> palavras_entrada = tokenizar(entrada);
        if (palavras_entrada.empty()) {
            cout << "Natacha: Nao entendi. Tenta escrever com letras e numeros." << endl;
            continue;
        }

        auto t0 = chrono::steady_clock::now();
        string resposta;

        if (motor_lstm) {
            // Contexto = histórico recente + mensagem atual
            vector<string> contexto = historico;
            contexto.insert(contexto.end(),
                            palavras_entrada.begin(), palavras_entrada.end());

            resposta = gerar_resposta(contexto, emb, id_para_palavra,
                                      *rnn, *out, gen,
                                      max_tokens, temperatura, debug);

            if (resposta.empty()) {
                resposta = "Ainda nao aprendi nada sobre isso. "
                           "Meu vocabulario e pequeno (Fase 4).";
            }

            // Atualiza histórico com a entrada (não com a resposta gerada,
            // para não realimentar erros e entrar em loop).
            for (const string& p : palavras_entrada) {
                historico.push_back(p);
            }
            if (historico.size() > max_historico) {
                historico.erase(historico.begin(),
                                historico.begin() + (historico.size() - max_historico));
            }
        } else {
            resposta = resposta_demo_transformer(entrada, emb, *bloco, d_model);
        }

        auto dt = chrono::duration_cast<chrono::milliseconds>(
                      chrono::steady_clock::now() - t0).count();

        cout << "Natacha: " << resposta << endl;
        if (debug) cout << "   [debug] tempo: " << dt << " ms" << endl;
        cout << endl;
    }

    cout << endl << "=== Chat encerrado. ===" << endl;
    return 0;
}
