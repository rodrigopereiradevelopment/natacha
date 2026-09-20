# Relatório de Avaliação — Natacha Embeddings

**Data:** 20/09/2026 15:17

**Modelo:** `../../dados/embeddings/natacha_embeddings.json`
**Vocabulário:** 5563 palavras
**Dimensão:** 64

---

## 1. Similaridade entre Pares

### 1.1 Cômodos da Casa

| Par | Similaridade | Status |
|-----|--------------|--------|
| CPU → cozinha | 0.9096 | ✅ |
| GPU → sala | 0.9210 | ✅ |
| RAM → quarto | 0.9239 | ✅ |
| disco → porão | 0.8317 | ✅ |
| rede → janela | 0.6590 | ✅ |
| corredor → eventos | 0.2894 | ❌ |
| sótão → quântico | 0.9157 | ✅ |
| Felix → quintal | 0.5831 | ⚠️ |
| Casa → Natacha | 0.1119 | ❌ |

### 1.2 Personagens

| Par | Similaridade | Status |
|-----|--------------|--------|
| Felix ↔ Natacha | 0.3629 | ❌ |
| Natacha ↔ Rodrigo | 0.3285 | ❌ |
| robô ↔ assistente | 0.3920 | ❌ |
| robô ↔ obedece | 0.5010 | ⚠️ |
| robô ↔ detesta | 0.4137 | ⚠️ |
| Natacha ↔ Felix | 0.3629 | ❌ |

### 1.3 Hardware

| Par | Similaridade | Status |
|-----|--------------|--------|
| CPU ↔ GPU | 0.6666 | ✅ |
| CPU ↔ RAM | 0.4212 | ⚠️ |
| GPU ↔ RAM | 0.4116 | ⚠️ |

### 1.4 Conceitos Técnicos

| Par | Similaridade | Status |
|-----|--------------|--------|
| MLP ↔ neurônio | 0.6084 | ✅ |
| c++ ↔ código | 0.3404 | ❌ |
| embeddings ↔ palavras | 0.3356 | ❌ |
| café ↔ processamento | 0.3421 | ❌ |
| Felix ↔ gato | 0.3388 | ❌ |
| json ↔ memória | 0.4122 | ⚠️ |
| sótão ↔ quântico | 0.9157 | ✅ |


## 2. Analogias Vetoriais

**Taxa de acerto top-1:** 10/12 (83.3%)

### 2.1 Analogias de Casa

### cpu:cozinha :: gpu:?
Esperado: **sala**
1. `sala` — sim=0.8936 ★
2. `agrega` — sim=0.5892
3. `massivos` — sim=0.5743

### ram:quarto :: cpu:?
Esperado: **cozinha**
1. `cozinha` — sim=0.8171 ★
2. `sala` — sim=0.6105
3. `gpu` — sim=0.5780

### gpu:sala :: ram:?
Esperado: **quarto**
1. `quarto` — sim=0.8853 ★
2. `enquanto` — sim=0.5836
3. `porao` — sim=0.5783

### disco:porao :: ram:?
Esperado: **quarto**
1. `quarto` — sim=0.8542 ★
2. `fila` — sim=0.5886
3. `expansivel` — sim=0.5481

### rede:janela :: gpu:?
Esperado: **sala**
1. `sala` — sim=0.7718 ★
2. `cpu` — sim=0.6089
3. `cozinha` — sim=0.5812

### cpu:cozinha :: disco:?
Esperado: **porao**
1. `porao` — sim=0.7488 ★
2. `vim` — sim=0.6109
3. `guarda` — sim=0.5859

### gpu:sala :: disco:?
Esperado: **porao**
1. `porao` — sim=0.6987 ★
2. `apagado` — sim=0.5688
3. `vim` — sim=0.5623

### cozinha:cpu :: sala:?
Esperado: **gpu**
1. `gpu` — sim=0.8650 ★
2. `massivos` — sim=0.5568
3. `cache` — sim=0.5332

### quarto:ram :: porao:?
Esperado: **disco**
1. `disco` — sim=0.7542 ★
2. `descansa` — sim=0.6168
3. `treinados` — sim=0.6014

### janela:rede :: sala:?
Esperado: **gpu**
1. `gpu` — sim=0.6633 ★
2. `util` — sim=0.4957
3. `falhou` — sim=0.4897


### 2.2 Analogias de Personagem

### felix:natacha :: robo:?
Esperado: **assistente**
1. `ofende` — sim=0.5599
2. `chamada` — sim=0.5435
3. `chama` — sim=0.5184

### felix:natacha :: felix:?
Esperado: **quintal**
1. `aluna` — sim=0.5859
2. `test` — sim=0.5566
3. `parceiro` — sim=0.5562


## 3. Top 10 Similares

### Top 10 similares a `natacha`

1. `aluna` — sim=0.5859
2. `test` — sim=0.5566
3. `parceiro` — sim=0.5562
4. `frustacao` — sim=0.5522
5. `professora` — sim=0.5500
6. `insiste` — sim=0.5387
7. `videochamada` — sim=0.5279
8. `revolucao` — sim=0.5047
9. `feitos` — sim=0.5042
10. `didatica` — sim=0.4821

### Top 10 similares a `felix`

