/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/
/*******************************************************************************
*
*									CON_GEN.C
*
* FILE:	consh.�/con_gen.c
* SUBJ:	Generate concept table code files, from Consh.
* CR:	10/6/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"


#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"

#include "consh/libconsh.h"	// 04/26/99 AM.
#include "consh/Aconsh.h"		// 04/26/99 AM.
#include "consh/cg.h"

#include "gen.h"
//#include "consh.h"
#include "consh_gen.h"
#include "sym_gen.h"
#include "ptr_gen.h"
#include "con_gen.h"

/* Number of con structures to generate per line. */
#define CON_LINE 1


/**************************************************
*						CON_GEN
* FUN:	con_gen
* SUBJ:	Generate code for con table in memory.
* CR:	10/6/95 AM.
* ALG:	Traverse con table, print it out as arrays.
*		Each segment goes into its own file.
*
**************************************************/

void con_gen(
	_TCHAR *dir,		/* Output directory for gend code.		*/
	_TCHAR *tail,		/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
int segs_tot, seg_curr;
long seg_size;
long count, tot;
CON *pseg;
_TCHAR s_nam[PATH],	/* Name of con table file.			*/
     s_tab[16];		/* Name of con table array.			*/
CON **con_segs;

_TCHAR siz[16];

_stprintf(siz, _T("%ld"), consh_CON_SEG_SIZE);

tot = 0;
segs_tot = cg->acon_->con_segs_tot();
seg_size = cg->acon_->con_seg_size();
con_segs = cg->acon_->con_seg_table();
seg_curr = cg->acon_->con_seg_curr();

/* GENERATE FILLED TABLE SEGMENTS */
for (ii = 0; ii <= seg_curr; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_CON_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp"), dir, DIR_SEP, s_tab);
//   if (!file_open(s_nam, "w", &fp))
//      return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

   gen_file_head(fp);
   consh_gen_includes(fp);
   
   pseg = con_segs[ii];
   count = -1;
   if (ii == seg_curr)				/* Last filled segment. */
      seg_size = 1 + cg->acon_->con_seg_p() - pseg;

   if (seg_size)
      {
      gen_array_hd(_T("CON"), s_tab, siz, fp);		/* Start array.	*/
      while (++count < seg_size)
         {
         if (tot >= CON_LINE)
            {
           // *fp << "\\\n";			/* Newline for readability.	*/
				*fp << _T("\n");																			// 06/28/00 AM.
            tot = 0;
            }

         con_gen_struct(pseg, fp, cg);		/* Gen one con table elt.	*/

         tot++;
         if (count+1 < seg_size)
            *fp << _T(",");  
         pseg++;
         }
      gen_array_tl(fp);							/* End array.				*/
      }
   else
      gen_array_def(_T("CON"), s_tab, siz, fp);	/* Array definition. */

   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   }

/* GENERATE EMPTY TABLE SEGMENTS */
for (ii = seg_curr + 1; ii < segs_tot; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_CON_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp"), dir, DIR_SEP, s_tab);
//   if (!file_open(s_nam, "w", &fp))
//      return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

   gen_file_head(fp);
   consh_gen_includes(fp);
   gen_array_def(_T("CON"), s_tab, siz, fp);	/* Array definition. */
   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   }
}


/**************************************************
*						CON_GEN_HDR
* FUN:	con_gen_hdr
* SUBJ:	Generate header file for con table.
* CR:	10/6/95 AM.
*
**************************************************/

void con_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
_TCHAR s_nam[PATH],	/* Name of con table hdr file.		*/
     s_tab[16];		/* Name of con table array.			*/
int segs_tot;

segs_tot = cg->acon_->con_segs_tot();
_stprintf(s_nam, _T("%s%s%s_ini.h%s"), dir, DIR_SEP, consh_CON_BASE, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

*fp << _T("extern bool cc_con_ini(void*);\n") << std::endl;					// 08/15/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   {
   _stprintf(s_tab, _T("%s%d"), consh_CON_BASE, ii);
   gen_array_decl(_T("extern CON"), s_tab, fp);
   }

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}


