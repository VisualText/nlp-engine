/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
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

	//Parse *getParse();
	//void   setParse(Parse *);

	virtual Algo &dup();					// 12/03/98 AM.
	virtual void setup(_TCHAR *s_data);					// 12/04/98 AM.
	bool Execute(Parse *, Seqn *);	// Perform the tokenization.
	bool Tokenize(Parse *);	// Perform the tokenization.
	void FirstToken(Tree<Pn> *tree, Htab *htab, /*DU*/ _TCHAR* *buf,
										long &start, Node<Pn>* &last,
										long &line	// Line number.		// 05/17/01 AM.
										);

	bool NextToken(
			Tree<Pn> *tree,
			Htab *htab,
			/*DU*/
			_TCHAR* *buf,
			long &start,
			Node<Pn>* &last,
			long &line			// Bookkeep line number.				// 05/17/01 AM.
			);

	void nextTok(
			_TCHAR *buf,		// Start char of token.
			long start,		// Start offset of token.
			/*UP*/
			long &end,		// End offset of token.
			_TCHAR* &ptr,		// End char of token.
			enum Pntype &typ,	// Token type.
			bool &lineflag		// Flag new line number.				// 05/17/01 AM.
			);

	Sym *internTok(
		_TCHAR *str,				// Ptr to string in a buffer.
		long len,				// Length of string within buffer.
		Htab *htab				// Hashed symbol table.		// 11/19/98 AM.
		);

private:
	//Parse *parse;			// The analyze instance that we're tokenizing for.
	bool bad_;					// If unknown char seen.	// 01/15/99 AM.
};


#endif
