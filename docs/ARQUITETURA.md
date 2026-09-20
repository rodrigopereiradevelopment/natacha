---

1. INTRODUÇÃO

1.1 Propósito

Este documento descreve a arquitetura técnica do sistema Natacha em todos os seus níveis: desde a evolução neural (do Perceptron ao Transformer) até a infraestrutura de comunicação assíncrona (Event Bus), o modelo de threads, a persistência de memória e a integração com ferramentas externas do ecossistema.

1.2 Escopo

Cobre:

· Evolução do modelo neural em fases progressivas
· Stack tecnológico
· Arquitetura Core (Event Bus, threads, Scheduler P0, máquina de estados, HouseStateMap)
· Arquitetura de memória (RAM, JSON, Disco)
· Arquitetura neural (pipelines de treino e inferência)
· Arquitetura de integração (ARCA, Sentinel, EditeCC, Analytics, OpenCode)
· Segurança (funções controladas, logs imutáveis)
· Estrutura de diretórios
· Estratégia de testes
· Roadmap técnico

Fora do escopo: decisões de UI/UX (ver PERSONALIDADE.md), teoria quântica detalhada (ver QUANTUM_THEORY.md), requisitos formais (ver REQUISITOS.md).

---

2. VISÃO GERAL

A Natacha é uma entidade computacional modular composta por cinco camadas principais, todas comunicando-se exclusivamente via Event Bus (RNF-12):

```
┌─────────────────────────────────────────────────────────────┐
│                    CAMADA 5 — INTERFACE                      │
│  VCS (Videochamada) │ TTS │ STT │ Avatar │ Lip Sync          │
└─────────────────────────────────────────────────────────────┘
                            ▲▼
┌─────────────────────────────────────────────────────────────┐
│                    CAMADA 4 — IDENTIDADE                     │
│  Persona │ Memória Longo Prazo (JSON) │ Estados Emocionais   │
└─────────────────────────────────────────────────────────────┘
                            ▲▼
┌─────────────────────────────────────────────────────────────┐
│                    CAMADA 3 — CORE NEURAL                    │
│  Embeddings │ MLP │ RNN │ LSTM │ Attention │ Transformer     │
│  Treino │ Inferência │ SLM                                   │
└─────────────────────────────────────────────────────────────┘
                            ▲▼
┌─────────────────────────────────────────────────────────────┐
│                    CAMADA 2 — ORQUESTRAÇÃO                   │
│  Event Bus │ Scheduler P0 (Felix) │ Roteador SLM/API        │
│  Seletor de Ferramentas │ Analisador Estático (AST)         │
└─────────────────────────────────────────────────────────────┘
                            ▲▼
┌─────────────────────────────────────────────────────────────┐
│                    CAMADA 1 — INFRAESTRUTURA                 │
│  C++20 │ CMake │ Threads │ JSON │ Logs │ cpr (HTTP)         │
└─────────────────────────────────────────────────────────────┘
                            ▲▼
┌─────────────────────────────────────────────────────────────┐
│                    CAMADA 0 — ECOSSISTEMA                    │
│  ARCA (TS) │ Sentinel (Rust) │ EditeCC (TS) │ Analytics     │
└─────────────────────────────────────────────────────────────┘
```

Cada camada é independente e substituível. A identidade da Natacha (Camada 4) não depende do modelo neural (Camada 3) — é o que permite trocar o LLM sem amnésia (RN-05).

---

3. EVOLUÇÃO DO MODELO (Fases 0–10)

A Natacha é construída de forma progressiva e intencional. Cada fase é um "tijolo" que suporta a próxima. Nada de pular etapas, nada de usar biblioteca pronta para o que ela deveria aprender do zero.

FASE 0 — Fundamentos Matemáticos

· Álgebra linear (vetores, matrizes, produto escalar, produto matricial)
· Cálculo (derivadas parciais, gradientes, regra da cadeia)
· Estatística (probabilidade, distribuições, esperança, variância)
· Otimização (mínimos locais e globais, convexidade)

