import unicodedata

def remover_acentos(texto):
    return ''.join(
        c for c in unicodedata.normalize('NFD', texto)
        if unicodedata.category(c) != 'Mn'
    )

arquivo_entrada = 'corpus_etapa1_wikipedia_v2.txt'
arquivo_saida = 'corpus_etapa1_wikipedia_sem_acento.txt'

print(f"Lendo {arquivo_entrada}...")
with open(arquivo_entrada, 'r', encoding='utf-8') as f:
    texto = f.read()

print(f"Tamanho original: {len(texto):,} caracteres")
print(f"Contem acentos? {'sim' if any(c in texto for c in 'áàâãéèêíìîóòôõúùûçÁÀÂÃÉÈÊÍÌÎÓÒÔÕÚÙÛÇ') else 'nao'}")

texto_limpo = remover_acentos(texto)

print(f"Tamanho limpo: {len(texto_limpo):,} caracteres")
print(f"Diferenca: {len(texto) - len(texto_limpo):,} caracteres removidos")

with open(arquivo_saida, 'w', encoding='utf-8') as f:
    f.write(texto_limpo)

print(f"OK: {arquivo_saida} criado")