import re
from datasets import load_dataset

def bloco_valido(texto):
    texto = texto.strip()
    if len(texto) < 500:
        return False
    if re.match(r'^\d{1,2}\s+de\s+\w+\s+de\s+\d{4}', texto):
        return False
    if len(texto) < 100:
        return False
    if re.match(r'^\s*\d+\.\s', texto):
        return False
    return True

ds = load_dataset("wikimedia/wikipedia", "20231101.pt", split="train", streaming=True)

count = 0
for exemplo in ds:
    texto = exemplo.get("text", "").strip()
    blocos = [b for b in texto.split("\n\n") if bloco_valido(b)]
    if not blocos:
        continue
    texto_limpo = "\n\n".join(blocos)
    print(f"--- Artigo {count} (após filtro: {len(texto_limpo)} chars) ---")
    print(texto_limpo[:600])
    print()
    count += 1
    if count >= 5:
        break
