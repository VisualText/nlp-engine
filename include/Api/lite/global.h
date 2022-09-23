/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	GLOBAL.H
* FILE:	include/API/lite/global.h
* CR:		10/09/98 AM.
* SUBJ:	Global data for main VisualText runtime library.
*
*******************************************************************************/

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdlib.h>


#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <iostream>											// Upgrade.	// 01/24/01 AM.
#include <fstream>											// Upgrade.	// 01/24/01 AM.
#endif

// INCONSISTENCY WITH MS VISUAL C++ ON VARIOUS PCS.				// 02/02/01 AM.
#ifdef NONSTD_
#define STDOPERATOR	operator
#elif LINUX
#define STDOPERATOR	operator
#else
#define STDOPERATOR	operator
#endif

#ifndef CONCEPT
typedef void CONCEPT;				// A concept.
typedef void ATTR;					// An attribute.
typedef void VAL;						// An attribute value.
typedef void PHRASE;					// A concept's phrase of nodes.
#endif

#include <ctype.h>
#include <string.h>
#include "Gui.h"
#include "lite.h"																// 11/24/99 AM.

// LINUX PORT.																	// 03/06/00 AM.
#ifdef LINUX
#define stricmp strcasecmp
#endif

//extern int currseqpass;													// 08/23/02 AM.
//extern int currpass;														// 08/23/02 AM.


// Quick and dirty limit on name sizes.								// 11/10/98 AM.
#define MAXSTR 512
#define MAXMSG 2048                                                 //02/10/21 Dd.
#define MAXPATH 2048                                                //02/10/21 Dd.

// Handling very long lines.												// 07/26/01 AM.
//#define MAXLINE 5120
#define MAXLINE 20000

// 11/19/98 AM. Limit on filename sizes.
#define FNAME_SIZE MAXSTR


// SIZE OF HASH TABLE FOR USER-DEFINED NLP++ FUNCTIONS.			// 12/20/01 AM.
#define HTFUNC_SIZE 1001													// 12/20/01 AM.

// MAX NESTING DEPTH IN NLP++ FUNCTION CALLS.						// 03/12/02 AM.
//#define MAX_DEPTH  100
// 10/05/04 AM.
#define MAX_DEPTH 80

// VC FEATURE.  BOMBS IF TRYING TO PRINT POINTER TO NULL.
extern const _TCHAR *STRNULL;									// PORT.	// 03/08/00 AM.

extern std::_t_ostream *gout;
void fileOut(_TCHAR *fname, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout);
void fileApp(_TCHAR *fname, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout); // 05/14/13 AM.
//void fileOut(std::_t_strstream*, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout);
void resetOut(/*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout);

extern std::_t_ostream *gerr;														// 03/22/99 AM.
void fileErr(_TCHAR *fname, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout);
void resetErr(/*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout);

// A global verbose, debug, timing, stream.							// 02/21/02 AM.
//extern ostream *dbgout;													// 08/26/02 AM.

// For breaking out of analyzer and RUG from VisualText.			// 11/10/99 AM.
extern LITE_API GUI gui_;													// 11/10/99 AM.
extern void PeekHere();														// 11/10/99 AM.

extern int confidence(int pct1, int pct2);							// 12/17/99 AM.
int webcoConfidence(long hits, long tot, double factor);			// 10/08/00 AM.

//extern long Linenum;														// 08/23/02 AM.
extern LITE_API _TCHAR Errbuf[];											// 05/18/01 AM.
extern LITE_API bool errOut(bool,										// 05/26/01 AM.
						 long=0,													// 08/22/02 AM.
						 long=0);												// 08/22/02 AM.

extern LITE_API bool errOut(                                   // 02/25/05 AM.
                   std::_t_strstream *,
                   bool,
                   long=0,
                   long=0);

#endif
