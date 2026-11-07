import json
import numpy as np

ARQUIVO = "dados/embeddings/word2vec_v38.json"

with open(ARQUIVO, encoding="utf-8") as f:
    dados = json.load(f)

embeddings = {
    item["palavra"]: np.array(item["vetor"], dtype=np.float32)
    for item in dados["embeddings"]
}

normas = {
    palavra: np.linalg.norm(vetor)
    for palavra, vetor in embeddings.items()
}

def analisar(palavra, top=20):
    if palavra not in embeddings:
        print(f"\n{palavra}: não encontrada")
        return

    vetor = embeddings[palavra]
    resultados = []

    for outra, v in embeddings.items():
        if outra == palavra:
            continue

        denominador = normas[palavra] * normas[outra]
        if denominador == 0:
            continue

        score = np.dot(vetor, v) / denominador
        resultados.append((outra, float(score)))

    resultados.sort(key=lambda x: x[1], reverse=True)

    print(f"\n{'=' * 45}")
    print(f"PALAVRA: {palavra}")
    print(f"{'=' * 45}")

    for termo, score in resultados[:top]:
        print(f"{termo:<25} {score:.4f}")

def comparar(a, b):
    va, vb = embeddings[a], embeddings[b]
    score = np.dot(va, vb) / (normas[a] * normas[b])
    print(f"{a} ↔ {b}: {score:.4f}")

for palavra in ["deep", "deepseek", "felix", "natacha", "blue", "web", "resgatado", "resgatar"]:
    analisar(palavra)

print("\n===== COMPARAÇÕES =====")
for a, b in [
    ("deep", "deepseek"),
    ("deep", "felix"),
    ("deepseek", "felix"),
    ("deep", "natacha"),
    ("deepseek", "natacha"),
]:
    comparar(a, b)
