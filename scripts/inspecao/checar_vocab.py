import json

# Carrega embeddings
with open('dados/embeddings/natacha_embeddings.json', 'r') as f:
    emb = json.load(f)
vocab_emb = set(item['palavra'] for item in emb['embeddings'])

# Carrega vocab local
with open('dados/embeddings/vocab_local.json', 'r') as f:
    vocab_local = json.load(f)
vocab_local_set = set(vocab_local['id_para_palavra'])

# Diferença
so_no_local = vocab_local_set - vocab_emb

print(f"Palavras so no vocab local: {len(so_no_local)}")
print(f"Exemplos: {list(so_no_local)[:20]}")