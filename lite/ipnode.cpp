/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IPNODE.CPP
* FILE:	lite/ipnode.cpp
* CR:		10/18/00 AM.
* SUBJ:	Interpreted/internal parse-tree-node semantic object. NLP++ semantics.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"
#include "lite/mach.h"
#include "io.h"
#include "iarg.h"
#include "str.h"
#include "chars.h"
#include "node.h"	// 07/07/03 AM.
//#include "thtab.h"		// 02/26/01 AM.
#include "lite/slist.h"	// 02/26/01 AM.
#include "tree.h"
#include "nlppp.h"	// 07/07/03 AM.
//#include "lite/node.h"
#include "pn.h"
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "thtab.h"		// 02/26/01 AM.
#include "var.h"
#include "pat.h"
//#include "gen.h"
#include "rfasem.h"
#include "parse.h"
#include "lite/Arun.h"	// To access runtime collect.
#include "ipnode.h"

// WARN:	Ipnodetype and s_Ipnodetype must be kept in sync.
_TCHAR *s_Ipnodetype[] =
	{
	0,
	_T("X"),
	_T("S"),
	_T("N"),
	_T("runtime"),
	_T("dummy")
	};

int Ipnode::count_ = 0;

/********************************************
* FN:		Special Functions for Ipnode Class
********************************************/

