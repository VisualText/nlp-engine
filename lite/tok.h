/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	TOK.H
* FILE:	lite\tok.h
* CR:		10/09/98 AM.
* SUBJ:	Class for tokenization.
* NOTE:	May become a subclass of a general "pass", and then different types
*			of tokenization may be subclasses of a general tokenization class.
*			Virtual classes should enable saying "pass -- go and do your thing".
*
*******************************************************************************/

#ifndef TOK_H_
#define TOK_H_

#include "pn.h"
//#include "tree.h"
#include "lite/algo.h"
#include "htab.h"
#include <unicode/uchar.h>

enum TokWhite
	{
	TKNOT,	// Not white space
	TKNL,	// New line
	TKTAB	// Tab
	};

/********************************************
* CLASS:	TOK
* CR:		10/09/98 AM.
* SUBJ:	Tokenization class.
* NOTE:	Tokenization converts a buffer into a parse tree.
*			Since using Parse instance, should get rid of most of what's
*			here!
********************************************/

class Tok : public Algo
{
public:
	Tok();					// Constructor.
	//Tok(Parse *);			// Constructor that performs the tokenization!
	Tok(const Tok &orig);	// Copy constructor.   // 12/03/98 AM.

	bool getBad();					// 01/15/99 AM.
	void setBad(bool);			// 01/15/99 AM.
	bool getSeparateChars();
	void setSeparateChars(bool);

	//Parse *getParse();
	//void   setParse(Parse *);

	virtual Algo &dup();					// 12/03/98 AM.
	virtual void setup(_TCHAR *s_data);					// 12/04/98 AM.
	bool Execute(Parse *, Seqn *);	// Perform the tokenization.
	bool Tokenize(Parse *);	// Perform the tokenization.
	void FirstToken(Tree<Pn> *tree, Htab *htab, _TCHAR* *buf, const char* s, int32_t length,
										int32_t &start, int32_t &ustart, Node<Pn>* &last,
										long &line	// Line number.		// 05/17/01 AM.
										);

	bool NextToken(
			Tree<Pn> *tree,
			Htab *htab,
			_TCHAR* *buf,
			const char* s,
			int32_t length,
			int32_t &start,
			int32_t &ustart,	// [UNICODE]
			Node<Pn>* &last,
			long &line			// Bookkeep line number.				// 05/17/01 AM.
			);

	void nextTok(
			const char *s,		// Start char of token.
			int32_t start,		// Start offset of token.
			int32_t &end,		// End offset of token.
			int32_t &ulen,		// Unicode chars in token.	// [UNICODE]
			int32_t length,
			enum Pntype &typ,	// Token type.
			TokWhite &white		// White type
			);

	Sym *internTok(
		_TCHAR *str,				// Ptr to string in a buffer.
		long len,				// Length of string within buffer.
		Htab *htab				// Hashed symbol table.		// 11/19/98 AM.
		);

	bool isPunct(UChar32 c);
	bool isSingle(UChar32 c);
	bool isChinese(UChar32 c);
	bool isEmoji(UChar32 c);

private:
	//Parse *parse;			// The analyze instance that we're tokenizing for.
	bool bad_;					// If unknown char seen.	// 01/15/99 AM.
	bool separateChars_;
};


#endif
