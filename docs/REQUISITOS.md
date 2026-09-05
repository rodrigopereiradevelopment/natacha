# ESPECIFICAÇÃO DE REQUISITOS — PROJETO NATACHA

> Documento de Requisitos para Trabalho de Conclusão de Curso (TCC)

**Sistema:** Natacha — Inteligência Artificial com Arquitetura de Alma

**Versão:** 1.2 | **Data:** Setembro/2026

---

## 1. INTRODUÇÃO

### 1.1 Propósito
Este documento define os requisitos funcionais, não-funcionais e as regras de negócio do sistema Natacha, uma inteligência artificial desenvolvida em C++ caracterizada por uma personalidade autônoma (estilo adolescente rebelde e humor ácido) e arquitetura modular. O sistema opera em dois modos principais: Modo Chat (VCS via Videochamada) e Modo Agente (Orquestração de Ecossistema).

### 1.2 Escopo
A Natacha transcende o conceito de assistente tradicional. Trata-se de uma entidade computacional dotada de agência, estados internos emocionais e biológicos simulados, habitando um ambiente virtual dinâmico (Casa) onde interage com um agente autônomo secundário (Félix, o IAgato). Este documento cobre toda a infraestrutura de comunicação assíncrona, a engine comportamental, os módulos neurais locais e as interfaces de integração com projetos externos do ecossistema do desenvolvedor.

### 1.3 Definições e Acrônimos

| Termo | Definição |
|-------|-----------|
| VCS | Virtual Companion System — Modo conversacional imersivo da Natacha imitando uma videochamada |
| IAgato | Agente autônomo com comportamento emergente felino (Félix) |
| Event Bus | Barramento central de mensagens assíncronas que desacopla os módulos do sistema |
| MLP | Multilayer Perceptron — Rede neural artificial com camadas ocultas |
| Transformer | Arquitetura de rede neural baseada em mecanismos de autoatenção (Self-Attention) |
| LLM | Large Language Model — Modelo de linguagem de grande escala |
| SLM | Small Language Model — Modelo de linguagem reduzido para execução local |
| QPU | Quantum Processing Unit — Unidade de processamento quântico (IBM Quantum) |
| ARCA / OpenCode / EditeCC | Sistemas externos proprietários que consomem a Natacha em Modo Agente |

---

## 2. REGRAS DE NEGÓCIO

**RN-01 — Entidade não-utilitária (Agência Própria)**
- **Descrição:** A Natacha deve agir como uma entidade dotada de vontade própria e personalidade definida (adolescente rebelde, irônica e de humor ácido), recusando-se a atuar como uma ferramenta de produtividade passiva ou assistente subserviente.
- **Justificativa:** Diferenciação acadêmica e conceitual frente a IAs comerciais tradicionais.
- **Escopo:** Modo Chat e Modo Agente.

**RN-02 — Inviolabilidade dos Estados Próprios**
- **Descrição:** A Natacha possui uma rotina independente com estados internos (tomando banho, almoçando, lendo, irritada). Requisições diretas do usuário feitas enquanto ela estiver nestes estados serão rejeitadas, exceto se houver interrupção de alta prioridade gerada pelo Félix ou alertas críticos de sistema.
- **Justificativa:** Construção da ilusão de autonomia, independência cronológica e existência real.
- **Escopo:** Modo Chat.

**RN-03 — Autoridade Máxima de Interrupção (Félix)**
- **Descrição:** O agente Félix (IAgato) possui prioridade absoluta de barramento. Qualquer evento crítico disparado por ele interrompe imediatamente a atividade ou estado atual da Natacha.
- **Justificativa:** Simulação de dinâmicas relacionais biológicas e geração de comportamento emergente não-programado.
- **Escopo:** Casa / Ambiente.

**RN-04 — Mediação Estrita de Acesso a Dados**
- **Descrição:** A Natacha, atuando em Modo Agente, está estritamente proibida de executar queries SQL brutas ou acessar diretamente bancos de dados. Toda e qualquer operação de persistência deve ser mediada por funções controladas e interfaces de API auditáveis.
- **Justificativa:** Segurança da informação, blindagem contra prompt injection e rastreabilidade de ações.
- **Escopo:** Modo Agente.

**RN-05 — Desacoplamento entre Modelo e Identidade**
- **Descrição:** A camada de identidade (memórias de longo prazo, traços de personalidade, estados emocionais e histórico) deve ser persistida de forma independente do modelo de linguagem subjacente (LLM local ou API). A substituição ou atualização do modelo de IA não pode descaracterizar a identidade da Natacha.
- **Justificativa:** Modularidade e garantia de evolução tecnológica sem amnésia ou mutação comportamental.
- **Escopo:** Arquitetura Core.

