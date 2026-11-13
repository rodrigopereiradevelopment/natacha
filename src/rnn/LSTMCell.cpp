#include "LSTMCell.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cmath>
#include "json.hpp"
using json = nlohmann::json;

// Construtor da célula LSTM.
// Inicializa todas as matrizes de pesos (W e U) e vetores de bias (b) para
// as quatro portas: forget (f), input (i), candidato (c) e output (o).
//
// Dimensões:
//   W* : hidden_dim x input_dim  (pesos da entrada atual x_t)
//   U* : hidden_dim x hidden_dim (pesos do estado oculto anterior h_prev)
//   b* : hidden_dim              (bias de cada porta)
//
// Inicialização de Xavier (Glorot): desvio padrão = sqrt(2 / (input_dim + hidden_dim))
// Isso mantém a variância das ativações estável através das camadas, evitando
// que os gradientes explodam ou someçam durante o treinamento.
LSTMCell::LSTMCell(size_t input_dim, size_t hidden_dim)
    : input_dim(input_dim), hidden_dim(hidden_dim) {
    
    size_t w_size = hidden_dim * input_dim;
    size_t u_size = hidden_dim * hidden_dim;

    Wf.resize(w_size); Wi.resize(w_size); Wc.resize(w_size); Wo.resize(w_size);
    Uf.resize(u_size); Ui.resize(u_size); Uc.resize(u_size); Uo.resize(u_size);
    
    // Bias do forget gate inicializado com 1.0f (heurística comum):
    // incentiva a LSTM a reter informações do estado celular anterior por
    // padrão, o que ajuda no aprendizado de dependências de longo prazo.
    bf.resize(hidden_dim, 1.0f); 
    bi.resize(hidden_dim, 0.0f); 
    bc.resize(hidden_dim, 0.0f); 
    bo.resize(hidden_dim, 0.0f);
    
    // Gradientes acumulados (zerados inicialmente)
    dWf.resize(w_size, 0.0f); dWi.resize(w_size, 0.0f); dWc.resize(w_size, 0.0f); dWo.resize(w_size, 0.0f);
    dUf.resize(u_size, 0.0f); dUi.resize(u_size, 0.0f); dUc.resize(u_size, 0.0f); dUo.resize(u_size, 0.0f);
    dbf.resize(hidden_dim, 0.0f); dbi.resize(hidden_dim, 0.0f); 
    dbc.resize(hidden_dim, 0.0f); dbo.resize(hidden_dim, 0.0f);

    inicializarPesos();
}

// Inicializa os pesos W e U com distribuição normal N(0, desvio_padrao)
// usando Xavier/Glorot initialization. O bias do forget gate já foi setado
// como 1.0 no construtor.
void LSTMCell::inicializarPesos() {
    std::default_random_engine generator(42); 
    float desvio_padrao = std::sqrt(2.0f / (input_dim + hidden_dim));
    std::normal_distribution<float> distribution(0.0f, desvio_padrao);

    auto preencher = [&](std::vector<float>& vec) {
        for (auto& val : vec) val = distribution(generator);
    };

    preencher(Wf); preencher(Wi); preencher(Wc); preencher(Wo);
    preencher(Uf); preencher(Ui); preencher(Uc); preencher(Uo);
}

