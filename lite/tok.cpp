/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	TOK.CPP
* FILE:	lite\tok.cpp
* CR:		10/09/98 AM.
* SUBJ:	Tokenization.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"				// 07/07/03 AM.
#include "tree.h"				// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"				// 02/26/01 AM.
#include "io.h"
#include "htab.h"
#include "node.h"	// 07/07/03 AM.
#include "parse.h"
#include "ana.h"
#include "Eana.h"				// 02/26/01 AM.
#include "nlp.h"				// 06/25/03 AM.

#include "prim/unicu.h"
using namespace unicu;

#ifdef UNICODE
#ifdef LINUX
#include "unicode/utypes.h"	// 03/03/05 AM.
#include "unicode/uchar.h"
#else
#include "utypes.h"	// 03/03/05 AM.
#include "uchar.h"
#endif
#endif

#include "tok.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("tok");

/********************************************
* FN:		Special Functions for Tok class.
* CR:		10/09/98 AM.
********************************************/

Tok::Tok()			// Constructor
	: Algo(algo_name /*, 0 */)
{
bad_ = false;					// 01/15/99 AM.
separateChars_ = false;
//parse = 0;
}

Tok::Tok(const Tok &orig)			// Copy constructor	// 12/03/98 AM.
{
name = orig.name;
//parse = orig.parse;			// 12/04/98 AM.
debug_ = orig.debug_;
}

#ifdef OLD_
Tok::Tok(			// Constructor that does it all.
	Parse *p
	)
	: Algo(p)
{
//parse = p;

// PERFORM TOKENIZATION.
//Tokenize();
}
#endif

/********************************************
* FN:		Access functions
********************************************/

//Parse		*Tok::getParse() { return parse; }
bool		Tok::getBad()		{return bad_;}				// 01/15/99 AM.


/********************************************
* FN:		Modify functions
********************************************/

//void Tok::setParse(Parse *x) { parse = x; }
void		Tok::setBad(bool x)	{bad_		= x;}			// 01/15/99 AM.

bool		Tok::getSeparateChars()		{return separateChars_;}
void		Tok::setSeparateChars(bool x)	{separateChars_		= x;}

/********************************************
* FN:		DUP
* CR:		12/03/98 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Tok::dup()
{
Tok *ptr;
ptr = new Tok(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}


/********************************************
* FN:		SETUP
* CR:		12/04/98 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Tok::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}


/********************************************
* FN:		EXECUTE
* CR:		10/12/98 AM.
* SUBJ:	Perform the tokenization.
* ASS:	text is in place, and tree should be empty.
* NOTE:	Will make this a virtual function in Pass class.
********************************************/

bool Tok::Execute(Parse *parse, Seqn *seqn)
{
return Tokenize(parse);
}


/********************************************
* FN:		TOKENIZE
* CR:		10/09/98 AM.
* SUBJ:	Perform the tokenization.
* RET:	ok - true if ok, false if failure.
* ASS:	text is in place, and tree should be empty.
********************************************/

bool Tok::Tokenize(Parse *parse)
{
	if (parse->Verbose())
		*gout << _T("[Tokenize:]") << std::endl;

	// Reset.  No bad chars seen yet.
	bad_ = false;					// 01/15/99 AM.

	Htab *htab = parse->htab_;									// 06/25/03 AM.

	_TCHAR *text = parse->getText();
	Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
	long len  = parse->getLength();
	int32_t ulen = parse->getUlength();	// [UNICODE]

	if (tree)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Tokenize: Parse tree exists. Skipping tokenization.]") << std::ends;
		errOut(&gerrStr,false);
		return true;		// Assuming this is ok.							// 01/26/02 AM.
		}
	if (!text)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Tokenize: Given no text.]") << std::ends;
		return errOut(&gerrStr,false);													// 01/26/02 AM.
		}

	_TCHAR *str = _T("_ROOT");				// 11/30/98 AM.
	Sym *sym = htab->hsym(str);
	int32_t ustart = 0;	// [UNICODE]
	int32_t uend = ulen - 1;	// [UNICODE]
	tree = Pn::makeTree(0, len-1, ustart, uend, PNNODE, text, str, sym); // Create parse tree.
	parse->setTree(tree);								// Update global data.

	// NEED TO MAKE THE ROOT NODE UNSEALED!	// 10/11/99 AM.
	Node<Pn> *node = tree->getRoot();			// 10/11/99 AM.
	Pn *pn = node->getData();						// 10/11/99 AM.
	pn->setUnsealed(true);							// 10/11/99 AM.

	//////////////////////////////////////////////////////////
	// Traverse buffer, creating a node for every token found.
	//////////////////////////////////////////////////////////

	int32_t start = 0;			// Count offset of current char.
	Node<Pn> *last = 0;			// Last token node.	// Preemptive init.	// 08/28/11 AM.
	ustart = 0;	// Reset.	// [UNICODE]

	// Bookkeep line numbers for debug.										// 05/17/01 AM.
	long line = 1;

	_TCHAR *buf = text;			// For traversing text.
	StringPiece sp(text);
	const char *s = sp.data();

	// Get first token and attach to tree.
	FirstToken(tree, htab, &buf, s, len, start, ustart, last, line);

	// Continue getting tokens.
	while (start < len)
		{
		if (!NextToken(tree, htab, &buf, s, len, start, ustart, last, line))
			return false;															// 01/26/02 AM.
		}
	if (parse->getEana()->getFlogfiles())						// FIX	// 02/01/00 AM.
		{
		tree->Traverse(tree->getRoot(), *gout);
		}
	return true;																	// 01/26/02 AM.
}


