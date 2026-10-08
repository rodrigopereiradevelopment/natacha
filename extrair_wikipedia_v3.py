import re
import unicodedata
from datasets import load_dataset

def remover_acentos(texto):
    return ''.join(
        c for c in unicodedata.normalize('NFD', texto)
        if unicodedata.category(c) != 'Mn'
    )

def carregar_vocab(caminho):
    vocab = set()
    with open(caminho, 'r', encoding='utf-8') as f:
        for linha in f:
            linha = remover_acentos(linha)
            vocab.update(re.findall(r'[a-z0-9+#]{2,}', linha.lower()))
    return vocab

def tokenizar(texto):
    return re.findall(r'[a-z0-9+#]{2,}', texto.lower())

def bloco_valido(texto):
    texto = texto.strip()
    if len(texto) < 500:
        return False
    if re.match(r'^\d{1,2}\s+de\s+\w+\s+de\s+\d{4}', texto):
        return False
    if len(texto) < 100:
        return False
    if re.match(r'^\s*\d+\.\s', texto):
        return False
    return True

def extrair_wikipedia(corpus_natacha, saida, meta_tokens=3_500_000):
    vocab = carregar_vocab(corpus_natacha)
    print(f"Vocabulario Natacha: {len(vocab)} palavras")

    ds = load_dataset("wikimedia/wikipedia", "20231101.pt",
                      split="train", streaming=True)

    tokens_total = 0
    artigos_salvos = 0
    artigos_descartados = 0

    with open(saida, 'w', encoding='utf-8') as f:
        for exemplo in ds:
            texto = exemplo.get("text", "").strip()
            if not texto:
                continue

            # 1. Remove acentos ANTES de processar
            texto = remover_acentos(texto)

            # 2. Filtra blocos
            blocos = [b for b in texto.split("\n\n") if bloco_valido(b)]
            if not blocos:
                artigos_descartados += 1
                continue

            texto_limpo = "\n\n".join(blocos)
            toks = tokenizar(texto_limpo)

            intersecao = sum(1 for t in toks if t in vocab)
            if intersecao < 10:
                artigos_descartados += 1
                continue

            f.write(texto_limpo + "\n\n")
            tokens_total += len(toks)
            artigos_salvos += 1

            if artigos_salvos % 200 == 0:
                print(f"Artigos: {artigos_salvos} | Tokens: {tokens_total:,} | Descartados: {artigos_descartados}")
            if tokens_total >= meta_tokens:
                break

    print(f"\nOK Tokens: {tokens_total:,} | Artigos: {artigos_salvos} | Descartados: {artigos_descartados}")
    print(f"Arquivo: {saida}")

if __name__ == "__main__":
    extrair_wikipedia(
        corpus_natacha="dados/embeddings/corpus_natacha_220k.txt",
        saida="dados/embeddings/corpus_etapa1_wikipedia_v3.txt",
        meta_tokens=3_500_000
    )
