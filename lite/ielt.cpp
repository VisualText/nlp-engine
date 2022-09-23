/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IELT.CPP
* FILE:	lite\ielt.cpp
* CR:		10/24/98 AM.
* SUBJ:	Interpreted/internal phrase element class.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"		// 05/19/99 AM.
#include "lite/iarg.h"	// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "chars.h"			// 03/27/05 AM.
#include "xml.h"				// 03/27/05 AM.
#include "starr.h"		// 05/06/00 AM.
#include "seqn.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"			// 05/08/00 AM.
#include "ipre.h"
#include "htab.h"			// 12/12/98 AM.
#include "tsym.h"			// 12/17/98 AM. Testing template tSym class.
#include "thtab.h"		// 12/17/98 AM. Testing template tHtab class.
#include "irule.h"		// 12/19/98 AM.
#include "lite/Arun.h"	// 06/09/00 AM.
#include "lite/parse.h"	// 08/23/02 AM.
#include "ielt.h"


#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Ielt>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Delt<Ielt>::count_ = 0;
#else
int Dlist<Ielt>::count_ = 0;
int Delt<Ielt>::count_ = 0;
#endif

// 12/12/98 AM. List length threshold for creating a hash table.
#define HTHRESH 10

// 11/23/98 AM. Part of bit-flags implementation.
// WARN! Numbers are octal.
static const int bitSINGLET  =  01;	// Elt's SINGLET attribute.
static const int bitTRIGGER  =  02;	// Elt's TRIGGER attribute.
static const int bitTREE     =  04;	// TREE search attr.		 // 03/17/99 AM
static const int bitXSINGLET = 010;	// Extract Singlet attr. // 03/24/99 AM.
static const int bitXTREE	  = 020;	// Extract Tree    attr. // 03/24/99 AM.
static const int bitCLOSED   = 040;	// RUG.  Closed set.		 // 07/14/99 AM.
static const int bitLOOKAHEAD = 0100;	// Lookahead elt.		 // 11/19/99 AM.
static const int bitDEACCENT = 0200;	// Deaccented match.  // 01/28/05 AM.
static const int bitDEJUNK = 0400;		// Dejunk match.      // 09/09/11 AM.
// 02, 04, 010, 020, 040, 0100...



/********************************************
* FN:		Special Functions for Class
* CR:		10/24/98 AM.
********************************************/

Ielt::Ielt(				// Default constructor.
	_TCHAR *nn,						// Element name.
	Dlist<Ipair> *prs,				// Pairs.
	long num							// Line num in pass file.			// 08/08/01 AM.
	)
	: Ielement(nn, prs,
	   0, num)																	// 08/08/01 AM.
{
min_ = max_ = -1;			// Undefined value for min,max.
rename_ = 0;
group_	= 0;				// 09/23/00 AM.
//singlet_ = false;		// 11/23/98 AM.
//ruleset_ = 0;
passes_ = 0;
matches_ = fails_ = excepts_ = 0;
hmatches_ = hfails_ = hexcepts_ = 0;
flags_ = 0;					// 11/23/98 AM.
pres_ = 0;					// 11/29/98 AM.
layers_ = 0;				// 05/06/00 AM.

// Tracking is in the base class, Ielement.
}

/*******************************************/

Ielt::~Ielt()
{
// Note: Ielement destructor called automatically.

// Delete stuff owned by the Ielt object.
Ielt::clear();

// Tracking is in the base class, Ielement.
}

/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Ielt::Ielt(Ielt &orig)
: Ielement(orig)			// Call parent class' copy constructor.
{
zero();
Ielt::copy(&orig);
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Ielt &Ielt::operator=(const Ielt &fm)
{
Ielt *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ielt object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

// Let the parent do its assignments.
to->Ielement::operator =(fm);

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}

/*******************************************/
/*******************************************/
void Ielt::clear()
{
// 07/02/99 AM. Moved stuff from destructor to here.
// WARN: DOESN'T CLEAR FOR THE PARENT CLASS.

// 11/17/98 AM.
if (passes_)
	Dlist<Seqn>::DeleteDlistAndData(passes_);
if (matches_)
	Dlist<Iarg>::DeleteDlistAndData(matches_);
if (fails_)
	Dlist<Iarg>::DeleteDlistAndData(fails_);
if (excepts_)
	Dlist<Iarg>::DeleteDlistAndData(excepts_);

// DELETE COPY OF PRE ACTIONS.			// 11/29/98 AM.
if (pres_)
	Dlist<Ipre>::DeleteDlistAndData(pres_);

// 12/12/98 AM. Delete any hash tables.
if (hmatches_)
	delete hmatches_;
if (hfails_)
	delete hfails_;
if (hexcepts_)
	delete hexcepts_;			// 10/13/99 AM.

if (layers_)					// 05/06/00 AM.
	delete layers_;			// 05/06/00 AM.
}

void Ielt::zero()
{
// WARN: DOESN'T ZERO FOR THE PARENT CLASS.
min_ = 0;
max_ = 0;
rename_ = 0;
group_	= 0;										// 09/23/00 AM.
passes_ = 0;
matches_ = fails_ = excepts_ = 0;			// 10/13/99 AM.
hmatches_ = hfails_ = hexcepts_ = 0;		// 10/13/99 AM.
flags_ = 0;

pres_ = 0;
layers_ = 0;										// 05/06/00 AM.
}

void Ielt::copy(const Ielt *orig)
{
// WARN: DOESN'T COPY THE PARENT CLASS.
Ielt *dest;
dest = this;

//dest->name_ = orig->name_;
dest->min_ = orig->min_;
dest->max_ = orig->max_;
dest->rename_ = orig->rename_;
dest->group_	= orig->group_;											// 09/23/00 AM.
if (orig->passes_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Ielt: NOT COPYING SEQUENCE. Unimplemented for now.]") << std::ends;
	errOut(&gerrStr,false);

	dest->passes_ = 0;
	//dest->passes_ = new Dlist<Seqn>(*(orig->passes_));
	}

if (orig->matches_)
	dest->matches_ = new Dlist<Iarg>(*(orig->matches_));
if (orig->fails_)
	dest->fails_ = new Dlist<Iarg>(*(orig->fails_));
if (orig->excepts_)
	dest->excepts_ = new Dlist<Iarg>(*(orig->excepts_));

// Not doing the hash tables.
// orig->hmatches_;
// orig->hfails_;
// orig->hexcepts_;

dest->flags_ = orig->flags_;

if (orig->pres_)
	dest->pres_ = new Dlist<Ipre>(*(orig->pres_));

dest->layers_ = orig->layers_;						// 05/06/00 AM.
	// Note: Doesn't copy the strings, only ptrs to them.
}

/*******************************************/


/*******************************************/


std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ielt &elt)
{
// 11/11/98 AM. Printing some internal data.
bool flag = false;				// If something prior was printed.
_TCHAR buf[MAXSTR];
long size = MAXSTR;

#ifdef UNICODE
char *lpstr8;																	// 01/27/06 AM.
#endif

if (!empty(elt.name_))
	{
#ifndef UNICODE
	output << pretty_str(elt.name_, buf, size);	// 11/24/98 AM.
#else
	u_to_mbcs((LPCWSTR)pretty_str(elt.name_,buf,size),					// 01/27/06 AM.
												 CP_UTF8, (LPCTSTR*&)lpstr8);	// 01/27/06 AM.
	output << lpstr8;																// 01/27/06 AM.
	u_delete((LPCTSTR*&)lpstr8);												// 01/27/06 AM.
#endif
	}
else
	output << _T("<NIL>");

output << _T(" [");
output << elt.min_ << _T(",") << elt.max_;
flag = true;

if (elt.layers_)						// 05/06/00 AM.
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("attrs=(");		// RIGHT???	// 01/27/06 AM.
	output << *(elt.layers_) << _T(")");
	}
else if (elt.attrs_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("attrs=(") << *(elt.attrs_) << _T(")");
	}
if (elt.rename_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("rename=");
#ifndef UNICODE
	output << pretty_str(elt.rename_, buf, size);	// 11/24/98 AM.
#else
	u_to_mbcs((LPCWSTR)pretty_str(elt.rename_,buf,size),				// 01/27/06 AM.
												CP_UTF8, (LPCTSTR*&)lpstr8);	// 01/27/06 AM.
	output << lpstr8;																// 01/27/06 AM.
	u_delete((LPCTSTR*&)lpstr8);												// 01/27/06 AM.
#endif
	}
if (elt.group_)																// 09/23/00 AM.
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("group=");
#ifndef UNICODE
	output << pretty_str(elt.group_, buf, size);						// 09/23/00 AM.
#else
	u_to_mbcs((LPCWSTR)pretty_str(elt.group_,buf,size),				// 01/27/06 AM.
												CP_UTF8, (LPCTSTR*&)lpstr8);	// 01/27/06 AM.
	output << lpstr8;																// 01/27/06 AM.
	u_delete((LPCTSTR*&)lpstr8);												// 01/27/06 AM.
#endif
	}
if (elt.getSinglet())
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("singlet");
	}
if (elt.getLookahead())
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("lookahead");
	}
if (elt.getXsinglet())
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("xs");
	}
if (elt.getTree())
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("tree");
	}
if (elt.getXtree())
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("xt");
	}
if (elt.getTrigger())
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("trigger");
	}
if (elt.getDejunk())	// 09/09/11 AM.
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("dejunk");
	}
else if (elt.getDeaccent())	// 01/28/05 AM.
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("deaccent");
	}
if (elt.getPres())
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("pre");
	}
if (elt.passes_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	//output << "passes=(" << *(elt.passes_) << ")";
	output << _T("passes");
	}
if (elt.matches_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("matches=(") << *(elt.matches_) << _T(")");
	}
if (elt.fails_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("fails=(") << *(elt.fails_) << _T(")");
	}
if (elt.excepts_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("excepts=(") << *(elt.excepts_) << _T(")");
	}
if (elt.hmatches_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("hmatch");
	}
if (elt.hfails_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("hfail");
	}
if (elt.hexcepts_)
	{
	if (flag)
		output << _T(" ");
	else
		flag = true;
	output << _T("hexcept");
	}
output << _T("]");

output << _T(" [");
if (elt.pairs_)
	output << *(elt.pairs_);
output << _T("]");
return output;
}

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Delt<Ielt> &delt)
{
Ielt *elt;
elt  = delt.getData();
output << *elt;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Ielt> &list)
{
Delt<Ielt> *delt;
delt = list.getFirst();
output << *delt;
while ((delt = delt->Right()))
	{
	output << _T(" ") << *delt;
	}
return output;
}


/********************************************
* FN:		Access Functions
* CR:		10/24/98 AM.
********************************************/

int			  Ielt::getMin()		{return min_;	  }		// 10/30/98 AM.
int			  Ielt::getMax()		{return max_;	  }		// 10/30/98 AM.
_TCHAR			 *Ielt::getRename()	{return rename_; }		// 11/02/98 AM.
_TCHAR			 *Ielt::getGroup()	{return group_;  }		// 09/23/00 AM.
//Dlist<Irule> *Ielt::getRuleset()	{return ruleset_;}	// 11/08/98 AM.
Dlist<Seqn>  *Ielt::getPasses()	{return passes_; }		// 11/09/98 AM.
Dlist<Iarg>	 *Ielt::getMatches()	{return matches_;}		// 11/09/98 AM.
Dlist<Iarg>	 *Ielt::getFails()	{return fails_;  }		// 11/09/98 AM.
Dlist<Iarg>	 *Ielt::getExcepts()	{return excepts_;}		// 10/13/99 AM.
Dlist<Ipre>	 *Ielt::getPres()		{return pres_;	  }		// 11/29/98 AM.
Htab			 *Ielt::getHmatches()	{return hmatches_;}	// 12/12/98 AM.
Htab			 *Ielt::getHfails()		{return hfails_;}		// 12/12/98 AM.
Htab			 *Ielt::getHexcepts()	{return hexcepts_;}	// 10/13/99 AM.
int			  Ielt::getFlags()		{return flags_;}		// 06/07/99 AM.
Starr			 *Ielt::getLayers()		{return layers_;}		// 05/06/00 AM.

bool Ielt::getSinglet()
{
//return singlet_;					// 11/23/98 AM.
if (flags_ & bitSINGLET)			// 11/23/98 AM.
	return true;
return false;
}

bool Ielt::getTrigger()				// 11/23/98 AM.
{
if (flags_ & bitTRIGGER)
	return true;
return false;
}

bool Ielt::getTree()					// 03/17/99 AM.
{
if (flags_ & bitTREE)
	return true;
return false;
}

