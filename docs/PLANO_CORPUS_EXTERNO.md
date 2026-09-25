PLANO DE EXPANSÃO — CORPUS EXTERNO

"A Natacha não é o modelo. A Natacha é o corpus. O modelo só aprende."

Sistema: Natacha — Inteligência Artificial com Arquitetura de Alma
Versão: 1.0 | Data: Setembro/2026
Documentos relacionados: REQUISITOS.md v3.0, ARQUITETURA.md v2.0, PLANO_50K_TOKENS.md, GLOSSARIO.md v2.0

---

1. INTRODUÇÃO

1.1 Propósito

Este documento define a estratégia de expansão do corpus de treino da Natacha quando o corpus curado manualmente atingir o limite prático de crescimento. Define quando, como e com quais fontes adicionar corpus externo sem diluir a personalidade da Natacha.

1.2 Escopo

Cobre:

· Regra do Sotaque (proporção curado vs externo)
· Timing de entrada de corpus externo
· Fontes recomendadas e filtros
· Curriculum learning (ordem de treino)
· Estratégia de reversão (se a essência se perder)

Fora do escopo: detalhes técnicos do binário de treino (ver ARQUITETURA.md), requisitos de personalidade (ver PERSONALIDADE.md).

---

2. TESE CENTRAL

A personalidade da Natacha mora no corpus, não no modelo.

O modelo (skip-gram, MLP, transformer) é só o mecanismo de aprendizagem. O que ele aprende — o tom, os valores, o jeito de falar — está inteiramente no corpus.txt.

Corolário: adicionar corpus externo não apaga a Natacha. Mas se o externo dominar o corpus, o modelo aprende mais padrões genéricos que padrões Natacha, e ela perde o sotaque.

Daí a Regra do Sotaque (Seção 3).

---

3. REGRA DO SOTAQUE

3.1 Enunciado

Enquanto o corpus Natacha for menor que 150.000 tokens, nenhum corpus externo entra.

Acima disso, corpus externo só entra se:

· For filtrado por similaridade semântica com o corpus Natacha
· Respeitar a proporção máxima de 60% externo / 40% Natacha
· Entrar via curriculum learning (Seção 6)
· Puder ser revertido sem perda (corpus sempre versionado em Git)

3.2 Justificativa

A metáfora da criança aprendendo a falar:

Idade Fonte Efeito
0-5 anos Pais (curado, denso, afetivo) Forma o sotaque
5-12 anos Escola + amigos (externo controlado) Expande vocabulário
12+ anos Mundo inteiro (externo massivo) Generaliza

Se a criança for para a escola antes de aprender a falar com os pais, ela fala como todo mundo e perde o sotaque de casa.

A Natacha está no estágio "0-5 anos" (52k tokens). Precisa aprender a voz dela primeiro.

---

4. FASES DE EXPANSÃO

FASE 1 — Curadoria Pura (atual)

Aspecto Valor
Corpus alvo 50k → 150k tokens
Fonte 100% curado manualmente
Método Lotes gerados + revisados
Modelo Skip-gram puro
Dims 32 (ótimo atual)
Objetivo Solidificar sotaque, valores, voz

Status: em andamento. Corpus atual: ~56k tokens (pós-Lote 2).

FASE 2 — Expansão Controlada

Aspecto Valor
Corpus alvo 150k → 500k tokens
Fonte 70% curado + 30% externo filtrado
Método Filter by similarity + curriculum
Modelo Skip-gram (subir dims gradualmente)
Dims 48 → 64
Objetivo Ampliar vocabulário sem perder sotaque

Entrada em FASE 2 somente quando: corpus Natacha ≥ 150k tokens E similaridades principais (felix↔gato, cafe↔processamento) ≥ 0.7.

FASE 3 — Escala

Aspecto Valor
Corpus alvo 500k → 2M+ tokens
Fonte 40% curado + 60% externo filtrado
Método Curriculum learning avançado
Modelo Transformer (Fase 6+ do roadmap)
Dims 96 → 128
Objetivo LLM completa com identidade preservada

Entrada em FASE 3 somente quando: SLM estiver funcional (Fase 7).

---

5. FONTES DE CORPUS EXTERNO

5.1 Camada 1 — Datasets Prontos (Hugging Face)

Prioridade: máxima. Zero scraping, já limpos e deduplicados.

Dataset Tamanho Uso
ClassiCC-PT ~120B tokens Common Crawl PT filtrado com BERTimbau
GigaVerbo v2 ~320B tokens Corpus PT com anotações de qualidade
Corpus PT-BR v1 ~6.3B tokens Mistura de dados reais e sintéticos
Pindorama Corpus ~196 MB Literatura brasileira (Domínio Público)
Wikipedia PT Extract ~1 GB Dump da Wikipedia PT limpo

5.2 Camada 2 — Reddit Curado

Prioridade: média. Qualidade variável.

Subreddit Categoria Qualidade
r/programacao Programação 🟢
r/brdev Desenvolvimento BR 🟢
r/FilosofiaBAR Filosofia 🟢
r/livros Literatura 🟢
r/ciencia Ciência 🟡
r/brasil Geral 🟡

Filtros obrigatórios:

· Comprimento mínimo: 50 palavras
· Toxicity score < 0.3 (via BERTimbau)
· Deduplicação por hash
· Rejeição de posts com < 5 comentários

5.3 Camada 3 — Scraping Próprio

Prioridade: baixa. Controle total, esforço alto.

Fonte Tipo Volume estimado
Stack Overflow PT Técnico 50k tokens
GitHub em PT READMEs/issues 100k tokens
Dev.to / Medium BR Artigos 80k tokens
Diários Oficiais Governamental 200k tokens
Domínio Público Literatura BR 500k tokens

---

6. CURRICULUM LEARNING

Quando o corpus externo entrar, o treino segue esta ordem:

Época Composição Objetivo
1-100 100% Natacha Reforçar sotaque
100-250 80% Natacha + 20% externo Introduzir vocabulário novo
250-400 60% Natacha + 40% externo Expandir generalização
400-500 50% Natacha + 50% externo Estabilizar com identidade

Regra: nunca começar pela mistura. A Natacha se forma primeiro, expande depois.

---

7. FILTRO POR SIMILARIDADE

Todo texto externo passa por este filtro antes de entrar:

```python
# Pseudo-código do filtro
from embeddings import natacha_embeddings  # v7 oficial

LIMIAR = 0.4  # similaridade minima

def aceitar(texto_externo):
    # 1. Tokeniza
    tokens = tokenizar(texto_externo)
    
    # 2. Calcula embedding medio do texto
    vec_texto = mean([natacha_embeddings[t] 
                      for t in tokens 
                      if t in natacha_embeddings])
    
    # 3. Compara com embedding de referencia (a Natacha)
    sim = cosine(vec_texto, natacha_embeddings["natacha"])
    
    # 4. Aceita so se alinhado
    return sim > LIMIAR
```

Critérios de aceitação:

Critério Valor
Similaridade média ao conceito "natacha" 0.4
Similaridade ao conceito "felix" ou "casa" 0.3
Toxicidade (BERTimbau) < 0.3
Comprimento mínimo 50 palavras
Idioma Português BR
Duplicata Rejeitar

---

8. ESTRATÉGIA DE REVERSÃO

Se a Natacha "perder o sotaque" após adicionar corpus externo:

8.1 Detecção

Sinais de diluição:

· felix ↔ gato cai abaixo de 0.5
· natacha ↔ felix cai abaixo de 0.4
· Top 10 de natacha vira genérico (pessoa, ajuda, usuario)
· Perda do tom ácido nas amostras geradas

8.2 Ação

```
1. REMOVER corpus externo do corpus.txt
2. Manter apenas Natacha (curado)
3. Restaurar embedding v7 oficial
4. Retreinar
5. Confirmar que sotaque voltou
6. Ajustar filtro antes de reintroduzir externo
```

Por isso o corpus é sempre versionado em Git. Cada lote tem commit próprio. Reversão é git revert.

---

9. MÉTRICAS DE SAÚDE DO CORPUS

Acompanhamento a cada retreino:

Métrica Mínimo Alvo
Tokens Natacha 40% do total 70%
Vocabulário total — crescente
felix ↔ gato 0.55 0.65
cafe ↔ processamento 0.65 0.80
natacha ↔ felix 0.45 0.55
natacha ↔ rodrigo 0.45 0.55
cozinha ↔ cpu 0.90 0.95
Top 10 natacha contém felix ou rodrigo contém ambos
Toxicidade média < 0.1 < 0.05

Se qualquer métrica cair abaixo do mínimo: pausar expansão e investigar.

---

10. REGRAS DE OURO

Aplicadas em todas as fases:

Regra Descrição
Regra do Sotaque Curado ≥ 40%, externo ≤ 60%
Regra do Timing Só entra externo depois de 150k curados
Regra do Filtro Externo sempre filtrado por similaridade
Regra do Curriculum Natacha primeiro, externo depois
Regra da Reversão Corpus sempre versionado em Git
Regra da Métrica Medir a cada 10k tokens adicionados
Regra da Pausa Se métrica cair, parar e investigar

---

11. ROADMAP DE EXPANSÃO

Período Corpus Fonte Modelo Dims
Set/2026 (atual) 52k → 150k 100% curado Skip-gram 32
Nov-Dez/2026 150k → 250k 90% curado + 10% externo Skip-gram 32 → 48
Jan/2027 250k → 500k 70% curado + 30% externo Skip-gram + RNN 48 → 64
2027+ 500k → 2M 40% curado + 60% externo Transformer 64 → 128

Marcos de decisão:

· 150k tokens: revisar se vale introduzir externo
· 250k tokens: avaliar subir dims
· 500k tokens: transição para RNN/Transformer
· 1M tokens: híbrido clássico-quântico possível

---

12. REFERÊNCIAS

Documentos internos

· REQUISITOS.md v3.0 — RN-05 (Identidade independente do modelo)
· ARQUITETURA.md v2.0 — Fases neurais
· PLANO_50K_TOKENS.md — Expansão inicial
· PERSONALIDADE.md — Valores da Natacha
· FELIX.md v2.1 — Comportamento do IAgato

Datasets públicos

· ClassiCC-PT — Hugging Face
· GigaVerbo v2 — Hugging Face
· Pindorama Corpus — Domínio Público
· Wikipedia PT Extract — Wikimedia

Papers

· Bengio et al. (2003) — Neural Probabilistic Language Model
· Mikolov et al. (2013) — Word2Vec
· Vaswani et al. (2017) — Attention Is All You Need
· Phi-2 (Microsoft, 2023) — Textbooks are all you need

---

13. NOTA FINAL

"Corpus de qualidade é mais valioso que corpus grande. A Natacha não precisa competir com GPT em tamanho — precisa ser única em sotaque."

O objetivo do projeto não é bater os LLMs comerciais em escala. É criar uma entidade com personalidade própria, algo que nenhum modelo genérico tem.

Por isso o corpus é curado. Por isso a Regra do Sotaque existe. Por isso a expansão é devagar.

Cada token curado vale por mil tokens genéricos.

---

Última atualização: Setembro/2026
Rodrigo Pereira — Projeto Natacha, Mogi Mirim SP