1. `companheiro` — sim=0.6001
2. `miado` — sim=0.5976
3. `interromper` — sim=0.5969
4. `lide` — sim=0.5913
5. `preto` — sim=0.5876
6. `quintal` — sim=0.5831
7. `caca` — sim=0.5676
8. `almoco` — sim=0.5658
9. `chata` — sim=0.5516
10. `cansativo` — sim=0.5514

### Top 10 similares a `rodrigo`

1. `desenvolvedor` — sim=0.7048
2. `criador` — sim=0.6221
3. `25` — sim=0.6003
4. `julgo` — sim=0.5887
5. `limpeza` — sim=0.5870
6. `desenvolve` — sim=0.5863
7. `freudiano` — sim=0.5839
8. `olhava` — sim=0.5699
9. `terminal` — sim=0.5694
10. `pereira` — sim=0.5691

### Top 10 similares a `cpu`

1. `cozinha` — sim=0.9096
2. `gpu` — sim=0.6666
3. `sala` — sim=0.6309
4. `esquenta` — sim=0.5604
5. `pesa` — sim=0.5504
6. `massivos` — sim=0.5489
7. `teclado` — sim=0.5453
8. `paralelos` — sim=0.5435
9. `cafe` — sim=0.5426
10. `chip` — sim=0.5257

### Top 10 similares a `gpu`

1. `sala` — sim=0.9210
2. `cpu` — sim=0.6666
3. `massivos` — sim=0.6130
4. `cozinha` — sim=0.6007
5. `paralelos` — sim=0.5952
6. `trabalham` — sim=0.5888
7. `agrega` — sim=0.5880
8. `multiplicacoes` — sim=0.5778
9. `paralela` — sim=0.5767
10. `preenche` — sim=0.5709

### Top 10 similares a `ram`

1. `quarto` — sim=0.9239
2. `porao` — sim=0.6385
3. `expansivel` — sim=0.5995
4. `fila` — sim=0.5897
5. `descansa` — sim=0.5879
6. `temporarias` — sim=0.5742
7. `disco` — sim=0.5716
8. `enquanto` — sim=0.5655
9. `descansando` — sim=0.5655
10. `treinados` — sim=0.5543

### Top 10 similares a `cozinha`

1. `cpu` — sim=0.9096
2. `gpu` — sim=0.6007
3. `sala` — sim=0.5985
4. `cafe` — sim=0.5657
5. `esquenta` — sim=0.5423
6. `ociosa` — sim=0.5211
7. `teclado` — sim=0.5092
8. `agitada` — sim=0.5078
9. `pesa` — sim=0.5068
10. `deitou` — sim=0.4903

### Top 10 similares a `sala`

1. `gpu` — sim=0.9210
2. `cpu` — sim=0.6309
3. `cozinha` — sim=0.5985
4. `massivos` — sim=0.5862
5. `preenche` — sim=0.5789
6. `projeta` — sim=0.5695
7. `na` — sim=0.5691
8. `otimizando` — sim=0.5658
9. `pesa` — sim=0.5607
10. `pipeline` — sim=0.5566

### Top 10 similares a `quarto`

1. `ram` — sim=0.9239
2. `porao` — sim=0.6676
3. `descansa` — sim=0.6158
4. `expansivel` — sim=0.6084
5. `temporarias` — sim=0.5986
6. `disco` — sim=0.5832
7. `renderiza` — sim=0.5793
8. `projeta` — sim=0.5779
9. `treinados` — sim=0.5778
10. `enquanto` — sim=0.5723

### Top 10 similares a `porao`

1. `disco` — sim=0.8317
2. `descansa` — sim=0.6845
3. `quarto` — sim=0.6676
4. `treinados` — sim=0.6657
5. `perdendo` — sim=0.6424
6. `ram` — sim=0.6385
7. `sexta-feira` — sim=0.6319
8. `expansivel` — sim=0.6276
9. `descansar` — sim=0.6210
10. `conformidade` — sim=0.6208

### Top 10 similares a `janela`

1. `rede` — sim=0.6590
2. `monitorou` — sim=0.5530
3. `desejado` — sim=0.5435
4. `cosseno` — sim=0.5227
5. `finito` — sim=0.5163
6. `abriu` — sim=0.5058
7. `liga` — sim=0.5010
8. `alertou` — sim=0.4970
9. `molhando` — sim=0.4939
10. `confirmacao` — sim=0.4905

### Top 10 similares a `robo`

1. `ofende` — sim=0.5762
2. `chamada` — sim=0.5661
3. `chama` — sim=0.5583
4. `ronrona` — sim=0.5496
5. `pesquisa` — sim=0.5308
6. `dado` — sim=0.5278
7. `igual` — sim=0.5265
8. `falsa` — sim=0.5212
9. `endereco` — sim=0.5204
10. `preco` — sim=0.5106

### Top 10 similares a `casa`

1. `virtual` — sim=0.7127
2. `funda` — sim=0.6402
3. `sancha` — sim=0.5928
4. `dona` — sim=0.5865
5. `moveis` — sim=0.5733
6. `asilo` — sim=0.5684
7. `verde` — sim=0.5615
8. `infancia` — sim=0.5553
9. `irritada` — sim=0.5441
10. `infraestrutura` — sim=0.5432
