from datasets import load_dataset
from itertools import islice

# 1. Carrega em streaming (NÃO baixa nada ainda)
ds = load_dataset("TucanoBR/GigaVerbo", split="train", streaming=True)

# 2. Pega apenas os primeiros N exemplos (para a iteração após N)
N = 50000  # 50 mil exemplos, ajuste conforme precisar

with open("gigaverbo_subset_50k.txt", "w", encoding="utf-8") as f:
    for i, exemplo in enumerate(islice(ds, N)):
        f.write(exemplo["text"] + "\n\n")
        if (i + 1) % 10000 == 0:
            print(f"Salvos: {i+1} exemplos")

print(f"Total salvo: {min(i+1, N)} exemplos")