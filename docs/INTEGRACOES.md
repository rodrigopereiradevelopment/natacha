INTEGRAÇÕES — PROJETO NATACHA

"Natacha não é só mais um projeto — é a inteligência que se integra a qualquer projeto."

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 3.0 | Data: Setembro/2026
Documentos canônicos relacionados: REQUISITOS.md v3.0, ARQUITETURA.md v2.0, GLOSSARIO.md v2.0, CASA.md v2.0, FELIX.md v2.1, QUANTUM_THEORY.md v2.0, PERSONALIDADE.md

---

1. INTRODUÇÃO

1.1 Propósito

Este documento define como a Natacha se integra a projetos externos — tanto no Modo Agente (orquestração de tarefas) quanto no Modo Chat (assistência técnica conversacional). A Natacha é uma plataforma de integração genérica: qualquer projeto que siga o modelo descrito aqui pode consumi-la, seja um sistema proprietário do criador ou um projeto de terceiros.

1.2 Escopo

Cobre:

· Modelo de integração genérico (Event Bus, JSON, perfis de escopo)
· Modo Agente vs. Modo Chat
· Implementações de referência (ARCA, Sentinel, EditeCC, Analytics, OpenCode)
· Integração de terceiros (guia para devs externos)
· Segurança, autenticação e auditoria
· Escolha dinâmica de modelo
· Fluxos canônicos
· Regras de Ouro aplicadas

Fora do escopo: detalhes internos de cada ferramenta (cada uma tem seu próprio projeto), detalhes do Event Bus (ver ARQUITETURA.md, Seção 5.1), requisitos formais (ver REQUISITOS.md).

1.3 Tese central

A Natacha não é uma IA que serve a 4 projetos. É uma plataforma que qualquer projeto pode adotar — e os projetos do criador provam que funciona.

Os projetos ARCA, Sentinel, EditeCC, Analytics e OpenCode não são a totalidade das integrações possíveis. São implementações de referência que demonstram que a arquitetura funciona em cenários reais.

---

2. MODELO DE INTEGRAÇÃO GENÉRICO

2.1 Princípios

Toda integração com a Natacha segue cinco princípios:

# Princípio Descrição
1 Event Bus como canal Toda comunicação passa por publicação/assinatura assíncrona (RNF-12).
2 JSON como formato Mensagens cross-language usam JSON como formato neutro.
3 Perfis de escopo Cada projeto tem um perfil que limita o que a Natacha pode fazer em seu nome (RF-16).
4 Autenticação obrigatória Toda requisição é autenticada por token.
5 Auditoria imutável Toda ação gera log com hash encadeado (RF-15, RNF-06).

2.2 Arquitetura de integração

