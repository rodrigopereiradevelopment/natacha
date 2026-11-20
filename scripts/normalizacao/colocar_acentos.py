import re
from pathlib import Path
import language_tool_python

# ==============================================================================
# 1. CONFIGURAÇÕES
# ==============================================================================
# Pipeline: limpo/ -> acentuado/  (os relatorios vao para relatorios/)
ENTRADA = "dados/embeddings/limpo/corpus_natacha_limpo.txt"
SAIDA = "dados/embeddings/acentuado/corpus_natacha_acentuado.txt"
RELATORIO_APLICADAS = "dados/embeddings/relatorios/acentos_aplicados.txt"
RELATORIO_REJEITADAS = "dados/embeddings/relatorios/sugestoes_rejeitadas.txt"

# Lista fechada: palavras mais comuns que o LanguageTool pode errar.
SUBSTITUICOES = {
    "nao": "não", "voce": "você", "sao": "são", "porao": "porão",
    "cafe": "café", "comodo": "cômodo", "comodos": "cômodos",
    "logica": "lógica", "memoria": "memória", "memorias": "memórias",
    "sotao": "sótão", "quantico": "quântica", "epocas": "épocas",
}

# Regras que queremos que o LT aplique (ortografia/acento)
REGRAS_PERMITIDAS = ("PT_SIMPLE_REPLACE_ORTHOGRAPHY", "MORFOLOGIK_RULE_PT_BR")

# Regras que o LT sugere mas NÃO queremos aplicar (vão para o relatório manual)
REGRAS_BLOQUEADAS = (
    "UPPERCASE_SENTENCE_START", 
    "GOOGLE", 
    "CONFUSÃO_E_É", 
    "CONFUSÃO_ATE_ATÉ", 
    "NAO_E_ADJ"
)

# ==============================================================================
# 2. FUNÇÕES AUXILIARES
# ==============================================================================
MAPA_SEM_ACENTO = str.maketrans("áàâãéêíóôõúüçÁÀÂÃÉÊÍÓÔÕÚÜÇ", "aaaaeeiooouucAAAAEEIOOOUUC")

def sem_acento(texto: str) -> str:
    return texto.translate(MAPA_SEM_ACENTO)

def so_acentos(original: str, corrigido: str) -> bool:
    # O PULO DO GATO: Remove espaços e pontuações ANTES de comparar.
    # Assim, se o LT mudar um espaço ou um ponto, a gente ignora e aceita a mudança de acento.
    limpar = lambda s: re.sub(r'[\s.,;:!?]', '', sem_acento(s).lower())
    return limpar(original) == limpar(corrigido)

def aplicar_lista_fechada(linha: str) -> str:
    for errada, certa in SUBSTITUICOES.items():
        def trocar(m):
            palavra = m.group(0)
            return certa.capitalize() if palavra[0].isupper() else certa
        linha = re.sub(rf"(?<!\w){errada}(?!\w)", trocar, linha, flags=re.IGNORECASE)
    return linha

# ==============================================================================
# 3. LANGUAGETOOL, COM FILTRO E BLACKLIST
# ==============================================================================
tool = language_tool_python.LanguageTool("pt-BR")

def aplicar_languagetool(linha: str):
    rejeitadas = []
    matches = tool.check(linha)

    # Filtra: Tem que ser uma regra permitida E NÃO pode estar na blacklist
    permitidas = [
        m for m in matches 
        if m.rule_id.startswith(REGRAS_PERMITIDAS) 
        and not any(bloqueada in m.rule_id for bloqueada in REGRAS_BLOQUEADAS)
    ]

    # Guarda as rejeitadas (incluindo e/é e maiúsculas) para revisão
    for m in matches:
        if m not in permitidas and m.replacements:
            trecho = linha[m.offset:m.offset+m.error_length]
            rejeitadas.append(f"[{m.rule_id}] '{trecho}' -> {m.replacements[:2]}")

    # Aplica APENAS as regras permitidas
    corrigida = language_tool_python.utils.correct(linha, permitidas)

    # Se, tirando acentos e pontuação, a frase ainda mudou, a palavra foi alterada. Rejeita a linha inteira.
    if not so_acentos(linha, corrigida):
        rejeitadas.append(f"[LINHA REJEITADA - mudou palavra] {linha} -> {corrigida}")
        return linha, rejeitadas

    return corrigida, rejeitadas

# ==============================================================================
# 4. FLUXO PRINCIPAL
# ==============================================================================
def processar():
    linhas = Path(ENTRADA).read_text(encoding="utf-8").splitlines()
    saida, aplicadas, rejeitadas = [], [], []

    for linha in linhas:
        if not linha.strip():
            continue

        # Passo 1: Lista fechada (seguro e instantâneo)
        nova = aplicar_lista_fechada(linha)

        # Passo 2: LanguageTool para o resto
        nova, rej = aplicar_languagetool(nova)

        if nova != linha:
            aplicadas.append(f"{linha}\n -> {nova}")
        rejeitadas.extend(rej)
        saida.append(nova)

    Path(SAIDA).write_text("\n".join(saida) + "\n", encoding="utf-8")
    Path(RELATORIO_APLICADAS).write_text("\n\n".join(aplicadas), encoding="utf-8")
    Path(RELATORIO_REJEITADAS).write_text("\n".join(rejeitadas), encoding="utf-8")

    print(f"Linhas: {len(saida)} | alteradas: {len(aplicadas)} | sugestões rejeitadas: {len(rejeitadas)}")

if __name__ == "__main__":
    try:
        processar()
    finally:
        tool.close()