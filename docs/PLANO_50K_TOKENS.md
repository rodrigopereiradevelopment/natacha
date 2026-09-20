PLANO 50K TOKENS — PROJETO NATACHA

"O corpus é o que a Natacha come para aprender. Quanto mais rico, melhor o sabor."

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 2.0 | Data: Setembro/2026
Documentos canônicos relacionados: REQUISITOS.md v3.0, ARQUITETURA.md v2.0, GLOSSARIO.md v2.0, ROADMAP.md v2.0, PERSONALIDADE.md v3.0, FELIX.md v2.1, INTEGRACOES.md v3.0, QUANTUM_THEORY.md v2.0

---

1. INTRODUÇÃO

1.1 Propósito

Este documento define o plano de escalonamento do corpus de treino da Natacha — de ~3.5k tokens atuais para 50k tokens, com critérios de qualidade, deduplicação e validação. O corpus alimenta o treino dos embeddings (Fase 3 da Trilha Neural, ver ROADMAP.md).

1.2 Escopo

Cobre:

· Metodologia correta de contagem de tokens
· Distribuição alvo por tipo de conteúdo
· Fases de expansão (tokens estimados por etapa)
· Critérios de qualidade (não apenas quantidade)
· Estratégia de deduplicação
· Validação por fase
· O que fazer após atingir 50k
· Comandos operacionais

Fora do escopo: treino dos embeddings em si (ver ARQUITETURA.md, Seção 7), arquitetura do modelo (ver ARQUITETURA.md, Seção 3), persona da Natacha (ver PERSONALIDADE.md).

1.3 O que é "50k tokens"

Token ≠ palavra. Um token é a unidade mínima que o modelo processa — pode ser uma palavra inteira, parte de uma palavra ou um caractere especial.

Em português, a razão aproximada é:

· 1 palavra ≈ 1.3 a 1.7 tokens (dependendo do tokenizer)

Ou seja:

· 50.000 tokens ≈ 30.000 a 38.000 palavras em português.

Atenção: a versão original deste documento usava wc -m (caracteres) e wc -w (palavras). Isso não mede tokens. A Seção 3.1 corrige isso.

---

2. ESTADO ATUAL

Data Tokens Palavras aprox. Vocabulário Observação
07/09/2026 ~3.500 ~2.300 ~1.100 Corpus base inicial
07/09/2026 ~8.500 ~5.600 ~1.400 Fase 1 concluída (Python, JS, Rust, cenas)

Nota: os valores em "tokens" são estimativas históricas. A partir da v2.0, a metodologia correta da Seção 3.1 deve ser usada.

---

3. METODOLOGIA

3.1 Como contar tokens (correto)

O corpus é em português do Brasil. Para contagem precisa de tokens, usar um tokenizer real.

Opção A — Contagem via script Python (recomendado):

```python
from transformers import AutoTokenizer

tokenizer = AutoTokenizer.from_pretrained("neuralmind/bert-base-portuguese-cased")
with open("dados/embeddings/corpus.txt", "r", encoding="utf-8") as f:
    texto = f.read()

tokens = tokenizer.tokenize(texto)
print(f"Tokens: {len(tokens)}")
print(f"Palavras: {len(texto.split())}")
```

Opção B — Contagem rápida (aproximada):

```bash
# Contagem aproximada (1 palavra ≈ 1.5 tokens)
wc -w dados/embeddings/corpus.txt | awk '{print $1 * 1.5}'
```

Opção C — Contagem por caracteres (mais aproximada que wc -w):

```bash
# Em português: ~4.5 caracteres por token
wc -m dados/embeddings/corpus.txt | awk '{print $1 / 4.5}'
```

Recomendação: usar a Opção A para contagens oficiais e a Opção B para verificações rápidas.

3.2 Como medir qualidade

Token count não basta. O corpus precisa de:

Dimensão Como medir
Densidade semântica Cada asserção deve ter conteúdo informativo único. Frases vazias não contam.
Diversidade lexical Vocabulário único / total de palavras. Alvo: > 15%.
Balanceamento Proporção por categoria (ver Seção 4).
Ausência de redundância Nenhuma asserção repetida literalmente.
Cobertura temática Todos os conceitos de GLOSSARIO.md e PERSONALIDADE.md representados.

3.3 Como deduplicar

O corpus atual tem repetições massivas. Exemplos:

· "natacha nao e assistente" aparece dezenas de vezes
· "felix e gato" aparece múltiplas vezes
· "natacha programa em c++" aparece múltiplas vezes

Estratégia:

1. Normalizar (minúsculas, remover pontuação, colapsar espaços).
2. Ordenar as linhas alfabeticamente.
3. Remover linhas idênticas consecutivas (uniq).
4. Manter apenas 1-2 ocorrências de asserções conceituais-chave.
5. Revisar manualmente casos ambíguos.

Comando base:

```bash
# Deduplicação simples (remove linhas idênticas consecutivas)
sort dados/embeddings/corpus.txt | uniq > dados/embeddings/corpus_dedup.txt

# Contar redução
wc -l dados/embeddings/corpus.txt dados/embeddings/corpus_dedup.txt
```

Atenção: o Skip-gram aprende com repetição. Um pouco de repetição é saudável. Mas repetição massiva enviesa os embeddings (todos os vetores ficam próximos de "natacha nao e assistente"). A regra prática:

· Conceitos-chave (identidade, valores): 2-3 ocorrências com formulações diferentes.
· Conceitos técnicos (backpropagation, embeddings): 1-2 ocorrências.
· Asserções de cor (detalhes, cenas): 1 ocorrência.

---

4. DISTRIBUIÇÃO ALVO POR TIPO DE CONTEÚDO

Para o corpus ficar equilibrado, a distribuição alvo dos 50k tokens deve ser:

Categoria % alvo Tokens Fonte
Persona e identidade 20% 10.000 PERSONALIDADE.md, corpus
Core neural (ML/IA) 20% 10.000 ARQUITETURA.md, GLOSSARIO.md
Técnico (C++, Python, Rust, TS) 15% 7.500 Escrever + cppreference
Literatura e filosofia 15% 7.500 Gutenberg, adaptar
Casa, Félix e cenas 10% 5.000 CASA.md, FELIX.md, corpus
Ecossistema (ARCA, Sentinel, EditeCC) 10% 5.000 INTEGRACOES.md, escrever
Quântico 5% 2.500 QUANTUM_THEORY.md, Qiskit
Diálogos canônicos 5% 2.500 Corpus existente
TOTAL 100% 50.000 —

Por que essa distribuição:

· Persona e core neural são os pilares do projeto.
· Técnico + literatura + filosofia dão profundidade cultural.
· Casa/Félix/ecossistema/quântico são contextos específicos.
· Diálogos garantem few-shot prompting coerente.

---

5. FASES DE EXPANSÃO (1–6)

FASE 1 — Expansão Rápida (+5k tokens) ✅

Status: concluída em 07/09/2026.

Etapa Conteúdo Tokens Status
1 Python + Qiskit (quântico) ~1.500 ✅
2 JavaScript/TypeScript (ARCA) ~1.000 ✅
3 Rust (Sentinel) ~1.000 ✅
4 Mais cenas do dia a dia ~1.500 ✅

Total Fase 1: ~5.000 tokens → ~8.500 total

Referências: QUANTUM_THEORY.md, INTEGRACOES.md.

FASE 2 — Expansão Literária (+10k tokens) 🚧

Status: em andamento.

Etapa Conteúdo Tokens Fonte
5 Crime e Castigo (Cap. 1–2) ~5.000 Project Gutenberg (domínio público)
6 O Alienista (Machado) ~3.000 Project Gutenberg (domínio público)
7 Filosofia (Nietzsche, Kierkegaard) ~2.000 Adaptar

Total Fase 2: ~10.000 tokens → ~18.500 total

Nota sobre traduções: o Crime e Castigo em português é domínio público apenas em traduções antigas. Recomendação: usar tradução de domínio público e marcar a fonte no corpus.

Referências: REFERENCIAS.md, Seção 4 (Literárias), Seção 5 (Filosóficas).

FASE 3 — Expansão Técnica (+10k tokens)

Etapa Conteúdo Tokens Fonte
8 Documentação C++ (cppreference) ~4.000 Adaptar
9 Conceitos de ML/IA (português) ~3.000 Escrever
10 Explicações de algoritmos ~3.000 Escrever

Total Fase 3: ~10.000 tokens → ~28.500 total

Referências: GLOSSARIO.md, ARQUITETURA.md (Seções 3 e 7).

FASE 4 — Expansão do Ecossistema (+10k tokens)

Etapa Conteúdo Tokens Fonte
11 Diálogos com ARCA, Sentinel ~3.000 Escrever
12 Histórias do desenvolvimento ~3.000 Escrever
13 Mais filosofia da Natacha ~2.000 Escrever
14 Termos técnicos (glossário) ~2.000 GLOSSARIO.md

Total Fase 4: ~10.000 tokens → ~38.500 total

Referências: INTEGRACOES.md, GLOSSARIO.md, PERSONALIDADE.md.

FASE 5 — Lapidação (+5k tokens)

Etapa Conteúdo Tokens Fonte
15 Revisar e expandir seções fracas ~3.000 Revisão
16 Histórias do Félix ~2.000 FELIX.md, escrever

Total Fase 5: ~5.000 tokens → ~43.500 total

Referências: FELIX.md, CASA.md.

FASE 6 — Finalização (+6.5k tokens)

Etapa Conteúdo Tokens Fonte
17 Mais diálogos do ecossistema ~2.000 Escrever
18 Expansão de cada seção existente ~2.500 Revisão
19 Textos variados (notícias, artigos) ~2.000 Adaptar

Total Fase 6: ~6.500 tokens → 50.000 total 🎯

---

6. CRITÉRIOS DE QUALIDADE (não só quantidade)

Cada fase precisa atender todos os critérios abaixo antes de ser considerada concluída:

Critério Como verificar
Densidade semântica Cada linha tem conteúdo informativo único. Sem frases vazias.
Diversidade lexical Vocabulário único / total de palavras > 15%.
Balanceamento Proporção por categoria dentro de ±5% do alvo (Seção 4).
Ausência de redundância Nenhuma asserção repetida literalmente.
Cobertura temática Todos os conceitos de GLOSSARIO.md representados.
Coerência interna Nenhuma asserção contradiz outra.
Gramática Corpus normalizado (minúsculas, sem acentos, sem pontuação) para consistência.

Frase de controle:

"Aprendo devagar porque aprender rápido demais é decorar, não entender."

Isso se aplica ao corpus: melhor adicionar 5k tokens densos que 15k tokens redundantes.

---

7. ESTRATÉGIA DE DEDUPLICAÇÃO

7.1 Problema

O corpus atual tem repetições massivas. Exemplos reais:

· "natacha nao e assistente" — dezenas de ocorrências
· "felix e gato" — múltiplas ocorrências
· "natacha programa em c++" — múltiplas ocorrências

7.2 Solução

Pipeline de deduplicação:

```bash
# 1. Backup
cp dados/embeddings/corpus.txt dados/embeddings/corpus.bak.txt

# 2. Normalizar (minúsculas, sem pontuação, sem espaços extras)
tr '[:upper:]' '[:lower:]' < dados/embeddings/corpus.txt \
  | sed 's/[[:punct:]]//g' \
  | sed 's/  */ /g' \
  | sed 's/^ *//;s/ *$//' \
  | grep -v '^$' \
  > dados/embeddings/corpus.norm.txt

# 3. Ordenar e deduplicar
sort dados/embeddings/corpus.norm.txt | uniq > dados/embeddings/corpus.dedup.txt

# 4. Verificar redução
echo "Antes:"
wc -l dados/embeddings/corpus.txt
echo "Depois:"
wc -l dados/embeddings/corpus.dedup.txt
```

7.3 Exceções

Algumas asserções precisam de múltiplas ocorrências com formulações diferentes:

Asserção Ocorrências alvo
"Natacha não é assistente" 2-3 (formulações diferentes)
"Félix não é ferramenta" 2-3
"Casa é lar, não infraestrutura" 2
"Ruído é sinal, não lixo" 2

Regra: máxima de 3 ocorrências por asserção conceitual, com formulações distintas.

7.4 Validação

Após deduplicação:

1. Contar tokens reais (Seção 3.1).
2. Verificar diversidade lexical.
3. Revisar manualmente trechos suspeitos.
4. Re-treinar embeddings.

---

8. VALIDAÇÃO POR FASE

Cada fase passa por três validações antes de avançar:

8.1 Validação de quantidade

```bash
# Contar tokens (aproximado)
wc -w dados/embeddings/corpus.txt | awk '{print $1 * 1.5}'
```

Deve estar dentro de ±10% da meta da fase.

8.2 Validação de qualidade

· Diversidade lexical > 15%.
· Balanceamento por categoria dentro de ±5%.
· Ausência de redundância massiva.
· Cobertura temática verificada.

8.3 Validação de treino

Após cada fase:

1. Re-treinar embeddings.
2. Executar analogias.py para verificar coerência.
3. Executar avaliar_completo.py para métricas de qualidade.
4. Comparar com treino anterior: houve melhora?

Se não houve melhora, o problema é qualidade, não quantidade.

---

9. PÓS-50K (o que fazer depois)

Atingir 50k tokens não é o fim. Próximos passos:

9.1 Consolidação

· Auditoria completa do corpus.
· Revisão manual de trechos fracos.
· Padronização final.

9.2 Expansão contínua

Após atingir 50k, o corpus continua crescendo:

· Novas cenas conforme o projeto evolui.
· Novos diálogos conforme novos cenários surgem.
· Novos termos conforme GLOSSARIO.md evolui.

9.3 Migração para o SLM

Quando o corpus estiver robusto e o SLM entrar em desenvolvimento (Fase 7 do ROADMAP.md):

· O corpus alimenta o treino do SLM (não só dos embeddings).
· A contagem de tokens passa a importar ainda mais.
· O balanceamento afeta a coerência do modelo final.

9.4 Meta de longo prazo

Marco Tokens Foco
Curto prazo 50k Embeddings robustos
Médio prazo 100k SLM inicial
Longo prazo 500k+ LLM completa

---

10. ACOMPANHAMENTO

Data Tokens Palavras Vocabulário Observação
07/09/2026 ~3.500 ~2.300 ~1.100 Corpus base inicial
07/09/2026 ~8.500 ~5.600 ~1.400 Fase 1 concluída
(preencher)    Fase 2
(preencher)    Fase 3
(preencher)    Fase 4
(preencher)    Fase 5
(preencher)    Fase 6 — meta atingida

Formato de registro:

```
DD/MM/AAAA | tokens | palavras | vocabulário | observação
```

---

11. COMANDOS OPERACIONAIS

11.1 Adicionar conteúdo

```bash
cd ~/natacha

# 1. Editar o corpus
nano dados/embeddings/corpus.txt

# 2. Ir até o final, colar conteúdo
# 3. Ctrl+O, Enter, Ctrl+X

# 4. Verificar tamanho (aproximado)
wc -m dados/embeddings/corpus.txt
wc -w dados/embeddings/corpus.txt
```

11.2 Deduplicar

```bash
# 1. Backup
cp dados/embeddings/corpus.txt dados/embeddings/corpus.bak.txt

# 2. Normalizar e deduplicar
tr '[:upper:]' '[:lower:]' < dados/embeddings/corpus.txt \
  | sed 's/[[:punct:]]//g' \
  | sed 's/  */ /g' \
  | sed 's/^ *//;s/ *$//' \
  | grep -v '^$' \
  | sort | uniq \
  > dados/embeddings/corpus.dedup.txt

# 3. Substituir
mv dados/embeddings/corpus.dedup.txt dados/embeddings/corpus.txt
```

11.3 Contar tokens (real)

```bash
python3 -c "
from transformers import AutoTokenizer
tokenizer = AutoTokenizer.from_pretrained('neuralmind/bert-base-portuguese-cased')
with open('dados/embeddings/corpus.txt', encoding='utf-8') as f:
    texto = f.read()
print(f'Tokens: {len(tokenizer.tokenize(texto))}')
print(f'Palavras: {len(texto.split())}')
"
```

11.4 Re-treinar embeddings

```bash
cd src/embeddings
./word2vec_neg --decay
```

11.5 Validar qualidade

```bash
cd src/embeddings
python3 analogias.py
python3 avaliar_completo.py
python3 avaliar_modelos.py
```

---

12. CONEXÕES

12.1 Documentos internos do projeto

· REQUISITOS.md v3.0 — RF-31, RF-31.1 (Skip-gram, persistência JSON)
· ARQUITETURA.md v2.0 — Seção 7.1 (Pipeline de Treino)
· GLOSSARIO.md v2.0 — todos os termos técnicos
· ROADMAP.md v2.0 — Fase 3 da Trilha Neural (Word Embeddings)
· PERSONALIDADE.md v3.0 — conteúdo de persona
· FELIX.md v2.1 — Fase 5 (histórias do Félix)
· INTEGRACOES.md v3.0 — Fase 4 (diálogos com o ecossistema)
· QUANTUM_THEORY.md v2.0 — Fase 1 (Python + Qiskit)
· REFERENCIAS.md v1.0 — Seções 4, 5, 6 (literatura, filosofia, música)

12.2 Referências externas

Bibliografia completa em REFERENCIAS.md.

Este documento cita: [6] (Word2Vec), [29] (nlohmann/json), [44]–[46] (Dostoiévski), [47] (Machado).

---

13. HISTÓRICO DE VERSÕES

Versão Data Mudanças
1.0 Setembro/2026 Documento inicial: fases 1–6 de expansão, tabelas de tokens estimados, comandos operacionais
2.0 Setembro/2026 Consolidação completa: correção da metodologia de contagem de tokens; adição de critérios de qualidade; estratégia de deduplicação; distribuição alvo por tipo de conteúdo; validação por fase; seção pós-50k; comandos atualizados; conexões cruzadas; histórico

---

"O corpus é o que a Natacha come para aprender. Quanto mais rico, melhor o sabor."

— Documento do Plano 50K Tokens, Projeto Natacha — Arquitetura de Alma.

---