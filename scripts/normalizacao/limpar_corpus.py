import os
import re
from difflib import SequenceMatcher

# ==============================================================================
# 1. CONFIGURAÇÕES DO SISTEMA
# ==============================================================================
# Pipeline: original/ -> limpo/
# A entrada é a fonte da verdade (nunca alterada); a saída é derivada.
ARQUIVO_ENTRADA = "dados/embeddings/original/corpus_natacha_220k.txt"
ARQUIVO_SAIDA = "dados/embeddings/limpo/corpus_natacha_limpo.txt"

# Limiar de similaridade (0.0 a 1.0). 
# 0.88 significa: se a frase for 88% parecida com uma já aprovada, ela é descartada.
LIMIAR_SIMILARIDADE = 0.88

# Número mínimo de palavras. 
# Aumente para 10 ou 12 na Fase 6/7 para cortar fatos soltos ("natacha tem 16 anos").
MIN_PALAVRAS = 10


# ==============================================================================
# 2. FUNÇÕES AUXILIARES
# ==============================================================================
def normalizar_texto(texto: str) -> str:
    """
    Prepara o texto APENAS PARA COMPARAÇÃO (não é salvo assim no arquivo final).
    - Converte para minúsculo
    - Remove pontuações (para que "Felix!" e "felix" sejam vistos como iguais)
    - Remove espaços extras
    """
    texto = texto.lower()
    texto = re.sub(r'[.,;:!?"\'()\[\]{}]', '', texto)
    texto = re.sub(r'\s+', ' ', texto).strip()
    return texto

def gerar_nome_saida(caminho_entrada: str) -> str:
    """
    Cria o nome do arquivo de saída. 
    Exemplo: 'corpus220k.txt' vira 'corpus220k_limpo.txt'.
    """
    base, extensao = os.path.splitext(caminho_entrada)
    return f"{base}_limpo{extensao}"

def eh_quase_identica(frase_norm: str, historico: list[str], limiar: float) -> bool:
    """
    Compara a frase atual com as frases já aprovadas usando o algoritmo SequenceMatcher.
    Usa uma otimização de tamanho para evitar comparações inúteis (ex: comparar frase de 5 letras com uma de 100).
    """
    len_atual = len(frase_norm)
    if len_atual == 0:
        return True

    for frase_existente in historico:
        len_exist = len(frase_existente)
        
        # OTIMIZAÇÃO: Se a diferença de tamanho for muito grande, é impossível atingir o limiar.
        # Ex: Se limiar é 0.88, a diferença de tamanho não pode ser maior que 12%.
        if abs(len_atual - len_exist) / max(len_atual, len_exist) > (1.0 - limiar):
            continue

        # Calcula a similaridade real (só roda se a otimização acima permitir)
        similaridade = SequenceMatcher(None, frase_norm, frase_existente).ratio()
        if similaridade >= limiar:
            return True

    return False


# ==============================================================================
# 3. FLUXO PRINCIPAL DE LIMPEZA
# ==============================================================================
def limpar_corpus(arquivo_entrada: str, arquivo_saida=None):
    # Verifica se o arquivo original existe antes de começar
    if not os.path.exists(arquivo_entrada):
        print(f"[ERRO] Arquivo não encontrado: {arquivo_entrada}")
        return

    # Sem saída explícita, mantém o comportamento antigo (sufixo "_limpo"
    # ao lado da entrada). Com saída explícita, respeita o layout do projeto.
    if arquivo_saida is None:
        arquivo_saida = gerar_nome_saida(arquivo_entrada)

    os.makedirs(os.path.dirname(arquivo_saida) or ".", exist_ok=True)

    # Lê todo o conteúdo original (sem alterar o arquivo no disco)
    with open(arquivo_entrada, "r", encoding="utf-8") as f:
        linhas_originais = f.readlines()

    # Estruturas de memória para controle de duplicatas
    linhas_limpas = []           # Onde as frases aprovadas (com pontuação original) são salvas
    vistas_exatas = set()        # Set para busca instantânea de duplicatas exatas (O(1))
    historico_normalizado = []   # Lista para o SequenceMatcher varrer (busca linear)

    # Contadores para o relatório final (útil para o TCC)
    total_lidas = 0
    descartadas_curtas = 0
    descartadas_duplicadas = 0

    print(f"Processando '{arquivo_entrada}'...")

    # Loop que passa por cada linha do arquivo original
    for linha in linhas_originais:
        linha_bruta = linha.strip()
        
        # Pula linhas em branco
        if not linha_bruta:
            continue

        total_lidas += 1
        
        # Cria a versão normalizada apenas para comparação
        frase_norm = normalizar_texto(linha_bruta)
        qtd_palavras = len(frase_norm.split())

        # FILTRO 1: Tamanho mínimo (com proteção para diálogos)
        # Verifica se a linha tem dois-pontos, travessão ou aspas. Se tiver, é diálogo.
        eh_dialogo = any(c in linha_bruta for c in [':', '—', '-', '"'])
        
        # Se for menor que MIN_PALAVRAS e NÃO for diálogo, descarta.
        if qtd_palavras < MIN_PALAVRAS and not eh_dialogo:
            descartadas_curtas += 1
            continue

        # FILTRO 2: Duplicata Exata
        # Se a frase normalizada já existe no set, descarta imediatamente.
        if frase_norm in vistas_exatas:
            descartadas_duplicadas += 1
            continue

        # FILTRO 3: Quase Duplicata (Similaridade)
        # Compara com o histórico de frases aprovadas.
        if eh_quase_identica(frase_norm, historico_normalizado, LIMIAR_SIMILARIDADE):
            descartadas_duplicadas += 1
            continue

        # Se passou por todos os filtros, a frase é aprovada!
        vistas_exatas.add(frase_norm)
        historico_normalizado.append(frase_norm)
        
        # PULO DO GATO: Salva a linha ORIGINAL (linha_bruta), com acento, maiúscula e pontuação.
        linhas_limpas.append(linha_bruta)

    # Grava o novo arquivo limpo preservando o original intacto
    with open(arquivo_saida, "w", encoding="utf-8") as f:
        for linha in linhas_limpas:
            f.write(linha + "\n")

    # Relatório final no terminal (Tire um print para o TCC!)
    print("-" * 50)
    print(f"Concluído! Arquivo salvo em: {arquivo_saida}")
    print(f"Linhas válidas lidas:     {total_lidas}")
    print(f"Descartadas (curtas):     {descartadas_curtas}")
    print(f"Descartadas (repetidas):  {descartadas_duplicadas}")
    print(f"Linhas finais mantidas:   {len(linhas_limpas)}")
    print("-" * 50)


# ==============================================================================
# 4. PONTO DE ENTRADA
# ==============================================================================
if __name__ == "__main__":
    limpar_corpus(ARQUIVO_ENTRADA, ARQUIVO_SAIDA)