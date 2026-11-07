import re
from datasets import load_dataset

# === NOVO: blacklist de geografia ===
GEOGRAFIA = {
    'rua', 'avenida', 'bairro', 'cidade', 'municipio', 'município', 'praça',
    'rodovia', 'estrada', 'distrito', 'regiao', 'região', 'estado', 'pais',
    'país', 'capital', 'populacao', 'população', 'habitantes', 'km2', 'km²',
    'censo', 'ibge', 'fundado', 'criado', 'inaugurado', 'seculo', 'século',
    'decada', 'década', 'ano', 'anos', 'prefeito', 'vereador', 'governador',
}

def carregar_vocab(caminho):
    vocab = set()
    with open(caminho, 'r', encoding='utf-8') as f:
        for linha in f:
            vocab.update(re.findall(r'[a-z0-9+#]{2,}', linha.lower()))
    return vocab

def tokenizar(texto):
    return re.findall(r'[a-z0-9+#]{2,}', texto.lower())

def bloco_valido(texto):
    """Filtra blocos individuais dentro do artigo."""
    texto = texto.strip()
    if len(texto) < 500:  # bloco curto demais
        return False
    # Blacklist de efemérides
    if re.match(r'^\d{1,2}\s+de\s+\w+\s+de\s+\d{4}', texto):
        return False
    # Blacklist de títulos soltos
    if len(texto) < 100:
        return False
    # Blacklist de listas numeradas tipo FAQ
    if re.match(r'^\s*\d+\.\s', texto):
        return False

    # === NOVO: blacklist de geografia ===
    toks = tokenizar(texto)
    if not toks:
        return False
    geo_count = sum(1 for t in toks if t in GEOGRAFIA)
    if geo_count / len(toks) > 0.15:  # >15% de palavras de geografia
        return False

    # === NOVO: blacklist de muitas maiúsculas (nomes próprios) ===
    palavras = texto.split()
    if len(palavras) > 0:
        maiusculas = sum(1 for p in palavras if p and p[0].isupper())
        if maiusculas / len(palavras) > 0.30:  # >30% maiúsculas
            return False

    return True

def extrair_wikipedia(corpus_natacha, saida, meta_tokens=3_000_000):
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

            # Quebra em blocos (parágrafos separados por \n\n)
            blocos = [b for b in texto.split("\n\n") if bloco_valido(b)]
            if not blocos:
                artigos_descartados += 1
                continue

            # Reconstrói o artigo com os blocos válidos
            texto_limpo = "\n\n".join(blocos)
            toks = tokenizar(texto_limpo)

            # === MUDANÇA: interseção sobe de 10 pra 20 ===
            intersecao = sum(1 for t in toks if t in vocab)
            if intersecao < 20:  # era 10
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
        saida="dados/embeddings/corpus_etapa1_wikipedia_v5.txt",
        meta_tokens=8_500_000
    )
    