import urllib.request
import urllib.parse
import json
import unicodedata
import time
import os

print("Baixando artigos completos da Wikipédia PT (com rate limit)...")

UA = {'User-Agent': 'NatachaCorpus/1.0 (contato: rodrigo@example)'}

# Continua de onde parou (se já tem arquivo)
aceitos = []
tokens_total = 0
if os.path.exists('carolina_light.txt'):
    with open('carolina_light.txt', encoding='utf-8') as f:
        for linha in f:
            aceitos.append(linha.strip())
            tokens_total += len(linha.split())
    print(f"Retomando: {len(aceitos)} artigos, {tokens_total} tokens")

LIMITE = 30000
tentativas_429 = 0

while tokens_total < LIMITE and len(aceitos) < 300:
    # 1. Pega 10 títulos aleatórios
    url_lista = "https://pt.wikipedia.org/w/api.php?action=query&list=random&rnnamespace=0&rnlimit=10&format=json"
    try:
        req = urllib.request.Request(url_lista, headers=UA)
        with urllib.request.urlopen(req, timeout=15) as r:
            data = json.load(r)
            titulos = [p['title'] for p in data['query']['random']]
        tentativas_429 = 0
    except urllib.error.HTTPError as e:
        if e.code == 429:
            tentativas_429 += 1
            espera = min(60, 5 * tentativas_429)
            print(f"429 — esperando {espera}s...")
            time.sleep(espera)
            continue
        else:
            time.sleep(2)
            continue
    except Exception as e:
        print(f"Erro lista: {e}")
        time.sleep(2)
        continue
    
    # 2. Conteúdo de cada título
    for titulo in titulos:
        if tokens_total >= LIMITE:
            break
        try:
            titulo_enc = urllib.parse.quote(titulo.replace(' ', '_'))
            url_pagina = f"https://pt.wikipedia.org/w/api.php?action=query&prop=extracts&explaintext=1&format=json&titles={titulo_enc}"
            req = urllib.request.Request(url_pagina, headers=UA)
            with urllib.request.urlopen(req, timeout=10) as r:
                data = json.load(r)
                pages = data.get('query', {}).get('pages', {})
                texto = ''
                for pid, pag in pages.items():
                    texto = pag.get('extract', '')
                    break
            
            if not texto or len(texto) < 500:
                continue
            
            texto = texto.lower()
            texto = ''.join(c for c in unicodedata.normalize('NFD', texto)
                            if unicodedata.category(c) != 'Mn')
            
            palavras = texto.split()
            if len(palavras) < 100:
                continue
            
            aceitos.append(texto)
            tokens_total += len(palavras)
            
            # Salva progresso a cada 3 artigos
            if len(aceitos) % 3 == 0:
                with open('carolina_light.txt', 'w', encoding='utf-8') as f:
                    for t in aceitos:
                        f.write(t + '\n')
                print(f"Artigos: {len(aceitos)}, tokens: {tokens_total}")
            
            time.sleep(1)  # 1 segundo entre artigos
        
        except urllib.error.HTTPError as e:
            if e.code == 429:
                print("429 — esperando 30s...")
                time.sleep(30)
                continue
            else:
                time.sleep(1)
                continue
        except Exception as e:
            continue
    
    time.sleep(2)  # 2 segundos entre lotes

with open('carolina_light.txt', 'w', encoding='utf-8') as f:
    for t in aceitos:
        f.write(t + '\n')

print(f"FINAL: {len(aceitos)} artigos, {tokens_total} tokens")
