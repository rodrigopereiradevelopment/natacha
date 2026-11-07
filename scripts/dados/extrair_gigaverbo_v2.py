import re
import requests
import time

DATASET = "TucanoBR/GigaVerbo"
CONFIG = "default"
SPLIT = "train"
BASE_URL = "https://datasets-server.huggingface.co/rows"

# Blacklist de padrões que denunciam output de LLM
BLACKLIST = [
    "como assistente", "sou programado", "como modelo de linguagem",
    "enquanto ia", "não posso ajudar", "sinto muito", "claro!",
    "certamente!", "em resumo", "é importante notar", "vamos lá",
    "espero ter ajudado", "fico à disposição", "não hesite em",
    "qualquer dúvida", "estou aqui para ajudar",
]

def carregar_vocab(caminho):
    vocab = set()
    with open(caminho, 'r', encoding='utf-8') as f:
        for linha in f:
            vocab.update(re.findall(r'[a-z0-9+#]{2,}', linha.lower()))
    return vocab

def tokenizar(texto):
    return re.findall(r'[a-z0-9+#]{2,}', texto.lower())

def linha_valida(texto, vocab, min_palavras=15):
    # Filtro 1: tamanho mínimo
    tokens = tokenizar(texto)
    if len(tokens) < min_palavras:
        return None
    # Filtro 2: blacklist de LLM
    low = texto.lower()
    for padrao in BLACKLIST:
        if padrao in low:
            return None
    # Filtro 3: interseção de vocabulário
    if not any(t in vocab for t in tokens):
        return None
    return tokens

def extrair(corpus_natacha, saida, meta_tokens=4_000_000, lote=100,
            offset_inicial=0, salto=5000):
    """
    salto: pula N registros entre lotes para diversificar as amostras
    """
    print("Carregando vocabulário...")
    vocab = carregar_vocab(corpus_natacha)
    print(f"Vocabulário: {len(vocab)} palavras")

    tokens_total = 0
    linhas_salvas = 0
    linhas_descartadas = 0
    offset = offset_inicial
    falhas = 0

    with open(saida, 'w', encoding='utf-8') as f:
        while tokens_total < meta_tokens:
            try:
                r = requests.get(BASE_URL, params={
                    "dataset": DATASET,
                    "config": CONFIG,
                    "split": SPLIT,
                    "offset": offset,
                    "length": lote
                }, timeout=30)
                r.raise_for_status()
                data = r.json()
            except Exception as e:
                falhas += 1
                print(f"Erro ({falhas}): {e}")
                if falhas > 5:
                    print("Muitas falhas. Abortando.")
                    break
                time.sleep(5)
                continue

            falhas = 0
            rows = data.get("rows", [])
            if not rows:
                print("Fim do dataset.")
                break

            for row in rows:
                if row["row"].get("label") != 1:
                    continue
                texto = row["row"].get("text", "")
                toks = linha_valida(texto, vocab)
                if toks is None:
                    linhas_descartadas += 1
                    continue
                f.write(texto.strip() + "\n")
                tokens_total += len(toks)
                linhas_salvas += 1
                if tokens_total >= meta_tokens:
                    break

            offset += lote + salto  # pula para diversificar
            if linhas_salvas % 500 < lote:
                print(f"Tokens: {tokens_total:,} | Salvas: {linhas_salvas} | "
                      f"Descartadas: {linhas_descartadas} | offset {offset}")

    print(f"\n✓ Tokens: {tokens_total:,} | Linhas: {linhas_salvas} | "
          f"Descartadas: {linhas_descartadas}")
    print(f"Arquivo: {saida}")

if __name__ == "__main__":
    extrair(
        corpus_natacha="dados/embeddings/corpus_natacha_220k.txt",
        saida="dados/embeddings/corpus_etapa1_externo_v2.txt"
    )
