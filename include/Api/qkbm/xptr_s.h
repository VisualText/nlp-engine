/*******************************************************************************
Copyright (c) 2007-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XPTR_S.H
* FILE:	include/cs/qkbm/xptr_s.h
* CR:		02/07/07 AM.
* SUBJ:	XPTR_S structure definition.
* NOTE:	QDBM implementation of Conceptual Grammar KBMS.
*			Using unique numbers to identify objects, rather than pointers.
*
*******************************************************************************/

#ifndef XPTR_S_H_
#define XPTR_S_H_


/**************************************************
*						XPKIND
* SUBJ:	Types of XPTR values.
**************************************************/

typedef enum xptr_kind
   {
   pNO_PTR,
   pXCON,		// Concept index.
   pXPTR,		// Ptr index.
   pXSTR,		// Str index.
   pNUM,			// Raw number.
   pCHAR,		// Raw char.
	pFLOAT		// Raw float.
   } XPKIND;


/**************************************************
*						XPVAL
* SUBJ:	XPVAL union, representing PTR's value.
**************************************************/

typedef union xpval
   {
   int    xcon;	// kind = pXCON.
   int    xptr;	// kind = pXPTR.
   int    xstr;	// kind = pXSTR.
   long   vnum;	// kind = pNUM.
   _TCHAR vchar;	// kind = pCHAR.
   float  vfloat;	// kind = pFLOAT.
   } XPVAL;


/**************************************************
*						XPTR_S
* SUBJ:	XPTR data structure.
* CR:		9/10/95 AM.
*
**************************************************/

typedef struct xptr_s
   {
   XPKIND kind;
   XPVAL v;
   int next;
   }XPTR_S;



#endif
/*************************      END OF FILE      ******************************/
