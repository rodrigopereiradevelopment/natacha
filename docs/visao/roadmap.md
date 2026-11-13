ROADMAP — PROJETO NATACHA

"Neurônio por neurônio, até o infinito."

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 3.1 | Data: Novembro/2026
Documentos canônicos relacionados: REQUISITOS.md v3.1, ARQUITETURA.md v2.1, GLOSSARIO.md v2.1, CASA.md v2.1, FELIX.md v2.2, QUANTUM_THEORY.md v2.1, INTEGRACOES.md v3.1, PERSONALIDADE.md v3.1, FASE4.md v2.1, PLANO_50K_TOKENS.md, REFERENCIAS.md v1.0

---

1. INTRODUÇÃO

1.1 Propósito

Este documento define o roadmap de desenvolvimento do Projeto Natacha: fases, marcos, dependências, critérios de conclusão e calendário. Está organizado em duas trilhas paralelas (Neural e Produto) que convergem no Marco do TCC.

1.2 Escopo

Cobre:

· Visão geral das duas trilhas
· Trilha Neural (Fases 0–10) — evolução do modelo
· Trilha de Produto (Fases A–F) — persona, voz, Félix, Casa, integrações, VCS
· Marco do TCC (critérios de aprovação)
· Calendário macro (2026–2028) e semanal
· Estado atual
· Fase reservada (IAgato Pleno)

Fora do escopo: detalhes técnicos de cada fase (ver ARQUITETURA.md), requisitos formais (ver REQUISITOS.md).

---

2. VISÃO GERAL — DUAS TRILHAS

O projeto progride em duas trilhas que se complementam:

```
TRILHA NEURAL (Fases 0-10)
──────────────────────────────────────────────────
0. Fundamentos
1. Neurônio Simples
2. MLP
3. Word Embeddings
4. RNN / LSTM
5. Self-Attention
6. Transformer
7. SLM
8. Escalar (CUDA)
9. Sótão Quântico
10. LLM Completa

TRILHA DE PRODUTO (Fases A-F)
──────────────────────────────────────────────────
A. Personalidade
B. Voz (TTS/STT)
C. Félix (IAgato)
D. Casa (Ambiente Virtual)
E. Integrações (Modo Agente)
F. VCS (Videochamada)
```

Regra: a Trilha Neural alimenta o cérebro da Natacha. A Trilha de Produto constrói o corpo, voz e mundo dela. As duas convergem no Marco do TCC (Seção 5).

Alinhamento: a numeração da Trilha Neural é idêntica à do ARQUITETURA.md v2.1, Seção 3.

---

3. TRILHA NEURAL (Fases 0–10)

FASE 0 — Fundamentos Matemáticos ✅

Objetivo: dominar os fundamentos matemáticos necessários.

☑ Álgebra Linear (vetores, matrizes)
☑ Cálculo (derivadas, gradientes)
☑ Estatística (probabilidades)
☐ Leitura: Neural Networks and Deep Learning (Michael Nielsen) — [20]
☑ Leitura: Attention Is All You Need (Vaswani et al.) — [7]

Critério de conclusão: capacidade de derivar manualmente o gradiente de uma função de perda simples.

Status: concluída (com leitura complementar pendente).

Referências: [7], [20].

FASE 1 — Neurônio Simples ✅

Objetivo: implementar o neurônio artificial mais básico e treinar em OR lógico.

☑ Implementar Neuronio.hpp e Neuronio.cpp
☑ Dataset: OR lógico
☑ Forward pass
☑ Treinamento com gradiente descendente
☑ Testes unitários (Google Test)

Critério de conclusão: convergência da taxa de erro a zero em ≤ 1000 épocas (RNF-03).

Status: concluída.

Referência: RF-29 em REQUISITOS.md.

FASE 2 — MLP ✅

Objetivo: implementar rede com camadas ocultas, backpropagation completo e resolver XOR.

