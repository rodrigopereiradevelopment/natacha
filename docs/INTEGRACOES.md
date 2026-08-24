# INTEGRACOES

> "Natacha nao e so mais um projeto — e a inteligencia que desenvolve os outros quatro."

## Ecossistema

- ARCA (comparador de precos)
- ARCA Analytics (analise de dados)
- Sentinel (seguranca — Rust)
- EditeCC (editor academico)

## Modo Agente (API)

Natacha expoe uma API que os projetos chamam:

ARCA → Natacha API → modelo escolhido → ferramentas → resposta

## Perfis por Projeto

| Perfil | Funcao | Ferramentas |
|--------|--------|-------------|
| Natacha Developer | Le codigo, edita, testa | ler_arquivo, editar_codigo, rodar_teste |
| Natacha ARCA | Busca produtos, compara precos | buscar_produto, comparar_precos, criar_lista |
| Natacha Analytics | Interpreta metricas | consultar_metricas, gerar_relatorio |
| Natacha Sentinel | Analisa alertas | listar_alertas, consultar_ip, resumir_incidente |
| Natacha EditeCC | Revisa texto | revisar_texto, sugerir_referencias, formatar_abnt |

## Comunicacao com Sentinel (Rust)

Natacha (C++) e Sentinel (Rust) conversam via:
- HTTP/gRPC (maquinas diferentes)
- IPC (mesma maquina)
- Message Queue (sistema distribuido)

Sentinel detecta → Natacha analisa → Natacha recomenda → Regra ou humano aprova

## Escolha Dinamica de Modelo

| Pergunta | Modelo |
|----------|--------|
| Simples | Modelo local pequeno |
| Complexa | Modelo maior por API |
| Dados privados | Somente modelo local |
| Codigo | Modelo especializado |
