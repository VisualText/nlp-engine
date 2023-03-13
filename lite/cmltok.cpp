/*******************************************************************************
Copyright (c) 2008-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CMLTOK.CPP
* FILE:	lite\cmltok.cpp
* CR:		
* SUBJ:	CML Tokenization and Conversion.
* DES:	Handle CML input format, with lines like
*			<Page_1>
*			0|0|0|290|305|87|31|9|After
*			Key = ZONE|LINE|WORD|Left|Top|Width|Height|Conf|TEXT
*			Abbr= Z|L1|W1|L|T|W|H|C|TEXT
*			The Confidence value may or may not be used, and may be a DECIMAL NUMBER.
* ALGO:
*			For each CML line
*				Get lookahead character (line, page).
*				If tag, handle it.
*					Get _PAGE.
*				If text line, handle it.
*					Create _WORD node.
*					Strip off and GATHER text box information.
*					Copy text to a NEW INPUT BUFFER.
*					Create nodes as usual pointing into the new buffer.
*				Else create some _JUNK holder.
*					(If "normal" text, may just want to assume that and call the
*					old tokenizer. Unread first char, etc.)
*				If new page number, handle it.
*				If new line number, handle it.
* NOTES:
*			IGNORING ZONE for now, because we want the new CML processing to do
*			its own zoning and validation of zones.  If text has multiple zones,
*			could put in blank lines, but defer this for now.
*			NUMBERING: CML PAGE NUMBERING IS ONE-BASED.
*				CML ZONE, WORD, AND OFFSET NUMBERS ARE ZERO-BASED.
* NAMES:	W1 is a funny name for word number.  The idea is that the analyzer will also
*			have Wn as the last word in a range of words (W1,Wn).
*
* OPT:	A tricky optimization is to use the given parse text buffer for the
*			newly created text buffer.  (Copy back.)
* [UNICODE]	Till we provide UNICODE input, all existing input samples are ANSI code.	06/18/22 AM.
*			Therefore no reworking of code required yet. Put in hooks anyway.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"
#include "u_out.h"
#include "lite/global.h"
#include "inline.h"
#include "lite/lite.h"
#include "dlist.h"
#include "node.h"
#include "tree.h"
#include "lite/iarg.h"
#include "str.h"
#include "io.h"
#include "htab.h"
#include "node.h"
#include "parse.h"
#include "ana.h"
#include "Eana.h"
#include "nlp.h"
#include "ivar.h"

#ifdef UNICODE
#include "utypes.h"
#include "uchar.h"
#endif

#include "cmltok.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("cmltok");	// 07/28/11 AM.

/********************************************
* FN:		Special Functions for CMLTok class.
* CR:		10/09/98 AM.
********************************************/

CMLTok::CMLTok()			// Constructor
	: Algo(algo_name /*, 0 */)
{
bad_ = false;
//parse = 0;
}

CMLTok::CMLTok(const CMLTok &orig)			// Copy constructor
{
name = orig.name;
//parse = orig.parse;
debug_ = orig.debug_;
}

