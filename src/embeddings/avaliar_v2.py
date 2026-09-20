#!/usr/bin/env python3
"""
avaliar_v2.py — Avaliação completa do modelo Natacha (pipeline JSON)
Gera dados/embeddings/relatorio_avaliacao.md
"""

import json
import math
from pathlib import Path
from datetime import datetime

MODELO = Path("../../dados/embeddings/natacha_embeddings.json")
RELATORIO = Path("../../dados/embeddings/relatorio_avaliacao.md")


# ══════════════════════════════════════════════════════════
#  Carregar modelo
# ══════════════════════════════════════════════════════════

def carregar(caminho):
    with open(caminho, 'r', encoding='utf-8') as f:
        dados = json.load(f)
    emb = {item["palavra"]: item["vetor"] for item in dados["embeddings"]}
    return dados.get("dimensao", 0), emb, dados.get("vocabSize", len(emb))


def cosine(a, b):
    dot = sum(x * y for x, y in zip(a, b))
    na = math.sqrt(sum(x * x for x in a))
    nb = math.sqrt(sum(x * x for x in b))
    return dot / (na * nb) if na * nb > 0 else 0.0


def analogia(a, b, c, emb, n=5):
    exclude = {a, b, c}
    va, vb, vc = emb.get(a), emb.get(b), emb.get(c)
    if not all([va, vb, vc]):
        return []
    target = [vb[d] - va[d] + vc[d] for d in range(len(va))]
    sims = [(w, cosine(target, v)) for w, v in emb.items() if w not in exclude]
    sims.sort(key=lambda x: -x[1])
    return sims[:n]


def top_n(palavra, emb, n=10):
    if palavra not in emb:
        return []
    pv = emb[palavra]
    sims = [(w, cosine(pv, v)) for w, v in emb.items() if w != palavra]
    sims.sort(key=lambda x: -x[1])
    return sims[:n]


# ══════════════════════════════════════════════════════════
#  Listas de teste
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
    ("natacha", "rodrigo", "Natacha ↔ Rodrigo"),
    ("robo", "assistente", "robô ↔ assistente"),
    ("robo", "obedece", "robô ↔ obedece"),
    ("robo", "detesta", "robô ↔ detesta"),
    ("natacha", "felix", "Natacha ↔ Felix"),
]

pares_hardware = [
    ("cpu", "gpu", "CPU ↔ GPU"),
    ("cpu", "ram", "CPU ↔ RAM"),
    ("gpu", "ram", "GPU ↔ RAM"),
]

pares_tecnicos = [
    ("mlp", "neuronio", "MLP ↔ neurônio"),
    ("c++", "codigo", "c++ ↔ código"),
    ("embeddings", "palavras", "embeddings ↔ palavras"),
    ("cafe", "processamento", "café ↔ processamento"),
    ("felix", "gato", "Felix ↔ gato"),
    ("json", "memoria", "json ↔ memória"),
    ("sotao", "quantico", "sótão ↔ quântico"),
]

palavras_top10 = [
    "natacha", "felix", "rodrigo", "cpu", "gpu", "ram",
    "cozinha", "sala", "quarto", "porao", "janela",
    "robo", "casa",
]

analogias_casa = [
    ("cpu", "cozinha", "gpu", "sala"),
    ("ram", "quarto", "cpu", "cozinha"),
    ("gpu", "sala", "ram", "quarto"),
    ("disco", "porao", "ram", "quarto"),
    ("rede", "janela", "gpu", "sala"),
    ("cpu", "cozinha", "disco", "porao"),
    ("gpu", "sala", "disco", "porao"),
    ("cozinha", "cpu", "sala", "gpu"),
    ("quarto", "ram", "porao", "disco"),
    ("janela", "rede", "sala", "gpu"),
]

analogias_personagens = [
    ("felix", "natacha", "robo", "assistente"),
    ("felix", "natacha", "felix", "quintal"),
]


# ══════════════════════════════════════════════════════════
#  Avaliação
# ══════════════════════════════════════════════════════════

def avaliar_pares(emb, pares):
    resultados = []
    for a, b, label in pares:
        if a in emb and b in emb:
            s = cosine(emb[a], emb[b])
            resultados.append((label, s, True))
        else:
            resultados.append((label, 0.0, False))
    return resultados


