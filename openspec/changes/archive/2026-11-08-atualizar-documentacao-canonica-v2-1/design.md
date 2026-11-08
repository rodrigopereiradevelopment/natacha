# Design

## Context

Ver `proposal.md — Why`. O conjunto canônico está em `docs/` e mistura documentos v2.0, v2.1 e v3.0, todos datados de setembro/2026. A implementação já avançou: a Fase 4.1 (forward pass) e a Fase 4.2 (BPTT) estão concluídas, o corpus de embeddings chegou à v39 e o README já registra a perplexidade de 176,7. Os documentos canônicos, porém, ainda dizem que a Fase 4 é "planejada" ou "prioridade atual", e a visão de longo prazo aparece de forma fragmentada.

Restrições:

- Nenhum código, corpus, peso ou vetor será alterado.
- O conteúdo conceitual (persona, Casa, Félix, requisitos) deve ser preservado; a revisão é de estado, versão e visão de longo prazo.
- Os documentos se referenciam mutuamente por versão, então a atualização precisa ser coerente em todo o conjunto.

## Goals / Non-Goals

**Goals:**

- Fazer os nove documentos canônicos contarem a mesma história sobre o estado atual: Fase 4 concluída, corpus v39 e próximo passo na Fase 5.
- Registrar a visão de longo prazo de forma explícita e consistente: MoE hierárquica 8x8, multimodalidade, modo agente, mobile/offline e Expert de Identidade sempre ativo.
- Versionar cada documento com seu próximo minor e manter o changelog de v2.0/setembro de 2026.
- Manter as referências cruzadas apontando para as versões corretas.

**Non-Goals:**

- Implementar MoE, multimodalidade, modo agente ou quantização.
- Criar novos requisitos de sistema além dos já previstos na visão de longo prazo.
- Alterar o README além de eventual correção de referência cruzada.
- Reescrever seções conceituais que não dependem do estado atual.

## Decisions

### 1. Fonte única de verdade para o estado atual

Criar um bloco canônico de métricas, repetido apenas onde o documento precisar dele:

```text
Fase 4.1 — LSTM Forward Pass: concluída
Fase 4.2 — LSTM com BPTT: concluída
Perplexidade: 884,5 (época 0) → 176,7 (época 15), estável em ~180
Corpus de embeddings: v39, 10,7 milhões de tokens, 72.075 palavras, 64 dimensões
Corpus narrativo da LSTM: 32.507 tokens, vocabulário local de 1.559 palavras
```

**Por quê:** hoje os números aparecem em formatos diferentes no README, no ROADMAP e no ARQUITETURA. Uma fonte única evita que a próxima atualização deixe um documento para trás.

**Alternativa considerada:** criar um `docs/ESTADO_ATUAL.md` e referenciá-lo. Rejeitada porque aumenta o número de documentos canônicos e o usuário pediu atualização do conjunto existente, não criação de um novo.

### 2. Versionamento por próximo minor, não um único número global

- `ROADMAP.md`: 2.0 → 3.1 (o changelog já registrava uma entrada 3.0/outubro de 2026; o cabeçalho estava desatualizado)
- `ARQUITETURA.md`: 2.0 → 2.1
- `GLOSSARIO.md`: 2.0 → 2.1
- `CASA.md`: 2.0 → 2.1
- `QUANTUM_THEORY.md`: sem cabeçalho de versão → passa a declarar 2.1
- `FASE4.md`: sem cabeçalho de versão → passa a declarar 2.1
- `FELIX.md`: 2.1 → 2.2
- `REQUISITOS.md`: 3.0 → 3.1
- `PERSONALIDADE.md`: 3.0 → 3.1
- `INTEGRACOES.md`: 3.0 → 3.1

**Por quê:** os documentos não estavam todos na mesma versão. Forçar todos para 2.1 rebaixaria REQUISITOS, PERSONALIDADE e INTEGRACOES, que já estão em 3.0. O próximo minor preserva a história de cada documento e mantém a revisão identificável como "geração v2.1 da documentação".

