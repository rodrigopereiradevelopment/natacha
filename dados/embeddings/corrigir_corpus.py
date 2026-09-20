#!/usr/bin/env python3
"""Corrige colagens, acentos e maiúsculas no corpus."""

import re
import unicodedata
from pathlib import Path

ENTRADA = Path("corpus.txt")
SAIDA = Path("corpus_corrigido.txt")

# 1. Preservar nomes técnicos que têm ponto
EXCECOES = {
    "corpus.txt": "corpustxt",      # temporário, volta depois
    "next.js": "nextjs_temp",        # temporário, volta depois
}

def corrigir(texto):
    # Etapa 1 — proteger exceções
    for original, marcador in EXCECOES.items():
        texto = texto.replace(original, marcador)
    
    # Etapa 2 — corrigir colagens: ".letra" vira ". letra"
    texto = re.sub(r'\.([a-z])', r'. \1', texto)
    
    # Etapa 3 — remover acentos (NFD + filtro)
    texto = unicodedata.normalize('NFD', texto)
    texto = ''.join(c for c in texto if unicodedata.category(c) != 'Mn')
    
    # Etapa 4 — minúsculas
    texto = texto.lower()
    
    # Etapa 5 — restaurar exceções
    for original, marcador in EXCECOES.items():
        texto = texto.replace(marcador, original)
    
    # Etapa 6 — colapsar espaços múltiplos
    texto = re.sub(r' +', ' ', texto)
    
    return texto


def main():
    print(f"Lendo {ENTRADA}...")
    texto = ENTRADA.read_text(encoding='utf-8')
    print(f"  {len(texto)} caracteres")
    
    corrigido = corrigir(texto)
    print(f"  {len(corrigido)} caracteres após correção")
    
    SAIDA.write_text(corrigido, encoding='utf-8')
    print(f"Salvo em {SAIDA}")
    
    # Verificações
    print("\nVerificações:")
    padrao_colagem = re.findall(r'[a-z]\.[a-z]', corrigido)
    print(f"  Colagens restantes: {len(padrao_colagem)}")
    
    acentos = re.findall(r'[áàãâäéèêëíìîïóòõôöúùûüçÁÀÃÂÉÈÊÍÓÔÕÚÇ]', corrigido)
    print(f"  Acentos restantes: {len(acentos)}")
    
    maiusculas = re.findall(r'[A-Z]', corrigido)
    print(f"  Maiúsculas restantes: {len(maiusculas)}")


if __name__ == "__main__":
    main()