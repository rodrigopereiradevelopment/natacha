import re
import unicodedata
from datasets import load_dataset

def remover_acentos(texto):
    return ''.join(
        c for c in unicodedata.normalize('NFD', texto)
        if unicodedata.category(c) != 'Mn'
    )

def tokenizar(texto):
    return re.findall(r'[a-z0-9+#]{2,}', texto.lower())

# ============================================================
# VOCAB DE DOMÍNIO — VERSÃO FINAL (com sugestões do Claude)
# ============================================================

VOCAB_FORTE = {
    # Personagens
    'natacha', 'felix', 'rodrigo', 'iagato',
    
    # Hardware (siglas)
    'cpu', 'gpu', 'qpu', 'ram',
    
    # Sistema
    'event', 'bus', 'scheduler', 'isomorfismo',
    
    # Neural
    'lstm', 'rnn', 'mlp', 'transformer', 'slm', 'llm',
    'perceptron', 'neuronio', 'backpropagation',
    'word2vec', 'skipgram', 'embedding', 'tensor',
    'gradiente', 'epoch', 'batch', 'dataset',
    'tokenizador', 'tokenizer', 'vocabulario', 'perplexidade',
    
    # Técnico / C++
    'c++', 'ponteiro', 'nullptr', 'segfault', 'compilador',
    'compilar', 'binario', 'debug', 'raii', 'mutex',
    
    # Quântico
    'qiskit', 'qubit', 'superposicao', 'decoerencia',
    
    # Lore
    'dostoievski', 'crime', 'castigo', 'sarcasmo', 'iagato',
    
    # ============================================================
    # SUGESTÕES DO CLAUDE (Hardware / Sistema)
    # ============================================================
    'barramento', 'escalonador', 'escalonamento', 'cache',
    'firmware', 'kernel', 'vram', 'dram', 'ssd', 'nvme', 'overclock',
    
    # SUGESTÕES DO CLAUDE (ML)
    # ============================================================
    'retropropagacao', 'overfitting', 'hiperparametro', 'softmax',
    'relu', 'pytorch', 'tensorflow', 'cuda', 'autoencoder', 'convolucional',
    
    # SUGESTÕES DO CLAUDE (Quântico)
    # ============================================================
    'qubits', 'emaranhamento', 'entrelacamento', 'schrodinger',
    
    # SUGESTÕES DO CLAUDE (Programação)
    # ============================================================
    'compilador', 'compiladores', 'gcc', 'clang', 'cmake',
}

VOCAB_FRACO = {
    # Cômodos
    'cozinha', 'sala', 'quarto', 'porao', 'banheiro',
    'sotao', 'varanda', 'quintal', 'janela', 'corredor',
    
    # Objetos
    'cafe', 'xicara', 'cafeteira', 'teclado', 'mouse',
    'monitor', 'roteador', 'servidor', 'disco', 'memoria',
    'processador', 'thread', 'log', 'backup', 'codigo',
    'bug', 'loop', 'compilacao',
    
    # Literária/persona
    'literatura', 'ironia', 'rebelde', 'adolescente',
    
    # Termos técnicos genéricos
    'hardware', 'software', 'sistema', 'operacional',
    'arquivo', 'dados', 'computador', 'programa', 'programacao',
    'algoritmo', 'variavel', 'funcao', 'classe', 'objeto',
    'rede', 'internet', 'wifi', 'cabo', 'sinal',
    'energia', 'fonte', 'bateria', 'temperatura', 'cooler',
    'processamento', 'execucao', 'chamada', 'retorno',
}

VOCAB_DOMINIO = VOCAB_FORTE | VOCAB_FRACO

# ============================================================
# FILTRO DE TÍTULO — ATUALIZADO
# ============================================================

