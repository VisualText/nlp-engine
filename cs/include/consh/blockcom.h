/*******************************************************************************
Copyright (c) 1998-2026 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	BLOCKCOM.H
* FILE:	cs/include/consh/blockcom.h
* CR:		07/31/26 DD.
* SUBJ:	C-style block comments for NLP++'s line-oriented data files.
*
*******************************************************************************/

#ifndef BLOCKCOM_H_
#define BLOCKCOM_H_

#include "consh/libconsh.h"

// Blank out C-style "/* ... */" block comments in a NUL-terminated buffer.
//
// The pass language (.nlp/.pat) gets block comments from the self-hosted RFB
// grammar (data/rfb/spec/retok.nlp + bigtok.nlp). The line-oriented data files
// -- .seq, .kb, .dict, .kbb -- are read by hand-written C lexers instead, and
// this is their shared front end.
//
// Comment characters are overwritten with SPACES rather than deleted, so every
// surviving character keeps its original byte offset. That matters: the .dict
// and .kbb parsers index into the line by offset, .kbb treats leading spaces as
// significant indentation, and error messages report line/column. A line that
// was nothing but a comment becomes all blanks, which every one of these
// parsers already skips.
//
// Semantics match C and the pass language: comments do not nest (the first
// "*/" closes), a "/*" inside a double-quoted string is text, and a "/*" inside
// a line comment is text.
//
// One exception to "blank in place": if anything was blanked, trailing
// whitespace at the END of the buffer is then cut. A line-oriented caller
// passes exactly one line, so this makes "google  /* the word */" behave the
// same as plain "google". It matters -- a .kbb word line with trailing
// whitespace does not parse, so without this a trailing comment would silently
// drop the word's block.
//
//	buf				NUL-terminated buffer, modified in place. May be a single
//						line or a whole file; '\n' ends a line comment either way.
//	inBlock			In/out. True while an unterminated "/*" is still open.
//						Seed it false before the first line of a file and pass the
//						same variable to each subsequent line; pass a fresh false
//						when only single-line comments make sense (see below).
//	lineComment		The file's line-comment char -- '#' for .seq/.dict/.kbb,
//						';' for .kb command files -- or 0 for none. Text from it to
//						end of line is left exactly as-is, since the caller's own
//						lexer already handles it.
//
// Returns true if anything was blanked.
LIBCONSH_API bool strip_block_comments(
	_TCHAR *buf,
	bool &inBlock,
	_TCHAR lineComment
	);

#endif	// BLOCKCOM_H_
