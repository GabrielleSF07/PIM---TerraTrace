# Publicação no GitHub Pages

O repositório publica a pasta `docs` da branch `main`. Essa pasta precisa conter o site compilado, não apenas o código React.

Após alterar o site, execute:

```powershell
npm.cmd run build:pages
```

Envie as alterações de código e os arquivos gerados em `docs` ao GitHub. O Pages publicará a nova versão após o push.

`npm.cmd run build` continua gerando uma versão em `dist` para outras hospedagens.

O Vite usa caminhos relativos; imagens e downloads usam `import.meta.env.BASE_URL`, permitindo servir o site no subdiretório do repositório.

`--emptyOutDir false` preserva os arquivos de documentação existentes em `docs`.