#ifdef OLD_
CMLTok::CMLTok(			// Constructor that does it all.
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

//Parse		*CMLTok::getParse() { return parse; }
bool		CMLTok::getBad()		{return bad_;}


/********************************************
* FN:		Modify functions
********************************************/

//void CMLTok::setParse(Parse *x) { parse = x; }
void		CMLTok::setBad(bool x)	{bad_		= x;}



/********************************************
* FN:		DUP
* CR:		12/03/98 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &CMLTok::dup()
{
CMLTok *ptr;
ptr = new CMLTok(*this);					// Copy constructor.
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

void CMLTok::setup(_TCHAR *s_data)
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

bool CMLTok::Execute(Parse *parse, Seqn *seqn)
{
// Initialize the CML tokenizer.
initCMLtok(parse);

// Build first page node.
// Build first line node.

bool ok;
// For each line of CML text....
// Check current cml line.
while (*fmptr_)	// While there's input buffer.
	{
	ok = false;
	if (_istdigit(*fmptr_))	// Assume standard CML line.
		ok = getCMLline();
	else if (*fmptr_ == _T('<'))
		ok = getCMLpagetag();
	else if (_istspace(*fmptr_))
		ok = getCMLblank();	// Accept and ignore line starting with whitespace.

	if (!ok)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[CMLTokenize: Unhandled format.]") << std::ends;
		return errOut(&gerrStr,false);
		}
	}
return finCMLtok();	// Final cleanups for pass.
}


/********************************************
* FN:		INITCMLTOK
* CR:		08/19/08 AM.
* SUBJ:	Initialize CML tokenizer.
********************************************/

bool CMLTok::initCMLtok(Parse *parse)
{
if (!parse)
	return false;

if (parse->Verbose())
	*gout << _T("[CMLTokenize:]") << std::endl;

text_ = parse->text;

if (!parse->text)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[CMLTokenize: Given no text.]") << std::ends;
	return errOut(&gerrStr,false);
	}

tree_ = (Tree<Pn> *)parse->getTree();

if (tree_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[CMLTokenize: Parse tree exists. Ignoring CML tokenization pass.]") << std::ends;
	errOut(&gerrStr,false);
	return true;
	}

//////////////////////

// Reset.  No bad chars seen yet.
bad_ = false;

// Track line numbers in input text, for debug messages, etc.
lineno_ = 1;

htab_ = parse->htab_;
parse_ = parse;

// Initialize parse tree levels.
root_ = 
page_ = line_ = word_ = tok_ = 0;
firsttok_ = 0;

fmpos_ = topos_ = 0;
fmupos_ = toupos_ = 0;	// [UNICODE]	// 06/15/22 AM.

// OPT: Use the input text buffer as both the from and the to.
// Copy back into the same buffer.
fmptr_ = toptr_ = parse->text;

P_ = 0;	// No page number yet. (1-based counting).
Z_ = L1_ = W1_ = -1;
prevZ_ = prevL1_ = -1;
lineR_ = gapL_ = gapR_ = 0;
L_ = T_ = W_ = H_ = -1;
C_ = 0;


wlen_ = wbars_ = walphas_ = wnums_ = wpuncts_ = 0;
llen_ = lbars_ = lalphas_ = lnums_ = lpuncts_ = 0;
lwords_ = ltoks_ = luppers_ = lcaps_ = llowers_ = lknowns_ = lunks_ = 0;

// The allocated input buffer length.
// Note that we'll be using less of it after removing CML information text.
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
	gerrStr << _T("[CMLTokenize: Could not create parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

root_ = tree_->getRoot();	// FETCH PARSE TREE ROOT.

if (!root_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[CMLTokenize: No parse tree root.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// NEED TO MAKE THE ROOT NODE UNSEALED!
Pn *pn = root_->getData();
pn->setUnsealed(true);

return true;
}

/********************************************
* FN:		FINCMLTOK
* CR:		08/19/08 AM.
* SUBJ:	Final processing of CML tokenizer pass.
********************************************/

bool CMLTok::finCMLtok()
{
if (!parse_ || !tree_ || !root_)
	return false;

// ADD A NEWLINE TO END THE RETEXT BUFFER.
*toptr_++ = '\n';
++topos_;
++toupos_;	// [UNICODE MOD]	// 06/15/22 AM.

// UPDATE ROOT, LAST LINE AND LAST PAGE.
updateLinenode(true);	// Update information for the current line node.
updatePagenode();			// Update info for the current page node.

// updateRootnode();
Pn *pn;
if (root_)
	{
	pn = root_->getData();
	pn->setStart(0);
	pn->setEnd(topos_);
	pn->setUstart(0);	// [UNICODE]
	pn->setUend(toupos_);	// [UNICODE]
	}

if (parse_->getEana()->getFlogfiles())
	{
	//*gout << "[Tokenize: Dumping parse tree.]" << std::endl;
	tree_->Traverse(root_, *gout);
	}

// If input buffer has been copied over, set a termination.
if (topos_ > 0)
	*toptr_ = '\0';	// Terminate buffer.

Ana *ana = parse_->ana;
if (!ana)
	return true;	// Let it go.
NLP *nlp = ana->getNLP();
if (!nlp)
	return true;	// Let it go.

if (!nlp->getFinteractive())
	return true;	// Done.

// INTERACTIVE VISUALTEXT SESSION.
// Write out the input buffer to _LOG\RETEXT.TXT.
// Mimic inputrangetofile!

#ifdef __linux__
std::_Ios_Openmode modes;
#else
int modes = 0;
#endif

modes |= std::ios::app;

_TCHAR fname[MAXSTR];
_stprintf(fname, _T("%s%cretext.txt"), parse_->getOutdir(),DIR_CH);
std::_t_ostream *ostr = new std::_t_ofstream(TCHAR2CA(fname), modes);

if (!ostr)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[CML Tokenizer: Couldn't create RETEXT.TXT file.]") << std::ends;
	errOut(&gerrStr,false);
	gerrStr << _T("[file=") << str << _T("]") << std::ends;
	errOut(&gerrStr,false);
	return true;	// Let it go.
	}

// Write out buffer here.
_TCHAR *ptr = text_ - 1;	// Minus 1 for convenient looping.
_TCHAR ch;
while ((ch = *++ptr))
	*ostr << ch;
*ostr << std::flush;

// Close the file.
delete ostr;
ostr = 0;

return true;
}


/********************************************
* FN:		GETCMLLINE
* CR:		08/19/08 AM.
* SUBJ:	Handle a standard CML line.
* RET:	ok - true if ok, false if failure.
********************************************/

bool CMLTok::getCMLline()
{

// Track previous values.
prevZ_ = Z_;
prevL1_ = L1_;

lineR_ = (L_ + W_);

// Get CML header info.

// ************ Z|L1|W1|L|T|W|H|C|WORD
if (!getCMLnum(Z_))
	return false;	// Error.
if (!getCMLnum(L1_))
	return false;	// Error.
if (!getCMLnum(W1_))
	return false;	// Error.
if (!getCMLnum(L_))
	return false;	// Error.
if (!getCMLnum(T_))
	return false;	// Error.
if (!getCMLnum(W_))
	return false;	// Error.
if (!getCMLnum(H_))
	return false;	// Error.
// Hopefully, confidence won't be a decimal.
if (!getCMLnum(C_))
	return false;	// Error.

if (!*fmptr_)		// End of buffer.
	return true;	// Assume it's ok.

// Get "normal" tokens comprising WORD.
// Build _line, _word, and token nodes as you go.
if (!getCMLword())
	return true;

return true;
}


/********************************************
* FN:		GETCMLWORD
* CR:		08/20/08 AM.
* SUBJ:	Get the WORD (multiple tokens) from CML line.
* RET:	ok - true if ok, false if failure.
********************************************/

bool CMLTok::getCMLword()
{
// Handle no text on the current line.
// Assume it's ok?
if (*fmptr_ == '\n')
	{
	// Get lookahead.
	++fmptr_;
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	return true;
	}

bool zoneflag = false;
bool lineflag = false;

// HEUR: If multiple zones on a line, SPLIT LINE.
// If new zone, output a newline.
if (prevZ_ != Z_)
	{
	zoneflag = true;

	// ADD A NEWLINE TO THE RETEXT BUFFER.
	if (P_ != 1 || W1_ != 0)	// Not very first word of document
		{
		*toptr_++ = '\n';
		++topos_;
		++toupos_;	// [UNICODE MOD]	// 06/15/22 AM.
		}
	}

// If new line, build node.
if (prevL1_ != L1_)
	{
	lineflag = true;

	// ADD A NEWLINE TO THE RETEXT BUFFER.
	if (P_ != 1 || W1_ != 0)	// Not very first word of document
		{
		*toptr_++ = '\n';
		++topos_;
		++toupos_;	// [UNICODE MOD]	// 06/15/22 AM.
		}
	}

if (lineflag || zoneflag)
	{
	updateLinenode(false);	// Update information for the current line node.
	makeLinenode();			// Create a new line node.
	}
else	// CONTINUE SAME LINE.
	{
	// If latest gap is the largest on line, then update it.
	if ((L_ - lineR_) > (gapR_ - gapL_))
		{
		gapL_ = lineR_;
		gapR_ = L_;
		}
	}

makeWordnode();	// Build new word node.

// ADD VALUES TO LINE NODE.
if (zoneflag)
	replaceNum(line_,_T("NZ"),1);	// NEW ZONE.
if (zoneflag && !lineflag)
	replaceNum(line_,_T("SL"),1);	// SPLIT LINE.

// ADD VALUES TO WORD NODE.
if (zoneflag)
	replaceNum(word_,_T("NZ"),1);	// NEW ZONE.
if (lineflag)
	replaceNum(word_,_T("NL"),1);	// NEW LINE.
if (zoneflag && !lineflag)
	replaceNum(word_,_T("SL"),1);	// SPLIT LINE.

// Get tokens till the end of line.
// Tokenize each token much like the vanilla tokenizer.
// Attach token under current word.

bool go = getCMLtoken();	// Build first token under word.

// ADD VALUES TO TOK NODE.
if (zoneflag)
	replaceNum(tok_,_T("NZ"),1);	// NEW ZONE.
if (lineflag)
	replaceNum(tok_,_T("NL"),1);	// NEW LINE.
if (zoneflag && !lineflag)
	replaceNum(word_,_T("SL"),1);	// SPLIT LINE.

if (go)
	{
	while (getCMLtoken())
		;
	}

updateWordnode();	// Update vars for current word.

// Get end of line.
if (*fmptr_ != '\n')
	return false;
// Get lookahead.
++fmptr_;
++fmpos_;
++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.

return true;
}


/********************************************
* FN:		GETCMLTOKEN
* CR:		08/20/08 AM.
* SUBJ:	Get a token from current CML line.
* RET:	True if didn't see end of line etc.  Else false.
* NOTE:	Ignoring any UNICODE issues for now.
********************************************/

bool CMLTok::getCMLtoken()
{
if (!*fmptr_)	// End of buffer.
	return false;

// Ignore any spaces and tabs for now.  (CML Error?)
if (!skipBlanks())
	return false;	// End of buffer.

if (*fmptr_ == '\n')
	return false;

// COLLECT LINE DATA.
++ltoks_;

// COPY TOKEN BACK INTO SAME BUFFER.
ptr_ = toptr_;	// Current position in "to" buffer.
_TCHAR *str = toptr_;	// Save this.
long len = 0;	// Token length.
long o_start = topos_;	// Save start offset.
long ulen = 0;	// [UNICODE]	// 06/15/22 AM.
long ustart = toupos_;	// [UNICODE]	// 06/15/22 AM.

// ALPHABETIC TOKEN
if (alphabetic(*fmptr_))
	{
	*ptr_ = *fmptr_;
	++len;
	++ulen;	// [UNICODE MOD]	// 06/15/22 AM.
	barcodeChar(*fmptr_);
	bool any_lower = false;
	bool any_upper = false;
	if (is_upper((_TUCHAR)*fmptr_))
		any_upper = true;
	else
		any_lower = true;
	while (alphabetic(*++fmptr_))
		{
		++fmpos_;
		*++ptr_ = *fmptr_;
		++len;
		++ulen;	// [UNICODE MOD]	// 06/15/22 AM.
		barcodeChar(*fmptr_);
		if (is_upper((_TUCHAR)*fmptr_))
			any_upper = true;
		else
			any_lower = true;
		}
	++fmpos_;
//	*++ptr_ = '\0';	// Terminate str.
	// Make token.
	// Attach to parse tree.
	makeToknode(o_start,len,ustart,ulen,PNALPHA,toptr_);
	// Update state.
	topos_ += len;
	toupos_ += ulen;	// [UNICODE MOD]	// 06/15/22 AM.
	toptr_ = ++ptr_;	// Sitting on the char position to fill next.
	*toptr_ = '\0';	// Terminate string for spellcheck.
	// From pointers are already at lookahead.

	// Collect data.
	wlen_ += len;
	walphas_ += len;
	// [UNICODE MOD]	// What to do about word length.	// 06/15/22 AM.

	if (Arun::spellword(0,str))
		{
		++lknowns_;
		replaceNum(tok_,_T("known"),1);
		}
	else
		{
		++lunks_;
		replaceNum(tok_,_T("unk"),1);
		}

	// Single-letter cap being counted as cap here.
	if (!any_lower && len > 1)
		{
		++luppers_;	// Uppercase only.
		replaceNum(tok_,_T("upper"),1);
		if (!lfirst_)
			lfirst_ = _T("upper");
		llast_ = _T("upper");
		}
	else if (any_upper)
		{
		++lcaps_;	// Any mix having upper and lower (or single cap letter).
		replaceNum(tok_,_T("cap"),1);
		if (!lfirst_)
			lfirst_ = _T("cap");
		llast_ = _T("cap");
		}
	else
		{
		++llowers_;	// All lowercase.
		replaceNum(tok_,_T("lower"),1);
		if (!lfirst_)
			lfirst_ = _T("lower");
		llast_ = _T("lower");
		}

	llasteos_ = llastcolon_ = 0;
	return true;
	}

// OTHER EXTENDED ANSI CHAR.
if (*fmptr_ < 0)
	{
	// Put from vars at lookahead.
	*ptr_++ = *fmptr_++;
	++fmpos_;
	++len;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	++ulen;	// [UNICODE MOD]	// 06/15/22 AM.
	makeToknode(o_start,len, ustart, ulen, PNCTRL,toptr_);	// [UNICODE]	// 06/15/22 AM.
	++topos_;
	++toupos_;	// [UNICODE MOD]	// 06/15/22 AM.

	toptr_ = ptr_;
	if (!lfirst_)
		lfirst_ = _T("ctrl");
	llast_ = _T("ctrl");
	llasteos_ = llastcolon_ = 0;
	return true;
	}

// NUMERIC TOKEN
if (_istdigit(*fmptr_) && *fmptr_ > 0)
	{
	*ptr_ = *fmptr_;
	++len;
	++ulen;	// [UNICODE MOD]	// 06/15/22 AM.
	barcodeChar(*fmptr_);
	while (_istdigit(*++fmptr_) && *fmptr_ > 0)
		{
		++fmpos_;
		*++ptr_ = *fmptr_;
		++len;
		++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
		++ulen;	// [UNICODE MOD]	// 06/15/22 AM.
		barcodeChar(*fmptr_);
		}
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
//	*++ptr_ = '\0';	// Terminate str.
	// Make token.
	// Attach to parse tree.
	makeToknode(o_start,len,ustart,ulen,PNNUM,toptr_);
	// Update state.
	topos_ += len;
	++toupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	toptr_ = ++ptr_;	// Sitting on the char position to fill next.
	// From pointers are already at lookahead.

	// Collect data.
	wlen_ += len;
	wnums_ += len;

	if (!lfirst_)
		lfirst_ = _T("num");
	llast_ = _T("num");
	llasteos_ = llastcolon_ = 0;

	return true;
	}

// PUNCT TOKEN
if (_istpunct((_TUCHAR)*fmptr_))
	{
	int ansi = *fmptr_;	// Put this as a variable on the node!
	barcodeChar(*fmptr_);
	// Put from vars at lookahead.
	*ptr_++ = *fmptr_++;
	++fmpos_;
	++len;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	++ulen;	// [UNICODE MOD]	// 06/15/22 AM.
	makeToknode(o_start,len,ustart,ulen,PNPUNCT,toptr_);
	_TCHAR ch = *toptr_;
	++topos_;
	++toupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	toptr_ = ptr_;

	// Collect data.
	wlen_ += 1;
	wpuncts_ += 1;

	if (!lfirst_)
		lfirst_ = _T("punct");
	llast_ = _T("punct");
	switch (ch)
		{
		case ':':
			llasteos_ = llastcolon_ = 1;
			break;
		case '.':
		case ';':	// Could be a mis-OCRed colon.
			llasteos_ = 1;
			llastcolon_ = 0;
			break;
		default:
			llasteos_ = llastcolon_ = 0;
			break;
		}

	return true;
	}


// Get whitespace -- shouldn't be any here.
if (_istspace(*fmptr_))
	return false;

// CTRL CHAR TOKEN
// Everything not trapped above is assumed control char, non-ASCII.
	{
	if (!bad_)
		{
		bad_ = true;
		std::_t_strstream gerrStr;
		gerrStr << _T("[Non-ASCII chars in file.]") << std::ends;
		errOut(&gerrStr,false);
		}

	int ansi = *fmptr_;	// Put this as a variable on the node!
	// Put from vars at lookahead.
	*ptr_++ = *fmptr_++;
	++fmpos_;
	++len;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	++ulen;	// [UNICODE MOD]	// 06/15/22 AM.
	makeToknode(o_start,len,ustart,ulen,PNCTRL,toptr_);
	++topos_;
	toptr_ = ptr_;

	if (!lfirst_)
		lfirst_ = _T("ctrl");
	llast_ = _T("ctrl");
	llasteos_ = llastcolon_ = 0;

	return true;
	}

// Grab token till next type or end of line.
return true;
}


/********************************************
* FN:		GETCMLNUM
* CR:		08/19/08 AM.
* SUBJ:	Get a CML numeric field.
* RET:	ok - true if ok, false if failure.
********************************************/

inline bool CMLTok::getCMLnum(
	long &field			// CML field to fill.
	)
{
ptr_ = &(buf_[0]);
*ptr_ = *fmptr_;	// Lookahead char.

while ((*++ptr_ = *++fmptr_))
	{
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	if (*ptr_ == '|')
		{
		*ptr_ = '\0';	// Terminate.
		break;
		}
	else if (!_istdigit(*ptr_))
		return false;	// Bad CML header.
	// else digit, keep going.
	}
if (!*fmptr_)	// If empty, error.
	return false;

// Slough the pipe char.
++fmptr_;
++fmpos_;
++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.

if (!str_to_long(buf_,/*UP*/ field))
	return false;
return true;
}

/********************************************
* FN:		GETCMLPAGETAG
* CR:		08/19/08 AM.
* SUBJ:	Handle a CML page tag line.
* RET:	ok - true if ok, false if failure.
********************************************/

bool CMLTok::getCMLpagetag()
{
long pagenum = 0;
bool endtag = false;
if (!readCMLpagetag(pagenum, endtag))
	return false;
if (endtag)
	return true;	// Just ignore the </page> end tag.

// Now build a _page node.
// Attach node to tree.
// End offset will be updated when processing
// first and last word of page.
return makePagenode(pagenum) ? true : false;
}


/********************************************
* FN:		READCMLPAGETAG
* CR:		08/19/08 AM.
* SUBJ:	Process a CML tag line.
* RET:	ok - true if ok, false if failure.
* NOTE:	Collect tag name to validate. Collect page number to record.
********************************************/

bool CMLTok::readCMLpagetag(
	long &pagenum,
	bool &endtag
	)
{
// Flag if the page endtag was seen.
endtag = false;

// Ok, we saw the left angle bracket <.

// If it's an end tag, we are ignoring this line.
if (*(fmptr_+1) == '/')	// END TAG.
	{
	while (*fmptr_ && *fmptr_ != '\n')
		{
		++fmptr_;
		++fmpos_;
		++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
		}
	endtag = true;
	return true;
	}

ptr_ = &(buf_[0]) - 1;	// Set up for loop.

// Get alphabetic token.
while ((*++ptr_ = *++fmptr_))
	{
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	if (!alphabetic(*ptr_))
		{
		*ptr_ = '\0';	// Terminate.
		break;
		}
	// else alpha, keep going.
	}
if (!*fmptr_)	// If empty, error.
	return false;

// See if tag name is PAGE.
if (strcmp_i(buf_,_T("PAGE")))
	return false;

// Get separator (blank or underscore)
ptr_ = &(buf_[0]) - 1;	// Set up for loop.

// Get separator chars.
while ((*++ptr_ = *++fmptr_))
	{
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	if (*ptr_ == '\n')
		return false;
	if (!_istspace(*ptr_) && *ptr_ != '_')
		{
		*ptr_ = '\0';	// Terminate.
		break;
		}
	// else separator, keep going.
	}
if (!*fmptr_)	// If empty, error.
	return false;

// Get page number.
ptr_ = &(buf_[0]);
*ptr_ = *fmptr_;			// First digit of page number.
while ((*++ptr_ = *++fmptr_))
	{
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	if (!_istdigit(*ptr_))
		break;
	// else separator, keep going.
	}
*ptr_ = '\0';	// Terminate.
if (!*fmptr_)	// If empty, error.
	return false;

// Grab the page number.
pagenum = 0;
if (!str_to_long(buf_,pagenum))
	return false;

// Todo: Can check that page number is one more than previous.
if (P_ + 1 != pagenum)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[CML Tokenize: Error in CML page numbering.]") << std::ends;
	errOut(&gerrStr,false);
	}

P_ = pagenum;	// Use the latest page number found.

if (!skipBlanks())
	return false;

// Get right angle bracket.
if (*fmptr_ != '>')
	return false;

// Get past >.
++fmptr_;
++fmpos_;
++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.

if (!skipBlanks())
	return false;

// Get end of line.
if (*fmptr_ != '\n')
	return false;

// Clear the end of line.
// Get lookahead char for next line.
++fmptr_;
++fmpos_;
++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.

return true;

}

/********************************************
* FN:		GETCMLBLANK
* CR:		08/19/08 AM.
* SUBJ:	Handle a CML blank line.
* RET:	ok - true if ok, false if failure.
********************************************/

bool CMLTok::getCMLblank()
{
// Grab and throw away this line.
while (*fmptr_ && _istspace(*fmptr_))
	{
	++fmptr_;
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	}

return true;
}


/********************************************
* FN:		SKIPBLANKS
* CR:		08/19/08 AM.
* SUBJ:	Skip blanks in the input buffer.
* RET:	ok - true if more text, false if end of buffer.
* NOTE:	Not skipping newlines.
********************************************/

bool CMLTok::skipBlanks()
{
while (*fmptr_ && _istspace(*fmptr_) && *fmptr_ != '\n')
	{
	++fmptr_;
	++fmpos_;
	++fmupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	}

if (!fmptr_)
	return false;

return true;
}


/********************************************
* FN:		MAKEPAGENODE
* CR:		08/19/08 AM.
* SUBJ:	Build a page node in parse tree.
* RET:	node - newly created page node.
********************************************/

Node<Pn> *CMLTok::makePagenode(long pagenum)
{
updatePagenode();			// Update info for the current page node.
updateLinenode(false);	// Update info for the current line node.

_TCHAR *lcstr;
Sym *sym = internTok(_T("_PAGE"), 5, htab_,lcstr);
_TCHAR *str = sym->getStr();

Node<Pn> *node = Pn::makeNode(
	topos_,		// o_start
	-1,			// o_end
	toupos_,	// [UNICODE]	// 06/15/22 AM.
	-1,	// [UNICODE]	// 06/15/22 AM.
	PNNODE,		// Node type
	toptr_,		// Pointer to text input buffer
	str,			// Suggested concept str
	sym,			// Suggested concept sym
	0,				// Assoc line number
	0,				// Assoc code pass
	0				// Assoc rule line
	);

if (!node)
	return 0;

Pn *pn = node->getData();
pn->setUnsealed(true);

// Attach node to tree.
if (page_)	// Already a page in tree.
	tree_->insertRight(*node,*page_);
else
	tree_->insertDown(*node,*root_);

page_ = node;	// Node is now the current page.

// No line or word node under this page yet.
line_ = word_ = 0;

tok_ = firsttok_ = 0;	// Reset.

// NEW PAGE RESETS CML TRACKING
Z_ = L1_ = W1_ = -1;
prevZ_ = prevL1_ = -1;

// ADD VARIABLES
replaceNum(node,_T("P"),P_);

return node;
}


/********************************************
* FN:		MAKELINENODE
* CR:		08/20/08 AM.
* SUBJ:	Build a line node in parse tree.
* RET:	node - newly created node.
********************************************/

Node<Pn> *CMLTok::makeLinenode()
{
// Reset data collection.
llen_ = lbars_ = lalphas_ = lnums_ = lpuncts_ = 0;
lwords_ = ltoks_ = lcaps_ = luppers_ = llowers_ = lknowns_ = lunks_ = 0;
lineR_ = gapL_ = gapR_ = 0;
lfirst_ = llast_ = 0;
llasteos_ = llastcolon_ = 0;

_TCHAR *lcstr;
Sym *sym = internTok(_T("_LINE"), 5, htab_,lcstr);
_TCHAR *str = sym->getStr();

Node<Pn> *node = Pn::makeNode(
	topos_,		// o_start
	-1,			// o_end
	toupos_,	// [UNICODE]	// 06/15/22 AM.
	-1,	// [UNICODE]	// 06/15/22 AM.
	PNNODE,		// Node type
	toptr_,		// Pointer to text input buffer
	str,			// Suggested concept str
	sym,			// Suggested concept sym
	0,				// Assoc line number
	0,				// Assoc code pass
	0				// Assoc rule line
	);

if (!node)
	return 0;

Pn *pn = node->getData();
pn->setUnsealed(true);

// Attach node to tree.
if (line_)	// A line is around.
	tree_->insertRight(*node,*line_);
else if (page_)
	tree_->insertDown(*node,*page_);
else	// Assuming ok to not have pages around.
	tree_->insertDown(*node,*root_);

line_ = node;	// Node is now the current line.

// No word nodes under this line yet.
word_ = 0;

tok_ = firsttok_ = 0;	// Reset.

// ADD VARIABLES
replaceNum(node,_T("P"),P_);
replaceNum(node,_T("Z"),Z_);
replaceNum(node,_T("L1"),L1_);

replaceNum(node,_T("L"),L_);		// Leftmost of line.
replaceNum(node,_T("R"),L_+W_);	// Rightmost of line. Placeholder.

replaceNum(node,_T("W1"),W1_);	// FIRST WORD IN LINE.
replaceNum(node,_T("Wn"),W1_);	// LAST WORD IN LINE.	Placeholder.

return node;
}


/********************************************
* FN:		MAKEWORDNODE
* CR:		08/20/08 AM.
* SUBJ:	Build a word node in parse tree.
* RET:	node - newly created node.
********************************************/

Node<Pn> *CMLTok::makeWordnode()
{
// Must have a line to put the word node under.
if (!line_)
	return 0;

// COLLECT LINE DATA.
++lwords_;

// Reset data collection.
wlen_ = wbars_ = walphas_ = wnums_ = wpuncts_ = 0;

_TCHAR *lcstr;
Sym *sym = internTok(_T("_WORD"), 5, htab_,lcstr);
_TCHAR *str = sym->getStr();

Node<Pn> *node = Pn::makeNode(
	topos_,		// o_start
	-1,			// o_end
	toupos_,	// [UNICODE]	// 06/15/22 AM.
	-1,	// [UNICODE]	// 06/15/22 AM.
	PNNODE,		// Node type
	toptr_,		// Pointer to text input buffer
	str,			// Suggested concept str
	sym,			// Suggested concept sym
	0,				// Assoc line number
	0,				// Assoc code pass
	0				// Assoc rule line
	);

if (!node)
	return 0;

// Attach node to tree.
if (word_)
	{
	tree_->insertRight(*node,*word_);
	// ADD A WHITESPACE TO THE RETEXT BUFFER.
	*toptr_++ = ' ';
	++topos_;
	++toupos_;	// [UNICODE MOD]	// 06/15/22 AM.
	}
else	// FIRST WORD IN LINE.
	{
	tree_->insertDown(*node,*line_);
	}

word_ = node;	// Node is now the current word.

tok_ = firsttok_ = 0;	// Reset.

// ADD VARIABLES
replaceNum(node,_T("Z"),Z_);
replaceNum(node,_T("L1"),L1_);
replaceNum(node,_T("W1"),W1_);
replaceNum(node,_T("L"),L_);
replaceNum(node,_T("T"),T_);
replaceNum(node,_T("W"),W_);
replaceNum(node,_T("H"),H_);
replaceNum(node,_T("C"),C_);

return node;
}


/********************************************
* FN:		MAKETOKNODE
* CR:		08/20/08 AM.
* SUBJ:	Build a token node in parse tree.
* RET:	node - newly created node.
* NOTE:	Update bookeeping as needed.
********************************************/

Node<Pn> *CMLTok::makeToknode(
	long o_start,
	long len,
	long ustart,	// [UNICODE]	// [IGNORE]
	long ulen,	// [UNICODE]	// [IGNORE]
	enum Pntype typ,
	_TCHAR *text	// Text buffer ptr.
	)
{
// Must have a word to put the tok node under.
if (!word_)
	return 0;

if (len <= 0)
	return 0;

long o_end = o_start + len - 1;

_TCHAR *lcstr;
Sym *sym = internTok(text,len,htab_,lcstr);
_TCHAR *str = sym->getStr();

long uend = ustart + ulen - 1;	// [UNICODE]

Node<Pn> *node = Pn::makeNode(
	o_start,			// o_start
	o_end,			// o_end
	ustart,	// [UNICODE]	// 06/15/22 AM.
	uend,	// [UNICODE]	// 06/15/22 AM.
	typ,			// Node type
	text,			// Pointer to text input buffer
	str,			// Suggested concept str
	sym,			// Suggested concept sym
	0,				// Assoc line number
	0,				// Assoc code pass
	0				// Assoc rule line
	);

if (!node)
	return 0;

// Attach node to tree.
if (tok_)
	{
	tree_->insertRight(*node,*tok_);
	replaceNum(node,_T("NOSP"),1);	// Flag NO SPACE before this node, for pretty print.
	}
else
	{
	firsttok_ = node;
	tree_->insertDown(*node,*word_);
	}

// ADD VARIABLES

if (typ == PNCTRL)
	{
	int ansi = (unsigned char) *text;	// Put this as a variable on the node!
	replaceNum(node,_T("CTRL"),ansi);
	}
replaceNum(node,_T("Z"),Z_);
replaceNum(node,_T("L1"),L1_);
replaceNum(node,_T("W1"),W1_);
replaceNum(node,_T("L"),L_);
replaceNum(node,_T("T"),T_);
replaceNum(node,_T("W"),W_);
replaceNum(node,_T("H"),H_);
replaceNum(node,_T("C"),C_);

tok_ = node;	// Node is now the current tok.

return node;
}


/********************************************
* FN:		UPDATEPAGENODE
* CR:		08/22/08 AM.
* SUBJ:	Update vars for a page node in parse tree.
* RET:	ok - true if ok..
********************************************/

bool CMLTok::updatePagenode()
{
if (!page_)
	return true;

// Update end offset for line.
if (!tok_)
	return false;

Pn *pn = page_->getData();
pn->setEnd(topos_);

//if (llen_)
//	replaceNum(page_,_T("len"),llen_);

return true;
}

/********************************************
* FN:		UPDATELINENODE
* CR:		08/22/08 AM.
* SUBJ:	Update vars for a line node in parse tree.
* RET:	ok - true if ok..
********************************************/

bool CMLTok::updateLinenode(
	bool last	// Flag last line in text.
	)
{
if (!line_)
	return true;

// Update end offset for line.
if (!tok_)
	return false;

// Get the current word's offset.
if (word_)
	{
	Pn *pnw = word_->getData();
	Pn *pn = line_->getData();
	pn->setEnd(pnw->getEnd());
	}
// Else error.

// Collect last word in current line.
if (last)	// Last line in text.
	replaceNum(line_,_T("Wn"),W1_);		// Last word in parse tree.
else if (W1_ > 0)							// First word in NEXT line.
	replaceNum(line_,_T("Wn"),W1_ - 1);	// Last word in current line.

if (lineR_ > 0)
	replaceNum(line_,_T("R"),lineR_);

if (gapR_)
	{
	replaceNum(line_,_T("gap L"),gapL_);
	replaceNum(line_,_T("gap R"),gapR_);
	}

if (llen_)
	replaceNum(line_,_T("len"),llen_);
if (lbars_)
	replaceNum(line_,_T("bars"),lbars_);
if (lalphas_)
	replaceNum(line_,_T("alphas"),lalphas_);
if (lnums_)
	replaceNum(line_,_T("nums"),lnums_);
if (lpuncts_)
	replaceNum(line_,_T("puncts"),lpuncts_);

if (lwords_)
	replaceNum(line_,_T("words"),lwords_);
if (ltoks_)
	replaceNum(line_,_T("toks"),ltoks_);
if (luppers_)
	replaceNum(line_,_T("uppers"),luppers_);
if (lcaps_)
	replaceNum(line_,_T("caps"),lcaps_);
if (llowers_)
	replaceNum(line_,_T("lowers"),llowers_);
if (lknowns_)
	replaceNum(line_,_T("knowns"),lknowns_);
if (lunks_)
	replaceNum(line_,_T("unks"),lunks_);

if (lfirst_)
	replaceStr(line_,_T("first"),lfirst_);
if (llast_)
	replaceStr(line_,_T("last"),llast_);
if (llasteos_)
	replaceNum(line_,_T("last eos"),llasteos_);
if (llastcolon_)
	replaceNum(line_,_T("last colon"),llastcolon_);

return true;
}


/********************************************
* FN:		UPDATEWORDNODE
* CR:		08/22/08 AM.
* SUBJ:	Update vars for a word node in parse tree.
* RET:	ok - true if ok.
* NOTE:	Update after word's last tok is done.
********************************************/

bool CMLTok::updateWordnode()
{
if (!word_)
	return true;

// Update end offset for word.
if (!tok_)
	return false;

Pn *pn = word_->getData();
Pn *pnt = tok_->getData();
pn->setEnd(pnt->getEnd());

if (wlen_)
	replaceNum(word_,_T("len"),wlen_);
if (wbars_)
	replaceNum(word_,_T("bars"),wbars_);
if (walphas_)
	replaceNum(word_,_T("alphas"),walphas_);
if (wnums_)
	replaceNum(word_,_T("nums"),wnums_);
if (wpuncts_)
	replaceNum(word_,_T("puncts"),wpuncts_);

// UPDATE LINE VARS.
llen_		+= wlen_;
lbars_	+= wbars_;
lalphas_	+= walphas_;
lnums_	+= wnums_;
lpuncts_	+= wpuncts_;

return true;
}

/********************************************
* FN:		BARCODECHAR
* CR:		08/22/08 AM.
* SUBJ:	Collect word's barcode char info.
* RET:	true if ok.
* NOTE:	Shouldn't need to worry about interning name.
********************************************/

inline bool CMLTok::barcodeChar(
	_TCHAR ch
	)
{
if (!ch)
	return true;

switch (ch)
	{
	case '|': case '1': case '!': case 'I': case 'i': case 'l':
	case '}': case '{': case ')': case '(': case '[': case ']':
	case ':': case 'H': case 'U':
		++wbars_;
	default:
		break;
	}

return true;
}


/********************************************
* FN:		REPLACENUM
* CR:		08/21/08 AM.
* SUBJ:	Add/replace numeric var in node.
* RET:	true if ok.
* NOTE:	Shouldn't need to worry about interning name.
********************************************/

inline bool CMLTok::replaceNum(
	Node<Pn> *node,
	_TCHAR *name,	// variable name.
	long long val
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

inline bool CMLTok::replaceStr(
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

/*************************************************************************************/
/*************************************************************************************/



/********************************************
* FN:		INTERNTOK
* CR:		10/28/98 AM.
* SUBJ:	Internalize the token.
* NOTE:	Need to lookup/place one copy of the token in a hash table.
*			(If alphabetic).  This is a placeholder for string and hash
*			table implementation in the future.
********************************************/

Sym *CMLTok::internTok(
	_TCHAR *str,				// Ptr to string in a buffer.
	long len,				// Length of string within buffer.
	Htab *htab,				// Hashed symbol table.
	/*UP*/ _TCHAR *&lcstr
	)
{
if (empty(str) || len <= 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[internTok: Given bad string or length.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// If token too long, truncate.	// FIX.
if (len >= MAXSTR)					// FIX.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern Token: Too long -- truncating.]") << std::ends;
	errOut(&gerrStr,false);
	len = MAXSTR - 1;	// Recover.	// FIX.
	}

// 01/26/99 AM. Building lowercase variant of sym here also.
Sym *sym;
//if (!(sym = htab->hsym(str, len)))	
if (!(sym = htab->hsym_kb(str, len,/*UP*/lcstr)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern Token: Failed.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

return sym;
//return sym->getStr();
//return make_str(str, len);		// Make null terminated string.
}
