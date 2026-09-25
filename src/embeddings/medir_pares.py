#!/usr/bin/env python3
import json, math

JSON = "../../dados/embeddings/natacha_embeddings.json"

with open(JSON, encoding="utf-8") as f:
    data = json.load(f)

word2vec = {e["palavra"]: e["vetor"] for e in data["embeddings"]}
print(f"Carregado: {len(word2vec)} palavras, {data['dimensao']} dims\n")

def cosine(a, b):
    dot = sum(x*y for x, y in zip(a, b))
    na = math.sqrt(sum(x*x for x in a))
    nb = math.sqrt(sum(x*x for x in b))
    return dot / (na * nb) if na and nb else 0.0

def sim(w1, w2):
    if w1 not in word2vec: return f"[{w1} ausente]"
    if w2 not in word2vec: return f"[{w2} ausente]"
    return f"{cosine(word2vec[w1], word2vec[w2]):.4f}"

pares = [
    ("felix", "gato"),
    ("felix", "iagato"),
    ("iagato", "gato"),
    ("felix", "padrinho"),
    ("felix", "natacha"),
    ("natacha", "rodrigo"),
    ("natacha", "casa"),
    ("cafe", "processamento"),
    ("c++", "codigo"),
    ("cozinha", "cpu"),
    ("mlp", "neuronio"),
    ("felix", "quintal"),
    ("felix", "teclado"),
]

print("=== SIMILARIDADES DIRETAS ===")
for a, b in pares:
    print(f"  {a:12s} <-> {b:15s} = {sim(a, b)}")

def top(w, n=10):
    if w not in word2vec:
        print(f"\n  [{w} ausente]"); return
    v = word2vec[w]
    scores = [(k, cosine(v, vec)) for k, vec in word2vec.items() if k != w]
    scores.sort(key=lambda x: -x[1])
    print(f"\n  Top {n} de '{w}':")
    for k, s in scores[:n]:
        print(f"    {k:20s} {s:.4f}")

print("\n=== TOPS ===")
for w in ["gato", "iagato", "padrinho", "felix", "natacha", "casa"]:
    top(w)
