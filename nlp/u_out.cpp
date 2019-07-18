/*******************************************************************************
Copyright (c) 2006 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	U_OUT.CPP
* FILE:	prim\u_out.cpp
* CR:		03/24/06 AM.
* SUBJ:	Conversions for output of Unicode to UTF-8 files.
* NOTE:	Override the standard << output operator.
*
*******************************************************************************/

#include "u_out.h"


#ifdef UNICODE
#ifndef LINUX


// Define these outside of any class!

_t_ofstream &STDOPERATOR<<(_t_ofstream &os, _TCHAR* &lpszBuffer)
{
char *lpstr8;
u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);
os << lpstr8;
u_delete((LPCTSTR*&)lpstr8);
return os;
}

_t_ostream &STDOPERATOR<<(_t_ostream &os, _TCHAR* &lpszBuffer)
{

char *lpstr8;
u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);
os << lpstr8;
u_delete((LPCTSTR*&)lpstr8);
return os;
}



#endif
#endif