/**************************************************
*						CON_GEN_INI
* FUN:	con_gen_ini
* SUBJ:	Generate initialization file for con table.
* CR:	10/6/95 AM.
*
**************************************************/

void con_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
_TCHAR s_nam[PATH];	/* Name of con table hdr file.		*/
//char s_tab[16];		/* Name of con table array.			*/
int segs_tot, tcurr;
long toff, tsize, tcount;
_TCHAR *tbase;

tbase		= consh_CON_BASE;
segs_tot	= cg->acon_->con_segs_tot();
toff		= cg->acon_->con_curr_off();
tcurr		= cg->acon_->con_seg_curr();
tsize		= cg->acon_->con_seg_size();
tcount		= cg->acon_->con_count();

_stprintf(s_nam, _T("%s%s%s_ini.cpp%s"), dir, DIR_SEP, tbase, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

consh_gen_includes(fp);
*fp << _T("\nbool cc_con_ini(void *xcg)") << std::endl;						// 05/05/01 AM.
*fp << _T("{") << std::endl;
*fp << _T("CON **segs;\n") << std::endl;
*fp << _T("CG *cg = (CG *) xcg;") << std::endl;									// 08/15/02 AM.
*fp << _T("segs = cg->acon_->con_seg_table();") << std::endl;				// 08/16/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   *fp << _T("segs[") << ii << _T("] = &(")
		 << tbase << ii << _T("[0]);") << std::endl;

*fp << _T("bool ok = cg->acon_->con_hard_ini(") << std::endl;				// 08/15/02 AM.
*fp << _T("   (long) ") << tsize << _T(",\t/* con seg size */") << std::endl;
*fp << _T("   (int) ") << segs_tot << _T(",\t/* con segs tot */") << std::endl;
*fp << _T("   (int) ") << tcurr << _T(",\t/* con seg curr */") << std::endl;
if (toff >= 0)									/* Table not empty. */
   *fp << _T("   &(")
		 << tbase << tcurr
		 << _T("[") << toff << _T("]),\t/* con seg p */") << std::endl;
else											/* Table empty.		*/
   *fp << _T("   &(")
		 << tbase << tcurr << _T("[0]) - 1,\t/* con seg p */") << std::endl;
*fp << _T("   (ID) ") << tcount << _T("\t/* concept count */") << std::endl;

*fp << _T("   );") << std::endl;
*fp << _T("return ok;") << std::endl;												// 05/05/01 AM.
*fp << _T("}") << std::endl;

//if (!file_close(s_nam, fp))
//   return;
delete fp;

}


/**************************************************
*						CON_GEN_STRUCT
* FUN:	con_gen_struct
* SUBJ:	Generate one con struct for con table.
* CR:	10/8/95 AM.
*
**************************************************/