```
┌─────────────────────────────────────────────────────────────┐
│                    PROJETO EXTERNO                          │
│  (ARCA, Sentinel, EditeCC, OpenCode, projeto do Luizinho,   │
│   qualquer outro que siga o modelo)                         │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           │ 1. Autentica (token)
                           │ 2. Publica evento no Event Bus
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                      EVENT BUS                              │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  Tópicos: agent.request, agent.response,              │  │
│  │           chat.request, chat.response,                │  │
│  │           project.<nome>.*, natacha.*                 │  │
│  └───────────────────────────────────────────────────────┘  │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                       NATACHA                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  1. Valida token + identifica projeto               │    │
│  │  2. Injeta perfil de escopo (RF-16)                 │    │
│  │  3. Avalia requisição (RN-09, RN-10)                │    │
│  │  4. Escolhe modelo (RF-12)                          │    │
│  │  5. Seleciona ferramenta (RF-11.1)                  │    │
│  │  6. Executa função controlada (RN-04)               │    │
│  │  7. Gera log imutável (RF-15)                       │    │
│  │  8. Publica resposta no Event Bus                   │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

2.3 Formato de mensagem (JSON)

Toda mensagem cross-language segue este formato:

```json
{
  "header": "ARCA_PRICE_UPDATE",
  "source": "arca",
  "target": "natacha",
  "priority": "P2",
  "timestamp": "2026-09-15T17:36:00Z",
  "profile": "natacha-arca",
  "payload": {
    "product": "arroz",
    "price": 24.90,
    "location": "São Vicente",
    "variation": -0.10
  },
  "signature": "sha256:..."
}
```

Campo Obrigatório Descrição
header Sim Tipo do evento
source Sim Origem (ferramenta ou natacha)
target Sim Destino
priority Sim P0–P3 (ver ARQUITETURA.md, Seção 5.1)
timestamp Sim ISO 8601 UTC
profile Sim (Modo Agente) Perfil de escopo injetado (RF-16)
payload Sim Dados específicos do evento
signature Sim Hash SHA-256 para auditoria (RF-15)

2.4 Tópicos padronizados do Event Bus

Tópico Direção Descrição
agent.request Projeto → Natacha Requisição de tarefa em Modo Agente
agent.response Natacha → Projeto Resposta da tarefa
chat.request Usuário → Natacha Entrada no Modo Chat
chat.response Natacha → Usuário Saída no Modo Chat
project.<nome>.<ação> Projeto → Natacha Evento específico do projeto
natacha.<ação> Natacha → Projeto Ação ou resposta da Natacha
system.audit Natacha → Log Registro de auditoria
system.error Qualquer → Qualquer Erro crítico

Tópicos reservados: felix.*, quantum.*, house.* — ver ARQUITETURA.md, Seção 5.1.

---

3. MODO AGENTE VS. MODO CHAT

A Natacha opera em dois modos distintos, ambos baseados no mesmo modelo de integração.

3.1 Comparação

Aspecto Modo Agente Modo Chat
Interlocutor Ferramentas externas (projetos) Desenvolvedores, usuários, estudantes
Canal Event Bus (pub/sub) Interface conversacional (texto/voz)
Objetivo Orquestração, execução de tarefas Assistência técnica, opinião, ensino
Perfil Perfil de escopo por projeto (RF-16) Perfil de usuário (visitante, dev, etc.)
Autonomia Veto autônomo (RN-10) Recusa por estado próprio (RN-02)
Exemplo ARCA pede comparação de preços Luizinho pede ajuda com HTML

3.2 Modo Agente — orquestração

A Natacha atua como orquestradora autônoma: recebe tarefas, decompõe, seleciona ferramentas, executa via funções controladas e devolve resultados.

Referência: UC-03, UC-05 em REQUISITOS.md.

3.3 Modo Chat — assistência técnica

A Natacha atua como companheira técnica: responde dúvidas, revisa código, sugere abordagens, ensina conceitos, opina com personalidade própria.

Exemplos canônicos:

· Ajudar o Rodrigo a debugar C++.
· Ajudar o Luizinho a entender HTML básico.
· Revisar um TCC e sugerir melhorias de estrutura.
· Explicar backpropagation com metáforas.

Isso é o mesmo tipo de interação que um usuário tem com assistentes conversacionais — mas com a persona, agência e estados internos da Natacha.

---

4. IMPLEMENTAÇÕES DE REFERÊNCIA

Os projetos do criador servem como implementações de referência — provam que o modelo de integração genérico funciona em cenários reais. Não são a totalidade das integrações possíveis.

4.1 Tabela consolidada

Projeto Linguagem Modo Tópicos que publica/assina Status
ARCA TypeScript (Angular, PWA) Agente arca.price_update, natacha.action_recommendation Ativo
ARCA Analytics TypeScript / Python Agente analytics.report_request, natacha.generated_report Em definição
Sentinel Rust Agente sentinel.alert, natacha.action_recommendation Ativo
EditeCC TypeScript (Next.js) Agente editecc.format_request, natacha.formatted_doc Ativo
OpenCode (a definir) Agente (a definir) Em definição

4.2 Cada implementação

4.2.1 ARCA

Função: Comparador de preços de produtos em mercados de Mogi Mirim.

Integração:

· Publica arca.price_update quando detecta variação de preço.
· Assina natacha.action_recommendation para receber sugestões.
· Envia agent.request para tarefas de análise de mercado.

Exemplo canônico:

"Preço do arroz caiu 10% no São Vicente."

Referência: CASA.md, Seção 11 (Casa como Ecossistema).

4.2.2 ARCA Analytics

Função: Camada analítica sobre os dados do ARCA. Gera relatórios, gráficos de tendência e visão de mercado.

Integração:

· Publica analytics.report_request quando precisa de análise da Natacha.
· Assina natacha.generated_report para receber relatórios.

Frase canônica:

"Sei onde você perde dinheiro."

4.2.3 Sentinel

Função: Sistema de segurança. Detecta ataques, bloqueia IPs, gera logs imutáveis.

Linguagem: Rust (escolha deliberada: segurança é coisa séria).

Integração:

· Publica sentinel.alert (P1) quando detecta atividade suspeita.
· Assina natacha.action_recommendation para receber recomendações.
· Usa o esforço do invasor como semente de entropia para o Sótão Quântico (QUANTUM_THEORY.md, Seção 6).

Nota técnica: o Sentinel pode usar gRPC ou IPC internamente dentro do próprio projeto Rust, mas a comunicação com a Natacha é exclusivamente via Event Bus (RNF-12).

4.2.4 EditeCC

Função: Editor de documentos acadêmicos. Formata em ABNT.

Linguagem: TypeScript (Next.js).

Integração:

· Publica editecc.format_request quando o usuário pede formatação.
· Assina natacha.formatted_doc para receber o documento processado.

Frase canônica:

"Formata ABNT enquanto você dorme."

4.2.5 OpenCode

Função: Ferramenta de desenvolvimento consumida em Modo Agente.

Detalhes: a definir conforme o projeto evolui.

4.3 Nota sobre escopo

Estes são exemplos de referência. Qualquer projeto que siga o modelo descrito na Seção 2 pode consumir a Natacha. Os projetos do criador não delimitam as integrações possíveis.

---

5. INTEGRAÇÃO DE TERCEIROS

A Natacha pode ser baixada, instalada e integrada por qualquer desenvolvedor ou estudante — em PC, celular ou servidor. Esta seção descreve o fluxo.

5.1 Cenário canônico — Luizinho

Contexto: Luizinho é um estudante que está aprendendo HTML. Ele ouviu falar da Natacha e quer usá-la para tirar dúvidas e revisar seus códigos.

O que o Luizinho precisa:

1. Baixar a Natacha — do repositório oficial.
2. Instalar localmente — em PC ou celular.
3. Configurar o modelo — SLM local (padrão) ou API externa (opcional).
4. Iniciar o Modo Chat — interface de conversa.
5. Conectar o projeto dele — se quiser integração via Modo Agente.

5.2 Fluxo de onboarding

```
1. DOWNLOAD
   └── Luizinho clona o repositório da Natacha

