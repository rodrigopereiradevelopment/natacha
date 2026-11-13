
---

## 2. `docs/requisitos/data_licenses.md`

```markdown
# Licenças de Dados

Este documento descreve a origem e a licença de cada componente do corpus da Natacha.

## Componentes do corpus

| Componente | Origem | Licença | Uso comercial |
|---|---|---|---|
| `corpus_natacha_220k.txt` | Rodrigo Pereira (autoral) | CC BY-SA 4.0 | Requer autorização |
| `natacha_corpus_ponte.txt` | Gerado por IA (Kimi, Grok, Qwen, Nemotron) | Uso acadêmico | Requer regeneração com modelo open-weight |
| `corpus_etapa1_wikipedia_v3.txt` | Wikimedia (dump 20231101.pt) | CC BY-SA 3.0 | Permitido com atribuição |
| Embeddings treinados | Derivados dos corpora acima | Mesma dos corpora | Requer revisão |

## Detalhes

### Corpus Natacha (autoral)
Textos escritos por Rodrigo Pereira ou gerados com edição humana significativa. É o núcleo da personalidade da Natacha.

### Corpus ponte
Gerado por LLMs em conversas com Rodrigo Pereira. Os termos de uso dos provedores (Kimi, Grok, Qwen) permitem uso acadêmico dos outputs. Para uso comercial, o corpus deve ser regenerado com modelos open-weight (Qwen 2.5 Apache 2.0, Llama 3 Community License, Mistral Apache 2.0).

### Wikipédia curada
Extraída do dump oficial em português, filtrada por relevância de vocabulário e com acentos removidos. Licença CC BY-SA 3.0, que exige atribuição e compartilhamento sob a mesma licença.

## Nota sobre uso do repositório

O **código** está sob licença MIT. O **corpus** tem licenças específicas por componente, conforme a tabela acima.

Se você clonar este repositório para fins acadêmicos, está tudo certo. Para uso comercial do corpus, consulte as licenças individuais e, quando aplicável, obtenha autorização.

---

## Histórico selecionado de embeddings (v21–v27)

Registro mantido para fins acadêmicos e para o TCC. O README apresenta apenas os resultados atuais da versão v39.

| Versão | Corpus/configuração | Dimensão | natacha ↔ casa |
|---|---|---|---|
| v21 | 220 mil tokens do núcleo Natacha | 32 | 0.31 |
| v22 | 315 mil tokens + corpus externo | 32 | 0.31 |
| v23D | 440 mil tokens duplicados | 32 | 0.30 |
| v24 | Corpus com problemas de qualidade | 64 | 0.31 |
| v25 | Corpus limpo | 64 | 0.31 |
| v26 | Sem conteúdo do Grok | 64 | 0.29 |
| v27 | Kimi + Grok | 64 | 0.419 |