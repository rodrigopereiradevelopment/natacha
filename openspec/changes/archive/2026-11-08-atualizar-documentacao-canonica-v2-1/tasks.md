# Tasks

## 1. Preparação e fonte de verdade

- [x] 1.1 Conferir as métricas atuais no `README.md`, em `docs/FASE4.md` e nos artefatos do corpus; registrar o bloco canônico (Fase 4.1/4.2, perplexidade 884,5 → 176,7, corpus v39 com 10,7M tokens e 72.075 palavras, 64 dimensões, corpus narrativo com 32.507 tokens e 1.559 palavras). Verificação: os valores batem com o README e com o FASE4.md.
- [x] 1.2 Auditar cabeçalhos de versão e referências cruzadas nos nove documentos canônicos; listar ocorrências de `v2.0`, `v2.1`, `v3.0` e nomes de documentos que precisarão mudar. Verificação: lista gerada por `grep` e anexada ao resumo da execução.

## 2. ROADMAP

- [x] 2.1 Atualizar o cabeçalho do `docs/ROADMAP.md` para `Versão: 3.1 | Data: Novembro/2026` (o changelog já registrava 3.0) e ajustar a lista de documentos canônicos relacionados. Verificação: `sed -n '1,8p' docs/ROADMAP.md` mostra v3.1 e as versões novas.
- [x] 2.2 Marcar a Fase 3 como concluída (v39) e a Fase 4 como concluída, detalhando 4.1 (forward pass) e 4.2 (BPTT, perplexidade 884,5 → 176,7). Verificação: `grep -n 'FASE 4' docs/ROADMAP.md` e a seção não contém mais "prioridade atual" nem checkboxes pendentes da Fase 4.
- [x] 2.3 Atualizar a seção "Estado Atual" para novembro/2026, registrar o corpus v39 e definir a Fase 5 (Self-Attention) como próximo passo. Verificação: a seção cita v39, 10,7M tokens, 72.075 palavras, 64d e Fase 5.
- [x] 2.4 Consolidar a visão de longo prazo: MoE hierárquica 8x8 (7B totais, ~1,5B ativos), Expert de Identidade sempre ativo, multimodalidade (visão, imagem, vídeo, slides), modo agente (Manus, OpenCode, GitNexus, SonarCloud) e mobile/offline (quantização mista 8-bit/4-bit). Verificação: todos os itens aparecem nomeados no documento.
- [x] 2.5 Adicionar a entrada v3.1 ao histórico de versões do ROADMAP. Verificação: a tabela final contém a linha v3.1 com data e resumo.

## 3. ARQUITETURA

- [x] 3.1 Adicionar/atualizar o cabeçalho do `docs/ARQUITETURA.md` para v2.1 com data de novembro/2026. Verificação: o topo do arquivo declara a versão.
- [x] 3.2 Corrigir o status das Fases 2, 3 e 4: Fase 2 concluída, Fase 3 concluída com embeddings de 64 dimensões, Fase 4 concluída com LSTM + BPTT e perplexidade 176,7. Verificação: `grep -n 'Status:' docs/ARQUITETURA.md` não mostra Fase 4 como planejada e a dimensão 64 substitui a 32.
- [x] 3.3 Descrever a evolução de longo prazo para MoE hierárquica 8x8 e o Expert de Identidade sempre ativo na trilha neural. Verificação: a seção de evolução do modelo contém ambos os conceitos e explica o Expert fora do roteamento.
- [x] 3.4 Atualizar referências cruzadas para as versões novas e, se houver seção de histórico, adicionar a entrada v2.1. Verificação: `grep -n 'v2.0\\|v3.0' docs/ARQUITETURA.md` só aponta versões ainda válidas.

## 4. REQUISITOS

- [x] 4.1 Atualizar o cabeçalho do `docs/REQUISITOS.md` para v3.1 com data de novembro/2026 e revisar as referências cruzadas. Verificação: o topo declara v3.1.
- [x] 4.2 Revisar o RF-32 para refletir a Fase 4 concluída (forward pass e BPTT) e o critério de perplexidade mensurável atingido. Verificação: o RF-32 não descreve mais a fase como pendente.
- [x] 4.3 Alinhar a RN-05 (desacoplamento entre modelo e identidade) com o Expert de Identidade sempre ativo, explicitando que a identidade influencia a inferência e não apenas o JSON externo. Verificação: a RN-05 menciona o Expert e mantém o requisito de persistência.
- [x] 4.4 Adicionar a entrada v3.1 ao histórico de versões. Verificação: a tabela final contém a linha v3.1.

