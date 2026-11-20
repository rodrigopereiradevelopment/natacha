# Fase 5 — Self-Attention

> *"Antes de aprender a falar, a Natacha precisou aprender a olhar para trás sem perder o fio."*

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 1.0 | Data: Novembro/2026
Documentos canônicos relacionados: ROADMAP.md v3.1, ARQUITETURA.md v2.1, REQUISITOS.md v3.1, GLOSSARIO.md v2.1, FASE4.md v2.1

A Fase 5 implementa o mecanismo de atenção — a peça que substitui a recorrência
da Fase 4 por acesso direto ao contexto. Este documento registra a matemática
**exatamente como está no código**, as dimensões de cada tensor e a validação de
cada gradiente por diferença numérica. É o material de defesa da banca: toda
fórmula abaixo corresponde a uma linha identificável em `src/attention/`.

## 5.0 — Notação e convenções

| Símbolo | Significado | Forma no código |
|---|---|---|
| $B$ | número de tokens da sequência (`seq_len`) | linhas da matriz |
| $d_{model}$ | dimensão do modelo | colunas da entrada |
| $H$ | número de cabeças (`num_heads`) | — |
| $d_k$ | dimensão por cabeça, $d_k = d_{model}/H$ | `d_k` |
| $d_{ff}$ | dimensão interna do FeedForward | `d_ff` |
| $X$ | entrada do bloco, $B \times d_{model}$ | `input`, `x` |
| $A$ | matriz de pesos de atenção, $B \times B$ | `cache.A` |
| $\odot$ | produto elemento a elemento | `.array() * .array()` |

Convenções:

- Cada **linha** é um token; cada **coluna** é uma dimensão.
- O gradiente que chega a uma camada é sempre denotado por $dY$ (ou $dO$ na
  atenção), com a mesma forma da saída dessa camada.
- O forward **guarda caches** de tudo que o backward precisa. Sem cache, não há
  gradiente analítico barato.
- Todos os testes usam `srand(42)` quando há aleatoriedade, para reprodutibilidade.

## 5.1 — Single-Head Attention

Arquivo: `src/attention/AttentionLayer.cpp`
Funções: `single_head_attention`, `single_head_attention_backward`, `softmax_estavel`

### Forward

Dadas as projeções $Q$ ($B \times d_k$), $K$ ($B \times d_k$) e $V$ ($B \times d_v$):

$$
S = \frac{Q K^\top}{\sqrt{d_k}} \qquad (B \times B)
$$

O fator $1/\sqrt{d_k}$ impede que o produto interno cresça com a dimensão e
empurre o softmax para uma região de saturação (gradiente quase nulo).

**Máscara causal.** Para todo $j > i$:

$$
S_{ij} \leftarrow -10^{9}
$$

Isso garante que o token $i$ só enxergue posições $\le i$ — condição necessária
para geração autoregressiva. O valor $-10^9$ é um substituto numérico de
$-\infty$: depois do softmax, $e^{-10^9} = 0$ em ponto flutuante.

**Softmax estável**, linha a linha:

$$
A_{ij} = \frac{\exp(S_{ij} - m_i)}{\sum_{k=1}^{B} \exp(S_{ik} - m_i)},
\qquad m_i = \max_k S_{ik}
$$

Subtrair o máximo não altera o resultado matemático (o softmax é invariante a
deslocamento constante) e evita `overflow` no `exp`.

**Saída:**

$$
O = A V \qquad (B \times d_v)
$$

Note que $d_v = d_k$ nesta implementação, porque o `MultiHeadAttention` fatia
$V$ com a mesma largura de $Q$ e $K$.

### Backward

Recebendo $dO$ ($B \times d_v$):

$$
dV = A^\top dO
$$

$$
dA = dO\, V^\top
$$

O gradiente do softmax, para cada linha $i$:

$$
dS_{ij} = A_{ij}\left(dA_{ij} - \sum_{k=1}^{B} dA_{ik} A_{ik}\right)
$$

