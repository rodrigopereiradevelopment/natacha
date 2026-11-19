import re
from pathlib import Path

# ==============================================================================
# 1. CONFIGURAÇÕES
# ==============================================================================
ENTRADA = "dados/embeddings/limpo/corpus_natacha_limpo.txt"
SAIDA = "dados/embeddings/acentuado/corpus_natacha_acentuado.txt"
RELATORIO = "dados/embeddings/acentuado/relatorio_acentos.txt"

# Dicionário expandido: palavra sem acento -> forma correta.
# A lista está em ordem alfabética para facilitar a manutenção.
SUBSTITUICOES = {
    "acido": "ácido",
    "alem": "além",
    "almocando": "almoçando",
    "almoco": "almoço",
    "amanha": "amanhã",
    "atencao": "atenção",
    "ativacao": "ativação",
    "basica": "básica",
    "classificacao": "classificação",
    "comeca": "começa",
    "computacao": "computação",
    "decisoes": "decisões",
    "decoerencia": "decoerência",
    "didatica": "didática",
    "diferenca": "diferença",
    "discussao": "discussão",
    "edicao": "edição",
    "entrelacamento": "entrelaçamento",
    "epocas": "épocas",
    "estao": "estão",
    "exigencias": "exigências",
    "frustacao": "frustração",
    "funcoes": "funções",
    "inicializacao": "inicialização",
    "inegociaveis": "inegociáveis",
    "interpretacao": "interpretação",
    "ironico": "irônico",
    "logica": "lógica",
    "matematica": "matemática",
    "maxima": "máxima",
    "memoria": "memória",
    "memorias": "memórias",
    "nao": "não",
    "neuronio": "neurônio",
    "observacao": "observação",
    "paciencia": "paciência",
    "parametro": "parâmetro",
    "parametros": "parâmetros",
    "porao": "porão",
    "poroes": "porões",
    "portugues": "português",
    "propria": "própria",
    "proprio": "próprio",
    "proprios": "próprios",
    "proxima": "próxima",
    "quantica": "quântica",
    "quantico": "quântico",
    "revolucao": "revolução",
    "sao": "são",
    "satisfacao": "satisfação",
    "seguranca": "segurança",
    "sequencias": "sequências",
    "sotao": "sótão",
    "superposicao": "superposição",
    "tambem": "também",
    "tedio": "tédio",
    "territorio": "território",
    "unica": "única",
    "unico": "único",
    "usuario": "usuário",
    "vizinhanca": "vizinhança",
    "vocabulario": "vocabulário",
    "voce": "você",
    "voces": "vocês",
    "ate": "até",
      "tecnico": "técnico",
    "tecnica": "técnica",
    "fisico": "físico",
    "medio": "médio",
    "possivel": "possível",
    "pratica": "prática",
    "conteudo": "conteúdo",
    "tres": "três",
    "sera": "será",
    "alguem": "alguém",
    "ninguem": "ninguém",
    "silencio": "silêncio",
    "espaco": "espaço",
    "codigo": "código",
    "musica": "música",
    "historia": "história",
    "funcao": "função",
    "versao": "versão",
    "pagina": "página",
    "ultimo": "último",  
}

# ==============================================================================
# 2. FUNÇÕES
# ==============================================================================
def aplicar_substituicoes(linha: str) -> str:
    """
    Troca cada palavra da lista, usando fronteira de palavra (?<!\\w) e (?!\\w)
    para não mexer em pedaços de outras palavras (ex.: 'nao' dentro de 'nacao').
    Preserva a inicial maiúscula se a palavra original tinha.
    """
    for errada, certa in SUBSTITUICOES.items():
        padrao = rf"(?<!\w){re.escape(errada)}(?!\w)"

        def trocar(m, certa=certa):
            original = m.group(0)
            # Se a palavra original começava com maiúscula, mantém a maiúscula
            if original[0].isupper():
                return certa[0].upper() + certa[1:]
            return certa

        linha = re.sub(padrao, trocar, linha, flags=re.IGNORECASE)
    return linha

# ==============================================================================
# 3. FLUXO PRINCIPAL
# ==============================================================================
def processar():
    linhas = Path(ENTRADA).read_text(encoding="utf-8").splitlines()
    saida, relatorio = [], []
    alteradas = 0

    for linha in linhas:
        if not linha.strip():
            continue

        nova = aplicar_substituicoes(linha)
        saida.append(nova)

        if nova != linha:
            alteradas += 1
            relatorio.append(f"ANTES: {linha}\nDEPOIS: {nova}\n")

    Path(SAIDA).write_text("\n".join(saida) + "\n", encoding="utf-8")
    Path(RELATORIO).write_text("\n".join(relatorio), encoding="utf-8")
    print(f"Linhas: {len(saida)} | alteradas: {alteradas}")
    print(f"Relatório: {RELATORIO}")

if __name__ == "__main__":
    processar()