// Forward Pass de uma célula LSTM (um único passo de tempo).
//
// Entradas:
//   x_t      : entrada do passo atual (vetor de input_dim)
//   h_prev   : estado oculto do passo anterior (vetor de hidden_dim)
//   c_prev   : estado da célula do passo anterior (vetor de hidden_dim)
//
// Saídas:
//   h_next   : novo estado oculto
//   c_next   : novo estado da célula
//
// As quatro portas da LSTM:
//   1. Forget gate (f_t): decide quanto do estado celular anterior manter.
//   2. Input gate (i_t): decide quanto da nova informação adicionar.
//   3. Candidate (c_tilde): novo candidato de estado celular.
//   4. Output gate (o_t): decide quanto do estado celular expor no h_next.
//
// Equações (para cada neurônio i):
//   f_t = sigmoid(Wf * x_t + Uf * h_prev + bf)
//   i_t = sigmoid(Wi * x_t + Ui * h_prev + bi)
//   c_tilde = tanh(Wc * x_t + Uc * h_prev + bc)
//   o_t = sigmoid(Wo * x_t + Uo * h_prev + bo)
//   c_next = f_t * c_prev + i_t * c_tilde
//   h_next = o_t * tanh(c_next)
LSTMState LSTMCell::forward(const std::vector<float>& x_t, 
                           const std::vector<float>& h_prev, 
                           const std::vector<float>& c_prev) {

    std::vector<float> h_next(hidden_dim, 0.0f);
    std::vector<float> c_next(hidden_dim, 0.0f);

    for (size_t i = 0; i < hidden_dim; ++i) {
        // Acumula o bias de cada porta
        float f_gate_in = bf[i];
        float i_gate_in = bi[i];
        float c_tilde_in = bc[i];
        float o_gate_in = bo[i];

        // Contribuição da entrada atual x_t (matriz W)
        for (size_t j = 0; j < input_dim; ++j) {
            float x_j = x_t[j]; // Confiando no contrato de dimensão
            f_gate_in += Wf[i * input_dim + j] * x_j;
            i_gate_in += Wi[i * input_dim + j] * x_j;
            c_tilde_in += Wc[i * input_dim + j] * x_j;
            o_gate_in += Wo[i * input_dim + j] * x_j;
        }

        // Contribuição do estado oculto anterior h_prev (matriz U)
        for (size_t j = 0; j < hidden_dim; ++j) {
            float h_j = h_prev[j];
            f_gate_in += Uf[i * hidden_dim + j] * h_j;
            i_gate_in += Ui[i * hidden_dim + j] * h_j;
            c_tilde_in += Uc[i * hidden_dim + j] * h_j;
            o_gate_in += Uo[i * hidden_dim + j] * h_j;
        }

        // Aplica as funções de ativação
        float f_t = sigmoid(f_gate_in);   // quanto esquecer do estado anterior
        float i_t = sigmoid(i_gate_in);   // quanto adicionar de novo
        float c_tilde = std::tanh(c_tilde_in); // novo candidato de estado
        float o_t = sigmoid(o_gate_in);   // quanto expor na saída

        // Atualiza o estado celular: esquece parte do antigo, adiciona novo candidato
        float c_p = c_prev[i];
        c_next[i] = (f_t * c_p) + (i_t * c_tilde);
        // Saída oculta: expõe parte do estado celular filtrada por o_t
        h_next[i] = o_t * std::tanh(c_next[i]);
    }

    return {h_next, c_next};
}