O termo $\sum_k dA_{ik} A_{ik}$ é o produto interno entre o gradiente e a
distribuição de probabilidade daquela linha. A subtração é o que corrige o fato
de as saídas do softmax somarem 1 — mexer numa probabilidade obriga a mexer nas
outras.

Como $S = QK^\top / \sqrt{d_k}$, o gradiente atravessa a escala:

$$
dS \leftarrow \frac{dS}{\sqrt{d_k}}
$$

E então:

$$
dQ = dS\, K, \qquad dK = dS^\top Q
$$

A máscara não aparece explicitamente no backward porque $A_{ij} = 0$ nas
posições mascaradas — o produto $A_{ij}(\cdot)$ já zera a contribuição.

## 5.2 — Multi-Head Attention

Arquivos: `src/attention/MultiHeadAttention.hpp` / `.cpp`
Classe: `MultiHeadAttention`

### Forward

Projeções globais, todas $d_{model} \times d_{model}$:

$$
Q_g = X W_q, \qquad K_g = X W_k, \qquad V_g = X W_v
$$

Cada cabeça $h \in \{0, \dots, H-1\}$ recebe o bloco de colunas
$[h\,d_k,\ (h+1)d_k)$:

$$
Q_h = Q_g[:,\ h d_k : (h+1)d_k]
$$

e analogamente para $K_h$ e $V_h$. A atenção é aplicada por cabeça:

$$
\text{head}_h = \text{Attention}(Q_h, K_h, V_h)
$$

Concatenação horizontal:

$$
C = [\text{head}_0 \ \| \ \text{head}_1 \ \| \ \cdots \ \| \ \text{head}_{H-1}]
\qquad (B \times d_{model})
$$

Projeção final:

$$
O = C W_o \qquad (B \times d_{model})
$$

A ideia é que cabeças diferentes aprendam relações diferentes (posição,
concordância, tópico) e a projeção $W_o$ misture esses pontos de vista.

### Backward

$$
dW_o = C^\top dO
$$

$$
dC = dO\, W_o^\top
$$

Para cada cabeça, fatia-se $dC$ e chama-se o backward da atenção simples:

$$
dO_h = dC[:,\ h d_k : (h+1)d_k]
$$

$$
(dQ_h, dK_h, dV_h) = \text{AttentionBackward}(Q_h, K_h, V_h, A_h, dO_h)
$$

Os gradientes dos pesos são acumulados **no bloco de colunas correspondente**:

$$
dW_q[:,\ h d_k : (h+1)d_k] = X^\top dQ_h
$$

(o mesmo para $W_k$ com $dK_h$ e $W_v$ com $dV_h$). Por fim, o gradiente da
entrada soma a contribuição das três projeções de todas as cabeças:

$$
dX = \sum_{h=0}^{H-1}
\left(
dQ_h W_{q,h}^\top + dK_h W_{k,h}^\top + dV_h W_{v,h}^\top
\right)
$$

### Contagem de parâmetros

Para $d_{model} = 16$, $H = 4$, $d_k = 4$:

| Componente | Parâmetros |
|---|---:|
| $W_q, W_k, W_v, W_o$ | $4 \times 16^2 = 1024$ |

## 5.3 — Layer Normalization

Arquivos: `src/attention/LayerNorm.hpp` / `.cpp`
Classe: `LayerNorm`

A normalização é feita **por token** (por linha), não por lote. É isso que torna
a LayerNorm adequada a sequências de tamanho variável.

### Forward

Para cada linha $i$, com $N = d_{model}$:

$$
\mu_i = \frac{1}{N}\sum_{j=1}^{N} x_{ij}
$$

$$
\sigma_i^2 = \frac{1}{N}\sum_{j=1}^{N}(x_{ij} - \mu_i)^2
$$

$$
\hat{x}_{ij} = \frac{x_{ij} - \mu_i}{\sqrt{\sigma_i^2 + \varepsilon}}
$$

