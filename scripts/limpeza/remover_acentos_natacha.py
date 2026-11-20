"""
Remove acentos de um corpus (script manual).

Pipeline do projeto:
    dados/embeddings/original/   -> fonte da verdade (nunca alterada)
    dados/embeddings/limpo/      -> derivados (fora do git)

Rode a partir da RAIZ do projeto:
    python3 scripts/limpeza/remover_acentos_natacha.py
"""

import unicodedata


def remover_acentos(texto):
    return ''.join(
        c for c in unicodedata.normalize('NFD', texto)
        if unicodedata.category(c) != 'Mn'
    )


ENTRADA = 'dados/embeddings/original/corpus_natacha_220k.txt'
SAIDA = 'dados/embeddings/limpo/corpus_natacha_sem_acento.txt'

with open(ENTRADA, 'r', encoding='utf-8') as f:
    texto = f.read()

print(f"Original: {len(texto):,} chars")
texto_limpo = remover_acentos(texto)
print(f"Limpo: {len(texto_limpo):,} chars")
print(f"Removidos: {len(texto) - len(texto_limpo):,}")

with open(SAIDA, 'w', encoding='utf-8') as f:
    f.write(texto_limpo)

print(f"Salvo: {SAIDA}")