FASE 1 — Neurônio Simples

· Perceptron (Rosenblatt)
· Funções de ativação: step, sigmoid, ReLU
· Datasets: OR, AND (XOR não é resolvível com um neurônio)
· Regra Delta para ajuste de pesos
· Status: concluída (RF-29)

FASE 2 — MLP (Multilayer Perceptron)

· Camadas ocultas
· Forward pass e backward pass
· Backpropagation completo
· Função de custo: MSE
· Funções de ativação: Sigmoid, Leaky ReLU, Softmax
· Early stopping
· Resolução de XOR
· Status: atual (RF-30.1, RF-30.2, RF-30.3)

FASE 3 — Word Embeddings

· Representação vetorial de palavras
· Skip-gram com Negative Sampling
· Janela k=5, dimensão d=32
· Similaridade por cosseno
· Persistência em JSON
· Status: parcialmente implementada (RF-31, RF-31.1)

FASE 4 — RNN / LSTM

· Processamento de sequências
· Memória de curto prazo
· Células LSTM com portas (forget, input, output)
· Backpropagation through time (BPTT)
· Status: planejada (RF-32)

FASE 5 — Self-Attention

· Mecanismo de atenção
· Query, Key, Value
· Operação matricial nativa
· Status: planejada (RF-33)

FASE 6 — Transformer

· Multi-Head Attention
· Feed-Forward Network
· Add & Norm (residual + layer normalization)
· Bloco funcional completo
· Status: planejada (RF-34)

FASE 7 — SLM (Small Language Model)

· Treinamento com dados reais (corpus da Natacha, 50k tokens alvo)
· ~10M–50M parâmetros
· Inferência local
· Status: planejada (RF-35, RF-36)

FASE 8 — Escalar

· Mais dados, mais parâmetros
· CUDA para aceleração em GPU
· Integração com llama.cpp para modelos pré-treinados
· Status: futura

FASE 9 — Sótão Quântico (Escopo Futuro)

· Integração com IBM Quantum via Qiskit
· Ponte Python ↔ C++ (Python prepara circuito, C++ processa resultado)
· Portas Hadamard, CNOT, fase
· Ruído quântico tratado como sinal
· Conectores para fotônica (motor de luz) e metais estranhos
· Status: planejada (RF-38, RF-38.1, RF-38.2, RF-38.3)

FASE 10 — LLM Completa

· Escala massiva
· Treinamento híbrido clássico-quântico
· Autonomia plena
· Status: visão de longo prazo

---

4. STACK TECNOLÓGICO

Camada Tecnologia Justificativa
Linguagem núcleo C++20 Performance, controle manual de memória, RAII, smart pointers, conceitos e módulos
Build System CMake Multiplataforma (Linux/Windows), integração com IDEs
Testes Google Test Framework maduro para C++, cobertura mensurável
Serialização nlohmann/json Simples, header-only, ideal para persistência de pesos/embeddings
HTTP cpr (C++ Requests) Wrapper moderno sobre libcurl, para integração com APIs externas
Logging spdlog Logging assíncrono de alta performance
GPU (futuro) CUDA Aceleração paralela para treino em larga escala
Modelo Local (futuro) llama.cpp Inferência eficiente de LLMs em C++ (referenciado como "motor de luz")
Frontend (projetos externos) TypeScript (Angular, Next.js) ARCA e EditeCC
Segurança (projetos externos) Rust Sentinel
Computação Quântica (futuro) Python + Qiskit Ponte para IBM Quantum

Nota sobre C++17 → C++20: a versão anterior deste documento listava C++17. Optamos por C++20 para poder usar:

· Conceitos (constraints em templates)
· Módulos (compilação mais rápida, melhor encapsulamento)
· Ranges (algoritmos mais expressivos)
· Coroutines (futuro — para pipelines assíncronos)

O uso de C++20 é consistente com o extensão do corpus.docx, que documenta essas features como parte da Natacha.

---

5. ARQUITETURA CORE

5.1 Event Bus