$$
y_{ij} = \gamma_j \hat{x}_{ij} + \beta_j
$$

Detalhes que a banca pode cobrar:

- A variância é **enviesada** (divide por $N$, não por $N-1$). É o padrão da
  LayerNorm e não muda o comportamento, só a escala.
- $\varepsilon = 10^{-5}$ por padrão (`eps` no construtor), configurável.
- $\gamma$ começa em 1 e $\beta$ em 0, então a camada é inicialmente a
  identidade em torno da normalização.
- O forward guarda `xhat_cache` e `inv_std_cache` ($1/\sqrt{\sigma_i^2 + \varepsilon}$).

### Backward

Com $dY$ ($B \times N$):

$$
d\beta_j = \sum_{i=1}^{B} dY_{ij}
$$

$$
d\gamma_j = \sum_{i=1}^{B} dY_{ij}\,\hat{x}_{ij}
$$

$$
d\hat{x}_{ij} = dY_{ij}\,\gamma_j
$$

E o gradiente em relação à entrada, por linha:

$$
dx_{ij} = \frac{1}{\sqrt{\sigma_i^2 + \varepsilon}}
\left[
d\hat{x}_{ij}
- \frac{1}{N}\sum_{k} d\hat{x}_{ik}
- \hat{x}_{ij} \cdot \frac{1}{N}\sum_{k} d\hat{x}_{ik}\hat{x}_{ik}
\right]
$$

Os dois termos de correção vêm de $\mu$ e $\sigma$ dependerem de todos os $x$
da mesma linha: mudar uma coordenada desloca a média e encolhe/expande o desvio,
e o gradiente precisa descontar isso.

## 5.4 — FeedForward Network

Arquivos: `src/attention/FeedForward.hpp` / `.cpp`
Classe: `FeedForward`

Rede posicional aplicada a cada token de forma independente:

$$
h = X W_1 + b_1 \qquad (B \times d_{ff})
$$

$$
a = \text{ReLU}(h) = \max(0, h)
$$

$$
Y = a W_2 + b_2 \qquad (B \times d_{model})
$$

Com $d_{ff} = 4\,d_{model}$ no Transformer original. No teste de overfit,
$d_{model} = 16$ e $d_{ff} = 32$.

### Backward

$$
dW_2 = a^\top dY, \qquad db_2 = \sum_{i=1}^{B} dY_{i,:}
$$

$$
da = dY\, W_2^\top
$$

$$
dh = da \odot \mathbb{1}[h > 0]
$$

$$
dW_1 = X^\top dh, \qquad db_1 = \sum_{i=1}^{B} dh_{i,:}
$$

$$
dX = dh\, W_1^\top
$$

O gradiente da ReLU é a máscara binária $h > 0$ — guarda-se `h_cache`, a saída
**antes** da ativação, justamente para reconstruir essa máscara.

### Contagem de parâmetros

Para $d_{model} = 16$, $d_{ff} = 32$:

| Componente | Parâmetros |
|---|---:|
| $W_1, b_1$ | $16 \times 32 + 32 = 544$ |
| $W_2, b_2$ | $32 \times 16 + 16 = 528$ |
| **Total** | **1072** |

## 5.5 — Transformer Block (Pre-Norm)

Arquivos: `src/attention/TransformerBlock.hpp` / `.cpp`
Classe: `TransformerBlock`

O bloco usa o esquema **Pre-Norm**: a normalização vem **antes** da subcamada, e
a conexão residual soma a entrada da subcamada à sua saída.

### Forward

$$
\ell_1 = \text{LN}_1(x)
$$

$$
r_1 = x + \text{MHA}(\ell_1)
$$

$$
\ell_2 = \text{LN}_2(r_1)
$$

$$
y = r_1 + \text{FF}(\ell_2)
$$

Por que Pre-Norm: o caminho residual fica livre de normalização, então o
gradiente flui da saída à entrada sem ser reescalado a cada bloco. Em redes
profundas isso dispensa o aquecimento (warmup) agressivo típico do Post-Norm.

