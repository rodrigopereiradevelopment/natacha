import re
from datasets import load_dataset

def carregar_vocab(caminho):
    vocab = set()
    with open(caminho, 'r', encoding='utf-8') as f:
        for linha in f:
            vocab.update(re.findall(r'[a-z0-9+#]{2,}', linha.lower()))
    return vocab

def tokenizar(texto):
    return re.findall(r'[a-z0-9+#]{2,}', texto.lower())

def extrair_wikipedia(corpus_natacha, saida, meta_tokens=3_000_000):
    vocab = carregar_vocab(corpus_natacha)
    print(f"Vocabulario Natacha: {len(vocab)} palavras")

    ds = load_dataset("wikimedia/wikipedia", "20231101.pt",
                      split="train", streaming=True)

    tokens_total = 0
    linhas_salvas = 0
    linhas_descartadas = 0

    with open(saida, 'w', encoding='utf-8') as f:
        for exemplo in ds:
            texto = exemplo.get("text", "").strip()
            if len(texto) < 500:
                linhas_descartadas += 1
                continue
            toks = tokenizar(texto)
            intersecao = sum(1 for t in toks if t in vocab)
            if intersecao < 5:
                linhas_descartadas += 1
                continue

            f.write(texto + "\n\n")
            tokens_total += len(toks)
            linhas_salvas += 1

            if linhas_salvas % 500 == 0:
                print(f"Artigos: {linhas_salvas} | Tokens: {tokens_total:,} | Descartados: {linhas_descartadas}")
            if tokens_total >= meta_tokens:
                break

    print(f"\nOK Tokens: {tokens_total:,} | Artigos: {linhas_salvas} | Descartados: {linhas_descartadas}")
    print(f"Arquivo: {saida}")

if __name__ == "__main__":
    extrair_wikipedia(
        corpus_natacha="dados/embeddings/corpus_natacha_220k.txt",
        saida="dados/embeddings/corpus_etapa1_wikipedia.txt",
        meta_tokens=3_000_000
    )
