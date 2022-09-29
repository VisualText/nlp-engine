/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ISUGG.CPP
* FILE:	lite\isugg.cpp
* CR:		10/24/98 AM.
* SUBJ:	Interpreted/internal suggested rule element class.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"		// 05/19/99 AM.
#include "io.h"
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"			// 06/07/99 AM.
#include "starr.h"		// 05/06/00 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"			// 05/19/00 AM.
#include "ielt.h"			// 05/19/00 AM.
#include "isugg.h"

/********************************************
* FN:		Special Functions for Class
* CR:		10/24/98 AM.
********************************************/

Isugg::Isugg(						// Default constructor.
	_TCHAR *nn,						// Element name.
	Dlist<Ipair> *prs,				// Pairs.
	long num							// Line num in pass file.			// 08/08/01 AM.
	)
	: Ielement(nn, prs,
			0, num)																// 08/08/01 AM.
{
base_ = false;							// 11/10/98 AM.
unsealed_ = false;					// 10/09/99 AM.
layers_   = 0;							// 05/06/00 AM.
// Tracking is in the base class, Ielement.
}

/*******************************************/

Isugg::~Isugg()
{
if (layers_)								// 05/06/00 AM.
	delete layers_;						// 05/06/00 AM.

// Tracking is in the base class, Ielement.
}

/*******************************************/

/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Isugg::Isugg(Isugg &orig)
: Ielement(orig)			// Call parent class' copy constructor.
{
zero();
Isugg::copy(&orig);
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Isugg &Isugg::operator=(const Isugg &fm)
{
Isugg *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Isugg object to itself.]") << std::ends;
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
void Isugg::clear()
{
// 07/02/99 AM.
// WARN: DOESN'T CLEAR FOR THE PARENT CLASS.

base_ = false;
unsealed_ = false;			// 10/09/99 AM.
delete layers_;				// 05/06/00 AM.
}

void Isugg::zero()
{
layers_ = 0;
// WARN: DOESN'T ZERO FOR THE PARENT CLASS.
}

void Isugg::copy(const Isugg *orig)
{
// WARN: DOESN'T COPY THE PARENT CLASS.
Isugg *dest;
dest = this;

dest->base_ = orig->base_;
dest->unsealed_ = orig->unsealed_;
dest->layers_   = orig->layers_;											// 05/06/00 AM.
	// Note: Doesn't copy strings -- only ptrs to strings.		// 05/06/00 AM.
}

/*******************************************/



std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Isugg &sugg)			// 11/23/98 AM.
{
bool flag = false;		// If something prior was printed.

#ifndef UNICODE
output << str(sugg.name_);
#else
char *lpstr8;																	// 01/28/06 AM.
u_to_mbcs((LPCWSTR)str(sugg.name_), CP_UTF8, (LPCTSTR*&)lpstr8);// 01/28/06 AM.
output << lpstr8;																// 01/28/06 AM.
u_delete((LPCTSTR*&)lpstr8);												// 01/28/06 AM.
#endif

output << _T(" [");
if (sugg.base_)
	{
	output << _T("base");
	flag = true;
	}
if (sugg.unsealed_)						// 10/09/99 AM.
	{
	if (flag)
		output << _T(" ");					// separator.
	else
		flag = true;
	output << _T("unsealed");
	flag = true;
	}

if (sugg.attrs_)
	{
	if (flag)
		output << _T(" ");					// separator.
	else
		flag = true;
	output << _T("attrs=(")
			 << *(sugg.attrs_)
			 << _T(")");
	}
else if (sugg.layers_)													// 05/06/00 AM.
	{
	if (flag)
		output << _T(" ");					// separator.
	else
		flag = true;
	output << _T("attrs=(")
			 << *(sugg.layers_)
			 << _T(")");
	}
output << _T("]");

output << _T(" [");
if (sugg.pairs_)
	output << *(sugg.pairs_);
output << _T("]");
return output;
}

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Delt<Isugg> &delt)		// 11/23/98 AM.
{
Isugg *sugg;
sugg  = delt.getData();
output << *sugg;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Isugg> &list)		// 11/23/98 AM.
{
Delt<Isugg> *delt;
delt = list.getFirst();
output << *delt;
while ((delt = delt->Right()))
	{
	output << _T(" ") << *delt;
	}
return output;
}