bool Ielt::getXsinglet()			// 03/24/99 AM.
{
if (flags_ & bitXSINGLET)
	return true;
return false;
}

bool Ielt::getXtree()				// 03/24/99 AM.
{
if (flags_ & bitXTREE)
	return true;
return false;
}

bool Ielt::getClosed()				// 07/14/99 AM.
{
if (flags_ & bitCLOSED)
	return true;
return false;
}

bool Ielt::getLookahead()			// 11/19/99 AM.
{
if (flags_ & bitLOOKAHEAD)
	return true;
return false;
}

bool Ielt::getDeaccent()			// 01/28/05 AM.
{
if (flags_ & bitDEACCENT)
	return true;
return false;
}


bool Ielt::getDejunk()			// 09/09/11 AM.
{
if (flags_ & bitDEJUNK)
	return true;
return false;
}

// Static function for use by compiled version.	// 03/28/05 AM.
bool Ielt::getDeaccent(int flags)
{
if (flags & bitDEACCENT)
	return true;
return false;
}

// Static function for use by compiled version.	// 09/09/11 AM.
bool Ielt::getDejunk(int flags)
{
if (flags & bitDEJUNK)
	return true;
return false;
}

/********************************************
* FN:		Modify Functions
* CR:		10/24/98 AM.
********************************************/

void Ielt::setMin(int x)					{min_		  = x;}
void Ielt::setMax(int x)					{max_		  = x;}
void Ielt::setRename(_TCHAR *x)				{rename_	  = x;}
void Ielt::setGroup(_TCHAR *x)				{group_	  = x;}		// 09/23/00 AM.
//void Ielt::setRuleset(Dlist<Irule> *x)	{ruleset_ = x;}	// 11/08/98 AM.
void Ielt::setPasses(Dlist<Seqn> *x)	{passes_	  = x;}	// 11/09/98 AM.
void Ielt::setMatches(Dlist<Iarg> *x)	{matches_  = x;}	// 11/09/98 AM.
void Ielt::setFails(Dlist<Iarg> *x)		{fails_	  = x;}	// 11/09/98 AM.
void Ielt::setExcepts(Dlist<Iarg> *x)	{excepts_  = x;}	// 10/13/99 AM.
void Ielt::setPres(Dlist<Ipre>	 *x)	{pres_	  = x;}	// 11/29/98 AM.
void Ielt::setHmatches(Htab *x)			{hmatches_ = x;}	// 12/12/98 AM.
void Ielt::setHfails(Htab *x)				{hfails_	  = x;}	// 12/12/98 AM.
void Ielt::setHexcepts(Htab *x)			{hexcepts_ = x;}	// 10/13/99 AM.
void Ielt::setLayers(Starr *x)			{layers_   = x;}	// 05/06/00 AM.

void Ielt::setSinglet(bool x)
{
//singlet_ = x;					// 11/23/98 AM.
if (x)
	flags_ |= bitSINGLET;
else
	flags_ &= (~bitSINGLET);
}

void Ielt::setTrigger(bool x)	// 11/23/98 AM.
{
// NOTE: An informational function only. Not used by the analyzer,
// but for pretty-printing.  Irule has the trigger element pointer
// used by the analyzer.	11/23/98 AM.
if (x)
	flags_ |= bitTRIGGER;
else
	flags_ &= (~bitTRIGGER);
}

void Ielt::setTree(bool x)		// 03/17/99 AM.
{
if (x)
	flags_ |= bitTREE;
else
	flags_ &= (~bitTREE);
}

void Ielt::setXsinglet(bool x)	// 03/24/99 AM.
{
if (x)
	flags_ |= bitXSINGLET;
else
	flags_ &= (~bitXSINGLET);
}


void Ielt::setXtree(bool x)		// 03/24/99 AM.
{
if (x)
	flags_ |= bitXTREE;
else
	flags_ &= (~bitXTREE);
}

void Ielt::setClosed(bool x)		// 07/14/99 AM.
{
if (x)
	flags_ |= bitCLOSED;
else
	flags_ &= (~bitCLOSED);
}

void Ielt::setLookahead(bool x)		// 11/19/99 AM.
{
if (x)
	flags_ |= bitLOOKAHEAD;
else
	flags_ &= (~bitLOOKAHEAD);
}

void Ielt::setDeaccent(bool x)		// 01/28/05 AM..
{
if (x)
	flags_ |= bitDEACCENT;
else
	flags_ &= (~bitDEACCENT);
}


void Ielt::setDejunk(bool x)		// 09/09/11 AM..
{
if (x)
	flags_ |= bitDEJUNK;
else
	flags_ &= (~bitDEJUNK);
}

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		getMinmax
* CR:		10/06/99 AM.
* SUBJ:	Get min and max values.
* RET:	True if values present in both.
* OPT:	Optimization function, gets both values in one call.
********************************************/

bool Ielt::getMinmax(
	/*UP*/
	int &min,
	int &max
	)
{
if ((min = min_) >= 0
    && (max = max_) >= 0)
   return true;
return false;
}


/********************************************
* FN:		makeDelt
* CR:		10/24/98 AM.
* SUBJ:	Create a list element with appropriate data.
* NOTE:	Must be a more elegant way to initialize the elt.
********************************************/
//static Ielt obj;			// 07/02/99 AM.

Delt<Ielt> *Ielt::makeDelt(
	_TCHAR *nn,				// Name.
	Dlist<Ipair> *prs,		// Pairs.
	int min,
	int max,
	_TCHAR *rename
	)
{
Delt<Ielt> *delt;

if (max && min > max)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Warning: Min greater than max.]") << std::ends;
	errOut(&gerrStr,false);
	}


// 07/02/99 AM. NEW WAY.
Ielt *obj;				// 07/02/99 AM.
obj = new Ielt();
obj->name_ = nn;
obj->pairs_ = prs;
obj->min_ = min;
obj->max_ = max;
obj->rename_ = rename;

delt = new Delt<Ielt>(obj);
return delt;

// 07/02/99 AM. OLD WAY.
// Hack. Keep refilling same static Ielement, because it will be copied.
//obj.name_		= nn;
//obj.pairs_	= prs;
//obj.min_		= min;
//obj.max_		= max;
//obj.rename_ = rename;

// obj.ruleset_ = 0;
// obj.passes_  = 0;
// obj.matches_ = 0;
// obj.fails_	 = 0;
// obj.excepts_ = 0;
// obj.pres_	 = 0;
// obj.hmatches_ = 0;
// obj.hfails_ = 0;
// obj.hexcepts_ = 0;

//delt = new Delt<Ielt>(obj);

//obj.pairs_ = 0;

//return delt;
}


/********************************************
* FN:		makeDlist
* CR:		10/24/98 AM.
* SUBJ:	Create a list with appropriate data.
* NOTE:	Class member function.
*			(Can be called without an instance existing).
********************************************/

Dlist<Ielt> *Ielt::makeDlist(
	_TCHAR *nn,					// Name.
	Dlist<Ipair> *prs,		// Pairs.
	int min,
	int max,
	_TCHAR *rename
	)
{
Delt<Ielt> *delt;
delt = makeDelt(nn, prs, min, max, rename);
return new Dlist<Ielt>(delt);
}


/********************************************
* FN:		addDelt
* CR:		10/24/98 AM.
* SUBJ:	Add new element to given list.
* NOTE:	Must be a more elegant way to initialize the elt.
*			Class member function. Need not be called with object.
********************************************/
//static Ielt obj;

Delt<Ielt> *Ielt::addDelt(
	Dlist<Ielt> *list,			// List to add element to.
	_TCHAR *nn,						// Name.
	Dlist<Ipair> *prs,				// Pairs.
	int min,
	int max,
	_TCHAR *rename
	)
{
Delt<Ielt> *delt;

if (max && min > max)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Warning: Min greater than max.]") << std::ends;
	errOut(&gerrStr,false);
	}


// 07/02/99 AM. NEW WAY.
Ielt *obj;
obj = new Ielt();

// Hack. Keep refilling same static Ielement, because it will be copied.
obj->name_	= nn;
obj->pairs_	= prs;
obj->min_		= min;
obj->max_		= max;
obj->rename_ = rename;
// obj.ruleset_ = 0;
// obj.passes _ = 0;
// obj.matches_ = 0;
// obj.fails_	 = 0;
// obj.excepts_ = 0;
// obj.pres_	 = 0;
// obj.hmatches_ = 0;
// obj.hfails_ = 0;
// obj.hexcepts_ = 0;

delt = new Delt<Ielt>(obj);

//obj.pairs_ = 0;			// 07/02/99 AM.

list->rpush(delt);
return delt;			// In case anyone wants it.
}


/********************************************
* FN:		INTERN
* CR:		11/22/98 AM.
* SUBJ:	Internalize the pairs data in a rule elt.
* NOTE:	Need min,max,opt flags to check redundancy. 03/22/99 AM.
********************************************/

void Ielt::intern(
	Dlist<Seqn> *seqs,			// For binding recursive rule calls.
	Dlist<Ipre> *pres,			// For binding pre actions.
	int ord,							// Current element number in rule's phrase.
	Parse *parse,																// 08/23/02 AM.
	/*UP*/
	bool &f_trig,					// Use this to flag trigger elt.
	bool &f_look,					// True if lookahead elt.			// 11/22/99 AM.
	bool fhash						// If hashing rules&elts.
	)
{
Ielt *elt;

parse->inputline_ = getLine();											// 08/24/02 AM.

f_look = f_trig = false;			// 11/22/99 AM.
elt = this;

bool fmin=false, fmax=false, frep=false;	// 03/22/99 AM.

// Interning attrs_ list into layers_ list.						// 05/06/00 AM.
if (elt->layers_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: elt already interned.]") << std::ends;
	parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
	return;
	}

// 11/29/98 AM.
// Find the pre actions to be executed by this rule element.
// Copy them into the element's list.
elt->internPres(pres, ord);	// 07/13/99 AM.

Dlist<Ipair> *list;
list = elt->getPairs();
if (!list)
	{
	// SET DEFAULTS HERE.
	elt->fillDefaults();
	return;
	}

Delt<Ipair> *dpair;
// Need to get the elt name, for setting defaults.