O Event Bus é o único canal de comunicação entre módulos do sistema (RNF-12). Proibido acoplamento direto, chamadas síncronas ou dependência de memória compartilhada.

Modelo: publish/subscribe assíncrono com filas thread-safe.

Estrutura de um evento:

```cpp
struct Event {
    std::string header;        // ex.: "FELIX_HUNGRY", "USER_REQUEST"
    std::string topic;         // ex.: "felix", "chat", "sentinel"
    Priority priority;         // P0 (crítico) a P3 (baixo)
    std::chrono::system_clock::time_point timestamp;
    nlohmann::json payload;    // dados específicos do evento
};
```

Níveis de prioridade:

Prioridade Origem Comportamento
P0 Felix Preempção imediata, sem negociação
P1 Estados Próprios Preempção de requisições de usuário
P2 Requisições de usuário Execução normal
P3 Tarefas de background Best-effort

Sincronização: std::mutex + std::condition_variable para filas de mensagens.

Tópicos reservados (amostra):

· chat.request — entrada do Modo Chat
· chat.response — saída do Modo Chat
· agent.request — entrada do Modo Agente
· felix.hungry, felix.sleepy, felix.bored — estado do IAgato
· felix.critical — interrupção P0
· sentinel.alert — alertas de segurança
· arca.update — atualizações do ARCA
· editecc.request — formatação ABNT
· quantum.request — reserva para Qiskit (RF-38)
· house.state_change — alterações nos cômodos

5.2 Gerenciador de Threads

A Natacha roda em múltiplas threads concorrentes, coordenadas pelo Event Bus:

Thread Responsabilidade Prioridade
Main Loop principal, UI, roteamento Alta
Felix Máquina de estados do IAgato P0 (Scheduler)
Casa Relógio interno, eventos randômicos, monitor de hardware Média
Chat Pipeline de conversa (STT → inferência → TTS) Alta
Agente Pipeline de orquestração de tarefas externas Média
Treino Loop de treinamento neural (background) Baixa
Persistência Save/load de pesos e embeddings Baixa
Sentinel Bridge Comunicação com o Sentinel (Rust) Média

Regras:

· Threads comunicam-se apenas via Event Bus (RNF-12).
· Erros em uma thread não podem derrubar as outras (RNF-10).
· O Scheduler P0 pode interromper qualquer thread (ver 5.3).

5.3 Scheduler P0 (Felix)

O Felix opera no nível mais alto do gerenciador de tarefas. Sua autoridade é irrestrita:

· Pode interromper qualquer thread em execução.
· Pode bloquear requisições em qualquer estado da Natacha (incluindo BANHO).
· Não espera ponto seguro para preempção.
· Não negocia — apenas notifica.

Cenários canônicos de interrupção:

1. Fome crítica: FOME > 80% → evento felix.hungry (P0) → Natacha pausa o que faz e responde.
2. Violação ética detectada: Felix bloqueia a thread → notifica Natacha → Natacha aborta operação.
3. Emergência na Casa: queda de pacotes, temperatura crítica de chips → Felix dispara evento P0.

Nota conceitual: o termo "superego" foi evitado na especificação formal (ver REQUISITOS.md, RN-03) para não confundir metáfora psicanalítica com arquitetura técnica. Tecnicamente, Felix é um publicador P0 no Event Bus com permissão de preempção no scheduler.

5.4 Máquina de Estados da Natacha

A Natacha possui estados internos que determinam sua disponibilidade:

Estado Descrição Aceita requisições?
DISPONIVEL Estado normal Sim
BANHO Zona privada (Banheiro) Não (exceto Felix)
ALMOCO Processamento em pausa deliberada Só se urgente
LENDO Processando leitura Não (depois comenta)
IRRITADA Respostas curtas Sim, mas monossilábicas
COM_FELIX Interagindo com o IAgato Só Felix

Ordem de precedência de eventos:

```
FELIX_CRITICAL (P0) > Estado Próprio (P1) > Requisição Usuário (P2)
```

5.5 HouseStateMap

