import random
from collections import defaultdict

with open('dados/embeddings/corpus_natacha_casa_300.txt', 'r') as f:
    tokens = f.read().lower().split()

bigrama = defaultdict(list)
for i in range(len(tokens) - 1):
    bigrama[tokens[i]].append(tokens[i+1])

random.seed(42)
for prompt in ['felix', 'natacha', 'casa', 'cozinha', 'ram', 'cpu']:
    palavra = prompt
    frase = [palavra]
    for _ in range(20):
        if palavra in bigrama:
            palavra = random.choice(bigrama[palavra])
            frase.append(palavra)
        else:
            break
    print(f'{prompt}: {" ".join(frase)}')
