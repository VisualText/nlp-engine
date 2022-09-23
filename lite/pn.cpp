/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PN.CPP
* FILE:	lite\pn.cpp
* CR:		10/08/98 AM.
* SUBJ:	XXX
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "lite/lite.h"		// 02/26/01 AM.
//#include "u_ofstream.h"		// 03/07/05 AM.
#include "lite/global.h"													// 02/14/01 AM.
#include "u_out.h"	// TRYING.	// 03/10/05 AM.
#include "inline.h"			// For str().								// 05/21/01 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"		// 02/26/01 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "sem.h"				// 02/26/01 AM.
#include "sym.h"				// 02/26/01 AM.
#include "ipair.h"			// 02/26/01 AM.
#include "str.h"				// 03/06/05 AM.

#include "pn.h"				// 02/14/01 AM.
#include "parse.h"
#include "io.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 aM.
int Tree<Pn>::count_ = 0;
template<>	// Linux.	// 04/26/07 aM.
int Node<Pn>::count_ = 0;
template<>	// Linux.	// 04/26/07 aM.
int Dlist<Pn>::count_ = 0;
template<>	// Linux.	// 04/26/07 aM.
int Delt<Pn>::count_ = 0;
#else
int Tree<Pn>::count_ = 0;
int Node<Pn>::count_ = 0;
int Dlist<Pn>::count_ = 0;
int Delt<Pn>::count_ = 0;
#endif
int Pn::count_ = -2;				// 11/17/98 AM. One static Pn.

// WARNING: Must match pn.h/enum Pntype definition.
static _TCHAR *Pntype_s[] =
	{
	_T("null"),
	_T("node"),
	_T("alpha"),
	_T("num"),
	_T("punct"),
	_T("white"),
	_T("ctrl"),
	_T("emoji")
	};

// WARN! Numbers are octal.
static const int bitBASE = 01;		// Node's BASE attribute.
static const int bitUNSEALED = 02;	// UNSEALED attr.				// 10/09/99 AM.
static const int bitFIRED = 04;		// FIRED flag.					// 05/21/01 DD.
static const int bitBUILT= 010;		// BUILT flag.					// 05/25/01 AM.
// 02, 04, 010, 020, 040, 0100...


#include "pn.h"


/********************************************
* FN:		Special Functions for Pn Class
* CR:		10/08/98 AM.
********************************************/

