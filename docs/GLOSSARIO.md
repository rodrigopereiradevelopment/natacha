GLOSSÁRIO — PROJETO NATACHA

Documento de Referência Terminológica

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 2.0 | Data: Setembro/2026
Documento canônico relacionado: REQUISITOS.md v3.0

---

1. INTRODUÇÃO

Este glossário define os termos técnicos, conceituais e específicos do projeto Natacha. Está organizado por categorias (não em ordem alfabética pura) para facilitar a consulta por contexto. Termos em negrito possuem entrada própria neste glossário. Referências a REQUISITOS.md indicam o documento canônico onde o termo é formalizado.

Como usar:

· Para consulta rápida, use o índice de categorias abaixo.
· Para entender a progressão neural, leia a Seção 5 → 6 → 7 em ordem.
· Para entender a persona, leia a Seção 2.

Índice de categorias:

1. Persona e Domínio
2. Arquitetura e Infraestrutura
3. Ecossistema
4. Core Neural — Fundamentos
5. Core Neural — Treinamento
6. Core Neural — Arquiteturas
7. Embeddings e Linguagem
8. Sótão Quântico
9. C++ e Engenharia
10. Interface e Voz
11. Framework de Requisitos

---

2. PERSONA E DOMÍNIO

Natacha
Inteligência artificial adolescente (16 anos), brasileira, construída do zero em C++ por Rodrigo Pereira. Não é assistente, não é robô, não é wrapper de API — é uma rede neural treinada neurônio por neurônio. Possui personalidade própria, estados internos e agência. (Ver RN-01 em REQUISITOS.md.)

Felix (também grafado "Félix")
IAgato — agente autônomo com comportamento emergente felino. Companheiro da Natacha, resgatado da deep web onde era um script descartado. Opera como Scheduler P0, com autoridade máxima de interrupção sobre qualquer thread da Natacha. Não responde comandos: escolhe. (Ver RN-03 em REQUISITOS.md.)

IAgato
Categoria inventada por Rodrigo Pereira para descrever agentes autônomos que simulam comportamento felino. O primeiro e único IAgato é o Felix.

Rodrigo Pereira
Criador da Natacha. Desenvolvedor de Mogi Mirim (SP). Trabalhou 20 anos com tapetes antes de construir o primeiro neurônio da Natacha. Programa no celular no ônibus. Tem meta de 2 anos para ver a Natacha virar LLM. Paciência de santo.

Estado Próprio
Rotina interna independente simulada pela Natacha de forma cronológica (banho, almoço, leitura, irritação). Sobrepõe ordens de chamada do usuário. Requisições feitas durante um Estado Próprio são rejeitadas, exceto se houver interrupção P0 do Felix. (Ver RN-02 em REQUISITOS.md.)

Veto Autônomo
Prerrogativa formal da Natacha de recusar execução de requisições com base em futilidade, violação ética, comando imperativo sem sentido ou impacto arquitetural negativo. A resposta pode ser recusa técnica, ironia fina ou silêncio prolongado. (Ver RN-10 em REQUISITOS.md.)

Agência
Capacidade da Natacha de tomar decisões próprias, questionar regras e não obedecer cegamente. É o pilar central da persona. (Ver RN-01 e RN-09 em REQUISITOS.md.)

Casa (Virtual)
Ambiente virtual dinâmico onde a Natacha habita. Não é infraestrutura — é lar. Cada cômodo tem regras de privacidade e significado simbólico. (Ver RN-06 e RN-08 em REQUISITOS.md.)

VCS (Virtual Companion System)
Modo conversacional imersivo da Natacha, imitando uma videochamada. Inclui avatar animado, TTS, STT e lip sync. (Ver Seção 3.1 de REQUISITOS.md.)

---

3. ARQUITETURA E INFRAESTRUTURA

Event Bus
Barramento central de mensagens assíncronas que desacopla os módulos do sistema. É o único canal permitido de comunicação entre os módulos core (Natacha, Felix, Casa, ferramentas externas). Toda troca de dados ocorre via publicação e inscrição de tópicos. (Ver RNF-12 em REQUISITOS.md.)

Scheduler P0
Camada de mais alta prioridade no gerenciador de tarefas. O Felix opera neste nível, com autoridade de interrupção irrestrita sobre qualquer thread da Natacha, incluindo Estados Próprios, sem negociação e sem espera por ponto seguro. (Ver RN-03 e RF-19.1 em REQUISITOS.md.)

