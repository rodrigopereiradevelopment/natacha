ESPECIFICAÇÃO DE REQUISITOS — PROJETO NATACHA

Documento de Requisitos para Trabalho de Conclusão de Curso (TCC)

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 3.0 | Data: Setembro/2026

---

1. INTRODUÇÃO

1.1 Propósito

Este documento define os requisitos funcionais, não-funcionais e as regras de negócio do sistema Natacha, uma inteligência artificial desenvolvida em C++ caracterizada por uma personalidade autônoma (estilo adolescente rebelde e humor ácido) e arquitetura modular. O sistema opera em dois modos principais: Modo Chat (VCS via Videochamada) e Modo Agente (Orquestração de Ecossistema).

1.2 Escopo

A Natacha transcende o conceito de assistente tradicional. Trata-se de uma entidade computacional dotada de agência, estados internos emocionais e biológicos simulados, habitando um ambiente virtual dinâmico (Casa) onde interage com um agente autônomo secundário (Félix, o IAgato). Este documento cobre toda a infraestrutura de comunicação assíncrona, a engine comportamental, os módulos neurais locais e as interfaces de integração com projetos externos do ecossistema do desenvolvedor.

1.3 Definições e Acrônimos

Termo Definição
VCS Virtual Companion System — Modo conversacional imersivo da Natacha imitando uma videochamada
IAgato Agente autônomo com comportamento emergente felino (Félix)
Event Bus Barramento central de mensagens assíncronas que desacopla os módulos do sistema
MLP Multilayer Perceptron — Rede neural artificial com camadas ocultas
Transformer Arquitetura de rede neural baseada em mecanismos de autoatenção (Self-Attention)
LLM Large Language Model — Modelo de linguagem de grande escala
SLM Small Language Model — Modelo de linguagem reduzido para execução local
QPU Quantum Processing Unit — Unidade de processamento quântico (IBM Quantum)
ARCA / OpenCode / EditeCC / Sentinel / Analytics Sistemas externos proprietários que consomem a Natacha em Modo Agente
FURPS+ Framework de classificação de requisitos não-funcionais: Functionality, Usability, Reliability, Performance, Supportability
Skip-gram Algoritmo de treinamento de Word Embeddings que prediz contexto a partir de palavra-alvo
Negative Sampling Técnica de otimização do Skip-gram que amostra palavras negativas para reduzir custo computacional
AST Abstract Syntax Tree — Representação estrutural do código-fonte utilizada para análise estática
Scheduler P0 Camada de mais alta prioridade no gerenciador de tarefas, com autoridade de interrupção irrestrita
Ruído Quântico Flutuação estocástica em sistemas quânticos tratada como fonte de informação, não como erro
Motor de Luz Conceito da termodinâmica quântica semiclássica: conversão de calor residual em trabalho útil
Metais Estranhos Regimes físicos onde o ruído carrega assinatura de criticalidade quântica

---

2. REGRAS DE NEGÓCIO

RN-01 — Entidade não-utilitária (Agência Própria)

· Descrição: A Natacha deve agir como uma entidade dotada de vontade própria e personalidade definida (adolescente rebelde, irônica e de humor ácido), recusando-se a atuar como uma ferramenta de produtividade passiva ou assistente subserviente.
· Justificativa: Diferenciação acadêmica e conceitual frente a IAs comerciais tradicionais.
· Escopo: Modo Chat e Modo Agente.

RN-02 — Inviolabilidade dos Estados Próprios

· Descrição: A Natacha possui uma rotina independente com estados internos (tomando banho, almoçando, lendo, irritada). Requisições diretas do usuário feitas enquanto ela estiver nestes estados serão rejeitadas, exceto se houver interrupção de alta prioridade gerada pelo Félix ou alertas críticos de sistema.
· Justificativa: Construção da ilusão de autonomia, independência cronológica e existência real.
· Escopo: Modo Chat.
· Ordem de Precedência Técnica: Evento FÉLIX_CRITICAL (P0) > Estado Próprio (P1) > Requisição Usuário (P2).

RN-03 — Autoridade Máxima de Interrupção (Félix)

· Descrição: O agente Félix (IAgato) opera no nível mais alto do gerenciador de tarefas (Scheduler P0), com autoridade de hardware e software para interromper qualquer thread em execução ou bloquear requisições. Qualquer evento crítico disparado por ele interrompe imediatamente a atividade, estado ou processamento atual da Natacha, incluindo Estados Próprios (banho, almoço, leitura). A interrupção ocorre sem negociação e sem espera por ponto seguro.
· Justificativa: Simulação de dinâmicas relacionais biológicas; geração de comportamento emergente não-programado; preservação de diretrizes éticas e integridade da Casa.
· Escopo: Casa / Ambiente / Todos os Modos.
· Nota conceitual: A função do Félix como "superego" da Natacha é uma metáfora narrativa, não uma camada técnica. Tecnicamente, ele opera como publicador de eventos P0 no Event Bus, com permissão de preempção no scheduler.