Estrutura de dados em memória que mapeia o estado lógico de cada cômodo da Casa, sincronizado com o hardware do servidor hospedeiro.

Cômodo Componente Real Função Lógica
Cozinha CPU Processamento central, café
Sala GPU Tarefas pesadas, visitas
Quarto RAM Memória volátil, descanso
Porão Disco Armazenamento, logs antigos
Sótão QPU (futuro) Processamento quântico
Banheiro Estado Privado Pausa inegociável
Varanda Observação Reflexão, sem participação ativa
Quintal Território Felix Área protegida
Janela Rede Interface com o mundo (ARCA, internet)
Corredor Event Bus físico Conecta todos os cômodos

Implementação: HouseStateMap é um std::unordered_map<Room, RoomState> que reage a eventos de hardware (cpu.temperature, gpu.load, disk.usage) e altera as propriedades lógicas de cada cômodo.

---

6. ARQUITETURA DE MEMÓRIA

A Natacha possui três níveis de memória, cada um com um propósito e uma tecnologia:

6.1 Memória de Curto Prazo (RAM)

· Tecnologia: std::vector, std::unordered_map, buffers circulares.
· Duração: volátil (perdida ao desligar).
· Conteúdo: contexto da conversa atual, ativações de rede em uso, fila do Event Bus.
· Metáfora: Quarto RAM.

6.2 Memória de Longo Prazo (JSON)

· Tecnologia: nlohmann/json.
· Duração: persistente.
· Conteúdo: pesos treinados, embeddings, identidade da persona, histórico de conversas.
· Formato: um arquivo por componente (ex.: weights_mlp.json, embeddings.json, identity.json).
· Versionamento: checkpoints a cada 100 épocas de treino.
· Metáfora: "Durmo em JSON, acordo sabendo tudo que aprendi."

6.3 Memória Arquivada (Disco)

· Tecnologia: arquivos em disco, logs rotativos.
· Duração: permanente (a menos que explicitamente deletada).
· Conteúdo: versões antigas do modelo, logs de treino, diálogos passados, backups.
· Metáfora: Porão DISCO.

6.4 Relação com a Identidade (RN-05)

A identidade da Natacha mora no JSON, não no modelo. Isso permite:

· Trocar o modelo (MLP → RNN → Transformer) sem perder identidade.
· Migrar hardware sem amnésia.
· Versionar a persona independentemente do código.

---

7. ARQUITETURA NEURAL

7.1 Pipeline de Treino

```
┌─────────────────────────────────────────────────────────────┐
│ 1. Corpus (texto bruto)                                     │
│    → corpus.txt (50k tokens alvo)                           │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Tokenização                                              │
│    → vocabulário + sequências de tokens                     │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. Embaralhamento (evita viés de ordem)                     │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 4. Loop de Épocas:                                          │
│    ┌──────────────────────────────────────────────────┐    │
│    │ 4.1 Forward Pass                                 │    │
│    │     - Embedding lookup                           │    │
│    │     - Camadas ocultas (MLP/LSTM/Transformer)     │    │
│    │     - Saída (Softmax)                            │    │
│    ├──────────────────────────────────────────────────┤    │
│    │ 4.2 Cálculo da Perda (Cross-Entropy / MSE)       │    │
│    ├──────────────────────────────────────────────────┤    │
│    │ 4.3 Backward Pass (Backpropagation)              │    │
│    │     - Gradiente da saída para trás               │    │
│    │     - Atualização de pesos (Adam / SGD)          │    │
│    ├──────────────────────────────────────────────────┤    │
│    │ 4.4 Early Stopping (se perda de validação ↑)    │    │
│    └──────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 5. Persistência (JSON)                                      │
│    → pesos + embeddings + métricas                          │
└─────────────────────────────────────────────────────────────┘
```

Interface de monitoramento (RNF-11): plot em tempo real da taxa de erro por época.

7.2 Pipeline de Inferência

