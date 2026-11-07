import os

# Corpus narrativo (principal)
narrativo = [
    "dados/embeddings/corpus_natacha_casa_300.txt",
    "dados/embeddings/corpus_felix_62.txt",
    "dados/embeddings/corpus_casa_expandida.txt",
    "dados/embeddings/corpus_rnn_lstm.txt",
]

# Corpus auxiliar (frases curtas, pensamentos)
auxiliar = [
    "dados/embeddings/natacha_corpus_pensadora.txt",
    "dados/embeddings/natacha_corpus_dev.txt",
]

def consolidar(fontes, saida):
    cenas = []
    for caminho in fontes:
        if not os.path.exists(caminho):
            print(f"AVISO: {caminho} nao encontrado.")
            continue
        with open(caminho, "r", encoding="utf-8") as f:
            texto = f.read()
        # Separa por linha em branco
        blocos = [b.strip() for b in texto.split("\n\n") if b.strip()]
        for b in blocos:
            if not b.endswith("."):
                b += "."
            cenas.append(b)
        print(f"{caminho}: {len(blocos)} cenas")
    
    with open(saida, "w", encoding="utf-8") as f:
        f.write("\n\n".join(cenas))
    
    palavras = sum(len(c.split()) for c in cenas)
    print(f"\nTotal: {len(cenas)} cenas, {palavras:,} palavras")
    print(f"Salvo em: {saida}\n")

print("=== CONSOLIDANDO NARRATIVO ===")
consolidar(narrativo, "dados/embeddings/corpus_lstm_narrativo.txt")

print("=== CONSOLIDANDO AUXILIAR ===")
consolidar(auxiliar, "dados/embeddings/corpus_lstm_auxiliar.txt")