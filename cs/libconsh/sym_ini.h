/*** AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
/*** IN CONSH: HAND-EDITED. ***/

// Distinguish library from auto-generated kb load.	// 03/23/19 AM.
#ifdef LINUX
extern bool ccx_sym_ini(void*);				// 03/23/19 AM.
#else
extern bool cc_sym_ini(void*);			// 08/16/02 AM.
#endif

// In the interp KBMS, no more hard-coded sym segments.			// 02/15/01 AM.
//extern SYM Sym0[];															// 02/15/01 AM.
//extern SYM Sym1[];															// 02/15/01 AM.

#ifdef SYM_OLD
/* Hard-wired initial number of symbol table segments. */
/* Hard table must contain the entire hashed table.	*/
#define TOT_SYM_SEGS 2

/* Hard-wired initial size of symbol table. */
/* Size in terms of sym elements. */
#define SYMTAB_SIZE (((long)TOT_SYM_SEGS) * ((long) SYM_SEG_SIZE))
#endif
