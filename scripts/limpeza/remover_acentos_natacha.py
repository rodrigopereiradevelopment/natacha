import unicodedata

def remover_acentos(texto):
    return ''.join(
        c for c in unicodedata.normalize('NFD', texto)
        if unicodedata.category(c) != 'Mn'
    )

with open('corpus_natacha_220k.txt', 'r', encoding='utf-8') as f:
    texto = f.read()

print(f"Original: {len(texto):,} chars")
texto_limpo = remover_acentos(texto)
print(f"Limpo: {len(texto_limpo):,} chars")
print(f"Removidos: {len(texto) - len(texto_limpo):,}")

with open('corpus_natacha_220k_sem_acento.txt', 'w', encoding='utf-8') as f:
    f.write(texto_limpo)

print("Salvo: corpus_natacha_220k_sem_acento.txt")
