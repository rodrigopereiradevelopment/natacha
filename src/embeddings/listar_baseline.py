#!/usr/bin/env python3
"""
Lista os 200 pares aleatórios com cosseno v1/v2 e delta.
Ordenado por delta (do maior ganho ao maior perda).

Uso: python3 listar_baseline.py <v1.json> <v2.json> [n=200]
"""

import json
import math
import random
import sys

def carregar(caminho):
    with open(caminho, encoding='utf-8') as f:
        data = json.load(f)
    return {e['palavra']: e['vetor'] for e in data['embeddings']}

def cosine(a, b):
    dot = sum(x*y for x, y in zip(a, b))
    na = math.sqrt(sum(x*x for x in a))
    nb = math.sqrt(sum(x*x for x in b))
    return dot / (na * nb) if na and nb else 0.0

def listar(v1, v2, n=200, seed=42):
    random.seed(seed)
    
    vocab = sorted(set(v1.keys()) & set(v2.keys()))
    pares = set()
    while len(pares) < n:
        a = random.choice(vocab)
        b = random.choice(vocab)
        if a != b:
            par = tuple(sorted([a, b]))
            pares.add(par)
    pares = list(pares)
    
    # Calcula cosseno pra cada par
    resultados = []
    for a, b in pares:
        c1 = cosine(v1[a], v1[b])
        c2 = cosine(v2[a], v2[b])
        resultados.append((a, b, c1, c2, c2 - c1))
    
    # Ordena por delta decrescente
    resultados.sort(key=lambda x: -x[4])
    
    print(f"=== 200 PARES ALEATÓRIOS (ordenado por delta) ===\n")
    print(f"{'#':<4} {'palavra A':<20} {'palavra B':<20} {'v21':>8} {'v22':>8} {'delta':>9}")
    print("-" * 78)
    
    for i, (a, b, c1, c2, d) in enumerate(resultados, 1):
        marker = ""
        if d > 0.05: marker = " 🚀"
        elif d > 0.02: marker = " ✅"
        elif d < -0.02: marker = " ⚠️"
        print(f"{i:<4} {a:<20} {b:<20} {c1:>8.4f} {c2:>8.4f} {d:>+9.4f}{marker}")
    
    # Resumo
    print()
    print(f"=== RESUMO ===")
    ganhos = [d for _, _, _, _, d in resultados if d > 0]
    perdas = [d for _, _, _, _, d in resultados if d < 0]
    
    print(f"Pares que subiram: {len(ganhos)} ({len(ganhos)*100//n}%)")
    print(f"Pares que caíram:  {len(perdas)} ({len(perdas)*100//n}%)")
    print(f"Delta médio positivo: +{sum(ganhos)/len(ganhos):.4f}" if ganhos else "")
    print(f"Delta médio negativo: {sum(perdas)/len(perdas):.4f}" if perdas else "")
    
    # Quantos >0.03 (sinal real, não deriva)
    reais = [d for _, _, _, _, d in resultados if d > 0.03]
    print(f"\nPares com delta > +0.03 (sinal real): {len(reais)}")
    print(f"Pares com delta < -0.03 (perda real): {len([d for d in perdas if d < -0.03])}")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Uso: python3 listar_baseline.py v1.json v2.json [n]")
        sys.exit(1)
    
    n = int(sys.argv[3]) if len(sys.argv) > 3 else 200
    v1 = carregar(sys.argv[1])
    v2 = carregar(sys.argv[2])
    listar(v1, v2, n)

    # Mostra primeiros 50 (se n > 50)
    if n > 50:
        print("\n\n=== PRIMEIROS 50 JÁ LISTADOS ACIMA ===")