### Backward

Como há dois caminhos (identidade e subcamada) em cada residual, o gradiente se
divide:

$$
d_{ff} = \text{FF.backward}(dY)
$$

$$
d_{\ell_2} = \text{LN}_2\text{.backward}(d_{ff})
$$

$$
d_{r_1} = dY + d_{\ell_2}
$$

$$
d_{attn} = \text{MHA.backward}(d_{r_1})
$$

$$
d_{\ell_1} = \text{LN}_1\text{.backward}(d_{attn})
$$

$$
dX = d_{r_1} + d_{\ell_1}
$$

Cada `+` é a conexão residual: a identidade não atenua nem transforma o
gradiente, apenas o soma ao caminho transformado.

### Contagem de parâmetros

Para $d_{model} = 16$, $H = 4$, $d_{ff} = 32$:

| Componente | Parâmetros |
|---|---:|
| MHA ($W_q, W_k, W_v, W_o$) | 1024 |
| $\text{LN}_1$ ($\gamma, \beta$) | 32 |
| $\text{LN}_2$ ($\gamma, \beta$) | 32 |
| FeedForward | 1072 |
| **Total** | **2160** |

## 5.6 — Validação por gradiente numérico

Nenhuma fórmula acima foi aceita por inspeção. Cada backward foi comparado à
derivada numérica central:

$$
\frac{\partial L}{\partial \theta}
\approx
\frac{L(\theta + \varepsilon) - L(\theta - \varepsilon)}{2\varepsilon}
$$

com $\varepsilon = 10^{-6}$ (LayerNorm e FeedForward) ou $10^{-5}$ (atenção).

A perda usada no teste é sempre uma forma bilinear que isola o gradiente que se
quer verificar:

- **Atenção:** $L = \sum_{ij} O_{ij}$, portanto $dO = \mathbf{1}$.
- **LayerNorm e FeedForward:** $L = \langle Y, dY \rangle = \sum_{ij} Y_{ij}\,dY_{ij}$,
  com $dY$ aleatório. Isso testa o gradiente da entrada contra todas as direções
  de uma vez.

### Resultados registrados

| Teste | Arquivo | Erro máximo | Limite |
|---|---|---:|---:|
| Self-attention ($dW_q$, todos os elementos) | `src/attention/test_attention_original.cpp` | $3{,}50 \times 10^{-11}$ | $10^{-4}$ |
| LayerNorm ($dx$, $B=4$, $d=8$) | `test_grad_ln.cpp` | $6{,}58 \times 10^{-10}$ | $10^{-6}$ |
| FeedForward ($dx$, $B=4$, $d_{model}=8$, $d_{ff}=16$) | `test_grad_ff.cpp` | $1{,}46 \times 10^{-10}$ | $10^{-6}$ |

Os três erros estão na ordem do erro de truncamento do método central
($O(\varepsilon^2)$) somado ao arredondamento de ponto flutuante — ou seja, o
backward analítico está correto.

### Como reproduzir

```bash
cd ~/natacha

# Self-attention (o .cpp inclui AttentionLayer.cpp)
g++ -std=c++17 -O2 src/attention/test_attention_original.cpp \
    -I/usr/include/eigen3 -o build/test_attention
./build/test_attention

# LayerNorm
g++ -std=c++17 -O2 test_grad_ln.cpp src/attention/LayerNorm.cpp \
    -I/usr/include/eigen3 -o build/test_grad_ln
./build/test_grad_ln

# FeedForward
g++ -std=c++17 -O2 test_grad_ff.cpp src/attention/FeedForward.cpp \
    -I/usr/include/eigen3 -o build/test_grad_ff
./build/test_grad_ff
```

## 5.7 — Teste de overfit do bloco completo

Um bloco com backward correto precisa conseguir decorar um alvo aleatório. Se
não decora nem isso, há erro de gradiente ou de atualização. O teste
`test_overfit.cpp` treina o bloco inteiro com SGD simples:

| Parâmetro | Valor |
|---|---|
| Sequência ($B$) | 3 tokens |
| $d_{model}$ | 16 |
| Cabeças ($H$) | 4 ($d_k = 4$) |
| $d_{ff}$ | 32 |
| Épocas | 500 |
| Learning rate | 0,01 |
| Semente | 42 |
| Perda | MSE: $\frac{1}{B\,d_{model}}\sum (y - t)^2$ |

Gradiente da MSE usado no backward:

$$
dY = \frac{2\,(y - t)}{B\,d_{model}}
$$

Atualização (SGD, sem momento):

$$
\theta \leftarrow \theta - \eta\, \frac{\partial L}{\partial \theta}
$$

para $W_q, W_k, W_v, W_o$, $W_1, b_1, W_2, b_2$, $\gamma_1, \beta_1,
\gamma_2, \beta_2$.

### Resultado registrado

```text
Perda inicial: 35.7537
Perda final:   0.000742045
Redução:       99.9979%
```

O bloco aprendeu a mapear a entrada aleatória para o alvo aleatório. Isso valida
o encadeamento forward/backward de todos os componentes — inclusive os
residuais e as duas LayerNorms — de ponta a ponta.

### Como reproduzir

```bash
cd ~/natacha
g++ -std=c++17 -O2 test_overfit.cpp \
    src/attention/TransformerBlock.cpp \
    src/attention/MultiHeadAttention.cpp \
    src/attention/AttentionLayer.cpp \
    src/attention/LayerNorm.cpp \
    src/attention/FeedForward.cpp \
    -I/usr/include/eigen3 -o build/test_overfit
./build/test_overfit
```

## 5.8 — Estrutura dos arquivos

```text
src/attention/
├── AttentionLayer.hpp / .cpp     ← softmax estável, single-head, backward
├── MultiHeadAttention.hpp / .cpp ← projeções, fatias por cabeça, Wo, backward
├── LayerNorm.hpp / .cpp          ← normalização por token, gamma/beta, backward
├── FeedForward.hpp / .cpp        ← W1, ReLU, W2, backward
├── TransformerBlock.hpp / .cpp   ← Pre-Norm + residuais + backward encadeado
├── test_attention_original.cpp   ← validação numérica da atenção
├── test_mha.cpp                  ← smoke test de formas do MHA
└── test_frase.cpp                ← MHA sobre uma frase (embeddings mock)
```

Testes na raiz:

```text
test_grad_ln.cpp     ← gradiente do LayerNorm
test_grad_ff.cpp     ← gradiente do FeedForward
test_overfit.cpp     ← overfit do bloco completo
```

## 5.9 — O que a Fase 5 entrega e o que falta para a Fase 6

Entregue e validado:

- ✅ Self-attention com máscara causal e softmax estável
- ✅ Multi-Head Attention com $H$ cabeças e projeção de saída
- ✅ LayerNorm com forward e backward
- ✅ FeedForward com ReLU, forward e backward
- ✅ Bloco Transformer Pre-Norm com residuais e backward encadeado
- ✅ Gradientes validados por diferença numérica (erro < $10^{-9}$)
- ✅ Overfit do bloco (redução de 99,9979% da perda)

Falta para o modelo completo da Fase 6:

- ☐ Embeddings posicionais (sem eles, a atenção é invariante a permutação e não
  sabe a ordem dos tokens)
- ☐ Cabeça de saída sobre o vocabulário + função de perda de linguagem
  (cross-entropy)
- ☐ Loop de treino no corpus e geração autoregressiva
- ☐ Empilhamento de $N$ blocos e escolha de escala ($d_{model}$, $H$, $d_{ff}$)

O bloco atual é a peça validada sobre a qual a Fase 6 será montada. Nenhuma
fórmula deste documento depende do corpus: a validação é matemática, com
entradas aleatórias de semente fixa.