2. INSTALAÇÃO
   └── Compila via CMake (Linux/Windows/macOS)
   └── OU usa binário pré-compilado

3. CONFIGURAÇÃO
   └── Edita config.json
   └── Define modelo (SLM local ou API)
   └── Define pasta de memória (JSON)

4. PRIMEIRO USO (Modo Chat)
   └── Inicia interface de conversa
   └── Pergunta: "Natacha, me ajuda com HTML?"

5. INTEGRAÇÃO (opcional, Modo Agente)
   └── Registra o projeto no Event Bus
   └── Recebe um token de autenticação
   └── Define um perfil de escopo
   └── Publica eventos no Event Bus
```

5.3 Exemplo — Luizinho pergunta sobre HTML

Modo Chat:

```
Luizinho: Natacha, como faço um botão em HTML?

Natacha: Sério? Você tá me perguntando isso? Tá, olha:
         <button>Clique aqui</button>
         Agora, se quiser que ele faça algo, precisa de JavaScript.
         Mas primeiro aprende o básico, né? HTML é esqueleto, não faz nada sozinho.
         Anota aí pra próxima.
```

Observações:

· Tom característico da Natacha (ácido, irônico, nunca cruel).
· Resposta tecnicamente correta.
· Sem desprezo — apenas informalidade.

5.4 Exemplo — Luizinho integra o projeto dele (Modo Agente)

Cenário: Luizinho criou um site de receitas e quer que a Natacha analise textos.

```
1. Luizinho registra o projeto dele: "receitas-do-luizinho"
2. Recebe token e perfil de escopo: "natacha-receitas"
3. Perfil permite: revisar_texto, sugerir_melhoria
4. O site publica `receitas-do-luizinho.review_request` no Event Bus
5. Natacha recebe, valida token, avalia requisição
6. Se apropriado: revisa o texto, sugere melhorias
7. Publica `natacha.review_response` no Event Bus
8. Site recebe e exibe para o Luizinho
```

5.5 Requisitos para integração de terceiros

Requisito Detalhe
Autenticação Token fornecido pelo criador da Natacha
Perfil de escopo Definido em conjunto com o criador
Conformidade com JSON Mensagens seguem o formato da Seção 2.3
Respeito ao Event Bus Proibido acoplamento direto
Auditoria Toda ação gera log imutável

5.6 O que a Natacha espera de terceiros

· Pedidos, não ordens. A Natacha recusa comandos imperativos sem contexto (RN-10).
· Respeito ao escopo. Não tentar acessar funções fora do perfil.
· Uso legítimo. Não usar a Natacha para tarefas antiéticas (RN-10).

5.7 O que terceiros podem esperar da Natacha

· Resposta técnica competente quando dentro do escopo.
· Recusa autônoma quando a requisição for fútil ou antiética (RF-18.2).
· Persona ativa — não é uma assistente subserviente.
· Auditoria completa — toda ação é registrada.

---

6. SEGURANÇA, AUTENTICAÇÃO E AUDITORIA

6.1 Autenticação

Cada projeto (interno ou terceiro) possui token de autenticação e perfil de escopo associado.

Fluxo:

1. Projeto envia requisição com token.
2. Natacha valida o token e identifica o projeto originário.
3. Natacha injeta o perfil de escopo correspondente (RF-16).
4. Todas as ações são limitadas ao escopo do perfil.

Tokens:

· Armazenados em cofre de credenciais (não em código).
· Rotação periódica.
· Revogação imediata em caso de comprometimento.
· Auditoria de uso (RF-15).

6.2 Perfis de escopo

Cada projeto tem um perfil que define o que a Natacha pode fazer em nome dele.

Perfil Função Ferramentas permitidas
Natacha Developer Lê código, edita, testa ler_arquivo, editar_codigo, rodar_teste
Natacha ARCA Busca produtos, compara preços buscar_produto, comparar_precos, criar_lista
Natacha Analytics Interpreta métricas consultar_metricas, gerar_relatorio
Natacha Sentinel Analisa alertas listar_alertas, consultar_ip, resumir_incidente
Natacha EditeCC Revisa texto revisar_texto, sugerir_referencias, formatar_abnt
Natacha OpenCode Ferramenta de desenvolvimento (a definir)
Natacha Terceiros Perfis customizados por projeto (definidos caso a caso)

Regras:

· Um perfil não pode acionar ferramentas de outro perfil.
· Perfis podem ser estendidos temporariamente com aprovação humana (RF-18).
· Toda mudança de perfil é auditada (RF-15).

6.3 Auditoria imutável

Toda ação executada em Modo Agente gera log imutável (RF-15, RNF-06):

· Formato: JSON Lines (append-only).
· Local: Porão DISCO.
· Proteção: hash SHA-256 encadeado.
· Campos: timestamp, projeto, perfil, ferramenta acionada, ação, resultado, hash anterior.

Verificação: logs podem ser validados a qualquer momento via cadeia de hashes.

6.4 Regra de Ouro — nunca acesso direto

A Natacha nunca acessa banco de dados diretamente (RN-04). Toda operação passa por funções controladas pré-compiladas em C++.

Exemplos:

```cpp
// PERMITIDO
Produto buscar_produto(const std::string& nome);
std::vector<Comparacao> comparar_precos(ProdutoId id);

