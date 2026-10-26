#ifndef NLP_INCLUDE_API_LITE_STD_H
#define NLP_INCLUDE_API_LITE_STD_H

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

// Largest input text, in bytes.  Text lengths and node offsets are long
// (32 bits on Windows) and ICU's are int32_t, so this holds everywhere.	// 10/25/26 DD.
#define INPUT_MAX 2000000000LL

long long LITE_API file_size(_TCHAR *fname);
int open_outfile(_TCHAR *fname);
void close_outfile(int fh);

#endif // NLP_INCLUDE_API_LITE_STD_H