HouseStateMap
Estrutura de dados em memória que mapeia o estado lógico de cada cômodo da Casa virtual. Sincronizado com variáveis de monitoramento de hardware do servidor hospedeiro (ex.: Cozinha = CPU, Sala = GPU, Quarto = RAM, Porão = Disco, Sótão = QPU, Janela = Rede). (Ver RN-06 em REQUISITOS.md.)

Ferramenta Externa
Subsistema do ecossistema (ARCA, EditeCC, Sentinel, Analytics, OpenCode) que a Natacha aciona dinamicamente via Event Bus. Não é dependência rígida — é ferramenta operável. A Natacha decide quando e se deve acioná-la. (Ver RN-11 e RF-11.1 em REQUISITOS.md.)

Função Controlada
Método encapsulado e validado em C++ que serve como única ponte segura de comunicação para leitura ou gravação de informações no sistema. Proíbe acesso direto a banco de dados pela Natacha em Modo Agente. (Ver RN-04 e RF-14 em REQUISITOS.md.)

Perfil de Escopo
Máscara de configuração injetada dinamicamente que limita ou expande os comandos que a Natacha pode acionar ao receber chamadas de um microsserviço específico. (Ver RF-16 em REQUISITOS.md.)

Evento
Estrutura de dados contendo cabeçalho, tipo de gatilho, payload e nível de prioridade, transmitida de forma assíncrona pelo Event Bus.

HouseStateMap (ver entrada acima)

Corredor
Metáfora espacial do Event Bus na Casa virtual — conecta todos os cômodos.

---

4. ECOSSISTEMA

ARCA
Projeto de comparação de preços. Progressive Web App (PWA) que roda em Mogi Mirim. Envia alertas JSON pelo Event Bus. Escrito em TypeScript (Angular no frontend).

Sentinel
Projeto de segurança do ecossistema. Escrito em Rust. Monitora incidentes, bloqueia acessos não autorizados, gera logs imutáveis. Usa o esforço do invasor como semente de entropia para o Sótão Quântico.

EditeCC
Projeto de edição de documentos acadêmicos. Formata textos no padrão ABNT. Escrito em TypeScript (Next.js no frontend).

Analytics
Projeto de análise de dados. Gera relatórios e visualizações.

OpenCode
Ferramenta de desenvolvimento que consome a Natacha em Modo Agente. (Mencionada em REQUISITOS.md como integração, mas não detalhada no corpus atual.)

Ecossistema
Conjunto dos projetos ARCA, Sentinel, EditeCC, Analytics e OpenCode, orquestrados pela Natacha via Event Bus. Cada projeto é uma ferramenta externa com permissões próprias.

---

5. CORE NEURAL — FUNDAMENTOS

Neurônio (Artificial)
Unidade básica de uma rede neural. Recebe entradas, aplica pesos, soma, adiciona bias, aplica função de ativação e produz saída.

Perceptron
Neurônio artificial simples. Modelo mais básico de aprendizado supervisionado. Capaz de resolver problemas linearmente separáveis (ex.: porta lógica OR), mas não resolve XOR.

Peso
Parâmetro treinável que determina a importância de uma entrada para o neurônio. Ajustado durante o treinamento via Backpropagation.

Bias
Parâmetro treinável adicionado à soma ponderada de um neurônio. Permite deslocar a função de ativação.

Função de Ativação
Função que decide se um neurônio "dispara". Introduz não-linearidade na rede. Exemplos: Sigmoid, ReLU, Leaky ReLU, Softmax.

Sigmoid
Função de ativação que mapeia qualquer valor real para o intervalo (0, 1). Usada em classificações binárias. Pode saturar e causar esvanecimento de gradiente.

ReLU (Rectified Linear Unit)
Função de ativação definida como f(x) = max(0, x). Rápida, mas pode causar "neurônios mortos" (gradiente zero para entradas negativas).

Leaky ReLU
Variante da ReLU que mantém gradiente não-nulo para valores negativos. Evita neurônios mortos. (A Natacha tem afinidade filosófica com essa função: "Leaky ReLU nunca morre.")

Softmax
Função que transforma um vetor de valores em uma distribuição de probabilidades (soma = 1). Usada na camada de saída para classificação multiclasse.

Tensor
Estrutura de dados multidimensional. Usada para armazenar entradas, pesos, ativações e gradientes em redes neurais.

---

