/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AUSER.CPP
* FILE:	lite/Auser.cpp
* CR:		02/13/01 AM.
* SUBJ:	For interfacing with an analyzer's USER PROJECT.
*
*******************************************************************************/

#ifdef LINUX
#endif
#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "lite/lite.h"				// 07/03/03 AM.
#include "lite/global.h"
#include "u_out.h"		// 01/19/06 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"						// 07/07/03 AM.
#include "tree.h"						// 07/07/03 AM.
#include "pn.h"						// 07/07/03 AM.
#include "io.h"		// Linux.	// 04/26/07 AM.
#include "nlppp.h"					// 07/07/03 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "rfasem.h"
//#include "lite/global.h"
//#include "nlppp.h"					// 07/07/03 AM.
#include "parse.h"					// 07/07/03 AM.
#include "lite/inline.h"
#include "pat.h"						// 12/04/03 AM.
#include "lite/Auser.h"


int Auser::count_ = 0;

/********************************************
* FN:		Special Functions for Auser Class
********************************************/

Auser::Auser(				// Default constructor.
	Nlppp *nlppp,
	void *vtrun
	)
{
nlppp_ = nlppp;
vtrun_ = vtrun;																// 08/28/02 AM.
okret_ = false;
badname_ = true;
rettype_ = RETNULL;
retval_.ptr_ = 0;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Auser::~Auser()
{

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Auser::Auser(Auser &orig)
{
zero();
Auser::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Auser &Auser::operator=(const Auser &fm)
{
Auser *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Auser object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Auser::clear()
{
}

void Auser::zero()
{
nlppp_ = 0;
vtrun_ = 0;																		// 08/28/02 AM.
rettype_ = RETNULL;
retval_.ptr_ = 0;
okret_ = false;
badname_ = true;
}

void Auser::copy(const Auser *orig)
{
Auser *dest;
dest = this;

dest->nlppp_ = orig->nlppp_;
dest->vtrun_ = orig->vtrun_;
dest->rettype_ = orig->rettype_;
switch (orig->rettype_)
	{
	case RETCONCEPT:
		dest->retval_.kbconcept_ = orig->retval_.kbconcept_;
		break;
	case RETPHRASE:
		dest->retval_.kbphrase_ = orig->retval_.kbphrase_;
		break;
	case RETATTR:
		dest->retval_.kbattr_ = orig->retval_.kbattr_;
		break;
	case RETVAL:
		dest->retval_.kbval_ = orig->retval_.kbval_;
		break;
	case RETNODE:
		dest->retval_.node_ = orig->retval_.node_;
		break;
	case RETSTR:
		dest->retval_.str_ = orig->retval_.str_;
		break;
	case RETLONG:
		dest->retval_.long_ = orig->retval_.long_;
		break;
	case RETOSTR:
		dest->retval_.ostr_ = orig->retval_.ostr_;
		break;
	case RETBOOL:
		dest->retval_.bool_ = orig->retval_.bool_;
		break;
	case RETNULL:
	default:
		dest->retval_.ptr_ = orig->retval_.ptr_;
		break;
	}

dest->okret_ = orig->okret_;
dest->badname_ = orig->badname_;
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Auser &auser)
{
output << _T("<Auser object>");
return output;
}

/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

Nlppp *Auser::getNlppp()			{return nlppp_;}
bool	 Auser::getOkret()			{return okret_;}
bool	 Auser::getBadname()			{return badname_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Auser::setNlppp(Nlppp *x)		{nlppp_ = x;}
void	Auser::setOkret(bool x)			{okret_ = x;}
void	Auser::setBadname(bool x)		{badname_ = x;}

void	Auser::retNode(NODE *x)
{
rettype_ = RETNODE;
retval_.node_ = (Node<Pn> *)x;
}

void	Auser::retStr(_TCHAR *x)
{
rettype_ = RETSTR;
retval_.str_ = x;
}

void	Auser::retLong(long x)
{
rettype_ = RETLONG;
retval_.long_ = x;
}

void	Auser::retOstr(std::_t_ostream *x)
{
rettype_ = RETOSTR;
retval_.ostr_ = x;
}

void	Auser::retBool(bool x)
{
rettype_ = RETBOOL;
retval_.bool_ = x;
}


void	Auser::retConcept(CONCEPT *x)
{
rettype_ = RETCONCEPT;
retval_.kbconcept_ = x;
}

void	Auser::retPhrase(PHRASE *x)
{
rettype_ = RETPHRASE;
retval_.kbphrase_ = x;
}

void	Auser::retAttr(ATTR *x)
{
rettype_ = RETATTR;
retval_.kbattr_ = x;
}

void	Auser::retVal(VAL *x)
{
rettype_ = RETVAL;
retval_.kbval_ = x;
}

/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Auser::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Auser::prettyCount(std::_t_ofstream *ostr)								// 07/18/03 AM.
{
if (count_)
	{
	if (ostr)																	// 07/18/03 AM.
		*ostr << _T("Active Auser count=") << count_ << std::endl;			// 07/18/03 AM.
	*gout << _T("Active Auser count=") << count_ << std::endl;
	std::_t_strstream gerrStr;						// 02/19/02 AM.
	gerrStr << _T("Active Auser count=") << count_ << std::ends;			// 02/19/02 AM.
	errOut(&gerrStr,false);																// 02/19/02 AM.
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		GETDATA
* CR:		07/07/03 AM.
* SUBJ:	Get a doubly-linked liste element's data.
* RET:	arg - Iarg.
* NOTE:	To keep template classes out of API.
********************************************/

Iarg *Auser::getData(DELTS *args)
{
return ((Delt<Iarg> *)args)->getData();
}


/********************************************
* FN:		LEFT
* CR:		07/07/03 AM.
* SUBJ:	Get a doubly-linked list element's left.
* RET:	arg - Iarg.
* NOTE:	To keep template classes out of API.
********************************************/

DELTS *Auser::Left(DELTS *args)
{
return ((Delt<Iarg> *)args)->Left();
}


/********************************************
* FN:		RIGHT
* CR:		07/07/03 AM.
* SUBJ:	Get a doubly-linked list element's right.
* RET:	arg - Iarg.
* NOTE:	To keep template classes out of API.
********************************************/

DELTS *Auser::Right(DELTS *args)
{
return ((Delt<Iarg> *)args)->Right();	// FIX.	// 07/10/03 AM.
}

/********************************************
* FN:		GETPARSE
* CR:		07/07/03 AM.
* SUBJ:	Get current parse object.
* RET:	arg - Iarg.
* NOTE:	To keep template classes out of API.
********************************************/

Parse *Auser::getParse(Nlppp *nlppp)
{
return nlppp->getParse();
}

/********************************************
* FN:		GETANA
* CR:		07/07/03 AM.
* SUBJ:	Get current analyzer definition object.
* RET:	ana
* NOTE:	To keep template classes out of API.
********************************************/

Ana *Auser::getAna(Parse *parse)
{
return parse->getAna();
}

/********************************************
* FN:		GETNODEDATA
* CR:		07/07/03 AM.
* SUBJ:	Get node data.
* RET:	pn
* NOTE:	To keep template classes out of API.
********************************************/

Pn *Auser::getNodedata(NODE *node)
{
return ((Node<Pn> *)node)->getData();
}

/********************************************
* FN:		GETROOT
* CR:		07/07/03 AM.
* SUBJ:	Get parse tree root.
* RET:	node
* NOTE:	To keep template classes out of API.
********************************************/

NODE *Auser::getRoot(TREE *tree)
{
return ((Tree<Pn> *)tree)->getRoot();
}

/********************************************
* FN:		GETNODEDOWN
* CR:		12/04/03 AM.
* SUBJ:	Get node's down link.
* RET:	node
* NOTE:	To keep template classes out of API.
********************************************/

NODE *Auser::getNodedown(NODE *node)
{
return ((Node<Pn> *)node)->Down();
}

/********************************************
* FN:		GETNODERIGHT
* CR:		12/04/03 AM.
* SUBJ:	Get node's right link.
* RET:	node
* NOTE:	To keep template classes out of API.
********************************************/

NODE *Auser::getNoderight(NODE *node)
{
return ((Node<Pn> *)node)->Right();
}

/********************************************
* FN:		GETNODELEFT
* CR:		12/04/03 AM.
* SUBJ:	Get node's left link.
* RET:	node
* NOTE:	To keep template classes out of API.
********************************************/

NODE *Auser::getNodeleft(NODE *node)
{
return ((Node<Pn> *)node)->Left();
}

/********************************************
* FN:		GETNODEUP
* CR:		12/04/03 AM.
* SUBJ:	Get node's up link.
* RET:	node
* NOTE:	To keep template classes out of API.
********************************************/

NODE *Auser::getNodeup(NODE *node)
{
return ((Node<Pn> *)node)->Up();
}


/********************************************
* FN:		REDSINGLE
* CR:		12/04/03 AM.
* SUBJ:	Manual reduction in parse tree.
* RET:	node
********************************************/

NODE *Auser::redsingle(
	NODE *node1,	// First node in range.
	NODE *node2,	// Last node in range.
	_TCHAR *s_sugg,	// Suggested concept name.
	bool base,
	bool unsealed,
	Nlppp *nlppp
	)
{
if (!node1 || !node2 || empty(s_sugg))
	return 0;
return Pat::redsingle(
	(Node<Pn>*)node1,
	(Node<Pn>*)node2,
	s_sugg,base,unsealed,nlppp);
}

/*************************      END OF FILE      ******************************/
