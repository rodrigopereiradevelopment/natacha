#!/usr/bin/env python3
"""
avaliar_completo.py — Avaliação ampliada de embeddings
Gera dados/embeddings/relatorio_avaliacao.md
"""

import struct
import math
import os
import datetime

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
    dot = sum(x * y for x, y in zip(a, b))
    na = math.sqrt(sum(x * x for x in a))
    nb = math.sqrt(sum(x * x for x in b))
    return dot / (na * nb) if na * nb > 0 else 0.0

def analogy_topN(a, b, c, emb, n=5):
    """a : b :: c : ?  →  b - a + c"""
    exclude = {a, b, c}
    va, vb, vc = emb.get(a), emb.get(b), emb.get(c)
    if not all([va, vb, vc]):
        return []
    target = [vb[d] - va[d] + vc[d] for d in range(len(va))]
    sims = [(w, cosine(target, v)) for w, v in emb.items() if w not in exclude]
    sims.sort(key=lambda x: -x[1])
    return sims[:n]

# ══════════════════════════════════════════════════════════
#  Pares para cosine
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

# ══════════════════════════════════════════════════════════
#  Analogias — 24 total, 4 categorias
# ══════════════════════════════════════════════════════════

analogias_casa = [
    ("cpu", "cozinha", "gpu", "sala", "CPU:cozinha :: GPU:? → sala"),
    ("ram", "quarto", "cpu", "cozinha", "RAM:quarto :: CPU:? → cozinha"),
    ("gpu", "sala", "ram", "quarto", "GPU:sala :: RAM:? → quarto"),
    ("disco", "porao", "ram", "quarto", "disco:porão :: RAM:? → quarto"),
    ("cpu", "cozinha", "disco", "porao", "CPU:cozinha :: disco:? → porão"),
    ("gpu", "sala", "disco", "porao", "GPU:sala :: disco:? → porão"),
    ("rede", "janela", "gpu", "sala", "rede:janela :: GPU:? → sala"),
    ("corredor", "eventos", "rede", "janela", "corredor:eventos :: rede:? → janela"),
]

analogias_hardware = [
    ("cozinha", "cpu", "sala", "gpu", "cozinha:CPU :: sala:? → GPU"),
    ("quarto", "ram", "porao", "disco", "quarto:RAM :: porão:? → disco"),
    ("janela", "rede", "sala", "gpu", "janela:rede :: sala:? → GPU"),
    ("porao", "disco", "quarto", "ram", "porão:disco :: quarto:? → RAM"),
    ("sala", "gpu", "cozinha", "cpu", "sala:GPU :: cozinha:? → CPU"),
    ("cozinha", "cpu", "porao", "disco", "cozinha:CPU :: porão:? → disco"),
]

analogias_personagens = [
    ("felix", "natacha", "robo", "assistente", "Felix:Natacha :: robô:? → assistente"),
    ("felix", "natacha", "felix", "quintal", "Felix:Natacha :: Felix:? → quintal"),
    ("rodrigo", "natacha", "felix", "quintal", "Rodrigo:Natacha :: Felix:? → quintal"),
    ("natacha", "felix", "robo", "obedece", "Natacha:Felix :: robô:? → obedece"),
]

analogias_processamento = [
    ("processa", "cpu", "guarda", "ram", "processa:CPU :: guarda:? → RAM"),
    ("guarda", "ram", "processa", "cpu", "guarda:RAM :: processa:? → CPU"),
    ("volatil", "ram", "permanente", "disco", "volátil:RAM :: permanente:? → disco"),
    ("processa", "cpu", "exibe", "gpu", "processa:CPU :: exibe:? → GPU"),
    ("memoria", "ram", "armazenamento", "disco", "memória:RAM :: armazenamento:? → disco"),
    ("rapido", "cpu", "paralelo", "gpu", "rápido:CPU :: paralelo:? → GPU"),
]

TODAS_ANALOGIAS = {
    "Casa": analogias_casa,
    "Hardware": analogias_hardware,
    "Personagens": analogias_personagens,
    "Processamento": analogias_processamento,
}

# ══════════════════════════════════════════════════════════
#  Funções de avaliação
# ══════════════════════════════════════════════════════════