Ipnode::Ipnode(				// Default constructor.
	enum Ipnodetype typ,		// Variable type.
	long num						// Variable num.
	)
{
type_ = typ;
num_  = num;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ipnode::~Ipnode()
{

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Ipnode::Ipnode(Ipnode &orig)
{
zero();
Ipnode::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Ipnode &Ipnode::operator=(const Ipnode &fm)
{
Ipnode *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ipnode object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Ipnode::clear()
{
}

void Ipnode::zero()
{
type_ = NULLPNODE;
num_  = 0;
}

void Ipnode::copy(const Ipnode *orig)
{
Ipnode *dest;
dest = this;

dest->type_ = orig->type_;
dest->num_  = orig->num_;
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ipnode &ipnode)
{
switch (ipnode.type_)
	{
	case CONTEXTPNODE:
		if (!ipnode.num_)
			output << _T("X()");
		else
			output << _T("X(") << ipnode.num_ << _T(")");
		break;
	case NODEPNODE:
		if (!ipnode.num_)
			output << _T("N()");
		else
			output << _T("N(") << ipnode.num_ << _T(")");
		break;
	case SUGGESTEDPNODE:
		output << _T("S()");
		break;
	case NULLPNODE:
	default:
		output << _T("<BAD PNODE>");
		break;
	}
return output;
}

/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

enum Ipnodetype	Ipnode::getType()	{return type_;}
long					Ipnode::getNum()	{return num_; }

/********************************************
* FN:		Modify Functions
********************************************/

void	Ipnode::setType(enum Ipnodetype x)	{type_  = x;}
void	Ipnode::setNum(long x)					{num_   = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ipnode::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ipnode::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ipnode count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ipnode count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENPNODE
* CR:		11/09/99 AM.
* SUBJ:	Generate var to a rules file.
*********************************************/
void Ipnode::genPnode(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		10/18/00 AM.
* SUBJ:	See if two ipnodes are identical in form.
*********************************************/

bool Ipnode::same(Ipnode *var1, Ipnode *var2)
{
if (!var1 && !var2)
	return true;
if (!var1 || !var2)
	return false;

if (var1->type_ != var2->type_)
	return false;

// WARN: Requiring types that don't care about num to have num==0.
// Zero in NODE or CONTEXT means last.
if (var1->num_ != var2->num_)
	return false;

return true;
}


/********************************************
* FN:		PNODETYPE
* CR:		10/18/00 AM.
* SUBJ:	Convert enum to str and vice versa.
*********************************************/

enum Ipnodetype Ipnode::Ipnodetype(_TCHAR ch)
{
switch (ch)
	{
	case 'X':	return CONTEXTPNODE;
	case 'S':	return SUGGESTEDPNODE;
	case 'N':	return NODEPNODE;
	default:		return NULLPNODE;
	}
}

_TCHAR Ipnode::Ipnodetype(enum Ipnodetype typ)
{
switch (typ)
	{
	case CONTEXTPNODE:	return 'X';
	case SUGGESTEDPNODE:	return 'S';
	case NODEPNODE:		return 'N';
	case NULLPNODE:		return '\0';
	default:					return '\0';
	}
}


/********************************************
* FN:		EVAL
* CR:		10/18/00 AM.
* SUBJ:	Eval ipnode, return RFASem type for node.
* RET:	True if ok.
*********************************************/

bool Ipnode::eval(
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Node<Pn> *node = 0;
if (eval(type_,num_,nlppp,/*UP*/node))
	sem = new RFASem(node);
return true;
}

/********************************************
* FN:		EVAL
* CR:		10/18/00 AM.
* SUBJ:	Eval ipnode, return parse tree node.
* RET:	True if ok.
*			UP pnode - parse tree node.
* NOTE:	Used by compiled runtime system.
*			Taken from Ipnode::make().
*********************************************/

bool Ipnode::eval(
	enum Ipnodetype typ,
	int num,
	Nlppp *nlppp,
	/*UP*/
	Node<Pn>* &pnode
	)
{
pnode = 0;
Node<Pn> *select, *tmp;
Node<Pn> *nstart, *nend;
COLL *coll;
switch (typ)
	{
	case CONTEXTPNODE:
		if (!(select = nlppp->getSelect()))
			return false;
		if (!num)						// LAST component of path.
			pnode = select;
		else								// Get component.
			{
			// Recurse to the root, then count down.
			tmp = select->pathNth(num);
			if (!tmp)
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Couldn't find node X(") << num
									<< _T(").]") << std::ends;
				errOut(&gerrStr,false);
				return false;
				}
			pnode = tmp;
			}
		break;
	case SUGGESTEDPNODE:
		// May not exist.  May want to modify pattern matcher to create a
		// "premature" or temporary suggested node.
		pnode = nlppp->getRed();
		break;
	case NODEPNODE:
		// Get node from collect.
		nstart = nend = 0;
		if (!Pat::collectNthnew(
									nlppp->getCollect(), num, /*UP*/ nstart, nend)
			 || !nstart || !nend)
			return false;
		//if (nstart != nend)
		//	return false;	// Elt matched multiple nodes.

		pnode = nstart;	// TAKE THE FIRST NODE OF LIST THAT MATCHED ELT.
		break;
	case RUNNODEPNODE:			// COMPILED RUNTIME NODEPNODE.
		// Get node from collect.
		if (!num)
			num = nlppp->len_;	// If 0, get the last coll.
		coll = &(nlppp->coll_[num]);
		nstart = (Node<Pn> *) coll->start;
		nend = (Node<Pn> *) coll->end;
		if (!nstart || !nend)
			return false;
		//if (nstart != nend)
		//	return false;	// Elt matched multiple nodes.

		pnode = nstart;	// TAKE THE FIRST NODE OF LIST THAT MATCHED ELT.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[makePnode: Bad type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}

return true;
}


/********************************************
* FN:		GENEVAL
* CR:		10/18/00 AM.
* SUBJ:	Gen evaluation of an NLP++ var.
*********************************************/

bool Ipnode::genEval(Gen *gen)
{
std::_t_ostream *fcode = gen->passc_;	// 04/03/09 AM.
long num = getNum();
*fcode << _T("Arun::");
switch (getType())
	{
	case CONTEXTPNODE:
		*fcode << _T("x(") << num << _T(",nlppp)");
		break;
	case SUGGESTEDPNODE:
		*fcode << _T("s(nlppp)");
		break;
	case NODEPNODE:
		*fcode << _T("n(") << num << _T(",nlppp)");
		break;
	default:
		*fcode << _T("ERROR();") << std::endl;										// 05/04/01 AM.
		*fcode << _T("// [ipnode: Bad node type.]") << std::endl;			// 05/04/01 AM.
		return false;
	}
return true;
}

/*************************      END OF FILE      ******************************/
