# LinkedIn Post — NLP++ Engine Version 4

---

🔍 **NLP++ Engine v4 is here — now you can watch your analyzer run.**

For years NLP++ has given you what LLMs can't: **explainable, rule-based, DETERMINISTIC** language analysis — same input, same output, every time, with rules you can audit. That's what makes it usable in **critical-path systems where statistical models simply can't go.**

But there was a gap in that promise, and it's worth naming. You could read the rules. You could read the result. You **couldn't watch the decision being made** — the only window into a run was a parse-tree dump, read afterwards, from which you inferred backwards.

**Version 4 closes it. The glass box now has a door.** 🚪

🐛 **A real debugger.** Press F5 and your analyzer runs under breakpoints. Stop on a rule before it's tried, or on a line inside an `@POST`. Step over, into, out. A rule that *failed* tells you how far it got before it stopped agreeing with the text — usually the whole question you were asking.

🔬 **Every variable, at the moment it matters.** `G()` globals, `L()` locals, `S()` suggested, `X()` context, `N()` matched elements — read exactly where you stopped, *before* the line runs. So what you see is the state that line is about to act on, not its result.

📞 **Step into your functions.** A call stack that names each function and opens at the line its call was written on. Parameters read back as locals; globals update as you step.

🎯 **The nodes the rule is actually about** — labelled `N(1)`, `N(2)`… the same names you'd write in the rule. Not the whole document tree.

⏪ **And you can step BACKWARDS.** A replay mode walks the parse trees a finished run already wrote to disk, so reverse-stepping costs nothing. Walk a node's history in either direction instead of re-running to get back where you were.

🔌 **NLP++ is no longer only a VS Code language.** Every language feature — outline, hover, go-to-definition, rename, diagnostics, formatting and more — now runs in a **Language Server**. Any LSP-speaking editor can use them.

---

One honest note, because it's the part I'd want to read. 📝

The first time this debugger met a real analyzer, **six bugs surfaced within hours** — and every one was *silent*. Nothing threw. The debugger just told a slightly wrong story, which is the worst failure mode a debugging tool has. A single-statement `if` body was stepped straight over. A `while` loop never showed its own test. The call stack said "4 calls deep" and listed nothing.

All fixed. More importantly: that layer had **no tests**, and CI wasn't running the ones that existed. Both fixed too — and the suites were checked the way test suites have to be checked, by **putting each bug back and confirming the tests catch it.**

---

Version 3 made a deterministic engine fast to run and easy to ship. Version 4 makes it **inspectable while it runs.** Deterministic systems have always been auditable in principle. Now they're auditable in practice. ✅

Much of this was built in collaboration with **Claude (Anthropic's Claude Code)** — an LLM helping build the tooling that makes a *deterministic* system auditable in motion. The LLM is the development partner; the runtime stays transparent and reproducible.

You could always read the rules. Now you can watch them run. 👀

👉 Explore it: github.com/VisualText

#NLP #NaturalLanguageProcessing #NLPplus #VisualText #ExplainableAI #GlassBoxAI #DeterministicAI #OpenSource #DeveloperTools #Debugging #LanguageServer #VSCode #AI #Claude
