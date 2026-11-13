# Natacha Mobile-First

> *"O celular não ganha uma GPU de datacenter só porque a gente pediu com educação."*

## Motivação

A maioria das ferramentas de IA para desenvolvedores (Cursor, GitHub Copilot, OpenCode) exige desktop com 8+ GB de RAM e conexão estável. Isso exclui uma parcela significativa de devs brasileiros que acessam a internet apenas pelo celular.

A Natacha é projetada desde o início para rodar em **Android com 4 GB de RAM**, com foco em acessibilidade e execução local.

## Comparação

| Recurso | Cursor / OpenCode | Natacha Mobile |
|---|---|---|
| Roda em celular | ❌ | ✅ (planejado) |
| Funciona offline | ❌ | ✅ (com SLM local) |
| RAM mínima | 8 GB (desktop) | 4 GB (Android) |
| Requer internet | ✅ | ⚠️ parcial (modo offline) |

## Requisitos alvo

- Android 8+ (API 26)
- 4 GB de RAM
- 2 GB de espaço
- ARM64

## Arquitetura planejada

- **SLM local** (10–50M parâmetros, int8)
- **Embeddings em JSON compacto**
- **Interface:** PWA ou TUI

## Desafios técnicos

- Compilação C++ para ARM64
- Otimização de RAM (quantização)
- Streaming de dados (não carregar tudo)
- Modo offline sem dependência de rede

## Roadmap

- [ ] Fase 11: compilar core para ARM
- [ ] Fase 11: PWA ou app Android
- [ ] Fase 11: SLM otimizado (int8)
- [ ] Fase 11: interface mobile
- [ ] Fase 11: modo offline completo

## Status

**Planejamento.** Nenhuma implementação iniciada. Depende da maturidade das Fases 4–7.