/*******************************************/


/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
* CR:		10/24/98 AM.
********************************************/

bool	 Isugg::getBase()			{return base_;}
bool	 Isugg::getUnsealed()	{return unsealed_;}					// 10/09/99 AM.
Starr *Isugg::getLayers()		{return layers_;}						// 05/06/00 AM.

/********************************************
* FN:		Modify Functions
* CR:		10/24/98 AM.
********************************************/

void Isugg::setBase(bool x)		{base_   = x;}
void Isugg::setUnsealed(bool x)	{unsealed_ = x;}					// 10/09/99 AM.
void Isugg::setLayers(Starr *x)	{layers_ = x;}						// 05/06/00 AM.


/********************************************
* FN:		General Functions
********************************************/


/********************************************
* FN:		INTERN
* CR:		11/23/98 AM.
* SUBJ:	Internalize the pairs data in a suggested element.
********************************************/

void Isugg::intern(
	bool &info			// Not used currently.
	)
{
Isugg *sugg;

info = false;
sugg = this;

// Interning attrs_ list into layers_ list.						// 05/06/00 AM.
if (sugg->layers_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: suggested elt already interned.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

Dlist<Ipair> *list;
list = sugg->getPairs();
if (!list)
	{
	// Set defaults here.
	//sugg->fillDefaults();			// 10/11/99 AM.
	sugg->setBase(false);			// 10/11/99 AM.
	sugg->setUnsealed(false);		// 10/11/99 AM.
	return;
	}

Delt<Ipair> *dpair;

// For each pair.
for (dpair = list->getFirst(); dpair; dpair = dpair->Right())
	{
	// See if it's something we know about.
	Ipair *pair;
	pair = dpair->getData();
	assert(pair);
	_TCHAR *key;
	key = pair->getKey();
	Dlist<Iarg> *vals;
	vals = pair->getVals();
	if (!strcmp_i(key, _T("layer"))
			|| !strcmp_i(key, _T("layers"))
			|| !strcmp_i(key, _T("attr"))
			|| !strcmp_i(key, _T("attrs"))
			)
		{
		// Move layer names from pair.
		sugg->attrs_ = vals;
		pair->setVals(0);		// So they won't be deleted. // 01/17/99 AM. FIX.
		}
	else if (!strcmp_i(key, _T("base")))
		{
		sugg->base_ = true;
		}
	else if (!strcmp_i(key, _T("unsealed")))						// 10/09/99 AM.
		{
		sugg->unsealed_ = true;									// 10/09/99 AM.
		}
	else
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Invalid key in rule's suggested element=") << key
				  << _T("]") << std::ends;
		errOut(&gerrStr,false);
		}
	}

// DELETE ALL THE PAIRS!
// If they ain't been internalized, they are ignored.
Dlist<Ipair>::DeleteDlistAndData(list);
sugg->setPairs(0);

// Further interning of the layer names.							// 05/06/00 AM.
if (sugg->attrs_)															// 05/06/00 AM.
	{
	// Traverse attrs list, installing into a layers array.
	sugg->setLayers(Iarg::strings_to_starr(sugg->attrs_));	// 05/06/00 AM.

	// SHOULD REMOVE THE ATTRS LIST HERE.  EVERYONE SHOULD
	// USE THE STRING ARRAY VERSION.
	// (Do this when everything else is converted.				// 05/06/00 AM.
	//Dlist<Iarg>::DeleteDlistAndData(attrs_);
	//attrs_ = 0;
	}


// See what hasn't been assigned, and assign it a default value.
//sugg->fillDefaults();		// 10/11/99 AM.
}


/********************************************
* FN:		FILLDEFAULTS
* CR:		11/23/98 AM.
* SUBJ:	Fill some default values for a suggested element.
* NOTE:	11/22/98 AM. Moved here from postrfa.cpp.
********************************************/

void Isugg::fillDefaults()
{
//base_ = false;
//unsealed_ = false;
}


/********************************************
* FN:		GENSUGG
* CR:		05/30/99 AM.
* SUBJ:	Generate text for a rule's suggested element to a file.
* ASS:	Not using PAIRS, but rather the internalized values in sugg.
********************************************/