// For each pair.
for (dpair = list->getFirst(); dpair; dpair = dpair->Right())
	{
	// See if it's something we know about.
	// Watch out for sugg vs elt pairs.
	Ipair *pair;
	pair = dpair->getData();
	assert(pair);
	_TCHAR *key;
	key = pair->getKey();
	Dlist<Iarg> *vals;
	vals = pair->getVals();
	if (!strcmp_i(key, _T("trigger"))
		 || !strcmp_i(key, _T("trig"))
		 || !strcmp_i(key, _T("t"))
		 )
		{
		elt->setTrigger(true);		// Informational.			// 11/23/98 AM.
		f_trig = true;					// Current elt is trigger.
		}
	else if (!strcmp_i(key, _T("min")))
		{
		// Get one value.  Should be string with numeric value.
		assert(vals);
		Delt<Iarg> *darg;
		darg = vals->getFirst();
		Iarg *iarg;
		iarg = darg->getData();
		long num;
		bool ok = true;						// 11/15/99 AM.
		switch (iarg->getType())			// 11/15/99 AM.
			{
			case IANUM:							// 11/15/99 AM.
				num = iarg->getNum();		// 11/15/99 AM.
				break;
			case IASTR:							// 11/15/99 AM.
				ok = str_to_long(iarg->getStr(), num);
				break;
			default:
				ok = false;
				break;
			}
		if (ok && (num >= 0))				// 11/15/99 AM.
			{
			if (!fmin && !frep)				// 03/22/99 AM.
				{
				elt->min_ = num;
				fmin = true;					// 03/22/99 AM.
				}
			else if (elt->min_ == num)
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Redundant MIN key. Ignoring.]") << std::ends;
//				errOut(false,parse->getInputpass());	// 04/03/03 AM.
				parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
				}

			else
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[MIN key conflicts with prior settings.]") << std::ends;
				parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
				}

			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[intern: Error with min key's value.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		}
	else if (!strcmp_i(key, _T("max")))
		{
		// Get one value.  Should be string with numeric value.
		assert(vals);
		Delt<Iarg> *darg;
		darg = vals->getFirst();
		Iarg *iarg;
		iarg = darg->getData();
		long num;
		bool ok = true;						// 11/15/99 AM.
		switch (iarg->getType())			// 11/15/99 AM.
			{
			case IANUM:							// 11/15/99 AM.
				num = iarg->getNum();		// 11/15/99 AM.
				break;
			case IASTR:							// 11/15/99 AM.
				ok = str_to_long(iarg->getStr(), num);
				break;
			default:
				ok = false;
				break;
			}
		if (ok && (num >= 0))				// 11/15/99 AM.
			{
			if (!fmax && !frep)			// 03/22/99 AM.
				{
				elt->max_ = num;
				fmax = true;				// 03/22/99 AM.
				}
			else if (elt->max_ == num)
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Redundant MAX key. Ignoring.]") << std::ends;
				parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
				}
			else
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[MAX key conflicts with prior settings.]") << std::ends;
				parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
				}
			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[intern: Error with max key's value.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		}
	else if (!strcmp_i(key, _T("optional"))	// 03/22/99 AM.
				|| !strcmp_i(key, _T("option"))
				|| !strcmp_i(key, _T("opt"))
				|| !strcmp_i(key, _T("o"))
				)
		{
		if (!fmin && !fmax && !frep)
			{
			elt->min_ = 0;
			elt->max_ = 1;
			frep = true;
			}
		else if (elt->min_ == 0 && elt->max_ == 1)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[OPTIONAL key is redundant. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[OPTIONAL key clashes with prior settings. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		}
	else if (!strcmp_i(key, _T("star"))	// 03/22/99 AM.
				|| !strcmp_i(key, _T("*"))
				)
		{
		if (!fmin && !fmax && !frep)
			{
			elt->min_ = 0;
			elt->max_ = 0;
			frep = true;
			}
		else if (elt->min_ == 0 && elt->max_ == 0)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[STAR key is redundant. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[STAR key clashes with prior settings. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		}
	else if (!strcmp_i(key, _T("plus"))	// 03/22/99 AM.
				|| !strcmp_i(key, _T("+"))
				)
		{
		if (!fmin && !fmax && !frep)
			{
			elt->min_ = 1;
			elt->max_ = 0;
			frep = true;
			}
		else if (elt->min_ == 1 && elt->max_ == 0)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[PLUS key is redundant. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[PLUS key clashes with prior settings. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		}
	else if (!strcmp_i(key, _T("one"))	// 03/22/99 AM.
				|| !strcmp_i(key, _T("1"))
				)
		{
		if (!fmin && !fmax && !frep)
			{
			elt->min_ = 1;
			elt->max_ = 1;
			frep = true;
			}
		else if (elt->min_ == 1 && elt->max_ == 1)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[ONE key is redundant. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[ONE key clashes with prior settings. Ignoring.]")
						  << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		}
	else if (!strcmp_i(key, _T("matches"))
				|| !strcmp_i(key, _T("match"))
				)
		{
		// Check one or more values.
		elt->matches_ = vals;
		pair->setVals(0);		// Else they'll be deleted!  // 12/04/98 AM. Fix.
		if (fhash)																// 05/02/00 AM.
			internList(/*DU*/matches_, hmatches_);
										// Build hash table if needed.	// 12/12/98 AM.
		}
	else if (!strcmp_i(key, _T("fails"))
				|| !strcmp_i(key, _T("fail"))
				)
		{
		// Check one or more values.
		elt->fails_ = vals;
		pair->setVals(0);		// Else they'll be deleted!  // 12/04/98 AM. Fix.
		if (fhash)																// 05/02/00 AM.
			internList(/*DU*/fails_, hfails_);
								// Build hash table if needed.	// 12/12/98 AM.
		}
	else if (!strcmp_i(key, _T("excepts"))
				|| !strcmp_i(key, _T("except"))							// 10/13/99 AM.
				)
		{
		// Check one or more values.
		elt->excepts_ = vals;
		pair->setVals(0);								// Else they'll be deleted!
		if (fhash)																// 05/02/00 AM.
			internList(/*DU*/excepts_, hexcepts_); // Build hash table if needed.
		}
	else if (!strcmp_i(key, _T("recurse"))
				|| !strcmp_i(key, _T("nest"))
				|| !strcmp_i(key, _T("passes"))
				|| !strcmp_i(key, _T("pass"))
				)
		{
		// Build sequence lists for recursive mini-passes.
		elt->internRecurse(vals, seqs);
		}
	else if (!strcmp_i(key, _T("layer"))
				|| !strcmp_i(key, _T("layers"))
				|| !strcmp_i(key, _T("attr"))
				|| !strcmp_i(key, _T("attrs"))
				)
		{
		// 03/18/99 AM. Now rule elts will do layering.
		// Move layer names from pair.
		elt->attrs_ = vals;
		pair->setVals(0);		// Else they'll be deleted!  // 12/04/98 AM. Fix.
		}
	else if (!strcmp_i(key, _T("singlet"))
				|| !_tcscmp(key, _T("s"))		// Abbrev.	// 01/16/99 AM.
			   )
		{
		if (vals)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Values after singlet key ignored.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		if (elt->getTree())								// 03/17/99 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[SINGLET and TREE are incompatible.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		elt->setSinglet(true);
		}
	else if (!strcmp_i(key, _T("lookahead"))				// 11/19/99 AM.
				|| !_tcscmp(key, _T("look"))		// Abbrev.
			   )
		{
		if (vals)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Values after lookahead key ignored.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		elt->setLookahead(true);
		f_look = true;		// Current elt is lookahead			// 11/22/99 AM.
		}
	else if (!strcmp_i(key, _T("tree")))					// 03/17/99 AM.
			// t already used for TRIGGER!
		{
		if (vals)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Values after tree key ignored.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		if (elt->getSinglet())
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[SINGLET and TREE are incompatible.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		elt->setTree(true);
		}
	else if (!strcmp_i(key, _T("xsinglet"))
				|| !_tcscmp(key, _T("xs"))
			   )
		{
		if (vals)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Values after xsinglet key ignored.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		if (elt->getTree() || elt->getXtree() || elt->getSinglet())
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Conflicting (x)singlet and (x)tree keys.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		elt->setXsinglet(true);
		}
	else if (!strcmp_i(key, _T("xtree"))
				|| !strcmp_i(key, _T("xt")))  // Confusing, with t=trigger.
		{
		if (vals)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Values after xtree key ignored.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		if (elt->getTree() || elt->getXsinglet() || elt->getSinglet())
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Conflicting (x)singlet and (x)tree keys.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}

		elt->setXtree(true);
		}
	else if (!strcmp_i(key, _T("rename"))
				|| !strcmp_i(key, _T("ren"))
					)
		{
		Delt<Iarg> *darg;
		Iarg *iarg;
		if (vals
			 && (darg = vals->getFirst())
			 && (!darg->Right())
			 && (iarg = darg->getData())
			 && (iarg->getType() == IASTR)
			 )
			elt->rename_ = iarg->getStr();
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Problem with rename key's value.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
		}
	else if (!strcmp_i(key, _T("group"))										// 09/23/00 AM.
				|| !strcmp_i(key, _T("gp"))										// 09/23/00 AM.
					)
		{
		Delt<Iarg> *darg;														// 09/23/00 AM.
		Iarg *iarg;																// 09/23/00 AM.
		if (vals																	// 09/23/00 AM.
			 && (darg = vals->getFirst())									// 09/23/00 AM.
			 && (!darg->Right())												// 09/23/00 AM.
			 && (iarg = darg->getData())									// 09/23/00 AM.
			 && (iarg->getType() == IASTR)								// 09/23/00 AM.
			 )
			elt->group_ = iarg->getStr();									// 09/23/00 AM.
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Problem with group key's value.]") << std::ends;
			parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
			}
			// 09/23/00 AM.
		}
	else if (!strcmp_i(key, _T("base"))
			   || !strcmp_i(key, _T("unsealed")))	// 10/11/99 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[The ") << key << _T(" key")
				  << _T(" is disallowed in a rule phrase. Ignoring.]")
				  << std::ends;
		parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
		}
	else if (!strcmp_i(key, _T("dejunk"))								// 09/09/11 AM.
				|| !_tcscmp(key, _T("dj"))									// 09/09/11 AM.
			   )
		{
		if (vals)																// 09/09/11 AM.
			{
			std::_t_strstream gerrStr;			// 09/09/11 AM.
			gerrStr << _T("[Values after dejunk key ignored.]") << std::ends;
			parse->errOut(&gerrStr,false,true);										// 09/09/11 AM.
			}
		elt->setDejunk(true);												// 09/09/11 AM.
		}
	else if (!strcmp_i(key, _T("deaccent"))							// 01/28/05 AM.
				|| !_tcscmp(key, _T("deacc"))								// 01/28/05 AM.
				|| !_tcscmp(key, _T("dcc"))								// 01/28/05 AM.
				|| !_tcscmp(key, _T("da"))									// 01/28/05 AM.
			   )
		{
		if (vals)																// 01/28/05 AM.
			{
			std::_t_strstream gerrStr;			// 01/28/05 AM.
			gerrStr << _T("[Values after deaccent key ignored.]") << std::ends;
			parse->errOut(&gerrStr,false,true);										// 01/28/05 AM.
			}
		elt->setDeaccent(true);												// 01/28/05 AM.
		}
	else
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Unknown element key=") << key << _T("]") << std::ends;
		parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.
		}
	}

// DELETE ALL THE PAIRS!
// If they ain't been internalized, they are ignored.
Dlist<Ipair>::DeleteDlistAndData(list);
elt->setPairs(0);

// Further interning of the layer names.							// 05/06/00 AM.
if (elt->attrs_)															// 05/06/00 AM.
	{
	// Traverse attrs list, installing into a layers array.
	elt->setLayers(Iarg::strings_to_starr(elt->attrs_));		// 05/06/00 AM.

	// SHOULD REMOVE THE ATTRS LIST HERE.  EVERYONE SHOULD
	// USE THE STRING ARRAY VERSION.
	// (Do this when everything else is converted.				// 05/06/00 AM.
	//Dlist<Iarg>::DeleteDlistAndData(attrs_);
	//attrs_ = 0;
	}

// See what hasn't been assigned, and assign it a default value.
if (elt->max_ == -1 && elt->min_ == -1)
	elt->fillDefaults();
else if (elt->max_ == -1)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Only min key has been set.]") << std::ends;
	parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.

	max_ = 0;			// Recover.  max is a "don't care".
	}
else if (elt->max_ == -1)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Only max key has been set.]") << std::ends;
	parse->errOut(&gerrStr,false,true);					// 04/03/03 AM.

	min_ = 0;			// Recover. min is a "don't care".
	}
}


/********************************************
* FN:		INTERNPRES
* CR:		07/13/99 AM.
* SUBJ:	Internalize the pre actions for a rule element.
* NOTE:	Split from intern() to help rule generation.
********************************************/

void Ielt::internPres(
	Dlist<Ipre> *pres,			// For binding pre actions.
	int ord							// Current element number in rule's phrase.
	)
{
Ielt *elt;

elt = this;

// 11/29/98 AM.
// Find the pre actions to be executed by this rule element.
// Copy them into the element's list.
if (pres)
	{
	Dlist<Ipre> *plist;		// New list copying pres for element to execute.
	Delt<Ipre> *ppre;
	Ipre *pre;
	Ipre *ptmp;						// 07/02/99 AM.
	plist = new Dlist<Ipre>();	// Empty list.
	for (ppre = pres->getFirst(); ppre; ppre = ppre->Right())
		{
		pre = ppre->getData();
		if (pre->getStart() > ord)
			continue;						// Ignore this pre action.
		if (pre->getEnd() && pre->getEnd() < ord)
			continue;						// Ignore this pre action.
		// Copy into element's list of pre actions.
		ptmp = new Ipre(*pre);			// 07/02/99 AM.
		plist->rpush(ptmp);				// 07/02/99 AM.
		}
	if (plist
		&& plist->getFirst()													// 12/24/00 AM.
		)
		elt->setPres(plist);
	else
		Dlist<Ipre>::DeleteDlistAndData(plist);
	}
}

/********************************************
* FN:		INTERNRECURSE
* CR:		11/24/98 AM.
* SUBJ:	Internalize passes list into a list of mini-passes.
********************************************/

void Ielt::internRecurse(
	Dlist<Iarg> *vals,			// List of strings naming recursive passes.
	Dlist<Seqn> *seqs				// Actual passes.
	)
{
Dlist<Seqn> *passes = 0;		// New list for recursively called passes.
Delt<Iarg> *velt = 0;
Delt<Seqn> *selt = 0;
Iarg *val = 0;
Seqn *seq = 0;
_TCHAR *name = 0;
Seqn *tmp = 0;

passes = new Dlist<Seqn>();

// For each called pass.
for (velt = vals->getFirst(); velt; velt = velt->Right())
	{
	val = velt->getData();
	name = val->getStr();
	// For each recursive region in file.
	for (selt = seqs->getFirst(); selt; selt = selt->Right())
		{
		seq = selt->getData();
		if (!strcmp_i(name, seq->getName()))
			{
			//*gout << "[Found named pass=" << name << ".]" << std::endl;
			// Make a copy of the pass.
			tmp = new Seqn();
			tmp->setRules(seq->getRules());
			tmp->setName(seq->getName());
			tmp->setRecid(seq->getRecid());								// 05/31/00 AM.
			// 07/02/99 AM. No longer makes a copy.
			passes->rpush(tmp);
			//tmp->setRules(0);
			//tmp->setName(0);
			//delete tmp;
			}
		}
	}
setPasses(passes);								// 11/25/98 AM. Fix.
}