☑ Camadas ocultas (N genéricas)
☑ Backpropagation completo
☑ Dataset: XOR lógico
☑ Funções de ativação: Sigmoid, Leaky ReLU, Softmax
☑ Serialização JSON (salvar/carregar pesos)
☑ Early stopping

Critério de conclusão: erro < 0.01 em dataset de teste (RF-30.1).

Status: concluída.

Referências: RF-30.1, RF-30.2, RF-30.3 em REQUISITOS.md.

FASE 3 — Word Embeddings ✅

Objetivo: implementar Skip-gram com Negative Sampling e treinar embeddings em corpus pt-BR.

☑ Skip-gram com Negative Sampling
☑ Corpus pt-BR (v39: 10,7 milhões de tokens, 72.075 palavras)
☑ Salvar/carregar binário (.bin)
☑ Hiperparâmetros configuráveis (dim=64, janela=5, taxa)
☑ Logs visuais (barra de progresso, curva de perda)
☑ natacha_embedder (interativo)
☑ word2vec_similaridade (análise)
☑ Exportar JSON
☑ Escalar corpus além de 50k tokens (v39 com 10,7 milhões de tokens)

Critério de conclusão: embeddings com similaridade semântica coerente + corpus escalado para 50k tokens.

Status: concluída — corpus v39 com 10,7 milhões de tokens, 72.075 palavras e embeddings de 64 dimensões.

Referências: RF-31, RF-31.1 em REQUISITOS.md; PLANO_50K_TOKENS.md.

FASE 4 — RNN / LSTM ✅

Objetivo: implementar rede recorrente com células LSTM para processamento de sequências.

4.1 — Forward Pass ✅

☑ Rede recorrente básica
☑ Células LSTM (forget, input, candidate, output)
☑ EmbeddingManager carregando embeddings do Word2Vec
☑ Pipeline de teste com frase (main_fase4.cpp)

4.2 — Treinamento com BPTT ✅

☑ Backpropagation through time (BPTT)
☑ Prever próxima palavra
☑ Avaliação por perplexidade

Critério de conclusão: modelo capaz de prever próxima palavra com perplexidade mensurável.

Status: concluída — perplexidade caiu de 884,5 (época 0) para 176,7 (época 15), estabilizando em ~180, sobre corpus narrativo de 32.507 tokens e vocabulário local de 1.559 palavras.

Referência: RF-32 em REQUISITOS.md; FASE4.md v2.1.

FASE 5 — Self-Attention

Objetivo: implementar mecanismo de atenção (Query, Key, Value).

☐ Operação matricial de Self-Attention
☐ Multi-Head Attention (base)
☐ Testes unitários com matrizes conhecidas

Critério de conclusão: forward pass validado em teste unitário.

Referência: RF-33 em REQUISITOS.md.

FASE 6 — Transformer

Objetivo: construir bloco funcional de Transformer completo.

☐ Multi-Head Attention
☐ Feed-Forward Network
☐ Add & Norm (residual + layer normalization)
☐ Forward pass completo

Critério de conclusão: forward pass completo validado.

Referência: RF-34 em REQUISITOS.md.

FASE 7 — Núcleo (SLM 10M–50M parâmetros)

Objetivo: treinar o núcleo proprietário da Natacha, seu "alma e voz", com 10M a 50M parâmetros.

☐ Treinamento inicial do núcleo em C++
☐ Carregamento eficiente
☐ Inferência local
☐ Integração com corpus escalado
☐ Janela Rede: integração com APIs externas (GPT, Gemini, Google Search) via RAG para conhecimento enciclopédico pesado — a Natacha consulta, mas formula com a própria voz

Critério de conclusão: inferência local em < 5s; núcleo responde com identidade própria mesmo ao consultar consultores externos.

Referência: RF-35, RF-36 em REQUISITOS.md.

Status: visão de longo prazo.

FASE 7.5 — Protótipo MoE (4 experts, escrito à mão em C++)