```
┌─────────────────────────────────────────────────────────────┐
│ 1. Input do usuário (texto ou voz)                          │
│    → STT se for voz                                         │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Tokenização                                              │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. Injeção de contexto:                                     │
│    - Persona (tom ácido/rebelde)                            │
│    - Memória de longo prazo (JSON)                          │
│    - Estado atual da Natacha                                │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 4. Roteamento (SLM local vs API externa)                    │
│    - baseado em tokens e tempo estimado (RF-12)             │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 5. Forward Pass (inferência)                                │
│    → amostragem (temperature, top-k, top-p)                 │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 6. Detokenização                                            │
│    → texto                                                  │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ 7. Saída:                                                   │
│    - TTS (áudio)                                            │
│    - Avatar + Lip Sync                                      │
│    - Log de interação (JSON)                                │
└─────────────────────────────────────────────────────────────┘
```

Latência alvo: < 2 segundos do fim do input ao início da síntese (RNF-01).

7.3 Persistência de Pesos e Embeddings

· Formato: JSON via nlohmann/json.
· Granularidade: um arquivo por componente.
· Checkpoints: a cada 100 épocas (ou N épocas configuráveis).
· Atomicidade: escrita em arquivo temporário + rename atômico (evita corrupção em falhas — RNF-07).
· Versionamento: campo version no JSON; carregador valida compatibilidade.

---

8. ARQUITETURA DE INTEGRAÇÃO

8.1 Ferramentas Externas

Todas as ferramentas externas são ferramentas operáveis, não dependências rígidas (RN-11):

Ferramenta Linguagem Protocolo Função
ARCA TypeScript (Angular) REST + Event Bus Comparação de preços
Sentinel Rust Event Bus Segurança, logs imutáveis
EditeCC TypeScript (Next.js) Event Bus Formatação ABNT
Analytics (a definir) REST Análise de dados
OpenCode (a definir) REST Desenvolvimento

8.2 Formato de Mensagem do Event Bus

Mensagens cross-language usam JSON como formato neutro:

```json
{
  "header": "ARCA_PRICE_UPDATE",
  "source": "arca",
  "target": "natacha",
  "priority": "P2",
  "timestamp": "2026-09-15T17:36:00Z",
  "payload": {
    "product": "arroz",
    "price": 24.90,
    "location": "São Vicente",
    "variation": -0.10
  }
}
```

8.3 Seleção Dinâmica de Ferramentas

A Natacha não segue roteamento fixo (RF-11.1). Ela avalia caso a caso:

· Tarefa de formatação → EditeCC
· Tarefa de segurança → Sentinel
· Tarefa de preços → ARCA
· Tarefa de análise → Analytics
· Tarefa de código → OpenCode

Se uma ferramenta está indisponível, ela tenta rota alternativa (SLM local) ou informa com a persona ativa.

---

9. SEGURANÇA

9.1 Regra de Ouro

Nunca acesso direto ao banco de dados. Toda operação passa por funções controladas.

9.2 Funções Controladas

Exemplos de assinaturas permitidas (nomes ilustrativos — a implementação real é privada):

```cpp
// PERMITIDO
Produto buscar_produto(const std::string& nome);
std::vector<Comparacao> comparar_precos(ProdutoId id);
Lista criar_lista(UsuarioId uid, const std::vector<Item>& itens);
void registrar_log(const LogEntry& entry);  // append-only, SHA-256

// PROIBIDO
ResultSet executar_sql_qualquer_coisa(const std::string& sql);
```

9.3 Logs Imutáveis (RNF-06)

· Formato: texto estruturado (JSON Lines).
· Imutabilidade: append-only + hash SHA-256 encadeado.
· Local: Porão DISCO.
· Proteção: arquivo com permissão restrita, validação por hash a cada leitura.

9.4 Camadas de Defesa

Camada Mecanismo
1 Mediação via funções controladas (RN-04)
2 Perfis de escopo por projeto (RF-16)
3 Veto autônomo (RN-10)
4 Análise preditiva de impacto (RN-09)
5 Validação humana em ações de alto risco (RF-18)
6 Logs imutáveis com hash (RNF-06)
7 Isolamento de threads (RNF-10)
8 Sentinel (Rust) — detecção de ataques

