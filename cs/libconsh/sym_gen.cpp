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
*									SYM_GEN.C
*
* FILE:	consh.�/sym_gen.c
* SUBJ:	Generate symbol and conflict table code files, from Consh.
* CR:	10/1/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"
#include "prim/list_s.h"
#include "prim/list.h"


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
#include "consh_gen.h"
#include "st_gen.h"
#include "sym_gen.h"
#include "con_gen.h"
#include "ptr_gen.h"
//#include "consh.h"

/* Number of sym structures to generate per line. */
#define SYM_LINE 1


/**************************************************
*						SYM_GEN
* FUN:	sym_gen
* SUBJ:	Generate code for symbol table in memory.
* CR:	10/4/95 AM.
* ALG:	Traverse symbol table, print it out as arrays.
*		Each segment goes into its own file.
*
**************************************************/

void sym_gen(
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
SYM *pseg;
_TCHAR s_nam[PATH],	/* Name of sym table file.			*/
     s_tab[16];		/* Name of sym table array.			*/
SYM **sym_segs;
_TCHAR siz[16];

_stprintf(siz, _T("%ld"), consh_SYM_SEG_SIZE);

tot = 0;
segs_tot = cg->asym_->sym_segs_tot();
seg_size = cg->asym_->sym_seg_size();
sym_segs = cg->asym_->sym_seg_table();
seg_curr = cg->asym_->sym_seg_curr();

/* GENERATE FILLED TABLE SEGMENTS */
for (ii = 0; ii <= seg_curr; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_SYM_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp%s"), dir, DIR_SEP, s_tab, tail);
   //if (!file_open(s_nam, "w", &fp))
   //   return;
	fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

   gen_file_head(fp);
   consh_gen_includes(fp);
   
   pseg = sym_segs[ii];
   count = -1;
   if (ii == seg_curr)				/* Last filled segment. */
      seg_size = 1 + cg->asym_->sym_seg_p() - pseg;

   if (seg_size)
      {
      gen_array_hd(_T("SYM"), s_tab, siz, fp);		/* Start array.	*/
      while (++count < seg_size)
         {
         if (tot >= SYM_LINE)
            {
//            *fp << "\\\n";			/* Newline for readability.	*/
				*fp << _T("\n");																			// 06/28/00 AM.
            tot = 0;
            }
      
         sym_gen_struct(pseg, fp, cg);
      
         tot++;
         if (count+1 < seg_size)
            *fp << _T(",");  
         pseg++;
         }
      gen_array_tl(fp);							/* End array.				*/
      }
   else
      gen_array_def(_T("SYM"), s_tab, siz, fp);

   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   }

/* GENERATE EMPTY TABLE SEGMENTS */
for (ii = seg_curr + 1; ii < segs_tot; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_SYM_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp"), dir, DIR_SEP, s_tab);
   //if (!file_open(s_nam, "w", &fp))
   //   return;
	fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

   gen_file_head(fp);
   consh_gen_includes(fp);
   gen_array_def(_T("SYM"), s_tab, siz, fp);	/* Array definition. */
   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   }
}


/**************************************************
*						SYM_GEN_HDR
* FUN:	sym_gen_hdr
* SUBJ:	Generate header file for sym table.
* CR:	5/20/95 AM.
*
**************************************************/

void sym_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
_TCHAR s_nam[PATH],	/* Name of sym table hdr file.		*/
     s_tab[16];		/* Name of sym table array.			*/
int segs_tot;

segs_tot = cg->asym_->sym_segs_tot();
_stprintf(s_nam, _T("%s%s%s_ini.h%s"), dir, DIR_SEP, consh_SYM_BASE, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

*fp << _T("extern bool cc_sym_ini(void*);\n") << std::endl;					// 08/15/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   {
   _stprintf(s_tab, _T("%s%d"), consh_SYM_BASE, ii);
   gen_array_decl(_T("extern SYM"), s_tab, fp);
   }

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}


/**************************************************
*						SYM_GEN_INI
* FUN:	sym_gen_ini
* SUBJ:	Generate initialization file for sym table.
* CR:	5/20/95 AM.
*
**************************************************/

void sym_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
_TCHAR s_nam[PATH];	/* Name of sym table hdr file.		*/
//char s_tab[16];		/* Name of sym table array.			*/
int segs_tot, tcurr;
long toff, tsize;
_TCHAR *tbase;

