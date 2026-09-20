#!/usr/bin/env python3
"""analogias_v2.py — Teste de analogias vetoriais no modelo Natacha (JSON)"""

import json
import math
import sys
from pathlib import Path

def carregar_modelo(caminho):
    print(f"Carregando {caminho}...")
    with open(caminho, 'r', encoding='utf-8') as f:
        dados = json.load(f)
    
    dim = dados.get("dimensao", 0)
    emb = {}
    for item in dados.get("embeddings", []):
        emb[item["palavra"]] = item["vetor"]
    
    print(f"  {len(emb)} palavras, {dim} dims")
    return emb


def cosine(a, b):
    dot = sum(x*y for x, y in zip(a, b))
    na = math.sqrt(sum(x*x for x in a))
    nb = math.sqrt(sum(x*x for x in b))
    return dot / (na * nb) if na * nb > 0 else 0.0


def analogia(a, b, c, emb, top_k=3):
    """a:b :: c:? — retorna os top_k candidatos (palavra, score)"""
    va, vb, vc = emb.get(a), emb.get(b), emb.get(c)
    if not all([va, vb, vc]):
        faltando = [w for w, v in [(a, va), (b, vb), (c, vc)] if v is None]
        return [], faltando
    
    target = [vb[d] - va[d] + vc[d] for d in range(len(va))]
    excluir = {a, b, c}
    
    sims = []
    for w, v in emb.items():
        if w in excluir:
            continue
        sims.append((w, cosine(target, v)))
    
    sims.sort(key=lambda x: -x[1])
    return sims[:top_k], []


def main():
    caminho = Path("../../dados/embeddings/natacha_embeddings.json")
    
    if not caminho.exists():
        print(f"ERRO: {caminho} não encontrado.")
        print("Confirme que o treino rodou e salvou o JSON.")
        sys.exit(1)
    
    emb = carregar_modelo(caminho)
    
    analogias = [
        ("cpu", "cozinha", "gpu", "sala", "CPU:cozinha :: GPU:?"),
        ("ram", "quarto", "cpu", "cozinha", "RAM:quarto :: CPU:?"),
        ("gpu", "sala", "ram", "quarto", "GPU:sala :: RAM:?"),
        ("disco", "porao", "ram", "quarto", "disco:porao :: RAM:?"),
        ("rede", "janela", "gpu", "sala", "rede:janela :: GPU:?"),
        ("cpu", "cozinha", "disco", "porao", "CPU:cozinha :: disco:?"),
        ("gpu", "sala", "disco", "porao", "GPU:sala :: disco:?"),
        ("felix", "natacha", "robo", "assistente", "Felix:Natacha :: robo:?"),
        ("felix", "natacha", "felix", "quintal", "Felix:Natacha :: Felix:?"),
        ("cozinha", "cpu", "sala", "gpu", "cozinha:CPU :: sala:?"),
        ("quarto", "ram", "porao", "disco", "quarto:RAM :: porao:?"),
        ("janela", "rede", "sala", "gpu", "janela:rede :: sala:?"),
    ]
    
    print()
    print("=" * 72)
    print("  ANALOGIAS — Resultados Reais")
    print("=" * 72)
    print()
    
    acertos = 0
    total = len(analogias)
    
    for a, b, c, esperado, label in analogias:
        print(f"  {label}")
        print(f"  esperado: {esperado}")
        
        top, faltando = analogia(a, b, c, emb, top_k=3)
        
        if faltando:
            print(f"    PALAVRA NAO ENCONTRADA: {', '.join(faltando)}")
            print()
            continue
        
        if not top:
            print("    (sem resultados)")
            print()
            continue
        
        for i, (w, s) in enumerate(top, 1):
            mark = " ★" if w == esperado else ""
            print(f"    {i}. {w:<20} sim={s:.4f}{mark}")
        
        if top[0][0] == esperado:
            acertos += 1
        
        print()
    
    print("=" * 72)
    print(f"  TOTAL: {acertos}/{total} acertos top-1  ({100*acertos/total:.1f}%)")
    print("=" * 72)


if __name__ == "__main__":
    main()