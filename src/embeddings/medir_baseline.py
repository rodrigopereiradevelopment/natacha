#!/usr/bin/env python3
"""
Mede a deriva geral do espaço vetorial entre dois embeddings.
Compara cosseno médio de 200 pares aleatórios.

Uso: python3 medir_baseline.py <v21.json> <v22.json>
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

def baseline(v1, v2, n=200, seed=42):
    random.seed(seed)
    
    # Palavras em comum
    vocab = sorted(set(v1.keys()) & set(v2.keys()))
    print(f"Vocabulário comum: {len(vocab)} palavras")
    
    # Sorteia 200 pares únicos (não importa ordem)
    pares = set()
    while len(pares) < n:
        a = random.choice(vocab)
        b = random.choice(vocab)
        if a != b:
            par = tuple(sorted([a, b]))
            pares.add(par)
    pares = list(pares)
    
    # Calcula cosseno médio
    soma_v1 = 0.0
    soma_v2 = 0.0
    for a, b in pares:
        soma_v1 += cosine(v1[a], v1[b])
        soma_v2 += cosine(v2[a], v2[b])
    
    media_v1 = soma_v1 / n
    media_v2 = soma_v2 / n
    deriva = media_v2 - media_v1
    
    print()
    print(f"=== BASELINE ALEATÓRIO ({n} pares) ===")
    print(f"  Média cosseno v1:  {media_v1:.4f}")
    print(f"  Média cosseno v2:  {media_v2:.4f}")
    print(f"  Deriva geral:      {deriva:+.4f}")
    print()
    
    if abs(deriva) < 0.01:
        print("  ✅ Deriva < 0.01 — ganhos da tabela são REAIS")
    elif abs(deriva) < 0.03:
        print("  ⚠️  Deriva 0.01-0.03 — só ganhos >0.03 são REAIS")
    else:
        print("  ❌ Deriva > 0.03 — maioria dos ganhos é ARTEFATO")
    
    return deriva

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Uso: python3 medir_baseline.py v1.json v2.json")
        sys.exit(1)
    
    print(f"Carregando v1: {sys.argv[1]}")
    v1 = carregar(sys.argv[1])
    print(f"Carregando v2: {sys.argv[2]}")
    v2 = carregar(sys.argv[2])
    
    baseline(v1, v2)
