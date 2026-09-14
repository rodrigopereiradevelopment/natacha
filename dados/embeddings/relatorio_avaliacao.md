# Relatório de Avaliação de Embeddings

Gerado em: 2026-09-13 21:50:21
Modelos avaliados: 30ep, 50ep, 100ep, 150ep, 200ep
Dimensão: 300 | Vocabulário: 4125 palavras

## 1. Visão Geral por Modelo

| Épocas | Média Cosine | Pares > 0.5 | Analogias Top-1 | Média Posição |
|--------|-------------|-------------|-----------------|---------------|
| 30 | 0.6075 | 12/16 | 14/24 (58%) | 2.75 |
| 50 | 0.5835 | 11/16 | 14/24 (58%) | 2.79 |
| 100 | 0.5462 | 10/16 | 15/24 (62%) | 2.75 |
| 150 | 0.5264 | 10/16 | 14/24 (58%) | 2.79 |
| 200 | 0.5149 | 10/16 | 14/24 (58%) | 2.79 |

## 2. Cosine Similarity por Categoria

### 2.1 Arquitetura da Casa

| Relação | 30ep | 50ep | 100ep | 150ep | 200ep |
|---------|--------|--------|--------|--------|--------|
| CPU → cozinha | 0.8352 | 0.8109 | 0.7798 | 0.7628 | 0.7502 |
| GPU → sala | 0.9091 | 0.9046 | 0.8877 | 0.8769 | 0.8690 |
| RAM → quarto | 0.8027 | 0.8070 | 0.8063 | 0.8095 | 0.8127 |
| disco → porão | 0.7729 | 0.7604 | 0.7537 | 0.7638 | 0.7642 |
| rede → janela | 0.5284 | 0.4489 | 0.3483 | 0.3082 | 0.2881 |
| corredor → eventos | 0.4922 | 0.4956 | 0.4200 | 0.3639 | 0.3233 |
| sótão → quântico | 0.7139 | 0.6600 | 0.6193 | 0.6012 | 0.5864 |
| Felix → quintal | 0.1972 | 0.1615 | 0.1495 | 0.1515 | 0.1621 |
| Casa → Natacha | 0.1915 | 0.1193 | 0.0069 | -0.0373 | -0.0525 |

### 2.2 Personagens

| Relação | 30ep | 50ep | 100ep | 150ep | 200ep |
|---------|--------|--------|--------|--------|--------|
| Felix ↔ Natacha | 0.7784 | 0.7398 | 0.6692 | 0.6177 | 0.5903 |
| robô ↔ assistente | 0.6846 | 0.6863 | 0.6621 | 0.6500 | 0.6375 |
| robô ↔ obedece | 0.6006 | 0.5894 | 0.5735 | 0.5619 | 0.5571 |
| robô ↔ detesta | 0.6165 | 0.5992 | 0.5838 | 0.5724 | 0.5688 |

### 2.3 Hardware

| Relação | 30ep | 50ep | 100ep | 150ep | 200ep |
|---------|--------|--------|--------|--------|--------|
| CPU ↔ GPU | 0.5759 | 0.5638 | 0.5399 | 0.5185 | 0.5049 |
| CPU ↔ RAM | 0.5273 | 0.5159 | 0.4759 | 0.4439 | 0.4206 |
| GPU ↔ RAM | 0.4943 | 0.4740 | 0.4635 | 0.4569 | 0.4548 |

### 2.4 Média por Categoria

| Épocas | Casa | Personagens | Hardware | Geral |
|--------|------|-------------|----------|-------|
| 30 | 0.6048 | 0.6700 | 0.5325 | 0.6075 |
| 50 | 0.5742 | 0.6537 | 0.5179 | 0.5835 |
| 100 | 0.5302 | 0.6221 | 0.4931 | 0.5462 |
| 150 | 0.5112 | 0.6005 | 0.4731 | 0.5264 |
| 200 | 0.5004 | 0.5884 | 0.4601 | 0.5149 |

## 3. Analogias Vetoriais

Formato: `a : b :: c : ?` — resultado = `b - a + c`
Excluídas da busca: A, B e C
Top-5 candidatos exibidos

### 3.1 Casa