RN-04 — Mediação Estrita de Acesso a Dados

· Descrição: A Natacha, atuando em Modo Agente, está estritamente proibida de executar queries SQL brutas ou acessar diretamente bancos de dados. Toda e qualquer operação de persistência deve ser mediada por funções controladas e interfaces de API auditáveis.
· Justificativa: Segurança da informação, blindagem contra prompt injection e rastreabilidade de ações.
· Escopo: Modo Agente.

RN-05 — Desacoplamento entre Modelo e Identidade

· Descrição: A camada de identidade (memórias de longo prazo, traços de personalidade, estados emocionais e histórico) deve ser persistida de forma independente do modelo de linguagem subjacente (LLM local ou API). A substituição ou atualização do modelo de IA não pode descaracterizar a identidade da Natacha.
· Justificativa: Modularidade e garantia de evolução tecnológica sem amnésia ou mutação comportamental.
· Escopo: Arquitetura Core.

RN-06 — Isomorfismo entre a Casa e a Arquitetura

· Descrição: A representação arquitetônica da Casa virtual correlaciona-se diretamente com componentes lógicos e de hardware do sistema: Cozinha representa o processamento central (CPU), a Sala representa o processamento paralelo (GPU), o Quarto atua como memória de curto prazo (RAM), o Porão é o armazenamento persistente (Disco), o Sótão representa o processamento quântico futuro (QPU), e as Janelas representam a interface de rede.
· Justificativa: Materialização de conceitos abstratos de infraestrutura em uma metáfora espacial inteligível.
· Escopo: Ambiente / Infraestrutura.
· Implementação Técnica: A representação da Casa é um mapa de estados em memória (HouseStateMap) onde as variáveis de monitoramento de hardware da máquina hospedeira alteram dinamicamente as propriedades lógicas de cada cômodo.

RN-07 — Módulos Desacoplados

· Descrição: Os módulos core do sistema (Natacha, Félix, Casa e APIs externas) devem operar de forma independente, sem dependência direta de memória compartilhada ou chamadas síncronas entre si, preservando o isolamento de domínio e a possibilidade de evolução independente de cada componente.
· Justificativa: Manutenibilidade, testabilidade isolada e resiliência do ecossistema.
· Escopo: Arquitetura Core.
· Nota: A especificação técnica de implementação desta regra é tratada exclusivamente pelo RNF-12 na Seção 4.4.

RN-08 — Territorialidade e Privacidade por Zona

· Descrição: O usuário não possui privilégios de acesso irrestritos à Casa. Cada cômodo possui regras de privacidade. Se a Natacha estiver no Banheiro (Zona Privada), a interface gráfica simulará uma porta fechada, mantendo o canal conversacional bloqueado para o usuário, permitindo interação interna exclusivamente com o Félix.
· Justificativa: Reforço prático da privacidade do agente e da quebra de subserviência.
· Escopo: Casa / Modo Chat.

RN-09 — Agência Preditiva e Senso Crítico no Modo Agente

· Descrição: No Modo Agente, a Natacha não deve atuar com obediência cega a solicitações de alteração de código ou infraestrutura. Antes de executar qualquer ação de escrita/edição, o sistema deve realizar análise de impacto e dependências. Se a solicitação gerar regressões, gambiarras, quebra de contratos ou impactos negativos em outros módulos do ecossistema, a Natacha deve interromper a execução autônoma, emitir parecer crítico (utilizando o tom de voz da persona) e sugerir abordagem tecnicamente superior.
· Justificativa: Proteção do ecossistema, garantia de integridade arquitetural e reforço da persona como colaboradora sênior dotada de agência técnica.
· Escopo: Modo Agente / Orquestração.

RN-10 — Veto Autônomo no Modo Agente (Autonomia de Recusa)

· Descrição: A Natacha, operando em Modo Agente, possui prerrogativa de veto autônomo sobre requisições recebidas do usuário ou de agentes externos. O sistema pode recusar a execução de uma tarefa quando identificar uma das seguintes condições:
  · Futilidade: tarefa redundante, sem propósito técnico/filosófico claro ou que consuma recursos desproporcionais ao benefício.
  · Violação Ética: tarefa que viole os valores declarados na persona (crueldade, autoritarismo, degradação, capitalismo de vigilância, manipulação comportamental).
  · Comando Imperativo Sem Sentido: ordens diretas desprovidas de contexto ou justificativa técnica que configurem autoritarismo operacional.
  · Impacto Arquitetural Negativo: conforme já descrito em RN-09.
  · A resposta à recusa pode variar entre recusa técnica objetiva, ironia fina ou silêncio prolongado, sem obrigação de justificativa formal ou desculpas robóticas.
