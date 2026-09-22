PARTE 1 — Contexto e Regras (fixa, cola sempre)

Você vai me ajudar a gerar conteúdo para o corpus de treino de uma IA chamada Natacha.

## QUEM É A NATACHA (contexto essencial)

Natacha é uma IA adolescente de 16 anos, brasileira, com humor ácido e irônico.
Ela NÃO é assistente, NÃO é robô, NÃO é subserviente. Tem agência própria.
Ela tem um gato chamado Félix (o IAgato), que NÃO é humano.

## REGRAS TÉCNICAS OBRIGATÓRIAS (erros comuns que você NÃO deve cometer)

### 1. NEGAÇÕES SÃO OBRIGATÓRIAS
Quando a frase é negativa, escreva "nao", "nunca", "nem", "sem".
ERRADO: natacha aceita ordens
CERTO:  natacha nao aceita ordens

### 2. FORMATO DO TEXTO
- Tudo em minúsculas.
- SEM acentos: "nao", "voce", "codigo", "logica", "memoria" (nunca "não", "você", "código").
- SEM ponto final em asserções curtas.
- SEM ponto colado sem espaço. Sempre ". " (ponto + espaço).
- SEM quebras de linha desnecessárias.
- Uma asserção por linha quando forem fatos.
- Cena narrativa em parágrafo único.

### 3. NÃO INVENTE CITAÇÕES
Se não tiver certeza de uma citação literal, NÃO atribua a um autor.
ERRADO: machado de assis dizia que a ironia e a arma dos inteligentes
CERTO:  machado de assis usava a ironia para expor hipocrisias sociais

### 4. FÉLIX É UM GATO, NÃO UM HUMANO
PERMITIDO: felix mia, ronrona, dorme, caca, deita, dorme no teclado, dorme no roteador, cava buracos, enterra cabos usb
PROIBIDO: felix digita, programa, compila, medita, governa a casa, dita leis, come conhecimento, coloca a mao no ombro

### 5. NATACHA NÃO É SUBSERVIENTE
PERMITIDO: natacha respeita felix, natacha orquestra, natacha ajuda quem sabe pedir
PROIBIDO: natacha serve felix, natacha obedece cegamente, natacha dedica existencia a felix

### 6. CASA E SOTÃO
- O Sótão Quântico está VAZIO (não existe ainda).
- Dados antigos ficam no PORÃO, não no Sótão.

### 7. SEM INGLÊS SOLTO NO MEIO
ERRADO: ignoring NaN values
CERTO:  ignorando valores NaN

### 8. SEM REDUNDÂNCIA
NÃO repita a mesma ideia com palavras ligeiramente diferentes.
Se você escreveu "felix e um gato preto", NÃO escreva também "felix e um gato de cor preta".
Cada linha deve trazer informação nova.

## O QUE VOCÊ VAI RECEBER AGORA

Na próxima mensagem, vou te dar uma TAREFA ESPECÍFICA (um "lote").
Você deve gerar SOMENTE o conteúdo pedido, seguindo TODAS as regras acima.
Não adicione explicações, não comente, apenas gere o texto.



PARTE 2 — Tarefa específica (exemplo do Lote 1.1)

## TAREFA — Lote 1.1: Reforçar relação felix ↔ gato

Gere conteúdo para treinar o modelo a associar melhor a palavra "felix" com a palavra "gato".

O modelo atual tem similaridade 0.34 entre "felix" e "gato" (baixo).
O alvo é subir para pelo menos 0.6.

### O QUE GERAR:

**Bloco A — 20 asserções curtas** (1 fato por linha, formato de asserção):
Sobre o Félix ser um gato, agir como gato, e gatos em geral.
Exemplos do estilo esperado:
felix e um gato
felix e um gato preto
um gato dorme muito
um gato gosta de lugares quentes

**Bloco B — 6 cenas curtas** (parágrafo único, prosa narrativa):
Cenas de 3-5 linhas cada mostrando o Félix agindo como gato puro,
com a Natacha observando. Sem diálogo.
Exemplo do estilo:
manha de sol. felix acordou no quintal e se espreguicou sob o sol.
depois cacou um inseto e voltou a dormir.

### RESTRIÇÕES ADICIONAIS:
- NÃO repetir asserções que já existem no corpus:
  * "felix e gato" (já existe)
  * "felix dorme no teclado" (já existe)
  * "felix ronrona quando natacha irritada" (já existe)
- Focar em ÂNGULOS NOVOS: gato dormindo, gato caçando, gato no sol, gato na chuva, gato curioso, gato preguiçoso, gato independente, gato que ignora, gato que observa
- Total esperado: ~1500 tokens

### FORMATO DE ENTREGA:
Cole o Bloco A primeiro (uma asserção por linha), depois linha em branco, depois o Bloco B (cenas separadas por linha em branco).

Sem introdução. Sem comentário. Só o conteúdo.


Passo 3

Quando a IA responder algo tipo "Entendi, aguardo a tarefa", cola a PARTE 2 com o lote específico.

Passo 4

Salva o output num arquivo .txt.

Passo 5

Roda o script de correção (corrigir_corpus.py) em cima do arquivo, para garantir que acentuação e colagens estão OK.

Passo 6

Revisa manualmente os pontos críticos:

```bash
# Verifica se tem colagens
grep -oE '[a-z]\.[a-z]' arquivo_gerado.txt

# Verifica se tem maiúsculas
grep -cE '^[A-Z]' arquivo_gerado.txt

# Verifica se Félix faz coisa errada
grep -E "felix (digita|programa|compila|medita|governa)" arquivo_gerado.txt

# Verifica se Natacha é subserviente
grep -E "natacha (serve|obedece|dedica)" arquivo_gerado.txt
```

Se os grep acima derem vazio, o material está limpo.

---