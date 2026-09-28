import urllib.request
import unicodedata

# Montando a URL em partes para burlar o auto-completar do terminal
parte1 = "https://raw."
parte2 = "githubusercontent"
parte3 = ".com/alvations/cipars/master/cipars/test_data/temario.txt"
url_real = parte1 + parte2 + parte3

print("Baixando TeMário de forma direta...")

try:
    req = urllib.request.Request(url_real, headers={'User-Agent': 'Mozilla/5.0'})
    with urllib.request.urlopen(req) as response:
        texto = response.read().decode('utf-8')
    
    texto_limpo = texto.lower()
    texto_limpo = ''.join(c for c in unicodedata.normalize('NFD', texto_limpo) if unicodedata.category(c) != 'Mn')
    
    with open('dados/embeddings/corpus.txt', 'a', encoding='utf-8') as f:
        f.write('\n' + texto_limpo + '\n')
        
    print("SUCESSO: TeMário baixado, limpo e injetado na Natacha!")
except Exception as e:
    print("Erro ao processar:", e)