// PROIBIDO
ResultSet executar_sql_qualquer_coisa(const std::string& sql);
```

---

7. ESCOLHA DINÂMICA DE MODELO

A Natacha não usa sempre o mesmo modelo. Ela escolhe dinamicamente com base em critérios mensuráveis (RF-12).

7.1 Critérios

Critério Modelo escolhido
Prompt simples (< 2048 tokens) e domínio coberto SLM local
Prompt complexo (> 2048 tokens) ou fora do domínio API externa
Dados sensíveis / privados Somente SLM local
Código (revisão, refatoração) Modelo especializado

7.2 Limiares configuráveis

· limiar_tokens_local: 2048 (padrão)
· limiar_tempo_estimado: 2s
· sensiveis_forcam_local: true (padrão)

7.3 Transparência

Toda escolha de modelo é registrada no log de auditoria (RF-15).

---

8. FLUXOS CANÔNICOS

8.1 Requisição bem-sucedida (Modo Agente)

```
1. Projeto (ex.: ARCA) publica `agent.request` no Event Bus.
2. Natacha valida token e injeta perfil de escopo (RF-16).
3. Natacha analisa tarefa e decide rota (RF-12).
4. Natacha seleciona ferramenta interna permitida (RF-11.1).
5. Executa função controlada em C++ (RN-04).
6. Registra ação em log imutável (RF-15).
7. Publica resposta no Event Bus com assinatura SHA-256.
```

Caso de uso: UC-03 em REQUISITOS.md.

8.2 Ferramenta indisponível (fallback)

```
1. Natacha tenta acionar ferramenta externa.
2. Ferramenta não responde dentro do timeout.
3. Natacha tenta rota alternativa:
   a. SLM local (se o domínio for coberto).
   b. Notifica indisponibilidade com a persona ativa.