Pn::Pn()
{
//*gout << "[Pn: " << (long) this << "]" << std::endl;
Start = End = -1;
Type = PNNULL;
Text = 0;
name_ = 0;
flags_ = 0;
rulelt_ = 0;
//base_ = false;				// 11/23/98 AM.
sem_ = 0;						// Ouch!  Bug fix.	// 11/11/98 AM.
dsem_ = 0;						// 08/01/99 AM.
sym_ = 0;						// 11/19/98 AM.
line_ = 0;						// 05/17/01 AM.
passnum_ = 0;					// 01/04/02 AM.
ruleline_ = 0;					// 01/04/02 AM.
restart_ = 0;					// 07/17/06 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Pn::Pn(long start, long end, enum Pntype type, _TCHAR *txt, _TCHAR *name,
	long line,		// Line number for debug.							// 05/17/01 AM.
	long passnum,	// Pass number of code.								// 01/04/02 AM.
	long ruleline	// Rule line number for debug.					// 01/04/02 AM.
	)
{
//*gout << "[Pn: " << (long) this << "]" << std::endl;
Start = start;
End = end;
Type = type;
Text = txt;
name_ = name;
rulelt_ = 0;
line_	= line;					// 05/17/01 AM.
passnum_  = passnum;			// 01/04/02 AM.
ruleline_ = ruleline;		// 01/04/02 AM.
//base_ = false;					// 11/23/98 AM.
sem_ = 0;						// Ouch!  Bug fix.	// 11/11/98 AM.
dsem_ = 0;						// 08/01/99 AM.
restart_ = 0;					// 07/17/06 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Pn::~Pn()
{
//*gout << "[~Pn: " << (long) this << "]" << std::endl;

// 11/17/98 AM.
if (sem_)
	delete (Sem *) sem_;

if (dsem_)					// 08/01/99 AM.
	Dlist<Ipair>::DeleteDlistAndData(dsem_);	// 08/01/99 AM.

// Don't delete sym_.  It belongs to the hashed symbol table.	// 11/19/98 AM.

#ifndef STABLE_
--count_;
#endif
}

// Copy constructor.  10/26/98 AM.
Pn::Pn(Pn &pn)
{
//*gout << "[Pn copy: " << (long) this << "]" << std::endl;
Start = pn.Start;
End	= pn.End;
Type	= pn.Type;
Text	= pn.Text;
name_ = pn.name_;			// Will point to a string buffer.
flags_ = pn.flags_;
rulelt_ = pn.rulelt_;
//base_  = pn.base_;		// 11/23/98 AM.
sem_  = pn.sem_;
dsem_ = pn.dsem_;			// 08/01/99 AM.
sym_  = pn.sym_;			// 11/19/98 AM.
line_ = pn.line_;			// 05/17/01 AM.
passnum_  = pn.passnum_;	// 01/04/02 AM.
ruleline_ = pn.ruleline_;	// 01/04/02 AM.
restart_ = pn.restart_;		// 07/17/06 AM.

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, const Pn &pn)
{
_TCHAR *buf;
//long oend;
// buf = Current_Parse->getText();
// 10/12/98 AM. Now buf already points to start of text.

// Print the text for this node.

//_TCHAR *tmpbuf;
//char *newbuf;
//if (buf = pn.name_)
//   {
//	while (*buf)
//		{
//		u_string *x;
//		x =(u_string *) pretty_char(*buf++);
//		operator<<(output,(u_string*)x);
//		output << x;
//		operator<<(output,(u_string*) pretty_char(*buf++));
//		output << (u_string *) pretty_char(*buf++);

//		tmpbuf = pretty_char(*buf++);
//		UTL_GetMBCSFromUnicodeString(tmpbuf, CP_UTF8, (LPCTSTR*&)newbuf);
//		output << newbuf;
//		if (newbuf)
//			delete newbuf;

//		}
//	}

// U+200E left-to-right mark.
// U+200F right-to-left mark.

if ((buf = pn.name_))
   {
	while (*buf)
		{
#ifndef UNICODE
		output << pretty_char(*buf++);	// 11/1/98 AM.
#else
		if (*buf == '\u200F') {
			int z = 1;
		}
		_TCHAR *lpszBuffer = pretty_char(*buf++);	// 01/25/06 AM.
		char *lpstr8;	// 01/25/06 AM.
		u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);	// 01/25/06 AM.
		output << lpstr8;	// 01/25/06 AM.
		u_delete((LPCTSTR*&)lpstr8);	// 01/25/06 AM.
#endif
		}
	}
else
	output << _T("<<NO NAME>>");
output
#ifdef UNICODE
	<< _T(" r_to_l")
#endif
	<< _T(" [")
	<< pn.Start
	<< _T(",") << pn.End
	<< _T(",") << pn.Ustart	// [UNICODE]
	<< _T(",") << pn.Uend		// [UNICODE]
	<< _T(",") << pn.passnum_			// CODE PASSNUM.					// 01/04/02 AM.
	<< _T(",") << pn.ruleline_			// CODE LINENO IN PASS FILE	// 01/04/02 AM.
	<< _T(",") << Pntype_s[pn.Type]	// 11/13/99 AM.
	;


if (pn.getBase())
	output << _T(",b");			// 11/13/99 AM.
if (pn.getUnsealed())		// 10/11/99 AM.
	output << _T(",un");			// 11/13/99 AM.
if (pn.sem_)
	{
	//output << "," << ((long) pn.sem_);			// 11/11/98 AM. FOR DEBUGGING.
	output << _T(",sem");			// 11/10/98 AM.
	}
if (pn.getFired())															// 05/21/01 DD.
	output << _T(",fired");														// 05/21/01 DD.
if (pn.getBuilt())															// 05/25/01 AM.
	output << _T(",blt");															// 05/25/01 AM.
if (pn.dsem_)					// 08/01/99 AM.
	{
	// PRINTING VARIABLES AND VALUES TO THE LOG FILE. Sheesh.	// 05/21/01 DD.
	//output << ",dsem";														// 05/21/01 DD.
	Delt<Ipair> *delt;														// 05/21/01 DD.
	Dlist<Iarg> *vals;														// 05/21/01 DD.
	Delt<Iarg> *darg;															// 05/21/01 DD.
	Iarg *arg;																	// 05/21/01 DD.
	Ipair *sempair;															// 05/21/01 DD.
	_TCHAR *key;																	// 05/21/01 DD.
//	char *argname;																// 05/21/01 DD.
//	long num;																	// 05/21/01 DD.
	bool firstOne = true;													// 05/21/01 DD.
	for (delt = pn.dsem_->getFirst();									// 05/21/01 DD.
											delt; delt = delt->Right())	// 05/21/01 DD.
		{
		sempair = delt->getData();											// 05/21/01 DD.
		key = sempair->getKey();											// 05/21/01 DD.
		if (firstOne)															// 05/21/01 AM.
			{
			output << _T(",");														// 05/21/01 AM.
			firstOne = false;													// 05/21/01 AM.
			}
		output << _T(" (\"") << key << _T("\"");									// 05/21/01 AM.

		vals = sempair->getVals();											// 05/21/01 DD.
		for (darg = vals->getFirst(); darg; darg = darg->Right())// 05/21/01 DD.
			{
			arg = darg->getData();											// 05/21/01 DD.
			output << _T(" ");														// 05/21/01 AM.
			// For a long, genArg outputs "(long)#".					// 10/01/01 AM.
			Iarg::genArg(arg, output,										// 05/21/01 AM.
								false);	// Don't display "(long)";		// 10/01/01 AM.
			}
		output << _T(")");															// 05/21/01 AM.
		}
	}
output << _T("]");
return output;
}