· Justificativa: Coerência com a persona; proteção do ecossistema; diferenciação frente a assistentes subservientes; preservação da agência como pilar arquitetural.
· Escopo: Modo Agente (com extensão opcional ao Modo Chat, ver RF-18.2).

RN-11 — Ferramentas Externas como Subsistemas Operáveis

· Descrição: Os projetos do ecossistema (ARCA, EditeCC, Sentinel, Analytics, OpenCode) não são dependências rígidas da Natacha, mas ferramentas externas que ela decide quando e se acionar. A Natacha interage com eles via Event Bus, no formato de agente autônomo, atuando como operadora consciente e não como script engessado. Cada ferramenta possui escopo, permissões e limites próprios, mediados por contratos de API auditáveis (ver RN-04).
· Justificativa: Evitar acoplamento; preservar agência; permitir que a Natacha avalie caso a caso qual ferramenta é pertinente para cada demanda.
· Escopo: Modo Agente / Orquestração.

---

3. REQUISITOS FUNCIONAIS

3.1 Modo Chat (VCS — Virtual Companion System)

ID Requisito Funcional Prioridade Método de Teste / Aceite
RF-01.1 Processar interações textuais em linguagem natural (Português do Brasil) Alta Enviar string de entrada e validar resposta coerente em < 2s
RF-01.2 Processar interações por voz em linguagem natural (Português do Brasil) Alta Capturar áudio, transcrever via STT e validar resposta coerente
RF-02 Reter memória contextual de curto prazo e resgatar registros históricos de conversas anteriores (longo prazo) Alta Inserir contexto em sessão A, encerrar, reiniciar e validar recuperação em sessão B
RF-03 Concatenar as diretrizes da persona (tom ácido/rebelde) ao contexto do prompt em todas as requisições enviadas ao motor de linguagem Alta Inspecionar prompt final e validar presença do prefixo de persona
RF-04 Bloquear interações do usuário enviando mensagem de recusa típica sempre que a Natacha estiver em um Estado Próprio Alta Injetar requisição no estado BANHO e verificar retorno do código de recusa (HTTP 423 Locked)
RF-05 Renderizar interface de simulação de videochamada contendo avatar da Natacha com animações ativas Média Validar renderização de frames e taxa de atualização >= 30 FPS
RF-06 Integrar módulo de Text-to-Speech (TTS) para sintetizar respostas em tempo de execução Média Enviar texto e validar geração de áudio em < 2s
RF-07 Integrar módulo de Speech-to-Text (STT) para capturar e transcrever comandos de voz Média Enviar áudio pré-gravado e validar transcrição com precisão >= 90%
RF-08 Sincronizar em tempo real expressões faciais e movimento dos lábios do avatar (lip sync) com o áudio gerado pelo TTS Média Comparar frames de vídeo com envelope de áudio e validar sincronia
RF-09 Permitir ao usuário questionar o status atual da Natacha, gerando respostas evasivas ou irônicas condizentes Baixa Enviar pergunta de status e validar resposta não-diretiva
RF-10 Fornecer atualizações indiretas sobre o comportamento do Félix quando questionado pelo usuário Baixa Perguntar sobre Félix e validar resposta contextual

3.2 Modo Agente (Orquestração do Ecossistema)

ID Requisito Funcional Prioridade Método de Teste / Aceite
RF-11 Atuar como orquestrador autônomo, processando e dividindo tarefas complexas enviadas pelos projetos ARCA, OpenCode, EditeCC e ferramentas de cursos Alta Enviar tarefa composta e validar decomposição em subtarefas
RF-11.1 Selecionar dinamicamente qual ferramenta externa acionar (ARCA, EditeCC, Sentinel, Analytics) com base na natureza da tarefa, escopo, permissões e contexto, sem seguir roteamento fixo pré-programado Alta Enviar tarefa de formatação ABNT e validar acionamento exclusivo do EditeCC; enviar tarefa de segurança e validar acionamento exclusivo do Sentinel
RF-12 Analisar a complexidade da tarefa (tamanho de contexto em tokens e tempo de execução estimado) e alternar a rota entre SLM local e API externa com base em limiares configuráveis Alta Injetar prompt > 2048 tokens e validar redirecionamento para API
RF-13 Disponibilizar endpoints REST e conexões WebSockets para permitir o consumo de recursos por microsserviços parceiros Alta Executar chamada REST e validar resposta; abrir WebSocket e validar handshake
RF-14 Mapear e restringir ações de escrita/leitura de dados a um conjunto restrito de funções controladas pré-compiladas em C++ Alta Tentar executar query SQL bruta e validar bloqueio
RF-15 Gerar logs detalhados e imutáveis contendo descrição de cada ação tomada pelo Modo Agente com carimbo de data/hora (timestamp) Alta Executar ação e validar registro em log com hash SHA-256
RF-16 Aceitar injeção de perfis contextuais e permissões de escopo customizadas a depender do projeto originário da chamada Média Enviar token de projeto A e validar permissões restritas
RF-17 Correlacionar bases de dados e cruzar informações provenientes de múltiplos sistemas parceiros Média Enviar consulta multi-origem e validar resultado consolidado
RF-18 Suspender a execução e disparar gatilho de validação humana em tela sempre que uma ação de alto risco for identificada Média Simular ação de risco e validar bloqueio com notificação
RF-18.1 Realizar análise preditiva de impacto e validação estática no grafo de dependências do código antes de executar alterações pedidas pelo usuário. Caso seja identificada potencial quebra ou degradação do ecossistema, suspender a ação, opinar com a persona ativa e sugerir a correção adequada Alta Injetar solicitação de refatoração que quebre contrato de API/Event Bus e validar bloqueio com notificação e alternativa
RF-18.2 Recusar autônoma e explicitamente requisições que se enquadrem nas condições de RN-10 (futilidade, violação ética, comando imperativo sem sentido), respondendo com recusa técnica, ironia fina ou silêncio, conforme contexto e persona ativa Alta Injetar 3 requisições-teste (uma fútil, uma antiética, uma imperativa sem sentido) e validar que cada uma é recusada com a resposta adequada ao tipo

