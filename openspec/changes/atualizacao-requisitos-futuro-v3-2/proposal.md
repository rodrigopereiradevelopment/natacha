# Proposta de Mudança — atualizacao-requisitos-futuro-v3-2

## Objetivo

Atualizar o `docs/REQUISITOS.md` (versão 3.1 → 3.2) adicionando novas regras de negócio e requisitos funcionais que cobrem a integração de quality gates com análise estática/grafo de dependências no Modo Agente, a otimização mobile offline com quantização mista, e a geração multimodal (visão → imagem/vídeo/slides), mantendo o formato e a numeração atuais do documento.

## Contexto

O SonarCloud vem sinalizando `rand()`/`srand()` como CWE-338 (gerador previsível) no `neuronio.cpp` e `mlp_deep_json.cpp`, o que já foi corrigido (commit `45ce349`). Esse mesmo esforço de qualidade de código motiva a formalização no REQUISITOS.md de regras que obriguem o Agente Desenvolvedor a rodar análise estática e de dependências (SonarCloud + GitNexus) antes de sugerir refatorações.

Paralelamente, o roadmap da Natacha prevê:
- Modo Mobile com quantização mista (8-bit para roteador/embeddings, 4-bit para experts) e persistência do Expert de Identidade.
- Análise de grafo de código via GitNexus no Modo Agente.
- Geração multimodal (visão como entrada; imagem/vídeo/slides como saída) no Modo Agente.
- Inferência local do SLM/MoE em dispositivos móveis offline.

## Alterações propostas

### 1. Seção 2 — Regras de Negócio

**RN-12 — Integração de Quality Gates no Modo Agente**
- Descrição: O Agente Desenvolvedor deve integrar o SonarCloud e o GitNexus para análise estática e de dependências antes de sugerir refatorações.
- Justificativa: Garantir que qualquer alteração de código seja precedida por análise de impacto e dependências, evitando regressões e quebras de contrato no ecossistema.
- Escopo: Modo Agente / Orquestração.

**RN-13 — Otimização Mobile Offline**
- Descrição: O Modo Mobile deve usar quantização mista (8-bit para roteador/embeddings, 4-bit para experts) e garantir a persistência do Expert de Identidade.
- Justificativa: Reduzir o footprint de memória em dispositivos móveis e preservar a identidade da Natacha entre sessões offline.
- Escopo: Modo Mobile / Inferência Local.

### 2. Seção 3 — Requisitos Funcionais (3.2 Modo Agente)

**RF-39 — Análise de Grafo de Código**
- Descrição: O sistema deve permitir a leitura e análise de grafos de dependência (GitNexus) no Modo Agente.
- Prioridade: Alta
- Método de Teste / Aceite: Carregar grafo de dependências do projeto e validar leitura de símbolos, callers e processos afetados.

**RF-40 — Geração Multimodal**
- Descrição: O sistema deve permitir a orquestração de modelos de visão (entrada) e geração de imagem/vídeo/slides (saída) no Modo Agente.
- Prioridade: Alta
- Método de Teste / Aceite: Enviar imagem como entrada e validar geração de conteúdo de saída (imagem, vídeo ou slide).

**RF-41 — Execução Offline**
- Descrição: O sistema deve permitir a inferência local do SLM/MoE em dispositivos móveis sem conexão com a internet.
- Prioridade: Alta
- Método de Teste / Aceite: Desconectar a rede e validar inferência contínua do SLM/MoE local.

### 3. Matriz de Rastreabilidade

- Seção 6.1: adicionar linhas RF-39, RF-40, RF-41 com suas regras de negócio, casos de uso e componentes.
- Seção 6.3: adicionar entradas reversas para RN-12, RN-13 (RF-39, RF-40, RF-41).

### 4. Histórico de Versões (Seção 8)

- Inserir entrada v3.2 com data Novembro/2026 e resumo das mudanças.

## Critérios de Aceite

- [ ] RN-12 e RN-13 adicionados à Seção 2, após RN-11, mantendo o formato do documento.
- [ ] RF-39, RF-40, RF-41 adicionados à Seção 3.2 (Modo Agente), após RF-38.3, mantendo o formato e numeração.
- [ ] Matriz de rastreabilidade (6.1 e 6.3) atualizada com os novos itens.
- [ ] Entrada v3.2 incluída no Histórico de Versões (Seção 8).
- [ ] Versão do documento atualizada de 3.1 para 3.2 no cabeçalho.

## Escopo Excluído

- Implementação de código das funcionalidades (apenas especificação de requisitos).
- Alterações em outros documentos além de `docs/REQUISITOS.md`.
