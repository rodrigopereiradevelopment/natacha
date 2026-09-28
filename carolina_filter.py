from datasets import load_dataset

print("Baixando Carolina (taxonomia 'soc' - redes sociais)...")
ds = load_dataset(
    'carolina-c4ai/corpus-carolina',
    data_files='hf://datasets/carolina-c4ai/corpus-carolina@refs/convert/parquet/corpus/soc/*.parquet',
    split='train'
)

MIN_PALAVRAS = 50
MAX_PALAVRAS = 300
LIMITE_TOKENS = 30000

aceitos = []
tokens_total = 0

for ex in ds:
    texto = ex.get('text', '')
    palavras = texto.split()
    n = len(palavras)
    if n < MIN_PALAVRAS or n > MAX_PALAVRAS:
        continue
    aceitos.append(texto)
    tokens_total += n
    if tokens_total >= LIMITE_TOKENS:
        break

with open('carolina_bruto.txt', 'w', encoding='utf-8') as f:
    for t in aceitos:
        f.write(t + '\n')

print(f"Textos: {len(aceitos)}, tokens: {tokens_total}")
