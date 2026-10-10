# NLP++ language support

Where NLP++ is supported outside VisualText: syntax highlighters, editors, the language server and
language directories. Each entry links to the pull request, package or page, and the status badges
update live.

Highlighting covers NLP++ pass files (`.nlp`, `.pat`) and the analyzer's other files: dictionaries
(`.dict`), knowledge bases (`.kbb`), the pass sequence (`analyzer.seq`) and parse trees (`.tree`).

## Language server

The NLP++ language server (outline, hover, go-to-definition, references, rename, completion,
diagnostics, formatting) is the one inside the VS Code extension. It is also published on its own
for other editors.

| Where | What | Status |
|---|---|---|
| [npm: `nlpplus-language-server`](https://www.npmjs.com/package/nlpplus-language-server) | The server for any LSP editor (`npm i -g nlpplus-language-server`). Published by each extension release ([vscode-nlp#1223](https://github.com/VisualText/vscode-nlp/pull/1223)) | [![npm](https://img.shields.io/npm/v/nlpplus-language-server)](https://www.npmjs.com/package/nlpplus-language-server) |
| [Microsoft's list of language servers](https://microsoft.github.io/language-server-protocol/implementors/servers/) | Listing | [![PR](https://img.shields.io/github/pulls/detail/state/microsoft/language-server-protocol/2331)](https://github.com/microsoft/language-server-protocol/pull/2331) |
| [nvim-lspconfig](https://github.com/neovim/nvim-lspconfig) | Neovim setup in one line: `vim.lsp.enable('nlpplus_ls')` | [![PR](https://img.shields.io/github/pulls/detail/state/neovim/nvim-lspconfig/4548)](https://github.com/neovim/nvim-lspconfig/pull/4548) |
| [Mason](https://github.com/mason-org/mason-registry) | Neovim install: `:MasonInstall nlpplus-language-server` | [![PR](https://img.shields.io/github/pulls/detail/state/mason-org/mason-registry/17377)](https://github.com/mason-org/mason-registry/pull/17377) |
| Helix, Emacs, Sublime | Setup in the [language server README](https://github.com/VisualText/vscode-nlp/tree/master/language-server#editor-setup) | Documented |

## Syntax highlighting

| Highlighter | Used by | Covers | Status |
|---|---|---|---|
| [VisualText for VS Code](https://marketplace.visualstudio.com/items?itemName=dehilster.nlp) | VS Code | all formats | Released |
| [nlpplus-tmbundle](https://github.com/VisualText/nlpplus-tmbundle) (TextMate grammars) | VS Code, Sublime Text, `bat`, any TextMate-grammar editor | all formats | Released |
| [Pygments](https://pygments.org) | Sphinx, Read the Docs, Jupyter, MkDocs, Hugo | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | [![PR](https://img.shields.io/github/pulls/detail/state/pygments/pygments/3238)](https://github.com/pygments/pygments/pull/3238) |
| [highlight.js](https://highlightjs.org): [highlightjs-nlpplus](https://github.com/VisualText/highlightjs-nlpplus) | Discourse, many documentation sites and blogs | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | Released; listing [![PR](https://img.shields.io/github/pulls/detail/state/highlightjs/highlight.js/4557)](https://github.com/highlightjs/highlight.js/pull/4557) |
| [Prism](https://prismjs.com) | Docusaurus, many documentation sites | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | [![PR](https://img.shields.io/github/pulls/detail/state/PrismJS/prism/4163)](https://github.com/PrismJS/prism/pull/4163) |
| [Rouge](https://github.com/rouge-ruby/rouge) | GitLab, Jekyll, GitHub Pages | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | Ready on [ddehilster/rouge:add-nlpplus-lexer](https://github.com/rouge-ruby/rouge/compare/main...ddehilster:rouge:add-nlpplus-lexer); Rouge is not accepting pull requests from new contributors right now |

## Editors

| Editor | What | Status |
|---|---|---|
| [Vim](https://github.com/vim/vim) (and Neovim, which follows Vim) | Recognize `.nlp` files as NLP++ | [![PR](https://img.shields.io/github/pulls/detail/state/vim/vim/21480)](https://github.com/vim/vim/pull/21480) |
| Neovim | Language server | See nvim-lspconfig and Mason above |

## Language directories

| Where | Status |
|---|---|
| [PLDB](https://pldb.io/concepts/nlp-plus-plus.html) | Live. Entry [![PR](https://img.shields.io/github/pulls/detail/state/breck7/pldb/658)](https://github.com/breck7/pldb/pull/658), creators [![PR](https://img.shields.io/github/pulls/detail/state/breck7/pldb/659)](https://github.com/breck7/pldb/pull/659) |
| [Rosetta Code](https://rosettacode.org/wiki/Category:NLP%2B%2B) | Live |
| [VS Code Marketplace](https://marketplace.visualstudio.com/items?itemName=dehilster.nlp) | Live |
| Wikidata | Not yet: the entry is drafted and needs to be submitted from a Wikimedia account |
| [Open VSX](https://open-vsx.org) (VSCodium, Cursor, Windsurf) | Not yet: needs a publishing token |
| [GitHub Linguist](https://github.com/github-linguist/linguist) | Not yet. Linguist needs at least 2,000 `.nlp` files on public GitHub from many unrelated people. Start new analyzers from the [NLP++ analyzer template](https://github.com/VisualText/nlpplus-analyzer-template) and tag repositories `nlp-plus-plus` to help |
| [Shiki](https://shiki.style) | Not yet. Shiki adds languages that GitHub Linguist already knows |

## Fixes made along the way

Work on the listings above turned up these, all merged in VisualText repos:

| Pull request | What |
|---|---|
| [vscode-nlp#1219](https://github.com/VisualText/vscode-nlp/pull/1219) | The language server as an npm package. Fixes for editors other than VS Code |
| [vscode-nlp#1221](https://github.com/VisualText/vscode-nlp/pull/1221) | Completion offers the `base` rule modifier |
| [nlpplus-tmbundle#2](https://github.com/VisualText/nlpplus-tmbundle/pull/2), [vscode-nlp#1222](https://github.com/VisualText/vscode-nlp/pull/1222) | Highlighting for `base`, all 17 `_x` node constants, and the closing paren of a `match=( )` list |
| [vscode-nlp#1223](https://github.com/VisualText/vscode-nlp/pull/1223) | Each release publishes the language server to npm |
| [nlp-engine#745](https://github.com/VisualText/nlp-engine/pull/745) | Each run starts with an empty `<file>_log` folder (engine 4.2.1) |
| [visualtext-files#227](https://github.com/VisualText/visualtext-files/pull/227) | The `group()` help page shows rule element numbers, not nodes |

Still open: [nlp-engine#408](https://github.com/VisualText/nlp-engine/issues/408) ![issue](https://img.shields.io/github/issues/detail/state/VisualText/nlp-engine/408): `$text` is cut short on nodes that start with a multi-word dictionary entry.
