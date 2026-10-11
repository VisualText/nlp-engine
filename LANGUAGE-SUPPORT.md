# NLP++ language support

Where NLP++ is supported outside VisualText: syntax highlighters, editors, the language server and
language directories. Each entry links to the pull request, package or page, and the status badges
update live.

Highlighting covers NLP++ pass files (`.nlp`, `.pat`) and the analyzer's other files: dictionaries
(`.dict`), knowledge bases (`.kbb`), the pass sequence (`analyzer.seq`) and parse trees (`.tree`).

## Pull requests in other repositories

Every pull request opened in another project for NLP++, in one place. The badges show each one's
current state, and the sections below give the details.

| Repository | Pull request | What it adds | Status |
|---|---|---|---|
| Pygments | [pygments#3238](https://github.com/pygments/pygments/pull/3238) | Highlighting for pass files and the four data formats | [![PR](https://img.shields.io/github/pulls/detail/state/pygments/pygments/3238)](https://github.com/pygments/pygments/pull/3238) |
| highlight.js | [highlight.js#4557](https://github.com/highlightjs/highlight.js/pull/4557) | Listing for highlightjs-nlpplus | [![PR](https://img.shields.io/github/pulls/detail/state/highlightjs/highlight.js/4557)](https://github.com/highlightjs/highlight.js/pull/4557) |
| Prism | [prism#4163](https://github.com/PrismJS/prism/pull/4163) | Highlighting for pass files and the four data formats | [![PR](https://img.shields.io/github/pulls/detail/state/PrismJS/prism/4163)](https://github.com/PrismJS/prism/pull/4163) |
| Helix | [helix#16372](https://github.com/helix-editor/helix/pull/16372) | NLP++ and its four data formats, with the language server | [![PR](https://img.shields.io/github/pulls/detail/state/helix-editor/helix/16372)](https://github.com/helix-editor/helix/pull/16372) |
| Vim | [vim#21480](https://github.com/vim/vim/pull/21480) | Recognize `.nlp` files (Neovim follows Vim) | [![PR](https://img.shields.io/github/pulls/detail/state/vim/vim/21480)](https://github.com/vim/vim/pull/21480) |
| Microsoft LSP site | [language-server-protocol#2331](https://github.com/microsoft/language-server-protocol/pull/2331) | Listing for the language server | [![PR](https://img.shields.io/github/pulls/detail/state/microsoft/language-server-protocol/2331)](https://github.com/microsoft/language-server-protocol/pull/2331) |
| nvim-lspconfig | [nvim-lspconfig#4548](https://github.com/neovim/nvim-lspconfig/pull/4548) | Neovim language server setup | [![PR](https://img.shields.io/github/pulls/detail/state/neovim/nvim-lspconfig/4548)](https://github.com/neovim/nvim-lspconfig/pull/4548) |
| Mason | [mason-registry#17377](https://github.com/mason-org/mason-registry/pull/17377) | Neovim language server install | [![PR](https://img.shields.io/github/pulls/detail/state/mason-org/mason-registry/17377)](https://github.com/mason-org/mason-registry/pull/17377) |
| Learn X in Y Minutes | [learnxinyminutes-docs#5628](https://github.com/adambard/learnxinyminutes-docs/pull/5628) | NLP++ tour | [![PR](https://img.shields.io/github/pulls/detail/state/adambard/learnxinyminutes-docs/5628)](https://github.com/adambard/learnxinyminutes-docs/pull/5628) |
| tree-sitter workflows | [workflows#75](https://github.com/tree-sitter/workflows/pull/75) | Fix found while releasing tree-sitter-nlpplus | [![PR](https://img.shields.io/github/pulls/detail/state/tree-sitter/workflows/75)](https://github.com/tree-sitter/workflows/pull/75) |
| PLDB | [pldb#658](https://github.com/breck7/pldb/pull/658), [pldb#659](https://github.com/breck7/pldb/pull/659) | Language entry and its creators | [![PR](https://img.shields.io/github/pulls/detail/state/breck7/pldb/658)](https://github.com/breck7/pldb/pull/658) [![PR](https://img.shields.io/github/pulls/detail/state/breck7/pldb/659)](https://github.com/breck7/pldb/pull/659) |

Ready but not yet submitted:

| Where | Waiting on |
|---|---|
| [Rouge](https://github.com/rouge-ruby/rouge) | Open the pull request from [the compare page](https://github.com/rouge-ruby/rouge/compare/main...ddehilster:rouge:add-nlpplus-lexer); GitHub blocked opening it from the command line |
| [Zed extensions](https://github.com/zed-industries/extensions) | A hands-on test of [zed-nlpplus](https://github.com/VisualText/zed-nlpplus) in Zed, which Zed requires before submission |
| [MELPA](https://github.com/melpa/melpa) | 2026-11-10: MELPA accepts a package once its repository has been public for a month ([nlpplus-ts-mode](https://github.com/VisualText/nlpplus-ts-mode)) |
| [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter) | The Vim pull request above, then Neovim picking up the filetype |

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
| [tree-sitter-nlpplus](https://github.com/VisualText/tree-sitter-nlpplus) (tree-sitter grammars) | Neovim, Helix, Zed, Emacs | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | [![release](https://img.shields.io/github/v/release/VisualText/tree-sitter-nlpplus)](https://github.com/VisualText/tree-sitter-nlpplus/releases) [![CI](https://github.com/VisualText/tree-sitter-nlpplus/actions/workflows/ci.yml/badge.svg)](https://github.com/VisualText/tree-sitter-nlpplus/actions/workflows/ci.yml) |
| [Pygments](https://pygments.org) | Sphinx, Read the Docs, Jupyter, MkDocs, Hugo | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | [![PR](https://img.shields.io/github/pulls/detail/state/pygments/pygments/3238)](https://github.com/pygments/pygments/pull/3238) |
| [highlight.js](https://highlightjs.org): [highlightjs-nlpplus](https://github.com/VisualText/highlightjs-nlpplus) | Discourse, many documentation sites and blogs | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | Released; listing [![PR](https://img.shields.io/github/pulls/detail/state/highlightjs/highlight.js/4557)](https://github.com/highlightjs/highlight.js/pull/4557) |
| [Prism](https://prismjs.com) | Docusaurus, many documentation sites | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | [![PR](https://img.shields.io/github/pulls/detail/state/PrismJS/prism/4163)](https://github.com/PrismJS/prism/pull/4163) |
| [Rouge](https://github.com/rouge-ruby/rouge) | GitLab, Jekyll, GitHub Pages | pass files, `.dict`, `.kbb`, `analyzer.seq`, `.tree` | Ready on [ddehilster/rouge:add-nlpplus-lexer](https://github.com/rouge-ruby/rouge/compare/main...ddehilster:rouge:add-nlpplus-lexer); Rouge is not accepting pull requests from new contributors right now |

## Editors

| Editor | What | Status |
|---|---|---|
| [Vim](https://github.com/vim/vim) (and Neovim, which follows Vim) | Recognize `.nlp` files as NLP++ | [![PR](https://img.shields.io/github/pulls/detail/state/vim/vim/21480)](https://github.com/vim/vim/pull/21480) |
| Neovim | Language server | See nvim-lspconfig and Mason above |
| [Helix](https://github.com/helix-editor/helix) | NLP++ and its four data formats, with the language server | [![PR](https://img.shields.io/github/pulls/detail/state/helix-editor/helix/16372)](https://github.com/helix-editor/helix/pull/16372) |
| [Zed](https://zed.dev): [zed-nlpplus](https://github.com/VisualText/zed-nlpplus) | NLP++ and its four data formats, with the language server | Extension built [![CI](https://github.com/VisualText/zed-nlpplus/actions/workflows/ci.yml/badge.svg)](https://github.com/VisualText/zed-nlpplus/actions/workflows/ci.yml); Zed's registry needs a hands-on test in Zed before submission |
| [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter) | Highlighting in Neovim with `:TSInstall nlpplus` | Not yet: waits for the Vim PR above, since nvim-treesitter only adds languages Neovim already detects. Until then, setup is in the [tree-sitter-nlpplus README](https://github.com/VisualText/tree-sitter-nlpplus#using-it) |
| Emacs: [nlpplus-ts-mode](https://github.com/VisualText/nlpplus-ts-mode) | Modes for NLP++ and its four data formats, with indentation, imenu and the language server through Eglot | Released [![CI](https://github.com/VisualText/nlpplus-ts-mode/actions/workflows/ci.yml/badge.svg)](https://github.com/VisualText/nlpplus-ts-mode/actions/workflows/ci.yml); MELPA listing after 2026-11-10 (MELPA wants a repository to be public for a month first) |

## Language directories

| Where | Status |
|---|---|
| [PLDB](https://pldb.io/concepts/nlp-plus-plus.html) | Live. Entry [![PR](https://img.shields.io/github/pulls/detail/state/breck7/pldb/658)](https://github.com/breck7/pldb/pull/658), creators [![PR](https://img.shields.io/github/pulls/detail/state/breck7/pldb/659)](https://github.com/breck7/pldb/pull/659) |
| [Rosetta Code](https://rosettacode.org/wiki/Category:NLP%2B%2B) | Live: 15 tasks, each run with engine 4.2.2, including Letter frequency and Word frequency |
| [Learn X in Y Minutes](https://learnxinyminutes.com) | NLP++ tour [![PR](https://img.shields.io/github/pulls/detail/state/adambard/learnxinyminutes-docs/5628)](https://github.com/adambard/learnxinyminutes-docs/pull/5628) |
| [VS Code Marketplace](https://marketplace.visualstudio.com/items?itemName=dehilster.nlp) | Live |
| Wikidata | Not yet: the entry is drafted and needs to be submitted from a Wikimedia account |
| [Open VSX](https://open-vsx.org) (VSCodium, Cursor, Windsurf) | Not yet: needs a publishing token |
| [GitHub Linguist](https://github.com/github-linguist/linguist) | Not yet. Linguist needs at least 2,000 `.nlp` files on public GitHub from many unrelated people. Start new analyzers from the [NLP++ analyzer template](https://github.com/VisualText/nlpplus-analyzer-template) and tag repositories `nlp-plus-plus` to help |
| [Shiki](https://shiki.style) | Not yet. Shiki adds languages that GitHub Linguist already knows |

## Fixes made along the way

Work on the listings above turned up these fixes:

| Pull request | What |
|---|---|
| [vscode-nlp#1219](https://github.com/VisualText/vscode-nlp/pull/1219) | The language server as an npm package. Fixes for editors other than VS Code |
| [vscode-nlp#1221](https://github.com/VisualText/vscode-nlp/pull/1221) | Completion offers the `base` rule modifier |
| [nlpplus-tmbundle#2](https://github.com/VisualText/nlpplus-tmbundle/pull/2), [vscode-nlp#1222](https://github.com/VisualText/vscode-nlp/pull/1222) | Highlighting for `base`, all 17 `_x` node constants, and the closing paren of a `match=( )` list |
| [vscode-nlp#1223](https://github.com/VisualText/vscode-nlp/pull/1223) | Each release publishes the language server to npm |
| [nlp-engine#745](https://github.com/VisualText/nlp-engine/pull/745) | Each run starts with an empty `<file>_log` folder (engine 4.2.1) |
| [tree-sitter/workflows#75](https://github.com/tree-sitter/workflows/pull/75) | Fix for tree-sitter's shared release and packaging workflows, which dropped a repository's main grammar when its other grammars live in subfolders (found when tree-sitter-nlpplus v0.1.0 shipped without its main WASM build) [![PR](https://img.shields.io/github/pulls/detail/state/tree-sitter/workflows/75)](https://github.com/tree-sitter/workflows/pull/75) |
| [visualtext-files#227](https://github.com/VisualText/visualtext-files/pull/227) | The `group()` help page shows rule element numbers, not nodes |
| [visualtext-files#228](https://github.com/VisualText/visualtext-files/pull/228) | The `loaddict` and `loadkbb` examples used a `>>` operator NLP++ doesn't have; `loaddict` now warns that a `#` in a `.dict` line drops the whole entry [![PR](https://img.shields.io/github/pulls/detail/state/VisualText/visualtext-files/228)](https://github.com/VisualText/visualtext-files/pull/228) |
| [nlp-engine#750](https://github.com/VisualText/nlp-engine/pull/750) | `cout()` output reaches the terminal in command-line runs (engine 4.2.2); it was going to a leftover test file [![PR](https://img.shields.io/github/pulls/detail/state/VisualText/nlp-engine/750)](https://github.com/VisualText/nlp-engine/pull/750) |
| [nlp-engine#752](https://github.com/VisualText/nlp-engine/pull/752) | `factorial()` computes up to 20! on Windows (engine 4.2.3); it reported an overflow from 13! on |
| [nlp-engine#754](https://github.com/VisualText/nlp-engine/pull/754) | Engine status lines go to stderr, so standard output holds only what the analyzer prints (engine 4.2.4) |
| [vscode-nlp#1224](https://github.com/VisualText/vscode-nlp/pull/1224), [vscode-nlp#1225](https://github.com/VisualText/vscode-nlp/pull/1225) | VisualText shows `cout()` output in an NLP++ Analyzer Output panel (extension 4.3.4), and the release workflow's language server step no longer blocks the Marketplace |

Still open: [nlp-engine#408](https://github.com/VisualText/nlp-engine/issues/408) ![issue](https://img.shields.io/github/issues/detail/state/VisualText/nlp-engine/408): `$text` is cut short on nodes that start with a multi-word dictionary entry.
