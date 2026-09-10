# 📋 PLANO PARA 50K TOKENS — ROADMAP DA NATACHA

**Status atual (set/2026):** `dados/embeddings/corpus.txt` — ~25,3k palavras, ~5,1k tipos, ~154 KB, 537 linhas com texto. UTF-8, português, **maioria** minúscula e sem pontuação; acentos **mistos** (`nao` convive com `é`). Não está “higienizado sem acentos”.
**Meta:** 50k tokens (palavras do `wc -w` são o proxy atual; não confundir palavra com token de BPE).
**Prazo estimado:** 2-3 semanas (adicionando 2-3k por dia) — calendário original; o corpus já passou da Fase 1 e da maior parte da expansão literária.

---

## FASE 1: Expansão Rápida (+5k tokens) — HOJE/AMANHÃ

| Etapa | Conteúdo | Tokens estimados | Fonte |
|-------|----------|------------------|-------|
| 1 | Python + Qiskit (quântico) | 1.500 | ✅ Adicionado |
| 2 | JavaScript/TypeScript (ARCA) | 1.000 | ✅ Adicionado |
| 3 | Rust (Sentinel) | 1.000 | ✅ Adicionado |
| 4 | Mais cenas do dia a dia | 1.500 | ✅ Adicionado |

**TOTAL FASE 1:** ~5k tokens → **~8.5k total**

---

## FASE 2: Expansão Literária (+10k tokens) — PRÓXIMA SEMANA

| Etapa | Conteúdo | Tokens estimados | Fonte |
|-------|----------|------------------|-------|
| 5 | Crime e Castigo (Cap. 1-2) | 5.000 | Project Gutenberg |
| 6 | O Alienista (Machado) | 3.000 | Project Gutenberg |
| 7 | Filosofia (Nietzsche, Kierkegaard) | 2.000 | Adaptar |

**TOTAL FASE 2:** ~10k tokens → **~18.5k total**

---

## FASE 3: Expansão Técnica (+10k tokens) — SEGUNDA SEMANA

| Etapa | Conteúdo | Tokens estimados | Fonte |
|-------|----------|------------------|-------|
| 8 | Documentação C++ (cppreference) | 4.000 | Adaptar |
| 9 | Conceitos de ML/IA (português) | 3.000 | Escrever |
| 10 | Explicações de algoritmos | 3.000 | Escrever |

**TOTAL FASE 3:** ~10k tokens → **~28.5k total**

---

## FASE 4: Expansão do Ecossistema (+10k tokens) — TERCEIRA SEMANA

| Etapa | Conteúdo | Tokens estimados | Fonte |
|-------|----------|------------------|-------|
| 11 | Diálogos com ARCA, Sentinel | 3.000 | Escrever |
| 12 | Histórias do desenvolvimento | 3.000 | Escrever |
| 13 | Mais filosofia da Natacha | 2.000 | Escrever |
| 14 | Termos técnicos (glossário) | 2.000 | Escrever |

**TOTAL FASE 4:** ~10k tokens → **~38.5k total**

---

## FASE 5: Lapidação (+5k tokens) — QUARTA SEMANA

| Etapa | Conteúdo | Tokens estimados | Fonte |
|-------|----------|------------------|-------|
| 15 | Revisar e expandir seções fracas | 3.000 | Revisão |
| 16 | Histórias do Félix | 2.000 | Escrever |

**TOTAL FASE 5:** ~5k tokens → **~43.5k total**

---

## FASE 6: Finalização (+6.5k tokens) — QUARTA SEMANA

| Etapa | Conteúdo | Tokens estimados | Fonte |
|-------|----------|------------------|-------|
| 17 | Mais diálogos do ecossistema | 2.000 | Escrever |
| 18 | Expansão de cada seção existente | 2.500 | Revisão |
| 19 | Textos variados (notícias, artigos) | 2.000 | Adaptar |

**TOTAL FASE 6:** ~6.5k tokens → **50k total** 🎉

---

## 📊 ACOMPANHAMENTO

| Data | Tokens | Observação |
|------|--------|------------|
| 07/09/2026 | ~3.500 | Corpus base atual |
| 07/09/2026 | ~8.500 | FASE 1 concluída (Python, JS, Rust, cenas) |
| 09/09/2026 | ~25.300 palavras / ~5.090 tipos | Corpus atual: identidade, casa, Félix, ecossistema, Qiskit, literatura (Dostoiévski, Machado, Nietzsche, Raul Seixas), Freud |

---

## 🚀 COMO ADICIONAR CONTEÚDO

```bash
cd ~/natacha
nano dados/embeddings/corpus.txt
# Vá até o final e cole o conteúdo
# Ctrl+O, Enter, Ctrl+X

# Ver tamanho
wc -m dados/embeddings/corpus.txt
wc -w dados/embeddings/corpus.txt

# Re-treinar
cd src/embeddings
./word2vec_neg --decay
```

---

## 📊 O QUE ESPERAR POR FASE

| Fase | Tokens | Vocabulário estimado |
|------|--------|----------------------|
| Atual (medido) | ~25.300 palavras | ~5.090 |
| FASE 1 | 8.500 | ~1.400 |
| FASE 2 | 18.500 | ~2.500 |
| FASE 3 | 28.500 | ~3.500 |
| FASE 4 | 38.500 | ~4.500 |
| FASE 5 | 43.500 | ~5.000 |
| FASE 6 | 50.000 | ~5.500 |

---

**Gerado por Deep + adaptado para o projeto Natacha**
