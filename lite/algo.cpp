/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ALGO.CPP
* FILE:	lite\pass.cpp
* CR:		10/12/98 AM.
* SUBJ:	Generic analyzer pass algorithm.
*			10/18/98 AM. Renamed from Pass to Algo.
*
*******************************************************************************/


#include "StdAfx.h"
#include "machine-min.h"	// 10/25/06 AM.
#include "lite/global.h"													// 01/24/01 AM.
#include "inline.h"															// 09/26/01 AM.
#include "slist.h"		// 12/04/98 AM.
#include "dlist.h"					// 07/07/03 AM.
//#include "parse.h"
#include "lite/algo.h"

int Algo::count_ = 0;

#ifdef LINUX
template<>
int Selt<Algo>::count_ = 0;
// 12/04/98 AM. -1 for algos_.
template<>
int Slist<Algo>::count_ = -1;
#else
int Selt<Algo>::count_ = 0;
// 12/04/98 AM. -1 for algos_.
int Slist<Algo>::count_ = -1;
#endif

// List of algorithm classes registered with Lite.
// 12/04/98 AM. Moved from algo.h to increase modularity, since
// I want to export the Algo class.
static Slist<Algo> algos_;					// 12/02/98 AM.
//Slist<Algo> *algos_ = 0;			// 12/02/98 AM.

/********************************************
* FN:		Special Functions for Algo class.
* CR:		10/12/98 AM.
********************************************/


Algo::Algo(			// Constructor.
	_TCHAR *nn		// Pretty name for algorithm.
	//Parse *pp			// Current parse instance.		// 12/04/98 AM.
	)
{
//parse = pp;				// 12/04/98 AM.
name  = nn;
debug_ = false;
#ifndef STABLE_
++count_;
#endif
}

Algo::~Algo()
{
#ifndef STABLE_
--count_;
#endif
}


/********************************************
* FN:		Access functions
* CR:		10/12/98 AM.
********************************************/

//Parse		*Algo::getParse() { return parse; }
_TCHAR		*Algo::getName()	{ return name;  }
bool		 Algo::Debug()		{ return debug_; }


/********************************************
* FN:		Modify functions
* CR:		10/12/98 AM.
********************************************/

//void Algo::setParse(Parse *x) { parse = x; }
void Algo::setName(_TCHAR *x)	{ name  = x; }
void Algo::setDebug(bool x)	{ debug_ = x; }


/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Algo::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Algo::prettyCount(std::_t_ofstream *ostr)
{
if (count_)
	{
	if (ostr)
		*ostr << _T("Active Algo count=") << count_ << std::endl;
	*gout << _T("Active Algo count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Algo count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		General Functions
********************************************/
/********************************************
* FN:		Class-Wide Functions
********************************************/

/********************************************
* FN:		USERALGO
* CR:		12/02/98 AM.
* SUBJ:	Function for user-registered algorithms.
* NOTE:	Class function.
*			Can't have user modify this function.  DLL won't allow any
*			unresolved references.  Will need a function that lets user
*			register a new algorithm class.  Then useralgo will traverse
*			the list of user-registered functions.
*			Would like a more elegant way to duplicate the object pointed
*			to by the Algo base class pointer.
********************************************/

bool Algo::userAlgo(
	_TCHAR *s_algo,			// Algorithm name from sequence file.
	_TCHAR *s_data,			// Data argument from sequence file.
	/*DU*/
	Algo* &algo				// Algorithm class object to return.
	)
{
algo = 0;

if (!s_algo)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[userAlgo: Given no algorithm name.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Traverse list of user-registered algorithms.
// If found, create an instance of the associated algorithm class.
// List will look like Slist<Algo> .
Selt<Algo> *ptr;
Algo *alg;
for (ptr = algos_.getFirst(); ptr; ptr = ptr->Right())
	{
	alg = ptr->getData();
	if (!strcmp_i(s_algo, alg->getName()))
		{
		//cout << "[Found registered algorithm name.]" << std::endl;
		algo = &(alg->dup());		// Duplicate the derived algo object.
		algo->setup(s_data);			// Do any setups for this algorithm.
		return true;
		}
	}

std::_t_strstream gerrStr;
gerrStr << _T("[Unknown analyzer pass=") << s_algo << _T(". Skipping.]")
  << std::ends;
errOut(&gerrStr,false);

return false;
}


/********************************************
* FN:		REGALGO
* CR:		12/02/98 AM.
* SUBJ:	Function for registering algorithms.
* NOTE:	Class function.
* ALLOC:	User creates the algorithm instance to add.
*			Expected to be a derived class of Algo.
********************************************/

bool Algo::regAlgo(
	Algo &algo				// Algorithm class to register.
	)
{
// Could check if algorithm with same name is already on the list.
algos_.rpush(&algo);
return true;
}

/********************************************
* FN:		CLEAN
* CR:		12/04/98 AM.
* SUBJ:	Cleanups for algorithm class.
* NOTE:	Class function.
* ALLOC:	Deletes the algorithms in the global registration list.
********************************************/

void Algo::clean()
{
if (algos_.getFirst())
	{
	Slist<Algo>::DelSlistAndData(algos_.getFirst());
	algos_.setFirst(0);
	algos_.setLast(0);
	}
}

/*************************      END OF FILE      ******************************/