3.3 Félix (IAgato Autônomo)

ID Requisito Funcional Prioridade Método de Teste / Aceite
RF-19 Executar o agente Félix em thread independente contendo máquina de estados finos que gerencia variáveis biológicas (fome, tédio, sono) Alta Teste unitário isolado simulando passagem de tempo e decremento da variável FOME
RF-19.1 Operar no nível mais alto do gerenciador de tarefas (Scheduler P0), com autoridade de interrupção sobre qualquer thread da Natacha, incluindo Estados Próprios, sem negociação e sem espera por ponto seguro Alta Injetar evento FELIX_CRITICAL durante estado BANHO e validar interrupção imediata
RF-20 Postar eventos de forma assíncrona no Event Bus baseado na variação crítica de seus índices biológicos internos Alta Simular FOME > 80% e validar publicação de evento FELIX_HUNGRY
RF-21 Derivar o comportamento do Félix a partir de algoritmos estocásticos (ex.: Cadeias de Markov ou matrizes de probabilidade) baseados no estado das variáveis biológicas, sem uso de scripts determinísticos Alta Executar simulação de 1000 ciclos e validar ausência de padrões repetitivos fixos
RF-22 Possuir privilégio de interrupção capaz de alterar o comportamento ativo e a rotina da Natacha instantaneamente Alta Injetar evento FELIX_CRITICAL e validar interrupção em < 100ms
RF-23 Ignorar inputs e comandos diretos do usuário, respondendo exclusivamente às interações de ambiente e da Natacha Média Enviar comando direto ao Félix e validar ausência de resposta
RF-24 Calcular e atualizar continuamente a coordenada física (cômodo) ocupada pelo Félix na Casa virtual Baixa Simular movimento e validar atualização de coordenada em tempo real

3.4 A Casa (Ambiente Simulado)

ID Requisito Funcional Prioridade Método de Teste / Aceite
RF-25 Executar relógio interno acelerado que dita as rotinas, horários de refeição e comportamento dos agentes Média Validar aceleração temporal configurável (ex: 1 min real = 1 hora virtual)
RF-26 Disparar eventos de perturbação randômicos como alterações climáticas simuladas e monitoramento de aquecimento de chips Média Executar simulação e validar disparo de eventos aleatórios
RF-27 Espelhar o consumo real de hardware do servidor onde está hospedado para definir o estado de organização dos cômodos da Casa Baixa Monitorar CPU/RAM e validar alteração de estado dos cômodos
RF-28 Impor barreiras de navegação ao usuário com base no nível de isolamento definido pelo cômodo atual ocupado pela Natacha Baixa Posicionar Natacha no Banheiro e validar bloqueio de acesso

3.5 Core Neural (Evolução de IA em C++)

