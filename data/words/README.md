# English word list

`en-words.txt` is the word list behind the NLP++ spelling and stemming
functions: `spellword`, `spellcorrect`, `spellcandidates`, `strspellcandidate`,
and the engine's internal stemmer. It holds 172,325 lowercase English words,
one per line, ASCII only.

The engine reads it the first time one of those functions runs. It looks for
it next to the NLP++ grammar: `<data>/rfb/spec` → `<data>/words/en-words.txt`.
If the file is missing, the engine prints `[Spelling word list not found: ...]`
once and every spelling lookup finds nothing.

Until engine 4.2.12 the same words were compiled into the engine as generated
C++ arrays (`src/words/word1.cpp` … `word30.cpp`); this file was written from
those arrays and gives identical results (nlp-engine#697).

## Editing

Words are looked up by binary search among the words of the same length, so
**words of the same length must stay in byte order** (plain `strcmp` order,
which is what `LC_ALL=C sort` gives). The simplest way to keep that true is
to keep the whole file sorted by length, then by byte order. Words longer
than 30 characters are ignored.
