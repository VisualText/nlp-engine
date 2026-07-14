# VisualText NLP Engine — Version 3 Overview

*An org-wide summary of the Version 3 work across the [VisualText](https://github.com/VisualText) repositories, with emphasis on the changes developed in collaboration with Claude (Anthropic's Claude Code / Claude Opus 4.x).*

**Snapshot date:** June 2026 · **Engine line:** v3.x (latest release **v3.4.0**, 2026-06-12)

---

## What "Version 3" means

Version 3 is the release line in which the **NLP++ engine became compilable, cloud-buildable, and installable from npm**, while hardening the engine across its already-supported platforms. The headline shifts are:

1. **One-click compiled analyzers.** Compiling NLP++ analyzers and knowledge bases to native C++ shared libraries (`.dll` / `.so` / `.dylib`) was *always* technically possible — but it was hidden, brittle, and effectively reachable only by someone with deep C++ build expertise. Version 3 turns it into a **one-click process available to any user**, with no C++ knowledge required.
2. **Cloud compile.** A **cloud-compile service** builds those native libraries for Windows, Linux, and macOS on GitHub-hosted runners, so users don't need a local C++ toolchain.
3. **A new Node.js package.** Version 3 introduces first-class **Node.js bindings** (`npm install nlpplus`) that embed the engine directly — alongside the pre-existing Python package, which gained the new compile/cloud-compile APIs.
4. **Numerous cross-platform fixes.** The engine has run on Windows, Linux, and macOS for years; v3 fixed a long list of platform-specific problems surfaced by compiled mode and the cloud-build pipeline.
5. **Cross-repo release automation.** A connected "percolation" system propagates releases automatically across the engine, analyzers, dictionaries, language parsers, and the platform-distribution repos.

A large share of this work was developed with **Claude** as a pair-programming collaborator — visible in the commit history through `Co-Authored-By: Claude` trailers and the systematic `NLP-ENGINE-###` / conventional-commit message style introduced in the v3 era.

> **Why it matters.** NLP++ is *glass-box* (explainable, rule-based) NLP — and, crucially, **deterministic**: the same input always produces the same output, with rules you can read. That makes it suitable for **critical-path systems where statistical/probabilistic models cannot be trusted**. Version 3 makes that deterministic engine faster (compiled), easier to build (one-click and in the cloud), and easier to adopt (now installable from npm as well as pip). Notably, an LLM (Claude) is being used as a *development* collaborator to help build a deterministic NLP system — the LLM helps create the tools; the runtime stays transparent and reproducible.

---

## The core engine — `nlp-engine`

**Repo:** [VisualText/nlp-engine](https://github.com/VisualText/nlp-engine) · the C++ NLP++ engine.
**Version journey (Claude era):** v3.1.17 → v3.1.40 → v3.2.0 → v3.2.1 → **v3.4.0**.
**Claude involvement:** Heavy — ~50+ Claude-co-authored commits drove the v3.1.x–v3.4.0 stabilization.

### 1. Compiled-analyzer mode (NLP++ → C++ → native library)
The engine code-generates C++ from NLP++ rule files and compiles it. v3 hardened this path:
- Added dozens of **`RFASem` overloads** for NLP++ builtins (e.g. `attrtype`, `pnremoveval`, `pnvartype`) and **`Arun::assign` / `iassign` / `vargt` / `varlt`** overloads so generated code type-checks correctly.
- **Provenance comments** (`/* nlp-source */`) emitted during `-COMPILE` so generated C++ can be traced back to its NLP++ origin — and the parser/error pipeline learned to handle those inline comments.
- Fixed compiled-mode correctness bugs: SaveKB writes now flush (compiled-mode output files were silently empty), null-`cgerr` guards (fixed a compiled-mode SEGV), and function-call args hoisted into temps to fix unspecified C++ evaluation order.

### 2. Cross-platform fixes (the generated code, not the engine itself)
The engine has run on Windows, Linux, and macOS for years. What was *new* in v3 — compiled mode and the cloud-build pipeline — surfaced a string of platform-specific problems in the **generated C++** and its build, which v3 fixed:
- Gated Windows-only headers behind `#ifndef LINUX`; emitted **forward-slash include paths** and `prim/libprim.h` so generated `St*.cpp` compiles on cloud Linux.
- **Fixed compiled-RUN dispatch on Linux/macOS** to match Windows (previously Linux loaded `run.so` then immediately fell back to interpreted).
- Used the correct **`.so` / `.dylib`** extensions for compiled analyzers and KBs; implemented `make_dir` / `rm_dir` via `std::filesystem`.
- macOS-specific fixes (e.g. casting string emits to `_TCHAR*` to dodge a `<<` bool-overload bug; dropping a `-std=c++11` override so `std::filesystem` works).

### 3. Robustness & safety
- Bounded `_stprintf` writes into `MAXPATH` buffers.
- Made `NLP_ENGINE::close()` **idempotent** (and fixed a Windows tempdir-cleanup `PermissionError` downstream).
- Fixed segfaults in NLP-source-file emission and codegen edge cases.

### 4. NLP++ language features (the "lite" runtime)
- `_xVAR("attribute")` match-list special for attribute access in rules.
- **Lazy dictionary loading** — `*-full` dictionaries load one word at a time (and multiple lazy dictionaries can be open at once), dramatically cutting memory/startup cost for large lexicons.
- Allow **digits in underscore-prefixed token names** (e.g. `_token2`).
- New `loadkbb` and `loaddict` NLP++ functions.

---

## The VS Code extension — `vscode-nlp` ("VisualText")

**Repo:** [VisualText/vscode-nlp](https://github.com/VisualText/vscode-nlp) · the VS Code language extension for NLP++ (this is the "VisualText" IDE experience).
**Version:** **v3.1.24** (the extension's own v3 line).
**Claude involvement:** Major — ~20+ commits driving the entire 3.1.x line.

- **Cloud compile in the editor.** Added a cloud-compile path that submits analyzers to the compile service, verifies the engine release exists before submitting, shows quiet progress with an elapsed-time counter and a 30-minute poll, and stages the resulting `.so`/`.dylib` into `bin/` on Linux/macOS.
- **Markdown help system.** Replaced browser/HTML help with in-editor **markdown previews** (Functions, Variables, Index).
- **Packaging hygiene.** `.vscodeignore` excludes `.vscode/`, `.claude/`, and logs from the packaged `.vsix`.

---

## Native language bindings

### Node.js — `npm-package-nlpengine` (`nlpplus`) — **NEW in v3**
**Repo:** [VisualText/npm-package-nlpengine](https://github.com/VisualText/npm-package-nlpengine) · **v1.0.5** · Node native addon (node-addon-api).
Version 3's brand-new package — it brings the engine to the JavaScript/Node ecosystem for the first time.
- **Scaffolded from scratch with Claude**; Windows build bundles ICU runtime DLLs so the addon is self-contained.
- Tests isolated per-process to survive the engine's process-global teardown.
- `cloudCompile` multipart upload; publish switched to **npm trusted publishing (OIDC)**.

### Python — `py-package-nlpengine` (`NLPPlus`) — pre-existing, enhanced in v3
**Repo:** [VisualText/py-package-nlpengine](https://github.com/VisualText/py-package-nlpengine) · **v2.0.9** · pip-installable native bindings (no subprocess).
The Python package predates v3; in this cycle it gained the new compile workflow.
- Native C++ bindings (pybind / scikit-build-core), embeds the **nlp-engine v3.x** submodule.
- **New compile-mode and cloud-compile APIs** (`compile()`, `cloud_compile()`).
- `Engine.close()` + context-manager support; Python **3.13 wheels**; manual bump-and-publish-to-PyPI workflow.

### Lighter-weight wrappers
- **`ts-nlp-engine`** — a single TypeScript class that shells out to `nlp.exe`; gained compile-mode support. Positioned as the simple option, pointing production users to native `NLPPlus`.
- **`python`** — example scripts / `NLPEngine` class (**v2.0.1**); gained compile-mode support + docs.

---

## The cloud-compile backend — `nlp-compile-service` ⭐

**Repo:** [VisualText/nlp-compile-service](https://github.com/VisualText/nlp-compile-service) · the backend behind `cloud_compile()` / `cloudCompile`.
**Claude involvement:** Overwhelming — the repo is almost entirely Claude-authored.

- A **Cloudflare Worker dispatcher** (TypeScript, KV-backed) plus a **GitHub Actions runner workflow** that compiles NLP++ analyzers into native shared libraries on Windows / Linux / macOS runners.
- The hard part — **cross-platform native linking** — was solved here: defining `LINUX` for non-Windows CMake builds, wrapping ICU static libs in `--whole-archive` (fixing an `icu::ByteSink` undefined-symbol error), and `--start-group` for engine static libs on Linux. This sequence brought Linux/macOS compiled-mode online.
- Runner workflow modernized: rolling `-latest` runner labels, normalized `engineVersion` tags, no hardcoded VS 2019 paths.

---

## Analyzers, parsers & dictionaries

| Repo | Purpose | Latest | Claude role |
|------|---------|--------|-------------|
| [package-analyzers](https://github.com/VisualText/package-analyzers) | Shared analyzers (email, telephone, links, address) embedded by the Python/Node packages | **v1.0.0** | **Zone-based re-architecture** of all four analyzers (line-based → whole-text/zone-based so they work on real HTML/Markdown pages); international telephone support |
| [parse-en-us](https://github.com/VisualText/parse-en-us) | Full English (US) NLP++ parser | v1.1.2 | Cross-repo release-percolation plumbing |
| [analyzer-templates](https://github.com/VisualText/analyzer-templates) | Starter templates consumed by the VS Code extension | v1.0.11 | Percolation wiring + dispatch scaffolding |
| [analyzers](https://github.com/VisualText/analyzers) | Umbrella collection of NLP++ analyzers | v1.9.5 | Percolation automation |
| [dehilster-analyzers](https://github.com/VisualText/dehilster-analyzers) | Personal analyzer/dictionary collection | v0.26.0 | Claude-built **English Phrase analyzer** |
| [visualtext-files](https://github.com/VisualText/visualtext-files) | Shared dictionaries, KBs, help docs | v2.2.2 | Root-lemma English dictionary + `en-full.kbb` KB; markdown help conversion |

---

## Platform distribution repos

**Repos:** [nlp-engine-linux](https://github.com/VisualText/nlp-engine-linux), [nlp-engine-mac](https://github.com/VisualText/nlp-engine-mac), [nlp-engine-windows](https://github.com/VisualText/nlp-engine-windows) · ready-to-run, prebuilt distributions of the engine for each OS. **All currently ship engine v3.4.0**, in lockstep.

- **Automated mirroring.** Each repo auto-updates via `repository_dispatch` when the upstream engine cuts a release — downloads per-OS release ZIPs, swaps binaries, commits, and tags to match upstream.
- **Claude fixes:**
  - A **release-automation race** where `git rm` ran *after* extraction, silently dropping a just-extracted directory (the v3.1.53/54 `compile-libs/` vs `ubuntu-*/` "flip-flop"). Diagnosed and fixed in linux + mac, aligned to the windows reference ordering.
  - **Full-analyzer compile scripts** rewritten from KB-only to building the *full* analyzer (`run/` + `kb/`) into native `.so` / `.dylib` / `.dll`, with per-platform linker specifics (GNU `--whole-archive` vs macOS `-force_load`; VS detection + ICU import-lib generation on Windows).
  - A real macOS engine bug: `make_analyzer` wrote `run/<pass>.cpp` without creating `run/` first, silently producing an **empty run tree** on macOS — fixed with directory pre-creation plus a loud guard.

---

## Cross-cutting theme: release percolation

Claude built a connected **cross-repo dispatch / auto-tag / release-listener system**. A release in one repo (e.g. `parse-en-us`) percolates downstream — to `analyzer-templates`, `package-analyzers`, `analyzers`, the Python/Node packages (via a shared `package-analyzers` submodule + auto-publish-on-bump), and the platform distribution repos — so a single human action fans out into coordinated, version-tagged releases across the org.

---

## Version reference (June 2026)

| Component | Version |
|-----------|---------|
| **nlp-engine** (core) | **v3.4.0** |
| nlp-engine-linux / -mac / -windows | v3.4.0 (mirrors core) |
| vscode-nlp (VisualText extension) | v3.1.24 |
| py-package-nlpengine (`NLPPlus`) | v2.0.9 |
| npm-package-nlpengine (`nlpplus`) | v1.0.5 |
| package-analyzers | v1.0.0 |
| visualtext-files | v2.2.2 |

*Not every repo carries the literal number "3" — the unifying thread is the work that landed alongside the **engine v3.x** line.*
