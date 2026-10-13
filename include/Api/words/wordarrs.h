#ifndef NLP_INCLUDE_API_WORDS_WORDARRS_H
#define NLP_INCLUDE_API_WORDS_WORDARRS_H

// Self-contained: _TCHAR comes from my_tchar.h on Linux and macOS, and
// WORDS_API from words.h, whatever the including file has seen so far.
#include "my_tchar.h"
#include "words/words.h"
/*******************************************************************************
All rights reserved.
********************************************************************************
*/

// The English word list behind spellword, spellcorrect, spellcandidates and
// strspellcandidate (#697). It used to be compiled in as word1..word30, 17 MB
// of generated C++; it is now data/words/en-words.txt, read on first use.
//
// Layout, unchanged from the compiled arrays: word_arrays[n] holds every word
// of n bytes in byte order, with a null at [0] and after the last word, and
// word_lengths[n] is how many there are. Callers must call words_load() first.
#define MAXWORDLENGTH	30
extern WORDS_API long word_lengths[];
extern WORDS_API _TCHAR **word_arrays[];

// Where the word list lives. The runtime manager sets it from the engine's
// data folder (<data>/words/en-words.txt).
WORDS_API void words_set_path(const _TCHAR *path);

// Load the word list once. Returns false if the file is missing or unreadable;
// the arrays then stay empty, so every spelling lookup simply finds nothing.
WORDS_API bool words_load();

#endif