tbase		= consh_SYM_BASE;
segs_tot	= cg->asym_->sym_segs_tot();
toff		= cg->asym_->sym_curr_off();
tcurr		= cg->asym_->sym_seg_curr();
tsize		= cg->asym_->sym_seg_size();

_stprintf(s_nam, _T("%s%s%s_ini.cpp"), dir, DIR_SEP, tbase);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

consh_gen_includes(fp);
*fp << _T("\nbool cc_sym_ini(void *xcg)") << std::endl;						// 08/15/02 AM.
*fp << _T("{") << std::endl;
*fp << _T("CG *cg = (CG *) xcg;") << std::endl;									// 08/15/02 AM.
*fp << _T("SYM **segs;\n") << std::endl;
*fp << _T("segs = cg->asym_->sym_seg_table();") << std::endl;				// 08/15/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   *fp << _T("segs[") << ii << _T("] = &(")
		 << tbase << ii << _T("[0]);")
		 << std::endl;

*fp << _T("bool ok = cg->asym_->sym_hard_ini(") << std::endl;				// 08/16/02 AM.
*fp << _T("   (long) ") << tsize << _T(",\t// sym seg size") << std::endl;
*fp << _T("   (long) ")
	 << cg->asym_->sym_hash_size() << _T(",\t// sym hash size")
	 << std::endl;
*fp << _T("   (int) ")
	 << cg->asym_->sym_hash_tot() << _T(",\t// sym hash tot")
	 << std::endl;

// Total hard-coded segments == total segments in sym table.	// 02/16/01 AM.
*fp << _T("   (int) ") << segs_tot << _T(",\t// hard segs tot") << std::endl; // 02/16/01 AM.

*fp << _T("   (int) ") << segs_tot << _T(",\t// sym segs tot") << std::endl;
*fp << _T("   (int) ") << tcurr << _T(",\t// sym seg curr") << std::endl;
if (toff >= 0)									/* Table not empty. */
   *fp << _T("   &(")
		 << tbase << tcurr
		 << _T("[") << toff << _T("])\t// sym seg p")
		 << std::endl;
else											/* Table empty.		*/
   *fp << _T("   &(")
		 << tbase << tcurr
		 << _T("[0]) - 1\t// sym seg p")
		 << std::endl;

*fp << _T("   );") << std::endl;
*fp << _T("return ok;") << std::endl;												// 05/05/01 AM.
*fp << _T("}") << std::endl;

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}


/**************************************************
*						SYM_GEN_STRUCT
* FUN:	sym_gen_struct
* SUBJ:	Generate one sym struct for sym table.
* CR:	10/8/95 AM.
*
**************************************************/

void
sym_gen_struct(
	SYM *pseg,
	std::_t_ofstream *fp,
	CG *cg
	)
{
int tseg;
long toff;
_TCHAR st_x[32], sym_x[32], con_x[32];

if (pseg->str)
   {
   cg->ast_->st_off(pseg->str, /*UP*/ &tseg, &toff);
   if (tseg >= 0)
      _stprintf(st_x, _T("&(%s%d[%ld])"), consh_ST_BASE, tseg, toff);
   else
      {
      std::_t_cerr << _T("[sym_gen: Kb error for str=")
			  << pseg->str << _T(".]")
			  << std::endl;
      _stprintf(st_x, _T("cNULL"));
      }

   cg->asym_->sym_off(pseg->chain,	/*UP*/ &tseg, &toff);
   if (tseg >= 0)
      _stprintf(sym_x, _T("&(%s%d[%ld])"), consh_SYM_BASE, tseg, toff);
   else
      _stprintf(sym_x, _T("SNULL"));

   cg->acon_->con_off(pseg->con,		/*UP*/ &tseg, &toff);
   if (tseg >= 0)
      _stprintf(con_x, _T("&(%s%d[%ld])"), consh_CON_BASE, tseg, toff);
   else
      _stprintf(con_x, _T("CNULL"));

   /* Generate a sym table struct. */
   *fp << _T("{ ") << st_x << _T(", ") << sym_x << _T(", ") << con_x << _T(" }");
   }
else			/* Generate empty struct. */
   {
   *fp << _T("{ cNULL, SNULL, CNULL }");
   }
}