def avaliar_cosine(modelos, epocas):
    resultados = {}
    for e in epocas:
        emb = modelos[e]
        scores_casa = []
        for a, b, _ in pares_casa:
            if a in emb and b in emb:
                scores_casa.append(cosine(emb[a], emb[b]))

        scores_pers = []
        for a, b, _ in pares_personagens:
            if a in emb and b in emb:
                scores_pers.append(cosine(emb[a], emb[b]))

        scores_hw = []
        for a, b, _ in pares_hardware:
            if a in emb and b in emb:
                scores_hw.append(cosine(emb[a], emb[b]))

        todos = scores_casa + scores_pers + scores_hw
        acima_05 = sum(1 for s in todos if s > 0.5)

        resultados[e] = {
            "casa_avg": sum(scores_casa) / len(scores_casa) if scores_casa else 0,
            "pers_avg": sum(scores_pers) / len(scores_pers) if scores_pers else 0,
            "hw_avg": sum(scores_hw) / len(scores_hw) if scores_hw else 0,
            "geral_avg": sum(todos) / len(todos) if todos else 0,
            "acima_05": acima_05,
            "total_pares": len(todos),
        }
    return resultados

def avaliar_analogias(modelos, epocas):
    resultados = {}
    for e in epocas:
        emb = modelos[e]
        cat_resultados = {}
        geral_acertos = 0
        geral_total = 0
        geral_posicoes = []

        for cat, lista in TODAS_ANALOGIAS.items():
            cat_acertos = 0
            cat_total = len(lista)
            cat_posicoes = []

            for a, b, c, esperado, label in lista:
                top = analogy_topN(a, b, c, emb, n=5)
                pos = None
                for i, (w, s) in enumerate(top):
                    if w == esperado:
                        pos = i + 1
                        break

                if pos is not None:
                    cat_posicoes.append(pos)
                    geral_posicoes.append(pos)
                    if pos == 1:
                        cat_acertos += 1
                        geral_acertos += 1
                else:
                    cat_posicoes.append(6)
                    geral_posicoes.append(6)

                geral_total += 1

            media_pos = sum(cat_posicoes) / len(cat_posicoes) if cat_posicoes else 0
            cat_resultados[cat] = {
                "acertos_top1": cat_acertos,
                "total": cat_total,
                "posicoes": cat_posicoes,
                "media_posicao": media_pos,
            }

        geral_media_pos = sum(geral_posicoes) / len(geral_posicoes) if geral_posicoes else 0
        cat_resultados["Geral"] = {
            "acertos_top1": geral_acertos,
            "total": geral_total,
            "posicoes": geral_posicoes,
            "media_posicao": geral_media_pos,
        }
        resultados[e] = cat_resultados
    return resultados

# ══════════════════════════════════════════════════════════
#  Geração do relatório
# ══════════════════════════════════════════════════════════

