/*******************************************************************************
Copyright (c) 2011 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DICTTOK.CPP
* FILE:	lite\tok.cpp
* CR:		07/29/11 AM.
* SUBJ:	Tokenization.
* NOTE:	(1) Copy the TOK.cpp pass.
*			(2) Clean it up to look more like the CMLTOK pass.
*			(3) Save that off as something like SIMPLETOK, to serve as a template.
*			(4) Include the "Unicode issues" in this one.
*			Because CMLTOK is nicely structured for building complex tokenizers and
*			parse trees. (eg, with pages, lines, blobs of tokens, and so on.)
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "lite/global.h"
#include "u_out.h"		// 01/19/06 AM.
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "htab.h"
#include "kb.h"
//#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"				// 07/07/03 AM.
#include "tree.h"				// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"				// 02/26/01 AM.
#include "io.h"
#include "node.h"	// 07/07/03 AM.
#include "parse.h"
#include "ana.h"
#include "Eana.h"				// 02/26/01 AM.
#include "nlp.h"				// 06/25/03 AM.
#include "ivar.h"

#include "pn.h"
#include "prim/libprim.h"
#include "kbm/libkbm.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"

#ifdef UNICODE
#include "utypes.h"	// 03/03/05 AM.
#include "uchar.h"
#endif

#include "dicttok.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("dicttok");

/********************************************
* FN:		Special Functions for DICTTok class.
* CR:		10/09/98 AM.
********************************************/

DICTTok::DICTTok()			// Constructor
	: Algo(algo_name /*, 0 */)
{
bad_ = false;					// 01/15/99 AM.
zapwhite_ = false;			// 08/16/11 AM.
//parse = 0;
lines_ = tabs_ = 0;	// 08/16/11 AM.
tottabs_ = 0;
totlines_ = 1;	// Every text has minimum 1 line.
totlowers_ = totcaps_ = totuppers_ = totnums_ = 0;
token_ = new Tok();
}

DICTTok::DICTTok(const DICTTok &orig)			// Copy constructor	// 12/03/98 AM.
{
name = orig.name;
//parse = orig.parse;			// 12/04/98 AM.
debug_ = orig.debug_;
}

