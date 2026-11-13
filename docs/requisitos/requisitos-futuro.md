REQUISITOS FUTUROS — PROJETO NATACHA

Documento isolado para requisitos de longo prazo, experimentais ou fora do escopo imediato do TCC. Estes requisitos não devem se misturar com o que está sendo implementado agora.

---

## Fase 7.5 — Arquitetura MoE Hierárquica (8x8)

**Status:** Opcional / Fase 7 estendida / Longo Prazo

Objetivo: evoluir o SLM (Small Language Model) para uma arquitetura de Mixture of Experts (MoE) esparsa, permitindo capacidade de 7B de parâmetros com custo de inferência de ~1.5B.

### Requisitos

- Implementar roteador primário (8 macro-experts).
- Implementar roteadores secundários (8 sub-experts por macro, totalizando 64).
- Implementar o Expert de Identidade sempre ativo (fora do roteamento dos 64 experts).
- Implementar perda auxiliar de balanceamento (Auxiliary Load Balancing Loss) para evitar colapso do roteador (Routing Collapse).
- Adicionar ruído estocástico (Gumbel-Softmax) no treinamento do roteador.
- Validação: garantir que todos os 64 experts recebam pelo menos X% dos tokens ao longo de uma época.

### Critério de conclusão

Modelo de 7B totais, 1.5B ativos, com distribuição de carga equilibrada entre os 64 experts.

### Expert de Identidade

Expert permanente, fora do roteamento dos 64 experts, que recebe todos os tokens de inferência e aplica os invariantes de persona e de identidade persistente (RN-05). Tem orçamento de compute pequeno e previsível, para preservar a persona sem inviabilizar o custo de ~1,5B ativos. É o guardião neural do "eu sou eu em qualquer hardware", complementando o JSON de identidade.

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
