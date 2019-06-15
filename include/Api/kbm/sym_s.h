/*******************************************************************************
Copyright © 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									SYM_S.H
*
* FILE:	include/API/kbm/sym_s.h
* SUBJ:	Structure definitions for symbol-table entry.
* NOTE:	
* CR:		5/2/95 AM.
*
*******************************************************************************/



/**************************************************
*						SYM_S
* SUBJ:	Symbol-table data structure.
* CR:		5/2/95 AM.
* NOTE:
* OPT:	Because string table is hard-wired, we use an offset into it.
*			08/30/95 AM. Using pointers instead of offsets.
*
**************************************************/

typedef struct sym
   {
   _TCHAR *str;					/* String in the string table.			*/
   struct sym *chain;		/* Ptr to next sym in conflict chain.	*/
   struct con *con;			/* Concept representing this word.		*/
   } SYM;