#ifdef OLD_
DICTTok::DICTTok(			// Constructor that does it all.
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

//Parse		*DICTTok::getParse() { return parse; }
bool		DICTTok::getBad()		{return bad_;}				// 01/15/99 AM.
bool		DICTTok::getZapwhite()		{return zapwhite_;}		// 08/16/11 AM.


/********************************************
* FN:		Modify functions
********************************************/

//void DICTTok::setParse(Parse *x) { parse = x; }
void		DICTTok::setBad(bool x)	{bad_		= x;}			// 01/15/99 AM.
void		DICTTok::setZapwhite(bool x)	{zapwhite_		= x;}			// 08/16/11 AM. AM.

/********************************************
* FN:		DUP
* CR:		12/03/98 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &DICTTok::dup()
{
DICTTok *ptr;
ptr = new DICTTok(*this);					// Copy constructor.
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

void DICTTok::setup(_TCHAR *s_data)
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

bool DICTTok::Execute(Parse *parse, Seqn *seqn)
{
// Initialize the tokenizer.
initTok(parse);

// Ignore the pass data.
Tokenize(parse);

return finTok();	// Final cleanups for pass.
}


/********************************************
* FN:		TOKENIZE
* CR:		10/09/98 AM.
* SUBJ:	Perform the tokenization.
* RET:	ok - true if ok, false if failure.
* ASS:	text is in place, and tree should be empty.
********************************************/

bool DICTTok::Tokenize(Parse *parse)
{

//////////////////////////////////////////////////////////
// Traverse buffer, creating a node for every token found.
//////////////////////////////////////////////////////////

_TCHAR *buf = text_;			// For traversing text.
int32_t start = 0;			// Count offset of current char.
Node<Pn> *last = 0;			// Last token node.	// FIX. ZERO INIT.	// 08/28/11 AM.
StringPiece sp(text_);
const char *s = sp.data();

int32_t ustart = 0;	// [UNICODE]

prevwh_ = false;	// No whitespace before first token.	// 08/16/11 AM.

// Bookkeep line numbers for debug.										// 05/17/01 AM.
long line = 1;
long len  = parse->getLength();

// Get first token and attach to tree.
FirstToken(tree_, htab_, &buf, s, len, start, ustart, last, line);

// Continue getting tokens.
while (*buf)
	{
	if (!NextToken(tree_, htab_, &buf, s, len, start, ustart, last, line))
		return false;															// 01/26/02 AM.
	}

// Match dict file
ApplyDictFiles();

return true;																	// 01/26/02 AM.
}

bool DICTTok::ApplyDictFiles() {
	Node<Pn> *root = tree_->getRoot();
	Node<Pn> *node = root->pDown;
	Node<Pn> *last = NULL;
	Node<Pn> *curr = NULL;
	CONCEPT *con = NULL;
	_TCHAR *suggested, *str, *lcstr;
	bool reduceIt = false;

	while (node) {
		lcstr = node->data.getName();
		con = cg_->findWordConcept(lcstr);
		reduceIt = findAttrs(node, con, lcstr, lcstr, false);

		if (reduceIt) {
			PTR *phrases = (PTR *)cg_->findVals(con, _T("phrase"));
			PTR *val = phrases;

			// Single word to be reduced
			if (!phrases) {
				VAL *vals = cg_->findVals(con, _T("s"));
				_TCHAR suggName[MAXSTR];
				if (vals) {
					suggested = popsval(vals);
					sprintf(suggName,_T("_%s"),suggested);
				} else {
					return NULL;
				}

				Pn *pn = node->getData();
				long start = pn->getStart();
				long end = pn->getEnd();
				long ustart = pn->getUstart();
				long uend = pn->getUend();
				long line = pn->getLine();
				Sym *sym = internTok(suggName, _tcslen(suggName), htab_, lcstr);
				str = sym->getStr();
				Node<Pn>* suggestedN = Pn::makeTnode(start, end, ustart, uend, PNNODE, suggName, str, sym, line);
				findAttrs(suggestedN, con, str, lcstr, true);
				if (last)
					tree_->insertRight(*suggestedN,*last);
				else if (root_)
					tree_->insertDown(*suggestedN,*root_);
				tree_->insertDown(*node,*suggestedN);
				suggestedN->getData()->setText(pn->getText());
				return suggestedN;
			}

			// More than one word to be reduced
			CONCEPT *valCon = NULL;
			bool checkForward = false;
			Node<Pn> *parentN = node;
			std::string sp = " ";
			CON *cc = NULL;

			int p = 1;
			while (phrases && parentN) {
				if (phrases->kind == pCON) {
					valCon = phrases->v.vcon;
					// _TCHAR buf[PATHSIZ];
					// cg_->conceptPath(valCon,buf);

					// CON *c = (CON *)valCon;
					VAL *vals = cg_->findVals(valCon, _T("s"));
					_TCHAR suggName[MAXSTR];
					if (vals) {
						suggested = popsval(vals);
						sprintf(suggName,_T("_%s"),suggested);
					}
					if (cg_->Down(valCon) && !checkForward) {
						checkForward = true;
						if (cg_->Down(valCon))
							cc = (CON *)cg_->Down(valCon);
						Node<Pn> *downMatch = MatchForward(valCon,parentN);
						if (downMatch) {
							curr = node;
							node = downMatch;
						} else
							curr = NULL;
						continue;
					} else {
						// Work up the concept hier and match node names
						CONCEPT *up = cg_->Up(valCon);
						bool matchedPhrase = true;
						_TCHAR conName[MAXSTR];
						std::string text = lcstr;
						parentN = parentN->pLeft;

						while (up) {
							cg_->conceptName(up, conName);
							if (!parentN) {
								matchedPhrase = false;
								break;
							}
							Pn *parentPN = &(parentN->data);
							_TCHAR *pnName = parentPN->getName();
							text = pnName + sp + text;
							if (parentPN->getType() == PNWHITE) {
								parentN = parentN->pLeft;
							}
							else if (_tcsicmp(conName, pnName)) {
								matchedPhrase = false;
								break;
							} else if (cg_->findVals(up, _T("top"))) {
								break;
							} else {
								up = cg_->Up(up);
								parentN = parentN->pLeft;
							}
						}

						if (checkForward && !matchedPhrase) {
							if (curr)
								node = curr;
							checkForward = false;
						}

						if (matchedPhrase) {
							Pn *pn = node->getData();
							Pn *ppn = parentN->getData();
							long start = ppn->getStart();
							long end = pn->getEnd();
							long ustart = ppn->getUstart();
							long uend = pn->getUend();
							long line = pn->getLine();
							Sym *sym = internTok(suggName, _tcslen(suggName), htab_, lcstr);
							str = sym->getStr();
							Node<Pn>* suggestedN = Pn::makeTnode(start, end, ustart, uend, PNNODE, suggName, str, sym, line);
							Node<Pn>* lefty  = parentN->Left();
							if (lefty) {
								suggestedN->setLeft(lefty);
								lefty->setRight(suggestedN);
							} else {
								tree_->insertDown(*suggestedN,*root_);
							}
							suggestedN->setRight(node->pRight);
							if (node->pRight)
								node->pRight->setLeft(suggestedN);
							suggestedN->setDown(parentN);
							parentN->setUp(suggestedN);
							parentN->setLeft(0);
							node->setRight(0);

							// Set text to be the entire text of the phrase
							_TCHAR txt[MAXPATH];
							sprintf(txt,_T("%s"),text.c_str());
							sym = internTok(txt, _tcslen(txt), htab_, lcstr);
							str = sym->getStr();
							suggestedN->getData()->setText(str);

							findAttrs(suggestedN, valCon, str, lcstr, true);
							node = suggestedN;
							break;
						}
					}
				}
				checkForward = false;
				parentN = node;
				phrases = phrases->next;
				p++;
			}
		}
		last = node;
		if (node)
			node = node->pRight;
	}

	return true;
}


Node<Pn>* DICTTok::MatchForward(CONCEPT *con, Node<Pn>*parentN) {
	_TCHAR conName[MAXSTR];
	parentN = parentN->pRight;
	CONCEPT *next = cg_->Down(con);
	CON *c = (CON *)next;

	while (next && parentN) {
		cg_->conceptName(next, conName);
		if (!parentN) {
			break;
		}
		Pn *parentPN = &(parentN->data);
		_TCHAR *pnName = parentPN->getName();
		// text = pnName + sp + text;
		if (parentPN->getType() == PNWHITE) {
			parentN = parentN->pLeft;
		}
		else if (_tcsicmp(conName, pnName)) {
			next = cg_->Next(next);
		} else if (!cg_->Down(next)) {
			return parentN;
		} else {
			Node<Pn> *match = MatchForward(next,parentN);
			if (match)
				return match;
			next = cg_->Next(next);
			parentN = parentN->pRight;
		}
	}
	return NULL;
}


/********************************************
* FN:		INITTOK
* CR:		07/31/11 AM.
* SUBJ:	Initialize tokenizer.
********************************************/

bool DICTTok::initTok(Parse *parse)
{
if (!parse)
	return false;

if (parse->Verbose())
	*gout << _T("[DICTTok:]") << std::endl;

// Need to get the current KB.
cg_ = parse->getAna()->getCG();
//CONCEPT *root = cg_->findRoot();


text_ = parse->text;

if (!parse->text)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[DICTTok: Given no text.]") << std::ends;
	return errOut(&gerrStr,false);
	}

tree_ = (Tree<Pn> *)parse->getTree();

if (tree_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[DICTTok: Parse tree exists. Ignoring tokenization pass.]") << std::ends;
	errOut(&gerrStr,false);
	return true;
	}

//////////////////////

// Reset.  No bad chars seen yet.
bad_ = false;
// zapwhite_	// Comes from caller of the tokenizer, leave it be.
lines_ = tabs_ = 0;
tottabs_ = 0;
totlines_ = 1;	// Every text has minimum 1 line.
totlowers_ = totcaps_ = totuppers_ = totnums_ = 0;

// Track line numbers in input text, for debug messages, etc.
lineno_ = 1;

htab_ = parse->htab_;
parse_ = parse;

// Initialize parse tree levels.
root_ = 
// page_ = line_ = word_ = 
tok_ = 0;
//firsttok_ = 0;

fmpos_ = 0;
fmupos_ = 0;	// [UNICODE]	// 06/15/22 AM.

fmptr_ = parse->text;

// The allocated input buffer length.
// Note that we'll be using less of it after removing information text.
long len  = parse->length;
long ulen = parse->ulength;	// [UNICODE]	// 06/15/22 AM.

// CREATE PARSE TREE ROOT.
_TCHAR *str = _T("_ROOT");
Sym *sym = htab_->hsym(str);
tree_ = Pn::makeTree(0, len-1, 0, ulen-1, PNNODE, fmptr_, str, sym); // Create parse tree.
parse->setTree(tree_);								// Update global data.

if (!tree_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[DICTTok: Could not create parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

root_ = tree_->getRoot();	// FETCH PARSE TREE ROOT.

if (!root_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[DICTTok: No parse tree root.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// NEED TO MAKE THE ROOT NODE UNSEALED!
Pn *pn = root_->getData();
pn->setUnsealed(true);

return true;
}


/********************************************
* FN:		FINTOK
* CR:		07/31/11 AM.
* SUBJ:	Final processing of tokenizer pass.
********************************************/

bool DICTTok::finTok()
{
if (!parse_ || !tree_ || !root_)
	return false;

// Place totals onto parse tree root.
replaceNum(root_,_T("TOT LINES"),totlines_);
replaceNum(root_,_T("TOT TABS"),tottabs_);
replaceNum(root_,_T("TOT LOWERS"),totlowers_);
replaceNum(root_,_T("TOT CAPS"),totcaps_);
replaceNum(root_,_T("TOT UPPERS"),totuppers_);
replaceNum(root_,_T("TOT NUMS"),totnums_);

// MOVED HERE FROM TOKENIZE fn.	// 08/18/11 AM.
//if (parse_->Verbose())											// FIX	// 02/01/00 AM.
if (parse_->getEana()->getFlogfiles())						// FIX	// 02/01/00 AM.
	{
	//*gout << "[Tokenize: Dumping parse tree.]" << std::endl;
	tree_->Traverse(root_, *gout);
	}

return true;
}

/********************************************
* FN:		FIRSTTOKEN
* CR:		10/09/98 AM.
* SUBJ:	Find and set up first token.
********************************************/

void DICTTok::FirstToken(
	Tree<Pn> *tree,
	Htab *htab,
	_TCHAR* *buf,
	const char* s,
	int32_t length,
	int32_t &start,
	int32_t &ustart,
	Node<Pn>* &last,
	long &line					// Bookkeep line number.				// 05/17/01 AM.
	)
{
int32_t end;
enum Pntype typ;
bool lineflag = false;														// 05/17/01 AM.

ustart = 0;	// [UNICODE]
int32_t uend = 0;	// [UNICODE]
int32_t ulen = 0;	// [UNICODE]

// Get first token information.
token_->nextTok(s, start, end, ulen, length, typ, lineflag);
int len = end-start+1;
uend = ustart + ulen - 1;	// [UNICODE]

/* Attach first node. */
Sym *sym;
_TCHAR *str;
_TCHAR *lcstr;

// If skipping whitespace!	// 08/16/11 AM.
if (zapwhite_ && typ == PNWHITE)	// 08/16/11 AM.
	{
	prevwh_ = true;	// 08/16/11 AM.
	}
else
	{
sym = internTok(*buf, end-start+1, htab,/*UP*/lcstr);
str = sym->getStr();
last = Pn::makeTnode(start, end, ustart, uend, typ, *buf, str, sym,				// 10/09/99 AM.
							line);												// 05/17/01 AM.

// Lookup, add attrs, reduce, attach to tree.	// 07/31/11 AM
last = handleTok(last,0,typ,str,lcstr,htab);	// 07/31/11 AM.
//tree->firstNode(*last);	// Attach first node to tree.

	}	// END else (not zapwhite or not whitespace)

if (lineflag)		// First token was a newline!						// 05/17/01 AM.
	++line;																		// 05/17/01 AM.

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

bool DICTTok::NextToken(
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
int32_t end;
enum Pntype typ;


// Get next token information.
bool lineflag = false;														// 05/17/01 AM.
int32_t ulen = 0;	// [UNICODE]
token_->nextTok(s, start, end, ulen, length, typ, lineflag);
int32_t len = end-start+1;													// 05/17/01 AM.
int32_t uend = ustart + ulen - 1;	// [UNICODE]

/* Attach next node to list. */
Sym *sym;
_TCHAR *str;
_TCHAR *lcstr;

// If skipping whitespace!	// 08/16/11 AM.
if (zapwhite_ && typ == PNWHITE)	// 08/16/11 AM.
	{
	prevwh_ = true;	// 08/16/11 AM.
	}
else
	{
	sym = internTok(*buf, end-start+1, htab,/*UP*/lcstr);
	str = sym->getStr();
	Node<Pn> *node;
	//node = Pn::makeNode(start, end, typ, *buf, str, sym);			// 10/09/99 AM.
	node = Pn::makeTnode(start, end, ustart, uend, typ, *buf, str, sym,				// 10/09/99 AM.
								line);												// 05/17/01 AM.

	// CHECK NODE OVERFLOW.														// 01/24/02 AM.
	if (!node)																		// 01/24/02 AM.
		{
		std::_t_strstream gerrStr;						// 01/24/02 AM.
		gerrStr << _T("[Node overflow at ") << start << _T(" chars, ")		// 01/24/02 AM.
			<< last->getCount() << _T(" nodes.]") << std::ends;					// 01/24/02 AM.
		return errOut(&gerrStr,false,0,0);												// 01/26/02 AM.
		}

	// Lookup, add attrs, reduce, attach to tree.	// 07/31/11 AM
	last = handleTok(node,last,typ,str,lcstr,htab);

	}	// END else (not zapwhite or not whitespace)

if (lineflag)																	// 05/17/01 AM.
	++line;																		// 05/17/01 AM.

/* UP */
start = ++end;	// Continue tokenizing from next char.
ustart = ++uend;	// [UNICODE]
*buf += len;
return true;																	// 01/26/02 AM.
}


/********************************************
* FN:		INTERNTOK
* CR:		10/28/98 AM.
* SUBJ:	Internalize the token.
* NOTE:	Need to lookup/place one copy of the token in a hash table.
*			(If alphabetic).  This is a placeholder for string and hash
*			table implementation in the future.
********************************************/

Sym *DICTTok::internTok(
	_TCHAR *str,				// Ptr to string in a buffer.
	long len,				// Length of string within buffer.
	Htab *htab,				// Hashed symbol table.		// 11/19/98 AM.
	/*UP*/ _TCHAR* &lcstr
	)
{
lcstr = 0;
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
if (!(sym = htab->hsym_kb(str, len,/*UP*/lcstr)))			// 01/26/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern Token: Failed.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

return sym;								// 11/19/98 AM.
}


/********************************************
* FN:		HANDLETOK
* CR:		07/31/11 AM.
* SUBJ:	Lookup, reduce, attach token to tree.
* NOTE:	Looking up alphabetics in KB DICTIONARY.
********************************************/

Node<Pn> * DICTTok::handleTok(
	Node<Pn> *node,
	Node<Pn> *last,
	enum Pntype typ,
	_TCHAR *str,
	_TCHAR *lcstr,	// Lowercase str
	Htab *htab
	)
{
if (!node)
	return 0;

// If ctrl, get code onto node.
// If alphabetic, lookup in dictionary.
// If unique part of speech, can reduce here as desired.
// Put attributes on.
CONCEPT *con = 0;
CONCEPT *conChild = 0;
bool reduceIt = false;
_TCHAR *suggested = 0;

switch (typ)
	{
	case PNCTRL:
		{
		// Put this as a variable on the node!
		int ansi = (unsigned char) *str;
		replaceNum(node,_T("CTRL"),ansi);
		if (!prevwh_)
			replaceNum(node,_T("NOSP"),1);
		if (lines_)
			replaceNum(node,_T("NL"),lines_);
		if (tabs_)
			replaceNum(node,_T("TABS"),tabs_);
		prevwh_ = false;
		lines_ = tabs_ = 0;
		}
		break;
	case PNALPHA:
	case PNEMOJI:
		if (!prevwh_)
			replaceNum(node,_T("NOSP"),1);
		if (lines_)
			replaceNum(node,_T("NL"),lines_);
		if (tabs_)
			replaceNum(node,_T("TABS"),tabs_);
		prevwh_ = false;
		lines_ = tabs_ = 0;

		if (lcstr && *lcstr) {
			con = cg_->findWordConcept(lcstr);
			reduceIt = findAttrs(node, con, str, lcstr, false);
		}
		break;
	case PNNUM:	// Placed here for easy reference.
		++totnums_;
		// Fall through to PNPUNCT for now.
	case PNPUNCT:
		if (!prevwh_)
			replaceNum(node,_T("NOSP"),1);
		if (lines_)
			replaceNum(node,_T("NL"),lines_);
		if (tabs_)
			replaceNum(node,_T("TABS"),tabs_);
		prevwh_ = false;
		lines_ = tabs_ = 0;
		break;
	case PNWHITE:
		prevwh_ = true;
		break;
	case PNNULL:
	default:
		break;
	}

if (last)
	tree_->insertRight(*node,*last);
else if (root_)	// Sanity check.
	tree_->insertDown(*node,*root_);
return node;
}


inline bool DICTTok::findAttrs(Node<Pn> *node, CONCEPT *con, _TCHAR *str, _TCHAR *lcstr, bool isSuggested) {
	_TCHAR attrName[NAMESIZ];
	_TCHAR bufval[NAMESIZ];

	ATTR *attrs = cg_->findAttrs(con);
	bool reduceIt = false;

	if (!_tcscmp(str,lcstr)) {
		replaceNum(node,_T("lower"),1);	// LOWERCASE.
		++totlowers_;
	}
	else {
		++totcaps_;
		replaceNum(node,_T("cap"),1);
		// CHECK UPPERCASE HERE.
		_TCHAR ucstr[MAXSTR];
		str_to_upper(str, ucstr);
		if (!_tcscmp(str,ucstr)) {
			++totuppers_;
			replaceNum(node,_T("upper"),1);
		}
	}	

	while (attrs) {
		cg_->attrName(attrs, attrName, NAMESIZ);
		
		if (!_tcscmp(_T("phrase"),attrName) || !_tcscmp(_T("s"),attrName)) {
			reduceIt = true;

		} else if (!_tcscmp(_T("pos"),attrName)) {
			VAL *vals = cg_->findVals(con, _T("pos"));
			_TCHAR *val;
			int pos_num = 0;	// Count parts of speech.
			while (vals) {
				val = popsval(vals);
				// Some kb editing.
				if (!_tcscmp(_T("adjective"),val))
					val = _T("adj");
				else if (!_tcscmp(_T("adverb"),val))
					val = _T("adv");
				else if (!_tcscmp(_T("pronoun"),val))
					val = _T("pro");
				else if (!_tcscmp(_T("conjunction"),val))
					val = _T("conj");
				// TODO: Look for COLON as first char, copy attr and VALUE.
				replaceNum(node,val,1);
				++pos_num;
				vals = cg_->nextVal(vals);
			}
			if (pos_num) replaceNum(node,_T("pos num"),pos_num);
		}

		VAL *vals = cg_->findVals(con, attrName);
		_TCHAR *strattr, *strval;
		parse_->internStr(attrName, strattr);

		if (_tcscmp(_T("pos"),strattr) && _tcscmp(_T("s"),strattr)) {
			// This needs to loop if there is more than one value for the same attribute
			if (cg_->isValStr(vals)) {
				cg_->popSval(vals,bufval);
				parse_->internStr(bufval, strval);
				replaceStr(node,strattr,strval);
			} else if (cg_->isValNum(vals)) {
				long num = 0L;
				cg_->popVal(vals,num);
				replaceNum(node,strattr,num);	
			} else
				cg_->nextVal(vals);
		}
		cg_->popAttr(attrs);
	}
	return reduceIt;
}


/********************************************
* FN:		REPLACENUM
* CR:		08/21/08 AM.
* SUBJ:	Add/replace numeric var in node.
* RET:	true if ok.
* NOTE:	Shouldn't need to worry about interning name.
********************************************/

inline bool DICTTok::replaceNum(
	Node<Pn> *node,
	_TCHAR *name,	// variable name.
	long val
	)
{
if (!node)
	return false;

Pn *pn = node->getData();
return Ivar::nodeReplaceval(pn, name, val);
}

/********************************************
* FN:		REPLACESTR
* CR:		08/30/08 AM.
* SUBJ:	Add/replace string var in node.
* RET:	true if ok.
* NOTE:	Shouldn't need to worry about interning name.
********************************************/

inline bool DICTTok::replaceStr(
	Node<Pn> *node,
	_TCHAR *name,	// variable name.
	_TCHAR *val
	)
{
if (!node)
	return false;

Pn *pn = node->getData();
return Ivar::nodeReplaceval(pn, name, val);
}


/********************************************
* FN:		POPSVAL
* CR:		08/01/11 AM.
* SUBJ:	Pop string attr value from list. KB convenience.
* RET:	Interned string value.
********************************************/

inline _TCHAR *DICTTok::popsval(
	VAL *val
	)
{

_TCHAR buf[MAXSTR];
cg_->popSval(val, /*UP*/ buf);
if (!buf[0])		
	return 0;

_TCHAR *str;
parse_->internStr(buf, /*UP*/ str);
return str;
}