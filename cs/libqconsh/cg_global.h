/*******************************************************************************
Copyright (c) 1999-2000 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	CG_GLOBAL.H
* FILE:	libconsh/cg_global.h
* CR:		09/16/99 AM.
* SUBJ:	Global variables and settings.
*
*******************************************************************************/

#ifndef CG_GLOBAL_H_
#define CG_GLOBAL_H_

// Quick and dirty limit on name sizes.
#define MAXSTR 512

// Limit on filename sizes.
#define FNAME_SIZE MAXSTR

// VC FEATURE.  BOMBS IF TRYING TO PRINT POINTER TO NULL.
// Changed from SNULL to avoid conflict with libkbm.			// 02/15/01 AM.
extern const _TCHAR *strNULL;

extern _t_ostream *cgout;
void cgfileOut(_TCHAR *fname, /*DU*/ _t_ofstream* &fout, _t_ostream* &sout);
void cgresetOut(/*DU*/ _t_ofstream* &fout, _t_ostream* &sout);

//extern ostream *cgerr;
//void cgfileErr(char *fname, /*DU*/ ofstream* &fout, ostream* &sout);
//void cgresetErr(/*DU*/ ofstream* &fout, ostream* &sout);
extern _t_ofstream *cgerr;	// 07/18/03 AM.
void cgfileErr(_TCHAR *fname);	// 07/18/03 AM.
void cgresetErr();	// 07/18/03 AM.

#endif