void Isugg::genSugg(Isugg *sugg, std::_t_ostream &ofile)
{
if (!sugg)
	return;
Iarg::genName(sugg->getName(), ofile);

//ofile << " [";				// 10/11/99 AM.

bool found = false;			// If any pairs were seen yet.	// 10/11/99 AM.

if (sugg->getBase())
	{
	if (found)
		ofile << _T(" ");
	else
		ofile << _T(" [");			// 10/11/99 AM.
	ofile << _T("base");
	found = true;
	}
if (sugg->getUnsealed())					// 10/11/99 AM.
	{
	if (found)
		ofile << _T(" ");
	else
		ofile << _T(" [");			// 10/11/99 AM.
	ofile << _T("unsealed");
	found = true;
	}
Dlist<Iarg> *dargs;
Starr *starr;
if ((starr = sugg->getLayers()))		// Interned attrs.			// 05/06/00 AM.
	{
	_TCHAR **arr = starr->getArr() - 1;	// One BEFORE array.
	int len = starr->getLength() + 1;	// One greater than length.
	if (found)
		ofile << _T(" ");
	else
		ofile << _T(" [");
	ofile << _T("layer=(");
	while (--len)
		ofile << *++arr << _T(" ");
	ofile << _T(")");
	found = true;
	}
else if ((dargs = sugg->getAttrs()))	// Uninterned, for some reason.
	{
	// Layering attribute.
	if (found)
		ofile << _T(" ");
	else
		ofile << _T(" [");			// 10/11/99 AM.
	Delt<Iarg> *darg;
	Iarg *arg;
	ofile << _T("layer=(");
	for (darg = dargs->getFirst(); darg; darg = darg->Right())
		{
		arg = darg->getData();
		ofile << arg->getStr() << _T(" ");
		}
	ofile << _T(")");
	found = true;
	}
if (found)							// 10/11/99 AM.
	ofile << _T("]");					// 10/11/99 AM.
ofile << std::flush;					// 10/11/99 AM.
}


/********************************************
* FN:		SAME
* CR:		05/30/99 AM.
* SUBJ:	If suggested elements are the same in form.
********************************************/

bool Isugg::same(Isugg *sugg1, Isugg *sugg2)
{
if (!sugg1 && !sugg2)
	return true;
if (!sugg1 || !sugg2)
	return false;

if (sugg1->getBase() != sugg2->getBase())
	return false;
if (sugg1->getUnsealed() != sugg2->getUnsealed())		// 10/09/99 AM.
	return false;

if (!str_equal(sugg1->getName(), sugg2->getName()))
	return false;

// Need to check both.  Perhaps there should be an "interned" flag.
if (!Starr::same(sugg1->getLayers(), sugg2->getLayers()))		// 05/06/00 AM.
	return false;																// 05/06/00 AM.
if (!Iarg::same(sugg1->getAttrs(), sugg2->getAttrs()))
	return false;

return true;
}


/********************************************
* FN:		GEN
* CR:		05/19/00 AM.
* SUBJ:	Gen compiled runtime data for suggested elt.
********************************************/

bool Isugg::gen(_TCHAR *asugg, Gen *gen,
	int passnum,	// Pass number of current rule.					// 08/09/02 AM.
	long ruleline	// Line number of current rule.					// 08/09/02 AM.
	)
{
//std::_t_ofstream *fdata = gen->fdata_;
std::_t_ofstream *fdata = gen->passh_;	// 04/04/09 AM.

_TCHAR a_attrs[MAXSTR];
if (layers_)
	{
	_stprintf(a_attrs, _T("attr%d_%d_%d"), gen->id_,gen->recid_, gen->ruleid_);
	Ielt::genEltstarr(layers_, a_attrs, gen);
	}
else
	_stprintf(a_attrs, _T("0"));

// Now gen a sugg structure.
*fdata << _T("const SUGG ")
		 << asugg << _T("={")
		 << _T("_T(\"") << name_ << _T("\"),")
		 << a_attrs << _T(",")
		 << (base_ ? _T("true") : _T("false")) << _T(",")
		 << (unsealed_ ? _T("true") : _T("false")) << ','
		 << passnum << _T(",")													// 08/09/02 AM.
		 << ruleline															// 08/09/02 AM.
		 << _T("};");	// 04/04/03 AM.

Gen::eol(fdata);																// 04/04/03 AM.

return true;
}

/*************************      END OF FILE      ******************************/
