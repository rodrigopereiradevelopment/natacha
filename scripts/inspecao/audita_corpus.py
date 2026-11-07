#!/usr/bin/env python3
"""Auditoria do corpus da Natacha.

Não altera o corpus. Gera:
  - auditoria_corpus.md   (relatório legível)
  - auditoria_corpus.json (flags e métricas por cena)

Uso:
  python3 audita_corpus.py [corpus] [--outdir DIR] [--samples N]
"""

from __future__ import annotations

import argparse
import json
import re
import statistics
import sys
import unicodedata
from collections import Counter, defaultdict
from pathlib import Path

BASE = Path(__file__).resolve().parent
DEFAULT_CORPUS = BASE / 'corpus.opencode.txt'
MAIN_DOC = BASE / 'docs/TEMAS_CORPUS_5_BLOCOS.md'
DOCS = [BASE / f'docs/TEMAS_180_BLOCO{i}.md' for i in range(1, 6)]

# ---------------------------------------------------------------------------
# normalização e tokens
# ---------------------------------------------------------------------------

STOP = set('''
a o as os um uma uns umas de do da dos das em no na nos nas por pelo pela
pelos pelas para pra pro com sem sob sobre entre ate apos antes depois que se
como quando onde qual quais quem cujo cuja e ou mas porem porque pois tambem
ja ainda so mais menos muito pouco todo toda todos todas outro outra outros
outras mesmo mesma isso isto aquilo ele ela eles elas eu tu voce nos vos seu
sua seus suas meu minha meus minhas dele dela deles delas lhe lhes ao aos
foi ser estar ter haver era eram sera serao tem tinha tinham teria ha havia
pode podia podem poder deve devia devem dever vai iam iria fica ficou ficava
ficam fazer fez fazia fazem mostrar mostrou mostra mostravam ver viu viu
olhar olhou olhava olham abrir abriu abria abrem fechar fechou fechava fecham
pegar pegou pegava pegam deixar deixou deixava deixam levar levou levava levam
trazer trouxe trazia trazem falar falou falava falam dizer disse dizia dizem
pedir pediu pedia pedem tentar tentou tentava tentam conseguir conseguiu
conseguia precisar precisava precisa precisam usar usou usava usam colocar
colocou colocava colocam mudar mudou mudava mudam virar virou virava viram
comecar comecou comecava comecam terminar terminou terminava terminam
acontecer aconteceu acontecia acontecem parecer pareceu parecia parecem achar
achou achava acham encontrar encontrou encontra encontram passar passou
passava passam continuar continuou continuava continuam esperar esperou
esperava esperam entrar entrou entra entram sair saiu saia saem voltar voltou
voltava voltam subir subiu subia sobem descer desceu descia descem correr
correu corria correm dormir dormiu dormia dormem acordar acordou acordava
acordam comer comeu comia comem beber bebeu bebia bebem dar deu dava dao
receber recebeu recebia recebem conhecer conheceu conhecia conhecem pensar
pensou pensava pensam querer quis queria querem saber soube sabia sabem
ficar ficou fica ficam ir foi vai vao vir veio vem vê ver visto
'''.split())

LORE = {
    'natacha', 'felix', 'casa', 'rodrigo', 'gato', 'tio', 'primo', 'tios',
    'primos', 'ia', 'ias', 'gatos', 'tias', 'tia',
}

# palavras que costumam carregar o tema; usadas só para medir sobreposição
THEME_STOP = STOP | LORE | {
    'coisa', 'coisas', 'lugar', 'parte', 'vez', 'dia', 'noite', 'agora',
    'ainda', 'depois', 'antes', 'durante', 'enquanto', 'sempre', 'nunca',
    'nada', 'tudo', 'alguem', 'ninguem', 'cada', 'outro', 'outra', 'mesmo',
    'mesma', 'modo', 'forma', 'jeito', 'tempo', 'hora', 'horas', 'minuto',
    'minutos', 'semana', 'mes', 'ano', 'anos', 'primeiro', 'primeira',
    'segundo', 'segunda', 'terceiro', 'terceira', 'ultimo', 'ultima',
    'novo', 'nova', 'novos', 'novas', 'velho', 'velha', 'grande', 'pequeno',
    'pequena', 'bom', 'boa', 'ruim', 'certo', 'certa', 'errado', 'errada',
}

