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
*									PTR_GEN.C
*
* FILE:	consh.�/ptr_gen.c
* SUBJ:	Generate ptr table code files, from Consh.
* CR:	10/8/95 AM.
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
#include "consh_gen.h"
#include "st_gen.h"
#include "sym_gen.h"
#include "ptr_gen.h"
#include "con_gen.h"
//#include "consh.h"

/* Number of ptr structures to generate per line. */
#define PTR_LINE 1


/**************************************************
*						PTR_GEN
* FUN:	ptr_gen
* SUBJ:	Generate code for ptr table in memory.
* CR:	10/6/95 AM.
* ALG:	Traverse ptr table, print it out as arrays.
*		Each segment goes into its own file.
*
**************************************************/

void ptr_gen(
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
PTR *pseg;
_TCHAR s_nam[PATH],	/* Name of ptr table file.			*/
     s_tab[16];		/* Name of ptr table array.			*/
PTR **ptr_segs;
_TCHAR siz[16];
//long v_num;

_stprintf(siz, _T("%ld"), consh_PTR_SEG_SIZE);

tot = 0;
segs_tot = cg->aptr_->ptr_segs_tot();
seg_size = cg->aptr_->ptr_seg_size();
ptr_segs = cg->aptr_->ptr_seg_table();
seg_curr = cg->aptr_->ptr_seg_curr();

/* GENERATE FILLED TABLE SEGMENTS */
for (ii = 0; ii <= seg_curr; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_PTR_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp%s"), dir, DIR_SEP, s_tab, tail);
   //if (!file_open(s_nam, "w", &fp))
   //   return;
	fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

   gen_file_head(fp);
   consh_gen_includes(fp);
   
   pseg = ptr_segs[ii];
   count = -1;
   if (ii == seg_curr)				/* Last filled segment. */
      seg_size = 1 + cg->aptr_->ptr_seg_p() - pseg;

   if (seg_size)					/* Seg is not empty.	*/
      {
      gen_array_hd(_T("PTR"), s_tab, siz, fp);		/* Start array.	*/

      while (++count < seg_size)
         {
         if (tot >= PTR_LINE)
            {
//            *fp << "\\\n";			/* Newline for readability.	*/
				*fp << _T("\n");																			// 06/28/00 AM.
            tot = 0;
            }

         ptr_gen_struct(pseg, fp,cg);	/* Generate filled ptr table elt.	*/

         tot++;
         if (count+1 < seg_size)
            *fp << _T(",");  
         pseg++;
         }

      gen_array_tl(fp);							/* End array.				*/
      }
   else								/* Seg is empty.	*/
      gen_array_def(_T("PTR"), s_tab, siz, fp);

   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   }

/* GENERATE EMPTY TABLE SEGMENTS */
for (ii = seg_curr + 1; ii < segs_tot; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_PTR_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp"), dir, DIR_SEP, s_tab);
	fp = new std::_t_ofstream(TCHAR2A(s_nam));

   gen_file_head(fp);
   consh_gen_includes(fp);
   gen_array_def(_T("PTR"), s_tab, siz, fp);	/* Array definition. */
   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   }
}


/**************************************************
*						PTR_GEN_HDR
* FUN:	ptr_gen_hdr
* SUBJ:	Generate header file for ptr table.
* CR:	10/6/95 AM.
*
**************************************************/

void ptr_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
_TCHAR s_nam[PATH],	/* Name of ptr table hdr file.		*/
     s_tab[16];		/* Name of ptr table array.			*/
int segs_tot;

segs_tot = cg->aptr_->ptr_segs_tot();
_stprintf(s_nam, _T("%s%s%s_ini.h%s"), dir, DIR_SEP, consh_PTR_BASE, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

*fp << _T("extern bool cc_ptr_ini(void*);\n") << std::endl;					// 08/15/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   {
   _stprintf(s_tab, _T("%s%d"), consh_PTR_BASE, ii);
   gen_array_decl(_T("extern PTR"), s_tab, fp);
   }

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}


