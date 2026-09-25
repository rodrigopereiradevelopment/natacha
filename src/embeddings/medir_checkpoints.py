#!/usr/bin/env python3
import json, math, sys, glob, os

def carregar(caminho):
    with open(caminho, encoding="utf-8") as f:
        data = json.load(f)
    return {e["palavra"]: e["vetor"] for e in data["embeddings"]}

def cosine(a, b):
    dot = sum(x*y for x, y in zip(a, b))
    na = math.sqrt(sum(x*x for x in a))
    nb = math.sqrt(sum(x*x for x in b))
    return dot / (na * nb) if na and nb else 0.0

def sim(w2v, a, b):
    if a not in w2v or b not in w2v: return None
    return cosine(w2v[a], w2v[b])

def top(w2v, alvo, n=8):
    if alvo not in w2v: return []
    v = w2v[alvo]
    sims = [(k, cosine(v, vec)) for k, vec in w2v.items() if k != alvo]
    sims.sort(key=lambda x: -x[1])
    return sims[:n]

# Localiza os arquivos
base = "../../dados/embeddings"
arquivos = sorted(glob.glob(f"{base}/natacha_embeddings_*ep.json"))

if not arquivos:
    print("Nenhum checkpoint encontrado em", base)
    sys.exit(1)

pares = [
    ("felix", "gato"),
    ("c++", "codigo"),
    ("cafe", "processamento"),
    ("cozinha", "cpu"),
    ("natacha", "felix"),
    ("natacha", "rodrigo"),
    ("mlp", "neuronio"),
]

print(f"Analisando {len(arquivos)} checkpoints\n")
print("=" * 90)
print(f"{'Checkpoint':<20}", end="")
for a, b in pares:
    print(f"{a}↔{b:<6}", end="  ")
print()
print("=" * 90)

resultados = []
for arq in arquivos:
    nome = os.path.basename(arq).replace("natacha_embeddings_", "").replace(".json", "")
    w2v = carregar(arq)
    linha = [nome]
    for a, b in pares:
        s = sim(w2v, a, b)
        linha.append(f"{s:.4f}" if s else "N/A")
    resultados.append((nome, w2v, linha))
    print(f"{nome:<20}", end="")
    for v in linha[1:]:
        print(f"{v:<13}", end="  ")
    print()

print("=" * 90)

# Top de natacha em cada checkpoint
print("\n=== TOP 10 DE 'natacha' POR CHECKPOINT ===\n")
for nome, w2v, _ in resultados:
    print(f"--- {nome} ---")
    for pal, s in top(w2v, "natacha", 10):
        print(f"  {pal:<20} {s:.4f}")
    print()

# Top de felix em cada checkpoint
print("\n=== TOP 8 DE 'felix' POR CHECKPOINT ===\n")
for nome, w2v, _ in resultados:
    print(f"--- {nome} ---")
    for pal, s in top(w2v, "felix", 8):
        print(f"  {pal:<20} {s:.4f}")
    print()