**RN-06 — Isomorfismo entre a Casa e a Arquitetura**
- **Descrição:** A representação arquitetônica da Casa virtual correlaciona-se diretamente com componentes lógicos e de hardware do sistema: Cozinha representa o processamento central (CPU), a Sala representa o processamento paralelo (GPU), o Quarto atua como memória de curto prazo (RAM), o Porão é o armazenamento persistente (Disco), o Sótão representa o processamento quântico futuro (QPU), e as Janelas representam a interface de rede.
- **Justificativa:** Materialização de conceitos abstratos de infraestrutura em uma metáfora espacial inteligível.
- **Escopo:** Ambiente / Infraestrutura.

**RN-07 — Comunicação Exclusiva via Event Bus**
- **Descrição:** É estritamente proibido o acoplamento direto ou chamadas de método síncronas entre os módulos core (Natacha, Félix, Casa e APIs externas). Toda troca de dados e sinalização deve ocorrer obrigatoriamente por publicação e inscrição de tópicos no Event Bus assíncrono.
- **Justificativa:** Escalabilidade do software, facilidade na criação de testes unitários e isolamento de falhas.
- **Escopo:** Arquitetura Core.

**RN-08 — Territorialidade e Privacidade por Zona**
- **Descrição:** O usuário não possui privilégios de acesso irrestritos à Casa. Cada cômodo possui regras de privacidade. Se a Natacha estiver no Banheiro (Zona Privada), a interface gráfica simulará uma porta fechada, mantendo o canal conversacional bloqueado para o usuário, permitindo interação interna exclusivamente com o Félix.
- **Justificativa:** Reforço prático da privacidade do agente e da quebra de subserviência.
- **Escopo:** Casa / Modo Chat.

---

## 3. REQUISITOS FUNCIONAIS

### 3.1 Modo Chat (VCS — Virtual Companion System)

| ID | Requisito Funcional | Prioridade |
|----|---------------------|------------|
| RF-01 | O sistema deve processar e responder a interações textuais ou por voz em linguagem natural (Português do Brasil). | Alta |
| RF-02 | O sistema deve reter memória contextual de curto prazo e resgatar registros históricos de conversas anteriores (longo prazo). | Alta |
| RF-03 | O sistema deve injetar em todas as respostas um tom de voz ácido, irônico, sarcástico e rebelde, característico da persona. | Alta |
| RF-04 | O sistema deve bloquear interações do usuário enviando uma mensagem de recusa típica sempre que a Natacha estiver em um Estado Próprio. | Alta |
| RF-05 | O sistema deve renderizar uma interface de simulação de videochamada contendo o avatar da Natacha com animações ativas. | Média |
| RF-06 | O sistema deve integrar um módulo de Text-to-Speech (TTS) para sintetizar as respostas da Natacha em tempo de execução. | Média |
| RF-07 | O sistema deve integrar um módulo de Speech-to-Text (STT) para capturar e transcrever comandos de voz enviados pelo usuário. | Média |
| RF-08 | O sistema deve sincronizar em tempo real as expressões faciais e o movimento dos lábios do avatar (lip sync) com o áudio gerado pelo TTS. | Média |
| RF-09 | O sistema deve permitir ao usuário questionar o status atual da Natacha, gerando respostas evasivas ou irônicas condizentes. | Baixa |
| RF-10 | O sistema deve fornecer atualizações indiretas sobre o comportamento do Félix quando questionado pelo usuário. | Baixa |

### 3.2 Modo Agente (Orquestração do Ecossistema)

