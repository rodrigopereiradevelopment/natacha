# Licenças de Dados

Este documento descreve a origem e a licença de cada componente do corpus da Natacha.

## Componentes do corpus

| Componente | Origem | Licença | Uso comercial |
|---|---|---|---|
| `dados/embeddings/original/corpus_natacha_220k.txt` | Rodrigo Pereira (autoral) | CC BY-SA 4.0 | Requer autorização |
| `dados/embeddings/original/natacha_corpus_ponte.txt` | Gerado por IA (Kimi, Grok, Qwen, Nemotron) | Uso acadêmico | Requer regeneração com modelo open-weight |
| `corpus_etapa1_wikipedia_v3.txt` | Wikimedia (dump 20231101.pt) | CC BY-SA 3.0 | Permitido com atribuição |
| Embeddings treinados | Derivados dos corpora acima | Mesma dos corpora | Requer revisão |

## Detalhes

### Corpus Natacha (autoral)
Textos escritos por Rodrigo Pereira ou gerados com edição humana significativa. É o núcleo da personalidade da Natacha.

### Corpus ponte
Gerado por LLMs em conversas com Rodrigo Pereira. Os termos de uso dos provedores (Kimi, Grok, Qwen) permitem uso acadêmico dos outputs. Para uso comercial, o corpus deve ser regenerado com modelos open-weight (Qwen 2.5 Apache 2.0, Llama 3 Community License, Mistral Apache 2.0).

### Wikipédia curada
Extraída do dump oficial em português, filtrada por relevância de vocabulário e com acentos removidos. Licença CC BY-SA 3.0, que exige atribuição e compartilhamento sob a mesma licença.

## O que é distribuído no repositório

Nem todo o corpus vai no git. A separação evita distribuir material de terceiros sem necessidade e mantém o clone pequeno.

| Caminho | Distribuído? | Observação |
|---|---|---|
| `original/cenas_cotidianas/`, `corpus_felix_62.txt`, `corpus_casa_expandida.txt` | ✅ sim | Textos autorais |
| `original/corpus_natacha_220k.txt` | ❌ não | Gerado por expansão do corpus autoral |
| `original/corpus_opencode.txt` | ❌ não | Importado de fonte externa |
| `original/corpus_wikipedia_dominio.txt` | ❌ não | Importado da Wikipédia (CC BY-SA 3.0) |
| `limpo/`, `acentuado/` | ❌ não | Derivados, regeneráveis |
| Embeddings e pesos (`*.json`, `*.bin`) | ❌ não | Regeneráveis pelo treino |

Quem clona o repositório recebe o código e os textos autorais. Os corpora de terceiros e os derivados precisam ser obtidos ou regenerados conforme as licenças acima.

## Nota sobre uso do repositório

O **código** está sob licença MIT. O **corpus** tem licenças específicas por componente, conforme a tabela acima.

Se você clonar este repositório para fins acadêmicos, está tudo certo. Para uso comercial do corpus, consulte as licenças individuais e, quando aplicável, obtenha autorização.
