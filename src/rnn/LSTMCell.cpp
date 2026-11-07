#include "LSTMCell.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cmath>
#include "json.hpp"
using json = nlohmann::json;

LSTMCell::LSTMCell(size_t input_dim, size_t hidden_dim)
    : input_dim(input_dim), hidden_dim(hidden_dim) {
    
    size_t w_size = hidden_dim * input_dim;
    size_t u_size = hidden_dim * hidden_dim;

    Wf.resize(w_size); Wi.resize(w_size); Wc.resize(w_size); Wo.resize(w_size);
    Uf.resize(u_size); Ui.resize(u_size); Uc.resize(u_size); Uo.resize(u_size);
    
    bf.resize(hidden_dim, 1.0f); 
    bi.resize(hidden_dim, 0.0f); 
    bc.resize(hidden_dim, 0.0f); 
    bo.resize(hidden_dim, 0.0f);

    dWf.resize(w_size, 0.0f); dWi.resize(w_size, 0.0f); dWc.resize(w_size, 0.0f); dWo.resize(w_size, 0.0f);
    dUf.resize(u_size, 0.0f); dUi.resize(u_size, 0.0f); dUc.resize(u_size, 0.0f); dUo.resize(u_size, 0.0f);
    dbf.resize(hidden_dim, 0.0f); dbi.resize(hidden_dim, 0.0f); 
    dbc.resize(hidden_dim, 0.0f); dbo.resize(hidden_dim, 0.0f);

    inicializarPesos();
}

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

LSTMState LSTMCell::forward(const std::vector<float>& x_t, 
                          const std::vector<float>& h_prev, 
                          const std::vector<float>& c_prev) {

    std::vector<float> h_next(hidden_dim, 0.0f);
    std::vector<float> c_next(hidden_dim, 0.0f);

    for (size_t i = 0; i < hidden_dim; ++i) {
        float f_gate_in = bf[i];
        float i_gate_in = bi[i];
        float c_tilde_in = bc[i];
        float o_gate_in = bo[i];

        for (size_t j = 0; j < input_dim; ++j) {
            float x_j = x_t[j]; // Confiando no contrato de dimensão
            f_gate_in += Wf[i * input_dim + j] * x_j;
            i_gate_in += Wi[i * input_dim + j] * x_j;
            c_tilde_in += Wc[i * input_dim + j] * x_j;
            o_gate_in += Wo[i * input_dim + j] * x_j;
        }

        for (size_t j = 0; j < hidden_dim; ++j) {
            float h_j = h_prev[j];
            f_gate_in += Uf[i * hidden_dim + j] * h_j;
            i_gate_in += Ui[i * hidden_dim + j] * h_j;
            c_tilde_in += Uc[i * hidden_dim + j] * h_j;
            o_gate_in += Uo[i * hidden_dim + j] * h_j;
        }

        float f_t = sigmoid(f_gate_in);
        float i_t = sigmoid(i_gate_in);
        float c_tilde = std::tanh(c_tilde_in);
        float o_t = sigmoid(o_gate_in);

        float c_p = c_prev[i];
        c_next[i] = (f_t * c_p) + (i_t * c_tilde);
        h_next[i] = o_t * std::tanh(c_next[i]);
    }

    return {h_next, c_next};
}

std::vector<float> LSTMCell::backward(
    const std::vector<float>& x_t,
    const std::vector<float>& h_prev,
    const std::vector<float>& c_prev,
    const std::vector<float>& dh_total, // renomeado conceitualmente
    const std::vector<float>& dc_next,
    std::vector<float>& dh_prev_out,
    std::vector<float>& dc_prev_out) {

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

    // CORREÇÃO: Usar assign para garantir o tamanho (o fill não redimensiona)
    dh_prev_out.assign(hidden_dim, 0.0f);
    dc_prev_out.assign(hidden_dim, 0.0f);
    std::vector<float> dx_t(input_dim, 0.0f);

    for (size_t i = 0; i < hidden_dim; ++i) {
        float dh = dh_total[i];
        float dc = dc_next[i];

        float tanh_c = std::tanh(c_current[i]);
        dc += dh * o_t[i] * (1.0f - tanh_c * tanh_c);

        float do_ = dh * tanh_c * o_t[i] * (1.0f - o_t[i]);
        float df_ = dc * c_prev[i] * f_t[i] * (1.0f - f_t[i]);
        float di_ = dc * c_tilde[i] * i_t[i] * (1.0f - i_t[i]);
        float dc_tilde = dc * i_t[i] * (1.0f - c_tilde[i] * c_tilde[i]);

        dc_prev_out[i] = dc * f_t[i];

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
            
            dh_prev_out[j] += df_ * Uf[i * hidden_dim + j] +
                              di_ * Ui[i * hidden_dim + j] +
                              dc_tilde * Uc[i * hidden_dim + j] +
                              do_ * Uo[i * hidden_dim + j];
        }

        dbf[i] += df_;
        dbi[i] += di_;
        dbc[i] += dc_tilde;
        dbo[i] += do_;

        for (size_t j = 0; j < input_dim; ++j) {
            dx_t[j] += df_ * Wf[i * input_dim + j] +
                       di_ * Wi[i * input_dim + j] +
                       dc_tilde * Wc[i * input_dim + j] +
                       do_ * Wo[i * input_dim + j];
        }
    }

    return dx_t;
}

void LSTMCell::atualizarPesos(float taxa_aprendizado) {
    auto atualizar = [&](std::vector<float>& pesos, const std::vector<float>& grads) {
        for (size_t i = 0; i < pesos.size(); ++i) {
            pesos[i] -= taxa_aprendizado * grads[i];
        }
    };

    atualizar(Wf, dWf); atualizar(Wi, dWi); atualizar(Wc, dWc); atualizar(Wo, dWo);
    atualizar(Uf, dUf); atualizar(Ui, dUi); atualizar(Uc, dUc); atualizar(Uo, dUo);
    atualizar(bf, dbf); atualizar(bi, dbi); atualizar(bc, dbc); atualizar(bo, dbo);

    std::fill(dWf.begin(), dWf.end(), 0.0f); std::fill(dWi.begin(), dWi.end(), 0.0f);
    std::fill(dWc.begin(), dWc.end(), 0.0f); std::fill(dWo.begin(), dWo.end(), 0.0f);
    std::fill(dUf.begin(), dUf.end(), 0.0f); std::fill(dUi.begin(), dUi.end(), 0.0f);
    std::fill(dUc.begin(), dUc.end(), 0.0f); std::fill(dUo.begin(), dUo.end(), 0.0f);
    std::fill(dbf.begin(), dbf.end(), 0.0f); std::fill(dbi.begin(), dbi.end(), 0.0f);
    std::fill(dbc.begin(), dbc.end(), 0.0f); std::fill(dbo.begin(), dbo.end(), 0.0f);
}

// As funções salvar e carregar permanecem as mesmas...

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