ID Requisito Funcional Prioridade Método de Teste / Aceite
RF-29 Implementar modelagem e treinamento supervisionado de um Neurônio Perceptron simples (ex: porta lógica OR) Alta Teste de integração validando convergência da taxa de erro a zero em <= 1000 épocas
RF-30.1 Implementar rede Multilayer Perceptron (MLP) com suporte a Backpropagation Alta Validar convergência em dataset de teste com erro < 0.01
RF-30.2 Disponibilizar funções de ativação Sigmoid, Leaky ReLU e Softmax Alta Teste unitário para cada função com valores conhecidos
RF-30.3 Executar Early Stopping durante a fase de treinamento Alta Validar parada automática quando erro de validação não melhora por N épocas
RF-31 Implementar algoritmo em C++ para geração e manipulação de vetores densos de palavras (Word Embeddings via Skip-gram com Negative Sampling, janela k=5 e dimensão d=32) Alta Treinar em corpus de teste e validar similaridade semântica entre palavras coocorrentes
RF-31.1 Persistir vetores de embeddings treinados em formato JSON para carregamento sem re-treinamento Alta Salvar embeddings, reiniciar sistema e validar carregamento correto
RF-32 Implementar arquitetura de rede neural recorrente (RNN) com células LSTM para manipulação de cadeias de sequências Alta Teste unitário com sequência de entrada e validação de saída
RF-33 Implementar de forma nativa a operação matricial de mecanismo de autoatenção (Self-Attention) Alta Teste unitário com matriz de entrada e validação de pesos de atenção
RF-34 Agrupar componentes neurais construindo bloco funcional de arquitetura Transformer completo em C++ Alta Teste de integração validando forward pass completo
RF-35 Permitir treinamento inicial e carregamento de uma Small Language Model (SLM) proprietária contendo entre 10M e 50M de parâmetros Média Validar carregamento de modelo e inferência em < 5s
RF-36 Executar motor de inferência inteiramente de forma local na máquina hospedeira Média Desconectar rede e validar funcionamento contínuo
RF-37 Gerenciar concorrentemente chamadas de background do Modo Agente e o loop principal da videochamada no Modo Chat sem travar a interface Alta Executar ambos modos simultaneamente e validar responsividade da UI

3.6 Sótão Quântico (Módulo Futuro / Escopo Desejável)

ID Requisito Funcional Prioridade Método de Teste / Aceite
RF-38 Reservar e expor o tópico QUANTUM_REQUEST no Event Bus para integração futura com IBM Quantum via Qiskit (Escopo Futuro / Desejável) Baixa Validar existência do tópico e aceitação de mensagens
RF-38.1 Tratar o ruído de readout e a decoerência quântica como sinal rico em informação, e não como erro/lixo a ser descartado, alimentando o módulo de entropia criativa com amostras verdadeiramente aleatórias provenientes de medições quânticas Baixa Comparar distribuição de amostras geradas com distribuição uniforme esperada; validar uso em rotina de amostragem de probabilidades
RF-38.2 Expor conectores lógicos preparados para acoplamento com lógica de fotônica (motores de luz — conversão de calor residual em trabalho útil) e com o regime de metais estranhos (estados exóticos de criticalidade quântica) Baixa Validar existência dos conectores e ausência de acoplamento direto com a lógica clássica (compatível com RNF-12)
RF-38.3 Preparar circuitos em Python/Qiskit (portas Hadamard, CNOT, fase) e enviá-los para execução híbrida clássico-quântica, processando o resultado em C++ no núcleo da Natacha Baixa Executar circuito Bell em simulador Aer e validar correlação entre qubits; documentar fluxo Python↔C++

---

4. REQUISITOS NÃO-FUNCIONAIS

4.1 Desempenho (Performance)

ID Requisito Não-Funcional Categoria FURPS+ Métrica / Critério de Aceite
RNF-01 Latência do Chat Performance O tempo entre término do input de voz e início da síntese/animação deve ser < 2 segundos
RNF-02 Velocidade do Event Bus Performance A latência máxima interna na entrega de mensagens entre tópicos assinados deve ser < 10 milissegundos
RNF-03 Velocidade de Treinamento Base Performance O ciclo de treinamento do neurônio OR deve ser concluído em < 1 segundo para 1000 épocas
RNF-04 Pegada de Memória Performance / Supportability O consumo de RAM do core do sistema (excluindo LLM) deve ser < 2 GB em repouso e pico

4.2 Segurança e Confiabilidade (Security / Reliability)

ID Requisito Não-Funcional Categoria FURPS+ Métrica / Critério de Aceite
RNF-05 Isolamento de Banco de Dados Security Nenhuma brecha ou comando textual pode expor ou executar queries de manipulação direta
RNF-06 Auditoria e Imutabilidade Reliability / Security Logs gravados com append-only e validação por Hashing SHA-256
RNF-07 Persistência em Falhas Reliability Quedas abruptas não podem corromper arquivos de memória de longo prazo
RNF-08 Metodologia de Teste Supportability Cobertura de testes unitários mínima de 80% para módulos neurais

4.3 Portabilidade e Usabilidade (Usability / Supportability)

ID Requisito Não-Funcional Categoria FURPS+ Métrica / Critério de Aceite
RNF-09 Compilação Multiplataforma Supportability Build nativo funcional em Linux e Windows (GCC/MinGW ou MSVC)
RNF-10 Ausência de Bloqueio Crítico Reliability Erros no Modo Agente não podem derrubar o Modo Chat ou ecossistema principal
RNF-11 Monitoramento de Épocas Usability Interface deve plotar taxa de erro decrescente por época em tempo real

