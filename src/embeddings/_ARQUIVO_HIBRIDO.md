# Híbrido FastText — Arquivado

**Data de arquivamento:** Setembro/2026
**Motivo:** Decisão de manter Natacha 100% "construída do zero".
FastText viola a essência do projeto (usa corpus externo pré-treinado).

**Reavaliar se:**
- Corpus não crescer além de 150k tokens em 6 meses
- Scraper não for implementado
- Precisar de ganho rápido em palavras raras sem expandir corpus

**Arquivos:**
- _arquivado_word2vec_hybrid_v2.cpp (com 5 fixes aplicados)
- _arquivado_word2vec_hybrid_v2 (binário compilado)

**Estado anterior testado:** perda de qualidade em corpus de 52k tokens.
FastText `.50.vec` não chegou a ser testado.