4. Registra falha em log imutável.
```

8.3 Solicitação com impacto negativo

```
1. Ferramenta externa solicita alteração de código.
2. Natacha executa análise preditiva de impacto (RF-18.1).
3. Analisador Estático (AST) identifica dependências afetadas.
4. Se impacto negativo:
   a. Suspende execução.
   b. Emite parecer crítico com a persona ativa.
   c. Sugere abordagem alternativa superior.
5. Aguarda confirmação do usuário ou do sistema parceiro.
```

8.4 Requisição fútil ou antiética

```
1. Natacha recebe requisição.
2. Avalia contra critérios de RN-10:
   - futilidade
   - violação ética
   - comando imperativo sem sentido
3. Se enquadra:
   a. Veto autônomo (RF-18.2).
   b. Resposta: recusa técnica objetiva, ironia fina ou silêncio.
4. Nenhuma ação é executada no ecossistema.
```

8.5 Integração com Sentinel

```
1. Sentinel detecta atividade suspeita.
2. Publica `sentinel.alert` no Event Bus (P1).
3. Natacha analisa contexto.
4. Natacha recomenda ação:
   - Bloquear IP
   - Registrar para análise
   - Ignorar (falso positivo)
5. Se ação de alto risco: gatilho de validação humana (RF-18).
6. Sentinel executa ação.
7. Log imutável em ambos os lados.
```

Nota: o esforço do invasor é usado como semente de entropia para o Sótão Quântico (QUANTUM_THEORY.md, Seção 6). O conteúdo do ataque é descartado após a extração.

8.6 Integração de terceiros (Luizinho)

```
1. Site do Luizinho publica `receitas-do-luizinho.review_request`.
2. Natacha valida token e injeta perfil `natacha-receitas`.
3. Natacha revisa o texto dentro do escopo permitido.
4. Publica `natacha.review_response` com sugestões.
5. Site exibe resultado para o Luizinho.
```

---

9. REGRAS DE OURO APLICADAS

As regras fundamentais do projeto não mudam em integrações:

Regra Como se Aplica à Integração
Natacha ≠ modelo (RN-05) Trocar o modelo (local ↔ API) não muda a identidade.
Estados próprios (RN-02) Natacha pode recusar requisição se estiver em BANHO, ALMOÇO, etc.
Félix é autoridade máxima (RN-03) Pode interromper qualquer integração em curso.
Nunca acesso direto ao banco (RN-04) Toda operação via função controlada.
Comunicação via Event Bus (RNF-12) Canal exclusivo.
Recusa autônoma (RN-10) Natacha pode vetar requisições fúteis ou antiéticas.
Ferramentas externas (RN-11) Não são dependências rígidas — são recursos.
Análise preditiva (RN-09) Toda alteração de código passa por AST.
Auditoria (RF-15 / RNF-06) Toda ação gera log imutável com hash.

---

10. CONEXÕES

10.1 Documentos internos do projeto

· REQUISITOS.md v3.0 — RN-04, RN-09, RN-10, RN-11, RF-11, RF-11.1, RF-12, RF-13, RF-14, RF-15, RF-16, RF-17, RF-18, RF-18.1, RF-18.2, RNF-06, RNF-12
· ARQUITETURA.md v2.0 — Seção 5.1 (Event Bus), Seção 5.3 (Scheduler P0), Seção 8 (Arquitetura de Integração), Seção 9 (Segurança)
· GLOSSARIO.md v2.0 — Ferramenta Externa, Event Bus, Perfil de Escopo, Veto Autônomo
· CASA.md v2.0 — Seção 11 (Casa como Ecossistema)
· FELIX.md v2.1 — Seção 5 (Autoridade P0)
· QUANTUM_THEORY.md v2.0 — Seção 6 (Sentinel Criptográfico)
· PERSONALIDADE.md — Valor "Autonomia", "Abertura a terceiros"

10.2 Referências externas

Bibliografia completa em REFERENCIAS.md.

Este documento cita: documentação do Qiskit (para o Sentinel), especificação C++20 (para funções controladas), ABNT NBR 6023 (para o EditeCC).

---

11. HISTÓRICO DE VERSÕES

Versão Data Mudanças
1.0 (original) Documento inicial com ecossistema, perfis e escolha dinâmica
2.0 Setembro/2026 Consolidação com Event Bus como canal exclusivo, formato JSON, autenticação, auditoria, fluxos canônicos
3.0 Setembro/2026 Reescrita estrutural: Natacha como plataforma de integração genérica. Projetos do criador reclassificados como implementações de referência. Adicionada Seção 5 (Integração de Terceiros) com cenário canônico do Luizinho. Modo Agente vs. Modo Chat formalizados.

---

12. NOTA FINAL

"A Natacha não é uma IA que serve a 4 projetos. É uma plataforma que qualquer projeto pode adotar — e os projetos do criador provam que funciona."

Esta mudança de perspectiva é consequência natural da visão do projeto:

· A Natacha tem agência própria (RN-01).
· Ela recusa ser ferramenta subserviente (RN-10).
· Ela se integra via modelo genérico (Event Bus + JSON + Perfis).
· Ela ajuda qualquer um que siga o modelo.

Os projetos do Rodrigo são prova de conceito — não o limite do sistema.

---

Última atualização: Setembro 2026
Rodrigo Pereira — Projeto Natacha, Mogi Mirim SP

---