## 5. GLOSSÁRIO

- [x] 5.1 Atualizar o cabeçalho do `docs/GLOSSARIO.md` para v2.1 com data de novembro/2026. Verificação: o topo declara v2.1.
- [x] 5.2 Adicionar/atualizar termos: MoE hierárquica 8x8, Expert de Identidade, quantização mista 8-bit/4-bit, SonarCloud, GitNexus e Manus; revisar as entradas de LSTM, BPTT, embeddings e corpus v39. Verificação: `grep -n` encontra cada termo com definição.
- [x] 5.3 Conferir a organização por categorias e as referências ao REQUISITOS.md v3.1. Verificação: nenhum termo novo ficou fora de categoria e as referências apontam para v3.1.

## 6. INTEGRAÇÕES

- [x] 6.1 Atualizar o cabeçalho do `docs/INTEGRACOES.md` para v3.1 com data de novembro/2026. Verificação: o topo declara v3.1.
- [x] 6.2 Incluir SonarCloud (qualidade/análise estática), GitNexus (grafo de código) e OpenCode (agente de desenvolvimento) no conjunto de ferramentas do modo agente, alinhando as Fases E1/E2. Verificação: as três ferramentas aparecem nas tabelas e nas seções de integração.
- [x] 6.3 Adicionar a entrada v3.1 ao histórico de versões. Verificação: a tabela final contém a linha v3.1.

## 7. CASA, FÉLIX, PERSONALIDADE E QUANTUM_THEORY

- [x] 7.1 Atualizar o `docs/CASA.md` para v2.1, revisar referências cruzadas e preservar o conteúdo conceitual. Verificação: o topo declara v2.1 e o diff não altera conceitos da Casa.
- [x] 7.2 Atualizar o `docs/FELIX.md` para v2.2, revisar referências cruzadas e adicionar a entrada no histórico de versões. Verificação: o topo declara v2.2 e a tabela final contém a linha v2.2.
- [x] 7.3 Atualizar o `docs/PERSONALIDADE.md` para v3.1, revisar referências cruzadas e adicionar a entrada no histórico de versões. Verificação: o topo declara v3.1 e a tabela final contém a linha v3.1.
- [x] 7.4 Adicionar cabeçalho de versão v2.1 ao `docs/QUANTUM_THEORY.md` e revisar referências cruzadas. Verificação: o topo declara v2.1.
- [x] 7.5 Revisar os quatro documentos para garantir que só versão, status e referências mudaram. Verificação: `git diff -- docs/CASA.md docs/FELIX.md docs/PERSONALIDADE.md docs/QUANTUM_THEORY.md` mostra apenas esses tipos de alteração.
- [x] 7.6 Atualizar o `docs/FASE4.md` para v2.1, marcar a Fase 4.2 como concluída e registrar as métricas da LSTM treinada. Verificação: o documento não contém mais "em andamento" para a Fase 4.2 e cita perplexidade, corpus e vocabulário.

## 8. Auditoria final

- [x] 8.1 Rodar `grep -rn 'v2.0\\|v2.1\\|v3.0\\|v3.1' docs/*.md` e corrigir referências cruzadas inconsistentes nos nove documentos. Verificação: nenhuma referência aponta para uma versão que o documento não tem mais.
- [x] 8.2 Verificar que a Fase 4 aparece como concluída em todos os documentos que a citam. Verificação: `grep -rn 'Fase 4\\|FASE 4' docs/*.md` não contém "planejada" nem "prioridade atual" para a Fase 4.
- [x] 8.3 Verificar a consistência das métricas canônicas (10,7M tokens, 72.075 palavras, 64d, 176,7, 32.507 tokens, 1.559 palavras). Verificação: os valores conferem com o README e com o FASE4.md.
- [x] 8.4 Verificar que todos os links para `docs/` no README e nos documentos canônicos apontam para arquivos existentes. Verificação: script de checagem de links retorna zero links quebrados.
- [x] 8.5 Revisar o diff completo e confirmar que nenhum código, corpus, peso ou vetor foi alterado. Verificação: `git status` e `git diff --stat` mostram apenas arquivos em `docs/` e `openspec/`.

## Workflow follow-up

- Arquivar a mudança após a revisão do conjunto documental.
- Verificar o resultado arquivado e o histórico de versões.
