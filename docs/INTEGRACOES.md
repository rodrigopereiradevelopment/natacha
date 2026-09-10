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

## Event Bus (canal canonico)

Desenho (RN-07): JSON assincrono entre C++, Rust e os PWAs. Sem chamada sincrona entre Natacha, Felix, Casa e APIs. O corredor da casa e essa metafora.

Transportes possiveis (ainda nao ha um barramento unico neste repo):
- HTTP/gRPC (maquinas diferentes)
- IPC (mesma maquina)
- fila / pub-sub

## Comunicacao com Sentinel (Rust)

Papel planejado: filtrar HTTP, permissoes em memoria, bloquear execucao nao autorizada, logs imutaveis. Com Felix, forma o **superego** (auditoria + interrupcao etica).

Fluxo pretendido: Sentinel detecta → Natacha analisa → Natacha recomenda → regra ou humano aprova.

Status no README: Sentinel em planejamento; nao documentar como modulo ja ligado ao `build/natacha`.

## Escolha Dinamica de Modelo

| Pergunta | Modelo |
|----------|--------|
| Simples | Modelo local pequeno |
| Complexa | Modelo maior por API |
| Dados privados | Somente modelo local |
| Codigo | Modelo especializado |