// Backpropagation Through Time (BPTT) para UM passo de tempo.
//
// Calcula os gradientes das perdas em relação a:
//   - W, U, b (matrizes de peso e bias da célula)
//   - x_t (entrada, para propagar o erro para camadas anteriores)
//   - h_prev, c_prev (estados anteriores, para propagar o erro no tempo)
//
// Parâmetros:
//   x_t         : entrada do passo t
//   h_prev      : estado oculto do passo t-1
//   c_prev      : estado da célula do passo t-1
//   dh_total    : gradiente do erro em relação a h_next (vém "do futuro")
//   dc_next     : gradiente do erro em relação a c_next (vém "do futuro")
//   dh_prev_out : saída — gradiente para h_prev (propagado para t-1)
//   dc_prev_out : saída — gradiente para c_prev (propagado para t-1)
//
// Retorna:
//   dx_t : gradiente em relação à entrada x_t (para camadas anteriores)
//
// A chave matemática é a regra da cadeia aplicada através das portas:
//   dc = dh * o_t * (1 - tanh²(c_next))        // gradiente do estado celular
//   do_ = dh * tanh(c_next) * o_t * (1 - o_t)  // gradiente da output gate
//   df_ = dc * c_prev * f_t * (1 - f_t)        // gradiente da forget gate
//   di_ = dc * c_tilde * i_t * (1 - i_t)       // gradiente da input gate
//   dc_tilde = dc * i_t * (1 - c_tilde²)       // gradiente do candidato
std::vector<float> LSTMCell::backward(
    const std::vector<float>& x_t,
    const std::vector<float>& h_prev,
    const std::vector<float>& c_prev,
    const std::vector<float>& dh_total, // renomeado conceitualmente
    const std::vector<float>& dc_next,
    std::vector<float>& dh_prev_out,
    std::vector<float>& dc_prev_out) {

    // Recalcula as ativações do forward pass (precisamos delas para os gradientes)
    std::vector<float> h_current(hidden_dim, 0.0f);
    std::vector<float> c_current(hidden_dim, 0.0f);
    std::vector<float> f_t(hidden_dim), i_t(hidden_dim), c_tilde(hidden_dim), o_t(hidden_dim);

    for (size_t i = 0; i < hidden_dim; ++i) {
        float f_in = bf[i], i_in = bi[i], c_in = bc[i], o_in = bo[i];
        for (size_t j = 0; j < input_dim; ++j) {
            float x_j = x_t[j];
            f_in += Wf[i * input_dim + j] * x_j;
            i_in += Wi[i * input_dim + j] * x_j;
            c_in += Wc[i * input_dim + j] * x_j;
            o_in += Wo[i * input_dim + j] * x_j;
        }
        for (size_t j = 0; j < hidden_dim; ++j) {
            float h_j = h_prev[j];
            f_in += Uf[i * hidden_dim + j] * h_j;
            i_in += Ui[i * hidden_dim + j] * h_j;
            c_in += Uc[i * hidden_dim + j] * h_j;
            o_in += Uo[i * hidden_dim + j] * h_j;
        }
        f_t[i] = sigmoid(f_in);
        i_t[i] = sigmoid(i_in);
        c_tilde[i] = std::tanh(c_in);
        o_t[i] = sigmoid(o_in);

        float c_p = c_prev[i];
        c_current[i] = (f_t[i] * c_p) + (i_t[i] * c_tilde[i]);
        h_current[i] = o_t[i] * std::tanh(c_current[i]);
    }

    // Zera os buffers de saída
    dh_prev_out.assign(hidden_dim, 0.0f);
    dc_prev_out.assign(hidden_dim, 0.0f);
    std::vector<float> dx_t(input_dim, 0.0f);

    // Para cada neurônio da camada oculta, calcula os gradientes das quatro portas
    for (size_t i = 0; i < hidden_dim; ++i) {
        float dh = dh_total[i];
        float dc = dc_next[i];

        // Gradiente do estado celular c_next
        // dc = dh * o_t * (1 - tanh²(c))
        float tanh_c = std::tanh(c_current[i]);
        dc += dh * o_t[i] * (1.0f - tanh_c * tanh_c);

        // Gradientes das portas (todos usam derivada da sigmoid: s * (1 - s))
        float do_ = dh * tanh_c * o_t[i] * (1.0f - o_t[i]);
        float df_ = dc * c_prev[i] * f_t[i] * (1.0f - f_t[i]);
        float di_ = dc * c_tilde[i] * i_t[i] * (1.0f - i_t[i]);
        float dc_tilde = dc * i_t[i] * (1.0f - c_tilde[i] * c_tilde[i]);

        // O gradiente do estado celular anterior é apenas a parte esquecida:
        // dc_prev_out = dc * f_t
        dc_prev_out[i] = dc * f_t[i];

        // Acumula gradientes nas matrizes de peso W (entrada x_t) e U (estado h_prev)
        for (size_t j = 0; j < input_dim; ++j) {
            float x_j = x_t[j];
            dWf[i * input_dim + j] += df_ * x_j;
            dWi[i * input_dim + j] += di_ * x_j;
            dWc[i * input_dim + j] += dc_tilde * x_j;
            dWo[i * input_dim + j] += do_ * x_j;
        }
        for (size_t j = 0; j < hidden_dim; ++j) {
            float h_j = h_prev[j];
            dUf[i * hidden_dim + j] += df_ * h_j;
            dUi[i * hidden_dim + j] += di_ * h_j;
            dUc[i * hidden_dim + j] += dc_tilde * h_j;
            dUo[i * hidden_dim + j] += do_ * h_j;
            
            // Propaga o erro de volta para h_prev (usado no passo t-1 do BPTT)
            dh_prev_out[j] += df_ * Uf[i * hidden_dim + j] +
                              di_ * Ui[i * hidden_dim + j] +
                              dc_tilde * Uc[i * hidden_dim + j] +
                              do_ * Uo[i * hidden_dim + j];
        }

        // Acumula gradientes nos bias
        dbf[i] += df_;
        dbi[i] += di_;
        dbc[i] += dc_tilde;
        dbo[i] += do_;

        // Propaga o erro de volta para a entrada x_t (para camadas anteriores)
        for (size_t j = 0; j < input_dim; ++j) {
            dx_t[j] += df_ * Wf[i * input_dim + j] +
                       di_ * Wi[i * input_dim + j] +
                       dc_tilde * Wc[i * input_dim + j] +
                       do_ * Wo[i * input_dim + j];
        }
    }

    return dx_t;
}

