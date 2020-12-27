/*** CONAN: AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
/*** CONSH: HAND-EDITING OK ***/

// Rename to distinguish hardwired kb loads from auto-generated kb load fns. // 02/20/19 AM.
#ifdef LINUX
extern bool ccx_con_ini(void*);	// 02/20/19 AM.
#else
extern bool cc_con_ini(void*);	// 08/16/02 AM.
#endif

//extern CON Con0[];

#ifdef CON_OLD
/* Hard-wired initial number of concept table segments. */
#define TOT_CON_SEGS 1

/* Hard-wired initial size of concept table. */
/* Size in terms of con elements. */
#define CONTAB_SIZE (((long)TOT_CON_SEGS) * CON_SEG_SIZE)

/* Hard-wired initial filled size of concept table */
#define TOT_CONS ((long) 0)

#endif