def avaliar_analogias(emb, analogias):
    resultados = []
    acertos = 0
    for a, b, c, esperado in analogias:
        top = analogia(a, b, c, emb, n=3)
        if not top:
            resultados.append((f"{a}:{b} :: {c}:?", esperado, [], False))
            continue
        acertou = top[0][0] == esperado
        if acertou:
            acertos += 1
        resultados.append((f"{a}:{b} :: {c}:?", esperado, top, acertou))
    return resultados, acertos


def formatar_pares(resultados):
    linhas = ["| Par | Similaridade | Status |", "|-----|--------------|--------|"]
    for label, sim, ok in resultados:
        status = "✅" if sim >= 0.6 else ("⚠️" if sim >= 0.4 else "❌")
        linhas.append(f"| {label} | {sim:.4f} | {status} |")
    return "\n".join(linhas)


def formatar_analogias(resultados):
    linhas = []
    for label, esperado, top, acertou in resultados:
        linhas.append(f"### {label}")
        linhas.append(f"Esperado: **{esperado}**")
        if not top:
            linhas.append("*Palavras não encontradas no vocabulário*\n")
            continue
        for i, (w, s) in enumerate(top, 1):
            mark = " ★" if w == esperado else ""
            linhas.append(f"{i}. `{w}` — sim={s:.4f}{mark}")
        linhas.append("")
    return "\n".join(linhas)


def formatar_top(palavras, emb):
    linhas = []
    for p in palavras:
        if p not in emb:
            continue
        linhas.append(f"### Top 10 similares a `{p}`\n")
        for i, (w, s) in enumerate(top_n(p, emb, 10), 1):
            linhas.append(f"{i}. `{w}` — sim={s:.4f}")
        linhas.append("")
    return "\n".join(linhas)


# ══════════════════════════════════════════════════════════
#  Main
# ══════════════════════════════════════════════════════════

def main():
    if not MODELO.exists():
        print(f"ERRO: {MODELO} não encontrado.")
        return

    print(f"Carregando {MODELO}...")
    dim, emb, vocab = carregar(MODELO)
    print(f"  Vocabulário: {vocab} palavras")
    print(f"  Dimensão: {dim}")

    # Avaliações
    print("Rodando avaliações...")
    pares_casa_res = avaliar_pares(emb, pares_casa)
    pares_pers_res = avaliar_pares(emb, pares_personagens)
    pares_hw_res = avaliar_pares(emb, pares_hardware)
    pares_tec_res = avaliar_pares(emb, pares_tecnicos)

    analogias_casa_res, acertos_casa = avaliar_analogias(emb, analogias_casa)
    analogias_pers_res, acertos_pers = avaliar_analogias(emb, analogias_personagens)

    total_analogias = len(analogias_casa) + len(analogias_personagens)
    total_acertos = acertos_casa + acertos_pers

    # Relatório
    print("Gerando relatório...")
    r = []
    r.append("# Relatório de Avaliação — Natacha Embeddings\n")
    r.append(f"**Data:** {datetime.now().strftime('%d/%m/%Y %H:%M')}\n")
    r.append(f"**Modelo:** `{MODELO}`")
    r.append(f"**Vocabulário:** {vocab} palavras")
    r.append(f"**Dimensão:** {dim}\n")
    r.append("---\n")

    r.append("## 1. Similaridade entre Pares\n")
    r.append("### 1.1 Cômodos da Casa\n")
    r.append(formatar_pares(pares_casa_res))
    r.append("\n### 1.2 Personagens\n")
    r.append(formatar_pares(pares_pers_res))
    r.append("\n### 1.3 Hardware\n")
    r.append(formatar_pares(pares_hw_res))
    r.append("\n### 1.4 Conceitos Técnicos\n")
    r.append(formatar_pares(pares_tec_res))
    r.append("\n")

    r.append("## 2. Analogias Vetoriais\n")
    r.append(f"**Taxa de acerto top-1:** {total_acertos}/{total_analogias} "
             f"({100*total_acertos/total_analogias:.1f}%)\n")
    r.append("### 2.1 Analogias de Casa\n")
    r.append(formatar_analogias(analogias_casa_res))
    r.append("\n### 2.2 Analogias de Personagem\n")
    r.append(formatar_analogias(analogias_pers_res))

    r.append("\n## 3. Top 10 Similares\n")
    r.append(formatar_top(palavras_top10, emb))

    RELATORIO.write_text("\n".join(r), encoding='utf-8')
    print(f"\nRelatório salvo: {RELATORIO}")
    print(f"Taxa de acerto em analogias: {total_acertos}/{total_analogias} "
          f"({100*total_acertos/total_analogias:.1f}%)")


if __name__ == "__main__":
    main()