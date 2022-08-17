/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
#include "StdAfx.h"


#include "machine.h"	// 10/25/06 AM.
#include "lite/global.h"
#include "u_ofstream.h"


#ifdef UNICODE

u_ofstream &u_ofstream::operator<<(const _TCHAR &lpszBuffer)
{
        char *lpstr8;
        u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);
        std::operator<<(*this,lpstr8);
        delete lpstr8;
        return *this;
}

#endif