/**************************************************
*						PTR_GEN_INI
* FUN:	ptr_gen_ini
* SUBJ:	Generate initialization file for ptr table.
* CR:	10/6/95 AM.
*
**************************************************/

void ptr_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
_TCHAR s_nam[PATH];	/* Name of ptr table hdr file.		*/
//char s_tab[16];		/* Name of ptr table array.			*/
int segs_tot, tcurr;
long toff, tsize;
_TCHAR *tbase;

tbase		= consh_PTR_BASE;
segs_tot	= cg->aptr_->ptr_segs_tot();
toff		= cg->aptr_->ptr_curr_off();
tcurr		= cg->aptr_->ptr_seg_curr();
tsize		= cg->aptr_->ptr_seg_size();

_stprintf(s_nam, _T("%s%s%s_ini.cpp%s"), dir, DIR_SEP, tbase, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

consh_gen_includes(fp);
*fp << _T("\nbool cc_ptr_ini(void *xcg)") << std::endl;						// 08/15/02 AM.
*fp << _T("{") << std::endl;
*fp << _T("PTR **segs;\n") << std::endl;
*fp << _T("CG *cg = (CG *) xcg;") << std::endl;									// 08/15/02 AM.
*fp << _T("segs = cg->aptr_->ptr_seg_table();") << std::endl;				// 08/15/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   *fp << _T("segs[") << ii << _T("] = &(")
		 << tbase << ii << _T("[0]);")
		 << std::endl;

*fp << _T("bool ok = cg->aptr_->ptr_hard_ini(") << std::endl;				// 08/15/02 AM.
*fp << _T("   (long) ") << tsize << _T(",\t/* ptr seg size */") << std::endl;
*fp << _T("   (int) ") << segs_tot << _T(",\t/* ptr segs tot */");
*fp << _T("   (int) ") << tcurr << _T(",\t/* ptr seg curr */") << std::endl;
if (toff >= 0)									/* Table not empty. */
   *fp << _T("   &(")
		 << tbase << tcurr
		 << _T("[") << toff << _T("])\t/* ptr seg p */")
		 << std::endl;
else											/* Table empty.		*/
   *fp << _T("   &(") << tbase << tcurr << _T("[0]) - 1\t/* ptr seg p */")
		 << std::endl;

*fp << _T("   );") << std::endl;
*fp << _T("return ok;") << std::endl;												// 05/05/01 AM.
*fp << _T("}") << std::endl;

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}


/**************************************************
*						PTR_GEN_STRUCT
* FUN:	ptr_gen_struct
* SUBJ:	Generate one ptr struct for ptr table.
* CR:	10/8/95 AM.
*
**************************************************/