/********************************************
* FN:		EHASH
* CR:		05/02/00 AM.
* SUBJ:	Hash the lists of an interned rule elt.
* NOTE:	Not deleting the lists that are hashed, because the rule's trigger
*			may need to be hashed using these same lists.
********************************************/

void Ielt::ehash()
{
if (matches_)
	internList(/*DU*/matches_, hmatches_);
if (fails_)
	internList(/*DU*/fails_, hfails_);
if (excepts_)
	internList(/*DU*/excepts_, hexcepts_);
}


/********************************************
* FN:		FILLDEFAULTS
* CR:		11/11/98 AM.
* SUBJ:	Fill some default values for an elt.
* NOTE:	11/22/98 AM. Moved here from postrfa.cpp.
********************************************/

void Ielt::fillDefaults()
{
if (name_ && !strcmp_i(name_, _T("_xWILD")))
	{
	min_ = 0;
	max_ = 0;
	}
else
	{
	min_ = 1;
	max_ = 1;
	}
}


/********************************************
* FN:		INTERNLIST
* CR:		12/12/98 AM.
* SUBJ:	Build hash table as needed for the match list.
********************************************/
static _TCHAR lcbuf[MAXSTR];
void Ielt::internList(
	/*DU*/
	Dlist<Iarg>* &list,
	Htab* &htab
	)
{
Dlist<Iarg> *vals;
long len;

if (!(vals = list))
	return;

len = vals->getLength();

if (len < HTHRESH)		// Too short to bother with hash table.
	return;

// Make a hash table.  Traverse the list, finding entries.
// DELETE them from the list, if not special tokens.
long hsize;			// Size of hash table.
hsize = Htab::makesize(len);		// Doc table size.		// 06/15/00 AM.
htab = new Htab(Htab::getGstab(), hsize);		// Add hash table.

Delt<Iarg> *darg = 0;
Iarg *arg = 0;
//Dlist<Iarg> *keep;		// New list of names to keep.	// 12/17/98 AM.
//keep = new Dlist<Iarg>();	// Empty list.				// 12/17/98 AM.
_TCHAR *str;
for (darg = vals->getFirst(); darg; darg = darg->Right())	// 12/17/98 AM.
//while (darg = vals->pop())			// Don't zap.	// 12/17/98 AM.
	{
	arg = darg->getData();
	str = arg->getStr();
	if (special(str))		// Special name like _xWHITE.
		//keep->push(darg);									// 12/17/98 AM.
		;															// 12/17/98 AM.
	else
		{
		// Note: dejunk key must precede match list!	// 09/09/11 AM.
		if (getDejunk())										// 09/09/11 AM.
			{
			long dlen = _tcsclen(str);							// 09/09/11 AM.
			_TCHAR *dejunk = Chars::create(dlen + 2);		// 09/09/11 AM.
			Xml::de_junk(str,/*DU*/dejunk);					// 09/09/11 AM.
			// Put into hash table.
			(void) htab->hget(str_to_lower(dejunk, lcbuf));// 09/09/11 AM.
			Chars::destroy(dejunk);								// 09/09/11 AM.
			}
		// Note: deaccent key must precede match list!	// 03/27/05 AM.
		else
		if (getDeaccent())									// 03/27/05 AM.
			{
			long dlen = _tcsclen(str);							// 03/27/05 AM.
			_TCHAR *deacc = Chars::create(dlen + 2);		// 03/27/05 AM.
			Xml::de_accent(str,/*DU*/deacc);					// 03/27/05 AM.
			// Put into hash table.
			(void) htab->hget(str_to_lower(deacc, lcbuf));// 03/27/05 AM.
			Chars::destroy(deacc);								// 03/27/05 AM.
			}
		else															// 03/27/05 AM.
		// Put into hash table.
		(void) htab->hget(str_to_lower(str, lcbuf));
		}
	}
// 12/17/98 AM. Pruning of these lists now done separately.
// Delete the old list.
//if (list)														// 12/17/98 AM.
//	Dlist<Iarg>::DeleteDlistAndData(list);				// 12/17/98 AM.
//list = keep;	// Keepers. (Special names like _xWHITE).	// 12/17/98 AM.
}


/********************************************
* FN:		PRUNELISTS
* CR:		12/17/98 AM.
* SUBJ:	Clean up match/fail/except lists that were interned.
********************************************/

void Ielt::pruneLists(Ielt *elt)
{
if (!elt)
	return;
elt->matches_ = pruneList(elt->matches_);
elt->fails_   = pruneList(elt->fails_);
elt->excepts_ = pruneList(elt->excepts_);			// 10/13/99 AM.
}


/********************************************
* FN:		PRUNELIST
* CR:		12/17/98 AM.
* RET:	The list of keepers.
* SUBJ:	Clean up a match/fail list that was interned.
********************************************/

Dlist<Iarg> *Ielt::pruneList(Dlist<Iarg> *list)
{
if (!list)
	return list;

Dlist<Iarg> *vals;
long len;

if (!(vals = list))
	return list;

len = vals->getLength();

if (len < HTHRESH)		// Too short to bother with hash table.
	return list;

Delt<Iarg> *darg = 0;
Iarg *arg = 0;
Dlist<Iarg> *keep;		// New list of names to keep.
keep = new Dlist<Iarg>();	// Empty list.
_TCHAR *str;
while ((darg = vals->pop()))
	{
	arg = darg->getData();
	str = arg->getStr();
	if (special(str))		// Special name like _xWHITE.
		keep->push(darg);
	else
		{
		// Zap it.
		// 07/02/99 AM. Seems you need to delete the delt's data also.
		//delete darg;			// 07/02/99 AM.
		Delt<Iarg>::DeleteDeltAndData(darg);	// FIX. // 07/02/99 AM.
		}
	}
// Delete the old list OBJECT.
if (list)
	{
	if (list->getFirst())
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[pruneList: List should be empty]") << std::ends;
		errOut(&gerrStr,false);
		}

	Dlist<Iarg>::DeleteDlistAndData(list);
	}
return keep;
}


/********************************************
* FN:		HASHABLE
* CR:		12/16/98 AM.
* SUBJ:	See if rule's phrase is hashable.
* NOTE:	If not hashable, that means rule will go on a list that
*			must be tried by nodes in general (the musts_ list).
*			For example, a wildcard elt or one that matches all alpha tokens
*			will be considered not hashable.
*			An element will be "done" when it is non-optional.  That is, we'll
*			hash all the optionals at the beginning of a rule till the first
*			non-optional is encountered.
*			3/17/99 AM. Note that Irule::rhash calls this for trigger elt also.
********************************************/

bool Ielt::hashable(Dlist<Ielt> *dlist,
	Parse *parse)	// 04/03/03 AM.
{
if (!dlist)
	return false;

Delt<Ielt> *delt = 0;
Ielt *elt = 0;
bool done = false;
for (delt = dlist->getFirst(); delt; delt = delt->Right())
	{
	if (!hashable(delt->getData(), parse, /*UP*/ done))
		return false;		// Couldn't hash elt, so don't hash rule.
	if (done)
		return true;		// Rule is hashable.
	}

// Set line number of error to last elt of rule, for now.	// 04/03/03 AM.
delt = dlist->getLast();												// 04/03/03 AM.
elt  = delt->getData();													// 04/03/03 AM.
parse->inputline_ = elt->line_;										// 04/03/03 AM.
std::_t_strstream gerrStr;
gerrStr << _T("[hashable: Rule may be entirely optional.]") << std::ends;
parse->errOut(&gerrStr,false,true);		// 04/03/03 AM.

return true;		// Rule appears hashable, anyway.
}


/********************************************
* FN:		HASHABLE
* CR:		12/17/98 AM.
* SUBJ:	See if current rule phrase element is hashable.
* NOTE:	01/06/99 AM. If element has a FAIL list, then it is placed on the
*			"must-try" list.
********************************************/

bool Ielt::hashable(
	Ielt *elt,				// Element to check.
	Parse *parse,			// 04/03/03 AM.
	/*UP*/
	bool &done				// If done checking phrase elements.
	)
{
done = true;				// Unless we find out otherwise.

if (elt->getMin() == 0)
	done = false;			// Optional element.

if (elt->wild())
	return false;			// Must match against every node.

if (elt->getTree())		// 03/18/99 AM.
	{
	// 03/18/99 AM. Since we'd have to search down the node's entire
	// subtree to build a list of rules to try, we instead just declare
	// rules with TREE attribute as nonhashable.  That is, every node will
	// have to try them out.
	return false;			// 03/18/99 AM.
	}

// If there are match/fail lists, see if they are hashable.
if (elt->matches_)
	{
	if (!hashables(elt->matches_))
		return false;
	return true;
	}
if (elt->fails_)
	return false;			// Fix.		// 01/06/99 AM.

if (elt->excepts_)						// 10/13/99 AM.
	return false;	// No fails, so excepts is acting like a fails list.

// Be careful of ordering.  This one fails on _xWILD also, even though
// that's our syntax for match/fail element.
if (special(elt->getName()))	// Stuff like "_xWHITE".
	return false;

// Passed the obstacle course.
return true;
}


/********************************************
* FN:		HASHABLES
* CR:		12/17/98 AM.
* SUBJ:	See if match/fail list is fully hashable.
********************************************/

bool Ielt::hashables(Dlist<Iarg> *dlist)
{
if (!dlist)
	return true;			// No list is ok.
Delt<Iarg> *darg;
Iarg *arg;
_TCHAR *str;
for (darg = dlist->getFirst(); darg; darg = darg->Right())
	{
	arg = darg->getData();
	str = arg->getStr();
	if (special(str))
		return false;
	}
return true;
}


/********************************************
* FN:		RHASHES
* CR:		12/17/98 AM.
* SUBJ:	Hash rule based on match/fail list.
********************************************/

bool Ielt::rhashes(Dlist<Iarg> *dlist, tHtab<Slist<Irule> > *htab, Irule *rule)
{
if (!dlist)
	return true;			// No list is ok.
Delt<Iarg> *darg;
Iarg *arg;
_TCHAR *str;
for (darg = dlist->getFirst(); darg; darg = darg->Right())
	{
	arg = darg->getData();
	str = arg->getStr();
	if (special(str))		// Todo: Remove this code.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[rhashes: Error]") << std::ends;
		errOut(&gerrStr,false);
		return false;
		}
	// HASH THE RULE HERE.
	Ielt::hashRule(str, htab, rule);
	}
return true;
}


/********************************************
* FN:		RHASH
* CR:		12/16/98 AM.
* SUBJ:	Hash the current rule based on current element.
********************************************/

bool Ielt::rhash(
	Ielt *elt,
	tHtab<Slist<Irule> > *htab,
	Irule *rule,
	Parse *parse,	// 04/03/03 AM.
	/*UP*/
	bool &done
	)
{
done = true;

if (elt->getMin() == 0)
	done = false;			// Optional element.

if (elt->wild())			// Todo: Remove this code.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rhash: Error]") << std::ends;
	errOut(&gerrStr,false);
	return false;			// Must match against every node.
	}

// If there are match/fail lists, see if they are hashable.
if (elt->matches_ || elt->fails_)
	{
	// HASH THE RULE HERE.
	rhashes(elt->matches_, htab, rule);
	rhashes(elt->fails_, htab, rule);
	return true;
	}

// Todo: Remove this code.
if (special(elt->getName()))	// Stuff like "_xWHITE".
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rhash: Error]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// HASH THE RULE HERE.
Ielt::hashRule(elt->getName(), htab, rule);

return true;
}


/********************************************
* FN:		RHASH
* CR:		12/16/98 AM.
* SUBJ:	Hash the current rule based on the rule's phrase.
********************************************/

