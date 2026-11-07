from pathlib import Path
from gensim.utils import simple_preprocess

FONTES = {
    "dados/embeddings/corpus_lstm_narrativo.txt":
        "dados/embeddings/corpus_lstm_narrativo_tokens.txt",

    "dados/embeddings/corpus_lstm_auxiliar.txt":
        "dados/embeddings/corpus_lstm_auxiliar_tokens.txt",
}

def preparar(entrada, saida):
    texto = Path(entrada).read_text(encoding="utf-8")
    cenas = [c.strip() for c in texto.split("\n\n") if c.strip()]

    linhas = []
    total_palavras = 0

    for cena in cenas:
        tokens = simple_preprocess(
            cena,
            deacc=False,
            min_len=2,
            max_len=15
        )

        if len(tokens) >= 3:
            linhas.append(" ".join(tokens))
            total_palavras += len(tokens)

    Path(saida).write_text(
        "\n".join(linhas) + "\n",
        encoding="utf-8"
    )

    print(f"\nArquivo: {saida}")
    print(f"Cenas originais: {len(cenas)}")
    print(f"Cenas válidas: {len(linhas)}")
    print(f"Palavras: {total_palavras:,}")

for entrada, saida in FONTES.items():
    preparar(entrada, saida)

print("\nPreparação concluída.")
