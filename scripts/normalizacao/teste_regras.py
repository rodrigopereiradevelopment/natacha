import language_tool_python
t = language_tool_python.LanguageTool('pt-BR')
for m in t.check('voce nao esta bem, pais e pais'):
    print(m.rule_id, '|', m.message, '|', m.replacements[:2])
t.close()
