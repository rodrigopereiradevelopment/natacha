import json
import unicodedata
import sys

# ============================================================
# CONFIG
# ============================================================
ARQUIVO = sys.argv[1] if len(sys.argv) > 1 else "dados/embeddings/word2vec_v39.json"

# ============================================================
# CARREGAR
# ============================================================
print(f"Carregando: {ARQUIVO}")
with open(ARQUIVO, "r", encoding="utf-8") as f:
    dados = json.load(f)

# Extrai o vocabulário
if "embeddings" in dados:
    vocab = [item["palavra"] for item in dados["embeddings"]]
elif "vocab" in dados:
    vocab = dados["vocab"]
else:
    print("ERRO: formato de JSON desconhecido.")
    sys.exit(1)

print(f"Total de palavras: {len(vocab):,}\n")

# ============================================================
# TESTES
# ============================================================
def tem_acento(p):
    return any(unicodedata.category(c).startswith('M') for c in unicodedata.normalize('NFD', p))

print("=" * 60)
print("1. VERIFICAÇÃO DE ACENTOS")
print("=" * 60)
com_acento = [p for p in vocab if tem_acento(p)]
print(f"Palavras com acento: {len(com_acento)}")
if com_acento:
    print(f"  Exemplos: {com_acento[:15]}")

print("\n" + "=" * 60)
print("2. VERIFICAÇÃO DE MAIÚSCULAS")
print("=" * 60)
com_maiuscula = [p for p in vocab if p != p.lower()]
print(f"Palavras com maiúscula: {len(com_maiuscula)}")
if com_maiuscula:
    print(f"  Exemplos: {com_maiuscula[:15]}")

print("\n" + "=" * 60)
print("3. VERIFICAÇÃO DE PALAVRAS TÉCNICAS")
print("=" * 60)
tecnicas = ["c++", "next.js", "teste346", "bem-estar", "event-bus", "word2vec", "qiskit", "cuda"]
for t in tecnicas:
    status = "OK" if t in vocab else "FALTANDO"
    print(f"  {status:8s} '{t}'")

print("\n" + "=" * 60)
print("4. VERIFICAÇÃO DE DUPLICATAS (nao vs não)")
print("=" * 60)
pares = [("nao", "não"), ("cafe", "café"), ("voce", "você"), ("esta", "está")]
for sem, com in pares:
    status_sem = "OK" if sem in vocab else "FALTANDO"
    status_com = "PRESENTE (ERRO)" if com in vocab else "OK (ausente)"
    print(f"  '{sem}': {status_sem:8s} | '{com}': {status_com}")

print("\n" + "=" * 60)
print("5. AMOSTRA DO VOCABULÁRIO")
print("=" * 60)
print("Primeiras 20 palavras:")
for i, p in enumerate(vocab[:20]):
    print(f"  {i+1}. {p}")

print("\nÚltimas 10 palavras:")
for i, p in enumerate(vocab[-10:]):
    print(f"  {len(vocab) - 10 + i + 1}. {p}")

# ============================================================
# VEREDITO FINAL
# ============================================================
print("\n" + "=" * 60)
print("VEREDITO")
print("=" * 60)

erros = 0
if len(com_acento) > 0:
    print(f"❌ {len(com_acento)} palavras com acento (deveria ser 0)")
    erros += 1
if len(com_maiuscula) > 0:
    print(f"❌ {len(com_maiuscula)} palavras com maiúscula (deveria ser 0)")
    erros += 1

faltando = [t for t in tecnicas if t not in vocab]
if faltando:
    print(f"⚠️  Palavras técnicas faltando: {faltando}")
    erros += 1

if erros == 0:
    print("✅ Vocabulário LIMPO! Pode treinar a LSTM.")
else:
    print(f"\n{erros} problema(s) encontrado(s). Verifique o tokenizador.")