void
con_gen_struct(
	CON *pseg,
	std::_t_ofstream *fp,
	CG *cg
	)
{
int tseg;
long toff;
_TCHAR id_x[16], kind_x[32], val_x[32], flags_x[8], attr_x[32],
     up_x[32], dn_x[32], prev_x[32], next_x[32];

if (pseg->kind)	/* If concept is present. */
   {
   _stprintf(id_x, _T("%ld"), pseg->id);
      
   switch(pseg->kind)
      {
      case cBASIC:
         _tcscpy(kind_x, _T("cBASIC"));																// 06/28/00 AM.
         cg->asym_->sym_off(pseg->v.vname,	/*UP*/ &tseg, &toff);
         if (tseg >= 0)
            _stprintf(val_x, _T("&(%s%d[%ld])"), consh_SYM_BASE, tseg, toff);
         else
            _tcscpy(val_x, _T("SNULL"));
         break;
      case cWORD:
         _tcscpy(kind_x, _T("cWORD"));																// 06/28/00 AM.
         cg->asym_->sym_off(pseg->v.vsym,	/*UP*/ &tseg, &toff);
         if (tseg >= 0)
            _stprintf(val_x, _T("&(%s%d[%ld])"), consh_SYM_BASE, tseg, toff);
         else
            _tcscpy(val_x, _T("SNULL"));
         break;
      case cPROXY:
         _tcscpy(kind_x, _T("cPROXY"));																// 06/28/00 AM.
         cg->acon_->con_off(pseg->v.vcon,	/*UP*/ &tseg, &toff);
         if (tseg >= 0)
            _stprintf(val_x, _T("(SYM *)&(%s%d[%ld])"), consh_CON_BASE, tseg, toff);
         else
            _tcscpy(val_x, _T("CNULL"));
         break;
      case cNUM:
         _tcscpy(kind_x, _T("cNUM"));																	// 06/28/00 AM.
         _stprintf(val_x, _T("%ld"), pseg->v.vnum);
         break;
      case cNO_CON:
      default:
         std::_t_cerr << _T("[con_gen: Bad kind]") << std::endl;
         //prog_exit(1);					// 04/29/99 AM.
			exit(1);								// 04/29/99 AM.
      }

   _stprintf(flags_x, _T("%d"), pseg->flags);

   cg->aptr_->ptr_off(pseg->attrs, /*UP*/ &tseg, &toff);
   if (tseg >= 0)
      _stprintf(attr_x, _T("&(%s%d[%ld])"), consh_PTR_BASE, tseg, toff);
   else
      _tcscpy(attr_x, _T("PNULL"));

	con_gen_addr(pseg->up,cg,	 up_x  );
	con_gen_addr(pseg->dn,cg,	 dn_x  );
	con_gen_addr(pseg->prev,cg, prev_x);
	con_gen_addr(pseg->next,cg, next_x);
   
   /* Generate a con table struct. */
   *fp << _T("{")
		 << id_x		<< _T(",")
		 << kind_x	<< _T(",")
		 << val_x	<< _T(",")
		 << flags_x	<< _T(",")
		 << attr_x	<< _T(",")
		 << up_x		<< _T(",")
		 << dn_x		<< _T(",")
		 << prev_x	<< _T(",")
		 << next_x
		 << _T("}");
	}
else			/* Generate empty struct. */
   {
   *fp << _T("{INULL,NO_CON,SNULL,0,PNULL,CNULL,CNULL,CNULL,CNULL}");
   }
}


/**************************************************
*						CON_GEN_ADDR
* FUN:	con_gen_addr
* SUBJ:	Generate a con table address for given concept.
* CR:		6/28/96 AM.
* RET:	true if concept present.
*			buf - Filled with printable address, eg, "&(Con0[33])".
*
**************************************************/

bool
con_gen_addr(CON *con, CG *cg, /*UP*/ _TCHAR *buf)
{
int tseg;
long toff;

if (con)
	{
	cg->acon_->con_off(con, /*UP*/ &tseg, &toff);
	if (tseg >= 0)
		{
		_stprintf(buf, _T("&(%s%d[%ld])"), consh_CON_BASE, tseg, toff);
		return true;
		}
	}
_tcscpy(buf, _T("CNULL"));
return false;
}



/**************************************************
*						CON_GEN_ENTRY
* FUN:	con_gen_entry
* SUBJ:	Generate a con table address for given concept.
* CR:		7/14/96 AM.
* RET:	true if concept present.
*			buf - Filled with printable address, eg, "&(Con0[33])".
* NOTE:	Like con_gen_addr, but omits ampersand.
*
**************************************************/

bool
con_gen_entry(CON *con, CG *cg, /*UP*/ _TCHAR *buf)
{
int tseg;
long toff;

if (con)
	{
	cg->acon_->con_off(con, /*UP*/ &tseg, &toff);
	if (tseg >= 0)
		{
		_stprintf(buf, _T("%s%d[%ld]"), consh_CON_BASE, tseg, toff);
		return true;
		}
	}
_tcscpy(buf, _T("(CON) NULL"));
return false;
}