FELIX_VERBS = (
    'miou', 'ronron', 'dormi', 'deit', 'subiu', 'empurr', 'derrub', 'cavou',
    'enterrou', 'pulou', 'tossiu', 'lambeu', 'sentou', 'passou', 'andou',
    'seguiu', 'ficou', 'olhou', 'ergueu', 'esticou', 'girou', 'farejou',
    'cheirou', 'bateu', 'abriu', 'acordou', 'saiu', 'voltou', 'comeu',
    'bebeu', 'correu', 'saltou', 'espreguic', 'bocej', 'lamb', 'pisou',
    'bloqueou', 'escolheu', 'levou', 'trouxe', 'largou', 'mordeu',
    'arranhou', 'escondeu', 'sujou', 'derramou', 'acomodou', 'encostou',
    'achou', 'usou', 'usava', 'desceu', 'procurou', 'entrou', 'apareceu',
    'assistiu', 'espirrou', 'deixou', 'marcou', 'puxou', 'brincou', 'amassou',
    'aconchegou', 'aninhou', 'enrolou', 'rolou', 'parou', 'mexeu', 'tocou',
    'cruzou', 'apertou', 'ressonou', 'roncou', 'cochil', 'tentou', 'segurou',
    'pediu', 'virou', 'comecou', 'encarou', 'subia', 'levava', 'trazia',
    'pegou', 'apanhou', 'arrancou', 'empinou', 'rastejou', 'trepou',
    'andava', 'miando', 'mudou', 'enroscou', 'suspirou', 'roncava',
)


def normalize(text: str) -> str:
    text = unicodedata.normalize('NFKD', text)
    text = ''.join(c for c in text if not unicodedata.combining(c))
    return text.lower()


def tokens(text: str) -> list[str]:
    return re.findall(r'[a-z0-9]+', normalize(text))


def content_tokens(text: str) -> set[str]:
    return {t for t in tokens(text) if len(t) >= 3 and t not in THEME_STOP}


def sentences(text: str) -> list[str]:
    return [x.strip() for x in re.split(r'(?<=[.!?])\s+', text) if x.strip()]


def words(text: str) -> int:
    return len(tokens(text))


def prefix(text: str, n: int = 3) -> str:
    ws = tokens(text)
    return ' '.join(ws[:n])


def jaccard(a: set[str], b: set[str]) -> float:
    if not a and not b:
        return 1.0
    return len(a & b) / len(a | b)


# ---------------------------------------------------------------------------
# temas
# ---------------------------------------------------------------------------

def scene_theme(i: int) -> int:
    """Cena (1-based) -> tema.

    O corpus abre com os temas 2 e 1 trocados (cenas 1-5 = tema 2,
    cenas 6-10 = tema 1); do 11 em diante segue (i-1)//5 + 1.
    """
    if i <= 5:
        return 2
    if i <= 10:
        return 1
    return (i - 1) // 5 + 1

SEED_OFFSETS = {
    # cabeçalho da seção no doc principal -> offset das sementes 1..20
    'Bloco 1': 0,
    'Bloco 2': 200,
    'Bloco 3': 400,
    'Bloco 4': 600,
    'Bloco 5': 800,
}