def gerar_relatorio(modelos, epocas, cosine_res, analogy_res):
    S = []

    def L(s=""):
        S.append(s)

    L("# Relatório de Avaliação de Embeddings")
    L(f"\nGerado em: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    L(f"Modelos avaliados: {', '.join(str(e)+'ep' for e in epocas)}")
    L(f"Dimensão: 300 | Vocabulário: {len(modelos[epocas[0]])} palavras")
    L("")

    # ── SEÇÃO 1: Tabela geral por modelo ──
    L("## 1. Visão Geral por Modelo\n")
    L("| Épocas | Média Cosine | Pares > 0.5 | Analogias Top-1 | Média Posição |")
    L("|--------|-------------|-------------|-----------------|---------------|")
    for e in epocas:
        c = cosine_res[e]
        a = analogy_res[e]["Geral"]
        acc_pct = a['acertos_top1'] / a['total'] * 100 if a['total'] > 0 else 0
        L(f"| {e} | {c['geral_avg']:.4f} | {c['acima_05']}/{c['total_pares']} | "
          f"{a['acertos_top1']}/{a['total']} ({acc_pct:.0f}%) | "
          f"{a['media_posicao']:.2f} |")
    L("")

    # ── SEÇÃO 2: Cosine por categoria ──
    L("## 2. Cosine Similarity por Categoria\n")

    L("### 2.1 Arquitetura da Casa\n")
    header = "| Relação |" + "".join(f" {e}ep |" for e in epocas)
    L(header)
    sep = "|---------|" + "".join("--------|" for _ in epocas)
    L(sep)
    for a, b, label in pares_casa:
        row = f"| {label} |"
        for e in epocas:
            emb = modelos[e]
            if a in emb and b in emb:
                row += f" {cosine(emb[a], emb[b]):.4f} |"
            else:
                row += " --- |"
        L(row)

    L("\n### 2.2 Personagens\n")
    header = "| Relação |" + "".join(f" {e}ep |" for e in epocas)
    L(header)
    sep = "|---------|" + "".join("--------|" for _ in epocas)
    L(sep)
    for a, b, label in pares_personagens:
        row = f"| {label} |"
        for e in epocas:
            emb = modelos[e]
            if a in emb and b in emb:
                row += f" {cosine(emb[a], emb[b]):.4f} |"
            else:
                row += " --- |"
        L(row)

    L("\n### 2.3 Hardware\n")
    header = "| Relação |" + "".join(f" {e}ep |" for e in epocas)
    L(header)
    sep = "|---------|" + "".join("--------|" for _ in epocas)
    L(sep)
    for a, b, label in pares_hardware:
        row = f"| {label} |"
        for e in epocas:
            emb = modelos[e]
            if a in emb and b in emb:
                row += f" {cosine(emb[a], emb[b]):.4f} |"
            else:
                row += " --- |"
        L(row)

    L("\n### 2.4 Média por Categoria\n")
    L("| Épocas | Casa | Personagens | Hardware | Geral |")
    L("|--------|------|-------------|----------|-------|")
    for e in epocas:
        c = cosine_res[e]
        L(f"| {e} | {c['casa_avg']:.4f} | {c['pers_avg']:.4f} | {c['hw_avg']:.4f} | {c['geral_avg']:.4f} |")
    L("")

    # ── SEÇÃO 3: Analogias ──
    L("## 3. Analogias Vetoriais\n")
    L("Formato: `a : b :: c : ?` — resultado = `b - a + c`")
    L(f"Excluídas da busca: A, B e C")
    L(f"Top-5 candidatos exibidos\n")

    cat_idx = 0
    for cat, lista in TODAS_ANALOGIAS.items():
        cat_idx += 1
        L(f"### 3.{cat_idx} {cat}\n")

        for a, b, c, esperado, label in lista:
            L(f"**{label}** (esperado: `{esperado}`)\n")
            header = "| # |" + "".join(f" {e}ep |" for e in epocas)
            L(header)
            sep = "|---|" + "".join("-------|" for _ in epocas)
            L(sep)

            for rank in range(5):
                row = f"| {rank+1} |"
                for e in epocas:
                    top = analogy_topN(a, b, c, modelos[e], n=5)
                    if rank < len(top):
                        w, s = top[rank]
                        mark = " ✓" if w == esperado else ""
                        row += f" {w} ({s:.3f}){mark} |"
                    else:
                        row += " — |"
                L(row)
            L("")

        L(f"**Resumo — {cat}:**\n")
        L("| Épocas | Acertos Top-1 | Média Posição |")
        L("|--------|---------------|---------------|")
        for e in epocas:
            r = analogy_res[e][cat]
            L(f"| {e} | {r['acertos_top1']}/{r['total']} | {r['media_posicao']:.2f} |")
        L("")

    # ── SEÇÃO 4: Resumo por modelo ──
    L("## 4. Resumo por Modelo\n")
    for e in epocas:
        c = cosine_res[e]
        a = analogy_res[e]
        L(f"### {e} Épocas\n")
        L(f"- **Média cosine geral:** {c['geral_avg']:.4f}")
        L(f"- **Pares acima de 0.5:** {c['acima_05']}/{c['total_pares']}")
        L(f"- **Analogias acertadas (top-1):** {a['Geral']['acertos_top1']}/{a['Geral']['total']}")
        L(f"- **Média posição do resultado esperado:** {a['Geral']['media_posicao']:.2f}")
        L(f"- **Por categoria:**")
        for cat in ["Casa", "Hardware", "Personagens", "Processamento"]:
            if cat in a:
                r = a[cat]
                L(f"  - {cat}: {r['acertos_top1']}/{r['total']} top-1, posição média {r['media_posicao']:.2f}")
        L("")

    # ── SEÇÃO 5: Análise comparativa ──
    L("## 5. Análise Comparativa\n")
    L("### Curva de cosine por época\n")
    L("```")
    max_avg = max(cosine_res[e]["geral_avg"] for e in epocas)
    for e in epocas:
        avg = cosine_res[e]["geral_avg"]
        bar_len = int((avg / max_avg) * 50) if max_avg > 0 else 0
        L(f"  {e:>3}ep |{'█' * bar_len}{'░' * (50 - bar_len)}| {avg:.4f}")
    L("```\n")

    L("### Curva de analogias (top-1) por época\n")
    L("```")
    for e in epocas:
        r = analogy_res[e]["Geral"]
        pct = r["acertos_top1"] / r["total"] if r["total"] > 0 else 0
        bar_len = int(pct * 50)
        L(f"  {e:>3}ep |{'█' * bar_len}{'░' * (50 - bar_len)}| {r['acertos_top1']}/{r['total']} ({pct*100:.0f}%)")
    L("```\n")

    L("### Curva de posição média por época (menor = melhor)\n")
    L("```")
    pos_vals = [analogy_res[e]["Geral"]["media_posicao"] for e in epocas]
    min_pos = min(pos_vals)
    max_pos = max(pos_vals)
    for e in epocas:
        pos = analogy_res[e]["Geral"]["media_posicao"]
        if max_pos > min_pos:
            bar_len = int(((max_pos - pos) / (max_pos - min_pos)) * 50)
        else:
            bar_len = 25
        L(f"  {e:>3}ep |{'█' * bar_len}{'░' * (50 - bar_len)}| {pos:.2f}")
    L("```\n")

    # ── SEÇÃO 6: Observações ──
    L("## 6. Observações\n")
    L("- Este relatório é informativo. Nenhum modelo foi classificado automaticamente como overfitting.")
    L("- **Cosine similarity** mede proximidade entre pares de palavras.")
    L("- **Analogias** medem geometria relacional (b - a + c).")
    L("- **Média posição** indica onde o resultado esperado aparece no top-5 (1.0 = sempre primeiro, 6.0 = nunca encontrado).")
    L("- Valores de posição > 5.0 indicam que o resultado esperado não estava no top-5.")
    L("")

    return "\n".join(S)

# ══════════════════════════════════════════════════════════
#  Main
# ══════════════════════════════════════════════════════════

def main():
    epocas = [30, 50, 100, 150, 200]

    print("Carregando modelos...")
    modelos = {}
    for e in epocas:
        arq = f"dados/embeddings/vetores_{e}ep.bin"
        if os.path.exists(arq):
            d, emb = read_bin(arq)
            modelos[e] = emb
            print(f"  {e:>3}ep: {len(emb)} palavras, {d} dims")
        else:
            print(f"  {e:>3}ep: ARQUIVO NÃO ENCONTRADO — {arq}")

    ep_c = [e for e in epocas if e in modelos]
    if len(ep_c) < 2:
        print("Precisa de pelo menos 2 modelos.")
        return

    print("\nAvaliando cosine similarity...")
    cosine_res = avaliar_cosine(modelos, ep_c)

    print("Avaliando analogias...")
    analogy_res = avaliar_analogias(modelos, ep_c)

    print("Gerando relatório...")
    relatorio = gerar_relatorio(modelos, ep_c, cosine_res, analogy_res)

    os.makedirs("dados/embeddings", exist_ok=True)
    with open("dados/embeddings/relatorio_avaliacao.md", "w") as f:
        f.write(relatorio)

    print(f"\nRelatório salvo em: dados/embeddings/relatorio_avaliacao.md")

    # Resumo no terminal
    print("\n" + "="*60)
    print("  RESUMO RÁPIDO")
    print("="*60)
    print(f"{'Ép':>4} │ Cosine │ Pares>0.5 │ Analogias │ Pos.Média")
    print("─"*60)
    for e in ep_c:
        c = cosine_res[e]
        a = analogy_res[e]["Geral"]
        print(f"{e:>4} │ {c['geral_avg']:.4f} │ {c['acima_05']:>2}/{c['total_pares']:>2}     │ "
              f"{a['acertos_top1']:>2}/{a['total']:>2}      │ {a['media_posicao']:.2f}")

if __name__ == "__main__":
    main()
