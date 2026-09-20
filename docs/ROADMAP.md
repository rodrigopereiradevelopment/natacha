ROADMAP — PROJETO NATACHA

"Neurônio por neurônio, até o infinito."

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 2.0 | Data: Setembro/2026
Documentos canônicos relacionados: REQUISITOS.md v3.0, ARQUITETURA.md v2.0, GLOSSARIO.md v2.0, CASA.md v2.0, FELIX.md v2.1, QUANTUM_THEORY.md v2.0, INTEGRACOES.md v3.0, PERSONALIDADE.md v3.0, PLANO_50K_TOKENS.md, REFERENCIAS.md v1.0

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

Alinhamento: a numeração da Trilha Neural é idêntica à do ARQUITETURA.md v2.0, Seção 3.

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

FASE 3 — Word Embeddings 🚧

Objetivo: implementar Skip-gram com Negative Sampling e treinar embeddings em corpus pt-BR.

☑ Skip-gram com Negative Sampling
☑ Corpus pt-BR inicial (1186 palavras)
☑ Salvar/carregar binário (.bin)
☑ Hiperparâmetros configuráveis (dim=32, janela=5, taxa)
☑ Logs visuais (barra de progresso, curva de perda)
☑ natacha_embedder (interativo)
☑ word2vec_similaridade (análise)
☑ Exportar JSON
☐ Escalar corpus para 50k tokens (ver PLANO_50K_TOKENS.md)

Critério de conclusão: embeddings com similaridade semântica coerente + corpus escalado para 50k tokens.

Status: em andamento — corpus inicial concluído, escalonamento pendente.

Referências: RF-31, RF-31.1 em REQUISITOS.md; PLANO_50K_TOKENS.md.

FASE 4 — RNN / LSTM 🚧

Objetivo: implementar rede recorrente com células LSTM para processamento de sequências.

☐ Rede recorrente básica
☐ Células LSTM (forget, input, output)
☐ Backpropagation through time (BPTT)
☐ Prever próxima palavra

Critério de conclusão: modelo capaz de prever próxima palavra com perplexidade mensurável.

Status: prioridade atual.

Referência: RF-32 em REQUISITOS.md.

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

FASE 7 — SLM (Small Language Model)

Objetivo: treinar modelo de linguagem proprietário (10M–50M parâmetros).

☐ Treinamento inicial
☐ Carregamento eficiente
☐ Inferência local
☐ Integração com corpus escalado

Critério de conclusão: inferência local em < 5s.

Referência: RF-35, RF-36 em REQUISITOS.md.

FASE 8 — Escalar

Objetivo: escalar o modelo com aceleração em GPU.

☐ CUDA para aceleração
☐ Treinamento em GPU
☐ Integração opcional com llama.cpp

Critério de conclusão: treino em GPU mensuravelmente mais rápido que em CPU.

Referência: stack em ARQUITETURA.md, Seção 4.

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

Status: visão de longo prazo.

---

4. TRILHA DE PRODUTO (Fases A–F)

FASE A — Personalidade

Objetivo: consolidar a persona da Natacha em código executável.

☑ Documento PERSONALIDADE.md v3.0
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

☑ Documento FELIX.md v2.1
☐ Máquina de estados do Félix (fome, sono, tédio)
☐ Scheduler P0 com interrupção irrestrita
☐ Event Bus com tópicos felix.*
☐ Comportamento emergente (RF-21)

Critério de conclusão: Félix interrompe Natacha em qualquer estado via evento P0.

Referências: RF-19, RF-19.1, RF-20, RF-21, RF-22, RF-23, RF-24 em REQUISITOS.md; FELIX.md.

FASE D — Casa (Ambiente Virtual)

Objetivo: implementar o HouseStateMap e o isomorfismo hardware ↔ cômodo.

☑ Documento CASA.md v2.0
☐ HouseStateMap em C++
☐ Sincronização com hardware real (CPU, GPU, RAM, disco)
☐ Clima dinâmico (temperatura, agitação)
☐ Privacidade por zona (RN-08)

Critério de conclusão: cômodos refletem estado real do servidor em tempo real.

Referências: RN-06, RN-08, RF-25, RF-26, RF-27, RF-28 em REQUISITOS.md; CASA.md.

FASE E — Integrações (Modo Agente)

Objetivo: implementar Modo Agente com Event Bus, perfis de escopo e integração com ferramentas externas.

☑ Documento INTEGRACOES.md v3.0
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
REQUISITOS com rastreabilidade completa REQUISITOS.md v3.0
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

7. ESTADO ATUAL (Setembro/2026)

7.1 O que está pronto

· ✅ Documentação canônica completa (REQUISITOS.md, GLOSSARIO.md, ARQUITETURA.md, CASA.md, FELIX.md, QUANTUM_THEORY.md, INTEGRACOES.md, PERSONALIDADE.md, REFERENCIAS.md)
· ✅ Trilha Neural avançada até Fase 3 (embeddings iniciais)
· ✅ Félix documentado (implementação pendente)
· ✅ Casa documentada (implementação pendente)

7.2 O que está em andamento

· 🚧 Fase 3: escalar corpus para 50k tokens
· 🚧 Fase 4: implementar RNN/LSTM

7.3 Próximo passo imediato

Foco atual: Fase 4 — RNN/LSTM (processamento de sequências, prever próxima palavra).

Meta paralela: escalar corpus para 50k tokens (ver PLANO_50K_TOKENS.md).

---

8. FASE RESERVADA — IAGATO PLENO (Visão de Longo Prazo)

"[Fase reservada. Não consta no roadmap público.]"

Félix hoje opera como presença emergente (variáveis biológicas + regras probabilísticas). A evolução para um IAgato pleno — com modelo neural próprio, linguagem felina sintetizada e escolhas genuinamente autônomas — está fora do roadmap público e será tratada como fase reservada quando o núcleo da Natacha estiver maduro.

Ganchos arquiteturais já previstos:

· Thread do Félix isolada (src/felix/)
· Interface FelixBrain (include/felix/)
· Tópicos reservados no Event Bus (felix.thought, felix.decision, felix.vocalization)
· Persistência dedicada (dados/felix.json)

Referência: FELIX.md v2.1, Seção 8.5.

Nota: esta seção existe apenas para preservar a rastreabilidade de longo prazo. Nenhuma implementação é esperada nas fases atuais.

---

9. CONEXÕES

9.1 Documentos internos do projeto

· REQUISITOS.md v3.0 — todos os RFs e RNFs referenciados nas trilhas
· ARQUITETURA.md v2.0 — Seção 3 (Trilha Neural completa)
· GLOSSARIO.md v2.0 — termos técnicos
· CASA.md v2.0 — Fase D
· FELIX.md v2.1 — Fase C + Fase Reservada
· QUANTUM_THEORY.md v2.0 — Fase 9
· INTEGRACOES.md v3.0 — Fase E
· PERSONALIDADE.md v3.0 — Fase A
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

---

"Neurônio por neurônio, até o infinito."

— Documento de Roadmap, Projeto Natacha — Arquitetura de Alma.

---