def parse_themes() -> dict[int, str]:
    themes: dict[int, str] = {}

    # 1) sementes 1-20 de cada bloco, no doc principal, por seção
    if MAIN_DOC.exists():
        text = MAIN_DOC.read_text(encoding='utf-8', errors='replace')
        parts = re.split(r'^## (Bloco \d)\b.*$', text, flags=re.M)
        # parts = [pre, 'Bloco 1', corpo1, 'Bloco 2', corpo2, ...]
        for i in range(1, len(parts), 2):
            bloco, body = parts[i], parts[i + 1]
            offset = SEED_OFFSETS.get(bloco)
            if offset is None:
                continue
            for m in re.finditer(r'^(\d+)\.\s+(.+)$', body, re.M):
                n = int(m.group(1))
                if 1 <= n <= 20:
                    themes[offset + n] = m.group(2).strip()

    # 2) arquivos de 180 temas (21-200, 221-400, 421-600, 621-800, 821-1000)
    for path in DOCS:
        if not path.exists():
            continue
        text = path.read_text(encoding='utf-8', errors='replace')
        for m in re.finditer(r'^(\d+)\.\s+(.+)$', text, re.M):
            n = int(m.group(1))
            if 1 <= n <= 1000:
                themes.setdefault(n, m.group(2).strip())
    return themes


# ---------------------------------------------------------------------------
# validação mecânica (espelha valida_corpus.py, sem alterar o corpus)
# ---------------------------------------------------------------------------

ENG = set('''
the of and with for this that is are was were be been being to in on at by
from it its not you your we our they their he she his her i my me have has
had does did will would can could should may might must there here what which
who when where why how all any some more most other into over after before
under again also just only very too than then now up down out off about
because if but while during through between against without within along
across behind beyond upon per via file files code bug bugs log logs cache
thread threads build chat prompt token dataset benchmark timer spam buffer
switch link repo readme changelog zip dump free reset array template event bus
overfitting backup print timeout schema fallback retry online offline download
upload server kernel socket pixel pixels byte bytes update speed fast slow run
runs start stop clean check copy save load open close read write make made get
got put set new old good bad big small high low hot cold warm cool error warning
debug deploy release commit branch merge push pull fork user users model models
train training layer layers tensor gradient batch epoch noise input output
sample samples label labels class classes score scores value values list map
string float bool true false null none def return import else elif try except
pass break continue lambda self super init main args kwargs std cout endl
include using namespace void char double long short unsigned signed struct
union enum case default goto sizeof typedef const static extern volatile
register auto inline explicit mutable friend virtual operator private public
protected throw catch delete scheduler
'''.split())
ENG.discard('virtual')

ACCENTS = re.compile(r'[áàâãéêíóôõúüçÁÀÂÃÉÊÍÓÔÕÚÜÇ]')
UPPER = re.compile(r'[A-Z]')
PUNCT = re.compile(r'[,;:](?![ \n])')


def mechanical_flags(scene: str) -> list[tuple[str, str, str]]:
    out = []
    ss = sentences(scene)
    if not 3 <= len(ss) <= 8:
        out.append(('critical', 'sentencas', f'{len(ss)} frases'))
    for w in ('natacha', 'a casa', 'felix'):
        if w not in scene:
            out.append(('critical', 'trio', f'falta {w}'))
    if scene.startswith('natacha'):
        out.append(('critical', 'abertura', 'comeca com natacha'))
    if UPPER.search(scene):
        out.append(('critical', 'maiuscula', UPPER.search(scene).group(0)))
    if ACCENTS.search(scene):
        out.append(('critical', 'acento', ACCENTS.search(scene).group(0)))
    t = re.sub(r'std::', 'std ', scene)
    t = re.sub(r'\d:\d', ' ', t)
    if PUNCT.search(t):
        out.append(('critical', 'pontuacao', 'pontuacao colada'))
    if not scene.endswith('.'):
        out.append(('critical', 'ponto', 'nao termina em ponto'))
    if '  ' in scene:
        out.append(('critical', 'espaco', 'espaco duplo'))
    bad = sorted({w for w in re.findall(r'[a-z]+', scene) if w in ENG and w not in ('std', 'for', 'classes')})
    if bad:
        out.append(('critical', 'ingles', ','.join(bad)))
    if scene.count('felix') > 3:
        out.append(('warning', 'felix_rep', f"felix x{scene.count('felix')}"))
    if scene.count('a casa') > 3:
        out.append(('warning', 'casa_rep', f"a casa x{scene.count('a casa')}"))
    return out


