#!/bin/bash
# baixar_fasttext.sh — Baixa embeddings fastText pt-BR
#
# Uso: ./baixar_fasttext.sh
#
# O arquivo será salvo em: dados/embeddings/cc.pt.300.vec
# Tamanho: ~1.5GB (descompactado)

set -e

DIRETORIO="dados/embeddings"
ARQUIVO="$DIRETORIO/cc.pt.300.vec"
ARQUIVO_GZ="$DIRETORIO/cc.pt.300.vec.gz"
URL="https://dl.fbaipublicfiles.com/fasttext/vectors-crawl/cc.pt.300.vec.gz"

echo "╔═══════════════════════════════════════════════════════╗"
echo "║     DOWNLOAD FASTTEXT PT-BR                          ║"
echo "╚═══════════════════════════════════════════════════════╝"
echo ""
echo "  Arquivo: cc.pt.300.vec.gz"
echo "  Dimensoes: 300"
echo "  Tamanho: ~1.5GB (descompactado)"
echo "  Fonte: Facebook AI Research"
echo ""

# Criar diretorio se não existir
mkdir -p "$DIRETORIO"

# Verificar se já existe
if [ -f "$ARQUIVO" ]; then
    echo "✓ Arquivo já existe: $ARQUIVO"
    echo "  Pulando download..."
else
    echo "Baixando..."
    wget -c --progress=bar:force:noscroll "$URL" -O "$ARQUIVO_GZ"

    echo ""
    echo "Descompactando..."
    gunzip "$ARQUIVO_GZ"

    echo "✓ Download concluído!"
fi

echo ""
echo "Arquivo: $ARQUIVO"
echo ""
echo "Para usar no treino híbrido:"
echo "  ./word2vec_hybrid --pre-treinado $ARQUIVO"
echo ""