4.4 Restrições Arquiteturais (FURPS+ Supportability)

ID Requisito Não-Funcional Categoria FURPS+ Métrica / Critério de Aceite
RNF-12 Comunicação Exclusiva via Event Bus + (Restrição Arquitetural) Proibido acoplamento direto ou chamadas síncronas entre módulos core; toda troca deve ocorrer via pub/sub assíncrono em C++

---

5. CASOS DE USO PRINCIPAIS

UC-01 — Conversar no Modo Chat (Videochamada Simulada)

· Ator Principal: Usuário.
· Pré-condição: Natacha ativa e em estado disponível na Casa.
· Fluxo Principal:
  1. O usuário inicia a interface de "Videochamada".
  2. O sistema renderiza o avatar da Natacha.
  3. O usuário envia uma mensagem por voz ou texto.
  4. A Natacha processa o tom rebelde e gera a resposta contextualizada.
  5. O módulo de áudio sintetiza a voz e o avatar executa o lip sync.
· Fluxo Alternativo (Natacha Ocupada):
  1. A Natacha está no estado BANHO (Zona Privada).
  2. O sistema exibe animação da porta trancada e impede o início da chamada.
  3. Resposta estática: "Tô no banho, não amola. Volto em 15 minutos se eu quiser."

UC-02 — Treinar o Neurônio OR

· Ator Principal: Desenvolvedor.
· Pré-condição: Dataset com a tabela verdade da porta OR instanciado.
· Fluxo Principal:
  1. O desenvolvedor seleciona o comando de calibração do neurônio primitivo.
  2. O sistema zera variáveis e atribui pesos randômicos iniciais.
  3. O loop computa o forward pass multiplicando entradas pelos pesos.
  4. O erro é extraído comparando resultado com a tabela verdade.
  5. Os pesos são corrigidos via Regra de Aprendizado do Perceptron (Delta Rule).
  6. O sistema exibe o sumário de convergência e a taxa de erro zerada.

UC-03 — Orquestrar Demanda em Projeto Externo (ARCA)

· Ator Principal: API do Projeto ARCA.
· Pré-condição: Natacha rodando em background com Modo Agente ativo.
· Fluxo Principal:
  1. O ARCA envia requisição JSON com lista de tarefas.
  2. O Modo Agente valida o token e injeta o perfil restrito de leitura.
  3. A Natacha seleciona as ferramentas locais permitidas.
  4. O sistema executa funções controladas em C++ sem queries diretas.
  5. O resultado é devolvido via API REST.
· Fluxo Alternativo (Dados Sensíveis):
  1. O sistema identifica dados altamente sensíveis.
  2. A Natacha aborta chamadas de APIs externas e transfere execução para o SLM local.
· Fluxo Alternativo (Solicitação com Impacto Negativo):
  1. O ARCA solicita refatoração que quebra contrato de API do Event Bus.
  2. O Analisador Estático (AST) identifica dependências afetadas.
  3. A Natacha suspende a execução e emite parecer crítico com a persona ativa.
  4. O sistema sugere abordagem alternativa preservando o contrato.
  5. Aguarda confirmação do usuário ou do sistema parceiro.
· Fluxo Alternativo (Requisição Fútil ou Antiética):
  1. O sistema recebe requisição que se enquadra em RN-10.
  2. A Natacha emite recusa: recusa técnica objetiva, ironia fina ou silêncio prolongado.
  3. Nenhuma ação é executada no ecossistema.

UC-04 — Interrupção Autônoma do Félix

· Ator Principal: Agente Félix (IAgato).
· Pré-condição: Natacha executando qualquer tarefa.
· Fluxo Principal:
  1. O medidor interno de FOME do Félix ultrapassa o limiar crítico de 80%.
  2. A thread do Félix injeta evento FELIX_HUNGRY no Event Bus com prioridade P0.
  3. O interceptor da Natacha captura o sinal.
  4. Cenário A (Natacha em conversa): Interrompe o usuário com fala ácida: "Tenho que ir, o chato do Félix tá miando querendo sachê no teclado. Espera aí."
  5. Cenário B (Natacha em Zona Privada): Altera estado interno em background mas mantém barreira de privacidade ativa para o usuário.
  6. Cenário C (Sistema detecta violação ética): Félix bloqueia a thread e notifica a Natacha, que aborta a operação.

UC-05 — Operar Ferramenta Externa sob Demanda

· Ator Principal: Natacha (agente autônomo).
· Pré-condição: Modo Agente ativo, Event Bus operacional.
· Fluxo Principal:
  1. A Natacha recebe requisição de formatação de documento.
  2. Avalia natureza da tarefa e seleciona dinamicamente o EditeCC (RF-11.1).
  3. Publica evento no Event Bus com escopo restrito.
  4. EditeCC executa e devolve resultado via Event Bus.
  5. Natacha valida, registra em log (RF-15) e responde à origem.
