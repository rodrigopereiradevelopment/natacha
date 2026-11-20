# Dados de treino — regra dos três níveis

Esta pasta separa **código**, **dado** e **recordação pessoal** para que o
repositório continue pequeno e reproduzível.

## Estrutura

```
dados/embeddings/
├── original/     # Fonte da verdade. Textos autorais ficam VERSIONADOS.
├── limpo/        # Derivado: deduplicação e filtro. FORA do git.
├── acentuado/    # Derivado: acentuação via LanguageTool. FORA do git.
└── relatorios/   # Auditoria pequena (.txt). VERSIONADA.
```

## O que vai no git

| Caminho | Vai no git? | Por quê |
|---|---|---|
| `original/cenas_cotidianas/` | ✅ sim | Textos autorais escritos à mão. Não são regeneráveis. |
| `original/corpus_*.txt` (pequenos) | ✅ sim | Curadoria manual (Félix, casa, LSTM, ponte). |
| `original/corpus_natacha_220k.txt` | ❌ não | Grande (~1,4 MB) e gerado por expansão. |
| `original/corpus_opencode.txt` | ❌ não | Importado de fonte externa. |
| `original/corpus_wikipedia_dominio.txt` | ❌ não | Importado da Wikipédia. |
| `limpo/` | ❌ não | Regerado por `scripts/normalizacao/limpar_corpus.py`. |
| `acentuado/` | ❌ não | Regerado por `scripts/normalizacao/colocar_acentos.py`. |
| `relatorios/` | ✅ sim | Pequenos e úteis para a banca (auditoria do processo). |

## Pipeline

```
original/ ──limpar_corpus.py──▶ limpo/ ──colocar_acentos.py──▶ acentuado/
                  │                        │
                  └──── relatorios/ ◀──────┘
```

Rodando a partir da **raiz do projeto**:

```bash
python3 scripts/normalizacao/limpar_corpus.py
python3 scripts/normalizacao/colocar_acentos.py
```

## Pesos e vetores

`word2vec_v39.json`, `lstm_pesos.json`, `output_pesos.json` e afins **não**
são versionados: são grandes e regeneráveis pelo treino. O
`EmbeddingManager` grava um cache binário (`word2vec_v39.json.bin`) ao lado
do JSON na primeira carga — a inicialização do chat cai de ~12 s para ~0,1 s.

## Recordação pessoal

Imagens da galeria (`pessoal/galeria/`) **não** entram no repositório. Elas
são memória afetiva do projeto, não código nem dado de treino. Mantenha um
backup externo (HD, nuvem pessoal) — o git não é esse backup.
