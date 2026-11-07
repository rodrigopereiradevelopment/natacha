import sys
import unicodedata

def remover_acentos(texto):
    return ''.join(
        c for c in unicodedata.normalize('NFD', texto)
        if unicodedata.category(c) != 'Mn'
    )

if len(sys.argv) < 3:
    print("Uso: python3 remover_acentos.py <entrada> <saida>")
    print("Ex:  python3 remover_acentos.py corpus_v25_final.txt corpus_v25_limpo.txt")
    sys.exit(1)

arquivo_entrada = sys.argv[1]
arquivo_saida = sys.argv[2]

print(f"Lendo {arquivo_entrada}...")
with open(arquivo_entrada, 'r', encoding='utf-8') as f:
    texto = f.read()

print(f"Tamanho original: {len(texto):,} caracteres")

acentos = 'áàâãéèêíìîóòôõúùûçÁÀÂÃÉÈÊÍÌÎÓÒÔÕÚÙÛÇ'
tem_acento = any(c in texto for c in acentos)
print(f"Contem acentos? {'sim' if tem_acento else 'nao'}")

texto_limpo = remover_acentos(texto)

print(f"Tamanho limpo: {len(texto_limpo):,} caracteres")
print(f"Removidos: {len(texto) - len(texto_limpo):,} caracteres")

with open(arquivo_saida, 'w', encoding='utf-8') as f:
    f.write(texto_limpo)

print(f"Salvo: {arquivo_saida}")