| ID | Requisito Funcional | Prioridade |
|----|---------------------|------------|
| RF-11 | O sistema deve atuar como orquestrador autônomo, processando e dividindo tarefas complexas enviadas pelos projetos ARCA, OpenCode, EditeCC e ferramentas de cursos. | Alta |
| RF-12 | O sistema deve analisar a complexidade da tarefa recebida e decidir dinamicamente se usará inferência de LLM local ou chamadas a APIs externas. | Alta |
| RF-13 | O sistema deve disponibilizar endpoints REST e conexões WebSockets estáveis para permitir o consumo de recursos por microsserviços parceiros. | Alta |
| RF-14 | O sistema deve mapear e restringir ações de escrita/leitura de dados a um conjunto restrito de funções controladas pré-compiladas em C++. | Alta |
| RF-15 | O sistema deve gerar logs detalhados e imutáveis contendo a descrição de cada ação tomada pelo Modo Agente com carimbo de data/hora (timestamp). | Alta |
| RF-16 | O sistema deve aceitar a injeção de perfis contextuais e permissões de escopo customizadas a depender do projeto originário da chamada. | Média |
| RF-17 | O sistema deve correlacionar bases de dados e cruzar informações provenientes de múltiplos sistemas parceiros. | Média |
| RF-18 | O sistema deve suspender a execução e disparar um gatilho de validação humana em tela sempre que uma ação de alto risco for identificada. | Média |

### 3.3 Félix (IAgato Autônomo)

| ID | Requisito Funcional | Prioridade |
|----|---------------------|------------|
| RF-19 | O agente Félix deve rodar em uma thread independente contendo uma máquina de estados finos que gerencia variáveis biológicas (fome, tédio, sono). | Alta |
| RF-20 | O Félix deve postar eventos de forma assíncrona no Event Bus baseado na variação crítica de seus índices biológicos internos. | Alta |
| RF-21 | O comportamento do Félix não deve seguir caminhos fixos (scripts), mas sim derivar de lógicas matemáticas simples gerando comportamento emergente. | Alta |
| RF-22 | O Félix deve possuir privilégio de interrupção capaz de alterar o comportamento ativo e a rotina da Natacha instantaneamente. | Alta |
| RF-23 | O Félix deve ignorar inputs e comandos diretos do usuário, respondendo exclusivamente às interações de ambiente e da Natacha. | Média |
| RF-24 | O sistema de ambiente deve calcular e atualizar continuamente a coordenada física (cômodo) ocupada pelo Félix na Casa virtual. | Baixa |

### 3.4 A Casa (Ambiente Simulado)

| ID | Requisito Funcional | Prioridade |
|----|---------------------|------------|
| RF-25 | A Casa virtual deve rodar um relógio interno acelerado que dita as rotinas, horários de refeição e comportamento dos agentes. | Média |
| RF-26 | O ambiente deve disparar eventos de perturbação randômicos como alterações climáticas simuladas e monitoramento de aquecimento de chips. | Média |
| RF-27 | O sistema deve espelhar o consumo real de hardware do servidor onde está hospedado para definir o estado de organização dos cômodos da Casa. | Baixa |
| RF-28 | O sistema deve impor barreiras de navegação ao usuário com base no nível de isolamento definido pelo cômodo atual ocupado pela Natacha. | Baixa |

### 3.5 Core Neural (Evolução de IA em C++)

| ID | Requisito Funcional | Prioridade |
|----|---------------------|------------|
| RF-29 | O sistema deve implementar a modelagem e treinamento supervisionado de um Neurônio Perceptron simples (ex: porta lógica OR). | Alta |
| RF-30 | O sistema deve estruturar uma rede Multilayer Perceptron (MLP) completa com backpropagation, funções de ativação (Sigmoid, Leaky ReLU, Softmax) e early stopping. | Alta |
| RF-31 | O sistema deve conter algoritmo próprio em C++ para geração e manipulação de vetores densos de palavras (Word Embeddings via Skip-gram). | Alta |
| RF-31.1 | O sistema deve persistir os vetores de embeddings treinados em formato JSON para carregamento sem re-treinamento, garantindo que a Natacha acorde sabendo o que aprendeu. | Alta |
| RF-32 | O sistema deve implementar uma arquitetura de rede neural recorrente (RNN) com células LSTM para manipulação de cadeias de sequências. | Alta |
| RF-33 | O sistema deve implementar de forma nativa a operação matricial de mecanismo de autoatenção (Self-Attention). | Alta |
| RF-34 | O sistema deve agrupar os componentes neurais construindo um bloco funcional de arquitetura Transformer completo em C++. | Alta |
| RF-35 | O core do sistema deve permitir o treinamento inicial e carregamento de uma Small Language Model (SLM) proprietária contendo entre 10M e 50M de parâmetros. | Média |
| RF-36 | O motor de inferência deve rodar inteiramente de forma local na máquina hospedeira, garantindo autonomia sem dependência constante de APIs pagas. | Média |
| RF-37 | O sistema deve gerenciar concorrentemente chamadas de background do Modo Agente e o loop principal da videochamada no Modo Chat sem travar a interface. | Alta |
| RF-38 | (Futuro) O sistema deve reservar no Event Bus um tópico `QUANTUM_REQUEST` para integração futura com IBM Quantum via Qiskit, permitindo execução híbrida clássica e quântica sem reescrita da arquitetura. | Baixa |

