#!/usr/bin/env python3
"""avaliar_modelos.py — Avaliação comparativa de 5 modelos de embeddings"""

import struct
import math
import sys
import os

# ══════════════════════════════════════════════════════════
#  Leitura de binário
# ══════════════════════════════════════════════════════════

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

def top_n(word, emb, n=10):
    if word not in emb:
        return []
    hv = emb[word]
    sims = [(w, cosine(hv, v)) for w, v in emb.items() if w != word]
    sims.sort(key=lambda x: -x[1])
    return sims[:n]

# ══════════════════════════════════════════════════════════
#  Pares de teste
# ══════════════════════════════════════════════════════════

pares_casa = [
    ("cpu", "cozinha", "CPU → cozinha"),
    ("gpu", "sala", "GPU → sala"),
    ("ram", "quarto", "RAM → quarto"),
    ("disco", "porao", "disco → porão"),
    ("rede", "janela", "rede → janela"),
    ("corredor", "eventos", "corredor → eventos"),
    ("sotao", "quantico", "sótão → quântico"),
    ("felix", "quintal", "Felix → quintal"),
    ("casa", "natacha", "Casa → Natacha"),
]

pares_personagens = [
    ("felix", "natacha", "Felix ↔ Natacha"),
    ("robo", "assistente", "robô ↔ assistente"),
    ("robo", "obedece", "robô ↔ obedece"),
    ("robo", "detesta", "robô ↔ detesta"),
]

pares_hardware = [
    ("cpu", "gpu", "CPU ↔ GPU"),
    ("cpu", "ram", "CPU ↔ RAM"),
    ("gpu", "ram", "GPU ↔ RAM"),
]

palavras_top10 = [
    "natacha", "felix", "rodrigo", "cpu", "gpu", "ram",
    "cozinha", "sala", "quarto", "porao", "janela",
    "robo", "casa"
]

# ══════════════════════════════════════════════════════════
#  Main
# ══════════════════════════════════════════════════════════