**CPU:cozinha :: GPU:? → sala** (esperado: `sala`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | sala (0.716) ✓ | sala (0.714) ✓ | sala (0.723) ✓ | sala (0.721) ✓ | sala (0.713) ✓ |
| 2 | processa (0.501) | recebe (0.496) | recebe (0.518) | recebe (0.509) | recebe (0.502) |
| 3 | recebe (0.484) | pesada (0.469) | pesada (0.496) | pesada (0.493) | visitas (0.493) |
| 4 | pesada (0.448) | processa (0.463) | chip (0.477) | visitas (0.487) | pesada (0.484) |
| 5 | esquenta (0.431) | chip (0.454) | ritual (0.475) | ritual (0.475) | ritual (0.472) |

**RAM:quarto :: CPU:? → cozinha** (esperado: `cozinha`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | cozinha (0.714) ✓ | cozinha (0.700) ✓ | cozinha (0.680) ✓ | cozinha (0.675) ✓ | cozinha (0.664) ✓ |
| 2 | processa (0.693) | processa (0.669) | processa (0.593) | processa (0.557) | processa (0.526) |
| 3 | gpu (0.563) | gpu (0.546) | gpu (0.510) | gpu (0.495) | gpu (0.488) |
| 4 | sala (0.525) | sala (0.522) | sala (0.478) | cafe (0.457) | sala (0.443) |
| 5 | cafe (0.503) | cafe (0.500) | cafe (0.474) | sala (0.451) | cafe (0.442) |

**GPU:sala :: RAM:? → quarto** (esperado: `quarto`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | quarto (0.750) ✓ | quarto (0.743) ✓ | quarto (0.727) ✓ | quarto (0.719) ✓ | quarto (0.710) ✓ |
| 2 | memoria (0.654) | memoria (0.632) | disco (0.589) | volatil (0.583) | volatil (0.568) |
| 3 | guarda (0.583) | guarda (0.594) | memoria (0.584) | disco (0.576) | disco (0.552) |
| 4 | porao (0.575) | porao (0.585) | porao (0.572) | porao (0.553) | porao (0.536) |
| 5 | disco (0.541) | disco (0.572) | volatil (0.570) | memoria (0.548) | memoria (0.523) |

**disco:porão :: RAM:? → quarto** (esperado: `quarto`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | quarto (0.736) ✓ | quarto (0.733) ✓ | quarto (0.707) ✓ | quarto (0.705) ✓ | quarto (0.701) ✓ |
| 2 | memoria (0.712) | memoria (0.663) | memoria (0.581) | memoria (0.542) | memoria (0.515) |
| 3 | cpu (0.581) | cpu (0.550) | estava (0.484) | volatil (0.467) | volatil (0.466) |
| 4 | enquanto (0.547) | enquanto (0.511) | json (0.480) | estava (0.466) | estava (0.454) |
| 5 | cozinha (0.506) | estava (0.488) | cpu (0.477) | json (0.465) | json (0.445) |

**CPU:cozinha :: disco:? → porão** (esperado: `porao`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | descansa (0.618) | descansa (0.625) | porao (0.640) ✓ | porao (0.657) ✓ | porao (0.656) ✓ |
| 2 | porao (0.588) ✓ | porao (0.605) ✓ | descansa (0.609) | descansa (0.605) | descansa (0.599) |
| 3 | backups (0.564) | backups (0.564) | volatil (0.558) | volatil (0.568) | volatil (0.564) |
| 4 | antigo (0.545) | volatil (0.542) | durmo (0.531) | curto (0.530) | curto (0.524) |
| 5 | comodo (0.539) | revisando (0.541) | backups (0.529) | durmo (0.525) | durmo (0.519) |

**GPU:sala :: disco:? → porão** (esperado: `porao`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | porao (0.664) ✓ | porao (0.664) ✓ | porao (0.655) ✓ | porao (0.653) ✓ | porao (0.650) ✓ |
| 2 | guarda (0.653) | guarda (0.638) | guarda (0.606) | descansa (0.587) | descansa (0.578) |
| 3 | volatil (0.582) | descansa (0.576) | descansa (0.572) | guarda (0.575) | guarda (0.557) |
| 4 | descansa (0.579) | volatil (0.573) | volatil (0.550) | volatil (0.551) | volatil (0.541) |
| 5 | backups (0.532) | backups (0.525) | ram (0.510) | backups (0.517) | ram (0.509) |

**rede:janela :: GPU:? → sala** (esperado: `sala`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | sala (0.719) ✓ | sala (0.716) ✓ | sala (0.713) ✓ | sala (0.708) ✓ | sala (0.699) ✓ |
| 2 | recebe (0.600) | recebe (0.572) | recebe (0.545) | pesada (0.511) | pesada (0.506) |
| 3 | pesada (0.557) | pesada (0.541) | pesada (0.543) | prediz (0.502) | prediz (0.491) |
| 4 | fica (0.543) | prediz (0.530) | prediz (0.532) | recebe (0.499) | visitas (0.487) |
| 5 | ritual (0.536) | ritual (0.528) | ritual (0.527) | ritual (0.492) | recebe (0.484) |

**corredor:eventos :: rede:? → janela** (esperado: `janela`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | neural (0.579) | neural (0.535) | neural (0.481) | neural (0.454) | neural (0.426) |
| 2 | camada (0.440) | camada (0.415) | processa (0.342) | processa (0.321) | processa (0.308) |
| 3 | arquitetura (0.413) | arquitetura (0.348) | camada (0.337) | tensores (0.313) | guarda (0.301) |
| 4 | ajusta (0.342) | ajusta (0.348) | tensores (0.319) | cache (0.311) | cache (0.290) |
| 5 | janela (0.338) ✓ | processa (0.326) | guarda (0.309) | armazena (0.305) | quarto (0.289) |

**Resumo — Casa:**

| Épocas | Acertos Top-1 | Média Posição |
|--------|---------------|---------------|
| 30 | 6/8 | 1.62 |
| 50 | 6/8 | 1.75 |
| 100 | 7/8 | 1.62 |
| 150 | 7/8 | 1.62 |
| 200 | 7/8 | 1.62 |

### 3.2 Hardware

**cozinha:CPU :: sala:? → GPU** (esperado: `gpu`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | gpu (0.763) ✓ | gpu (0.738) ✓ | gpu (0.725) ✓ | gpu (0.713) ✓ | gpu (0.702) ✓ |
| 2 | quarto (0.615) | quarto (0.587) | quarto (0.515) | quarto (0.484) | quarto (0.476) |
| 3 | processa (0.578) | processa (0.568) | processa (0.514) | processa (0.458) | processa (0.414) |
| 4 | ram (0.478) | ram (0.472) | ram (0.442) | ram (0.419) | ram (0.405) |
| 5 | porao (0.409) | recebe (0.391) | recebe (0.416) | recebe (0.407) | recebe (0.383) |

**quarto:RAM :: porão:? → disco** (esperado: `disco`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | disco (0.628) ✓ | disco (0.642) ✓ | disco (0.649) ✓ | disco (0.661) ✓ | disco (0.660) ✓ |
| 2 | guarda (0.586) | guarda (0.607) | guarda (0.585) | guarda (0.573) | guarda (0.563) |
| 3 | memoria (0.542) | memoria (0.513) | memoria (0.484) | volatil (0.487) | volatil (0.491) |
| 4 | json (0.420) | json (0.421) | volatil (0.457) | memoria (0.468) | memoria (0.455) |
| 5 | banheiro (0.405) | volatil (0.416) | json (0.431) | json (0.421) | json (0.411) |

**janela:rede :: sala:? → GPU** (esperado: `gpu`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | gpu (0.652) ✓ | gpu (0.634) ✓ | gpu (0.620) ✓ | gpu (0.611) ✓ | gpu (0.609) ✓ |
| 2 | processa (0.550) | processa (0.509) | processa (0.453) | processa (0.407) | quarto (0.390) |
| 3 | cpu (0.495) | cpu (0.464) | cpu (0.409) | quarto (0.389) | processa (0.380) |
| 4 | quarto (0.460) | quarto (0.436) | quarto (0.406) | cpu (0.364) | neural (0.346) |
| 5 | ram (0.392) | ram (0.366) | ram (0.336) | neural (0.341) | cpu (0.344) |

**porão:disco :: quarto:? → RAM** (esperado: `ram`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | ram (0.689) ✓ | ram (0.702) ✓ | ram (0.699) ✓ | ram (0.704) ✓ | ram (0.705) ✓ |
| 2 | volatil (0.631) | volatil (0.629) | volatil (0.612) | volatil (0.609) | volatil (0.602) |
| 3 | guarda (0.579) | guarda (0.560) | descansa (0.555) | descansa (0.548) | descansa (0.548) |
| 4 | sala (0.536) | descansa (0.529) | guarda (0.514) | curto (0.496) | curto (0.507) |
| 5 | gpu (0.531) | gpu (0.513) | curto (0.478) | guarda (0.483) | gpu (0.462) |

**sala:GPU :: cozinha:? → CPU** (esperado: `cpu`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | cpu (0.792) ✓ | cpu (0.752) ✓ | cpu (0.718) ✓ | cpu (0.699) ✓ | cpu (0.682) ✓ |
| 2 | processa (0.638) | processa (0.605) | processa (0.549) | processa (0.518) | processa (0.500) |
| 3 | cafe (0.502) | cafe (0.471) | quarto (0.453) | quarto (0.437) | quarto (0.422) |
| 4 | quarto (0.449) | quarto (0.463) | estava (0.428) | estava (0.410) | estava (0.399) |
| 5 | casa (0.438) | estava (0.425) | cafe (0.418) | cafe (0.380) | cafe (0.357) |

**cozinha:CPU :: porão:? → disco** (esperado: `disco`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | disco (0.655) ✓ | disco (0.662) ✓ | disco (0.661) ✓ | disco (0.662) ✓ | disco (0.659) ✓ |
| 2 | quarto (0.650) | quarto (0.641) | guarda (0.609) | ram (0.603) | ram (0.590) |
| 3 | guarda (0.608) | guarda (0.631) | ram (0.608) | guarda (0.578) | quarto (0.562) |
| 4 | ram (0.605) | ram (0.615) | quarto (0.590) | quarto (0.575) | guarda (0.556) |
| 5 | memoria (0.519) | memoria (0.522) | memoria (0.503) | memoria (0.477) | memoria (0.445) |

**Resumo — Hardware:**

| Épocas | Acertos Top-1 | Média Posição |
|--------|---------------|---------------|
| 30 | 6/6 | 1.00 |
| 50 | 6/6 | 1.00 |
| 100 | 6/6 | 1.00 |
| 150 | 6/6 | 1.00 |
| 200 | 6/6 | 1.00 |

### 3.3 Personagens

**Felix:Natacha :: robô:? → assistente** (esperado: `assistente`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | tambem (0.665) | tambem (0.621) | tambem (0.524) | sua (0.432) | sua (0.378) |
| 2 | mas (0.657) | mas (0.600) | sua (0.505) | tambem (0.432) | tambem (0.378) |
| 3 | se (0.635) | sua (0.592) | nao (0.484) | nao (0.412) | nao (0.358) |
| 4 | sua (0.625) | se (0.570) | mas (0.476) | mas (0.388) | mas (0.323) |
| 5 | porque (0.607) | nao (0.570) | se (0.447) | se (0.367) | rodrigo (0.315) |

**Felix:Natacha :: Felix:? → quintal** (esperado: `quintal`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | se (0.833) | se (0.811) | se (0.752) | se (0.707) | se (0.666) |
| 2 | mas (0.820) | mas (0.799) | mas (0.729) | mas (0.672) | mas (0.616) |
| 3 | tambem (0.813) | tambem (0.781) | tambem (0.713) | tambem (0.648) | tambem (0.597) |
| 4 | tem (0.791) | tem (0.756) | tem (0.666) | rodrigo (0.605) | rodrigo (0.572) |
| 5 | porque (0.771) | porque (0.726) | rodrigo (0.657) | porque (0.595) | porque (0.554) |

**Rodrigo:Natacha :: Felix:? → quintal** (esperado: `quintal`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | se (0.837) | se (0.814) | se (0.743) | se (0.686) | se (0.642) |
| 2 | mas (0.804) | mas (0.776) | mas (0.679) | mas (0.604) | mas (0.539) |
| 3 | tem (0.794) | tem (0.747) | tem (0.640) | tem (0.560) | tem (0.501) |
| 4 | tambem (0.784) | tambem (0.735) | tambem (0.634) | tambem (0.555) | porque (0.495) |
| 5 | porque (0.756) | porque (0.707) | porque (0.613) | porque (0.546) | tambem (0.490) |

**Natacha:Felix :: robô:? → obedece** (esperado: `obedece`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | roteador (0.674) | roteador (0.663) | roteador (0.626) | roteador (0.581) | roteador (0.549) |
| 2 | foge (0.672) | tapete (0.631) | irritada (0.591) | irritada (0.563) | gambiarras (0.537) |
| 3 | prende (0.657) | foge (0.631) | tapete (0.589) | mia (0.556) | irritada (0.537) |
| 4 | tocou (0.657) | deitou (0.626) | mia (0.584) | acalma (0.551) | cavando (0.536) |
| 5 | tapete (0.655) | prende (0.620) | cama (0.582) | cavando (0.551) | muita (0.531) |

**Resumo — Personagens:**

| Épocas | Acertos Top-1 | Média Posição |
|--------|---------------|---------------|
| 30 | 0/4 | 6.00 |
| 50 | 0/4 | 6.00 |
| 100 | 0/4 | 6.00 |
| 150 | 0/4 | 6.00 |
| 200 | 0/4 | 6.00 |

### 3.4 Processamento

**processa:CPU :: guarda:? → RAM** (esperado: `ram`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | ram (0.647) ✓ | ram (0.644) ✓ | ram (0.595) ✓ | ram (0.559) ✓ | ram (0.534) ✓ |
| 2 | memoria (0.573) | memoria (0.570) | memoria (0.522) | disco (0.497) | disco (0.498) |
| 3 | quarto (0.558) | quarto (0.554) | disco (0.518) | memoria (0.496) | memoria (0.473) |
| 4 | porao (0.536) | porao (0.521) | quarto (0.511) | quarto (0.473) | quarto (0.458) |
| 5 | disco (0.474) | disco (0.496) | porao (0.472) | porao (0.443) | porao (0.435) |

**guarda:RAM :: processa:? → CPU** (esperado: `cpu`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | cpu (0.738) ✓ | cpu (0.714) ✓ | cpu (0.658) ✓ | quarto (0.631) | quarto (0.616) |
| 2 | cozinha (0.696) | quarto (0.687) | quarto (0.646) | cpu (0.617) ✓ | cpu (0.583) ✓ |
| 3 | quarto (0.679) | cozinha (0.674) | cozinha (0.623) | cozinha (0.583) | cozinha (0.553) |
| 4 | gpu (0.671) | gpu (0.645) | gpu (0.592) | gpu (0.562) | gpu (0.541) |
| 5 | sala (0.600) | sala (0.599) | sala (0.551) | sala (0.517) | sala (0.498) |

**volátil:RAM :: permanente:? → disco** (esperado: `disco`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | — | — | — | — | — |
| 2 | — | — | — | — | — |
| 3 | — | — | — | — | — |
| 4 | — | — | — | — | — |
| 5 | — | — | — | — | — |

**processa:CPU :: exibe:? → GPU** (esperado: `gpu`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | — | — | — | — | — |
| 2 | — | — | — | — | — |
| 3 | — | — | — | — | — |
| 4 | — | — | — | — | — |
| 5 | — | — | — | — | — |

**memória:RAM :: armazenamento:? → disco** (esperado: `disco`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | — | — | — | — | — |
| 2 | — | — | — | — | — |
| 3 | — | — | — | — | — |
| 4 | — | — | — | — | — |
| 5 | — | — | — | — | — |

**rápido:CPU :: paralelo:? → GPU** (esperado: `gpu`)

| # | 30ep | 50ep | 100ep | 150ep | 200ep |
|---|-------|-------|-------|-------|-------|
| 1 | cozinha (0.739) | cozinha (0.697) | cozinha (0.640) | cozinha (0.608) | cozinha (0.595) |
| 2 | processa (0.713) | processa (0.682) | processa (0.597) | processa (0.543) | processa (0.501) |
| 3 | gpu (0.595) ✓ | gpu (0.580) ✓ | gpu (0.540) ✓ | gpu (0.508) ✓ | gpu (0.488) ✓ |
| 4 | sala (0.535) | sala (0.539) | sala (0.491) | xicara (0.460) | xicara (0.450) |
| 5 | quarto (0.495) | quarto (0.492) | xicara (0.466) | central (0.450) | central (0.438) |

**Resumo — Processamento:**

| Épocas | Acertos Top-1 | Média Posição |
|--------|---------------|---------------|
| 30 | 2/6 | 3.83 |
| 50 | 2/6 | 3.83 |
| 100 | 2/6 | 3.83 |
| 150 | 1/6 | 4.00 |
| 200 | 1/6 | 4.00 |

## 4. Resumo por Modelo

### 30 Épocas

- **Média cosine geral:** 0.6075
- **Pares acima de 0.5:** 12/16
- **Analogias acertadas (top-1):** 14/24
- **Média posição do resultado esperado:** 2.75
- **Por categoria:**
  - Casa: 6/8 top-1, posição média 1.62
  - Hardware: 6/6 top-1, posição média 1.00
  - Personagens: 0/4 top-1, posição média 6.00
  - Processamento: 2/6 top-1, posição média 3.83

### 50 Épocas

- **Média cosine geral:** 0.5835
- **Pares acima de 0.5:** 11/16
- **Analogias acertadas (top-1):** 14/24
- **Média posição do resultado esperado:** 2.79
- **Por categoria:**
  - Casa: 6/8 top-1, posição média 1.75
  - Hardware: 6/6 top-1, posição média 1.00
  - Personagens: 0/4 top-1, posição média 6.00
  - Processamento: 2/6 top-1, posição média 3.83

### 100 Épocas

- **Média cosine geral:** 0.5462
- **Pares acima de 0.5:** 10/16
- **Analogias acertadas (top-1):** 15/24
- **Média posição do resultado esperado:** 2.75
- **Por categoria:**
  - Casa: 7/8 top-1, posição média 1.62
  - Hardware: 6/6 top-1, posição média 1.00
  - Personagens: 0/4 top-1, posição média 6.00
  - Processamento: 2/6 top-1, posição média 3.83

### 150 Épocas

- **Média cosine geral:** 0.5264
- **Pares acima de 0.5:** 10/16
- **Analogias acertadas (top-1):** 14/24
- **Média posição do resultado esperado:** 2.79
- **Por categoria:**
  - Casa: 7/8 top-1, posição média 1.62
  - Hardware: 6/6 top-1, posição média 1.00
  - Personagens: 0/4 top-1, posição média 6.00
  - Processamento: 1/6 top-1, posição média 4.00

### 200 Épocas

- **Média cosine geral:** 0.5149
- **Pares acima de 0.5:** 10/16
- **Analogias acertadas (top-1):** 14/24
- **Média posição do resultado esperado:** 2.79
- **Por categoria:**
  - Casa: 7/8 top-1, posição média 1.62
  - Hardware: 6/6 top-1, posição média 1.00
  - Personagens: 0/4 top-1, posição média 6.00
  - Processamento: 1/6 top-1, posição média 4.00

## 5. Análise Comparativa

### Curva de cosine por época

```
   30ep |██████████████████████████████████████████████████| 0.6075
   50ep |████████████████████████████████████████████████░░| 0.5835
  100ep |████████████████████████████████████████████░░░░░░| 0.5462
  150ep |███████████████████████████████████████████░░░░░░░| 0.5264
  200ep |██████████████████████████████████████████░░░░░░░░| 0.5149
```

### Curva de analogias (top-1) por época

```
   30ep |█████████████████████████████░░░░░░░░░░░░░░░░░░░░░| 14/24 (58%)
   50ep |█████████████████████████████░░░░░░░░░░░░░░░░░░░░░| 14/24 (58%)
  100ep |███████████████████████████████░░░░░░░░░░░░░░░░░░░| 15/24 (62%)
  150ep |█████████████████████████████░░░░░░░░░░░░░░░░░░░░░| 14/24 (58%)
  200ep |█████████████████████████████░░░░░░░░░░░░░░░░░░░░░| 14/24 (58%)
```

### Curva de posição média por época (menor = melhor)

```
   30ep |██████████████████████████████████████████████████| 2.75
   50ep |░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░| 2.79
  100ep |██████████████████████████████████████████████████| 2.75
  150ep |░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░| 2.79
  200ep |░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░| 2.79
```

## 6. Observações

- Este relatório é informativo. Nenhum modelo foi classificado automaticamente como overfitting.
- **Cosine similarity** mede proximidade entre pares de palavras.
- **Analogias** medem geometria relacional (b - a + c).
- **Média posição** indica onde o resultado esperado aparece no top-5 (1.0 = sempre primeiro, 6.0 = nunca encontrado).
- Valores de posição > 5.0 indicam que o resultado esperado não estava no top-5.
