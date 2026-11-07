import os
import random

caminho = "gigaverbo_subset_50k.txt"

# 1. Tamanho em MB
tamanho_mb = os.path.getsize(caminho) / (1024 * 1024)
print(f"Tamanho: {tamanho_mb:.2f} MB\n")

# 2. Contar linhas, palavras, caracteres
with open(caminho, "r", encoding="utf-8") as f:
    texto = f.read()

linhas = texto.split("\n")
blocos = [b for b in texto.split("\n\n") if b.strip()]
palavras = texto.split()

print(f"Linhas: {len(linhas):,}")
print(f"Blocos (paragrafos): {len(blocos):,}")
print(f"Palavras: {len(palavras):,}")
print(f"Tokens aprox.: {len(palavras):,}\n")

# 3. Media de palavras por bloco
if blocos:
    media = sum(len(b.split()) for b in blocos) / len(blocos)
    print(f"Media palavras/bloco: {media:.1f}\n")

# 4. Amostra: 10 blocos aleatorios
random.seed(42)
print("=== AMOSTRA (10 blocos aleatorios) ===\n")
for i, b in enumerate(random.sample(blocos, min(10, len(blocos)))):
    print(f"--- bloco {i+1} ({len(b.split())} palavras) ---")
    print(b[:400])
    print()
