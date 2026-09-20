#!/usr/bin/env python3
"""
corrigir_corpus.py — Corrige colagens, acentos e maiúsculas.

Uso:
    python3 corrigir_corpus.py                     # usa corpus.txt por padrão
    python3 corrigir_corpus.py extensao.txt        # corrige arquivo específico
    python3 corrigir_corpus.py entrada.txt saida.txt
"""

import re
import sys
import unicodedata
from pathlib import Path

# Nomes técnicos que têm ponto e devem ser preservados
EXCECOES = {
    "corpus.txt": "corpustxt_temp",
    "next.js":    "nextjs_temp",
    "c++":        "cplusplus_temp",
}


def corrigir(texto):
    # 1. Proteger exceções
    for original, marcador in EXCECOES.items():
        texto = texto.replace(original, marcador)

    # 2. Corrigir colagens: ".letra" vira ". letra"
    texto = re.sub(r'\.([a-z])', r'. \1', texto)

    # 3. Remover acentos (NFD + filtro Mn)
    texto = unicodedata.normalize('NFD', texto)
    texto = ''.join(c for c in texto if unicodedata.category(c) != 'Mn')

    # 4. Minúsculas
    texto = texto.lower()

    # 5. Restaurar exceções
    for original, marcador in EXCECOES.items():
        texto = texto.replace(marcador, original)

    # 6. Colapsar espaços múltiplos
    texto = re.sub(r' +', ' ', texto)

    # 7. Colapsar linhas vazias múltiplas em uma
    texto = re.sub(r'\n{3,}', '\n\n', texto)

    # 8. Remover espaço antes de ponto
    texto = re.sub(r' \.', '.', texto)

    return texto


def main():
    # Argumentos
    if len(sys.argv) >= 2:
        entrada = Path(sys.argv[1])
    else:
        entrada = Path("corpus.txt")

    if len(sys.argv) >= 3:
        saida = Path(sys.argv[2])
    else:
        saida = entrada.with_name(entrada.stem + "_corrigido" + entrada.suffix)

    if not entrada.exists():
        print(f"ERRO: {entrada} não existe.")
        sys.exit(1)

    print(f"Entrada: {entrada}")
    print(f"Saída:   {saida}")

    texto = entrada.read_text(encoding='utf-8')
    print(f"  {len(texto)} caracteres")

    corrigido = corrigir(texto)
    print(f"  {len(corrigido)} caracteres após correção")

    saida.write_text(corrigido, encoding='utf-8')
    print(f"  Salvo em {saida}")

    # Verificações
    print("\nVerificações:")
    colagens = re.findall(r'[a-z]\.[a-z]', corrigido)
    acentos  = re.findall(r'[áàãâäéèêëíìîïóòõôöúùûüç]', corrigido)
    maius    = re.findall(r'[A-Z]', corrigido)

    print(f"  Colagens restantes: {len(colagens)}")
    print(f"  Acentos restantes:  {len(acentos)}")
    print(f"  Maiúsculas:         {len(maius)}")

    if colagens:
        print(f"  Exemplos de colagens: {colagens[:5]}")
    if acentos:
        print(f"  Exemplos de acentos: {acentos[:5]}")


if __name__ == "__main__":
    main()