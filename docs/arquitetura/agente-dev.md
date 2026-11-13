ARQUITETURA DO AGENTE DESENVEDOR — NATACHA

Este documento especifica a integração futura da Natacha com ferramentas externas de engenharia de software e qualidade de código, visando o Modo Agente Desenvolvedor (Fase E1) e a análise preditiva de impacto (RN-09).

---

## Integrações planejadas

### GitNexus — Grafo de dependências

A Natacha consumirá o grafo de código indexado pelo GitNexus para:
- Ler símbolos, classes, funções e processos de execução de repositórios conectados.
- Identificar callers, processos afetados e blast radius de alterações propostas.
- Executar análise de impacto upstream/downstream antes de sugerir refatorações.

### SonarCloud — Qualidade de código estática

A Natacha consultará alertas do SonarCloud (ex.: CWE-338 sobre `rand()`, bugs, code smells) para:
- Bloquear sugestões de alteração que introduzam novos problemas de segurança ou manutenibilidade.
- Validar que refatorações propostas não degradam a qualidade estática do código.

### OpenCode — Agente de desenvolvimento

Integração com OpenCode como orquestrador de tarefas de engenharia:
- Execução de comandos de build, testes e lint em ambiente sandbox.
- Sugestão de patches e refatorações com base no contexto global do repositório.

### Cursor — Editor como ferramenta

Uso do Cursor como interface de edição assistida, permitindo que a Natacha acione edições de arquivo com validação humana prévia.

---

## Análise preditiva de impacto (RN-09)

A Natacha usará os dados acima para realizar, antes de qualquer alteração de código:

1. **Leitura do grafo:** quais símbolos e arquivos são afetados pela mudança.
2. **Qualidade estática:** o que o SonarCloud reporta sobre os arquivos envolvidos.
3. **Impacto upstream:** quem chama o código que será alterado (via GitNexus).
4. **Decisão:** se houver regressão, quebra de contrato ou impacto negativo, a Natacha interrompe a execução autônoma, emite parecer crítico (com a persona ativa) e sugere abordagem tecnicamente superior.

Referências: RN-09, RN-10, RF-18.1 em `docs/requisitos/requisitos-gerais.md`.

---

**Status:** Especificado no Roadmap (Fase E1), não implementado.