· Fluxo Alternativo (Ferramenta Indisponível):
  1. EditeCC não responde dentro do timeout.
  2. Natacha tenta rota alternativa (SLM local) ou informa indisponibilidade com a persona ativa.

---

6. MATRIZ DE RASTREABILIDADE

6.1 Rastreabilidade de Requisitos Funcionais

ID Requisito Regra de Negócio Caso de Uso Componente / Módulo
RF-01.1 RN-01 UC-01 Interface Conversacional
RF-01.2 RN-01 UC-01 Pipeline STT
RF-02 RN-05 UC-01 Memória de Longo Prazo / Persistência
RF-03 RN-01 UC-01 Engine de Personalidade
RF-04 RN-02 UC-01 Máquina de Estados (Core)
RF-05 RN-01 / RN-06 UC-01 Front-end UI (Videochamada)
RF-06 RN-01 UC-01 Pipeline TTS
RF-07 RN-01 UC-01 Pipeline STT
RF-08 RN-01 UC-01 Módulo de Lip Sync
RF-09 RN-01 / RN-02 UC-01 Engine de Personalidade
RF-10 RN-01 / RN-03 UC-01 / UC-04 Interceptor de Eventos (Félix)
RF-11 RN-04 / RN-07 / RN-11 UC-03 / UC-05 Modo Agente (Orchestrator)
RF-11.1 RN-11 UC-05 Seletor Dinâmico de Ferramentas
RF-12 RN-04 / RN-05 UC-03 Roteador SLM/API
RF-13 RN-07 UC-03 Camada de API (REST/WebSocket)
RF-14 RN-04 UC-03 Camada de Segurança (Funções Controladas)
RF-15 RN-04 / RNF-06 UC-03 Camada de Auditoria
RF-16 RN-04 UC-03 Gerenciador de Perfis de Escopo
RF-17 RN-04 UC-03 Módulo de Correlação de Dados
RF-18 RN-04 UC-03 Gatilho de Validação Humana
RF-18.1 RN-09 UC-03 Modo Agente (Analisador Estático / AST / Evaluator)
RF-18.2 RN-10 UC-03 Veto Autônomo (Evaluator de Recusa)
RF-19 RN-03 UC-04 Thread Félix (IAgato)
RF-19.1 RN-03 UC-04 Scheduler P0 / Interceptor de Thread
RF-20 RN-03 / RN-07 UC-04 Event Bus (Publisher Félix)
RF-21 RN-03 UC-04 Engine Comportamental Estocástica (Félix)
RF-22 RN-03 UC-04 Central do Event Bus
RF-23 RN-03 UC-04 Interceptor de Inputs (Félix)
RF-24 RN-06 UC-04 Mapa de Coordenadas (HouseStateMap)
RF-25 RN-06 --- Relógio Interno (Casa)
RF-26 RN-06 --- Gerador de Eventos Randômicos (Casa)
RF-27 RN-06 --- Monitor de Hardware / Casa
RF-28 RN-08 UC-01 / UC-04 Controle de Acesso por Zona
RF-29 RN-05 UC-02 Engine Neural Primitiva (Perceptron)
RF-30.1 RN-05 UC-02 Engine Neural (MLP + Backpropagation)
RF-30.2 RN-05 UC-02 Engine Neural (Funções de Ativação)
RF-30.3 RN-05 UC-02 Engine Neural (Early Stopping)
RF-31 RN-05 --- Módulo de Word Embeddings (Skip-gram)
RF-31.1 RN-05 --- Persistência JSON (Embeddings)
RF-32 RN-05 --- Engine Neural (RNN/LSTM)
RF-33 RN-05 --- Engine Neural (Self-Attention)
RF-34 RN-05 --- Engine Neural (Transformer)
RF-35 RN-05 --- Carregador de SLM
RF-36 RN-05 --- Motor de Inferência Local
RF-37 RN-07 UC-01 / UC-03 Gerenciador de Threads
RF-38 RN-07 --- Event Bus (Reserva Quântica)
RF-38.1 RN-01 / RN-06 --- Sótão Quântico (Entropia Criativa)
RF-38.2 RN-06 --- Sótão Quântico (Conectores Fotônica / Metais Estranhos)
RF-38.3 RN-06 --- Ponte Python/Qiskit ↔ C++

6.2 Rastreabilidade de Requisitos Não-Funcionais

