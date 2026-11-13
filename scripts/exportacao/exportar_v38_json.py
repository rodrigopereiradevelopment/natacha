import json
from gensim.models import Word2Vec

print("Carregando modelo...")
model = Word2Vec.load("../dados/embeddings/word2vec_v38_natacha_giga.model")

print("Exportando para o formato do EmbeddingManager...")
dados = {"embeddings": []}

for palavra in model.wv.index_to_key:
    dados["embeddings"].append({
        "palavra": palavra,
        "vetor": model.wv[palavra].tolist()
    })

with open("../dados/embeddings/word2vec_v38.json", "w", encoding="utf-8") as f:
    json.dump(dados, f)

print(f"Exportado: {len(dados['embeddings'])} palavras")