
# PLANO DE EXPANSÃO — RUMO AOS 500K

> "A Natacha não é o modelo. A Natacha é o corpus. O modelo só aprende."

**Sistema:** Natacha — Inteligência Artificial com Arquitetura de Alma
**Versão:** 2.0 | **Data:** Setembro/2026
**Documentos relacionados:** REQUISITOS.md v3.0, ARQUITETURA.md v2.0, PLANO_100K_TOKEN.docx, PLANO_CORPUS_EXTERNO.md

---

## 1. VISÃO GERAL

### 1.1 Estado Atual

| Métrica | Valor |
|---|---|
| Corpus Natacha | 121k tokens |
| Vocabulário | 9.882 palavras |
| Dims | 32 (validado empiricamente) |
| Modelo | v17 |
| felix ↔ gato | 0.644 |
| cafe ↔ processamento | 0.777 |
| cozinha ↔ cpu | 0.916 |

### 1.2 Metas

| Meta | Total | Natacha | Carolina | Dims |
|---|---|---|---|---|
| Atual | 121k | 121k | 0 | 32 |
| **Meta 1** | **300k** | **255k** | **45k (15%)** | **testar** |
| **Meta 2** | **500k** | **425k** | **75k (15%)** | **testar** |

**Regra:** 85% curado Natacha + 15% Corpus externo (filtrado e balanceado).

### 1.3 Regra de Ouro

> **Núcleo Natacha nunca abaixo de 40%.**
> Se uma família cresce, ela cresce proporcionalmente às outras.

---

## 2. PRINCÍPIOS DO PLANO

### 2.1 Princípio Central

A personalidade da Natacha mora no **corpus**, não no modelo. O `.json` de embeddings pode ser apagado e treinado novamente. O que leva meses para construir é o corpus curado.

### 2.2 Correções Aplicadas (vs versão anterior)

| # | Correção | Motivo |
|---|---|---|
| 1 | Removida coluna "densidade palavras/dim" | Métrica inválida. Não existe proporção ótima universal |
| 2 | Dims NÃO predeterminadas | Testar 32/40/48 em cada checkpoint |
| 3 | Filtro Carolina temático (não por similaridade) | Evitar viés circular "Natacha só aprende o que já sabe" |
| 4 | Especificar versão Carolina 2.0.1 / Carol·(D+B) | Evitar confusão com Ada 1.2 |
| 5 | Licença: headers CC BY 4.0 + licenças dos textos | Não simplificar como CC BY-NC-SA |
| 6 | Testar 5% → 10% → 15% (não fixar 15%) | Verificar se ganho linguístico vem antes de 15% |
| 7 | Métricas como indicadores históricos | Cosine pode cair por enriquecimento |
| 8 | Removida "saturação skip-gram: alta" | Sem evidência. v17 provou o contrário |
| 9 | RNN = próximo passo DIDÁTICO | Não é emergência por saturação |
| 10 | Nova seção: Música e Interpretação Cultural | 40 músicas geradoras de temas |

---

## 3. O CORPUS EXTERNO

### 3.1 Corpus Carolina

**O que é:** Corpus de português brasileiro contemporâneo (1970-2021), desenvolvido pela USP (LaViHD + C4AI). ~823M tokens na versão Ada 1.2.

**Versão a usar:** **2.0.1** (~2,1M documentos, 15 GB).
**Subcorpus preferido:** **Carol·(D+B)** — 361M tokens, balanceado por domínio, deduplicado.

### 3.2 Licença

- **Headers:** CC BY 4.0
- **Textos:** licenças próprias de cada fonte (devem ser respeitadas individualmente)
- **Uso do projeto:** pesquisa e educação (compatível com TCC)

### 3.3 Estratégia de Filtragem

**Pipeline de 4 etapas (temático, não por similaridade):**

**Etapa 1 — Tipologia**
Incluir: obras literárias (domínio público), jornalismo, wikis, textos gerais.
Excluir: jurídico, legislativo, diário oficial.

**Etapa 2 — Originalidade**
Manter apenas textos com `originality > 85` (mediana do corpus base).

**Etapa 3 — Diversidade por domínio**
Distribuição equilibrada entre: literatura, jornalismo, ciências, história, filosofia, artes.

**Etapa 4 — Tamanho e toxicidade**
- Comprimento mínimo: 50 palavras
- Toxicidade (BERTimbau): < 0.3
- Idioma: Português BR

**Nota:** NÃO filtrar por similaridade com `natacha`. Isso criaria viés circular — rejeitaria textos sobre temas que a Natacha ainda não conhece.

### 3.4 Meta de Filtragem

| Etapa | Textos | Tokens |
|---|---|---|
| Carol·(D+B) bruto | ~400k | 361M |
| Filtro tipologia | ~150k | ~150M |
| Filtro originalidade | ~80k | ~90M |
| Filtro diversidade | ~5.000 | ~50k |

**Taxa de aceitação esperada: ~1.25%.** Menos rigoroso que a versão anterior, mas mais honesto.

---

## 4. PROPORÇÕES POR FAMÍLIA

| Família | % Alvo | Meta 1 (300k) | Meta 2 (500k) |
|---|---|---|---|
| **Núcleo Natacha** | **40%** | 120k | 200k |
| Computação | 20% | 60k | 100k |
| Linguagem/Cultura | 15% | 45k | 75k |
| STEM | 15% | 45k | 75k |
| Mundo Geral | 10% | 30k | 50k |
| **Total** | **100%** | **300k** | **500k** |

**Carolina entra nas famílias Linguagem/Cultura e Mundo Geral** (proporção total ≤ 15%).

---

## 5. CRONOGRAMA

### Meta 1 — 300k tokens

| Fase | Delta | Fonte | Dims |
|---|---|---|---|
| 1.1 | +30k | Curado (Claude + GPT + música) | 32 |
| 1.2 | +30k | Curado (STEM básico) | 32 |
| 1.3 | +30k | Curado (Linguagem/Cultura) | 32 |
| **Checkpoint** | **~211k** | Retreinar, medir | **32** |
| 1.4 | +30k | Curado (Mundo Geral) | 32 |
| 1.5 | +14k | Curado (Núcleo expandido) | 32 |
| **Checkpoint** | **~255k** | Testar dims 32/40/48 | **decisão** |
| 1.6 | **+45k** | **Carolina 15%** | dims vencedoras |
| **Meta 1** | **~300k** | | |

### Experimento de proporção (antes de fixar 15%)

Antes de adicionar os 45k do Carolina, testar 4 níveis:

| Nível | Próprio | Carolina | Total |
|---|---|---|---|
| Controle | 255k | 0 | 255k |
| 5% | 255k | 13k | 268k |
| 10% | 255k | 28k | 283k |
| 15% | 255k | 45k | 300k |

Mesma bateria de avaliação nos 4. Decidir com base no resultado.

### Meta 2 — 500k tokens

| Fase | Delta | Fonte | Dims |
|---|---|---|---|
| 2.1 | +50k | Curado (Núcleo expandido) | testar |
| 2.2 | +50k | Curado (STEM avançado) | testar |
| 2.3 | +50k | Curado (Computação) | testar |
| 2.4 | **+75k** | **Carolina** | testar |
| **Meta 2** | **~500k** | | |

**Tempo realista:** **7-9 meses** (2 horas/dia).

---

## 6. PROGRESSÃO DE DIMS

**Regra:** testar dims apenas em checkpoints (nunca em movimento).

| Corpus | Ação |
|---|---|
| 121k (atual) | ✅ 32d validado |
| 211k | Testar 32 (baseline) |
| **255k** | **Testar 32 / 40 / 48** |
| 300k | Usar vencedor |
| 400k | Manter |
| **500k** | **Testar novamente** |

**Se 32d continuar vencendo, não mudar.** Dims não são destino, são ferramenta.

---

## 7. MÚSICA E INTERPRETAÇÃO CULTURAL

### 7.1 Princípio

**Não usar letras completas.** Cada música gera material original sobre tema, contexto, interpretação e reflexão da Natacha.

### 7.2 Volume por Música

| Tipo | Tokens |
|---|---|
| Tema | ~80 |
| Contexto | ~70 |
| Interpretação | ~80 |
| Reflexão Natacha | ~70 |
| **Total** | **~300** |

### 7.3 Top 40 (Brasileiras + Internacionais)

**Brasileiras (20):**

1. Metamorfose Ambulante — Raul Seixas — identidade em movimento
2. Tempo Perdido — Legião Urbana — juventude e escolhas
3. Infinita Highway — Engenheiros do Hawaii — liberdade e estrada
4. O Alvo — Engenheiros do Hawaii — objetivo e pressão
5. Geração Coca-Cola — Legião Urbana — alienação jovem
6. Ideologia — Cazuza — recusa de rótulos
7. O Tempo Não Para — Cazuza — passagem do tempo
8. Maluco Beleza — Raul Seixas — recusa de conformidade
9. Exagerado — Cazuza — intensidade emocional
10. Até Quando? — Gabriel o Pensador — paciência e revolta
11. Admirável Gado Novo — Zé Ramalho — sociedade de rebanho
12. Comida — Titãs — necessidades humanas vs consumo
13. Polícia — Titãs — autoridade e medo
14. Há Tempos — Legião Urbana — distância e mudança
15. A Via Láctea — Legião Urbana — amor e incompreensão
16. Ouro de Tolo — Raul Seixas — sucesso imposto
17. Pais e Filhos — Legião Urbana — relações e incompreensão
18. Toda Forma de Poder — Engenheiros — autoridade e questionamento
19. Homem Primata — Titãs — sociedade e capitalismo
20. Que País É Este — Legião Urbana — crítica social

**Internacionais (20):**

1. Paranoid Android — Radiohead — alienação
2. Smells Like Teen Spirit — Nirvana — geração perdida
3. Master of Puppets — Metallica — controle e dependência
4. Welcome to the Machine — Pink Floyd — indivíduo vs tecnologia
5. Computer World — Kraftwerk — tecnologia e humanidade
6. No Surprises — Radiohead — rotina e isolamento
7. Personal Jesus — Depeche Mode — fé e consumo
8. Another Brick in the Wall — Pink Floyd — educação e autoridade
9. Creep — Radiohead — inadequação
10. The Man Who Sold the World — Bowie — identidade
11. Byte — Martin Garrix — digital e humano
12. Bullet with Butterfly Wings — Smashing Pumpkins — raiva e frustração
13. Digital Witness — St. Vincent — vigilância
14. People Are Strange — The Doors — estranhamento
15. Closer — Nine Inch Nails — desconexão
16. The Unforgiven — Metallica — identidade vs expectativas
17. Bohemian Rhapsody — Queen — culpa e destino
18. Mad World — Tears for Fears — estranhamento e rotina
19. The Sound of Silence — Simon & Garfunkel — silêncio
20. Boulevard of Broken Dreams — Green Day — solidão

### 7.4 Núcleos Semânticos

| Núcleo | Músicas |
|---|---|
| **Identidade** | Metamorfose Ambulante, Ideologia, The Unforgiven, Creep |
| **Tempo/Existência** | Tempo Perdido, O Tempo Não Para, Há Tempos, Mad World |
| **Sociedade/Poder** | Geração Coca-Cola, Homem Primata, Polícia, Another Brick |
| **Tecnologia/Humano** | Welcome to the Machine, Computer World, Digital Witness, Paranoid Android |
| **Solidão/Relações** | A Via Láctea, No Surprises, People Are Strange, Boulevard |

### 7.5 Volume Total

40 músicas × ~300 tokens = **~12k tokens**

---

## 8. PIPELINE TÉCNICO

### 8.1 Download Carol·(D+B)

```bash
pip install datasets
python3 -c "
from datasets import load_dataset
ds = load_dataset('carolina-c4ai/corpus-carolina', split='train')
print(ds)
"
```

8.2 Script de Filtro Temático

```python
# filtro_carolina.py
from datasets import load_dataset
from collections import Counter

LIMIAR_ORIG = 85
MIN_PALAVRAS = 50
MAX_TOKENS_POR_DOMINIO = 8000

TIPOLOGIAS_ACEITAS = ['literatura', 'jornalismo', 'wiki', 'geral']
TIPOLOGIAS_REJEITADAS = ['juridico', 'legislativo', 'diario_oficial']

def aceitar(ex):
    if ex['originality'] < LIMIAR_ORIG: return False
    if ex['carolina_typology'] in TIPOLOGIAS_REJEITADAS: return False
    if ex['carolina_typology'] not in TIPOLOGIAS_ACEITAS: return False
    if len(ex['text'].split()) < MIN_PALAVRAS: return False
    return True

ds = load_dataset('carolina-c4ai/corpus-carolina', split='train')
por_dominio = Counter()
aceitos = []

for ex in ds:
    if aceitar(ex):
        tipo = ex['carolina_typology']
        if por_dominio[tipo] < MAX_TOKENS_POR_DOMINIO:
            aceitos.append(ex['text'])
            por_dominio[tipo] += len(ex['text'].split())
        if len(aceitos) >= 5000:
            break

with open('carolina_filtrado.txt', 'w') as f:
    for t in aceitos:
        f.write(t + '\n')

print(f"Aceitos: {len(aceitos)}")
print(f"Por dominio: {por_dominio}")
```

8.3 Integração no Corpus

```bash
cd ~/natacha/dados/embeddings
cp corpus.txt corpus_backup_pre_carolina.txt
cat carolina_filtrado.txt >> corpus.txt
wc -m corpus.txt
```

---

9. MÉTRICAS DE CONTROLE

9.1 Indicadores Históricos (não pass/fail)

Par Mínimo histórico Alvo
felix ↔ gato 0.55 0.65
cafe ↔ processamento 0.70 0.80
cozinha ↔ cpu 0.85 0.94
c++ ↔ codigo 0.58 0.68
natacha ↔ felix 0.40 0.50
natacha ↔ rodrigo 0.40 0.52

Nota: cosine pode cair porque o espaço ficou mais rico. Avaliar em conjunto com top-N.

9.2 Três Dimensões de Avaliação

1. Relações do universo Natacha: felix, casa, cafe, iagato
2. Analogias e conhecimento geral: teste com pares de outros domínios
3. Estabilidade do Top-N: top 10 de natacha, felix, cpu, cafe

9.3 Sinal de Saúde do Top de Natacha

Saudável: emoções, verbos humanos, cotidiano
Alerta: pointer, dangling, iterator, aluna, insiste voltando

9.4 Proporção por Família

Família Mínimo
Núcleo Natacha 40%
Computação 15%
Linguagem/Cultura 10%
STEM 10%
Mundo Geral 5%

---

10. RISCOS E MITIGAÇÕES

Risco Probabilidade Impacto Mitigação
Carolina dominar o corpus Média Alto Filtro temático + teto 15%
Diluição do núcleo Natacha Alta Alto Regra dos 40% mínimo
Dims insuficientes Média Médio Testar em cada checkpoint
Fadiga de produção Alta Médio Alternar fontes
Viés circular no filtro Alta (evitado) Alto Filtro temático, não por similaridade

---

11. TRANSIÇÃO PARA RNN/LSTM

Quando: após Meta 2 (500k tokens).

Por quê: RNN/LSTM é o próximo passo didático do roadmap. Introduz dependências sequenciais que embeddings puros não capturam. Não é resposta a saturação — o skip-gram continua funcionando bem em 121k, 32d.

Como: embeddings atuais viram camada de entrada da RNN. Corpus continua o mesmo.

Roadmap técnico:

Fase Arquitetura Marco
3 (atual) Skip-gram 121k → 500k tokens
4 RNN / LSTM 500k → 1M
5 Self-Attention 1M+
6 Transformer 1M+
7 SLM 1M+

---

12. REFERÊNCIAS

Documentos internos

· REQUISITOS.md v3.0
· ARQUITETURA.md v2.0
· PLANO_100K_TOKEN.docx
· PLANO_CORPUS_EXTERNO.md

Corpus Carolina

· USP C4AI. Carolina: a General Corpus of Contemporary Brazilian Portuguese. arXiv:2303.16098.
· Hugging Face: carolina-c4ai/corpus-carolina v2.0.1
· Subcorpus: Carol·(D+B)

Papers

· Mikolov et al. (2013) — Word2Vec
· Vaswani et al. (2017) — Attention Is All You Need
· Finger et al. (2025) — Building Carolina

---

13. NOTA FINAL

"O arquivo .json de embeddings pode ser apagado e treinado novamente. O que vocês passaram meses construindo de verdade é o corpus."

A Natacha não precisa competir com GPT em tamanho. Precisa ser única em sotaque.

Cada token curado vale por mil tokens genéricos.

---

Última atualização: Setembro/2026
Rodrigo Pereira — Projeto Natacha, Mogi Mirim SP
