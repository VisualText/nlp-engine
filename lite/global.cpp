/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	GLOBAL.CPP
* FILE:	lite\xxx.cpp
* CR:		10/09/98 AM.
* SUBJ:	Global data for Lite.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"								// 02/21/02 AM.
//#include <iostream> // 07/18/03 AM.
#include <math.h>									// 10/08/00 AM.
#include <float.h>								// 12/17/99 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"							// 11/24/99 AM.
//#include "dlist.h"					// 07/07/03 AM.
#include "lite/global.h"
//#include "node.h"	// 07/07/03 AM.
//#include "tree.h"	// 07/07/03 AM.
//#include "lite/nlppp.h"							// 08/22/02 AM.
//#include "lite/parse.h"							// 08/22/02 AM.

// Testing export of a global.				// 11/24/99 AM.
//LITE_API int boo = 33;

// The currently active parse instance.
//Parse *Current_Parse;

// The current pass of an analyzer being INTERNED.
// Moving to class.									// 08/23/02 AM.
//int currseqpass = 0;		// DEBUGGING!!		// 02/27/00 AM.

// Current pass of the operating analyzer.
// Moving to class.									// 08/22/02 AM.
//int currpass = 0;			// DEBUGGING!!		// 02/27/00 AM.

// VC FEATURE. NULL string bombs when printed out.
const _TCHAR *STRNULL = _T("");	// PORT			// 03/08/00 AM.

// 12/01/98 AM.	Trying a global output stream.
// This way, output will be dynamically controlled.
std::_t_ostream *gout = &std::_t_cout;			// Default is standard output.

// 03/22/99 AM.	Global error stream.
// This way, error output will be dynamically controlled.
std::_t_ostream *gerr = &std::_t_cerr;			// Default is standard error.

// Global debug output stream.											// 02/21/02 AM.
//ostream *dbgout = 0;														// 08/26/02 AM.

// 11/29/99 DD. Communication with GUI
#ifdef GEODUCK_
LITE_API GUI gui_;				// 11/29/99 DD.
#endif

// Current line number in pass file (being interned, or executed!)
// Getting rid of globals.													// 08/23/02 AM.
//long Linenum=0;		// Line number of current function.			// 05/17/01 AM.

_TCHAR Errbuf[MAXSTR];	// Buffer for error msgs.						// 05/17/01 AM.

extern char BOM_UTF8[];

/********************************************
* FN:		FILEOUT
* CR:		12/01/98 AM.
* SUBJ:	Set global output to a file.
********************************************/

void fileOut(_TCHAR *fname, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
fout = new std::_t_ofstream(TCHAR2CA(fname), std::ios::out);
#ifdef UNICODE
*fout << BOM_UTF8;
#endif
sout = gout;				// Save current output stream.
gout = fout;				// Bind output to file.
}

// VARIANT. Append eg to VisualText.log logfile.	// 05/14/13 AM.

void fileApp(_TCHAR *fname, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
fout = new std::_t_ofstream(TCHAR2CA(fname), std::ios::app);
#ifdef UNICODE
*fout << BOM_UTF8;
#endif
sout = gout;				// Save current output stream.
gout = fout;				// Bind output to file.
}

#ifdef _JUNKY_
// VARIANT.	// TRYING SOMETHING HERE.	// 02/26/05 AM.
void fileOut(std::_t_strstream *outStr, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
//fout = new std::_t_ofstream(TCHAR2CA(fname), std::ios::out);
//fout = new std::_t_ofstream(outStr->str().c_str, std::ios::out);
sout = gout;				// Save current output stream.
gout = fout;				// Bind output to file.
}
#endif

/********************************************
* FN:		RESETOUT
* CR:		12/01/98 AM.
* SUBJ:	Reset global output.
********************************************/

