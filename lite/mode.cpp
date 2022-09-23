/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MODE.CPP
* FILE:	lite/mode.cpp
* CR:		07/19/99 AM.
* SUBJ:	Mode data for Gram concept hierarchy.
* NOTE:	Mode data passes information down the Gram hierarchy. It includes
*			flags for the levels of generalization of rules from samples.
*			Rules for each flagged level will be written to rules files and
*			retained internally.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "iarg.h"							// 12/10/99 AM.
#include "mode.h"

int Mode::count_ = 0;

// WARN! Numbers are octal.
static const int bitLIT = 01;				// Keep literal rules.
static const int bitCONSTRAIN = 02;		// Keep constrained rules.
static const int bitSPLIT = 04;			// Keep split rules.
static const int bitGEN = 010;			// Keep generalized rules.
static const int bitOPT = 020;			// Keep rules w optionals.
static const int bitCLOSED = 040;		// Closed-set on rule elts.
static const int bitXWHITE = 0100;		// Generalize whitespace.	// 08/09/99 AM.
static const int bitLOCKED = 0200;		// Locked from rule gen.	// 08/25/99 AM.
static const int bitDIRTY = 0400;		// Dirty for RUG.				// 08/27/99 AM.
static const int bitRAW = 01000;			// Retain RAW rules.			// 10/31/99 AM.
static const int bitQUICKSEM = 02000;	// Quick semantic copy.		// 12/27/99 AM.
static const int bitCOUNTER = 04000;	// Rule-match counter.		// 11/27/00 AM.
static const int bitNOTRIG = 010000;	// No auto-trigger gen.		// 12/08/00 AM.
// 02, 04, 010, 020, 040, 0100...

// Forward references.

/********************************************
* FN:		Special Functions for Class
********************************************/

Mode::Mode(				// Default constructor.
	)
{
flags_ = 0;				// Initialize bit-flags.
layer_ = 0;				// 12/10/99 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Mode::~Mode()
{
Mode::clear();

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Mode::Mode(Mode &orig)
{
zero();
Mode::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Mode &Mode::operator=(const Mode &fm)
{
Mode *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Mode object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}

/*******************************************/
/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Mode &mode)
{
output << _T("mode(");
if (mode.getLit())
	output << _T("lit ");
if (mode.getRaw())				// 10/29/99 AM.
	output << _T("raw ");				// 10/29/99 AM.
if (mode.getConstrain())
	output << _T("constrain ");
if (mode.getSplit())
	output << _T("split ");
if (mode.getOpt())
	output << _T("opt ");
if (mode.getClosed())
	output << _T("closed");
if (mode.getQuicksem())			// 12/27/99 AM.
	output << _T("quicksem");
if (mode.getCounter())			// 11/27/00 AM.
	output << _T("counter");			// 11/27/00 AM.
if (mode.getNotrig())			// 12/08/00 AM.
	output << _T("notrig");			// 12/08/00 AM.
output << _T("]");
return output;
}

/*******************************************/
void Mode::clear()
{
if (layer_)												// FIX!		// 12/11/99 AM.
	Dlist<Iarg>::DeleteDlistAndData(layer_);					// 12/11/99 AM.
}

void Mode::zero()
{
flags_ = 0;
layer_ = 0;						// 12/10/99 AM.
}

void Mode::copy(const Mode *orig)
{
Mode *dest;
dest = this;

dest->flags_ = orig->flags_;
// No slist assign operator.													// 12/10/99 AM.
if (orig->layer_)						// FIX									// 12/10/99 AM.
	dest->layer_ = new Dlist<Iarg>(*orig->layer_); // Copy constr	// 12/10/99 AM.
}

/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

int          Mode::getFlags()		{return flags_;}
Dlist<Iarg>	*Mode::getLayer()		{return layer_;}					// 12/10/99 AM.

bool Mode::getLit() const	{return (flags_ & bitLIT);}
bool Mode::getRaw() const							// 10/29/99 AM.
{return (flags_ & bitRAW) ? true : false;}	// 10/29/99 AM.
bool Mode::getConstrain() const
{return (flags_ & bitCONSTRAIN) ? true : false;}
bool Mode::getSplit() const
{return (flags_ & bitSPLIT) ? true : false;}
bool Mode::getGen() const
{return (flags_ & bitGEN) ? true : false;}
bool Mode::getOpt() const
{return (flags_ & bitOPT) ? true : false;}
bool Mode::getClosed() const
{return (flags_ & bitCLOSED) ? true : false;}
bool Mode::getXwhite() const
{return (flags_ & bitXWHITE) ? true : false;}
bool Mode::getLocked() const
{return (flags_ & bitLOCKED) ? true : false;}
bool Mode::getDirty() const
{return (flags_ & bitDIRTY) ? true : false;}
bool Mode::getQuicksem() const							// 12/27/99 AM.
{return (flags_ & bitQUICKSEM) ? true : false;}
bool Mode::getCounter() const								// 11/27/00 AM.
{return (flags_ & bitCOUNTER) ? true : false;}
bool Mode::getNotrig() const								// 12/08/00 AM.
{return (flags_ & bitNOTRIG) ? true : false;}		// 12/08/00 AM.

/********************************************
* FN:		Modify Functions
********************************************/

void Mode::setFlags(int x)				{flags_  = x;}
void Mode::setLayer(Dlist<Iarg> *x)	{layer_  = x;}	// 12/10/99 AM.

void Mode::setRaw(bool x)									// 10/29/99 AM.
{
if (x)
	flags_ |= bitRAW;											// 10/29/99 AM.
else
	flags_ &= (~bitRAW);										// 10/29/99 AM.
}

void Mode::setLit(bool x)
{
if (x)
	flags_ |= bitLIT;
else
	flags_ &= (~bitLIT);
}

void Mode::setConstrain(bool x)
{
if (x)
	flags_ |= bitCONSTRAIN;
else
	flags_ &= (~bitCONSTRAIN);
}

void Mode::setSplit(bool x)
{
if (x)
	flags_ |= bitSPLIT;
else
	flags_ &= (~bitSPLIT);
}

void Mode::setGen(bool x)
{
if (x)
	flags_ |= bitGEN;
else
	flags_ &= (~bitGEN);
}

void Mode::setOpt(bool x)
{
if (x)
	flags_ |= bitOPT;
else
	flags_ &= (~bitOPT);
}

void Mode::setClosed(bool x)
{
if (x)
	flags_ |= bitCLOSED;
else
	flags_ &= (~bitCLOSED);
}

void Mode::setXwhite(bool x)
{
if (x)
	flags_ |= bitXWHITE;
else
	flags_ &= (~bitXWHITE);
}

void Mode::setLocked(bool x)
{
if (x)
	flags_ |= bitLOCKED;
else
	flags_ &= (~bitLOCKED);
}

void Mode::setDirty(bool x)
{
if (x)
	flags_ |= bitDIRTY;
else
	flags_ &= (~bitDIRTY);
}

void Mode::setQuicksem(bool x)				// 12/27/99 AM.
{
if (x)
	flags_ |= bitQUICKSEM;
else
	flags_ &= (~bitQUICKSEM);
}

void Mode::setCounter(bool x)					// 11/27/00 AM.
{
if (x)
	flags_ |= bitCOUNTER;
else
	flags_ &= (~bitCOUNTER);
}

void Mode::setNotrig(bool x)					// 12/08/00 AM.
{
if (x)
	flags_ |= bitNOTRIG;
else
	flags_ &= (~bitNOTRIG);
}

/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Mode::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Mode::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Mode count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Mode count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		General Functions
********************************************/

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/*************************      END OF FILE      ******************************/