bool Ielt::rhash(
	Dlist<Ielt> *dlist,
	tHtab<Slist<Irule> > *htab,
	Irule *rule,
	Parse *parse	// 04/03/03 AM.
	)
{
if (!dlist)
	return false;

Delt<Ielt> *delt = 0;
Ielt *elt = 0;
bool done = false;
for (delt = dlist->getFirst(); delt; delt = delt->Right())
	{
//if (parse->getInputpass() == 3		// DEBUGGING!! // 02/27/00 AM.
//	&& !strcmp(rule->getSugg()->getName(), "_ENDRULE"))
//	parse->getInputpass() = 3;
	rhash(delt->getData(), htab, rule, parse, /*UP*/ done);
	if (done)
		return true;		// Done hashing rule.
	}
//*gerr << "[rhash: Rule may be entirely optional.]" << std::endl;
return true;
}


/********************************************
* FN:		HASHRULE
* CR:		12/17/98 AM.
* SUBJ:	Install pointer to rule in hash table with given string as key.
* RET:	True if successful, else false.
* NOTE:	12/19/98 AM. Check that rule isn't already present in the hash
*			chain.  If it is, it will be last in chain.
********************************************/

bool Ielt::hashRule(
	_TCHAR *str,
	tHtab<Slist<Irule> > *htab,
	Irule *rule
	)
{
if (!str || !htab || !rule)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hashRule: Given null data.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Created template classes tSym and tHash.
// Get symbol.
tSym<Slist<Irule> > *sym;
sym = htab->hget_lc(str)->getData();	// Lowercase find or add.

// Push the current rule onto its list.  That's all!
Slist<Irule> *list;
if (!(list = sym->getPtr()))
	{
	// Create list.
	list = new Slist<Irule>();		// Empty list.
	sym->setPtr(list);
	}

// If rule is already on the hash chain, don't add it.
Selt<Irule> *selt;
Irule *hrule;
if ((selt = list->getLast()))
	{
	hrule = selt->getData();
	if (hrule->getNum() == rule->getNum())			// 12/19/98 AM.
		return true;
	}

// Add the rule to the hash chain.
list->rpush(rule);

return true;
}

/********************************************
* FN:		GENPHRASE
* CR:		05/30/99 AM.
* SUBJ:	Generate text for a rule's phrase to a file.
* NOTE:	Part of RUG rules file generation.
********************************************/

void Ielt::genPhrase(
	Dlist<Ielt> *phr,
	_TCHAR *sep,				// Elt separation.							// 11/04/99 AM.
	std::_t_ostream &ofile,
	bool trunc																	// 06/05/00 AM.
	)
{
if (!phr)
	return;

_TCHAR *esep;																		// 11/04/99 AM.
if (*sep == '\n')		// If rule printed multi line,				// 11/04/99	AM.
	esep = _T("\t");		// elts look like X\t[...]						// 11/04/99 AM.
else
	esep = _T(" ");			// Else simple space.							// 11/04/99 AM.

Delt<Ielt> *delt;
Ielt *elt;
for (delt = phr->getFirst(); delt; delt = delt->Right())
	{
	elt = delt->getData();
	ofile << sep << std::flush;
	genElt(elt, esep, ofile, trunc);
	}
}


/********************************************
* FN:		GENELT
* CR:		05/30/99 AM.
* SUBJ:	Generate text for a rule's element to a file.
* ASS:	Not using PAIRS, but rather the internalized values in elt.
********************************************/

void Ielt::genElt(
	Ielt *elt,
	_TCHAR *sep,								// 11/04/99 AM.
	std::_t_ostream &ofile,
	bool trunc								// 06/05/00 AM.
	)
{
if (!elt)
	return;
_TCHAR *name;
name = elt->getName();
Iarg::genName(name, ofile);			// 07/11/99 AM.

bool seen = false;	// If prior pair has been seen.
_TCHAR s_start[16];							// 11/04/99 AM.
_TCHAR *s_end = _T("]");

_stprintf(s_start, _T("%s["), sep);			// 11/04/99 AM.

int max, min;
min = elt->min_;
max = elt->max_;
bool wild = false;
if (!_tcscmp(_T("_xWILD"), name))
	wild = true;

_TCHAR *str;

if (min == 0)
	{
	if (max == 0)
		{
		if (!wild)
			{
			if (seen)
				ofile << _T(" ");
			else
				{
				ofile << s_start;
				seen = true;
				}
			ofile << _T("star");
			}
		// else if (wild), star is the default.
		}
	else if (max == 1)
		{
		if (seen)
			ofile << _T(" ");
		else
			{
			ofile << s_start;
			seen = true;
			}
		ofile << _T("opt");
		}
	else
		{
		if (seen)
			ofile << _T(" ");
		else
			{
			ofile << s_start;
			seen = true;
			}
		ofile << _T("min=") << min << _T(" max=") << max;
		}
	}
else if (min == 1)
	{
	if (max == 0)
		{
		if (seen)
			ofile << _T(" ");
		else
			{
			ofile << s_start;
			seen = true;
			}
		ofile << _T("plus");
		}
	else if (max != 1 || wild)
		{
		if (seen)
			ofile << _T(" ");
		else
			{
			ofile << s_start;
			seen = true;
			}
		ofile << _T("min=") << min << _T(" max=") << max;
		}
	// else if (max==1 && wild) that's the default for nonwilds.
	}
else
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("min=") << min << _T(" max=") << max;
	}

if (elt->getTrigger())
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("trig");
	}
if (elt->getSinglet())
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("s");
	}
if (elt->getLookahead())				// 11/19/99 AM.
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("lookahead");				// 11/19/99 AM.
	}
if (elt->getTree())
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("tree");
	}
if (elt->getDejunk())				// 09/09/11 AM.
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("dejunk");
	}
else if (elt->getDeaccent())		// 01/28/05 AM.
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("da");
	}
if ((str = elt->getRename()))
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	if (*(str+1))					// strlen_gt(str, 1)			// 11/04/99 AM.
		ofile << _T("ren=") << str;
	else
		ofile << _T("ren=") << pretty_char(*str);					// 11/04/99 AM.
	}
if ((str = elt->getGroup()))												// 09/23/00 AM.
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	if (*(str+1))					// strlen_gt(str, 1)
		ofile << _T("group=") << str;											// 09/23/00 AM.
	else
		ofile << _T("group=") << pretty_char(*str);						// 09/23/00 AM.
	}
Dlist<Iarg> *dargs;
Starr *starr;															// 05/06/00 AM.
if ((starr = elt->getLayers()))
	{
	_TCHAR **arr = starr->getArr() - 1;	// One BEFORE array.
	int len = starr->getLength() + 1;	// One greater than length.
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("layer=(");
	while (--len)
		ofile << *++arr << _T(" ");
	ofile << _T(")");
	}
else if ((dargs = elt->getAttrs()))
	{
	// Layering attribute.
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("layer=(");
	Iarg::genArgs(dargs, _T(" "), ofile,trunc);
	ofile << _T(")");
	}
if ((dargs = elt->getMatches()))
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("match=(");
	Iarg::genArgs(dargs, _T(" "), ofile,trunc);
	ofile << _T(")");
	}
if ((dargs = elt->getFails()))
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("fail=(");
	Iarg::genArgs(dargs, _T(" "), ofile,trunc);
	ofile << _T(")");
	}
if ((dargs = elt->getExcepts()))			// 10/14/99 AM.
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("excepts=(");
	Iarg::genArgs(dargs, _T(" "), ofile,trunc);
	ofile << _T(")");
	}
Dlist<Seqn> *seqs;							// 11/04/99 AM.
if ((seqs = elt->getPasses()))				// 11/04/99 AM.
	{
	if (seen)
		ofile << _T(" ");
	else
		{
		ofile << s_start;
		seen = true;
		}
	ofile << _T("recurse=(");
	Seqn::genRecurses(seqs, _T(" "), ofile);
	ofile << _T(")");
	}
if (seen)
	ofile << s_end << std::flush;
}


/********************************************
* FN:		GENPHRASERECURSES
* CR:		11/04/99 AM.
* SUBJ:	Generate recurse rule regions for a rule phrase.
* NOTE:	Generating from internal RFA is the only use so far.
*********************************************/

bool Ielt::genPhraserecurses(
	Dlist<Ielt> *phrase,
	_TCHAR *sep,			// Element separator.
	std::_t_ostream &ofile)
{
if (!phrase)
	return false;

// For each elt, generate its recurse regions.
Delt<Ielt> *delt;
Ielt *elt;
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	elt = delt->getData();
	elt->genEltrecurses(sep, ofile);
	}
return true;
}


/********************************************
* FN:		GENELTRECURSES
* CR:		11/04/99 AM.
* SUBJ:	Generate recurse rule regions for a rule element.
* NOTE:	Generating from internal RFA is the only use so far.
*			NEED TO TRACK RECURSE REGIONS ALREADY GEND IN THE CURRENT FILE.
*********************************************/

bool Ielt::genEltrecurses(
	_TCHAR *sep,			// Element separator.
	std::_t_ostream &ofile)
{
// Get list of recurse sequences.
Dlist<Seqn> *seqs = getPasses();
if (!seqs)
	return true;
Delt<Seqn> *dseq;
Seqn *seq;
for (dseq = seqs->getFirst(); dseq; dseq = dseq->Right())
	{
	seq = dseq->getData();
	Dlist<Irule> *rules = seq->getRules();
	if (!rules)
		continue;

	ofile << _T("@RECURSE ") << seq->getName() << std::endl;

	// Generate recurse rule blocks.
	// (And these shouldn't have recurses,.)
	Irule::genRuleblocks(rules, sep, ofile);
	ofile << std::endl
			<< _T("@@RECURSE ") << seq->getName() << std::endl << std::endl;
	}

return true;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	If two lists of elts (ie, phrases) are the same in form.
********************************************/

bool Ielt::same(Dlist<Ielt> *list1, Dlist<Ielt> *list2)
{
if (!list1 && !list2)
	return true;
if (!list1 || !list2)
	{
	if (list1 && list1->getFirst())
		return false;
	if (list2 && list2->getFirst())
		return false;
	return true;			// Empty lists even so.
	}

// Compare elt-by-elt.
Delt<Ielt> *delt1, *delt2;
delt1 = list1->getFirst();
delt2 = list2->getFirst();
if (!delt1 && !delt2)	// FIX	// 07/31/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Empty lists.]") << std::ends;
	errOut(&gerrStr,false);
	return true;
	}
if (!delt1 || !delt2)	// FIX	// 07/31/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
for (;
	  delt1 && delt2;		// FIX	// 07/31/99 AM.
	  delt1 = delt1->Right(), delt2 = delt2->Right())
	{
	if (!same(delt1->getData(), delt2->getData()))
		return false;
	}

if (delt1 || delt2)	// Lists of different length.
	return false;

return true;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	If two elts are the same in form.
* ASS:	Not checking hash tables of matches and fails.
*			Assumes they're in the match and fail lists.
********************************************/

bool Ielt::same(Ielt *elt1, Ielt *elt2)
{
if (!elt1 && !elt2)
	return true;
if (!elt1 || !elt2)
	return false;

// Singlet, trigger, tree, xsinglet (unimp), xtree (unimp),
// and whatever other flags.
if (elt1->getFlags() != elt2->getFlags())
	return false;

if (elt1->min_ != elt2->min_)
	return false;
if (elt1->max_ != elt2->max_)
	return false;

if (!str_equal(elt1->getRename(), elt2->getRename()))
	return false;

if (!str_equal(elt1->getGroup(), elt2->getGroup()))				// 09/23/00 AM.
	return false;																// 09/23/00 AM.

// Pre actions presumably handled at rule level.
// Check them here, too.  Sometimes redundant, sometimes
// not.
if (!Ipre::same(elt1->getPres(), elt2->getPres()))
	return false;

// Recursive passes also. Eek!
if (!Seqn::same(elt1->getPasses(), elt2->getPasses()))
	return false;

if (!Iarg::same(elt1->getMatches(), elt2->getMatches()))
	return false;

if (!Iarg::same(elt1->getFails(), elt2->getFails()))
	return false;

if (!Iarg::same(elt1->getExcepts(), elt2->getExcepts()))		// 10/14/99 AM.
	return false;

// Oh yeah.  The meat of this function.
if (!str_equal(elt1->getName(), elt2->getName()))
	return false;

if (!Starr::same(elt1->getLayers(), elt2->getLayers()))		// 05/06/00 AM.
	return false;
if (!Iarg::same(elt1->getAttrs(), elt2->getAttrs()))
	return false;

return true;
}


/********************************************
* FN:		NAMEMATCHES
* CR:		07/05/99 AM.
* SUBJ:	See if elt has given string in name or matches fields.
* RET:	True if name found.
*********************************************/

bool Ielt::nameMatches(_TCHAR *str)
{
if (!str || !*str)
	return false;

_TCHAR *name = getName();
if (!_tcscmp(name, str))
	return true;

Dlist<Iarg> *matches = 0;
if ((matches = getMatches()))
	{
	// (name should be "_xWILD" here).
	if (Iarg::findArg(matches, str))
		return true;
	}

return false;
}


/********************************************
* FN:		LITNAME
* CR:		07/05/99 AM.
* SUBJ:	Get first literal name for elt, if any.
* RET:	First literal name found, else 0.
*********************************************/

_TCHAR *Ielt::litName()
{
_TCHAR *str;
if (*(str = getName()) != '_')
	return str;			// Name is literal.

Dlist<Iarg> *matches;
if ((matches = getMatches()))
	{
	Delt<Iarg> *delt;
	Iarg *arg;
	for (delt = matches->getFirst(); delt; delt = delt->Right())
		{
		arg = delt->getData();
		if (*(str = arg->getStr()) != '_')
			return str;
		}
	}
return 0;
}


/********************************************
* FN:		TRIGPHRASE
* CR:		10/07/99 AM.
* SUBJ:	Find best candidate to be a phrase's trigger element.
* RET:	trig - If found a good candidate other than first elt.
*			Else, first element is "trigger" by default.
*********************************************/

Delt<Ielt> *Ielt::trigPhrase(
	Dlist<Ielt> *phrase)
{
if (!phrase)
	return 0;

// See how good a trigger the first element is.
// If it has the highest rating, then return nil.
// Else, save its rating.  (Will give a numeric rating.)
bool bestrating = false;
Delt<Ielt> *first = phrase->getFirst();
if (!first)
	return 0;
if (!(first->Right()))	// OPT	// 11/01/99 AM.
	return 0;				// OPT	// 11/01/99 AM.
int nfirst = first->getData()->trigRating(/*UP*/ bestrating);
if (bestrating)
	return 0;		// First elt is best kind of trigger.

// Traverse elts, looking for ever better trigger candidate.
// If highest rating found, return it.
Delt<Ielt> *delt;
Delt<Ielt> *best = 0;
int nbest = 0;
int ncurr = 0;
for (delt = first->Right(); delt; delt = delt->Right())
	{
	ncurr = delt->getData()->trigRating(/*UP*/ bestrating);
	if (bestrating)		// Found best possible trigger.
		return delt;		// Return it.

	if (ncurr > nbest)	// Found best so far.
		{
		nbest = ncurr;
		best = delt;
		}
	}

// If final candidate is better than trigger, return it.
if (nbest > nfirst)
	return best;
return 0;
}



/********************************************
* FN:		TRIGRATING
* CR:		10/07/99 AM.
* SUBJ:	Get a "rating number" for how good a trigger an elt is.
* RET:	rating -  See trigRatename.
*		0 - yuck like _xALPHA, _xWHITE, _xSTART, _xEND, _xNIL, _xEOF, _xANY,
*		    _xBLANK
*		   (don't know if _xEOF can be a trigger, otherwise would be good.)
*		1 - _xCAP
*		2 - _xNUM
*		3 - _xPUNCT
*		4 - Particular puncuation mark.
*		5 - nonliteral, like _year.
*		6 - literal, like "email" or "1983".
*		UP bestrating - flag whether elt has the best rating possible.
*********************************************/

int Ielt::trigRating(
	/*UP*/
	bool &bestrating
	)
{
bestrating = false;

// Traverse all names of elt, finding updating to the WORST of the bunch.
int nworst;								// The worst rating found so far.
_TCHAR *name = getName();
Dlist<Iarg> *matches = getMatches();


if (matches && _tcscmp(_T("_xWILD"), name))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Matches in nonwild elt. Ignoring.]") << std::ends;
	errOut(&gerrStr,false);

	matches = 0;
	}

