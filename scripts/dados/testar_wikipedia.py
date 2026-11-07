from datasets import load_dataset

ds = load_dataset("wikimedia/wikipedia", "20231101.pt", split="train", streaming=True)

for i, exemplo in enumerate(ds):
    if i >= 3:
        break
    texto = exemplo["text"]
    print(f"--- Artigo {i} ({len(texto)} chars) ---")
    print(texto[:400])
    print()