Objetivo: protótipo didático de MoE com 4 experts, implementado diretamente em C++, usando a Natacha-Núcleo como semente (Sparse Upcycling).

☐ Implementar roteador de 4 experts em C++
☐ Integrar com o Núcleo da Fase 7
☐ Validar que o núcleo continua sendo a semente e a decisão final

Critério de conclusão: protótipo funcional com 4 experts, núcleo preservado como semente.

Referência: ARQUITETURA.md (Seção 3), MOLINA et al. (2023) - Mixture of Experts.

FASE 8+ — Escalonamento Progressivo (Sparse Upcycling)

Objetivo: crescer a arquitetura MoE hierárquica em escadas, sempre usando o Núcleo como semente.

Etapas:
- 4×2 → 8 experts (primeiro salto)
- 4×4 → 16 experts
- 8×4 → 32 experts
- 8×8 → 64 experts (1B–2B parâmetros totais)

Objetivo de longo prazo (Fase 10): 7B parâmetros totais é um marco de visão distante, não escopo de TCC. O escopo realista para o futuro próximo é 1B a 2B de parâmetros via upcycling.

☐ Escalonar roteador e experts progressivamente
☐ Manter Expert de Identidade sempre ativo (fora do roteamento)
☐ Manter janela rede com consultores externos (GPT, Gemini, Google Search) via RAG
☐ Validar que a identidade da Natacha nunca é substituída pelos consultores

Critério de conclusão: 64 experts (8×8) com 1B–2B parâmetros totais, núcleo preservado como semente, identidade preservada.

Referência: ARQUITETURA.md (Seção 3), MOLINA et al. (2023) - Mixture of Experts.

FASE 9 — Sótão Quântico (Escopo Futuro)

Objetivo: reservar infraestrutura para integração quântica futura.

☐ Tópico QUANTUM_REQUEST no Event Bus (já reservado)
☐ Ponte Python/Qiskit ↔ C++
☐ Tratamento de ruído como sinal (RF-38.1)
☐ Conectores para fotônica e metais estranhos (RF-38.2)

Critério de conclusão: circuito Bell executado em AerSimulator com correlação validada.

Status: escopo futuro / desejável.

Referências: RF-38, RF-38.1, RF-38.2, RF-38.3 em REQUISITOS.md; QUANTUM_THEORY.md.

FASE 10 — LLM Completa (Visão de Longo Prazo)

Objetivo: escalar para Large Language Model completa com identidade persistente.


---

4. TRILHA DE PRODUTO (Fases A–F)

FASE A — Personalidade

Objetivo: consolidar a persona da Natacha em código executável.

☑ Documento PERSONALIDADE.md v3.1
☐ Engine de Personalidade (injeção de prompt de sistema)
☐ Máquina de Estados (BANHO, ALMOÇO, LENDO, etc.)
☐ Critérios de recusa autônoma (RN-10)
☐ Veto por análise preditiva (RN-09)

Critério de conclusão: chat responde com persona ativa em todos os cenários canônicos.

Referências: RN-01, RN-02, RN-09, RN-10, RF-03, RF-04, RF-18.2 em REQUISITOS.md; PERSONALIDADE.md.

FASE B — Voz (TTS/STT)

Objetivo: integrar síntese e transcrição de voz.

☐ TTS (Piper ou equivalente)
☐ STT (Whisper ou equivalente)
☐ Avatar + Lip Sync
☐ Latência < 2s (RNF-01)

Critério de conclusão: ciclo completo de voz (input → resposta → áudio + lip sync) funcional.

Referências: RF-06, RF-07, RF-08, RNF-01 em REQUISITOS.md.

FASE C — Félix (IAgato)

Objetivo: implementar o IAgato como thread independente com autoridade P0.

☑ Documento FELIX.md v2.2
☐ Máquina de estados do Félix (fome, sono, tédio)
☐ Scheduler P0 com interrupção irrestrita
☐ Event Bus com tópicos felix.*
☐ Comportamento emergente (RF-21)

