/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// tear.h : class definitions for CTearSession and CTearException
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1997-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Created by Mike Blaszczak.

class CTearSession : public CInternetSession
{
public:
	CTearSession(LPCTSTR pszAppName, int nMethod);
	virtual void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus,
		LPVOID lpvStatusInformation, DWORD dwStatusInformationLen);
};


class CTearException : public CException
{
	DECLARE_DYNCREATE(CTearException)

public:
	CTearException(int nCode = 0);
	~CTearException() { }

	int m_nErrorCode;
};


WEB_API int tear_url(LPCTSTR url, _TCHAR *outfile);	// 02/11/99 AM.
