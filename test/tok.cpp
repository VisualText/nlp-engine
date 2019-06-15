/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
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

#ifdef UNICODE
#include "unicode/utypes.h"	// 03/03/05 AM.
#include "unicode/uchar.h"
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
// Ignore the pass data.
return Tokenize(parse);														// 01/26/02 AM.
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
	*gout << _T("[Tokenize:]") << endl;

// Reset.  No bad chars seen yet.
bad_ = false;					// 01/15/99 AM.

#ifdef OLD_030625_
Ana *ana;
Htab *htab;						// Get the hash table.	// 11/19/98 AM.
ana = parse->getAna();
assert(ana);
htab = ana->getHtab();
assert(htab);
#endif

Htab *htab = parse->htab_;									// 06/25/03 AM.

_TCHAR *text = parse->getText();
Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
long len  = parse->getLength();

if (tree)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Tokenize: Parse tree exists. Skipping tokenization.]") << ends;
	errOut(&gerrStr,false);
	return true;		// Assuming this is ok.							// 01/26/02 AM.
	}
if (!text)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Tokenize: Given no text.]") << ends;
	return errOut(&gerrStr,false);													// 01/26/02 AM.
	}

_TCHAR *str = _T("_ROOT");				// 11/30/98 AM.
Sym *sym = htab->hsym(str);
tree = Pn::makeTree(0, len-1, PNNODE, text, str, sym); // Create parse tree.
parse->setTree(tree);								// Update global data.

// NEED TO MAKE THE ROOT NODE UNSEALED!	// 10/11/99 AM.
Node<Pn> *node = tree->getRoot();			// 10/11/99 AM.
Pn *pn = node->getData();						// 10/11/99 AM.
pn->setUnsealed(true);							// 10/11/99 AM.

//////////////////////////////////////////////////////////
// Traverse buffer, creating a node for every token found.
//////////////////////////////////////////////////////////

_TCHAR *buf = text;			// For traversing text.
long start = 0;			// Count offset of current char.
Node<Pn> *last;			// Last token node.


// Bookkeep line numbers for debug.										// 05/17/01 AM.
long line = 1;

// Get first token and attach to tree.
FirstToken(tree, htab, /*DU*/ &buf, start, last,
					line															// 05/17/01 AM.
					);

// Continue getting tokens.
while (*buf)
	{
	if (!NextToken(tree, htab, /*DU*/ &buf, start, last,			// 01/26/02 AM.
					line															// 05/17/01 AM.
					))
		return false;															// 01/26/02 AM.
	}

//if (parse->Verbose())											// FIX	// 02/01/00 AM.
if (parse->getEana()->getFlogfiles())						// FIX	// 02/01/00 AM.
	{
	//*gout << "[Tokenize: Dumping parse tree.]" << endl;
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
	/*DU*/
	_TCHAR* *buf,
	long &start,
	Node<Pn>* &last,
	long &line					// Bookkeep line number.				// 05/17/01 AM.
	)
{
long end;
_TCHAR *ptr;	// Point to end of token.
enum Pntype typ;
bool lineflag = false;														// 05/17/01 AM.

// Get first token information.
nextTok(*buf, start, /*UP*/ end, ptr, typ,
	lineflag		// Flag newline seen										// 05/17/01 AM.
	);

/* Attach first node. */
Sym *sym;
_TCHAR *str;
sym = internTok(*buf, end-start+1, htab);
str = sym->getStr();
//last = Pn::makeNode(start, end, typ, *buf, str, sym);			// 10/09/99 AM.
last = Pn::makeTnode(start, end, typ, *buf, str, sym,				// 10/09/99 AM.
							line);												// 05/17/01 AM.
tree->firstNode(*last);	// Attach first node to tree.
if (lineflag)		// First token was a newline!						// 05/17/01 AM.
	++line;																		// 05/17/01 AM.

/* UP */
start = end + 1;	// Continue tokenizing from next char.
*buf   = ptr + 1;	// Continue tokenizing from next char.
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
	/*DU*/
	_TCHAR* *buf,
	long &start,
	Node<Pn>* &last,
	long &line					// Bookkeep line number.				// 05/17/01 AM.
	)
{
long end;
_TCHAR *ptr;	// Point to end of token.
enum Pntype typ;

// Get next token information.
bool lineflag = false;														// 05/17/01 AM.
nextTok(*buf, start, /*UP*/ end, ptr, typ,
			lineflag);															// 05/17/01 AM.

/* Attach next node to list. */
Sym *sym;
_TCHAR *str;
sym = internTok(*buf, end-start+1, htab);
str = sym->getStr();
Node<Pn> *node;
//node = Pn::makeNode(start, end, typ, *buf, str, sym);			// 10/09/99 AM.
node = Pn::makeTnode(start, end, typ, *buf, str, sym,				// 10/09/99 AM.
							line);												// 05/17/01 AM.

// CHECK NODE OVERFLOW.														// 01/24/02 AM.
if (!node)																		// 01/24/02 AM.
	{
	_t_strstream gerrStr;						// 01/24/02 AM.
	gerrStr << _T("[Node overflow at ") << start << _T(" chars, ")		// 01/24/02 AM.
		<< last->getCount() << _T(" nodes.]") << ends;					// 01/24/02 AM.
	return errOut(&gerrStr,false,0,0);												// 01/26/02 AM.
	}

tree->insertRight(*node, *last);
last = node;		// node is last node of list now.
if (lineflag)																	// 05/17/01 AM.
	++line;																		// 05/17/01 AM.

/* UP */
start = end + 1;	// Continue tokenizing from next char.
*buf  = ptr + 1;	// Continue tokenizing from next char.
return true;																	// 01/26/02 AM.
}


/********************************************
* FN:		NEXTTOK
* CR:		10/09/98 AM.
* SUBJ:	Get the data for next token.
* OPT:	Could make a big switch statement.
* UNI:	Not worrying about Unicode.
********************************************/

void Tok::nextTok(
	_TCHAR *buf,		// Start char of token.
	long start,		// Start offset of token.
	/*UP*/
	long &end,		// End offset of token.
	_TCHAR* &ptr,		// End char of token.
	enum Pntype &typ,	// Type of token.
	bool &lineflag		// Flag new line number.						// 05/17/01 AM.
	)
{
end = start;
ptr = buf;

//if (*ptr < 0)		// Non-ASCII char.	// 07/28/99 AM.	// 09/22/99 AM.
//	{
	// Fall through.
//	}

if (alphabetic(*ptr))						// 09/22/99 AM.
	{
#ifdef UNICODE
	 short u_gcb = u_getIntPropertyValue((UChar32)*ptr, UCHAR_GRAPHEME_CLUSTER_BREAK);	// 01/30/06 AM.
	 short new_gcb = 0;

	short u_wb = u_getIntPropertyValue((UChar32)*ptr, UCHAR_WORD_BREAK);			// 01/30/06 AM.
	short new_wb = 0;																				// 01/30/06 AM.

	UErrorCode errorCode;
	short u_script = uscript_getScript(*ptr,&errorCode);								// 01/30/06 AM.
	short new_script = 0;																		// 01/30/06 AM.
#endif

	typ = PNALPHA;
	// 09/22/99 AM. Negative char values now handled in alphabetic() fn.
	++ptr;		// 11/05/99 AM.
	++end;		// 11/05/99 AM.

#ifdef UNICODE
//	while (alphabetic(*ptr) && ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z')))
//   while (alphabetic(*ptr) && ! u_hasBinaryProperty((_TUCHAR)*ptr, UCHAR_WORD_BREAK))
	while (alphabetic_extend(*ptr,u_gcb,u_wb,u_script,/*UP*/new_gcb,new_wb,new_script))
		{
		u_gcb = new_gcb;
		u_wb = new_wb;
		u_script = new_script;
		new_gcb = new_wb = new_script = 0;

		++ptr;
		++end;
		}
#else
	while (alphabetic(*ptr)					// 09/22/99 AM.
			// && *ptr > 0						// 07/28/99 AM.	// 09/22/99 AM.
			 )
		{
		++ptr;
		++end;
		}
#endif

	--end;		// Back up to final alpha char.
	--ptr;
	return;		// 07/28/99 AM.
	}
else if (*ptr < 0)	// Other extended ANSI chars.	// 09/22/99 AM.
	{
	// Fall through.			// 09/22/99 AM.
	}
else if (_istdigit(*ptr))
	{
	typ = PNNUM;
	++ptr;		// 11/05/99 AM.
	++end;		// 11/05/99 AM.
	while (_istdigit(*ptr)
			 && *ptr > 0						// 07/28/99 AM.
			)
		{
		++ptr;
		++end;
		}
	--end;		// Back up to final digit char.
	--ptr;
	return;		// 07/28/99 AM.
	}
else if (_istspace(*ptr))
	{
	// Assuming newline always present at end of line.				// 05/17/01 AM.
	if (*ptr == '\n')															// 05/17/01 AM.
		lineflag = true;	// Flag new line number.					// 05/17/01 AM.
	typ = PNWHITE;
	return;		// 07/28/99 AM.
	}
else if (_istpunct((_TUCHAR)*ptr))
	{
	typ = PNPUNCT;
	return;		// 07/28/99 AM.
	}

// NON ASCII CHAR.			// 07/28/99 AM.

if (!bad_)						// 01/15/99 AM.
	{
	bad_ = true;					// 01/15/99 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[Non-ASCII chars in file.]") << ends;
	errOut(&gerrStr,false);
	}
// Hack. 12/05/98 AM.
//typ = PNPUNCT;
//*ptr = '~';				// 07/28/99 AM. I didn't like @.
typ = PNCTRL;																	// 07/19/00 AM.
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
	_t_strstream gerrStr;
	gerrStr << _T("[internTok: Given bad string or length.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

// If token too long, truncate.	// FIX.	// 08/06/06 AM.
if (len >= MAXSTR)					// FIX.	// 08/06/06 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Intern Token: Too long -- truncating.]") << ends;
	errOut(&gerrStr,false);
	len = MAXSTR - 1;	// Recover.	// FIX.	// 08/06/06 AM.
	}

// 01/26/99 AM. Building lowercase variant of sym here also.
Sym *sym;
//if (!(sym = htab->hsym(str, len)))			// 01/26/99 AM.
if (!(sym = htab->hsym_kb(str, len)))			// 01/26/99 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Intern Token: Failed.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

return sym;								// 11/19/98 AM.
//return sym->getStr();
//return make_str(str, len);		// Make null terminated string.
}
