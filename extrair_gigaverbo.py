import re
from datasets import load_dataset

# 1. Carrega o SEU vocabulário da Natacha (extraído do seu corpus 220k)
def carregar_vocabulario_natacha(caminho_corpus):
    vocab = set()
    with open(caminho_corpus, 'r', encoding='utf-8') as f:
        for linha in f:
            # Tokenização simples (mesma lógica do seu C++)
            palavras = re.findall(r'[a-z0-9+#]{2,}', linha.lower())
            vocab.update(palavras)
    return vocab

# 2. Tokenizador simples (aproximação do seu código C++)
def tokenizar(texto):
    return re.findall(r'[a-z0-9+#]{2,}', texto.lower())

# 3. Pipeline principal
def extrair_gigaverbo(caminho_corpus_natacha, caminho_saida, meta_tokens=4_000_000):
    print("Carregando vocabulário da Natacha...")
    vocab_natacha = carregar_vocabulario_natacha(caminho_corpus_natacha)
    print(f"Vocabulário da Natacha: {len(vocab_natacha)} palavras")

    print("Conectando ao GigaVerbo em modo streaming...")
    # Streaming evita baixar 780GB. label=1 filtra por qualidade.
    ds = load_dataset("TucanoBR/GigaVerbo", split="train", streaming=True)
    
    tokens_coletados = 0
    linhas_salvas = 0

    with open(caminho_saida, 'w', encoding='utf-8') as f_out:
        for exemplo in ds:
            # Filtro de qualidade (label 1 = alta qualidade)
            if exemplo.get("label") != 1:
                continue

            texto = exemplo["text"]
            tokens = tokenizar(texto)

            # FILTRO DE VOCABULÁRIO: só aceita se tiver interseção com a Natacha
            if not any(t in vocab_natacha for t in tokens):
                continue

            # Escreve a linha e conta
            f_out.write(texto + "\n")
            tokens_coletados += len(tokens)
            linhas_salvas += 1

            if tokens_coletados >= meta_tokens:
                break

            if linhas_salvas % 1000 == 0:
                print(f"Progresso: {tokens_coletados:,} tokens | {linhas_salvas} linhas")

    print(f"\n✓ Concluído!")
    print(f"  Tokens coletados: {tokens_coletados:,}")
    print(f"  Linhas salvas: {linhas_salvas}")
    print(f"  Arquivo: {caminho_saida}")

if __name__ == "__main__":
    # AJUSTE OS CAMINHOS AQUI
    CORPUS_NATACHA = "dados/embeddings/corpus_natacha_220k.txt" 
    SAIDA = "dados/embeddings/corpus_etapa1_externo_3M.txt"
    
    extrair_gigaverbo(CORPUS_NATACHA, SAIDA)