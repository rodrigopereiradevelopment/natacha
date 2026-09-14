#!/usr/bin/env python3
"""analogias.py — Teste de analogias vetoriais (5 modelos)"""

import struct
import math
import os

def read_bin(path):
    with open(path, 'rb') as f:
        dim = struct.unpack('i', f.read(4))[0]
        data = {}
        while True:
            tb = f.read(4)
            if not tb or len(tb) < 4:
                break
            tam = struct.unpack('I', tb)[0]
            raw = f.read(tam)
            raw_v = f.read(dim * 4)
            if len(raw_v) < dim * 4:
                break
            word = raw.decode('utf-8', errors='replace')
            data[word] = list(struct.unpack(f'{dim}f', raw_v))
    return dim, data

def cosine(a, b):
    dot = sum(x*y for x,y in zip(a,b))
    na = math.sqrt(sum(x*x for x in a))
    nb = math.sqrt(sum(x*x for x in b))
    return dot/(na*nb) if na*nb > 0 else 0

def analogy(a, b, c, emb, exclude=None):
    if exclude is None:
        exclude = set()
    exclude.update([a, b, c])
    va, vb, vc = emb.get(a), emb.get(b), emb.get(c)
    if not all([va, vb, vc]):
        return []
    target = [vb[d] - va[d] + vc[d] for d in range(len(va))]
    sims = [(w, cosine(target, v)) for w, v in emb.items() if w not in exclude]
    sims.sort(key=lambda x: -x[1])
    return sims[:10]

def main():
    epocas = [30, 50, 100, 150, 200]
    print("Carregando modelos...")
    modelos = {}
    for e in epocas:
        arq = f"dados/embeddings/vetores_{e}ep.bin"
        if os.path.exists(arq):
            d, emb = read_bin(arq)
            modelos[e] = emb
            print(f"  {e:>3} ep: {len(emb)} palavras, {d} dims")

    analogias = [
        ("cpu", "cozinha", "gpu", "sala", "CPU:cozinha :: GPU:? → sala"),
        ("ram", "quarto", "cpu", "cozinha", "RAM:quarto :: CPU:? → cozinha"),
        ("gpu", "sala", "ram", "quarto", "GPU:sala :: RAM:? → quarto"),
        ("disco", "porao", "ram", "quarto", "disco:porao :: RAM:? → quarto"),
        ("rede", "janela", "gpu", "sala", "rede:janela :: GPU:? → sala"),
        ("cpu", "cozinha", "disco", "porao", "CPU:cozinha :: disco:? → porao"),
        ("gpu", "sala", "disco", "porao", "GPU:sala :: disco:? → porao"),
        ("felix", "natacha", "robo", "assistente", "Felix:Natacha :: robô:? → assistente"),
        ("felix", "natacha", "felix", "quintal", "Felix:Natacha :: Felix:? → quintal"),
        ("cozinha", "cpu", "sala", "gpu", "cozinha:CPU :: sala:? → GPU"),
        ("quarto", "ram", "porao", "disco", "quarto:RAM :: porao:? → disco"),
        ("janela", "rede", "sala", "gpu", "janela:rede :: sala:? → GPU"),
    ]

    # Tabela resumo
    header = f"{'Analogia':<38}"
    for e in epocas:
        if e in modelos:
            header += f" {e:>4}ep"
    print(f"\n{'='*len(header)}")
    print("  ACERTOS TOP-1 POR ÉPOCA")
    print(f"{'='*len(header)}")
    print(header)
    print("─"*len(header))

    acertos = {e: 0 for e in epocas if e in modelos}
    total = len(analogias)

    for a, b, c, esperado, label in analogias:
        row = f"  {label:<36}"
        for e in epocas:
            if e in modelos:
                top = analogy(a, b, c, modelos[e])
                w1 = top[0][0] if top else "---"
                hit = w1 == esperado
                if hit:
                    acertos[e] += 1
                mark = "✓" if hit else "✗"
                row += f"   {mark:>3}"
        print(row)

    print("─"*len(header))
    row = f"  {'TOTAL':<36}"
    for e in epocas:
        if e in modelos:
            row += f"  {acertos[e]:>2}/{total}"
    print(row)

    # Detalhes por analogia
    print(f"\n{'='*72}")
    print("  DETALHES — TOP-3 POR ANALOGIA")
    print(f"{'='*72}")

    for a, b, c, esperado, label in analogias:
        print(f"\n  {label}")
        print(f"  (esperado: {esperado})")

        for e in epocas:
            if e not in modelos:
                continue
            top = analogy(a, b, c, modelos[e])
            top3 = top[:3]
            items = []
            for w, s in top3:
                mark = "★" if w == esperado else " "
                items.append(f"{w}({s:.3f}){mark}")
            print(f"    {e:>3}ep: {',  '.join(items)}")

if __name__ == "__main__":
    main()
