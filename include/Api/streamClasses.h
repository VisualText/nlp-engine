/*******************************************************************************
Copyright (c) 2006-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// streamClasses.h

#include "libStream.h"

#ifdef UNICODE
extern LIBSTREAM_API LPSTR CTCHAR2A(LPCTSTR lpszBuffer);
extern LIBSTREAM_API LPCSTR CTCHAR2CA(LPCTSTR lpszBuffer);
extern LIBSTREAM_API LPSTR TCHAR2A(LPTSTR lpszBuffer);
extern LIBSTREAM_API LPCSTR TCHAR2CA(LPTSTR lpszBuffer);
#else
#define CTCHAR2A(x) x
#define CTCHAR2CA(x) x
#define TCHAR2A(x) x
#define TCHAR2CA(x) x
#endif