def syntax_flags(scene: str) -> list[tuple[str, str, str]]:
    out = []
    for q in sentences(scene):
        nq = normalize(q)
        # palavra repetida de verdade (ignora compostos com hifen: "dois-pontos")
        if re.search(r'\b(\w+) \1\b(?!-)', nq):
            out.append(('critical', 'palavra_repetida', q))
        if re.search(r'\b(que que|e e|o o|a a)\b', nq):
            out.append(('critical', 'repeticao', q))
        if re.search(r'\b(de|em|com|por|para) (de|em|com|por|para)\b', nq):
            out.append(('warning', 'preposicao_dupla', q))
    return out


# ---------------------------------------------------------------------------
# auditoria principal
# ---------------------------------------------------------------------------

def audit(corpus_path: Path, sample_n: int = 8) -> dict:
    raw = corpus_path.read_text(encoding='utf-8')
    scenes = [s.strip() for s in re.split(r'\n\s*\n', raw) if s.strip()]
    themes = parse_themes()

    # índices globais
    scene_tokens = [set(tokens(s)) for s in scenes]
    sentence_lists = [sentences(s) for s in scenes]
    all_sentences = [q for ss in sentence_lists for q in ss]
    sent_count = Counter(all_sentences)
    prefix_count = Counter(prefix(q, 3) for q in all_sentences)
    open_count = Counter(prefix(ss[0], 3) for ss in sentence_lists if ss)
    close_count = Counter(prefix(ss[-1], 3) for ss in sentence_lists if ss)
    exact_scene = Counter(re.sub(r'[^a-z0-9 ]', '', normalize(s)) for s in scenes)

    # near-dup global (mesma lógica do valida_corpus)
    tf = Counter(w for s in scenes for w in tokens(s))
    rare = {w for w, n in tf.items() if n <= 80}
    idx: dict[str, list[int]] = defaultdict(list)
    for i, s in enumerate(scenes, 1):
        for w in set(tokens(s)) & rare:
            idx[w].append(i)
    near_pairs = []
    for i, s in enumerate(scenes, 1):
        a = set(tokens(s))
        cand = set()
        for w in a & rare:
            cand.update(idx[w])
        for j in cand:
            if j <= i:
                continue
            b = set(tokens(scenes[j - 1]))
            jac = jaccard(a, b)
            if jac > 0.62:
                near_pairs.append((round(jac, 3), i, j))

    flags: list[dict] = []
    metrics: list[dict] = []

    for i, scene in enumerate(scenes, 1):
        theme_id = scene_theme(i)
        theme_desc = themes.get(theme_id, '')
        tt = content_tokens(theme_desc)
        st = content_tokens(scene)
        hits = sorted(tt & st)
        overlap = len(hits) / max(1, len(tt))
        ss = sentence_lists[i - 1]
        wc = words(scene)
        ttr = len(set(tokens(scene))) / max(1, wc)
        theme_prefix = theme_desc[:70]

        metrics.append({
            'scene': i,
            'theme': theme_id,
            'theme_desc': theme_prefix,
            'words': wc,
            'sentences': len(ss),
            'ttr': round(ttr, 3),
            'theme_overlap': round(overlap, 3),
            'theme_hits': hits,
        })

        def add(sev, cat, msg, ev=''):
            flags.append({
                'scene': i,
                'theme': theme_id,
                'severity': sev,
                'category': cat,
                'message': msg,
                'evidence': ev,
                'scene_text': scene,
            })

        for sev, cat, ev in mechanical_flags(scene):
            add(sev, cat, ev, ev)
        for sev, cat, ev in syntax_flags(scene):
            add(sev, cat, ev, ev)

        # coerência temática: métrica informativa, sem flag por cena
        # (paráfrase e sinônimos tornam a sobreposição lexical ruidosa demais
        #  para virar aviso acionável)

        # tamanho / diversidade (limiares calibrados na distribuição real:
        # palavras p1=17, p99=51; únicas p1=16)
        if wc < 18:
            add('warning', 'cena_curta', f'{wc} palavras', scene[:120])
        if wc > 52:
            add('warning', 'cena_longa', f'{wc} palavras', scene[:120])
        for q in ss:
            if words(q) > 32:
                add('warning', 'frase_longa', q, q)
        if wc >= 30 and ttr < 0.70:
            add('warning', 'ttr_baixo', f'ttr={ttr:.2f}', scene[:120])
        if len(set(tokens(scene))) < 15:
            add('warning', 'vocab_baixo', f'{len(set(tokens(scene)))} palavras unicas', scene[:120])

        # repetição interna
        if len(ss) != len(set(ss)):
            add('critical', 'frase_repetida', 'frase repetida na cena', '')
        for q in ss:
            if sent_count[q] > 10:
                add('critical', 'frase_exata', f'{sent_count[q]}x: {q}', q)

        # Padrões repetidos (aberturas, fechamentos, prefixos) não viram flag
        # por cena: são medidos no resumo para revisão opcional.

        # near-dup
        for jac, a, b in near_pairs:
            if a == i or b == i:
                add('critical', 'near_dup', f'par {a}/{b} j={jac}', '')
                break

        # duplicata exata
        key = re.sub(r'[^a-z0-9 ]', '', normalize(scene))
        if exact_scene[key] > 1:
            add('critical', 'duplicada', f'cena repetida {exact_scene[key]}x', '')

    # Félix age em pelo menos 3 de cada 5 cenas
    for t in range((len(scenes) - 1) // 5 + 1):
        blk = scenes[t * 5:(t + 1) * 5]
        if len(blk) != 5:
            continue
        acts = 0
        for s in blk:
            if any(
                ('felix' in x or re.search(r'\b(ele|o gato|dele)\b', x))
                and any(v in x for v in FELIX_VERBS)
                for x in sentences(s)
            ):
                acts += 1
        if acts < 3:
            first = t * 5 + 1
            flags.append({
                'scene': first,
                'theme': scene_theme(first),
                'severity': 'critical',
                'category': 'felix_acao',
                'message': f'bloco {first}-{first + 4}: felix age em {acts}/5',
                'evidence': '',
                'scene_text': '',
            })

    # resumo
    sev_count = Counter(f['severity'] for f in flags)
    cat_count = Counter(f['category'] for f in flags)
    scenes_with = Counter()
    for f in flags:
        scenes_with[f['scene']] += 1
    critical_scenes = sorted({f['scene'] for f in flags if f['severity'] == 'critical'})
    warning_scenes = sorted({f['scene'] for f in flags if f['severity'] == 'warning'})

    # amostras por categoria
    samples = {}
    by_cat = defaultdict(list)
    for f in flags:
        by_cat[f['category']].append(f)
    for cat, items in by_cat.items():
        samples[cat] = items[:sample_n]

    # temas com menor coerência (média)
    by_theme = defaultdict(list)
    for m in metrics:
        by_theme[m['theme']].append(m['theme_overlap'])
    theme_avg = sorted(
        ((sum(v) / len(v), t, themes.get(t, '')[:70]) for t, v in by_theme.items()),
    )[:20]

    # top frases / aberturas / fechamentos
    top_sentences = [(n, q) for q, n in sent_count.most_common(20) if n > 1]
    top_open = [(n, p) for p, n in open_count.most_common(20)]
    top_close = [(n, p) for p, n in close_count.most_common(20)]
    top_prefix = [(n, p) for p, n in prefix_count.most_common(20)]

    def pattern_scenes(key: str, pos: str) -> list[int]:
        out = []
        for i, ss in enumerate(sentence_lists, 1):
            if not ss:
                continue
            q = ss[0] if pos == 'open' else ss[-1]
            if prefix(q, 3) == key:
                out.append(i)
                if len(out) >= 12:
                    break
        return out

    open_examples = {p: pattern_scenes(p, 'open') for _, p in top_open[:10]}
    close_examples = {p: pattern_scenes(p, 'close') for _, p in top_close[:10]}

    # templates por posição da frase, bloco a bloco
    position_templates = {}
    for b in range(0, len(sentence_lists), 1000):
        block = sentence_lists[b:b + 1000]
        label = f'{b + 1}-{min(b + 1000, len(sentence_lists))}'
        by_pos = {}
        for pos in range(6):
            c = Counter(prefix(ss[pos], 3) for ss in block if len(ss) > pos)
            by_pos[pos + 1] = c.most_common(5)
        position_templates[label] = by_pos
    no_fim_open = [
        i for i, scene in enumerate(scenes, 1)
        if normalize(scene).startswith('no fim')
    ]
    no_fim_comma = [
        i for i, scene in enumerate(scenes, 1)
        if normalize(scene).startswith('no fim, ')
    ]
    no_fim_any = [
        i for i, scene in enumerate(scenes, 1)
        if re.search(r'\bno fim\b|\bno final\b', normalize(scene))
    ]

    return {
        'corpus': str(corpus_path),
        'scenes': len(scenes),
        'themes_parsed': len(themes),
        'flags': flags,
        'metrics': metrics,
        'summary': {
            'severity': dict(sev_count),
            'categories': dict(cat_count),
            'critical_scenes': len(critical_scenes),
            'warning_scenes': len(warning_scenes),
            'clean_scenes': len(scenes) - len(set(critical_scenes) | set(warning_scenes)),
            'near_pairs': len(near_pairs),
            'max_sentence_count': max(sent_count.values()) if sent_count else 0,
            'sentence_distribution': dict(sorted(Counter(len(s) for s in sentence_lists).items())),
            'word_stats': {
                'mean': round(statistics.mean(m['words'] for m in metrics), 1),
                'min': min(m['words'] for m in metrics),
                'max': max(m['words'] for m in metrics),
                'median': statistics.median(m['words'] for m in metrics),
            },
            'ttr_stats': {
                'mean': round(statistics.mean(m['ttr'] for m in metrics), 3),
                'min': min(m['ttr'] for m in metrics),
            },
            'theme_overlap_stats': {
                'mean': round(statistics.mean(m['theme_overlap'] for m in metrics), 3),
                'median': round(statistics.median(m['theme_overlap'] for m in metrics), 3),
                'zero': sum(1 for m in metrics if m['theme_overlap'] == 0),
                'below_004': sum(1 for m in metrics if m['theme_overlap'] < 0.04),
            },
            'top_sentences': top_sentences,
            'top_open': top_open,
            'top_close': top_close,
            'top_prefix': top_prefix,
            'open_examples': open_examples,
            'close_examples': close_examples,
            'no_fim_open': no_fim_open,
            'no_fim_comma': no_fim_comma,
            'no_fim_any': no_fim_any,
            'position_templates': position_templates,
            'worst_themes': theme_avg,
        },
        'samples': samples,
        'near_pairs': near_pairs,
    }


# ---------------------------------------------------------------------------
# relatório markdown
# ---------------------------------------------------------------------------

def write_report(result: dict, outdir: Path) -> Path:
    s = result['summary']
    lines = []
    lines.append('# Auditoria do corpus da Natacha')
    lines.append('')
    lines.append(f"Corpus: `{result['corpus']}`")
    lines.append('')
    lines.append('## Resumo')
    lines.append('')
    lines.append('| métrica | valor |')
    lines.append('| --- | ---: |')
    lines.append(f"| cenas | {result['scenes']} |")
    lines.append(f"| temas parseados | {result['themes_parsed']} |")
    lines.append(f"| erros críticos (flags) | {s['severity'].get('critical', 0)} |")
    lines.append(f"| avisos (flags) | {s['severity'].get('warning', 0)} |")
    lines.append(f"| cenas com flag crítica | {s['critical_scenes']} |")
    lines.append(f"| cenas com aviso | {s['warning_scenes']} |")
    lines.append(f"| cenas limpas | {s['clean_scenes']} |")
    lines.append(f"| near-duplicatas > 0.62 | {s['near_pairs']} |")
    lines.append(f"| frase exata mais repetida | {s['max_sentence_count']}x |")
    lines.append(f"| cenas com abertura `no fim, ` | {len(s['no_fim_comma'])} |")
    lines.append(f"| cenas com abertura `no fim` (qualquer) | {len(s['no_fim_open'])} |")
    lines.append(f"| cenas com `no fim`/`no final` | {len(s['no_fim_any'])} |")
    lines.append(f"| palavras/cena (média/min/max) | {s['word_stats']['mean']} / {s['word_stats']['min']} / {s['word_stats']['max']} |")
    lines.append(f"| TTR médio | {s['ttr_stats']['mean']} |")
    lines.append(f"| overlap temático médio | {s['theme_overlap_stats']['mean']} |")
    lines.append(f"| cenas com overlap 0 | {s['theme_overlap_stats']['zero']} |")
    lines.append(f"| cenas com overlap < 0.04 | {s['theme_overlap_stats']['below_004']} |")
    lines.append('')
    lines.append('Distribuição de frases por cena: ' + ', '.join(f'{k}: {v}' for k, v in s['sentence_distribution'].items()))
    lines.append('')

    lines.append('## Flags por categoria')
    lines.append('')
    lines.append('| categoria | total |')
    lines.append('| --- | ---: |')
    for cat, n in sorted(s['categories'].items(), key=lambda t: (-t[1], t[0])):
        lines.append(f'| `{cat}` | {n} |')
    lines.append('')

    lines.append('## Amostras de flags críticas')
    lines.append('')
    critical_cats = [c for c in s['categories'] if any(f['category'] == c and f['severity'] == 'critical' for f in result['flags'])]
    if not critical_cats:
        lines.append('Nenhuma flag crítica.')
        lines.append('')
    for cat in critical_cats:
        items = [f for f in result['flags'] if f['category'] == cat and f['severity'] == 'critical'][:8]
        lines.append(f'### `{cat}` ({s["categories"][cat]})')
        lines.append('')
        for f in items:
            lines.append(f"- cena {f['scene']} (tema {f['theme']}): {f['evidence'] or f['message']}")
        lines.append('')

    lines.append('## Amostras de avisos')
    lines.append('')
    warning_cats = [c for c in s['categories'] if any(f['category'] == c and f['severity'] == 'warning' for f in result['flags'])]
    for cat in warning_cats:
        items = [f for f in result['flags'] if f['category'] == cat and f['severity'] == 'warning'][:6]
        lines.append(f'### `{cat}` ({s["categories"][cat]})')
        lines.append('')
        for f in items:
            lines.append(f"- cena {f['scene']} (tema {f['theme']}): {f['evidence'] or f['message']}")
        lines.append('')

    lines.append('## Frases exatas mais repetidas')
    lines.append('')
    lines.append('Limite do corpus: 10 ocorrências. Acima disso vira flag crítica.')
    lines.append('')
    for n, q in s['top_sentences'][:15]:
        lines.append(f'- {n}x `{q}`')
    lines.append('')

    lines.append('## Padrões repetidos (revisão opcional)')
    lines.append('')
    lines.append('Contagens altas aqui não são erro: são candidatas a variação pontual.')
    lines.append('')
    lines.append('### Aberturas')
    lines.append('')
    for n, p in s['top_open'][:12]:
        ex = s['open_examples'].get(p, [])[:8]
        lines.append(f"- {n}x `{p}...` — cenas {', '.join(map(str, ex))}{'...' if n > 8 else ''}")
    lines.append('')
    lines.append('### Fechamentos')
    lines.append('')
    for n, p in s['top_close'][:12]:
        ex = s['close_examples'].get(p, [])[:8]
        lines.append(f"- {n}x `{p}...` — cenas {', '.join(map(str, ex))}{'...' if n > 8 else ''}")
    lines.append('')
    lines.append('### Prefixos de frase')
    lines.append('')
    for n, p in s['top_prefix'][:12]:
        lines.append(f'- {n}x `{p}...`')
    lines.append('')
    lines.append('### Templates por posição (por bloco de 1000 cenas)')
    lines.append('')
    lines.append('Mostra o prefixo de 3 palavras mais comum em cada posição da cena.')
    lines.append('')
    for label, by_pos in s['position_templates'].items():
        lines.append(f'**Bloco {label}**')
        lines.append('')
        for pos, items in by_pos.items():
            if not items:
                continue
            top = ', '.join(f'`{p}` {n}x' for p, n in items[:3])
            lines.append(f'- frase {pos}: {top}')
        lines.append('')
    lines.append('### `no fim` / `no final`')
    lines.append('')
    lines.append(f"- cenas que começam com `no fim, `: {len(s['no_fim_comma'])}")
    lines.append(f"- cenas que começam com `no fim` (inclui `no fim da tarde`): {len(s['no_fim_open'])}")
    lines.append(f"- cenas que usam `no fim`/`no final` em qualquer posição: {len(s['no_fim_any'])}")
    if s['no_fim_comma']:
        lines.append(f"- exemplos: {', '.join(map(str, s['no_fim_comma'][:12]))}")
    lines.append('')

    lines.append('## Coerência temática (informativa)')
    lines.append('')
    lines.append('A sobreposição lexical entre a cena e a descrição do tema é só um termômetro:')
    lines.append('paráfrase e sinônimos derrubam o número sem indicar erro. Não vira flag.')
    lines.append('')
    lines.append(f"- overlap médio: {s['theme_overlap_stats']['mean']}")
    lines.append(f"- overlap mediano: {s['theme_overlap_stats']['median']}")
    lines.append(f"- cenas com overlap zero: {s['theme_overlap_stats']['zero']}")
    lines.append('')
    lines.append('Temas com menor overlap médio (para leitura manual, não para correção automática):')
    lines.append('')
    lines.append('| média | tema | descrição |')
    lines.append('| ---: | ---: | --- |')
    for avg, t, desc in s['worst_themes']:
        lines.append(f'| {avg:.3f} | {t} | {desc} |')
    lines.append('')

    lines.append('## Observações')
    lines.append('')
    lines.append('- `critical` = erro objetivo (trio, acento, pontuação, duplicata, frase exata > 10, Félix < 3 ações em 5 cenas).')
    lines.append('- `warning` = outlier para revisão: cena curta/longa, vocabulário baixo, TTR baixo, repetição interna.')
    lines.append('- Heurísticas de sintaxe que geravam falso positivo (`verbo_sem_objeto`, `elipse`, `pronome_sem_antecedente`, `uma_sem_nome`, `felix_dormiu`, `transicao_no_fim`) foram removidas.')
    lines.append('- `template_forte`, `abertura_repetida` e `fechamento_repetido` saíram das flags e viraram o painel de padrões acima.')
    lines.append('- O script não altera o corpus.')
    lines.append('')

    path = outdir / 'auditoria_corpus.md'
    path.write_text('\n'.join(lines), encoding='utf-8')
    return path


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument('corpus', nargs='?', default=str(DEFAULT_CORPUS))
    ap.add_argument('--outdir', default='/tmp/opencode/auditoria')
    ap.add_argument('--samples', type=int, default=8)
    args = ap.parse_args()

    corpus = Path(args.corpus)
    if not corpus.exists():
        print(f'corpus nao encontrado: {corpus}', file=sys.stderr)
        return 1

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    result = audit(corpus, args.samples)
    json_path = outdir / 'auditoria_corpus.json'
    json_path.write_text(json.dumps(result, ensure_ascii=False, indent=1), encoding='utf-8')
    md_path = write_report(result, outdir)

    s = result['summary']
    print(f"cenas: {result['scenes']}")
    print(f"flags criticas: {s['severity'].get('critical', 0)} em {s['critical_scenes']} cenas")
    print(f"avisos: {s['severity'].get('warning', 0)} em {s['warning_scenes']} cenas")
    print(f"cenas limpas: {s['clean_scenes']}")
    print(f"near-dups: {s['near_pairs']}")
    print(f"overlap tematico medio: {s['theme_overlap_stats']['mean']} | zero: {s['theme_overlap_stats']['zero']} | <0.04: {s['theme_overlap_stats']['below_004']}")
    print(f"relatorio: {md_path}")
    print(f"json: {json_path}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