void resetOut(/*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
gout = sout;				// Restore current output stream.
if (fout && fout != &std::_t_cout)	// 08/20/03 AM.
	delete fout;				// Done with file output stream.
fout = 0;
sout = 0;
}


/********************************************
* FN:		FILEERR
* CR:		03/22/99 AM.
* SUBJ:	Set global error output to a file.
********************************************/

void fileErr(_TCHAR *fname, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
fout = new std::_t_ofstream(TCHAR2CA(fname), std::ios::out);
sout = gerr;				// Save current output stream.
gerr = fout;				// Bind output to file.
}


/********************************************
* FN:		RESETERR
* CR:		03/22/99 AM.
* SUBJ:	Reset global error output.
********************************************/

void resetErr(/*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
gerr = sout;				// Restore current output stream.
if (fout												// 06/16/02 AM.
 && fout != &std::_t_cerr)								// 08/20/03 AM.
	delete fout;			// Done with file output stream.
fout = 0;
sout = 0;
}




/********************************************
* FN:		PEEKHERE
* CR:		11/10/99 AM.
* SUBJ:	For breaking out of loops from VisualText.
* NOTE:	Dave's code.
********************************************/

#ifdef OLD_
void PeekHere()
{
MSG message;

// Allow other processes
if (::PeekMessage(&message,NULL,0,0,PM_REMOVE))
	{
	::TranslateMessage(&message);
	::DispatchMessage(&message);
	}
}
#endif


/********************************************
* FN:		CONFIDENCE
* CR:		12/17/99 AM.
* SUBJ:	Special math for combining percent confidence numbers.
* NOTE:	Used in NLP++ to compose confidence values.
*			P = 100 * (1 -  1/ (1 + E))
*			E = P / (100 - P)
*				where
*			P = percent confidence
*			E = fabricated "evidence metric" that goes from 0 to infinity.
*			Since all this is approximate anyway, round the results of
*			computations.
*			12/21/99 AM. Allowing any value as input!  Normalizing to
*			-100 to +100.
* ALGO:	Convert percentages to evidence.  Add evidence values.
*			Convert back to percentages.  Just like adding logarithms to
*			multiply two numbers!!
********************************************/

int confidence(int pct1, int pct2)
{
if (pct1 < -100)
   pct1 = -100;		// 12/21/99 AM.
if (pct1 > 100)
   pct1 = 100;			// 12/21/99 AM.
if (pct2 < -100)
   pct2 = -100;		// 12/21/99 AM.
if (pct2 > 100)
   pct2 = 100;			// 12/21/99 AM.

double e1, e2, etot;
bool neg1 = false, neg2 = false;
if (pct1 < 0)
	{
	pct1 = -pct1;
	neg1 = true;
	}
if (pct2 < 0)
	{
	pct2 = -pct2;
	neg2 = true;
	}

// If both percents are negative, wipe out confidence.
if (neg1 && neg2)
	return 0;			// Wipe out all confidence.

// Watch for division by zero.
if (pct1 == 100)
	e1 = FLT_MAX;			// Microsoft float limit.  <float.h>
else
	e1 = (double)pct1 / (100.0 - (double)pct1);

if (pct2 == 100)
	e2 = FLT_MAX;			// Microsoft float limit.	<float.h>
else
	e2 = (double)pct2 / (100.0 - (double)pct2);

// If one of the percents is negative, then doing a subtraction.
if (neg1)
	etot = e2 - e1;
else if (neg2)
	etot = e1 - e2;
else						// Composing confidences.
   {
   // tolerance.
	// CHECK FOR OVERFLOW HERE.				// 12/21/99 AM.
	double delta = FLT_MAX - e1;				// 12/21/99 AM.
	if (e2 >= (delta - 0.00001))				// 12/21/99 AM.
		return 100;									// 12/21/99 AM.
	etot = e1 + e2;
	}

if (etot < 0.00001)
	return 0;			// Confidence is wiped out.

// Compute the percentage corresponding to total evidence.
// P = 100 * (1 -  1/ (1 + E))
int ptot = (int) (100.0 * (1.0 - (1.0 / (1.0 + etot))));
return ptot;
}


/********************************************
* FN:		webcoConfidence
* CR:		12/09/98 AM.
* SUBJ:	Compute a confidence value for keyword hits vs total nodes.
* RET:	Int representing percent confidence (0-100%).
* NOTE:	This one is geared to hits vs likely domain words.
*			factor = 3.0  good for vs other domains.
*			factor = 13.0 good for vs total text.
*			User must take care of one column (conf < 10).  Could return
*			a string ;-)
*			08/18/01 AM. Actually, should return float.
********************************************/

int webcoConfidence(long hits, long tot, double factor)
{
double tmp = 0.0;
int fin;

//*fout << "hits=" << hits << " tot=" << tot << std::endl;
tmp = (log(1.0 + factor * ((double)hits/((double)tot + 0.01))))
//		* ((1.0 + (double)tot) / (3.0 + (double)tot))// Lower conf for small text.
		* 100;									// Convert to percent.
fin = (int) tmp;
if (fin >= 100)		// Luke -- don't get cocky.
	fin = 99;			// Besides, I don't want 3 columns.
//*fout << "Confidence=" << fin << std::endl;
return fin;
}


/********************************************
* FN:		ERROUT
* CR:		05/17/01 AM.
* SUBJ:	Print out an error message and return true/false.
* RET:	Return given boolean value.
* NOTE:	Utility function for printing error message.
* ASS:	Assume Linenum has current line number, Errbuf is filled with
*			an error message just created.
********************************************/

bool errOut(
	bool retval,				// Value to return.
	long passnum,				// 05/19/01 AM.
	long linenum				// 05/26/01 AM.
	)
{
//if (!gerr)						// 06/16/02 AM.
//	return retval;

//if (passnum < 0)															// 08/22/02 AM.
//	passnum = currpass;		// Get from global pass count.		// 08/22/02 AM.
//if (linenum < 0)															// 08/22/02 AM.
//	linenum = Linenum;		// Current line number in pat file.	// 08/22/02 AM.

*gerr
		<< passnum << _T(" ")	// Current analyzer pass.
		<< linenum << _T(" ")		// Current line number in pat file.
		<< Errbuf
		<< std::endl;
return retval;
}

// VARIANT.	For Unicode, and to remove global buffer.				// 02/25/05 AM.
bool errOut(
	std::_t_strstream *st,			// Error message to print.
	bool retval,				// Value to return.
	long passnum,
	long linenum
	)
{
*gerr
		<< passnum << _T(" ")	// Current analyzer pass.
		<< linenum << _T(" ")	// Current line number in pat file.
		<< st->str()
		<< std::endl;

// Trying to get rid of Purify mem leak.	// 06/23/05 AM.
// Not checked in unicode config.
#ifndef UNICODE
#ifndef LINUX
st->freeze(false);								// 06/23/05 AM.
st->clear();										// 06/23/05 AM.
#endif
#endif

return retval;
}
