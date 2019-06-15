/*******************************************************************************
Copyright © 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MACH.H
* FILE:	include/API/lite/mach.h
* CR:		10/29/98 AM.
* SUBJ:	Things that may be machine specific.
*
*******************************************************************************/

#include "lite.h"

LITE_API _TCHAR *today();
LITE_API _TCHAR *today1();			// WORKAROUND.				// 02/25/00 AM.
LITE_API bool dosReadargs(
	int argc,
	_TCHAR *argv[],
	/*UP*/
	_TCHAR* &,			// Input.
	_TCHAR* &,			// Output.
	bool &,			// Development mode. // 12/25/98 AM.
	bool &compiled	// true - compiled ana, false - interp.// 07/02/00 AM.
	);
LITE_API void dosHelpargs(_TCHAR *);
LITE_API bool dir_exists(_TCHAR *name);	// 03/10/99 AM.
LITE_API bool make_dir(_TCHAR *name);		// 03/10/99 AM.
LITE_API bool rm_dir(_TCHAR *name);		// 03/10/99 AM.
LITE_API bool remove_path(_TCHAR *name);	// 03/10/99 AM.

LITE_API void			// 04/20/99 AM.
date_str(
	_TCHAR str[],		/* Buffer to fill with date-time. */
	int siz		/* Max size of buffer. */
	);