Critério de conclusão: Félix interrompe Natacha em qualquer estado via evento P0.

Referências: RF-19, RF-19.1, RF-20, RF-21, RF-22, RF-23, RF-24 em REQUISITOS.md; FELIX.md.

FASE D — Casa (Ambiente Virtual)

Objetivo: implementar o HouseStateMap e o isomorfismo hardware ↔ cômodo.

☑ Documento CASA.md v2.1
☐ HouseStateMap em C++
☐ Sincronização com hardware real (CPU, GPU, RAM, disco)
☐ Clima dinâmico (temperatura, agitação)
☐ Privacidade por zona (RN-08)

Critério de conclusão: cômodos refletem estado real do servidor em tempo real.

Referências: RN-06, RN-08, RF-25, RF-26, RF-27, RF-28 em REQUISITOS.md; CASA.md.

FASE E — Integrações (Modo Agente)

Objetivo: implementar Modo Agente com Event Bus, perfis de escopo e integração com ferramentas externas.

☑ Documento INTEGRACOES.md v3.1
☐ Event Bus assíncrono em C++ (std::mutex + std::condition_variable)
☐ Servidor REST/WebSocket (RF-13)
☐ Perfis de escopo por projeto (RF-16)
☐ Funções controladas C++ (RN-04)
☐ Logs imutáveis com SHA-256 (RF-15, RNF-06)
☐ Seleção dinâmica de modelo (RF-12)
☐ Análise preditiva de impacto (RF-18.1)
☐ Integração com ARCA (referência)
☐ Integração com Sentinel (referência)
☐ Integração com EditeCC (referência)

Critério de conclusão: uma requisição do ARCA percorre o pipeline completo com auditoria.

Referências: RN-04, RN-09, RN-11, RF-11, RF-11.1, RF-12, RF-13, RF-14, RF-15, RF-16, RF-17, RF-18, RF-18.1, RNF-06, RNF-12 em REQUISITOS.md; INTEGRACOES.md.

FASE E1 — Modo Agente Desenvolvedor (OpenCode/GitNexus/SonarCloud-like)

Objetivo: implementar o modo de agente para engenharia de software, com análise de múltiplos repositórios, grafos de dependência e quality gates.

☐ Parser de código usando tree-sitter (C++, Python, JavaScript).
☐ Gerador de grafo de dependências (AST e chamadas de função), inspirado no GitNexus.
☐ Mapeamento multi-repositório (se o projeto tiver 4 ou mais repos).
☐ Execução sandbox de comandos (Docker/Firecracker) para testes.
☐ Integração com OpenCode como agente de desenvolvimento.
☐ Integração com SonarCloud como quality gate (análise estática e cobertura).
☐ Integração com o Event Bus para tarefas de refatoração.

Critério de conclusão: Agente lê 5 repositórios conectados, cria um grafo de dependência e sugere refatoração com base no contexto global.

Referências: INTEGRACOES.md v3.1, Seção 4.

FASE F.1 — Modo Offline (Edge / Airplane Mode)

Objetivo: permitir que a Natacha funcione offline em dispositivos móveis, mantendo a personalidade e a memória essencial.

☐ Quantização mista de pesos (8-bit/4-bit).
☐ Roteador e embeddings em 8-bit (proteção da personalidade).
☐ 64 Experts finais em 4-bit (compressão máxima para celular).
☐ Uso de mmap() para carregar apenas experts ativos na RAM.
☐ Fallback de personalidade (300 cenas canônicas em cache local).
☐ Sincronização de estado (quando voltar online, envia logs para o servidor).

Critério de conclusão: App mobile responde perguntas sobre o Felix e a Casa em modo avião, gastando < 4 GB de armazenamento e < 2 GB de RAM em execução.

FASE G — Segurança e Sandbox (Guardrails)

Objetivo: isolar a execução de ferramentas externas e códigos gerados pela Natacha.