if (!matches)
	{
	nworst = Ielt::trigRatename(name);
	if (nworst == BESTTRIG)
		bestrating = true;
	return nworst;
	}

nworst = BESTTRIG;	// Sentinel.
Delt<Iarg> *darg;
Iarg *arg;
int ncurr;				// Current rating.
for (darg = matches->getFirst(); darg; darg = darg->Right())
	{
	arg = darg->getData();
	name = arg->getStr();
	ncurr = Ielt::trigRatename(name);
	if (ncurr == 0)		// Lowest rating.
		return ncurr;
	if (ncurr < nworst)
		nworst = ncurr;		// Worst so far.
	}

if (nworst == BESTTRIG)
	bestrating = true;
return nworst;
}


/********************************************
* FN:		TRIGRATENAME
* CR:		10/07/99 AM.
* SUBJ:	Get a "rating number" for how good a trigger an elt name is.
* RET:	rating -  
*		0 - yuck like _xALPHA, _xWHITE, _xSTART, _xEND, _xNIL, _xEOF, _xANY,
*		    _xBLANK
*		   (don't know if _xEOF can be a trigger, otherwise would be good.)
*		1 - _xCAP
*		2 - _xNUM
*		3 - _xPUNCT
*		4 - Particular puncuation mark.
*		5 - nonliteral, like _year.
*		6 - literal, like "email" or "1983".  (BESTTRIG)
* WARN:	Must keep BESTTRIG up to date.
*********************************************/

int Ielt::trigRatename(
	_TCHAR *name
	)
{
if (!name)
	return 0;
if (alphabetic(*name) || _istdigit((_TUCHAR)*name))
	return BESTTRIG;
if (*name != '_')
	return 4;
if (!_tcscmp(_T("_xWILD"), name)
	 || !_tcscmp(_T("_xALPHA"), name)
	 || !_tcscmp(_T("_xWHITE"), name)
	 || !_tcscmp(_T("_xSTART"), name)
	 || !_tcscmp(_T("_xEND"), name)
	 || !_tcscmp(_T("_xNIL"), name)
	 || !_tcscmp(_T("_xANY"), name)
	 || !_tcscmp(_T("_xEOF"), name)
	 || !_tcscmp(_T("_xCTRL"), name)										// 07/24/00 AM.
	 )
	return 0;
if (!_tcscmp(_T("_xCAP"), name))
	return 1;
if (!_tcscmp(_T("_xNUM"), name))
	return 2;
if (!_tcscmp(_T("_xPUNCT"), name))
	return 3;
if (!_tcscmp(_T("_xLET"), name))		// 07/10/09 AM.
	return 4;								// 07/10/09 AM.
if (!_tcscmp(_T("_xCAPLET"), name))		// 07/10/09 AM.
	return 4;								// 07/10/09 AM.
return 5;	// Random nonliteral.
}


/********************************************
* FN:		PHRASEACTIONS
* CR:		05/08/00 AM.
* SUBJ:	See if there are ANY phrase "actions".
* RET:	true if found any.
* NOTE:	That is, pairs like layer, recurse, etc.
*********************************************/

bool Ielt::phraseActions(Dlist<Ielt> *phrase)
{
Delt<Ielt> *delt;
Ielt *elt;
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	elt = delt->getData();
	if (elt->rename_ || elt->attrs_ || elt->layers_ || elt->passes_
		 || elt->group_														// 09/23/00 AM.
		)
		return true;
	}
return false;
}


/********************************************
* FN:		GENFIRST
* CR:		05/02/00 AM.
* SUBJ:	Code generation for first elt of rule's phrase.
* RET:	true if ok.
*********************************************/

bool Ielt::genFirst(Delt<Ielt> *first, Gen *gen)
{
// Treating like any other element, if feasible.					// 05/25/00 AM.
return genRight(first, gen);												// 05/25/00 AM.
}


/********************************************
* FN:		GENMATCHELT
* CR:		05/02/00 AM.
* SUBJ:	Code generation for an elt of rule's phrase.
* RET:	true if ok.
*********************************************/

bool Ielt::genMatchelt(Delt<Ielt> *delt, _TCHAR *fnname, _TCHAR *matchfn, Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
int eltid = gen->eltid_;

*fcode << _T("\t\t");
*fcode << _T("if (!") << _T("Arun::") << fnname << _T("(");
*fcode << eltid << _T(",");			// Give it its element number.	// 05/18/00 AM.
if (matchfn && *matchfn)													// 05/23/00 AM.
	*fcode << matchfn << _T(",");												// 05/23/00 AM.
*fcode << _T("nlppp");
*fcode << _T("))");																// 08/14/03 AM.

// Moved here.																	// 08/14/03 AM.
// Handling case of _xSTART as part of match list.					// 08/10/03 AM.
Ielt *elt = delt->getData();												// 08/10/03 AM.
if (Iarg::findArgcase(elt->getMatches(),_T("_xSTART")))				// 08/10/03 AM.
	{
	int eltid = gen->eltid_;												// 08/10/03 AM.

	Gen::eol(fcode);															// 08/14/03 AM.
	*fcode << _T("\t\t\t");														// 08/14/03 AM.
	*fcode << _T("if (!") << _T("Arun::") << _T("match_start") << _T("(");		// 08/10/03 AM.
	*fcode << eltid << _T(",");													// 08/10/03 AM.
	*fcode << _T("nlppp");														// 08/10/03 AM.
	*fcode << _T("))");															// 08/10/03 AM.
	}

*fcode << _T(" return false;");												// 08/14/03 AM.
Gen::eol(fcode);																// 04/04/03 AM.

return true;
}


/********************************************
* FN:		GENSPECIALELT
* CR:		05/25/00 AM.
* SUBJ:	Code generation for a special elt of rule's phrase.
* RET:	true if ok.
*********************************************/

bool Ielt::genSpecialelt(
	Delt<Ielt> *delt,
	_TCHAR *fnname,
	_TCHAR *matchfn,
	_TCHAR *microfn,		// The "micro" matching function.
	Gen *gen
	)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
int eltid = gen->eltid_;

*fcode << _T("\t\t");
*fcode << _T("if (!") << _T("Arun::") << fnname << _T("(");
*fcode << eltid << _T(",");			// Give it its element number.
*fcode << matchfn << _T(",");
*fcode << microfn << _T(",");
*fcode << _T("nlppp");
*fcode << _T(")) return false;");
Gen::eol(fcode);																// 04/04/03 AM.
return true;
}


/********************************************
* FN:		GENTRIGGER
* CR:		05/02/00 AM.
* SUBJ:	Code generation for trigger elt of rule's phrase.
* RET:	True if ok.
*********************************************/

bool Ielt::genTrigger(Delt<Ielt> *trig, Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.

if (!trig)
	{
	*fcode << std::endl
			<< _T("// ERROR in genTrigger.") << std::endl;
	return false;
	}

Ielt *ielt = trig->getData();
//_TCHAR *fn = ielt->matchFn();												// 05/25/00 AM.

if (ielt->wild())
	{
	*fcode << std::endl
			<< _T("// ERROR: wildcard in genTrigger.") << std::endl;
	return false;
	}

return genRight(trig, gen);												// 05/25/00 AM.
// return genMatchelt(trig, "match_first", fn, gen);				// 05/25/00 AM.
}


/********************************************
* FN:		GENLEFT
* CR:		05/02/00 AM.
* SUBJ:	Code generation for elts to the left of trigger.
* RET:	true if ok.
*********************************************/

bool Ielt::genLeft(Delt<Ielt> *elt, Gen *gen)
{
return genDirection(
	elt,
	true,				// Moving left.
	gen);
}


/********************************************
* FN:		GENRIGHT
* CR:		05/02/00 AM.
* SUBJ:	Code generation for elts to the right of trigger.
* RET:	true if ok.
* NOTE:	05/30/00 AM. Parameterized.
*********************************************/

bool Ielt::genRight(Delt<Ielt> *elt, Gen *gen)
{
return genDirection(
	elt,
	false,			// Moving right.
	gen);
}


/********************************************
* FN:		GENDIRECTION
* CR:		05/30/00 AM.
* SUBJ:	Code generation for elts to the left or right of trigger.
* RET:	true if ok.
*********************************************/

