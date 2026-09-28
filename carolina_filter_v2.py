from datasets import load_dataset

print("Baixando Carolina (streaming, taxonomia soc)...")

ds = load_dataset(
    'carolina-c4ai/corpus-carolina',
    'default',
    split='corpus',
    streaming=True,
    trust_remote_code=True,
)

MIN_PALAVRAS = 50
MAX_PALAVRAS = 300
LIMITE_TOKENS = 30000

aceitos = []
tokens_total = 0

for ex in ds:
    texto = ex.get('text', '')
    if not texto:
        continue
    palavras = texto.split()
    n = len(palavras)
    if n < MIN_PALAVRAS or n > MAX_PALAVRAS:
        continue
    aceitos.append(texto)
    tokens_total += n
    if len(aceitos) % 100 == 0:
        print(f"Aceitos: {len(aceitos)}, tokens: {tokens_total}")
    if tokens_total >= LIMITE_TOKENS:
        break

with open('carolina_bruto.txt', 'w', encoding='utf-8') as f:
    for t in aceitos:
        f.write(t + '\n')

print(f"FINAL: Textos: {len(aceitos)}, tokens: {tokens_total}")