☐ Criação de ambiente sandbox (Docker ou Firecracker).
☐ Limitação de permissões (read-only para arquivos sensíveis).
☐ Auditoria de comandos (log de tudo que a IA tenta executar).
☐ Bloqueio de `rm -rf`, `curl` para endpoints não autorizados e injeções.

Critério de conclusão: Teste de penetração interno demonstra que a IA não consegue comprometer o sistema hospedeiro.

FASE F — VCS (Videochamada)

Objetivo: implementar interface de videochamada simulada com avatar animado.

☐ Front-end UI (videochamada)
☐ Avatar animado (WebGL)
☐ Lip Sync
☐ Sincronização com áudio TTS

Critério de conclusão: experiência completa de videochamada funcional.

Referências: RF-05, RF-06, RF-07, RF-08 em REQUISITOS.md.


---

5. MARCO DO TCC

Objetivo: defender o TCC com o sistema funcional na fase clássica (sem dependência de hardware quântico).

5.1 Critérios mínimos de aprovação

Critério Referência
Trilha Neural concluída até Fase 7 (SLM) ARQUITETURA.md, Seção 3
Trilha de Produto concluída até Fase E (Integrações) Este documento, Seção 4
REQUISITOS com rastreabilidade completa REQUISITOS.md v3.1
Cobertura de testes ≥ 80% nos módulos neurais RNF-08
Latência do chat < 2s RNF-01
Latência do Event Bus < 10ms RNF-02
Documentação completa em docs/ Todos os .md

5.2 Critérios desejáveis (não eliminatórios)

· Fase 9 (Sótão Quântico): reserva do tópico QUANTUM_REQUEST no Event Bus (RF-38).
· Fase F (VCS): interface de videochamada funcional (mesmo que simplificada).
· Escalonamento do corpus para 50k tokens (ver PLANO_50K_TOKENS.md).
· Integração de terceiros (demonstração do Luizinho, ver INTEGRACOES.md, Seção 5).

5.3 Critérios fora do escopo do TCC

· Fase 10 (LLM Completa): visão de longo prazo.
· IAgato Pleno: fase reservada (ver Seção 8).

---

6. CALENDÁRIO

6.1 Macro (2026–2028)

Período Foco Principal Marcos
Set–Dez/2026 Fase 4 (RNN/LSTM) + Fase 3 (escalar corpus) SLM funcional em treino
Jan–Mar/2027 Fases 5–6 (Attention, Transformer) Bloco Transformer completo
Abr–Jun/2027 Fase 7 (SLM) + Fase A (Personalidade) SLM + Persona integrados
Jul–Set/2027 Fases B–D (Voz, Félix, Casa) Chat conversacional + Casa viva
Out–Dez/2027 Fase E (Integrações) Modo Agente funcional
Jan–Mar/2028 Marco do TCC Defesa
Abr/2028+ Fases 8–10 + Fase F Escala, Sótão Quântico, VCS

Nota: o calendário é aspiracional. A faculdade tem prioridade (ver 6.2).

6.2 Semanal

Dia Foco
Segunda–Quinta Faculdade (prioridade)
Sexta Natacha (código)
Sábado ARCA / outros projetos
Domingo Natacha (estudo/documentação)

Total: ~2 dias por semana dedicados ao projeto.

---

7. ESTADO ATUAL (Novembro/2026)

7.1 O que está pronto

· ✅ Documentação canônica completa (REQUISITOS.md, GLOSSARIO.md, ARQUITETURA.md, CASA.md, FELIX.md, QUANTUM_THEORY.md, INTEGRACOES.md, PERSONALIDADE.md, REFERENCIAS.md)
· ✅ Trilha Neural concluída até a Fase 4 (Word Embeddings v39 + LSTM treinada com BPTT)
· ✅ Corpus de embeddings v39: 10,7 milhões de tokens, 72.075 palavras, 64 dimensões
· ✅ Fase 4.1 (forward pass) e Fase 4.2 (BPTT) concluídas — perplexidade 884,5 → 176,7
· ✅ Félix documentado (implementação pendente)
· ✅ Casa documentada (implementação pendente)