bool Ielt::genDirection(
	Delt<Ielt> *elt,
	bool fleft,					// If going left of trigger.
	Gen *gen
	)
{
_TCHAR *wild_fn;
_TCHAR *reg_fn;
_TCHAR *special_fn;
if (fleft)																		// 05/30/00 AM.
	{
	wild_fn = _T("match_left_wild");
	reg_fn = _T("match_left");
	special_fn = _T("special_left");
	}
else
	{
	wild_fn = _T("match_right_wild");
	reg_fn = _T("match_right");
	special_fn = _T("special_right");
	}

std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
Ielt *ielt = elt->getData();
_TCHAR *name = ielt->name_;

if (ielt->wild())
	return genMatchelt(elt, wild_fn, 0, gen);

_TCHAR *micro;

// Empty wild with preconditions.	// 05/14/13 AM.
// _xWILD [one] same as _xANY			// 05/14 13 AM.
if (!_tcscmp(name, _T("_xWILD"))		// 05/14/13 AM.
	 && ielt->pres_						// 05/14/13 AM.
	 && !ielt->matches_					// 05/14/13 AM.
	 && !ielt->fails_						// 05/14/13 AM.
	)
	{
	micro = _T("Arun::micro_any");	// 05/14/13 AM.
	return genSpecialelt(				// 05/14/13 AM.
			elt,
			special_fn,
			ielt->matchSpecialfn(),
			micro,					// The micro match function.
			gen);
	}

if (!special(name)	// Not something like _xSTART.				// 05/25/00 AM.
	 || !_tcscmp(name, _T("_xWILD")))	// Guy with lists.				// 05/25/00 AM.
	return genMatchelt(elt,reg_fn,ielt->matchFn(),gen);			// 05/23/00 AM.

// SPECIAL MATCH MODES.  TWO TYPES										// 05/25/00 AM.
// Look at the individual match mode.									// 05/25/00 AM.
name += 2;		// Slough the "_x";

// TYPE I. SPECIAL MODES LIKE BOUNDARIES.
if (!_tcscmp(name, _T("START")))
	{
	return genMatchelt(elt, _T("match_start"), 0, gen);
	}
else if (!_tcscmp(name, _T("END")))
	{
	return genMatchelt(elt, _T("match_end"), 0, gen);
	}
else if (!_tcscmp(name, _T("EOF")))
	{
	return genMatchelt(elt, _T("match_eof"), 0, gen);
	}
// _xNIL

// TYPE II. SPECIAL MODES THAT MATCH LIKE NORMAL.
else if (!_tcscmp(name, _T("ANY")))	// Can be treated here. Will always match.
	micro = _T("Arun::micro_any");
else if (!_tcscmp(name, _T("ALPHA")))
	micro = _T("Arun::micro_alpha");
else if (!_tcscmp(name, _T("BLANK")))
	micro = _T("Arun::micro_blank");
else if (!_tcscmp(name, _T("CAP")))
	micro = _T("Arun::micro_cap");
else if (!_tcscmp(name, _T("NUM")))
	micro = _T("Arun::micro_num");
else if (!_tcscmp(name, _T("PUNCT")))
	micro = _T("Arun::micro_punct");
else if (!strcmp_i(name, _T("WHITE")))
	micro = _T("Arun::micro_white");
else if (!_tcscmp(name, _T("CTRL")))											// 07/25/00 AM.
	micro = _T("Arun::micro_ctrl");											// 07/25/00 AM.
else if (!_tcscmp(name, _T("CAPLET")))					// 07/10/09 AM.
	micro = _T("Arun::micro_caplet");					// 07/10/09 AM.
else if (!_tcscmp(name, _T("LET")))						// 07/10/09 AM.
	micro = _T("Arun::micro_let");						// 07/10/09 AM.
else if (!_tcscmp(name, _T("FEAT")))
	{
	*fcode << _T("\n// [Error. _xFEAT unimplemented.]\nERROR;") << std::endl;

	std::_t_strstream gerrStr;
	gerrStr << _T("[Error. _xFEAT unimplemented.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
else
	{
	*fcode << _T("\n// [Error. Unknown type=") << name-2 << _T(".]\nERROR;") << std::endl;

	std::_t_strstream gerrStr;
	gerrStr << _T("[Error. Unknown type=") << name-2 << _T(".]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

return genSpecialelt(
			elt,
			special_fn,
			ielt->matchSpecialfn(),
			micro,					// The micro match function.
			gen);
}


/********************************************
* FN:		GENELTLIST
* CR:		05/04/00 AM.
* SUBJ:	Code generation for match,fail,except list.
* RET:	arr - name of generated static array.
* NOTE:	Put sentinels at end of arrays.
*********************************************/

void Ielt::genEltlist(
	Dlist<Iarg> *list,
	_TCHAR *buf,
	Gen *gen)
{
// Ch from ostream.	// 04/04/03 AM.
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
std::_t_ofstream *fdata = gen->passh_;	// 04/04/09 AM.
std::_t_ofstream *fhead = gen->fhead_;

Iarg *arg;
Delt<Iarg> *darg;
_TCHAR buff[MAXSTR+1];
long num = 0;
int count = 0;																	// 06/05/00 AM.
_TCHAR *str;
if (!list || !(darg = list->getFirst()))
	return;
*fdata << _T("static const _TCHAR *") << buf << _T("[] = {");
for (; darg; darg = darg->Right())
	{
	arg = darg->getData();
	str = arg->getStr();
	if (special(str))		// If it's a special token like _xCAP.	// 06/09/00 AM.
		continue;			// Ignore it here.  Handle elsewhere.	// 06/09/00 AM.

	*fdata << _T("_T(\"")
			 << c_str(arg->getStr(), buff, MAXSTR)
			 << _T("\"),");
// VC++ handles very long lines, but only 64K lines per  file.	// 04/04/03 AM.
#ifdef GENPRETTY_
	if (++count >= 10)														// 06/05/00 AM.
		{
		*fdata << std::endl;
		count = 0;
		}
#endif
	}
*fdata << _T("0");
*fdata << _T("};");																// 04/04/03 AM.
Gen::eol(fdata);																// 04/04/03 AM.

// Declare it.
//*fhead << "extern const _TCHAR *" << buf << "[];" << std::endl;
}

/********************************************
* FN:		GENELTSPECIAL
* CR:		06/09/00 AM.
* SUBJ:	Code generation for special tokens in match,fail,except list.
* RET:	arr - name of generated static array.
* NOTE:	Put sentinels at end of arrays.
*********************************************/

void Ielt::genEltspecial(
	Dlist<Iarg> *list,
	_TCHAR *buf,
	Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
std::_t_ofstream *fdata = gen->passh_;	// 04/04/09 AM.
std::_t_ofstream *fhead = gen->fhead_;

Iarg *arg;
Delt<Iarg> *darg;
long num = 0;
int count = 0;
_TCHAR *str;
long index = 0;
if (!list || !(darg = list->getFirst()))
	return;
*fdata << _T("static const int ") << buf << _T("[] = {");
for (; darg; darg = darg->Right())
	{
	arg = darg->getData();
	str = arg->getStr();
	if (!(index = Arun::find_special(str)))
		continue;			// If not a special token, ignore.

	*fdata << index << _T(",");	// Put enum val directly into gen'd array.

#ifdef GENPRETTY_
	if (++count >= 20)
		{
		*fdata << std::endl;
		count = 0;
		}
#endif
	}
*fdata << _T("0");						// NULL TERMINATOR.
*fdata << _T("};");	// 04/04/03 AM.
Gen::eol(fdata);																// 04/04/03 AM.
}


/********************************************
* FN:		GENELTSTARR
* CR:		05/06/00 AM.
* SUBJ:	Code generation for string array.
* RET:	arr - name of generated static array.
* NOTE:	Put sentinels at end of arrays.
*********************************************/

void Ielt::genEltstarr(Starr *starr, _TCHAR *buf, Gen *gen)
{
std::_t_ofstream *fdata = gen->passh_;	// 04/04/09 AM.

if (!starr)
	return;
int len = starr->getLength() + 1;
int tmp = len;
_TCHAR **arr = starr->getArr() - 1;
int count = 0;								// For long line wrapping.	// 06/05/00 AM.
*fdata << _T("static const _TCHAR *") << buf << _T("[") << len << _T("] = {");
while (--tmp)
	{
	*fdata << _T("_T(\"") << *++arr << _T("\"),");
#ifdef GENPRETTY_
	if (++count >= 10)														// 06/05/00 AM.
		{
		*fdata << std::endl;
		count = 0;
		}
#endif
	}
*fdata << _T("0");
*fdata << _T("};");	// 04/04/03 AM.
Gen::eol(fdata);																// 04/04/03 AM.

//*fhead << "extern const _TCHAR *" << buf << "[];" << std::endl;
}


/********************************************
* FN:		GENELTS
* CR:		05/17/00 AM.
* SUBJ:	Generate an array of data for the elts of a rule.
* RET:	arr - name of generated static array.
* NOTE:	Put sentinels at end of arrays.
*********************************************/

void Ielt::genElts(Dlist<Ielt> *phrase, _TCHAR *buf, Gen *gen)
{
// First generate arrays for each element.
genEltarrays(phrase, gen);

std::_t_ofstream *fdata = gen->passh_;	// 04/04/09 AM.
int id = gen->id_;
int recid = gen->recid_;	// 0 if not in a recurse minipass.	// 05/31/00 AM.
int ruleid = gen->ruleid_;
int count = 0;

long rlen = 0;				// Length of regular toks.					// 06/15/00 AM.
long slen = 0;				// Length of special toks.					// 04/29/01 AM.
bool hashed = false;		// If building hash table.					// 04/29/01 AM.
long hlen = 0;				// Length of hash table.					// 04/29/01 AM.

_TCHAR a_attrs[128];
_TCHAR a_match[128];
_TCHAR a_fail[128];
_TCHAR a_except[128];

_TCHAR s_match[128];															// 06/09/00 AM.
_TCHAR s_fail[128];
_TCHAR s_except[128];

_TCHAR buff[MAXSTR+1];

*fdata << _T("static const ELT ") << buf << _T("[] = {");
*fdata << _T("{0,0,{0,0,0,0},{0,0,0,0},{0,0,0,0},-1,-1,0,0,0,false,0}");	
										// Putting in a dummy as first!	// 05/18/00 AM.
Delt<Ielt> *delt;
Ielt *elt;
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	++count;
	elt = delt->getData();
	*fdata << _T(",");		// 04/04/03 AM.
	Gen::nl(fdata);															// 04/04/03 AM.

	*fdata << _T("{_T(\"")	// 04/04/03 AM.
			 << c_str(elt->name_, buff, MAXSTR)
			 << _T("\"),");

	if (elt->layers_ || elt->attrs_)
		_stprintf(a_attrs, _T("attr%d_%d_%d_%d,"), id, recid, ruleid, count);
	else
		_stprintf(a_attrs, _T("0,"));
	*fdata << a_attrs;

	lookList(elt->matches_, /*UP*/ hashed,rlen,slen);				// 04/29/01 AM.
	hlen = (hashed ? (rlen+slen) : 0);									// 04/29/01 AM.
	*fdata << _T("{");			// Start of ELIST.							// 06/15/00 AM.
	if (rlen)																	// 04/29/01 AM.
		_stprintf(a_match, _T("match%d_%d_%d_%d,"), id, recid, ruleid, count);
	else
		_stprintf(a_match, _T("0,"));
	*fdata << a_match;
	if (hashed)				// Will be hashed.							// 06/14/00 AM.
		_stprintf(a_match, _T("ematch%d_%d_%d_%d"), id,recid,ruleid,count);
	else
		_stprintf(a_match, _T("0"));												// 06/14/00 AM.
	*fdata << a_match << _T(",")
//			 << Htab::makesize(rlen)   << ",";	// WORKAROUND.		// 04/29/01 AM.
			 << Htab::makesize(hlen)   << _T(",");							// 04/29/01 AM.
	if (slen)																	// 04/29/01 AM.
		_stprintf(s_match, _T("smatch%d_%d_%d_%d"), id, recid, ruleid, count);
	else
		_stprintf(s_match, _T("0"));
	*fdata << s_match;
	*fdata << _T("},");		// End of ELIST.								// 06/15/00 AM.


	lookList(elt->fails_, /*UP*/ hashed,rlen,slen);					// 04/29/01 AM.
	hlen = (hashed ? (rlen+slen) : 0);									// 04/29/01 AM.
	*fdata << _T("{");			// Start of ELIST.							// 06/15/00 AM.
	if (rlen)																	// 04/29/01 AM.
		_stprintf(a_fail, _T("fail%d_%d_%d_%d,"), id, recid, ruleid, count);
	else
		_stprintf(a_fail, _T("0,"));
	*fdata << a_fail;
	if (hashed)			// Will be hashed.								// 06/14/00 AM.
		_stprintf(a_fail, _T("efail%d_%d_%d_%d"), id,recid,ruleid,count);
	else
		_stprintf(a_fail, _T("0"));												// 06/14/00 AM.
	*fdata << a_fail  << _T(",")
			 << Htab::makesize(hlen)   << _T(",");							// 04/29/01 AM.
	if (slen)																	// 04/29/01 AM.
		_stprintf(s_fail, _T("sfail%d_%d_%d_%d"), id, recid, ruleid, count);
	else
		_stprintf(s_fail, _T("0"));
	*fdata << s_fail;
	*fdata << _T("},");		// End of ELIST.								// 06/15/00 AM.

	lookList(elt->excepts_, /*UP*/ hashed,rlen,slen);				// 04/29/01 AM.
	hlen = (hashed ? (rlen+slen) : 0);									// 04/29/01 AM.
	*fdata << _T("{");			// Start of ELIST.							// 06/15/00 AM.
	if (rlen)																	// 04/29/01 AM.
		_stprintf(a_except, _T("except%d_%d_%d_%d,"), id, recid, ruleid, count);
	else
		_stprintf(a_except, _T("0,"));
	*fdata << a_except;
	if (hashed)			// Will be hashed.								// 06/14/00 AM.
		_stprintf(a_except, _T("eexcept%d_%d_%d_%d"), id,recid,ruleid,count);
	else
		_stprintf(a_except, _T("0"));												// 06/14/00 AM.
	*fdata << a_except << _T(",")
			 << Htab::makesize(hlen)   << _T(",");							// 04/29/01 AM.
	if (slen)																	// 04/29/01 AM.
		_stprintf(s_except, _T("sexcept%d_%d_%d_%d"), id, recid, ruleid, count);
	else
		_stprintf(s_except, _T("0"));
	*fdata << s_except;
	*fdata << _T("},");		// End of ELIST.								// 06/15/00 AM.

	*fdata << elt->min_ << _T(",")
			 << elt->max_ << _T(",");
	if (elt->rename_)
		*fdata << _T("_T(\"")
				 << c_str(elt->rename_, buff, MAXSTR)
				 << _T("\"),");
	else
		*fdata << _T("0,");
// Group appears to be unused in compiled code.						// 01/02/01 AM.
#ifdef UNUSED_
	if (elt->group_)															// 09/23/00 AM.
		*fdata << _T("_T(\"")
				 << c_str(elt->group_, buff, MAXSTR)					// 09/23/00 AM.
				 << _T("\"),");
	else
		*fdata << _T("0,");
#endif

	*fdata << _T("0 /*pres*/,");
	*fdata << _T("0 /*recurse*/,");
//	*fdata << "0 /*hash*/,";
	if (elt->wild())
		*fdata << _T("true,");
	else
		*fdata << _T("false,");
	*fdata << elt->flags_;
	*fdata << _T("}");
	}

*fdata << _T("};");
Gen::eol(fdata);																// 04/04/03 AM.
}



/********************************************
* FN:		GENELTARRAYS
* CR:		05/22/00 AM.
* SUBJ:	Code gen arrays for all of a rule's elements.
* RET:	True if ok.
*********************************************/

bool Ielt::genEltarrays(Dlist<Ielt> *list, Gen *gen)
{
int id = gen->id_;
int recid = gen->recid_;													// 05/31/00 AM.
int ruleid = gen->ruleid_;
std::_t_ofstream *fdata = gen->passh_;	// 04/04/09 AM.

// Generate arrays for lists of ids.
_TCHAR a_attrs[128];
_TCHAR a_match[128];
_TCHAR a_fail[128];
_TCHAR a_except[128];

// Generate arrays for special match tokens (eg, "_xCAP").		// 06/09/00 AM.
_TCHAR s_match[128];															// 06/09/00 AM.
_TCHAR s_fail[128];																// 06/09/00 AM.
_TCHAR s_except[128];															// 06/09/00 AM.

int count = 0;
Delt<Ielt> *delt;
Ielt *elt;
bool hashed;																	// 04/29/01 AM.
long rlen=0;
long slen=0;																	// 04/29/01 AM.
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	++count;
	elt = delt->getData();

	if (elt->layers_)
		{
		_stprintf(a_attrs, _T("attr%d_%d_%d_%d"), id, recid,ruleid, count);
		genEltstarr(elt->layers_, a_attrs, gen);
		}
	else if (elt->attrs_)
		{
		_stprintf(a_attrs, _T("attr%d_%d_%d_%d"), id, recid,ruleid, count);
		genEltlist(elt->attrs_, a_attrs, gen);
		}


	if (elt->matches_)
		{
		lookList(elt->matches_, /*UP*/ hashed,rlen,slen);			// 04/29/01 AM.
		if (rlen)																// 04/29/01 AM.
			{
			_stprintf(a_match, _T("match%d_%d_%d_%d"), id, recid,ruleid, count);
			genEltlist(elt->matches_, a_match, gen);
			}
		if (slen)																// 04/29/01 AM.
			{
			_stprintf(s_match, _T("smatch%d_%d_%d_%d"), id, recid,ruleid, count);
			genEltspecial(elt->matches_, s_match, gen);				// 06/09/00 AM.
			}
		}
	if (elt->fails_)
		{
		lookList(elt->fails_, /*UP*/ hashed,rlen,slen);				// 04/29/01 AM.
		if (rlen)																// 04/29/01 AM.
			{
			_stprintf(a_fail, _T("fail%d_%d_%d_%d"), id, recid,ruleid, count);
			genEltlist(elt->fails_, a_fail,  gen);
			}
		if (slen)																// 04/29/01 AM.
			{
			_stprintf(s_fail, _T("sfail%d_%d_%d_%d"), id, recid,ruleid, count);
			genEltspecial(elt->fails_, s_fail, gen);					// 06/09/00 AM.
			}
		}
	if (elt->excepts_)
		{
		lookList(elt->excepts_, /*UP*/ hashed,rlen,slen);			// 04/29/01 AM.
		if (rlen)																// 04/29/01 AM.
			{
			_stprintf(a_except, _T("except%d_%d_%d_%d"), id, recid,ruleid, count);
			genEltlist(elt->excepts_, a_except,  gen);
			}
		if (slen)																// 04/29/01 AM.
			{
			_stprintf(s_except, _T("sexcept%d_%d_%d_%d"), id, recid,ruleid, count);
			genEltspecial(elt->excepts_, s_except, gen);				// 06/09/00 AM.
			}
		}
	}

return true;
}


/********************************************
* FN:		MATCHFN
* CR:		05/23/00 AM.
* SUBJ:	Decide which match function to use.
* RET:	True if ok.
* NOTE:	For passing match function as funarg in compiled runtime
*			analyzer.
* OPT:	Segmenting into many little functions, so runtime system doesn't
*			have to keep asking "is it this, is it that?".
*********************************************/

_TCHAR *Ielt::matchFn()
{
// Figure out the match function to use.
if (!this->getDeaccent()		// 03/29/05 AM.
 && !this->getDejunk())			// 09/09/11 AM.
	{
	if (matches_)
		{
		if (getTree())
			return _T("Arun::match_list_tree");
		else if (getSinglet())
			return _T("Arun::match_list_singlet");
		return _T("Arun::match_list");
		}
	else if (fails_)
		{
		if (getTree())
			return _T("Arun::match_fails_tree");
		else if (getSinglet())
			return _T("Arun::match_fails_singlet");
		return _T("Arun::match_fails");
		}
	else if (getTree())
		return _T("Arun::match_tree");
	else if (getSinglet())
		return _T("Arun::match_singlet");
	else
		return _T("Arun::match_simple");
	}
else if (this->getDejunk())	// 09/09/11 AM.
	{
	if (matches_)
		{
		if (getTree())
			return _T("Arun::match_list_tree_dejunk");
		else if (getSinglet())
			return _T("Arun::match_list_singlet_dejunk");
		return _T("Arun::match_list_dejunk");
		}
	else if (fails_)
		{
		if (getTree())
			return _T("Arun::match_fails_tree_dejunk");
		else if (getSinglet())
			return _T("Arun::match_fails_singlet_dejunk");
		return _T("Arun::match_fails_dejunk");
		}
	else if (getTree())
		return _T("Arun::match_tree_dejunk");
	else if (getSinglet())
		return _T("Arun::match_singlet_dejunk");
	else
		return _T("Arun::match_simple_dejunk");
	}
else								// 03/29/05 AM.
	{
	if (matches_)
		{
		if (getTree())
			return _T("Arun::match_list_tree_deaccent");
		else if (getSinglet())
			return _T("Arun::match_list_singlet_deaccent");
		return _T("Arun::match_list_deaccent");
		}
	else if (fails_)
		{
		if (getTree())
			return _T("Arun::match_fails_tree_deaccent");
		else if (getSinglet())
			return _T("Arun::match_fails_singlet_deaccent");
		return _T("Arun::match_fails_deaccent");
		}
	else if (getTree())
		return _T("Arun::match_tree_deaccent");
	else if (getSinglet())
		return _T("Arun::match_singlet_deaccent");
	else
		return _T("Arun::match_simple_deaccent");
	}
}


/********************************************
* FN:		MATCHSPECIALFN
* CR:		05/25/00 AM.
* SUBJ:	Decide which special match function to use.
* RET:	True if ok.
* NOTE:	For passing match function as funarg in compiled runtime
*			analyzer.
* OPT:	Segmenting into many little functions, so runtime system doesn't
*			have to keep asking "is it this, is it that?".
*********************************************/

_TCHAR *Ielt::matchSpecialfn()
{
// Figure out the match function to use.
if (!this->getDeaccent()		// 03/29/05 AM.
 && !this->getDejunk())			// 09/09/11 AM.
	{
	if (getTree())
		return _T("Arun::special_tree");
	else if (getSinglet())
		return _T("Arun::special_singlet");
	else
		return _T("Arun::special_simple");
	}
else if (this->getDejunk())	// 09/09/11 AM.
	{
	if (getTree())
		return _T("Arun::special_tree");
	else if (getSinglet())
		return _T("Arun::special_singlet_dejunk");
	else
		return _T("Arun::special_simple_dejunk");
	}
else									// 03/29/05 AM.
	{
	if (getTree())
		return _T("Arun::special_tree");
	else if (getSinglet())
		return _T("Arun::special_singlet_deaccent");
	else
		return _T("Arun::special_simple_deaccent");
	}
}


/********************************************
* FN:		LOOKLIST
* CR:		06/09/00 AM.
* SUBJ:	See if match,fail,except list has special and regular tokens.
* RET:	True if ok.
*********************************************/

void Ielt::lookList(
	Dlist<Iarg> *list,
	/*UP*/
//	bool &regular,																// 04/29/01 AM.
//	bool &special,																// 04/29/01 AM.
	bool &hashed,																// 06/14/00 AM.
	long &rlen,			// # of regular elt names.						// 06/15/00 AM.
	long &slen			// # of special names (eg, _xPUNCT)			// 04/29/01 AM.
	)
{
hashed = false;																// 04/29/01 AM.
rlen = slen = 0;																// 04/29/01 AM.
if (!list)
	return;
Delt<Iarg> *delt;
Iarg *arg;
long index;
//long len = 0;																// 04/29/01 AM.
// Now always traversing the entire list.								// 06/15/00 AM.
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	arg = delt->getData();
	index = Arun::find_special(arg->getStr());
	if (!index)
		{
		++rlen;				// # of regular elt names.					// 06/15/00 AM.
//		regular = true;														// 04/29/01 AM.
		}
	else
		++slen;																	// 04/29/01 AM.
//		special = true;														// 04/29/01 AM.
	}

// NOTE:	This is the old (crude) determination of whether to hash.
// TODO: Determine how many regular strings there are and check against
//			threshold.
if ((rlen + slen) >= HTHRESH)												// 04/29/01 AM.
	hashed = true;

//if (!hashed)																	// 04/29/01 AM.
//	rlen = 0;		// RESET if not producing hash table.			// 04/29/01 AM.
}


/********************************************
* FN:		GENHASH
* CR:		06/14/00 AM.
* SUBJ:	Generate hash table for each rule element in list.
* RET:	True if ok.
*********************************************/

bool Ielt::genHash(Dlist<Ielt> *list, Gen *gen)
{
if (!list)
	return false;
Delt<Ielt> *delt;
Ielt *elt;
gen->eltid_ = 0;
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	++(gen->eltid_);															// 06/14/00 AM.
	elt = delt->getData();
	if (!elt->genHash(gen))
		return false;
	}
return true;
}


/********************************************
* FN:		GENHASH
* CR:		06/14/00 AM.
* SUBJ:	Generate hash table for rule element.
* RET:	True if ok.
*********************************************/

bool Ielt::genHash(Gen *gen)
{
std::_t_ostream *ehash = gen->ehash_;	// File for outputting element hash tables.
std::_t_ostream *edata = gen->edata_;	// File for element hash lists.

// Got to name the tables.
// EMATCH_PASS_REC_RULE_ELT
// EFAIL_
// EEXCEPT_

if (hmatches_)
	hmatches_->gen(_T("ematch"),gen);
if (hfails_)
	hfails_->gen(_T("efail"),gen);
if (hexcepts_)
	hexcepts_->gen(_T("eexcept"),gen);

return true;
}



/*************************      END OF FILE      ******************************/
