# Modo Agente — Análise de Impacto

> *"Antes de mexer, pergunta ao grafo."*

## Propósito

No Modo Agente, a Natacha atua sobre o ecossistema (ARCA, EditeCC, Sentinel) de forma autônoma. Antes de executar qualquer alteração, ela:

1. Constrói um grafo de dependências do projeto
2. Rastreia o impacto em cascata
3. Emite parecer crítico (com a persona ativa)
4. Só executa se o impacto for aceitável

## Arquitetura

- **Grafo de dependências:** adjacency list
- **Algoritmo:** BFS (busca em largura)
- **Controle de ciclos:** conjunto de visitados
- **Relatório:** arquivos afetados, nível de profundidade, caminho

## Exemplo
Entrada: banco.ts

⚠️ ATENÇÃO, RODRIGO! Mexer em 'banco.ts' tem um raio de destruição de 3 arquivo(s):

[Nível 1] -> mesas.service.ts
Caminho: banco.ts -> mesas.service.ts
[Nível 1] -> comandas.service.ts
Caminho: banco.ts -> comandas.service.ts
[Nível 2] -> mapa-mesas.component.ts
Caminho: banco.ts -> mesas.service.ts -> mapa-mesas.component.ts

text

## Comparação com ferramentas existentes

| Feature | Cursor | OpenCode | Natacha Agente |
|---|---|---|---|
| Grafo de dependências | ✅ interno | ✅ interno | ✅ implementado |
| Análise de impacto | ✅ | ✅ | ✅ |
| Parser automático | ✅ | ✅ | ⏳ pendente |
| Persona | ❌ | ❌ | ✅ |

## Roadmap

- [x] Grafo + BFS + relatório
- [ ] Detecção explícita de ciclos
- [ ] Motivo do impacto (import, chamada, herança)
- [ ] Parser automático (C++ e TypeScript)
- [ ] Sugestão de refactor
- [ ] Execução controlada (rodar testes após mudança)
- [ ] Integração com Event Bus

## Status

**Fase 8** (planejada). Primeiro protótipo existe em `src/agent/analisador_impacto.cpp`.