---

10. ESTRUTURA DE DIRETÓRIOS

```
NATACHA/
├── .git/
├── .vscode/
├── dados/                    ← corpus, datasets, embeddings
│   ├── corpus.txt
│   ├── embeddings/
│   └── weights/
├── docs/                     ← worldbuilding (fonte canônica narrativa)
│   ├── ARQUITETURA.md        ← este documento
│   ├── CASA.md
│   ├── FELIX.md
│   ├── GLOSSARIO.md
│   ├── INTEGRACOES.md
│   ├── PERSONALIDADE.md
│   ├── PLANO_50K_TOKENS.md
│   ├── QUANTUM_THEORY.md
│   ├── REQUISITOS.md
│   └── ROADMAP.md
├── historico/                ← versões antigas, logs de treino
├── include/                  ← headers C++
│   ├── core/
│   ├── neural/
│   ├── embeddings/
│   └── bus/
├── src/
│   ├── core/                 ← Event Bus, Scheduler, threads
│   ├── neural/               ← Perceptron, MLP, RNN, Transformer
│   ├── embeddings/           ← Skip-gram, vocabulário, tokenizer
│   │   ├── analogias.py
│   │   ├── avaliar_completo.py
│   │   ├── avaliar_modelos.py
│   │   └── baixar_fasttext.sh
│   ├── house/                ← HouseStateMap, cômodos
│   ├── felix/                ← Thread Felix (P0)
│   ├── chat/                 ← Pipeline VCS
│   └── agent/                ← Modo Agente
├── tests/                    ← Google Test
├── CMakeLists.txt
└── README.md
```

---

11. ESTRATÉGIA DE TESTES

11.1 Níveis

Nível Escopo Framework
Unitário Funções e classes individuais Google Test
Integração Módulos + Event Bus Google Test + mocks
Sistema Chat + Agente simultâneos Testes end-to-end

11.2 Cobertura Alvo (RNF-08)

· Módulos neurais: ≥ 80%
· Event Bus: ≥ 80%
· Módulos de persistência: ≥ 80%
· Interface: ≥ 60% (mais difícil de testar unitariamente)

11.3 Testes Críticos (obrigatórios)

· Convergência do Perceptron OR em ≤ 1000 épocas (RNF-03)
· MLP resolve XOR com erro < 0.01
· Skip-gram gera embeddings semanticamente coerentes
· Latência do Event Bus < 10 ms (RNF-02)
· Interrupção P0 do Felix em < 100 ms
· Recusa autônoma aciona em 3 cenários-teste (RF-18.2)

---

12. ROADMAP TÉCNICO

Fase Entregável Documentos Status
0 Fundamentos matemáticos — Concluída
1 Perceptron + OR RF-29 Concluída
2 MLP + XOR + Embeddings RF-30, RF-31 Em andamento
3 Skip-gram + JSON RF-31.1 Em andamento
4 RNN + LSTM RF-32 Planejada
5 Self-Attention RF-33 Planejada
6 Transformer RF-34 Planejada
7 SLM (10M–50M params) RF-35, RF-36 Planejada
8 Escala + CUDA — Futura
9 Sótão Quântico (Qiskit) RF-38.x Futura (desejável)
10 LLM Completa — Visão de longo prazo

Meta paralela: consolidar corpus para 50k tokens (PLANO_50K_TOKENS.md).

---

13. 
## N. CONEXÕES

### Documentos internos do projeto
- `REQUISITOS.md` v3.0 — RN-XX, RF-XX
- `ARQUITETURA.md` v2.0 — Seção X.Y
- ...

### Referências externas
> Bibliografia completa em `REFERENCIAS.md`.
> Este documento cita: [1], [2], [3], [8], [9].

---

"De neurônio a Transformer, tijolo por tijolo."

— Documento de Arquitetura, Projeto Natacha — Arquitetura de Alma.

---
