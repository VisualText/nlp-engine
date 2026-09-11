# VisualText NLP Engine — Version 4 Overview

*An org-wide summary of the Version 4 work across the [VisualText](https://github.com/VisualText) repositories, with emphasis on the changes developed in collaboration with Claude (Anthropic's Claude Code / Claude Opus 5).*

**Snapshot date:** September 2026 · **Engine:** v4.0.0 · **Extension:** v4.0.0

> **Release status.** Engine and extension are both at **4.0.0**. The work shipped incrementally across the 3.10–3.13 (engine) and 3.13–3.17 (extension) lines and is live on the VS Code Marketplace; 4.0.0 is the point at which it is named. The announcement material in this folder is written and waiting on screenshots — see [Announcing it](#announcing-it).

---

## What "Version 4" means

Version 3 was about **how you build, deploy, and install** an analyzer: one-click compiled analyzers, cloud builds, npm and pip packages. Version 4 is about **how you develop and understand one**. The headline shift is a single sentence:

> **You can now watch an NLP++ analyzer run, and stop it wherever you like.**

Concretely:

1. **A real debugger.** Breakpoints, stepping, variables, and a call stack — inside a running analyzer. Stop on a rule before it is tried, on the statement in an `@POST` before it executes, or inside a `@DECL` function three calls deep, and read `G()`, `L()`, `S()`, `X()` and `N()` at that moment.
2. **Two debuggers, honestly separated.** A **replay** debugger steps the per-pass parse trees a finished run already wrote to disk — so it can move *backwards*, which a live engine cannot. A **live** debugger drives a running `nlp` process stopped inside its rule-matching loop — so it can stop between individual rule attempts, which a dump cannot. They are offered as two modes rather than merged, because a single session that silently lost Step Back depending on a config value would be worse than two honest ones.
3. **A language server.** Every language feature — outline, hover, go-to-definition, find references, rename, completion, signature help, folding, semantic highlighting, quick fixes, structural diagnostics and formatting — now runs in a separate process speaking LSP. The features themselves are not new; what is new is that they no longer compete with the tree views for the extension host, and that **any LSP-speaking editor can now run them**, not just VS Code.
4. **A debug protocol in the engine.** The engine gained a small, documented, newline-delimited JSON server (`nlp -DEBUG <port>`) with a capability handshake, so a client can ask what a given build supports rather than inferring it from a version string.
5. **Tests where there were none.** The debugger's two layers — the engine protocol and the DAP session — are now covered by suites that are checked against reintroduced bugs, and CI actually runs them. It did not before.

> **Why it matters.** Version 3's case for NLP++ was that it is *glass-box*: explainable, rule-based, and **deterministic** — the same input always produces the same output, with rules a human can audit. That argument had a gap. You could read the rules and you could read the result, but you could not watch the decision being made; the only window into a run was a `.tree` dump read after the fact. Version 4 closes it. Auditing why a decision was made is no longer an act of inference — you can stop on the line and look. **The glass box now has a door.**

---

## The core engine — `nlp-engine`

**Repo:** [VisualText/nlp-engine](https://github.com/VisualText/nlp-engine) · the C++ NLP++ engine.
**Version journey:** v3.10.0 → v3.10.1 → v3.11.0 → v3.12.0 → v3.12.1 → v3.12.2 → v3.12.3 → v3.13.0 → **v4.0.0**.

### 1. A debug server (`-DEBUG <port>`)

Started with `-DEBUG`, the engine listens on a loopback port and speaks newline-delimited JSON, one object per line, in both directions. The engine runs until it reaches a pause point, sends an unsolicited `stopped` event, and blocks reading commands until one of them resumes it — strictly half-duplex, which is what makes it simple enough to reason about.

The message catalogue lives at the top of `lite/nlpdebug.cpp` and is the authority.

**Pause points**, each an inline test of one static `bool` so an un-attached engine pays essentially nothing:

- a pass boundary (`passStart` / `passEnd`)
- a rule about to be tried, one that matched, and one that failed
- a statement about to run in an `@CODE`, `@POST` or `@DECL` body

**Commands:** `continue`, `stepRule`, `stepMatch`, `stepPass`, `stepStatement`, `stepOverStatement`, `stepOutStatement`, `setBreakpoints`, `stopOnFailure`, `state`, `rule`, `node`, `tree`, `globals`, `locals`, `suggested`, `context`, `collect`, `stack`, `capabilities`, `detach`.

### 2. Arming

The hooks sit in the hottest loop in the engine, and the engine parses **its own grammar** with the same `Pat` code before it analyses anything. An un-armed build stops hundreds of times on rules nobody wrote. The debugger is therefore armed only for the analysis itself — and that is asserted in the regression suite, because it is exactly the kind of property a later optimisation removes without anyone noticing.

### 3. Statement-level execution (v3.12.0)

`@POST`, `@CODE` and `@DECL` are ordinary imperative code and all three run through the same `Istmt::eval` loop, so one hook covers them. A stop is reported **before** the statement runs — the state you read is the state that line is about to act on, not its result. `Ifunc::eval` already swapped the current pass to the one a function was *written* in, which is what makes a breakpoint inside a `@DECL` land in the right file.

### 4. Four stepping fixes found by using it (v3.12.1 – v3.12.3)

Each was found within hours of the feature reaching a real analyzer, and each was silent — nothing threw, the debugger simply told a slightly wrong story:

| version | fix |
| --- | --- |
| 3.12.1 | A block holding exactly **one** statement is kept as a bare statement rather than a one-element list, and only the list path carried the pause point — so a single-statement `if` body, much the commonest shape in NLP++, was stepped straight over. Affected `if`, `else` and `while` alike. |
| 3.12.2 | A loop's condition is re-evaluated inside `Iwhilestmt::eval`'s own loop, which never passes back through the statement hook — so the `while` line was reported once and never again. Stepping went from the bottom of the body straight back to its top, as though the loop had no test in it. |
| 3.12.3 | An if-statement took its line from its **body** rather than from `if (cond)`. Identical whenever the body opens with a brace on the `if` line; write the body on the next line unbraced and the test never appeared at all. |
| 3.12.1 | (Same change.) `parse->line_` was left pointing at whatever ran last inside a one-statement block, so an error raised in one was reported against the wrong line. |

### 5. Call stacks (v3.13.0)

The engine records each live call — the function entered, and the pass and line it was called **from**, captured before the current pass is swapped to the defining one.

Nothing pops the list. `Ifunc::eval` has several ways out — an early return, an error, `exitpass` — and a pop placed on one would be missed by the others, leaving a stack that drifts deeper the longer a run goes on. Instead a call at depth *d* truncates the list to *d-1* and appends, so it is corrected by the next call at that depth and a reader only ever looks at the first `depth` entries. The suite reads the stack three deep *after* shallower calls have come and gone, which exercises the truncation rather than just the appending.

### 6. Capability handshake

```
{"command":"capabilities"}
  -> {"ok":true,"capabilities":["statements","variables","nodeText","callStack"]}
```

Asked rather than inferred from the version string. A client pairs with whatever engine the user happens to have installed, and a breakpoint is set long before anything could be tried and found missing — an accepted breakpoint that never fires is the worst of the available outcomes.

---

## The VS Code extension — `vscode-nlp`

**Repo:** [VisualText/vscode-nlp](https://github.com/VisualText/vscode-nlp)
**Version journey:** 3.13.0 → 3.14.x → 3.15.x → 3.16.x → 3.17.0 → **4.0.0**.

### 1. The language server (3.13.0)

Twelve providers moved out of the extension host into a Node process speaking LSP. The analysis engines were already free of any VS Code dependency, so the move added no duplicate logic — and the build enforces that by compiling the server under a Node-only config, where an accidental `vscode` import fails rather than ships.

Three bugs surfaced during the migration, all of which would have been invisible in normal use: language features stopped answering for unsaved buffers when the server was scoped to saved files only; formatting could hang forever against an editor that does not implement `workspace/configuration`; and asking for the text of one line returned everything from that line to the end of the file.

### 2. The replay debugger (3.13.0)

F5 after a run steps the per-pass parse trees already on disk. Because the whole run is recorded rather than generated as you go, **Step Back and reverse-continue cost nothing** — you can walk a node's history in either direction instead of re-running the analyzer to get back where you were. Granularity is the pass.

### 3. The live debugger (3.14.0 onward)

Drives a real engine. What each release added:

| version | |
| --- | --- |
| 3.14.0 | Rules debugged as they run — stop between individual rule attempts, see a rule that failed and how far it got. |
| 3.14.1 | All five NLP++ variable kinds: `G()` globals, `L()` locals, `S()` suggested, `X()` context, `N()` matched elements. |
| 3.14.5 | The text a rule is being matched against, sent by the engine from its own buffer. |
| 3.15.0 | **Nodes in play** — at a match, exactly the nodes the rule took, labelled `N(1)`, `N(2)`… the same names you would write in the rule. At an attempt, the current node and the candidates after it. |
| 3.15.1 | A breakpoint anywhere inside a rule stops on that rule. |
| 3.16.0 | Breakpoints in `@CODE`, `@POST` and `@DECL`, and context-sensitive stepping. |
| 3.16.1 | Starting the debugger lands on real code, not the built-in tokenizer. |
| 3.16.2 | Starting with a breakpoint set takes you to the breakpoint. |
| 3.17.0 | The Call Stack draws the calls that led here. |
| 4.0.0 | The line is named Version 4. |

### 4. Context-sensitive stepping

An analyzer has two kinds of execution in it and one set of buttons:

| | at a rule | in a statement body |
| --- | --- | --- |
| **Step Over** | next rule tried | next statement, calls run whole |
| **Step Into** | next rule that matched | next statement, entering a call |
| **Step Out** | next pass | until this function returns |

---

## Testing

The debugger has two layers and both now have suites that are checked against **reintroduced bugs** rather than merely passing:

- **Engine protocol** — a Python driver runs the fixture analyzer under `-DEBUG` across three engine sessions and asserts on what is reported at each pause point. **68 assertions.** Every block shape is walked, because they differ in the engine and not just on the page.
- **DAP session** — the session is driven in process over a pair of streams against a fake engine: no VS Code, no Electron, no `nlp` binary. **44 assertions.**
- **Engine client** — the wire: split reads, coalesced messages, correlation, waiters left hanging when the engine exits. **68 assertions.**

Each of the shipped debugger bugs was put back and confirmed to fail the suite — 1, 2, 3 and 5 assertions respectively.

**CI never ran the debugger or trace tests at all.** They existed in `package.json` and no workflow invoked them, which is how three bugs reached a release with a green tick. Now wired in.

---

## Known limits

Stated plainly, because a debugger that is quietly wrong about something is worse than one that says so:

- **Selecting an outer call frame does not rewind the panes.** The engine keeps one set of locals, the live one, so every scope reads current state whichever frame is selected.
- **`if (cond) stmt;` written entirely on one line reports that line twice**, because two statements genuinely run there. Nothing short of column information could separate them.
- **The replay debugger's granularity is the pass**, not the rule: between two snapshots the engine ran a whole pass, so there is no inspecting a partial match and no changing a value and continuing.
- **Statement stepping applies to interpreted passes.** A natively compiled pass has no NLP++ line to stop on.
- **No conditional breakpoints.**

---

## Announcing it

The version numbers moved first, deliberately: the code was already published, so the bump names what users can already install rather than gating it. Two things are worth having in place before the blog and LinkedIn posts go out:

1. **Screenshots.** `BLOG-version-4.md` carries four marked slots with notes on what to capture. A debugger article without pictures of the debugger is a hard sell.
2. **Mileage.** A few days of real use. Six bugs surfaced in the first hours this met a real analyzer, every one of them silent — all fixed and all now covered by tests that were verified to catch them, but an announcement points attention at the newest surface.

A **walkthrough** is the other thing worth writing: a debugger nobody knows how to start is invisible.

### Version numbering

4.0.0 is a **marketing major, not a semver one** — nothing in this line breaks compatibility, and the engine's protocol additions are all additive behind a capability handshake. The bump also resynchronises the two version lines, which had drifted to engine 3.13 against extension 3.17 under a single "Version 3" brand.

---

## Built with an AI collaborator

As with Version 3, much of this line was developed in collaboration with **Claude** (Anthropic's Claude Code), visible in the commit history through `Co-Authored-By` trailers.

The symmetry Version 3 pointed at gets sharper here. A large language model helped build the tooling that makes a **deterministic** system auditable in motion — breakpoints, a call stack, variables you can read at the moment a rule fires. The LLM is the development partner; the runtime stays transparent and reproducible. You can always read the rules, and now you can watch them run.