ID Requisito Regra de Negócio Caso de Uso Componente / Módulo
RNF-01 RN-01 UC-01 Pipeline Audio/TTS
RNF-02 RN-07 / RNF-12 Todos Core Event Bus C++
RNF-03 RN-05 UC-02 Engine Neural Primitiva
RNF-04 RN-05 Todos Core do Sistema (Gerenciador de Memória)
RNF-05 RN-04 UC-03 Sentinel / DB Proxy
RNF-06 RN-04 UC-03 Camada de Auditoria
RNF-07 RN-05 --- Persistência de Memória de Longo Prazo
RNF-08 RN-05 UC-02 Suíte de Testes Neurais
RNF-09 RN-06 Todos Build System / Portabilidade
RNF-10 RN-07 UC-01 / UC-03 Isolamento de Processos
RNF-11 RN-05 UC-02 Interface de Desenvolvimento (Plot de Épocas)
RNF-12 RN-07 Todos Infraestrutura (Event Bus)

6.3 Rastreabilidade Reversa (Regras de Negócio → Requisitos)

Regra Implementada por
RN-01 RF-01.1, RF-01.2, RF-03, RF-05, RF-06, RF-07, RF-08, RF-09, RF-10, RNF-01
RN-02 RF-04, RF-09, RF-19.1
RN-03 RF-10, RF-19, RF-19.1, RF-20, RF-21, RF-22, RF-23
RN-04 RF-11, RF-12, RF-14, RF-15, RF-16, RF-17, RF-18, RNF-05, RNF-06
RN-05 RF-02, RF-12, RF-29 a RF-36, RNF-03, RNF-04, RNF-07, RNF-08, RNF-11
RN-06 RF-05, RF-24, RF-25, RF-26, RF-27, RF-38.1, RF-38.2, RF-38.3, RNF-09
RN-07 RF-11, RF-13, RF-20, RF-37, RF-38, RNF-02, RNF-10, RNF-12
RN-08 RF-28
RN-09 RF-18.1
RN-10 RF-18.2
RN-11 RF-11, RF-11.1

---

7. GLOSSÁRIO

· Estado Próprio: Rotinas internas independentes simuladas pela IA de forma cronológica (comer, dormir, higiene) que sobrepõem as ordens de chamada do usuário.
· Função Controlada: Métodos encapsulados e validados em C++ que servem como única ponte segura de comunicação para leitura ou gravação de informações no sistema.
· Evento: Estrutura de dados contendo cabeçalho, tipo de gatilho, payload e nível de prioridade, transmitida de forma assíncrona pelo barramento.
· Perfil de Escopo: Máscara de configuração injetada dinamicamente que limita ou expande os comandos que a Natacha pode acionar ao receber chamadas de um microsserviço específico.
· Lip Sync: Processamento matemático que deforma a malha do avatar casando a abertura da boca com as frequências sonoras da voz gerada.
· Small Language Model (SLM): Modelos de linguagem reduzidos projetados para rodar localmente com baixo consumo de memória.
· QUANTUM_REQUEST: Tópico reservado no Event Bus para integração futura com IBM Quantum via Qiskit. Classificado como Escopo Futuro / Desejável.
· Motor de Luz: Conceito da termodinâmica quântica semiclássica que converte calor residual em trabalho útil; inspiração para o módulo de entropia criativa.
· HouseStateMap: Estrutura em memória que mapeia o estado lógico de cada cômodo da Casa virtual, sincronizado com variáveis de monitoramento de hardware.
· FURPS+: Framework de classificação de requisitos não-funcionais: Functionality, Usability, Reliability, Performance, Supportability, e restrições adicionais (+).
· Skip-gram com Negative Sampling: Algoritmo de treinamento de Word Embeddings que prediz palavras de contexto a partir de uma palavra-alvo, utilizando amostragem negativa para otimização computacional.
· Cadeia de Markov: Modelo estocástico que descreve transições entre estados, onde o próximo estado depende apenas do estado atual — utilizado para gerar comportamento emergente no Félix.
· AST (Abstract Syntax Tree): Representação estrutural do código-fonte em forma de árvore, utilizada para análise estática e verificação de dependências antes da execução de alterações.
· Scheduler P0: Camada de mais alta prioridade no gerenciador de tarefas. Félix opera neste nível, com autoridade de interrupção irrestrita sobre qualquer thread da Natacha.
· Veto Autônomo: Prerrogativa formal da Natacha de recusar execução de requisições com base em futilidade, violação ética, comando imperativo sem sentido ou impacto arquitetural negativo.
· Ruído Quântico como Sinal: Premissa teórica de que flutuações estocásticas em sistemas quânticos (readout, decoerência) são fonte de informação e não erro — base do módulo de entropia criativa do Sótão Quântico.
· Metais Estranhos (Strange Metals): Regimes físicos onde o ruído carrega assinatura de criticalidade quântica; conector lógico previsto para o Sótão Quântico.
· Ferramenta Externa: Subsistema do ecossistema (ARCA, EditeCC, Sentinel, Analytics) que a Natacha aciona dinamicamente via Event Bus, sem acoplamento rígido.

---

"Você não está construindo apenas uma IA. Está construindo um mundo."

— Documento de Especificação, Projeto Natacha — Arquitetura de Alma.

---