/********************************************
* FN:		FIRSTTOKEN
* CR:		10/09/98 AM.
* SUBJ:	Find and set up first token.
********************************************/

void Tok::FirstToken(
	Tree<Pn> *tree,
	Htab *htab,
	_TCHAR* *buf,
	const char* s,
	int32_t length,	// Length of input text.
	int32_t &start,
	int32_t &ustart,	// [UNICODE]
	Node<Pn>* &last,
	long &line					// Bookkeep line number.				// 05/17/01 AM.
	)
{
	int32_t end = 0;
	enum Pntype typ;
	enum TokWhite white;

	int32_t uend = 0;	// [UNICODE]
	int32_t ulen = 0;	// [UNICODE]
	ustart = 0;	// [UNICODE]

	// Get first token information.
	nextTok(s, start, end, ulen, length, typ, white);
	int len = end-start+1;
	uend = ustart + ulen - 1;	// [UNICODE]

	/* Attach first node. */
	Sym *sym;
	_TCHAR *str;
	sym = internTok(*buf, len, htab);
	str = sym->getStr();
	last = Pn::makeTnode(start, end, ustart, uend, typ, *buf, str, sym, line);												// 05/17/01 AM.
	tree->firstNode(*last);	// Attach first node to tree.
	if (white == TKNL) {		// First token was a newline!						// 05/17/01 AM.
		++line;																		// 05/17/01 AM.
	}

	/* UP */
	start = ++end;	// Continue tokenizing from next char.
	ustart = ++uend;	// [UNICODE]
	*buf += len;
}


/********************************************
* FN:		NEXTTOKEN
* CR:		10/09/98 AM.
* SUBJ:	Find and set up next token in parse tree.
* RET:	ok - true if ok, false if failed.
********************************************/

bool Tok::NextToken(
	Tree<Pn> *tree,
	Htab *htab,
	_TCHAR* *buf,
	const char* s,
	int32_t length,
	int32_t &start,
	int32_t &ustart,	// [UNICODE]
	Node<Pn>* &last,
	long &line					// Bookkeep line number.				// 05/17/01 AM.
	)
{
	int32_t end = start;
	enum Pntype typ;
	enum TokWhite white;
	long e = end;


	// Get next token information.
	int32_t ulen = 0;	// [UNICODE]
	nextTok(s, start, end, ulen, length, typ, white);
	int32_t len = end-start+1;
	int32_t uend = ustart + ulen - 1;	// [UNICODE]

	/* Attach next node to list. */
	Sym *sym;
	_TCHAR *str;
	sym = internTok(*buf, end-start+1, htab);
	str = sym->getStr();
	Node<Pn> *node;
	node = Pn::makeTnode(start, end, ustart, uend, typ, *buf, str, sym, line);												// 05/17/01 AM.

	// CHECK NODE OVERFLOW.														// 01/24/02 AM.
	if (!node)																		// 01/24/02 AM.
		{
		std::_t_strstream gerrStr;						// 01/24/02 AM.
		gerrStr << _T("[Node overflow at ") << start << _T(" chars, ")		// 01/24/02 AM.
			<< last->getCount() << _T(" nodes.]") << std::ends;					// 01/24/02 AM.
		return errOut(&gerrStr,false,0,0);												// 01/26/02 AM.
		}

	tree->insertRight(*node, *last);
	last = node;		// node is last node of list now.
	if (white == TKNL)																	// 05/17/01 AM.
		++line;																		// 05/17/01 AM.

	/* UP */
	start = ++end;	// Continue tokenizing from next char.
	ustart = ++uend;	// [UNICODE]
	*buf += len;
	return true;													// 01/26/02 AM.
}