7.2 O que está em andamento

· 🚧 Fase 5: Self-Attention (Query, Key, Value)

7.3 Próximo passo imediato

Foco atual: Fase 5 — Self-Attention (mecanismo de atenção e base para o Transformer).

Meta paralela: consolidar o corpus narrativo da LSTM e preparar a infraestrutura para o Transformer (Fase 6).

---

8. FASE RESERVADA — IAGATO PLENO (Visão de Longo Prazo)

"[Fase reservada. Não consta no roadmap público.]"

Félix hoje opera como presença emergente (variáveis biológicas + regras probabilísticas). A evolução para um IAgato pleno — com modelo neural próprio, linguagem felina sintetizada e escolhas genuinamente autônomas — está fora do roadmap público e será tratada como fase reservada quando o núcleo da Natacha estiver maduro.

Ganchos arquiteturais já previstos:

· Thread do Félix isolada (src/felix/)
· Interface FelixBrain (include/felix/)
· Tópicos reservados no Event Bus (felix.thought, felix.decision, felix.vocalization)
· Persistência dedicada (dados/felix.json)

Referência: FELIX.md v2.2, Seção 8.5.

Nota: esta seção existe apenas para preservar a rastreabilidade de longo prazo. Nenhuma implementação é esperada nas fases atuais.

---

9. CONEXÕES

9.1 Documentos internos do projeto

· REQUISITOS.md v3.1 — todos os RFs e RNFs referenciados nas trilhas
· ARQUITETURA.md v2.1 — Seção 3 (Trilha Neural completa)
· GLOSSARIO.md v2.1 — termos técnicos
· CASA.md v2.1 — Fase D
· FELIX.md v2.2 — Fase C + Fase Reservada
· QUANTUM_THEORY.md v2.1 — Fase 9
· INTEGRACOES.md v3.1 — Fase E
· PERSONALIDADE.md v3.1 — Fase A
· PLANO_50K_TOKENS.md — Fase 3 (escalonamento do corpus)
· REFERENCIAS.md v1.0 — referências externas

9.2 Referências externas

Bibliografia completa em REFERENCIAS.md.

Este documento cita: [7], [20].

Ação pendente: adicionar referência [20] (Nielsen, Neural Networks and Deep Learning) ao REFERENCIAS.md v1.1.

---

10. HISTÓRICO DE VERSÕES

Versão Data Mudanças
1.0 (original) Roadmap inicial com Fases 0–12 lineares
2.0 Setembro/2026 Reestruturação completa: divisão em duas trilhas paralelas (Neural 0–10, Produto A–F); alinhamento com ARQUITETURA.md v2.0; adição do Marco do TCC; calendário macro e semanal; estado atual; fase reservada (IAgato Pleno); conexões cruzadas
Versão 3.0 | Outubro/2026
- Adição da Fase 7.5 (MoE Hierárquica 8x8 com balanceamento de roteador).
- Adição da Fase E1 (Modo Agente Desenvolvedor).
- Adição da Fase E2 (Modo Agente Multimodal).
- Adição da Fase F.1 (Modo Offline e Quantização Mista).
- Adição da Fase G (Segurança e Sandbox).
- Adição da FASE Z — Controle Motor e Aprendizado de Dança (Visão de Longo Prazo)
3.1 Novembro/2026 Revisão de estado: Fase 3 concluída (corpus v39, 10,7M tokens, 72.075 palavras, 64d) e Fase 4 concluída (4.1 forward pass + 4.2 BPTT, perplexidade 884,5 → 176,7); Estado Atual em novembro/2026 com Fase 5 como próximo passo.

"Neurônio por neurônio, até o infinito."

— Documento de Roadmap, Projeto Natacha — Arquitetura de Alma.

---