**Alternativa considerada:** congelar todos em v2.1. Rejeitada por ser um downgrade semântico.

### 3. Expert de Identidade sempre ativo

Descrever o Expert de Identidade como um expert permanente da MoE hierárquica, fora do roteamento dos 64 experts:

- recebe todos os tokens (ou todos os passos de inferência), sem depender do roteador;
- aplica os invariantes de persona e de identidade persistente (RN-05);
- tem orçamento de compute pequeno e previsível, para não inviabilizar o custo de ~1,5B ativos;
- é o guardião do "eu sou eu em qualquer hardware", complementando o JSON de identidade.

**Por quê:** a identidade não pode ser uma rota esporádica; se o roteador não a selecionar, a persona se degrada. O Expert sempre ativo transforma a RN-05 em arquitetura neural, não só em persistência.

**Alternativa considerada:** manter a identidade apenas no JSON externo. Rejeitada porque o pedido exige um Expert de Identidade sempre ativo e porque o JSON sozinho não influencia a geração token a token.

### 4. Visão de longo prazo consolidada no ROADMAP e no ARQUITETURA

- **MoE hierárquica 8x8**: 8 macro-experts, 8 sub-experts por macro (64 no total), 7B totais e ~1,5B ativos. A Fase 7.5 já existe; será atualizada e ligada ao Expert de Identidade.
- **Multimodalidade**: visão, geração de imagem, vídeo e slides. A Fase E2 já cobre imagem/vídeo/slides; será explicitada a entrada de visão.
- **Modo agente**: Manus como referência multimodal, OpenCode como agente de código, GitNexus como grafo de código e SonarCloud como quality gate. E1/E2 e INTEGRACOES serão alinhados.
- **Mobile/offline**: quantização mista 8-bit/4-bit, com roteador e embeddings em maior precisão e experts em 4-bit; `mmap()` para carregar só experts ativos. A Fase F.1 já existe e será atualizada para a nomenclatura 8/4-bit.

**Por quê:** a visão já está parcialmente escrita; o problema é dispersão e nomenclatura inconsistente. Consolidar evita duplicar fases e facilita a defesa no TCC.

### 5. Changelog e referências cruzadas

Cada documento com seção de histórico ganha uma linha v2.1 (ou próximo minor) com data de novembro/2026 e um resumo curto. Documentos sem seção de histórico (ARQUITETURA, GLOSSARIO, CASA, QUANTUM_THEORY) recebem o cabeçalho de versão e, quando já houver seção equivalente, a entrada correspondente. As referências cruzadas passam a citar as versões novas.

**Por quê:** o TCC precisa de rastreabilidade. Sem changelog, a revisão vira uma edição invisível.

## Risks / Trade-offs

- **Referências cruzadas desatualizadas** → rodar uma auditoria final com `grep` por `v2.0`, `v2.1`, `v3.0` e pelos nomes dos documentos, corrigindo apenas o conjunto canônico.
- **Métrica duplicada divergir** → usar o bloco canônico da Decisão 1 e conferir os nove documentos antes de encerrar.
- **Documento já em versão superior ser rebaixado** → aplicar o próximo minor por documento, conforme a Decisão 2.
- **Visão de longo prazo parecer promessa de implementação** → manter linguagem de "visão futura/planejada" e reforçar que nada foi implementado além da Fase 4.2.
- **Escopo crescer para outros documentos** (PLANO_50K_TOKENS, README, PLANO_500K) → tratar apenas referências cruzadas quebradas; não reescrever conteúdo fora dos nove.

## Migration Plan

1. Atualizar os documentos na ordem: ROADMAP → ARQUITETURA → REQUISITOS → GLOSSARIO → INTEGRACOES → CASA → FELIX → PERSONALIDADE → QUANTUM_THEORY.
2. Rodar a auditoria de versões e referências cruzadas.
3. Revisar o diff completo.
4. Rollback: `git revert` do commit documental; nenhum artefato de código ou dados é afetado.

## Open Questions

Nenhuma pendente. A decisão de escopo (nove documentos) e de versionamento (próximo minor) foi confirmada pelo usuário.
