#include "wn.h"

#define ALLWORDS	0	/* print all words */
#define SKIP_ANTS	0	/* skip printing antonyms in printsynset() */
#define PRINT_ANTS	1	/* print antonyms in printsynset() */
#define SKIP_MARKER	0	/* skip printing adjective marker */
#define PRINT_MARKER	1	/* print adjective marker */

#define DEFON 1
#define DEFOFF 0

void WNPOSSummary(CString &searchStr, CString &outputStr, int pos);
void WNDisplaySynset(CString &outputStr, CString headStr, SynsetPtr synptr, CString tailStr, int definition,
				   int wdnum, int antflag, int markerflag);
void WNDisplayWord(CString &buffStr, SynsetPtr synptr, int wdnum, int adjmarker, int antflag);
CString WNDisplayAnt(int dbase, SynsetPtr synptr, int wdnum, CString formatStr, _TCHAR *tail);
void WNDisplayAntSynset(CString &outputStr, SynsetPtr synptr, _TCHAR *tail, int anttype, int definition);
CString WNDeadjify(CString wordStr);
