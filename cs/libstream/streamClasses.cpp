// streamClases.cpp
#ifndef LINUX
#include "stdafx.h"
#endif
#include "stdio.h"
#include "stdlib.h"

#include "streamClasses.h"

LIBSTREAM_API LPSTR CTCHAR2A(LPCTSTR lpszBuffer)
{
	static char output[4096];
	LPSTR _lpa = output;
	_lpa[0] = 0;

	LPCWSTR _lpw = lpszBuffer;
	if (NULL == _lpw)
		return NULL;
	int nChars = lstrlenW(_lpw)+1;
	WideCharToMultiByte(CP_ACP, 0, _lpw, -1, _lpa, nChars, NULL, NULL);
	return _lpa;
}

LIBSTREAM_API LPCSTR CTCHAR2CA(LPCTSTR lpszBuffer)
{
	return (LPCSTR)CTCHAR2A(lpszBuffer);
}

LIBSTREAM_API LPSTR TCHAR2A(LPTSTR lpszBuffer)
{
	return CTCHAR2A((LPCTSTR)lpszBuffer);
}

LIBSTREAM_API LPCSTR TCHAR2CA(LPTSTR lpszBuffer)
{
	return (LPCSTR)CTCHAR2A((LPCTSTR)lpszBuffer);
}
