/*******************************************************************************
Copyright (c) 2001-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STD.H
* FILE:	lite\std.h
* CR:		10/08/98 AM.
* SUBJ:	C-only declarations.
*
*******************************************************************************/

long LITE_API file_size(_TCHAR *fname);
int open_outfile(_TCHAR *fname);
void close_outfile(int fh);
