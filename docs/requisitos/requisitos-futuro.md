REQUISITOS FUTUROS — PROJETO NATACHA

Documento isolado para requisitos de longo prazo, experimentais ou fora do escopo imediato do TCC. Estes requisitos não devem se misturar com o que está sendo implementado agora.

---

## Fase 7 — Núcleo (SLM 10M–50M parâmetros)

**Status:** Longo Prazo / Escopo Futuro

Objetivo: treinar o núcleo proprietário da Natacha, seu "alma e voz", com 10M a 50M parâmetros.

### Requisitos

- Treinamento inicial do núcleo em C++
- Carregamento eficiente
- Inferência local em < 5s
- Integração com corpus escalado
- Janela Rede: integração com APIs externas (GPT, Gemini, Google Search) via RAG (Retrieval-Augmented Generation) para conhecimento enciclopédico pesado. A Natacha consulta os consultores externos, mas formula a resposta com a própria voz. A identidade dela nunca é substituída.

### Referências

RF-35, RF-36 em `docs/requisitos/requisitos-gerais.md`.

---

## Fase 7.5 — Protótipo MoE (4 experts, escrito à mão em C++)

**Status:** Opcional / Fase 7 estendida / Longo Prazo

Objetivo: protótipo didático de MoE com 4 experts, implementado diretamente em C++, usando a Natacha-Núcleo como semente (Sparse Upcycling).

### Requisitos

- Implementar roteador de 4 experts em C++
- Integrar com o Núcleo da Fase 7
- Validar que o núcleo continua sendo a semente e a decisão final

### Critério de conclusão

Protótipo funcional com 4 experts, núcleo preservado como semente.

---

## Fase 8+ — Escalonamento Progressivo (Sparse Upcycling)

**Status:** Longo Prazo / Visão Futura

Objetivo: crescer a arquitetura MoE hierárquica em escadas, sempre usando o Núcleo como semente.

### Escada de especialistas

- 4×2 → 8 experts (primeiro salto)
- 4×4 → 16 experts
- 8×4 → 32 experts
- 8×8 → 64 experts (1B–2B parâmetros totais)

### Requisitos

- Escalonar roteador e experts progressivamente
- Manter Expert de Identidade sempre ativo (fora do roteamento dos experts)
- Manter Janela Rede com consultores externos (GPT, Gemini, Google Search) via RAG
- Garantir que a identidade da Natacha nunca seja substituída pelos consultores externos — ela apenas consulta e formula com a própria voz

### Critério de conclusão

64 experts (8×8) com 1B–2B parâmetros totais, núcleo preservado como semente, identidade preservada.

### Nota de realismo

A MoE de 7B de parâmetros totais é um marco de longo prazo (Fase 10), não escopo de TCC nem de futuro próximo. O escopo realista para o futuro próximo é de 1B a 2B de parâmetros via Sparse Upcycling, partindo do Núcleo da Fase 7.

### Referências

ARQUITETURA.md (Seção 3), MOLINA et al. (2023) - Mixture of Experts.

---

## Fase E2 — Modo Agente Multimodal (Manus-like)

**Status:** Longo Prazo

Objetivo: implementar o agente multimodal com visão, geração de imagens, vídeos, slides e transcrição de áudios.

### Requisitos

- Entrada de visão (imagens e screenshots) para o agente.
- Integração com modelo de imagem (Stable Diffusion / Flux) via API.
- Integração com modelo de vídeo (SVD / Runway) para animações curtas.
- Geração de slides (python-pptx) a partir de texto e imagem.
- Transcrição de áudios longos (Whisper) com timestamps.
- Orquestração de ferramentas (Tool Calling) para encadear as ações, no modelo do Manus.

### Critério de conclusão

Dado um prompt de texto, o agente gera um slide completo com imagem e áudio de narração.

### Referências

INTEGRACOES.md v3.1, Seção 4.

---

## Fase Z — Controle Motor e Aprendizado de Dança (Tango)

**Status:** Visão de Longo Prazo

Objetivo: integrar cinemática e controle motor para a geração de movimentos corporais fluidos (Tango, Lambada e Capoeira), aplicando os mesmos princípios de predição de sequência (BPTT) usados na linguagem.

### Sub-requisitos

**Z1. Representação Corporal:** Definir esqueleto, articulações, coordenadas e formato dos dados de movimento (poses como vetores).

**Z2. Animação Determinística:** Fazer o avatar executar poses e transições predefinidas (sem IA) para validar a estrutura do corpo.

**Z3. Aprendizado Temporal:** Treinar um modelo com sequências de movimento (BPTT) e avaliar continuidade, precisão e estabilidade.

**Z4. Dança e Integração:** Integrar as sequências aprendidas ao avatar 3D, trabalhando ritmo, transições e os estilos (Tango, Lambada, Capoeira).

### Critério de conclusão

Uma mesma sequência de controle motor executa a mesma dança em dois corpos diferentes (simulados).

---

## Fase Z2.5 — Adaptador de Hardware (Hardware Agnostic Layer)

**Status:** Visão de Longo Prazo

Objetivo: abstrair a comunicação com diferentes corpos físicos (braços robóticos, humanoides como Optimus e Unitree), permitindo que o núcleo neural da Natacha controle qualquer hardware via ROS2 ou WebSockets.

### Requisitos

- Camada de abstração HAL (Hardware Abstraction Layer).
- API de controle de juntas e sensores (propriocepção).
- Integração com simuladores (Isaac Sim / MuJoCo) para testes.

### Critério de conclusão

Uma mesma sequência de controle motor executa a mesma dança em dois corpos diferentes (simulados).

---

> Estes requisitos são de longo prazo e não entram no escopo de curto prazo do TCC. Para o Marco do TCC, consulte a Seção 5 do `docs/visao/roadmap.md`.
