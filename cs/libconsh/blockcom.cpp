/*******************************************************************************
Copyright (c) 1998-2026 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									BLOCKCOM.CPP
*
* FILE:	cs/libconsh/blockcom.cpp
* SUBJ:	C-style block comments for NLP++'s line-oriented data files.
* CR:		07/31/26 DD.
* NOTE:	See consh/blockcom.h for the contract. Scanning is byte-wise, which
*			is safe for UTF-8: every byte of a multi-byte sequence is >= 0x80,
*			so it can never be mistaken for one of the ASCII delimiters we look
*			for ('/', '*', '"', '\\', '\n' or the line-comment char).
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>
#include <fstream>

#include "u_out.h"
#include "prim/libprim.h"
#include "prim/prim.h"

#include "consh/libconsh.h"
#include "consh/blockcom.h"

LIBCONSH_API bool strip_block_comments(
	_TCHAR *buf,
	bool &inBlock,
	_TCHAR lineComment
	)
{
if (!buf)
	return false;

bool changed = false;
bool inString = false;			// Inside a "..." literal.
_TCHAR *p = buf;

while (*p)
	{
	if (inBlock)
		{
		// Inside a comment: blank everything, including newlines, until the
		// first "*/". Comments do not nest, as in C.
		if (p[0] == '*' && p[1] == '/')
			{
			*p++ = ' ';
			*p++ = ' ';
			inBlock = false;
			}
		else
			*p++ = ' ';
		changed = true;
		continue;
		}

	if (inString)
		{
		// A backslash escapes the next char, so \" stays inside the string.
		if (p[0] == '\\' && p[1])
			p += 2;
		else if (*p == '\n')
			{
			// Unterminated string. Don't let one stray quote swallow the rest
			// of the file -- end it at the line, as these formats all do.
			inString = false;
			++p;
			}
		else if (*p++ == '"')
			inString = false;
		continue;
		}

	if (*p == '"')
		{
		inString = true;
		++p;
		}
	else if (lineComment && *p == lineComment)
		{
		// Leave the line comment alone -- the caller's lexer owns it. Skipping
		// past it is what keeps a "/*" written inside one from opening a block.
		while (*p && *p != '\n')
			++p;
		}
	else if (p[0] == '/' && p[1] == '*')
		{
		*p++ = ' ';
		*p++ = ' ';
		inBlock = true;
		changed = true;
		}
	else
		++p;
	}

// Cut the whitespace a blanked trailing comment left behind. A .kbb word line
// ("  google") does not parse with trailing whitespace, so "  google /* x */"
// has to end up byte-identical to "  google". Only touched buffers are
// trimmed, so a file with no block comments is passed through untouched.
if (changed)
	{
	while (p > buf && _istspace((_TUCHAR)*(p - 1)))
		*--p = '\0';
	}

return changed;
}
