import sys
sys.path.insert(0, "/home/rodrigo/natacha")
from extrair_wikipedia_v4 import extrair_wikipedia

extrair_wikipedia(
    corpus_natacha="dados/embeddings/corpus_natacha_220k.txt",
    saida="dados/embeddings/corpus_etapa1_wikipedia_v5.txt",
    meta_tokens=8_500_000
)