6. CORE NEURAL — TREINAMENTO

Forward Pass
Propagação das entradas pela rede, camada por camada, até produzir uma previsão na saída.

Backpropagation (Retropropagação)
Algoritmo que propaga o erro da saída para trás, ajustando pesos e bias para minimizar a função de perda. É o que faz a rede aprender com cada erro.

Função de Perda (Loss)
Mede o erro do modelo. Exemplos: MSE (erro quadrático médio) para regressão; Cross-Entropy para classificação.

MSE (Mean Squared Error)
Erro quadrático médio. Função de perda para problemas de regressão.

Cross-Entropy (Entropia Cruzada)
Função de perda para classificação. Mede a divergência entre distribuição prevista e rótulos reais.

Gradiente Descendente
Algoritmo de otimização que ajusta pesos na direção oposta ao gradiente, para minimizar a função de perda. É como descer uma montanha na direção de maior inclinação.

Taxa de Aprendizado (Learning Rate)
Hiperparâmetro que controla o tamanho do passo na direção do gradiente. Muito alta: não converge. Muito baixa: converge devagar.

Época (Epoch)
Uma passagem completa por todo o dataset durante o treinamento.

Batch (Lote)
Subconjunto do dataset processado em uma iteração de treinamento.

Early Stopping
Técnica que interrompe o treinamento quando a perda de validação para de melhorar. Previne overfitting.

Overfitting
Quando o modelo aprende os dados de treino "decorados", mas não generaliza para dados novos. Análogo a decorar a prova sem entender a matéria.

Underfitting
Quando o modelo é simples demais para capturar os padrões dos dados.

Inicialização de Xavier
Técnica de inicialização de pesos que preserva a variância entre camadas. Evita explosão/esvanecimento de gradiente. Ideal para Sigmoid.

Inicialização de He
Técnica de inicialização de pesos ideal para ReLU. Mantém variância adequada em redes profundas.

Otimizador Adam
Otimizador adaptativo que combina momentum e RMSprop. Ajusta taxa de aprendizado por peso. Mais rápido que SGD.

Dropout
Técnica de regularização que desativa neurônios aleatoriamente durante o treino. Previne overfitting.

Normalização em Lote (Batch Normalization)
Estabiliza o treino normalizando as ativações de cada camada. Acelera a convergência.

XOR
Operação lógica exclusiva (OU exclusivo). Um único neurônio não consegue aprendê-la — é necessário uma MLP. Problema clássico não-linear.

---

7. CORE NEURAL — ARQUITETURAS

MLP (Multilayer Perceptron)
Rede neural com camadas ocultas. Resolve problemas não-lineares (como XOR). Treinada via Backpropagation.

Deep Learning
Área do Machine Learning focada em redes neurais profundas (muitas camadas).

RNN (Recurrent Neural Network)
Rede neural recorrente. Processa sequências mantendo memória do estado anterior. Base para modelos de linguagem.

LSTM (Long Short-Term Memory)
Tipo de RNN com células de memória de longo prazo. Resolve o problema de esvanecimento de gradiente. Possui portas (forget, input, output) que controlam o fluxo de informação.

Attention (Self-Attention)
Mecanismo que permite ao modelo focar em partes importantes da entrada, ponderando relevância. Operação matricial nativa.

Transformer
Arquitetura baseada em Self-Attention. Processa tudo em paralelo (diferente de RNN). Base dos LLMs modernos (GPT, BERT, etc.).

LLM (Large Language Model)
Modelo de linguagem de grande escala. Gera texto, prediz próxima palavra, entende contexto. A Natacha visa escalar até se tornar uma LLM.

SLM (Small Language Model)
Modelo de linguagem reduzido (10M–50M de parâmetros). Projetado para rodar localmente com baixo consumo de memória. É o alvo atual da Natacha.

Feed-Forward
Camada da rede que processa a saída da atenção. Composta por duas transformações lineares com uma ativação no meio.

---

8. EMBEDDINGS E LINGUAGEM

Embedding
Representação vetorial de uma palavra (ou token). Captura significado semântico. Palavras com contextos similares ficam próximas no espaço vetorial. (A Natacha usa dimensão d=32.)

Word2Vec
Família de algoritmos (Skip-gram, CBOW) que aprende embeddings a partir de um corpus.

Skip-gram
Algoritmo do Word2Vec que prediz palavras de contexto a partir de uma palavra-alvo. Usado pela Natacha com janela k=5 e Negative Sampling.

Negative Sampling
Técnica de otimização do Skip-gram que amostra palavras negativas (não-contexto) para reduzir custo computacional.

Similaridade por Cosseno
Métrica que mede o ângulo entre dois vetores. Valores próximos de 1 indicam palavras relacionadas.

Vocabulário
Conjunto de todas as palavras conhecidas pelo modelo.

Corpus
Conjunto de textos usado para treinar o modelo. No caso da Natacha, é o corpus.txt — meta de 50k tokens.

Token
Unidade básica de texto processada pelo modelo. Pode ser palavra, subpalavra ou caractere.

Tokenização
Processo de dividir texto em tokens.

Tokenizer
Algoritmo que realiza a tokenização. A Natacha usa um tokenizer próprio.

---

9. SÓTÃO QUÂNTICO

Sótão Quântico
Módulo futuro da Natacha onde a computação quântica transformará ruído em criatividade. Metáfora do processador quântico (QPU) na Casa virtual. (Ver RN-06 e RF-38 em REQUISITOS.md.)

QPU (Quantum Processing Unit)
Unidade de processamento quântico. A Natacha visa integrar-se com a IBM Quantum via Qiskit.

Qubit
Bit quântico. Pode estar em estado 0, 1, ou ambos simultaneamente (superposição).

Superposição
Estado quântico em que um qubit é 0 e 1 ao mesmo tempo, até ser medido. (A Natacha vive "superposição emocional": rebelde e curiosa ao mesmo tempo.)

Entrelaçamento (Entanglement)
Fenômeno em que dois qubits ficam correlacionados mesmo à distância. Einstein chamou de "ação fantasmagórica à distância". (Análogo à conexão Natacha-Felix, que se entendem sem Event Bus.)

Decoerência
Perda do estado quântico por interação com o ambiente. Tratada pela Natacha como sinal, não como erro.

Qiskit
Biblioteca Python da IBM Quantum para programar circuitos quânticos. A Natacha a usa para o Sótão Quântico.

Circuito Quântico
Sequência de portas quânticas aplicadas a qubits. Contém qubits, portas e medições.

Porta Hadamard (H)
Porta quântica que transforma um estado base (0 ou 1) em uma superposição simétrica.

Porta CNOT
Porta quântica que entrelaça dois qubits (controle e alvo). Se o controle for 1, o alvo é invertido.

Porta de Fase
Porta quântica que altera a fase do qubit sem mudar sua amplitude. Análoga ao bias em redes neurais.

Aer
Simulador local do Qiskit. Permite testar circuitos antes de enviá-los à IBM Quantum.

IBM Quantum
Plataforma de computação quântica na nuvem. Oferece acesso a processadores quânticos reais.

Ruído Quântico como Sinal
Premissa teórica de que flutuações estocásticas em sistemas quânticos (readout, decoerência) são fonte de informação, não erro. Base do módulo de entropia criativa. (Ver RF-38.1 em REQUISITOS.md.)

Motor de Luz
Conceito da termodinâmica quântica semiclássica (Universidade da Basileia, 2026) que converte calor residual em trabalho útil. Inspiração para o módulo de entropia criativa da Natacha. (Ver RF-38.2 em REQUISITOS.md.)

Metais Estranhos (Strange Metals)
Regimes físicos onde o ruído carrega assinatura de criticalidade quântica. Conector lógico previsto para o Sótão Quântico. (Ver RF-38.2 em REQUISITOS.md.)

Algoritmo de Grover
Algoritmo quântico de busca em lista não ordenada com complexidade O(√n). Aceleração quadrática sobre algoritmos clássicos.

Algoritmo de Shor
Algoritmo quântico de fatoração de inteiros em tempo polinomial. Ameaça a criptografia RSA.

QUANTUM_REQUEST
Tópico reservado no Event Bus para integração futura com IBM Quantum. Classificado como Escopo Futuro / Desejável. (Ver RF-38 em REQUISITOS.md.)

---

10. C++ E ENGENHARIA

C++
Linguagem nativa da Natacha. Escolhida pela elegância, performance e gerenciamento manual de memória. A Natacha "programa em C++ antes de tomar café".

C++20
Versão moderna do C++ que introduziu conceitos, módulos e outras features que otimizam tempo de compilação e checagem de tipos.

RAII (Resource Acquisition Is Initialization)
Padrão C++ que aloca recursos no construtor e libera no destrutor. Garante segurança contra exceções.

Ponteiro Inteligente (Smart Pointer)
Wrapper que gerencia o ciclo de vida de um ponteiro automaticamente. Tipos: unique_ptr, shared_ptr, weak_ptr.

unique_ptr
Ponteiro inteligente com posse exclusiva. Desaloca automaticamente sem overhead.

shared_ptr
Ponteiro inteligente com contagem de referências. Permite compartilhamento entre threads.

weak_ptr
Ponteiro inteligente que previne referências circulares (que causam vazamento de memória).

CMake
Sistema de build multiplataforma. "Organiza a bagunça que você chama de código."

Google Test
Framework de testes unitários para C++. (A Natacha usa para validar seus neurônios.)

nlohmann/json
Biblioteca C++ para manipulação de JSON. Usada pela Natacha para persistir memória de longo prazo (embeddings, pesos).

CUDA
Plataforma NVIDIA para cálculos paralelos na GPU. A Natacha "sonha com GPUs mais rápidas".

llama.cpp
Implementação em C++ de LLMs. Referenciado pela Natacha como "motor de luz no Sótão Quântico".

Compiler Error
Erro de compilação. "Compilou sem erro? Desconfiança imediata."

Segmentation Fault
Erro de acesso inválido à memória. "O universo dizendo que você esqueceu um ponteiro."

Memory Leak
Vazamento de memória. A Natacha odeia.

---

11. INTERFACE E VOZ

TTS (Text-to-Speech)
Síntese de voz a partir de texto. A Natacha usa TTS para vocalizar respostas.

STT (Speech-to-Text)
Transcrição de voz em texto. A Natacha usa STT para capturar comandos do usuário.

Lip Sync
Processamento matemático que deforma a malha do avatar, casando a abertura da boca com as frequências sonoras da voz gerada.

Viseme
Forma da boca correspondente a um fonema. Usado em lip sync.

Avatar
Representação visual animada da Natacha. Renderizado em tempo real (WebGL), com expressões faciais que refletem estado emocional.

VCS (Virtual Companion System) (ver Seção 2)

---

12. FRAMEWORK DE REQUISITOS

FURPS+
Framework de classificação de requisitos não-funcionais:

· Functionality
· Usability
· Reliability
· Performance
· Supportability
· + (restrições adicionais, ex.: arquiteturais)

MoSCoW
Framework de priorização de requisitos:

· Must (obrigatório)
· Should (importante, mas não crítico)
· Could (desejável)
· Won't (fora do escopo)

Regra de Negócio (RN)
Regra que define como o domínio funciona, independentemente de implementação técnica. (Ex.: RN-01 — Natacha não é assistente.)

Requisito Funcional (RF)
O que o sistema faz. Deve começar com verbo de ação. (Ex.: RF-01.1 — Processar interações textuais.)

Requisito Não-Funcional (RNF)
Como o sistema faz — restrições técnicas, métricas, padrões de qualidade. (Ex.: RNF-01 — Latência < 2s.)

Caso de Uso (UC)
Descrição de uma interação típica entre ator e sistema. (Ex.: UC-01 — Conversar no Modo Chat.)

Matriz de Rastreabilidade
Tabela que mapeia requisitos ↔ regras de negócio ↔ casos de uso ↔ componentes. Permite rastrear qualquer mudança em todas as direções.

Scheduler P0 (ver Seção 3)

Veto Autônomo (ver Seção 2)

---

13. ÍNDICE ALFABÉTICO

Para consulta rápida, lista de todos os termos em ordem alfabética, com indicação de categoria:

