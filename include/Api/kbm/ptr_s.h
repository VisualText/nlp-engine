/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									PTR_S.H
*
* FILE:	include/API/kbm/ptr_s.h
* SUBJ:	Structure definitions for PTR list data structure.
* CR:		9/10/95 AM.
* NOTE:	These are list data structures embedded in the knowledge base.
*			Unlike the LIST structures, which are for temporary use by the
*			running program.  (Though they look the same, they are treated
*			differently.)
*			10/7/95 AM. Code gen needs force substantial change to this structure.
*			Will have a kind field, and value will be a union, so that what the
*			value is will be unambiguous.  Eventually, kind field could be used
*			for additional flags, if desired.
*			One flag could be whether a value is present here. (eg, this lets us
*			use the full range of numbers, with a flag saying whether value is
*			present.)
*
*******************************************************************************/




/**************************************************
*						PKIND
* SUBJ:	Types of PTR values.
* CR:		10/7/95 AM.
**************************************************/

typedef enum ptr_kind
   {
   pNO_PTR,
   pCON,
   pPTR,
   pSYM,
   pST,			// String in string table.
   pNUM,
   pCHAR,
   pID,
	pFLOAT		// Adding FLOAT data type to KB.						// 08/16/01 AM.
   } PKIND;


/**************************************************
*						PVAL
* SUBJ:	PVAL union, representing PTR's value.
* CR:		10/11/95 AM.
* NOTE:	08/16/01 AM. Workaround to Visual C++ problem Q39910.
*			Can't handle initializing union elements other than the first.
*			Only affects code generation for PTR table when value is float.
**************************************************/

typedef union pval
   {
   struct con *vcon;	// kind = pCON
   struct ptr *vptr;	// kind = pPTR
   struct sym *vsym;	// kind = pSYM
   _TCHAR *vst;			// kind = pST	/* String in string table.	*/
   long long vnum;			// kind = pNUM
   _TCHAR vchar;			// kind = pCHAR
	float vfloat;		// kind = pFLOAT									// 08/16/01 AM.
   ID vid;				// kind = pID
   } PVAL;


/**************************************************
*						PTR_S
* SUBJ:	PTR list data structure.
* CR:		9/10/95 AM.
*
**************************************************/

typedef struct ptr
   {
   PKIND kind;
   PVAL v;
   struct ptr *next;
   }PTR;