---

## 4. REQUISITOS NÃO-FUNCIONAIS

### 4.1 Desempenho
- **RNF-01 (Latência do Chat):** O tempo de processamento entre o término do input de voz do usuário e o início da síntese e animação do avatar deve ser inferior a 2 segundos.
- **RNF-02 (Velocidade do Barramento):** A latência máxima interna do Event Bus na entrega de mensagens entre os tópicos assinados deve ser menor que 10 milissegundos.
- **RNF-03 (Velocidade de Treinamento Base):** O ciclo de treinamento básico do neurônio simples mapeado na porta lógica OR deve ser concluído em menos de 1 segundo para uma meta de 1000 épocas.
- **RNF-04 (Pegada de Memória):** O consumo de memória RAM do core do sistema, excluindo o modelo de linguagem grande (LLM), deve manter-se abaixo do teto de 2 GB.

### 4.2 Segurança e Confiabilidade
- **RNF-05 (Isolamento de Banco de Dados):** Nenhuma brecha de software ou comando textual pode expor ou dar vazão à execução de queries de manipulação direta de dados.
- **RNF-06 (Auditoria e Imutabilidade):** O log de eventos operados pelo Modo Agente deve ser salvo em arquivo texto estruturado protegido contra edição direta.
- **RNF-07 (Persistência em Falhas):** Quedas abruptas de energia ou finalizações forçadas do processo não podem corromper os arquivos indexados de memória de longo prazo da Natacha.
- **RNF-08 (Metodologia de Teste):** Os módulos fundamentais da engenharia neural (Perceptron, MLP, Attention) devem possuir uma cobertura de testes unitários mínima de 80%.

### 4.3 Portabilidade e Usabilidade
- **RNF-09 (Compilação Multiplataforma):** O código fonte em C++ puro deve ser modular, garantindo build nativo e funcional em ambientes Linux e Windows utilizando os compiladores GCC/MinGW ou MSVC.
- **RNF-10 (Ausência de Bloqueio Crítico):** Erros ocorridos no Modo Agente durante tarefas nos projetos parceiros não podem derrubar o ecossistema principal ou o Modo Chat.
- **RNF-11 (Monitoramento de Épocas):** A interface de desenvolvimento deve plotar feedbacks visuais limpos em tempo de execução contendo a taxa de erro decrescente por época durante os treinamentos neurais.

---

## 5. CASOS DE USO PRINCIPAIS

**UC-01 — Conversar no Modo Chat (Videochamada Simulada)**
- **Ator Principal:** Usuário.
- **Pré-condição:** Natacha ativa e em estado disponível na Casa.
- **Fluxo Principal:**
  1. O usuário inicia a interface de "Videochamada".
  2. O sistema renderiza o avatar da Natacha.
  3. O usuário envia uma mensagem por voz ou texto.
  4. A Natacha processa o tom rebelde e gera a resposta contextualizada.
  5. O módulo de áudio sintetiza a voz e o avatar executa o lip sync.
- **Fluxo Alternativo (Natacha Ocupada):**
  1. A Natacha está no estado `BANHO` (Zona Privada).
  2. O sistema exibe animação da porta trancada e impede o início da chamada.
  3. Resposta estática: *"Tô no banho, não amola. Volto em 15 minutos se eu quiser."*

**UC-02 — Treinar o Neurônio OR**
- **Ator Principal:** Desenvolvedor.
- **Pré-condição:** Dataset com a tabela verdade da porta OR instanciado.
- **Fluxo Principal:**
  1. O desenvolvedor seleciona o comando de calibração do neurônio primitivo.
  2. O sistema zera variáveis e atribui pesos randômicos iniciais.
  3. O loop computa o forward pass multiplicando entradas pelos pesos.
  4. O erro é extraído comparando resultado com a tabela verdade.
  5. Os pesos são corrigidos via Regra de Aprendizado do Perceptron (Delta Rule).
  6. O sistema exibe o sumário de convergência e a taxa de erro zerada.