void
ptr_gen_struct(
	PTR *pseg,
	std::_t_ofstream *fp,
	CG *cg
	)
{
int tseg;
long toff;
_TCHAR kind_x[32], val_x[32], next_x[32];

if (pseg->kind)	/* If ptr is present. */
   {
   switch(pseg->kind)
      {
      case pCON:
         _tcscpy(kind_x, _T("pCON"));
         cg->acon_->con_off(pseg->v.vcon, /*UP*/ &tseg, &toff);
         if (tseg >= 0)
            _stprintf(val_x, _T("&(%s%d[%ld])"), consh_CON_BASE, tseg, toff);
         else
            _tcscpy(val_x, _T("CNULL"));
         break;
      case pPTR:
         _tcscpy(kind_x, _T("pPTR"));
         cg->aptr_->ptr_off(pseg->v.vptr, /*UP*/ &tseg, &toff);
         if (tseg >= 0)
            _stprintf(val_x, _T("(CON *)&(%s%d[%ld])"), consh_PTR_BASE, tseg, toff);
         else
            _tcscpy(val_x, _T("PNULL"));
         break;
      case pSYM:
         _tcscpy(kind_x, _T("pSYM"));
         cg->asym_->sym_off(pseg->v.vsym,	/*UP*/ &tseg, &toff);
         if (tseg >= 0)
            _stprintf(val_x, _T("(CON *)&(%s%d[%ld])"), consh_SYM_BASE, tseg, toff);
         else
            _tcscpy(val_x, _T("SNULL"));
         break;
      case pST:
         _tcscpy(kind_x, _T("pST"));
         cg->ast_->st_off(pseg->v.vst, /*UP*/ &tseg, &toff);
         if (tseg >= 0)
            _stprintf(val_x, _T("(CON *)&(%s%d[%ld])"), consh_ST_BASE, tseg, toff);
         else
            _tcscpy(val_x, _T("SNULL"));
         break;
      case pNUM:
         _tcscpy(kind_x, _T("pNUM"));
         /* FOR NOW, NEG means no number. */
         _stprintf(val_x, _T("(CON *)%llu"), pseg->v.vnum);
         break;
		case pFLOAT:															// 08/16/01 AM.
			_tcscpy(kind_x, _T("pFLOAT"));										// 08/16/01 AM.
			// Workaround: Pretend float is a LONG!					// 08/16/01 AM.
			_stprintf(val_x, _T("(CON *)%llu"), pseg->v.vnum);				// 08/16/01 AM.
			//sprintf(val_x, "(CON *)%f", pseg->v.vfloat);			// 08/16/01 AM.
			break;																// 08/16/01 AM.
      case pCHAR:
         _tcscpy(kind_x, _T("pCHAR"));
         _stprintf(val_x, _T("(CON *)%c"), pseg->v.vchar);
         break;

      case pNO_PTR:
      default:
         std::_t_cerr << _T("[ptr_gen: Bad kind]") << std::endl;
         //prog_exit(1);				// 04/29/99 AM.
			exit(1);							// 04/29/99 AM.
      }

   cg->aptr_->ptr_off(pseg->next, /*UP*/ &tseg, &toff);
   if (tseg >= 0)
      _stprintf(next_x, _T("&(%s%d[%ld])"), consh_PTR_BASE, tseg, toff);
   else
      _tcscpy(next_x, _T("PNULL"));
   
   /* Generate a ptr table struct. */
   *fp << _T("{") << kind_x << _T(",") << val_x << _T(",") << next_x << _T("}")
		 << std::flush;
   }
else			/* Generate empty struct. */
   {
   *fp << _T("{NO_PTR,CNULL,PNULL}");
   }
}



/**************************************************
*						PTR_GEN_ADDR
* FUN:	ptr_gen_entry
* SUBJ:	Generate a ptr table address for given ptr.
* CR:		7/15/96 AM.
* RET:	true if ptr present.
*			buf - Filled with printable address, eg, "&(Ptr0[33])".
* NOTE:	Like con_gen_addr, but omits ampersand.
*
**************************************************/

bool
ptr_gen_addr(PTR *ptr, CG *cg, /*UP*/ _TCHAR *buf)
{
int tseg;
long toff;

if (ptr)
	{
	cg->aptr_->ptr_off(ptr, /*UP*/ &tseg, &toff);
	if (tseg >= 0)
		{
		_stprintf(buf, _T("&(%s%d[%ld])"), consh_PTR_BASE, tseg, toff);
		return true;
		}
	}
_tcscpy(buf, _T("PNULL"));
return false;
}



/**************************************************
*						PTR_GEN_ENTRY
* FUN:	ptr_gen_entry
* SUBJ:	Generate a ptr table address for given ptr.
* CR:		7/15/96 AM.
* RET:	true if ptr present.
*			buf - Filled with printable address, eg, "&(Ptr0[33])".
* NOTE:	Like con_gen_addr, but omits ampersand.
*
**************************************************/

bool
ptr_gen_entry(PTR *ptr, CG *cg, /*UP*/ _TCHAR *buf)
{
int tseg;
long toff;

if (ptr)
	{
	cg->aptr_->ptr_off(ptr, /*UP*/ &tseg, &toff);
	if (tseg >= 0)
		{
		_stprintf(buf, _T("%s%d[%ld]"), consh_PTR_BASE, tseg, toff);
		return true;
		}
	}
_tcscpy(buf, _T("(PTR) NULL"));
return false;
}
