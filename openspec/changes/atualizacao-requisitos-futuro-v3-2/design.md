# Design — atualizacao-requisitos-futuro-v3-2

## Visão Geral

Especificar as alterações no `docs/REQUISITOS.md` para elevar o documento da versão 3.1 para 3.2, inserindo:
- Duas novas regras de negócio (RN-12, RN-13) na Seção 2.
- Três novos requisitos funcionais (RF-39, RF-40, RF-41) na Seção 3.2.
- Atualização da matriz de rastreabilidade (6.1 e 6.3).
- Entrada v3.2 no Histórico de Versões (Seção 8).
- Atualização da versão no cabeçalho (3.1 → 3.2).

## Detalhamento por Seção

### Cabeçalho (linha 6)

Alterar:
```
Versão: 3.1 | Data: Novembro/2026
```
para:
```
Versão: 3.2 | Data: Novembro/2026
```

### Seção 2 — Regras de Negócio (após RN-11, antes do `---`)

Inserir **RN-12** e **RN-13** seguindo o formato existente:
```
RN-NN — Título

· Descrição: ...
· Justificativa: ...
· Escopo: ...
```

### Seção 3.2 — Modo Agente (após RF-38.3, antes do `---` da Seção 4)

Inserir **RF-39**, **RF-40**, **RF-41** na tabela existente:
```
ID Requisito Funcional Prioridade Método de Teste / Aceite
RF-39 ... Alta Carregar grafo de dependências do projeto e validar leitura de símbolos, callers e processos afetados.
RF-40 ... Alta Enviar imagem como entrada e validar geração de conteúdo de saída (imagem, vídeo ou slide).
RF-41 ... Alta Desconectar a rede e validar inferência contínua do SLM/MoE local.
```

### Seção 6.1 — Matriz de Rastreabilidade de RFs

Adicionar linhas:
```
RF-39 RN-12 UC-03 Modo Agente (Analisador de Grafo / GitNexus)
RF-40 RN-12 UC-03 / UC-05 Orquestração Multimodal (Visão / Geração)
RF-41 RN-13 UC-03 Motor de Inferência Local (SLM/MoE Mobile)
```

### Seção 6.3 — Rastreabilidade Reversa (RN → RFs)

Adicionar entradas:
```
RN-12 RF-18.1, RF-39, RF-40, RF-41
RN-13 RF-41
```
(e garantir consistência com RF-39, RF-40, RF-41 apontando para RN-12/RN-13).

### Seção 8 — Histórico de Versões

Inserir linha:
```
3.2 Novembro/2026 Adicao de RN-12 (Quality Gates SonarCloud/GitNexus no Modo Agente), RN-13 (quantizacao mista mobile offline + persistenca Expert de Identidade), RF-39 (analise de grafo de codigo), RF-40 (geracao multimodal), RF-41 (execucao offline SLM/MoE mobile), e atualizacao da matriz de rastreabilidade.
```

## Regras de Formato

- Manter a numeração sequencial existente (RN-12 segue RN-11; RF-39 segue RF-38.3).
- Manter os separadores `---` entre seções.
- Manter o tom e a estrutura de cada entrada (Descrição / Justificativa / Escopo para RN; tabela ID/Requisito/Prioridade/Teste para RF).
- Não alterar requisitos existentes.

## Critérios de Aceite (Design)

- [ ] Cabeçalho: Versão 3.2.
- [ ] Seção 2: RN-12 e RN-13 presentes e formatados.
- [ ] Seção 3.2: RF-39, RF-40, RF-41 presentes na tabela.
- [ ] Seção 6.1: linhas RF-39/40/41 na matriz.
- [ ] Seção 6.3: entradas reversas RN-12/RN-13.
- [ ] Seção 8: linha v3.2 no historico.
