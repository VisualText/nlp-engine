#include "StdAfx.h"
#include "words/words.h"
#include "words/wordarrs.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <string>
#include <vector>

// The word list is read from data/words/en-words.txt on first use instead of
// being compiled in (#697). See wordarrs.h for the layout callers rely on.

WORDS_API long word_lengths[MAXWORDLENGTH + 2] = {0};
WORDS_API _TCHAR **word_arrays[MAXWORDLENGTH + 2] = {0};

static std::string s_path;
static std::once_flag s_once;
static bool s_loaded = false;
static std::vector<char> s_text;					// The file, NUL-separated.
static std::vector<_TCHAR *> s_slots[MAXWORDLENGTH + 2];	// Backing for word_arrays.

WORDS_API void words_set_path(const _TCHAR *path)
{
	// The first engine to start decides; every engine in the process shares
	// one data folder.
	if (path && *path && s_path.empty())
		s_path = path;
}

static void load()
{
	std::ifstream in(s_path.c_str(), std::ios::in | std::ios::binary);
	if (s_path.empty() || !in)
		{
		std::cerr << "[Spelling word list not found: "
				  << (s_path.empty() ? "(no data folder)" : s_path) << "]" << std::endl;
		return;
		}
	s_text.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	s_text.push_back('\n');

	// One word per line, already grouped by length and in byte order. Turn
	// each line end into a NUL so the words can be used in place. A CRLF
	// checkout leaves a '\r' at each line end, which is dropped too.
	for (int n = 0; n <= MAXWORDLENGTH; ++n)
		s_slots[n].assign(1, (_TCHAR *)0);			// [0] is null, as before.
	char *p = s_text.data();
	char *end = p + s_text.size();
	while (p < end)
		{
		char *eol = (char *)memchr(p, '\n', end - p);
		if (!eol)
			break;
		char *wend = eol;
		if (wend > p && wend[-1] == '\r')
			--wend;
		*wend = '\0';
		size_t len = wend - p;
		if (len >= 1 && len <= MAXWORDLENGTH)
			s_slots[len].push_back((_TCHAR *)p);
		p = eol + 1;
		}
	for (int n = 1; n <= MAXWORDLENGTH; ++n)
		{
		word_lengths[n] = (long)s_slots[n].size() - 1;
		s_slots[n].push_back((_TCHAR *)0);			// Trailing null, as before.
		word_arrays[n] = s_slots[n].data();
		}
	s_loaded = true;
}

WORDS_API bool words_load()
{
	std::call_once(s_once, load);
	return s_loaded;
}
