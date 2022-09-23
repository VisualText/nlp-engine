/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SEM.CPP
* FILE:	lite\sem.cpp
* CR:		11/05/98 AM.
* SUBJ:	Node's semantic information.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "sem.h"

int Sem::count_ = 0;


/********************************************
* FN:		Special Functions for Sem class.
********************************************/


Sem::Sem(			// Constructor.
	)
{
#ifndef STABLE_
++count_;
//*gerr << (long)this << "+" << count_ << std::endl;
#endif
}

Sem::~Sem()
{
#ifndef STABLE_
--count_;
//*gerr << (long)this << "-" << count_ << std::endl;
#endif
}


/********************************************
* FN:		Access functions
********************************************/


/********************************************
* FN:		Modify functions
********************************************/


/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Sem::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Sem::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Sem count=") << count_ << std::endl;
	*gout << _T("Active Sem count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Sem count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif
