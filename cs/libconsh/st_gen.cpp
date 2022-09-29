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
*									ST_GEN.C
*
* FILE:	consh.�/st_gen.c
* SUBJ:	Generate string table code for Consh.
* CR:	5/07/95 AM.
* NOTE:	9/25/95 AM. Moved from st.c .
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


#include "kbm/libkbm.h"
#include "kbm/st.h"

#include "consh/libconsh.h"	// 04/26/99 AM.
#include "consh/Aconsh.h"		// 04/26/99 AM.
#include "consh/cg.h"

#include "st_gen.h"
#include "gen.h"


/**************************************************
*						ST_GEN
* FUN:	st_gen
* SUBJ:	Generate code for string table in memory.
* CR:	5/19/95 AM.
* ALG:	Traverse string table, print it out as arrays.
*		Each segment goes into its own file.
* NOTE:	To preserve offsets, need to print out the entire
*		segment, char for char.
*
**************************************************/

void st_gen(
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
_TCHAR *ptr;
_TCHAR s_nam[PATH],	/* Name of string table file.			*/
     s_tab[16];		/* Name of string table array.			*/
_TCHAR **st_segs;
_TCHAR siz[16];		/* Size of generated array.				*/

_stprintf(siz, _T("%ld"), consh_ST_SEG_SIZE);

tot = 0;
segs_tot = cg->ast_->st_segs_tot();
seg_size = cg->ast_->st_seg_size();
st_segs  = cg->ast_->st_seg_table();
seg_curr = cg->ast_->st_seg_curr();

// For various char sizes.	// 01/17/06 AM.
long csiz = sizeof(_TCHAR);		// 01/17/06 AM.

/* GENERATE FILLED TABLE SEGMENTS */
for (ii = 0; ii <= seg_curr; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_ST_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp%s"), dir, DIR_SEP, s_tab, tail);
   //if (!file_open(s_nam, "w", &fp))
   //   return;
	fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.
   gen_file_head(fp);
// LINUX: Need to resolve to same capitalization in file name.	// 03/23/19 AM.
#ifdef LINUX
	*fp << _T("#include \"stdafx.h\"") << std::endl;	// 03/23/19 AM.
#else
	*fp << _T("#include \"StdAfx.h\"") << std::endl;	// 06/28/00 AM.
#endif

   ptr = st_segs[ii];
   count = -1;
   if (ii == seg_curr)				/* Last filled segment. */
      seg_size = 1 + cg->ast_->st_seg_p() - ptr;

   if (seg_size)
      {
      // Unicode conversion.	// 01/17/06 AM.
      gen_array_hd(_T("_TCHAR"), s_tab, siz, fp);	/* Start array.	*/
//      _TCHAR buf[32];	// 01/17/06 AM.
      while (++count < seg_size)
         {
         if (tot >= STD_LINE)
            {
            //*fp << "\\\n";				/* Newline for readability.	*/
				*fp << _T("\n");																			// 06/28/00 AM.
            tot = 0;
            }
         if (*ptr)
            {
            switch (*ptr)		/* 10/31/95 AM */
               {
               case '\n':
                  *fp << _T("'\\n'");
                  break;
               case '\t':
                  *fp << _T("'\\t'");
                  break;
               case '\\':
                  *fp << _T("'\\\\'");
                  break;
               case '\'':
                  *fp << _T("'\\''");
                  break;
               default:
#ifndef UNICODE
                  *fp << _T("'") << *ptr << _T("'");
#else
						if ((long)*ptr > 127)
							{
							_TCHAR buf[512];							// 01/23/06 AM.
							num_to_hex((long)*ptr,/*DU*/buf);	// 01/17/06 AM.
							*fp << _T("L'\\x");						// 01/17/06 AM.
							*fp << buf;									// 01/17/06 AM.
							*fp << _T("'");
							}
						else
							*fp << _T("'") << *ptr << _T("'");

#endif
                  break;
               }
            tot += 3 + csiz;			/* For "'X'," where X is a char */
            }
         else
            {
            *fp << _T("'\\0'");
            tot += 5;			/* For "'\0'," */
            }
         if (count != seg_size - 1)
            *fp << _T(",");
         ptr++;
         }
      gen_array_tl(fp);							/* End array.				*/
      }
   else
      gen_array_def(_T("_TCHAR"), s_tab, siz, fp);	/* Array definition. */

   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   if ((ptr >= cg->ast_->st_seg_p()) && (ii == cg->ast_->st_seg_curr()))
      return;
   }

/* GENERATE EMPTY TABLE SEGMENTS */
for (ii = seg_curr + 1; ii < segs_tot; ii++)
   {
   /* Set up file for current segment. */
   _stprintf(s_tab, _T("%s%d"), consh_ST_BASE, ii);
   _stprintf(s_nam, _T("%s%s%s.cpp"), dir, DIR_SEP, s_tab);
   //if (!file_open(s_nam, "w", &fp))
   //  return;
	fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

   gen_file_head(fp);
   gen_array_def(_T("_TCHAR"), s_tab, siz, fp);	/* Array definition. */
   //if (!file_close(s_nam, fp))
   //   return;
	delete fp;
   }
}


/**************************************************
*						ST_GEN_HDR
* FUN:	st_gen_hdr
* SUBJ:	Generate header file for string table.
* CR:	5/20/95 AM.
*
**************************************************/

void st_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
_TCHAR s_nam[PATH],	/* Name of string table hdr file.		*/
     s_tab[16];		/* Name of string table array.			*/
int segs_tot;

segs_tot = cg->ast_->st_segs_tot();
_stprintf(s_nam, _T("%s%s%s_ini.h%s"), dir, DIR_SEP, consh_ST_BASE, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

*fp << _T("extern bool cc_st_ini(void*);\n") << std::endl;					// 08/16/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   {
   _stprintf(s_tab, _T("%s%d"), consh_ST_BASE, ii);
   gen_array_decl(_T("extern _TCHAR"), s_tab, fp);
   }

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}


/**************************************************
*						ST_GEN_INI
* FUN:	st_gen_ini
* SUBJ:	Generate initialization file for string table.
* CR:	5/20/95 AM.
*
**************************************************/

void st_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	)
{
std::_t_ofstream *fp;
int ii;
//long off;
_TCHAR s_nam[PATH];	/* Name of string table hdr file.		*/
//char s_tab[16];		/* Name of string table array.			*/
int segs_tot, tcurr;
long toff, tsize;
_TCHAR *tbase;

tbase		= consh_ST_BASE;
segs_tot	= cg->ast_->st_segs_tot();
toff		= cg->ast_->st_curr_off();
tcurr		= cg->ast_->st_seg_curr();
tsize		= cg->ast_->st_seg_size();

_stprintf(s_nam, _T("%s%s%s_ini.cpp%s"), dir, DIR_SEP, tbase, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);

#ifdef LINUX
*fp << _T("#include \"stdafx.h\"") << std::endl;			// 06/28/00 AM.
*fp << _T("#include <stdio.h>") << std::endl;
*fp << _T("#include <stdlib.h>") << std::endl;	
//*fp << "#include <fstream.h>" << std::endl;			// 04/23/99 AM.
*fp << _T("#include <iostream>") << std::endl;	// Upgrade.	// 01/24/01 AM.
*fp << _T("#include <fstream>") << std::endl;	// Upgrade.	// 01/24/01 AM.
*fp << _T("#include \"prim\\libprim.h\"") << std::endl;
*fp << _T("#include \"prim\\prim.h\"") << std::endl;
//*fp << "#include \"prim\\mach.h\"" << std::endl;			// 04/23/99 AM.
*fp << _T("#include \"kbm\\libkbm.h\"") << std::endl;
*fp << _T("#include \"kbm\\st.h\"") << std::endl;
*fp << _T("#include \"consh\\libconsh.h\"") << std::endl;		// 08/15/02 AM.
*fp << _T("#include \"consh\\cg.h\"") << std::endl;			// 08/15/02 AM.
#else
*fp << _T("#include \"StdAfx.h\"") << std::endl;			// 06/28/00 AM.
*fp << _T("#include <stdio.h>") << std::endl;
*fp << _T("#include <stdlib.h>") << std::endl;
//*fp << "#include <fstream.h>" << std::endl;			// 04/23/99 AM.
*fp << _T("#include <iostream>") << std::endl;	// Upgrade.	// 01/24/01 AM.
*fp << _T("#include <fstream>") << std::endl;	// Upgrade.	// 01/24/01 AM.
*fp << _T("#include \"prim/libprim.h\"") << std::endl;
*fp << _T("#include \"prim/prim.h\"") << std::endl;
//*fp << "#include \"prim/mach.h\"" << std::endl;			// 04/23/99 AM.
*fp << _T("#include \"kbm/libkbm.h\"") << std::endl;
*fp << _T("#include \"kbm/st.h\"") << std::endl;
*fp << _T("#include \"consh/libconsh.h\"") << std::endl;		// 08/15/02 AM.
*fp << _T("#include \"consh/cg.h\"") << std::endl;			// 08/15/02 AM.
#endif
*fp << _T("#include \"") << tbase << _T("_ini.h\"") << std::endl;
*fp << _T("\nbool cc_st_ini(void *xcg)") << std::endl;		// 08/15/02 AM.
*fp << _T("{") << std::endl;
*fp << _T("_TCHAR **segs;\n") << std::endl;
*fp << _T("CG *cg = (CG *) xcg;") << std::endl;			// 08/15/02 AM.
*fp << _T("segs = cg->ast_->st_seg_table();") << std::endl;		// 08/15/02 AM.

for (ii = 0; ii < segs_tot; ii++)
   *fp << _T("segs[") << ii << _T("] = &(") << tbase << ii << _T("[0]);")
		 << std::endl;

*fp << _T("bool ok = cg->ast_->st_hard_ini(") << std::endl;					// 08/18/02 AM.
*fp << _T("   (long) ") << tsize << _T(",\t/* st seg size */") << std::endl;
*fp << _T("   (int) ") << segs_tot << _T(",\t/* st segs tot */") << std::endl;
*fp << _T("   (int) ") << tcurr << _T(",\t/* st seg curr */") << std::endl;
if (toff >= 0)									/* Table not empty. */
   *fp << _T("   &(")
		 << tbase << tcurr
		 << _T("[") << toff << _T("])\t/* st seg p */") << std::endl;
else											/* Table empty.		*/
   *fp << _T("   &(") << tbase << tcurr << _T("[0]) - 1\t/* st seg p */")
		 << std::endl;


*fp << _T("   );") << std::endl;
*fp << _T("return ok;") << std::endl;												// 05/05/01 AM.
*fp << _T("}") << std::endl;

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}