def main():
    epocas = [30, 50, 100, 150, 200]
    arquivos = {e: f"dados/embeddings/vetores_{e}ep.bin" for e in epocas}

    print("Carregando modelos...")
    modelos = {}
    for e in epocas:
        arq = arquivos[e]
        if os.path.exists(arq):
            d, emb = read_bin(arq)
            modelos[e] = emb
            print(f"  {e:>3} epocas: {len(emb):>5} palavras, {d} dims")
        else:
            print(f"  {e:>3} epocas: ARQUIVO NÃO ENCONTRADO")

    ep_c = [e for e in epocas if e in modelos]
    if len(ep_c) < 2:
        print("Precisa de pelo menos 2 modelos para comparar.")
        return

    header = f"{'Relação':<28}"
    for e in ep_c:
        header += f" {e:>5}ep"
    header += f"  {'Δ 30→100':>9} {'Δ 30→200':>9}"
    print("\n" + "="*len(header))
    print("  EVOLUÇÃO POR ÉPOCA")
    print("="*len(header))

    # ═══ TABELA 1: Arquitetura da Casa ═══
    print(f"\n{'─ ARQUITETURA DA CASA ─':^{len(header)}}")
    print(header)
    print("─"*len(header))

    for a, b, label in pares_casa:
        row = f"  {label:<26}"
        scores = {}
        for e in ep_c:
            if a in modelos[e] and b in modelos[e]:
                s = cosine(modelos[e][a], modelos[e][b])
                scores[e] = s
                row += f" {s:>7.4f}"
            else:
                row += f" {'---':>7}"

        d30_100 = ""
        d30_200 = ""
        if 30 in scores and 100 in scores:
            d = scores[100] - scores[30]
            d30_100 = f" {d:>+7.4f}"
        if 30 in scores and 200 in scores:
            d = scores[200] - scores[30]
            d30_200 = f" {d:>+7.4f}"

        row += f" {d30_100:>9} {d30_200:>9}"
        print(row)

    # ═══ TABELA 2: Personagens ═══
    print(f"\n{'─ PERSONAGENS ─':^{len(header)}}")
    print(header)
    print("─"*len(header))

    for a, b, label in pares_personagens:
        row = f"  {label:<26}"
        scores = {}
        for e in ep_c:
            if a in modelos[e] and b in modelos[e]:
                s = cosine(modelos[e][a], modelos[e][b])
                scores[e] = s
                row += f" {s:>7.4f}"
            else:
                row += f" {'---':>7}"

        d30_100 = ""
        d30_200 = ""
        if 30 in scores and 100 in scores:
            d = scores[100] - scores[30]
            d30_100 = f" {d:>+7.4f}"
        if 30 in scores and 200 in scores:
            d = scores[200] - scores[30]
            d30_200 = f" {d:>+7.4f}"

        row += f" {d30_100:>9} {d30_200:>9}"
        print(row)

    # ═══ TABELA 3: Hardware ═══
    print(f"\n{'─ HARDWARE ─':^{len(header)}}")
    print(header)
    print("─"*len(header))

    for a, b, label in pares_hardware:
        row = f"  {label:<26}"
        scores = {}
        for e in ep_c:
            if a in modelos[e] and b in modelos[e]:
                s = cosine(modelos[e][a], modelos[e][b])
                scores[e] = s
                row += f" {s:>7.4f}"
            else:
                row += f" {'---':>7}"

        d30_100 = ""
        d30_200 = ""
        if 30 in scores and 100 in scores:
            d = scores[100] - scores[30]
            d30_100 = f" {d:>+7.4f}"
        if 30 in scores and 200 in scores:
            d = scores[200] - scores[30]
            d30_200 = f" {d:>+7.4f}"

        row += f" {d30_100:>9} {d30_200:>9}"
        print(row)

    # ═══ TOP 10 VIZINHOS ═══
    print("\n" + "="*72)
    print("  TOP 10 VIZINHOS — EVOLUÇÃO")
    print("="*72)

    for word in palavras_top10:
        print(f"\n  ┌─ {word.upper()} {'─'*(50-len(word))}")

        # Header with all models
        h = f"  │ {'#':<3}"
        for e in ep_c:
            h += f" {e}ep{'':>12}"
        print(h)
        print(f"  │ {'─'*65}")

        tops = {e: top_n(word, modelos[e], 10) for e in ep_c}

        for i in range(10):
            row = f"  │ {i+1:<3}"
            for e in ep_c:
                if i < len(tops[e]):
                    w, s = tops[e][i]
                    row += f" {w:<12} {s:.3f}  "
                else:
                    row += f" {'---':<12} {'':>6}  "
            print(row)

        print(f"  └{'─'*68}")

    # ═══ RESUMO GERAL ═══
    print("\n" + "="*72)
    print("  RESUMO: MÉDIA DOS SCORES POR ÉPOCA")
    print("="*72)

    for e in ep_c:
        todos = []
        for a, b, _ in pares_casa + pares_personagens + pares_hardware:
            if a in modelos[e] and b in modelos[e]:
                todos.append(cosine(modelos[e][a], modelos[e][b]))
        if todos:
            avg = sum(todos) / len(todos)
            print(f"  {e:>3} épocas: média = {avg:.4f} ({len(todos)} pares)")

    # Melhor época por par
    print("\n  MELHOR ÉPOCA POR PAR:")
    for a, b, label in pares_casa + pares_personagens + pares_hardware:
        best_e = None
        best_s = -1
        for e in ep_c:
            if a in modelos[e] and b in modelos[e]:
                s = cosine(modelos[e][a], modelos[e][b])
                if s > best_s:
                    best_s = s
                    best_e = e
        if best_e:
            print(f"  {label:<28} → {best_e}ep ({best_s:.4f})")

if __name__ == "__main__":
    main()
