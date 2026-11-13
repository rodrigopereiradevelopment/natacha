# Proposal

## Why

A documentação canônica ainda trata a Fase 4 como planejada ou em andamento, não registra a LSTM treinada e validada com BPTT (perplexidade 884,5 → 176,7) nem o corpus v39 (10,7 milhões de tokens, 72.075 palavras, embeddings de 64 dimensões). Ao mesmo tempo, a visão de longo prazo — MoE hierárquica 8x8, multimodalidade, modo agente, mobile/offline e Expert de Identidade — está espalhada entre documentos, com status e referências cruzadas desatualizados. Isso enfraquece o TCC e faz o conjunto documental contar uma história diferente da implementação real.

## What Changes

- Atualizar os nove documentos canônicos e o documento de apoio `docs/arquitetura/fase4.md` para a revisão **v2.1** da documentação (cada arquivo recebe seu próximo minor), com data de novembro/2026 e changelog da revisão.
- **`docs/ROADMAP.md`**:
  - marcar a Fase 4 como concluída (4.1 forward pass e 4.2 BPTT), registrando a perplexidade final;
  - atualizar o Estado Atual para novembro/2026 e o próximo passo para a Fase 5 (Self-Attention);
  - consolidar a visão de longo prazo: MoE hierárquica 8x8 (7B totais, ~1,5B ativos), multimodalidade (visão, imagem, vídeo e slides), modo agente (Manus, OpenCode, GitNexus, SonarCloud), mobile/offline com quantização mista 8-bit/4-bit e Expert de Identidade sempre ativo.
- **`docs/ARQUITETURA.md`**:
  - corrigir o status das Fases 2, 3 e 4;
  - registrar embeddings de 64 dimensões e o pipeline LSTM + BPTT;
  - descrever a evolução para MoE hierárquica e o Expert de Identidade sempre ativo como parte da trilha neural de longo prazo.
- **`docs/REQUISITOS.md`**: revisar os requisitos da Fase 4 (RF-32) e explicitar os requisitos de identidade persistente no contexto do Expert de Identidade e da MoE.
- **`docs/GLOSSARIO.md`**: adicionar/atualizar termos como MoE hierárquica 8x8, Expert de Identidade, quantização mista, SonarCloud, GitNexus e Manus.
- **`docs/INTEGRACOES.md`**: incluir SonarCloud, GitNexus e OpenCode no conjunto de ferramentas do modo agente, alinhando a Fase E1/E2.
- **`docs/CASA.md`, `docs/FELIX.md`, `docs/PERSONALIDADE.md`, `docs/QUANTUM_THEORY.md`**: atualizar cabeçalhos de versão, referências cruzadas e status para v2.1, sem alterar os conceitos canônicos.
- **`docs/arquitetura/fase4.md`**: atualizar o status da Fase 4.2 para concluída e registrar as métricas da LSTM treinada, alinhando o documento de apoio ao restante da documentação.
- **Fora de escopo**: implementar qualquer item da visão de longo prazo, retreinar modelos, alterar corpus ou modificar código.

## Capabilities

### New Capabilities

Nenhuma. Esta é uma mudança exclusivamente documental, marcada com `skip_specs: true`; nenhum comportamento de sistema muda.

### Modified Capabilities

Nenhuma. Os documentos descrevem comportamento já existente ou visão futura, mas não alteram requisitos de sistema.

## Impact

- **Documentos afetados**: `docs/ROADMAP.md`, `docs/ARQUITETURA.md`, `docs/REQUISITOS.md`, `docs/GLOSSARIO.md`, `docs/CASA.md`, `docs/FELIX.md`, `docs/PERSONALIDADE.md`, `docs/INTEGRACOES.md`, `docs/QUANTUM_THEORY.md` e o documento de apoio `docs/arquitetura/fase4.md`.
- **Código, APIs e dependências**: nenhum impacto.
- **Risco**: baixo; o principal risco é inconsistência de referências cruzadas entre documentos, mitigado por uma revisão final de versões e links.
- **README**: já reflete a Fase 4.2 e o corpus v39; só será tocado se a revisão apontar uma referência cruzada quebrada.