// 10/26/98 AM.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Node<Pn> &node)
{
Pn *pn;

pn = node.getData();
if (pn)
	output << *pn;
return output;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
* CR:		10/08/98 AM.
********************************************/

long			Pn::getStart()		{return Start;  }
long			Pn::getEnd()		{return End;    }
long			Pn::getUstart()		{return Ustart; }	// [UNICODE]
long			Pn::getUend()		{return Uend; }	// [UNICODE]
enum Pntype Pn::getType()		{return Type;   }
_TCHAR		  *Pn::getText()		{return Text;   }
_TCHAR		  *Pn::getName()		{return name_;  }
int         Pn::getFlags()		{return flags_; }
Delt<Ielt> *Pn::getRulelt()	{return rulelt_;}
Sem		  *Pn::getSem()		{return sem_;   }
Sym		  *Pn::getSym()		{return sym_;   }
Dlist<Ipair> *Pn::getDsem()	{return dsem_;	 }		// 08/01/99 AM.
long		   Pn::getLine()		{return line_;	 }
long			Pn::getPassnum()	{return passnum_;}	// 01/04/02 AM.
long			Pn::getRuleline()	{return ruleline_;}	// 01/04/02 AM.
Node<Pn>	  *Pn::getRestart()	{return restart_;}	// 07/17/06 AM.

/********************************************
* FN:		Access Bitflags
* CR:		11/03/98 AM.
********************************************/

bool Pn::getBase() const
{
// return base_;																// 11/23/98 AM.
return (flags_ & bitBASE);													// 11/23/98 AM.
}

bool Pn::getUnsealed() const												// 10/09/99 AM.
{
if (flags_ & bitUNSEALED)													// 10/11/99 AM.
	return true;
return false;
}

bool Pn::getFired() const													// 05/21/01 DD.
{
if (flags_ & bitFIRED)
	return true;
return false;
}

bool Pn::getBuilt() const													// 05/25/01 AM.
{
if (flags_ & bitBUILT)
	return true;
return false;
}


// Little convenience function.  Saves some function calls.
void Pn::getTextStartEnd(_TCHAR* &text, long &start, long &end)
{
text = Text;
start = Start;
end = End;
}

/********************************************
* FN:		Modify Functions
* CR:		10/08/98 AM.
********************************************/

void Pn::setStart(long x)			 {Start   = x;}
void Pn::setEnd(long x)				 {End     = x;}
void Pn::setUstart(long x)			 {Ustart   = x;}
void Pn::setUend(long x)			 {Uend     = x;}
void Pn::setType(enum Pntype x)	 {Type    = x;}
void Pn::setText(_TCHAR *x)			 {Text    = x;}
void Pn::setName(_TCHAR *x)			 {name_   = x;}
void Pn::setFlags(int x)			 {flags_  = x;}
void Pn::setRulelt(Delt<Ielt> *x) {rulelt_ = x;}
void Pn::setSem(Sem *x)				 {sem_    = x;}
void Pn::setSym(Sym *x)				 {sym_    = x;}
void Pn::setDsem(Dlist<Ipair> *x) {dsem_	 = x;}
void Pn::setLine(long x)			 {line_	 = x;}
void Pn::setPassnum(long x)		 {passnum_  = x;}
void Pn::setRuleline(long x)		 {ruleline_ = x;}
void Pn::setRestart(Node<Pn> *x)	 {restart_  = x;}


/********************************************
* FN:		Modify Bitflags
* CR:		11/03/98 AM.
********************************************/

void Pn::setBase(bool x)
{
//base_ = x;			// 11/23/98 AM.

if (x)
	flags_ |= bitBASE;
else
	flags_ &= (~bitBASE);
}

void Pn::setUnsealed(bool x)			// 10/09/99 AM.
{
if (x)
	flags_ |= bitUNSEALED;				// 10/11/99 AM.
else
	flags_ &= (~bitUNSEALED);			// 10/11/99 AM.
}


void Pn::setFired(bool x)													// 05/21/01 DD.
{
if (x)
	flags_ |= bitFIRED;
else
	flags_ &= (~bitFIRED);
}
void Pn::setBuilt(bool x)													// 05/25/01 AM.
{
if (x)
	flags_ |= bitBUILT;
else
	flags_ &= (~bitBUILT);
}


/********************************************
* FN:		SETSTARTEND
* CR:		10/08/98 AM.
********************************************/

void Pn::setStartEnd(long start, long end)
{
if (start < 0 || end < start)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[setStartEnd: Given bad value. start=")
		  << start << _T(" , end=") << end << _T("]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
Start = start;
End = end;
}


/********************************************
* FN:		GETCOUNT
* CR:		10/11/98 AM.
********************************************/
#ifndef STABLE_

int Pn::getCount() { return count_; }
#endif


/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Pn::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Pn count=") << count_ << std::endl;
	*gout << _T("Active Pn count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Pn count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		makeNode
* CR:		10/08/98 AM.
* SUBJ:	Create a node with appropriate data.
* NOTE:	10/12/98 Make into a class member fn.
*			Must be a more elegant way to initialize the node.
* OPT:	To not keep creating a temporary Pn, it is static!
********************************************/
static Pn p;

Node<Pn> *Pn::makeNode(long start, long end,
	long ustart, long uend,	// [UNICODE]
	enum Pntype type, _TCHAR *text,
	_TCHAR *name, Sym *sym,
	long line,				// Line number for debug.					// 05/17/01 AM.
	long passnum,			// Pass number of code.						// 01/04/02 AM.
	long ruleline			// Rule line num for debug.				// 01/04/02 AM.
	)
{
Node<Pn> *node;


p.Start = start;
p.End   = end;
p.Ustart = ustart;	// [UNICODE]
p.Uend = uend;	// [UNICODE]
p.Type  = type;
p.Text  = text;
p.name_ = name;
p.flags_ = 0;
p.rulelt_ = 0;
p.sem_ = 0;
p.dsem_ = 0;					// 08/01/99 AM.
p.sym_ = sym;
//p.base_ = false;			// 11/23/98 AM.
p.line_ = line;				// 05/17/01 AM.
p.passnum_  = passnum;		// 01/04/02 AM.
p.ruleline_ = ruleline;		// 01/04/02 AM.
p.restart_ = 0;				// 07/17/06 AM.

node    = new Node<Pn>(p);
return node;
}

/********************************************
* FN:		makeTnode
* CR:		10/09/99 AM.
* SUBJ:	Create a token node.	
* OPT:	A makeNode optimized for tokens.
********************************************/

Node<Pn> *Pn::makeTnode(long start, long end,
	long ustart, long uend,	// [UNICODE]
	enum Pntype type, _TCHAR *text,
	_TCHAR *name, Sym *sym,
	long line,				// Line number for debug.					// 05/17/01 AM.
	long passnum,			// Pass number of code.						// 01/04/02 AM.
	long ruleline			// Rule line num for debug.				// 01/04/02 AM.
	)
{
Node<Pn> *node;

p.Start = start;
p.End   = end;
p.Ustart = ustart;	// [UNICODE]
p.Uend = uend;	// [UNICODE]
p.Type  = type;
p.Text  = text;
p.name_ = name;
p.flags_ = 0;
p.rulelt_ = 0;
p.sem_ = 0;
p.dsem_ = 0;
p.sym_ = sym;

//p.setBase(true);
//p.setUnsealed(false);		// Default for a node.	// 10/11/99 AM.
p.line_ = line;																// 05/17/01 AM.
p.passnum_  = passnum;														// 01/04/02 AM.
p.ruleline_ = ruleline;														// 01/04/02 AM.
p.restart_ = 0;	// 07/17/06 AM.

node    = new Node<Pn>(p);
return node;
}



/********************************************
* FN:		makeTree
* CR:		10/11/98 AM.
* SUBJ:	Create a tree with appropriate root data.
* NOTE:	Class member function. (Can be called without an instance existing).
********************************************/

Tree<Pn> *Pn::makeTree(
	long ostart,
	long oend,
	long ustart,	// [UNICODE]
	long uend,	// [UNICODE]
	enum Pntype typ,
	_TCHAR *text,
	_TCHAR *name,
	Sym *sym
	)
{
Node<Pn> *root;
root = makeNode(ostart, oend, ustart, uend, typ, text, name, sym,
						1);		// Root line number is 1!				// 05/17/01 AM.
return new Tree<Pn>(root);
}


/********************************************
* FN:		prettyNodes
* CR:		11/05/98 AM.
* SUBJ:	Pretty print list of nodes.
********************************************/

void Pn::prettyNodes(Node<Pn> *nodes)
{
for (; nodes; nodes = nodes->Right())
	*gout << *nodes << _T(" ");
}


/********************************************
* FN:		PNSTR
* CR:		11/06/98 AM.
* SUBJ:	Get a copy of the given node's text.
* ALLOC:	Creates text buffer.
* NOTE:	Right now, node's Text points into the full input text buffer.
* WARN:	In general, the text differs from the node's NAME.
********************************************/

_TCHAR *Pn::pnStr()
{
return make_str(Text, End - Start + 1);
}

// VARIANT.  Gets the allocated length.
_TCHAR *Pn::pnStr(
	long &len		/*UP*/													// 07/23/00 AM.
	)
{
return make_str(Text, len = (End - Start + 1));
}

/********************************************
* FN:		GETLCSYM
* CR:		01/26/99 AM.
* SUBJ:	Get lowercase sym.
********************************************/

Sym *Pn::getlcSym()
{
if (sym_)
	return sym_->getLcsym();
return 0;
}


/********************************************
* FN:		FINDTREENODE
* CR:		03/22/99 AM.
* SUBJ:	Traverse given subtree, looking for given name.
* RET:	Node, if found, else NULL.
* NOTE:	RECURSIVE.
*			Stops at first matching node found.
********************************************/

Node<Pn> *Pn::findTreenode(
	_TCHAR *name,
	Node<Pn> *node,
	bool root			// If this is the top-level node of subtree.
	)
{
// TRAVERSE THE ENTIRE SUBTREE.
if (node == 0)
	return 0;

Node<Pn> *tmp;
Pn *pn;
pn = node->getData();
if (!_tcscmp(name, pn->getName()))
	return node;

if ((tmp =findTreenode(name, node->pDown, false)))		// RECURSIVE.
	return tmp;

// Minimal recursion in this part (optimization and resource saving!).
if (!root && !(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		if ((tmp = findTreenode(name, node, false)))
			return tmp;
		}
	}
return 0;
}


/********************************************
* FN:		FINDOFFSETS
* CR:		05/18/99 AM.
* SUBJ:	Traverse given subtree, looking for start,end offset.
* RET:	True if ok.
*			UP n_start,n_end - First phrase that precisely matches offsets.
*			UP parent - the parent in which the phrase occurs.
* NOTE:	RECURSIVE. (Will call a recursive workhorse.)
*			The phrase could be one node long.
*			Phrase could potentially be split between multiple
*			parents, leading to failure.
*			This fn will be used by rule generation to find the sample
*			in the parse tree.
*			If the edges of the range have disappeared, will cut the
*			caller some slack (e.g., if whitespace has been zapped).
*			Similarly, don't care about gaps within the range.
********************************************/
bool Pn::findOffsets(
	Node<Pn> *tree,		// Root of subtree to search.
	long o_start,			// Start offset sought.
	long o_end,				// End offset sought.
	/*UP*/
	Node<Pn>* &n_start,	// Start node that matched range.
	Node<Pn>* &n_end,		// End node that matched range.
	Node<Pn>* &parent		// Parent containing the found phrase.
	)
{
n_start = n_end = parent = 0;
if (!tree)
	return false;
Pn *pn;
pn = tree->getData();

// Handle root specially.
if (pn->getStart() >= o_start && pn->getEnd() <= o_end)
	{
	// User is asking for the whole subtree or more.
	parent = tree;
	n_start = tree->pDown;
	n_end = n_start->last();	// Last node of phrase.
	return true;
	}
return findOffsetsRec(tree->pDown, o_start,o_end,
									/*UP*/ n_start,n_end,parent);
}


/********************************************
* FN:		FINDOFFSETSREC
* CR:		05/18/99 AM.
* SUBJ:	Traverse given subtree, looking for start,end offset.
* RET:	True if ok.
*			UP n_start,n_end - First phrase that precisely matches offsets.
*			UP parent - the parent in which the phrase occurs.
* NOTE:	RECURSIVE part of findOffsets().
********************************************/

bool Pn::findOffsetsRec(
	Node<Pn> *node,
	long o_start,		// Start offset sought.
	long o_end,			// End offset sought.
	/*UP*/
	Node<Pn>* &n_start,	// Start node that matched range.
	Node<Pn>* &n_end,		// End node that matched range.
	Node<Pn>* &parent		// Parent containing the found phrase.
	)
{
if (!node)
	return false;			// Didn't find offsets in tree.

// Node is always at the start of a list of nodes, and manages the list.

Pn *pn;
pn = node->getData();

// Move from left to right, finding the nodes that cover the phrase.
Node<Pn> *par;				// Node's parent.
Node<Pn> *l_node;			// Node that is within o_start.
Node<Pn> *r_node;			// Node that is within o_end.
par = l_node = r_node = 0;

par = node->pUp;

// Find the first point of contact, if any.
while (node && (pn = node->getData()) && (pn->getEnd() < o_start))
	node = node->Right();
if (!node)
	return false;

if (pn->getStart() < o_start)
	{
	// Node overreaches phrase on left side.
	if (pn->getEnd() >= o_end	// Overreaches or matches on right side.
		 || !(node->pRight) )	// Part of phrase zapped from tree.
		return findOffsetsRec(node->pDown, o_start,o_end,
											/*UP*/ n_start,n_end,parent);

	r_node = node->pRight;
	pn = r_node->getData();
	if (pn->getStart() <= o_end)		// Next node also in phrase.
		return false;						// Phrase is split.
	// Current node is the only node overlapping phrase, so continue,
	// assuming that part of the phrase has been zapped.
	return findOffsetsRec(node->pDown, o_start,o_end,
										/*UP*/ n_start,n_end,parent);
	}
else if (pn->getStart() == o_start)	// Exact match at left side.
	{
	if (pn->getEnd() == o_end)		// Node precisely matches entire phrase.
		{
		parent = par;
		n_start = node;
		n_end = node;
		return true;					// Done!
		}
	else if (pn->getEnd() > o_end	// Overreaches on right side.
		 || !(node->pRight) )	// Part of phrase zapped from tree.
		return findOffsetsRec(node->pDown, o_start,o_end,
											/*UP*/ n_start,n_end,parent);
	l_node = node;				// Node is within phrase.
	}
else if (pn->getStart() <= o_end)
	{
	if (pn->getEnd() >= o_end	// Overreaches or matches on right side.
		 || !(node->pRight) )	// Part of phrase zapped from tree.
		return findOffsetsRec(node->pDown, o_start,o_end,
											/*UP*/ n_start,n_end,parent);
	l_node = node;				// Node is within phrase
	}
else
	return false;		// Node overshoots the phrase entirely.

// At this point, left node is within phrase.  Keep traversing, looking
// for the right boundary node.  If a node overreaches, then phrase is
// split and we fail.  If never quite reaches, then the right side is
// assumed to be partly zapped and we succeed.
r_node = node;				// Rightmost node so far.
while ((node = node->pRight)
		 && (pn = node->getData())
		 )
	{
	if (pn->getEnd() > o_end)			// Reached end.
		{
		if (pn->getStart() <= o_end)	// Node overreaches.
			return false;					// Phrase is split. Fail.
		else									// Assume right end of phrase zapped.
			{
			n_start = l_node;
			n_end   = r_node;				// Prior node.
			parent  = par;
			return true;					// Success.
			}
		}
	else if (pn->getEnd() == o_end	// Exact match on the right.
				|| !(node->pRight))		// Last node.  Assume zapped on right.
		{
		n_start = l_node;
		n_end = node;
		parent = par;
		return true;						// Success
		}
	// Else haven't reached right boundary.  Keep going.
	r_node = node;			// Rightmost node so far.
	}

std::_t_strstream gerrStr;
gerrStr << _T("[findOffsetsRec: Error]") << std::ends;
errOut(&gerrStr,false);
return false;
}




/********************************************
* FN:		TRAVERSESETFLAGS
* CR:		05/21/01 Dd.
* NOTE:	Sets the fired flag in the entire tree
*			05/25/01 AM. Adding the BUILT flag here also. Renamed.
********************************************/

void Pn::TraverseSetFlags(
	const Node<Pn> *node,
	bool fired,																	// 05/25/01 AM.
	bool built																	// 05/25/01 AM.
	)
{
if (node == 0)
	return;

TraverseSetFlags(node->pDown, fired, built);							// 05/25/01 AM.
Pn *pn = ((Node<Pn> *)node)->getData();
pn->setFired(fired);															// 05/25/01 AM.
pn->setBuilt(built);															// 05/25/01 AM.

if (!(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		TraverseSetFlags(node, fired, built);							// 05/25/01 AM.
		}
	}
}

/******************************************************************************/