// Atualiza os pesos usando os gradientes acumulados (descida do gradiente):
//   peso = peso - taxa_aprendizado * gradiente
// Depois zera todos os gradientes para o próximo passo de treinamento.
void LSTMCell::atualizarPesos(float taxa_aprendizado) {
    auto atualizar = [&](std::vector<float>& pesos, const std::vector<float>& grads) {
        for (size_t i = 0; i < pesos.size(); ++i) {
            pesos[i] -= taxa_aprendizado * grads[i];
        }
    };

    atualizar(Wf, dWf); atualizar(Wi, dWi); atualizar(Wc, dWc); atualizar(Wo, dWo);
    atualizar(Uf, dUf); atualizar(Ui, dUi); atualizar(Uc, dUc); atualizar(Uo, dUo);
    atualizar(bf, dbf); atualizar(bi, dbi); atualizar(bc, dbc); atualizar(bo, dbo);

    // Zera os gradientes após a atualização (pronto para o próximo forward+backward)
    std::fill(dWf.begin(), dWf.end(), 0.0f); std::fill(dWi.begin(), dWi.end(), 0.0f);
    std::fill(dWc.begin(), dWc.end(), 0.0f); std::fill(dWo.begin(), dWo.end(), 0.0f);
    std::fill(dUf.begin(), dUf.end(), 0.0f); std::fill(dUi.begin(), dUi.end(), 0.0f);
    std::fill(dUc.begin(), dUc.end(), 0.0f); std::fill(dUo.begin(), dUo.end(), 0.0f);
    std::fill(dbf.begin(), dbf.end(), 0.0f); std::fill(dbi.begin(), dbi.end(), 0.0f);
    std::fill(dbc.begin(), dbc.end(), 0.0f); std::fill(dbo.begin(), dbo.end(), 0.0f);
}

// Salva os pesos da LSTM em formato JSON para reuso posterior.
// Permite que a Natacha carregue o modelo treinado sem retreinar do zero.
void LSTMCell::salvar(const std::string& caminho) const {
    json j;
    j["input_dim"] = input_dim;
    j["hidden_dim"] = hidden_dim;
    j["Wf"] = Wf; j["Wi"] = Wi; j["Wc"] = Wc; j["Wo"] = Wo;
    j["Uf"] = Uf; j["Ui"] = Ui; j["Uc"] = Uc; j["Uo"] = Uo;
    j["bf"] = bf; j["bi"] = bi; j["bc"] = bc; j["bo"] = bo;

    std::ofstream arquivo(caminho);
    if (arquivo.is_open()) {
        arquivo << j.dump(2);
        arquivo.close();
        std::cout << "  LSTM salva em: " << caminho << std::endl;
    }
}

// Carrega os pesos da LSTM de um arquivo JSON previamente salvo.
bool LSTMCell::carregar(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo.is_open()) return false;

    json j;
    try {
        arquivo >> j;
    } catch (...) {
        return false;
    }
    arquivo.close();

    input_dim = j["input_dim"];
    hidden_dim = j["hidden_dim"];

    Wf = j["Wf"].get<std::vector<float>>();
    Wi = j["Wi"].get<std::vector<float>>();
    Wc = j["Wc"].get<std::vector<float>>();
    Wo = j["Wo"].get<std::vector<float>>();
    Uf = j["Uf"].get<std::vector<float>>();
    Ui = j["Ui"].get<std::vector<float>>();
    Uc = j["Uc"].get<std::vector<float>>();
    Uo = j["Uo"].get<std::vector<float>>();
    bf = j["bf"].get<std::vector<float>>();
    bi = j["bi"].get<std::vector<float>>();
    bc = j["bc"].get<std::vector<float>>();
    bo = j["bo"].get<std::vector<float>>();

    return true;
}