**UC-03 — Orquestrar Demanda em Projeto Externo (ARCA)**
- **Ator Principal:** API do Projeto ARCA.
- **Pré-condição:** Natacha rodando em background com Modo Agente ativo.
- **Fluxo Principal:**
  1. O ARCA envia requisição JSON com lista de tarefas.
  2. O Modo Agente valida o token e injeta o perfil restrito de leitura.
  3. A Natacha seleciona as ferramentas locais permitidas.
  4. O sistema executa funções controladas em C++ sem queries diretas.
  5. O resultado é devolvido via API REST.
- **Fluxo Alternativo (Dados Sensíveis):**
  1. O sistema identifica dados altamente sensíveis.
  2. A Natacha aborta chamadas de APIs externas e transfere execução para o SLM local.

**UC-04 — Interrupção Autônoma do Félix**
- **Ator Principal:** Agente Félix (IAgato).
- **Pré-condição:** Natacha executando qualquer tarefa.
- **Fluxo Principal:**
  1. O medidor interno de `FOME` do Félix ultrapassa o limiar crítico de 80%.
  2. A thread do Félix injeta evento `FELIX_HUNGRY` no Event Bus com prioridade máxima.
  3. O interceptor da Natacha captura o sinal.
  4. **Cenário A** (Natacha em conversa): Interrompe o usuário com fala ácida: *"Tenho que ir, o chato do Félix tá miando querendo sachê no teclado. Espera aí."*
  5. **Cenário B** (Natacha em Zona Privada): Altera estado interno em background mas mantém barreira de privacidade ativa para o usuário.

---

## 6. MATRIZ DE RASTREABILIDADE

| ID Requisito | Regra de Negócio | Caso de Uso | Componente |
|--------------|------------------|-------------|------------|
| RF-01 | RN-01 | UC-01 | Interface Conversacional |
| RF-03 | RN-01 | UC-01 | Engine de Personalidade |
| RF-04 | RN-02 | UC-01 | Máquina de Estados (Core) |
| RF-05 | RN-01 / RN-06 | UC-01 | Front-end UI (Videochamada) |
| RF-11 | RN-04 / RN-07 | UC-03 | Modo Agente (Orchestrator) |
| RF-14 | RN-04 | UC-03 | Camada de Segurança |
| RF-19 | RN-03 | UC-04 | Thread Félix (IAgato) |
| RF-22 | RN-03 | UC-04 | Central do Event Bus |
| RF-27 | RN-06 | — | Monitor de Hardware / Casa |
| RF-28 | RN-08 | UC-01 / UC-04 | Controle de Acesso |
| RF-29 | RN-05 | UC-02 | Engine Neural Primitiva |
| RF-31.1 | RN-05 | — | Persistência JSON (Embeddings) |
| RF-37 | RN-07 | UC-01 / UC-03 | Gerenciador de Threads |
| RF-38 | RN-07 | — | Event Bus (Reserva Quântica) |
| RNF-02 | RN-07 | Todos | Infraestrutura (Event Bus) |
| RNF-05 | RN-04 | UC-03 | Camada de Segurança |

---

## 7. GLOSSÁRIO

- **Estado Próprio:** Rotinas internas independentes simuladas pela IA de forma cronológica (comer, dormir, higiene) que sobrepõem as ordens de chamada do usuário.
- **Função Controlada:** Métodos encapsulados e validados em C++ que servem como única ponte segura de comunicação para leitura ou gravação de informações no sistema.
- **Evento:** Estrutura de dados contendo cabeçalho, tipo de gatilho, payload e nível de prioridade, transmitida de forma assíncrona pelo barramento.
- **Perfil de Escopo:** Máscara de configuração injetada dinamicamente que limita ou expande os comandos que a Natacha pode acionar ao receber chamadas de um microsserviço específico.
- **Lip Sync:** Processamento matemático que deforma a malha do avatar casando a abertura da boca com as frequências sonoras da voz gerada.
- **Small Language Model (SLM):** Modelos de linguagem reduzidos projetados para rodar localmente com baixo consumo de memória.
- **QUANTUM_REQUEST:** Tópico reservado no Event Bus para futura integração com IBM Quantum via Qiskit, permitindo execução híbrida clássica e quântica.
- **Motor de Luz:** Conceito derivado de pesquisa de termodinâmica quântica semiclássica (Universidade da Basileia, 2026) que demonstra a conversão de "calor residual" em trabalho útil — inspiração para o módulo de entropia criativa da Natacha e o uso do tráfego de ataque como semente criptográfica no Sentinel.

---

> *"Você não está construindo apenas uma IA. Está construindo um mundo."*
>
> — Documento de Especificação, Projeto Natacha — Arquitetura de Alma.