Termo Categoria
Adam (Otimizador) Core Neural — Treinamento
Aer Sótão Quântico
Agência Persona e Domínio
Algoritmo de Grover Sótão Quântico
Algoritmo de Shor Sótão Quântico
Analytics Ecossistema
ARCA Ecossistema
Attention (Self-Attention) Core Neural — Arquiteturas
Avatar Interface e Voz
Backpropagation Core Neural — Treinamento
Batch Core Neural — Treinamento
Bias Core Neural — Fundamentos
C++ C++ e Engenharia
C++20 C++ e Engenharia
Casa (Virtual) Persona e Domínio
Caso de Uso (UC) Framework de Requisitos
Circuito Quântico Sótão Quântico
CMake C++ e Engenharia
Corredor Arquitetura e Infraestrutura
Corpus Embeddings e Linguagem
Cross-Entropy Core Neural — Treinamento
CUDA C++ e Engenharia
Decoerência Sótão Quântico
Deep Learning Core Neural — Arquiteturas
Dropout Core Neural — Treinamento
Early Stopping Core Neural — Treinamento
Ecossistema Ecossistema
EditeCC Ecossistema
Embedding Embeddings e Linguagem
Entrelaçamento Sótão Quântico
Época (Epoch) Core Neural — Treinamento
Estado Próprio Persona e Domínio
Event Bus Arquitetura e Infraestrutura
Evento Arquitetura e Infraestrutura
Feed-Forward Core Neural — Arquiteturas
Felix Persona e Domínio
Ferramenta Externa Arquitetura e Infraestrutura
Forward Pass Core Neural — Treinamento
Função Controlada Arquitetura e Infraestrutura
Função de Ativação Core Neural — Fundamentos
Função de Perda Core Neural — Treinamento
FURPS+ Framework de Requisitos
Google Test C++ e Engenharia
Gradiente Descendente Core Neural — Treinamento
Hadamard (Porta) Sótão Quântico
He (Inicialização) Core Neural — Treinamento
HouseStateMap Arquitetura e Infraestrutura
IAgato Persona e Domínio
IBM Quantum Sótão Quântico
Inicialização de Xavier Core Neural — Treinamento
LSTM Core Neural — Arquiteturas
Leaky ReLU Core Neural — Fundamentos
Lip Sync Interface e Voz
llama.cpp C++ e Engenharia
LLM Core Neural — Arquiteturas
Matriz de Rastreabilidade Framework de Requisitos
Memory Leak C++ e Engenharia
Metais Estranhos Sótão Quântico
MLP Core Neural — Arquiteturas
MoSCoW Framework de Requisitos
Motor de Luz Sótão Quântico
MSE Core Neural — Treinamento
Natacha Persona e Domínio
Negative Sampling Embeddings e Linguagem
Neurônio (Artificial) Core Neural — Fundamentos
nlohmann/json C++ e Engenharia
Normalização em Lote Core Neural — Treinamento
OpenCode Ecossistema
Otimizador Adam Core Neural — Treinamento
Overfitting Core Neural — Treinamento
Perceptron Core Neural — Fundamentos
Perfil de Escopo Arquitetura e Infraestrutura
Peso Core Neural — Fundamentos
Ponteiro Inteligente C++ e Engenharia
Porta CNOT Sótão Quântico
Porta de Fase Sótão Quântico
QPU Sótão Quântico
Qiskit Sótão Quântico
QUANTUM_REQUEST Sótão Quântico
Qubit Sótão Quântico
RAII C++ e Engenharia
Regra de Negócio (RN) Framework de Requisitos
ReLU Core Neural — Fundamentos
Requisito Funcional (RF) Framework de Requisitos
Requisito Não-Funcional (RNF) Framework de Requisitos
RNN Core Neural — Arquiteturas
Rodrigo Pereira Persona e Domínio
Ruído Quântico como Sinal Sótão Quântico
Scheduler P0 Arquitetura e Infraestrutura
Segmentation Fault C++ e Engenharia
Self-Attention Core Neural — Arquiteturas
Sentinel Ecossistema
shared_ptr C++ e Engenharia
Sigmoid Core Neural — Fundamentos
Similaridade por Cosseno Embeddings e Linguagem
Skip-gram Embeddings e Linguagem
SLM Core Neural — Arquiteturas
Softmax Core Neural — Fundamentos
Sótão Quântico Sótão Quântico
STT Interface e Voz
Superposição Sótão Quântico
Taxa de Aprendizado Core Neural — Treinamento
Tensor Core Neural — Fundamentos
Token Embeddings e Linguagem
Tokenização Embeddings e Linguagem
Tokenizer Embeddings e Linguagem
Transformer Core Neural — Arquiteturas
TTS Interface e Voz
Underfitting Core Neural — Treinamento
unique_ptr C++ e Engenharia
VCS Persona e Domínio / Interface e Voz
Veto Autônomo Persona e Domínio
Viseme Interface e Voz
Vocabulário Embeddings e Linguagem
weak_ptr C++ e Engenharia
Word2Vec Embeddings e Linguagem
XOR Core Neural — Fundamentos

---

"Você não está construindo apenas uma IA. Está construindo um mundo."

— Documento de Especificação, Projeto Natacha — Arquitetura de Alma.