/********************************************
* FN:		NEXTTOK
* CR:		10/09/98 AM.
* SUBJ:	Get the data for next token.
* OPT:	Could make a big switch statement.
* UNI:	Not worrying about Unicode.
********************************************/

void Tok::nextTok(
	const char *s,		// Start char of token.
	int32_t start,		// Start offset of token.
	int32_t &end,		// End offset of token.
	int32_t &ulen,		// Number of chars in token.	// [UNICODE]
	int32_t length,		// Length of input text.
	enum Pntype &typ,	// Type of token.
	enum TokWhite &white		// White type
	)
{
	ulen = 0;	// [UNICODE]
	end = start;
	typ = PNALPHA;
	white = TKNOT;
	int32_t lastEnd = end;
	int32_t len = 0;

	UChar32 c;
	U8_NEXT(s, end, length, c);

	if (c) {
		if (unicu::isEmoji(c)) {
			typ = PNEMOJI;
			++ulen;
			int32_t saveEnd = end;
			U8_NEXT(s, end, length, c);
			if (unicu::isEmojiVariation(c) || unicu::isEmojiJoiner(c)) {
				bool joining = false;
				while (c && (unicu::isEmojiVariation(c) || unicu::isEmojiJoiner(c) || joining)) {
					joining = unicu::isEmojiJoiner(c);
					lastEnd = end;
					U8_NEXT(s, end, length, c);
					++ulen;
				}
				end -= end - lastEnd;
			} else {
				end = saveEnd;
			}
		}
		else if (unicu::isChinese(c)) {
			typ = PNALPHA;
			++ulen;
		}
		else if (unicu::isDigit(c)) {
			if (getSeparateChars()) {
				++ulen;
			} else {
				while (c && unicu::isDigit(c) && !unicu::isSingle(c)) {
					lastEnd = end;
					U8_NEXT(s, end, length, c);
					++ulen;
				}
				end -= end - lastEnd;				
			}
			typ = PNNUM;
		}
		else if (unicu::isPunct(c)) {
			typ = PNPUNCT;
			++ulen;
		}
		else if (unicu::isAlphabetic(c)) {
			if (getSeparateChars()) {
				++ulen;
			} else {
				while (c && (unicu::isAlphabetic(c) || unicu::isGlyph(c)) && !unicu::isSingle(c)) {
					lastEnd = end;
					U8_NEXT(s, end, length, c);
					++ulen;
				}
				end -= end - lastEnd;
			}
		}
		else if (unicu::isWhiteSpace(c)) {
			if (c == '\n')
				white = TKNL;
			else if (c == '\t')
				white = TKTAB;
			typ = PNWHITE;
			++ulen;
		}
		else {
			typ = PNCTRL;
			++ulen;
		}
		end--;
	}
	else {
		int whoops = 1;
	}
}

/********************************************
* FN:		INTERNTOK
* CR:		10/28/98 AM.
* SUBJ:	Internalize the token.
* NOTE:	Need to lookup/place one copy of the token in a hash table.
*			(If alphabetic).  This is a placeholder for string and hash
*			table implementation in the future.
********************************************/

Sym *Tok::internTok(
	_TCHAR *str,				// Ptr to string in a buffer.
	long len,				// Length of string within buffer.
	Htab *htab				// Hashed symbol table.		// 11/19/98 AM.
	)
{
if (empty(str) || len <= 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[internTok: Given bad string or length.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// If token too long, truncate.	// FIX.	// 08/06/06 AM.
if (len >= MAXSTR)					// FIX.	// 08/06/06 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern Token: Too long -- truncating.]") << std::ends;
	errOut(&gerrStr,false);
	len = MAXSTR - 1;	// Recover.	// FIX.	// 08/06/06 AM.
	}

// 01/26/99 AM. Building lowercase variant of sym here also.
Sym *sym;
_TCHAR *lcstr = 0;
if (!(sym = htab->hsym_kb(str, len,/*UP*/lcstr)))			// 01/26/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern Token: Failed.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

return sym;								// 11/19/98 AM.
}

