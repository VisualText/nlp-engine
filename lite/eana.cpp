/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	EANA.CPP
* FILE:	lite\eana.h
* CR:		10/12/99 AM.
* SUBJ:	Executable analyzer object.
* NOTE:	For now, will hold flags and major analyzer configurations.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"
#include "std.h"
#include "chars.h"
#include "io.h"

#include "Eana.h"

int Eana::count_ = 0;

/********************************************
* FN:		Special functions for class
********************************************/

Eana::Eana(enum Eanaconf conf)
{
configure(conf);

#ifndef STABLE_
++count_;
#endif
}

Eana::~Eana()
{

#ifndef STABLE_
--count_;
#endif
}

/********************************************
* FN:		Access functions
********************************************/

bool	Eana::getFlogfiles()		{return flogfiles_; }
bool	Eana::getFsilent()		{return fsilent_;}					// 06/16/02 AM.
bool	Eana::getFlogfinal()		{return flogfinal_; }
bool	Eana::getFoutfiles()		{return foutfiles_; }
bool	Eana::getFdumpfiles()	{return fdumpfiles_;}
bool	Eana::getFcompile()		{return fcompile_;  }				// 05/10/00 AM.

bool	Eana::getFtimesum()		{return ftimesum_;  }
bool	Eana::getFtimepass()		{return ftimepass_; }

bool	Eana::getFverbose()		{return fverbose_;  }
bool	Eana::getFdebug()			{return fdebug_;	  }
bool	Eana::getFerrout()		{return ferrout_;   }
bool	Eana::getFintern()		{return fintern_;	  }
bool	Eana::getFreadrfb()		{return freadrfb_;  }				// 12/20/01 AM.


/********************************************
* FN:		Modify functions
********************************************/

void	Eana::setFlogfiles(bool x)		{flogfiles_  = x;}
void	Eana::setFsilent(bool x)		{fsilent_	 = x;}			// 06/16/02 AM.
void	Eana::setFlogfinal(bool x)		{flogfinal_  = x;}
void	Eana::setFoutfiles(bool x)		{foutfiles_  = x;}
void	Eana::setFdumpfiles(bool x)	{fdumpfiles_ = x;}
void	Eana::setFcompile(bool x)		{fcompile_	 = x;}			// 05/10/00 AM.
void	Eana::setFtimesum(bool x)		{ftimesum_   = x;}
void	Eana::setFtimepass(bool x)		{ftimepass_  = x;}

void	Eana::setFverbose(bool x)		{fverbose_   = x;}
void	Eana::setFdebug(bool x)			{fdebug_     = x;}
void	Eana::setFerrout(bool x)		{ferrout_    = x;}
void	Eana::setFintern(bool x)		{fintern_	 = x;}
void	Eana::setFreadrfb(bool x)		{freadrfb_	 = x;}			// 12/20/01 AM.

/********************************************
* FN:		General functions
********************************************/

// Note: These can be used as convenient starting points.
// Then can do things like set debug to true where desired.
void Eana::configure(enum Eanaconf conf)
{
fcompile_ = false;															// 05/10/00 AM.

switch (conf)
	{
	case conf_ZILCH:
		flogfiles_  = false;
		fsilent_		= true;													// 06/16/02 AM.
		flogfinal_  = false;
		foutfiles_  = false;
		fdumpfiles_ = false;
		ftimesum_   = false;
		ftimepass_  = false;
		fverbose_   = false;
		fdebug_		= false;
		ferrout_    = false;
		fintern_		= false;													// 05/20/01 AM.
		freadrfb_	= false;													// 12/20/01 AM.
		break;

	case conf_RUG:
		flogfiles_  = false;
		fsilent_		= false;													// 06/16/02 AM.
		flogfinal_  = false;
		foutfiles_  = false;
		fdumpfiles_ = false;
		ftimesum_   = false;
		ftimepass_  = false;
		fverbose_   = false;
		fdebug_		= false;
		ferrout_    = true;
		fintern_		= false;													// 05/20/01 AM.
		freadrfb_	= false;													// 05/20/01 AM.
		break;
	case conf_RFA:		// For RFA read of RFB.							// 12/20/01 AM.
		flogfiles_  = false;
		fsilent_		= false;													// 06/16/02 AM.
		flogfinal_  = false;
		foutfiles_  = false;
		fdumpfiles_ = false;
		ftimesum_   = false;													// 12/21/02 AM.
		ftimepass_  = false;
		fverbose_   = false;
		fdebug_		= false;
		ferrout_    = true;
		fintern_		= true;													// 05/20/01 AM.
		freadrfb_	= true;													// 12/20/01 AM.
		break;
	case conf_INTERN:		// For RFA or RFB to intern analyzer.	// 12/20/01 AM.
		flogfiles_  = false;
		fsilent_		= false;													// 06/16/02 AM.
		flogfinal_  = false;
		foutfiles_  = false;
		fdumpfiles_ = false;
		ftimesum_   = true;
		ftimepass_  = false;
		fverbose_   = false;
		fdebug_		= false;
		ferrout_    = true;
		fintern_		= true;
		freadrfb_	= false;
		break;
	default:
	case conf_REG:
		flogfiles_  = false;

		flogfinal_  = true;
		fsilent_		= false;													// 06/16/02 AM.
		foutfiles_  = true;
		fdumpfiles_ = true;
		ftimesum_   = true;
		ftimepass_  = true;
		fverbose_   = false;
		fdebug_		= false;
		ferrout_    = true;
		fintern_		= false;													// 05/20/01 AM.
		freadrfb_	= false;													// 12/20/01 AM.
		break;
	case conf_DEV:
		flogfiles_  = true;
		fsilent_		= false;													// 06/16/02 AM.
		flogfinal_  = true;
		foutfiles_  = true;
		fdumpfiles_ = true;
		ftimesum_   = true;
		ftimepass_  = true;
		fverbose_   = false;
		fdebug_		= false;
		fintern_		= false;													// 05/20/01 AM.
		freadrfb_	= false;													// 12/20/01 AM.
		ferrout_    = true;
		break;
	case conf_DEBUG:
	case conf_ALL:
		flogfiles_  = true;
		fsilent_		= false;													// 06/16/02 AM.
		flogfinal_  = true;
		foutfiles_  = true;
		fdumpfiles_ = true;
		ftimesum_   = true;
		ftimepass_  = true;
		fverbose_   = true;
		fdebug_		= true;
		ferrout_    = true;
		fintern_		= false;													// 05/20/01 AM.
		freadrfb_	= false;													// 12/20/01 AM.
		break;
	}
}


/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Eana::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Eana::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Eana count=") << count_ << std::endl;
	*gout << _T("Active Eana count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Eana count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif
