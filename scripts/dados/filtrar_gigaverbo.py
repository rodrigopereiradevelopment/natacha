import os
import re
import random
from datasets import load_dataset

# ============================================================
# CONFIG
# ============================================================
N_EXEMPLOS = 200_000
ARQUIVO_BRUTO = "gigaverbo_subset_200k.txt"
ARQUIVO_FILTRADO = "gigaverbo_filtrado.txt"

# ============================================================
# FILTRO DE QUALIDADE
# ============================================================
def bloco_valido(texto, min_palavras=40, max_palavras=300):
    """Aceita só blocos que parecem português geral de qualidade."""
    palavras = texto.split()
    n = len(palavras)

    # 1. Tamanho
    if n < min_palavras or n > max_palavras:
        return False

    # 2. Rejeita lista numerada (1., 2., 3.)
    if re.match(r'^\s*\d+[\.\)]\s', texto):
        return False

    # 3. Rejeita URL
    if re.search(r'https?://|www\.', texto):
        return False

    # 4. Rejeita prompt de IA
    if re.match(r'^\s*(Gere|Faça|Crie|Escreva|Liste|Escolha|Explique|Descreva|Qual|Quais|Como|Quando|Onde)\s',
                texto, re.IGNORECASE):
        return False

    # 5. Rejeita se tem muitas maiúsculas (slogan/título)
    letras = [c for c in texto if c.isalpha()]
    if letras and sum(1 for c in letras if c.isupper()) / len(letras) > 0.3:
        return False

    # 6. Rejeita se tem muitos dois-pontos (lista disfarçada)
    if texto.count(':') > 3:
        return False

    # 7. Rejeita se tem muitas vírgulas (lista disfarçada)
    if texto.count(',') / max(n, 1) > 0.3:
        return False

    # 8. Precisa ter pelo menos 1 ponto final (frase completa)
    if '.' not in texto:
        return False

    return True

# ============================================================
# DOWNLOAD (streaming, sem travar RAM)
# ============================================================
print(f"Baixando {N_EXEMPLOS:,} exemplos do GigaVerbo (streaming)...\n")

ds = load_dataset("TucanoBR/GigaVerbo", split="train", streaming=True)
subset = ds.take(N_EXEMPLOS)

with open(ARQUIVO_BRUTO, "w", encoding="utf-8") as f:
    for i, exemplo in enumerate(subset):
        f.write(exemplo["text"] + "\n\n")
        if (i + 1) % 20000 == 0:
            print(f"  Baixados: {i+1:,} exemplos")

print(f"\nBruto salvo: {ARQUIVO_BRUTO}\n")

# ============================================================
# FILTRO
# ============================================================
print("Filtrando...\n")

with open(ARQUIVO_BRUTO, "r", encoding="utf-8") as f:
    blocos = [b.strip() for b in f.read().split("\n\n") if b.strip()]

blocos_ok = [b for b in blocos if bloco_valido(b)]

print(f"Antes: {len(blocos):,} blocos")
print(f"Depois: {len(blocos_ok):,} blocos ({len(blocos_ok)/len(blocos)*100:.1f}%)\n")

with open(ARQUIVO_FILTRADO, "w", encoding="utf-8") as f:
    f.write("\n\n".join(blocos_ok))

print(f"Filtrado salvo: {ARQUIVO_FILTRADO}\n")

# ============================================================
# ESTATÍSTICAS DO FILTRADO
# ============================================================
tamanho_mb = os.path.getsize(ARQUIVO_FILTRADO) / (1024 * 1024)
palavras = sum(len(b.split()) for b in blocos_ok)
media = palavras / max(len(blocos_ok), 1)

print("=" * 60)
print("ESTATÍSTICAS DO CORPUS FILTRADO")
print("=" * 60)
print(f"Tamanho: {tamanho_mb:.2f} MB")
print(f"Blocos: {len(blocos_ok):,}")
print(f"Palavras: {palavras:,}")
print(f"Média palavras/bloco: {media:.1f}")
print("=" * 60 + "\n")

# ============================================================
# AMOSTRA (10 blocos aleatórios do filtrado)
# ============================================================
random.seed(42)
print("=== AMOSTRA (10 blocos aleatórios do filtrado) ===\n")
for i, b in enumerate(random.sample(blocos_ok, min(10, len(blocos_ok)))):
    print(f"--- bloco {i+1} ({len(b.split())} palavras) ---")
    print(b[:400])
    print()
