/*

   wnglobals.h - global variables used by various WordNet applications

   $Id: wnglobal.h,v 1.30 1997/09/02 16:31:18 wn Rel $

*/

#ifndef _WNGLOBALS_
#define _WNGLOBALS_

#include <stdio.h>

extern _TCHAR *wnrelease;		/* WordNet release/version number */

extern _TCHAR *lexfiles[];	/* names of lexicographer files */
extern _TCHAR *ptrtyp[];		/* pointer characters */
extern _TCHAR *partnames[];	/* POS strings */
extern _TCHAR partchars[];	/* single chars for each POS */
extern _TCHAR *adjclass[];	/* adjective class strings */
extern _TCHAR *frametext[];	/* text of verb frames */


#endif /* _WNGLOBALS_ */
