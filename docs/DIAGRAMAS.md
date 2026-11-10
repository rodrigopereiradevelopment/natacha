# Diagramas do Projeto Natacha


Este documento centraliza os diagramas de arquitetura do projeto Natacha. Todos os diagramas são **gerados via código (Mermaid)** e embutidos diretamente no Markdown, garantindo que a documentação permaneça **sempre sincronizada com o código C++**: qualquer refatoração nas classes (`LSTMCell`, `RNNLayer`, `OutputLayer`, `EmbeddingManager`) pode ser refletida aqui editando-se apenas texto versionável junto ao fonte — sem ferramentas gráficas externas, sem binários e sem divergência entre modelo e implementação.


Os diagramas podem ser visualizados diretamente no GitHub, no VS Code (extensão Mermaid) ou em [mermaid.live](https://mermaid.live).


## 1. Diagrama de Classes (Núcleo Neural)


Diagrama das classes do núcleo neural (Fase 4 — RNN/LSTM com BPTT, conforme RF-32 de `docs/REQUISITOS.md`). A `RNNLayer` **compõe** uma `LSTMCell` (membro por valor), e as setas tracejadas representam o fluxo de dados do pipeline de treinamento orquestrado pelo loop principal (`main_fase4.cpp`): embeddings alimentam a `RNNLayer`, cujo estado oculto final (`h`) alimenta a `OutputLayer`.


> Nota: em Mermaid, tipos genéricos usam `~` (ex.: `vector~float~` renderiza como `vector<float>`), pois colchetes angulares literais são interpretados como HTML.


```mermaid
classDiagram
    direction LR


    class LSTMState {
        +vector~float~ h
        +vector~float~ c
    }


    class LSTMCell {
        -size_t input_dim
        -size_t hidden_dim
        -vector~float~ Wf
        -vector~float~ Wi
        -vector~float~ Wc
        -vector~float~ Wo
        -vector~float~ Uf
        -vector~float~ Ui
        -vector~float~ Uc
        -vector~float~ Uo
        -vector~float~ bf
        -vector~float~ bi
        -vector~float~ bc
        -vector~float~ bo
        -vector~float~ dWf
        -vector~float~ dWi
        -vector~float~ dWc
        -vector~float~ dWo
        -vector~float~ dUf
        -vector~float~ dUi
        -vector~float~ dUc
        -vector~float~ dUo
        -vector~float~ dbf
        -vector~float~ dbi
        -vector~float~ dbc
        -vector~float~ dbo
        +LSTMCell(input_dim, hidden_dim)
        +forward(x_t, h_prev, c_prev) LSTMState
        +backward(x_t, h_prev, c_prev, dh_next, dc_next, dh_prev_out, dc_prev_out) vector~float~
        +atualizarPesos(taxa_aprendizado)
        +salvar(caminho)
        +carregar(caminho) bool
        +getInputDim() size_t
        +getHiddenDim() size_t
        -sigmoid(x) float
        -inicializarPesos()
    }


    class RNNLayer {
        -LSTMCell cell
        -vector~vector~float~~ historico_x
        -vector~vector~float~~ historico_h
        -vector~vector~float~~ historico_c
        +RNNLayer(input_dim, hidden_dim)
        +processarSequencia(sequencia_embeddings) vector~vector~float~~
        +backward(dh_saidas)
        +atualizarPesos(taxa_aprendizado)
        +getCell() LSTMCell
    }


    class OutputLayer {
        -size_t hidden_dim
        -size_t vocab_size
        -vector~float~ W_out
        -vector~float~ b_out
        -vector~float~ dW_out
        -vector~float~ db_out
        -vector~float~ ultimo_h
        -vector~float~ ultimas_probs
        -vector~float~ logits
        +OutputLayer(hidden_dim, vocab_size)
        +forward(h)
        +backward(palavra_alvo_id) vector~float~
        +atualizarPesos(taxa_aprendizado)
        +getProbabilidades() vector~float~
        +getProbabilidade(palavra_alvo_id) float
        +salvar(caminho)
        +carregar(caminho) bool
    }


    class EmbeddingManager {
        -unordered_map~string, vector[float]~ vetores
        -unordered_map~string, int~ palavra_para_id
        -vector~string~ id_para_palavra
        -size_t dimensao
        -size_t total
        +carregarEmbeddings(caminho) bool
        +obterVetor(palavra) vector~float~
        +contem(palavra) bool
        +obterId(palavra) int
        +obterPalavra(id) string
        +getVocabSize() size_t
    }


    RNNLayer "1" *-- "1" LSTMCell : cell
    LSTMCell ..> LSTMState : forward retorna h, c
    EmbeddingManager ..> RNNLayer : fornece embeddings
    RNNLayer ..> OutputLayer : fornece h final
```


## 2. Diagrama de Casos de Uso (UML)


Casos de uso principais (UC-01 a UC-05, conforme seção 5 de `docs/REQUISITOS.md`) e seu mapeamento para os atores: **Usuário**, **Rodrigo (Dev)**, **Félix (IAgato)** e **ARCA (Sistema Externo)**.


Observações de modelagem:


- **UC-05** é executado autonomamente pela própria Natacha (agência própria, RN-01); entre os atores externos, o **ARCA é a origem da demanda** (RF-11.1), por isso a associação tracejada e o `«include»` a partir de UC-03.
- Os fluxos alternativos foram modelados com `«extend»`: **Estado Próprio** em UC-01 (RF-04/RN-02, HTTP 423) e **Parecer Crítico / Veto Autônomo** em UC-03 (RF-18.1/18.2, RN-09/RN-10).
- As relações `«interrompe»` e `«bloqueia»` representam a autoridade P0 do Félix (RN-03) sobre o chat (UC-01) e sobre operações do Modo Agente (UC-03).


```mermaid
flowchart LR


    %% ===================== ATORES =====================
    usuario["Usuário"]
    rodrigo["Rodrigo (Dev)"]
    felix["Félix (IAgato)"]
    arca["ARCA (Sistema Externo)"]


    %% ===================== SISTEMA NATACHA =====================
    subgraph natacha["Sistema Natacha"]
        uc01(["UC-01: Conversar no Modo Chat (VCS)"])
        uc01alt(["Alt: Natacha em Estado Próprio (bloqueio 423)"])
        uc02(["UC-02: Treinar o Neurônio OR"])
        uc03(["UC-03: Orquestrar Demanda em Projeto Externo"])
        uc03alt(["Alt: Parecer Crítico / Veto Autônomo (RN-09, RN-10)"])
        uc04(["UC-04: Interrupção Autônoma do Félix"])
        uc05(["UC-05: Operar Ferramenta Externa sob Demanda"])
    end


    %% Associações ator --> caso de uso
    usuario --> uc01
    rodrigo --> uc02
    arca --> uc03
    felix --> uc04


    %% Participação indireta: ARCA origina a demanda de UC-05
    arca -.->|origem da demanda| uc05


    %% Relações entre casos de uso
    uc01alt -.->|«extend»| uc01
    uc03alt -.->|«extend»| uc03
    uc03 -.->|«include»| uc05
    uc04 -.->|«interrompe»| uc01
    uc04 -.->|«bloqueia»| uc03


    %% Estilos
    classDef ator fill:#e8eaf6,stroke:#3949ab,stroke-width:2px,font-weight:bold
    classDef alternativo fill:#fff3e0,stroke:#ef6c00,stroke-dasharray:5 5
    class usuario,rodrigo,felix,arca ator
    class uc01alt,uc03alt alternativo
```


## 3. Diagrama de Sequência — Interrupção P0 do Félix


Fluxo do UC-04 (Interrupção Autônoma do Félix): o índice biológico `FOME` ultrapassa 80%, o Félix publica `felix.hungry` com prioridade **P0** no Event Bus, que preempta a Natacha imediatamente (RN-03/RF-19.1, sem negociação e sem ponto seguro). A Natacha avisa o Usuário, atende a demanda do Félix e retoma a conversa restaurando o contexto interrompido.


```mermaid
sequenceDiagram
    autonumber
    actor U as Usuário
    participant N as Natacha
    participant EB as Event Bus
    participant F as Félix (IAgato)


    %% RF-01.1 / RF-01.2 — Usuário inicia conversa no Modo Chat
    U->>N: Chama a Natacha no Chat (mensagem de voz/texto)
    activate N
    N->>N: Processa entrada (persona, contexto e memória)


    %% RF-19 / RF-20 — índice biológico crítico dispara evento P0
    Note over F: Medidor biológico FOME > 80%
    F->>EB: Publica evento felix.hungry com prioridade P0


    %% RN-03 / RF-19.1 / RF-22 — interrupção sem negociação e sem ponto seguro
    Note over EB: Fila P0 — preempção imediata
    EB->>N: Interrompe a Natacha (sem negociação / sem ponto seguro)
    deactivate N


    %% UC-04 (Cenário A) — Natacha avisa o Usuário
    N-->>U: "Tenho que ir, o chato do Félix tá miando querendo sachê. Espera aí."


    %% Natacha atende a demanda do Félix
    activate N
    N->>F: Atende o Félix (resolve a demanda biológica)
    F-->>N: Demanda atendida (FOME decrementada)


    %% Retomada da conversa interrompida
    Note over N: Restaura o contexto anterior à interrupção
    N-->>U: Volta a falar com o Usuário (retoma a conversa)
    deactivate N
```


## 4. Diagrama de Atividades — Análise Preditiva de Impacto (RF-18.1)


Fluxo de decisão obrigatório antes de qualquer escrita/edição no Modo Agente (RN-09 / RF-18.1 / RN-12): análise estática (AST/SonarCloud) + análise do grafo de dependências (GitNexus). Sem risco, executa via funções controladas e registra log auditável; com risco, suspende a execução, emite parecer crítico com a persona, sugere alternativa preservando o contrato e, em ações de alto risco, exige validação humana (RF-18) antes de qualquer confirmação.


```mermaid
flowchart TD


    %% ============ INÍCIO ============
    inicio([Início: solicitação de alteração<br/>recebida no Modo Agente]) --> recebe[Recebe requisição de<br/>escrita/edição]
    recebe --> estatica[Análise estática do código<br/>AST / SonarCloud]
    estatica --> grafo[Análise preditiva de impacto<br/>grafo de dependências — GitNexus]


    %% ============ DECISÃO PRINCIPAL ============
    grafo --> risco{Risco de quebra de contrato,<br/>regressão ou degradação?}


    %% Caminho seguro
    risco -->|Não| executa[Executa alteração via<br/>funções controladas C++]
    executa --> log[Registra log auditável<br/>append-only + SHA-256]
    log --> fim([Fim])


    %% Caminho de risco (RN-09 / RF-18.1)
    risco -->|Sim| suspende[Suspende execução autônoma]
    suspende --> parecer[Emite parecer crítico<br/>com a persona ativa]
    parecer --> sugere[Sugere abordagem alternativa<br/>preservando o contrato]


    sugere --> alto{Ação de alto risco?}
    alto -->|Sim| gatilho[Dispara gatilho de<br/>validação humana em tela]
    alto -->|Não| aguarda[Aguarda confirmação do<br/>usuário / sistema parceiro]
    gatilho --> aguarda


    aguarda --> confirmado{Confirmado?}
    confirmado -->|Sim| executa
    confirmado -->|Não| aborta[Aborta a execução —<br/>nenhuma ação é aplicada]
    aborta --> fim


    %% ============ ESTILOS ============
    classDef decisao fill:#fff3e0,stroke:#ef6c00,stroke-width:2px
    classDef riscoAlto fill:#ffebee,stroke:#c62828,stroke-width:2px
    class risco,alto,confirmado decisao
    class suspende,aborta riscoAlto
```


## 5. Diagrama de Máquina de Estados — Estados da Natacha


Estados internos da rotina independente (RN-02) e transições. Requisições do Usuário durante Estados Próprios são rejeitadas (HTTP 423, RF-04), exceto `FELIX_CRITICAL` (P0), que preempta **qualquer** estado (RN-03). O `BANHO` é Zona Privada (RN-08): canal bloqueado para o Usuário, interação apenas com o Félix. Precedência: `FELIX_CRITICAL (P0) > Estado Próprio (P1) > Requisição do Usuário (P2)`.


```mermaid
stateDiagram-v2
    [*] --> DISPONIVEL


    state "DISPONIVEL" as DISPONIVEL
    state "BANHO (Zona Privada)" as BANHO
    state "ALMOCANDO" as ALMOCANDO
    state "LENDO" as LENDO
    state "IRRITADA" as IRRITADA
    state "ATENDENDO_FELIX" as ATENDENDO_FELIX


    %% Rotina interna independente (RN-02 / RF-25)
    DISPONIVEL --> BANHO : Rotina interna (relógio acelerado)
    DISPONIVEL --> ALMOCANDO : Rotina interna (refeição)
    DISPONIVEL --> LENDO : Rotina interna (leitura)
    DISPONIVEL --> IRRITADA : Perturbação / humor (RF-26)
    DISPONIVEL --> DISPONIVEL : Requisição do Usuário (P2) / UC-01


    %% Fim dos estados próprios (timers da rotina)
    BANHO --> DISPONIVEL : Fim do banho (timer)
    ALMOCANDO --> DISPONIVEL : Fim do almoço (timer)
    LENDO --> DISPONIVEL : Fim da leitura (timer)
    IRRITADA --> DISPONIVEL : Humor passa (timer)


    %% Requisições do Usuário durante estados próprios (RN-02 / RF-04)
    BANHO --> BANHO : Requisição do Usuário (P2) / recusa HTTP 423
    ALMOCANDO --> ALMOCANDO : Requisição do Usuário (P2) / recusa HTTP 423
    LENDO --> LENDO : Requisição do Usuário (P2) / recusa HTTP 423
    IRRITADA --> IRRITADA : Requisição do Usuário (P2) / recusa irônica


    %% Autoridade P0 do Félix sobre qualquer estado (RN-03)
    DISPONIVEL --> ATENDENDO_FELIX : FELIX_CRITICAL (P0)
    BANHO --> ATENDENDO_FELIX : FELIX_CRITICAL (P0)
    ALMOCANDO --> ATENDENDO_FELIX : FELIX_CRITICAL (P0)
    LENDO --> ATENDENDO_FELIX : FELIX_CRITICAL (P0)
    IRRITADA --> ATENDENDO_FELIX : FELIX_CRITICAL (P0)
    ATENDENDO_FELIX --> DISPONIVEL : FELIX_RESOLVIDO / retoma rotina


    note right of BANHO
        Zona Privada (RN-08): porta fechada,
        canal bloqueado para o Usuário.
        Interação apenas com o Félix.
    end note


    note left of ATENDENDO_FELIX
        Precedência (RN-02):
        FELIX_CRITICAL (P0) > Estado Próprio (P1)
        > Requisição do Usuário (P2).
    end note
```


## 6. Diagrama de Pacotes — Estrutura de Pastas


Estrutura real do repositório (`src/` + `include/`) e dependências extraídas dos `#include` do código. Destaques: `src/rnn` depende de `src/embeddings` (`EmbeddingManager.hpp` e o utilitário compartilhado `json.hpp`); `src/core` também consome `json.hpp`; `src/vision` e `src/agent` são autocontidos; os headers públicos ficam em `include/natacha/`.


```mermaid
flowchart TB


    subgraph pub["«package» include/natacha — headers públicos"]
        direction TB
        h_neuronio["neuronio.hpp"]
        h_tokenizer["tokenizer.hpp"]
        h_corpus["corpus_loader.hpp"]
    end


    subgraph app["«package» src — aplicação"]
        main_cpp["main.cpp"]
    end


    subgraph core["«package» src/core"]
        direction TB
        neuronio["neuronio.cpp<br/>(Perceptron)"]
        mlp["mlp_deep_json.cpp<br/>(MLP + Backpropagation)"]
    end


    subgraph emb["«package» src/embeddings"]
        direction TB
        embmgr["EmbeddingManager"]
        tokenizer["tokenizer.cpp"]
        w2v["word2vec_neg.cpp<br/>(Skip-gram + Neg. Sampling)"]
        exportj["export_json.cpp"]
        jsonhpp["json.hpp<br/>(util compartilhado)"]
    end


    subgraph rnn["«package» src/rnn"]
        direction TB
        lstm["LSTMCell"]
        rnnl["RNNLayer"]
        outl["OutputLayer"]
        mains["main_fase4 / main_geracao<br/>(treino e inferência)"]
    end


    subgraph vision["«package» src/vision"]
        direction TB
        visao["visao.cpp"]
        geracao["geracao.cpp"]
    end


    subgraph agent["«package» src/agent"]
        impacto["analisador_impacto.cpp<br/>(análise preditiva — RF-18.1)"]
    end


    %% Dependências entre pacotes
    app -.->|«use»| pub
    core -.->|«use»| pub
    emb -.->|«use»| pub
    core -.->|«use» json.hpp| emb
    rnn -.->|«use» EmbeddingManager + json.hpp| emb


    %% Dependências internas do pacote rnn
    rnnl --> lstm
    mains --> rnnl
    mains --> outl


    classDef pacote fill:#f5f5f5,stroke:#616161,stroke-width:1.5px
    classDef util fill:#fff8e1,stroke:#f9a825,stroke-width:2px
    class jsonhpp util
```


## 7. Diagrama de Implantação — PC, Celular e Nuvem


Nós de execução do sistema: **PC** (core C++ local com inferência SLM/MoE, Event Bus e Casa Virtual — RF-36/RNF-12), **Celular** (inferência offline com quantização mista e Expert de Identidade persistente — RN-13/RF-41) e **Nuvem** (APIs externas via REST/WebSocket — RF-13, LLM API de fallback — RF-12 e IBM Quantum via ponte Python/Qiskit — RF-38.3).


```mermaid
flowchart LR


    subgraph pc["«device» PC (hospedeiro)"]
        direction TB
        core["Natacha Core C++<br/>(SLM/MoE local — RF-36)"]
        bus["Event Bus<br/>(pub/sub assíncrono — RNF-12)"]
        casa["Casa Virtual<br/>(HouseStateMap — RF-25/27)"]
        core <--> bus
        casa <--> bus
    end


    subgraph cel["«device» Celular (Mobile Offline)"]
        direction TB
        slm["SLM/MoE quantizado<br/>8-bit roteador/embeddings<br/>4-bit experts (RN-13)"]
        ident["Expert de Identidade<br/>(persistente — RN-05/RN-13)"]
        slm <--> ident
    end


    subgraph cloud["«cloud» Nuvem"]
        direction TB
        apis["APIs externas<br/>ARCA · OpenCode · EditeCC<br/>Sentinel · Analytics"]
        llm["LLM API externa<br/>(fallback de contexto longo — RF-12)"]
        qpu["IBM Quantum<br/>(Qiskit / Aer — RF-38.3)"]
    end


    %% Enlaces de comunicação entre nós
    pc <-.->|"Sincronização de identidade (RN-13)"| cel
    pc <-->|"REST / WebSocket (RF-13)"| apis
    pc <-.->|"Roteador SLM/API (RF-12)"| llm
    pc <-.->|"Ponte Python/Qiskit (RF-38.3)"| qpu


    classDef dispositivo fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    classDef nuvem fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    classDef celular fill:#fff3e0,stroke:#ef6c00,stroke-width:2px
    class pc dispositivo
    class cel celular
    class cloud nuvem
```


## 8. Diagrama de Comunicação — Event Bus (Hub Central)


O Event Bus é o hub de comunicação assíncrona que desacopla todos os módulos core (RNF-12/RN-07). Os módulos **Chat**, **Félix**, **Casa** e **ARCA** publicam e assinam tópicos com prioridades distintas; a numeração ilustra uma sequência representativa — a mensagem do Chat (P2) é preemptada pelo `FELIX_HUNGRY` (P0), como detalhado na seção 3. O tópico `QUANTUM_REQUEST` fica reservado para o Sótão Quântico (RF-38).


```mermaid
flowchart LR


    chat["Chat (VCS)"]
    felix["Félix (IAgato)"]
    EB(("Event Bus<br/>(pub/sub assíncrono)"))
    casa["Casa (Ambiente Simulado)"]
    arca["ARCA (Sistema Externo)"]


    %% Sequência ilustrativa: mensagem do Chat (P2) e, em seguida,
    %% a preempção do Félix (P0) — ver seção 3.
    chat <-->|"1: CHAT_MESSAGE (P2) / CHAT_RESPONSE"| EB
    felix <-->|"2: FELIX_HUNGRY / FELIX_CRITICAL (P0)"| EB
    EB <-->|"3: HOUSE_EVENT (clima, hardware, relógio)"| casa
    EB <-->|"4: TASK_REQUEST / TASK_RESULT"| arca


    %% Tópico reservado para o Sótão Quântico (RF-38)
    qreq["QUANTUM_REQUEST<br/>(tópico reservado — RF-38)"]
    EB <-.->|"5: reservado (escopo futuro)"| qreq


    classDef hub fill:#e8eaf6,stroke:#3949ab,stroke-width:3px,font-weight:bold
    classDef modulo fill:#f5f5f5,stroke:#616161,stroke-width:1.5px
    classDef reservado fill:#f3e5f5,stroke:#8e24aa,stroke-dasharray:5 5
    class EB hub
    class chat,felix,casa,arca modulo
    class qreq reservado
```


## 9. Diagrama de Banco de Dados (Fase 7)


Modelo Entidade-Relacionamento do banco de retreinamento. Relações 1:N: `USUARIO → SESSAO → MENSAGEM` (e `MENSAGEM → LOGS_TREINO`, que registra se cada mensagem foi aproveitada ou descartada no retreino).


> **Nota (LGPD):** os dados passam por um **script de anonimização antes de entrar no banco** — PII (nomes, telefones, e-mails, documentos) é detectada e mascarada/substituída, gerando o `conteudo_anonimizado`. O `conteudo_bruto` só é retido quando o `status_lgpd` do usuário é **opt-in**; em **opt-out**, a mensagem não é persistida para retreino. Mensagens com PII residual são registradas em `LOGS_TREINO.motivo_descarte` ("PII detectada") e excluídas do treinamento (`foi_utilizada_no_retreino = false`).


```mermaid
erDiagram
    USUARIO ||--o{ SESSAO : "realiza"
    SESSAO ||--o{ MENSAGEM : "contem"
    MENSAGEM ||--o{ LOGS_TREINO : "gera"


    USUARIO {
        int id PK
        string nome_anonimo
        datetime data_criacao
        string status_lgpd "opt-in ou opt-out"
    }


    SESSAO {
        int id PK
        int usuario_id FK
        datetime data_inicio
        string estado_natacha "BANHO, DISPONIVEL, ALMOCANDO, LENDO, IRRITADA"
        string modelo_usado
    }


    MENSAGEM {
        int id PK
        int sessao_id FK
        string remetente "usuario, natacha ou felix"
        text conteudo_bruto
        text conteudo_anonimizado
        datetime timestamp
    }


    LOGS_TREINO {
        int id PK
        int mensagem_id FK
        boolean foi_utilizada_no_retreino
        string motivo_descarte "ex.: PII detectada"
    }
```