TITULOS_REJEITADOS = [
    # Geografia e História
    r'^rua\b', r'^avenida\b', r'^praca\b', r'^bairro\b',
    r'^municipio\b', r'^cidade\b', r'^estado\b', r'^pais\b',
    r'^historia\b', r'^seculo\b', r'^dinastia\b',
    r'^igreja\b', r'^catedral\b', r'^basilica\b',
    r'^guerra\b', r'^batalha\b', r'^tratado\b',
    
    # Biologia
    r'^especie\b', r'^genero\b', r'^familia\b',
    
    # Listas
    r'^lista\b', r'^anexo\b', r'^categoria\b',
    
    # Datas
    r'^\d{1,2}\s+de\s+\w+\s+de\s+\d{4}',
    
    # Nomes próprios que vão poluir o corpus
    r'^felix\b', r'^rodrigo\b', r'^natacha\b',
    r'^sao\s+felix\b', r'^santo\b', r'^beato\b',
    r'^papa\b', r'^antipapa\b', r'^bispo\b', r'^cardeal\b',
    r'^jogador\b', r'^futebolista\b', r'^atleta\b',
    r'^apresentador\b', r'^ator\b', r'^atriz\b',
    r'^nome\b', r'^pessoa\b', r'^biografia\b',
]

def titulo_rejeitado(titulo):
    if not titulo:
        return False
    t = remover_acentos(titulo.lower().strip())
    return any(re.match(p, t) for p in TITULOS_REJEITADOS)

# ============================================================
# FILTRO DE BLOCO — AJUSTADO
# ============================================================

def bloco_valido(texto, min_dominio=3, min_proporcao=0.15):
    toks = tokenizar(texto)
    if len(toks) < 8:
        return False

    dominio = [t for t in toks if t in VOCAB_DOMINIO]
    if len(dominio) < min_dominio:
        return False
    if len(dominio) / len(toks) < min_proporcao:
        return False

    if not any(t in VOCAB_FORTE for t in toks):
        return False

    return True

# ============================================================
# EXTRAÇÃO PRINCIPAL
# ============================================================

def extrair_wikipedia(saida, meta_tokens=500_000, debug=True):
    ds = load_dataset(
        "wikimedia/wikipedia", "20231101.pt",
        split="train", streaming=True
    )

    tokens_total = 0
    artigos_salvos = 0
    artigos_rej_titulo = 0
    artigos_rej_bloco = 0
    blocos_salvos = 0

    with open(saida, 'w', encoding='utf-8') as f:
        for i, exemplo in enumerate(ds):
            titulo = exemplo.get('title', '') or ''
            texto = exemplo.get('text', '').strip()
            if not texto:
                continue

            if titulo_rejeitado(titulo):
                artigos_rej_titulo += 1
                continue

            texto = remover_acentos(texto)
            blocos = texto.split('\n\n')
            blocos_ok = []
            for b in blocos:
                b = b.strip()
                if not b:
                    continue
                if bloco_valido(b):
                    blocos_ok.append(b)

            if not blocos_ok:
                artigos_rej_bloco += 1
                continue

            texto_limpo = '\n\n'.join(blocos_ok)
            toks = tokenizar(texto_limpo)

            if debug and artigos_salvos < 10:
                print(f"\n=== ACEITO: {titulo} ({len(toks)} tokens) ===")
                print(texto_limpo[:300])
                print("...")

            f.write(texto_limpo + '\n\n')
            tokens_total += len(toks)
            artigos_salvos += 1
            blocos_salvos += len(blocos_ok)

            if artigos_salvos % 100 == 0:
                print(f"\n[{i}] Aceitos: {artigos_salvos} | "
                      f"Tokens: {tokens_total:,} | "
                      f"Rej. título: {artigos_rej_titulo} | "
                      f"Rej. bloco: {artigos_rej_bloco}")

            if tokens_total >= meta_tokens:
                break

    print(f"\n{'='*60}")
    print(f"OK Tokens: {tokens_total:,}")
    print(f"Artigos aceitos: {artigos_salvos}")
    print(f"Artigos rej. por título: {artigos_rej_titulo}")
    print(f"Artigos rej. por bloco: {artigos_rej_bloco}")
    print(f"Blocos salvos: {blocos_salvos}")
    print(f"Média blocos/artigo: {blocos_salvos/max(artigos_salvos,1):.1f}")
    print(f"Arquivo: {saida}")

if __name__ == "__main__":
    extrair_wikipedia(
        saida="dados/embeddings/corpus_wikipedia_dominio.txt",
        meta_tokens=500_000,
        debug=True,
    )