from gensim.models import Word2Vec
import unicodedata
import re
import json
import time

# ============================================================
# TOKENIZADOR NATACHA (Substitui o simple_preprocess)
# ============================================================
def remover_acentos(texto):
    return ''.join(c for c in unicodedata.normalize('NFD', texto) if unicodedata.category(c) != 'Mn')

def tokenizar_natacha(texto):
    # 1. Minúsculo e sem acento
    texto = remover_acentos(texto.lower())
    
    # 2. Regex que captura letras, números e símbolos técnicos (. + # - _)
    # Ex: "next.js", "c++", "teste346", "bem-estar", "gpu_0"
    tokens = re.findall(r'[a-z0-9][a-z0-9.+#_-]*', texto)
    
    # 3. Limpa pontuações soltas no final (ex: "natacha." -> "natacha")
    # Mas mantém "c++" e "next.js"
    tokens = [t.strip('.-_') for t in tokens]
    
    # 4. Remove tokens vazios e filtra tamanho mínimo (1 letra já serve para "a", "e", "o")
    return [t for t in tokens if t]

# ============================================================
# CARREGAR FRASES
# ============================================================
def carregar_frases(caminho, repeticoes=1, nome=""):
    frases = []
    print(f"Carregando {nome} (x{repeticoes})...")
    with open(caminho, "r", encoding="utf-8") as f:
        for _ in range(repeticoes):
            f.seek(0)
            for linha in f:
                linha = linha.strip()
                if linha and not linha.startswith("#"):
                    toks = tokenizar_natacha(linha)
                    if len(toks) >= 3:  # Mantém a regra original de >= 3 tokens
                        frases.append(toks)
    print(f"  {len(frases):,} frases")
    return frases

FONTES = [
    ("dados/embeddings/corpus_natacha_220k.txt", 8, "Natacha"),
    ("dados/embeddings/natacha_corpus_ponte.txt", 8, "Ponte"),
    ("dados/embeddings/corpus_opencode.txt", 8, "OpenCode"),
    ("dados/embeddings/corpus_felix_62.txt", 8, "Felix"),
    ("dados/embeddings/corpus_rnn_lstm.txt", 8, "RNN/LSTM"),
    ("dados/embeddings/corpus_casa_expandida.txt", 2, "Casa Exp"),
    ("gigaverbo_filtrado.txt", 1, "GigaVerbo"),
]

inicio = time.time()
todas_frases = []
for caminho, rep, nome in FONTES:
    frases = carregar_frases(caminho, rep, nome)
    todas_frases.extend(frases)

print(f"\nTotal: {len(todas_frases):,} frases")
print(f"Tempo de carregamento: {time.time() - inicio:.1f}s\n")

# ============================================================
# TREINAR
# ============================================================
print("Treinando Word2Vec v39 (d=64, w=5, epochs=20, neg=5)...")
inicio_treino = time.time()

model = Word2Vec(
    sentences=todas_frases,
    vector_size=64,
    window=5,
    min_count=3,
    workers=4,
    epochs=20,
    sg=1,
    negative=5,
    seed=42,
)

print(f"\nTempo de treino: {time.time() - inicio_treino:.1f}s\n")
print(f"Vocab: {len(model.wv):,} palavras\n")

# ============================================================
# EXPORTAR PARA JSON (Direto, sem script extra)
# ============================================================
print("Exportando para JSON...")
dados = {"embeddings": []}
for palavra in model.wv.index_to_key:
    dados["embeddings"].append({
        "palavra": palavra,
        "vetor": model.wv[palavra].tolist()
    })

with open("dados/embeddings/word2vec_v39.json", "w", encoding="utf-8") as f:
    json.dump(dados, f)

print("Exportado: dados/embeddings/word2vec_v39.json")

# ============================================================
# TESTE RÁPIDO
# ============================================================
print("\n" + "=" * 60)
print("TESTE DE SIMILARIDADE (v39)")
print("=" * 60)
for palavra in ["felix", "cozinha", "natacha", "c++", "next.js", "cafe"]:
    if palavra in model.wv:
        similares = model.wv.most_similar(palavra, topn=3)
        print(f"{palavra:12s} -> {[f'{p}({s:.2f})' for p, s in similares]}")
    else:
        print(f"{palavra:12s} -> NÃO está no vocab")