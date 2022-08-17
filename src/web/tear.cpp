// tear.cpp : implements the TEAR console application
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

#include "stdafx.h"
#include "web.h"
/*
#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
*/
#include "tear.h"
#include "Aweb.h"			// 03/03/99 AM.

#include <iostream>											// Upgrade.	// 01/24/01 AM.
#include <fstream>											// Upgrade.	// 01/24/01 AM.
#include <stdlib.h>

#include "url.h"			// 02/11/99 AM.


/////////////////////////////////////////////////////////////////////////////
// Globals

LPCTSTR pszURL = NULL;	// 02/11/99 AM. Now this is a FILE of URLs.
BOOL    bStripMode = FALSE;
BOOL    bProgressMode = FALSE;
DWORD   dwAccessType = PRE_CONFIG_INTERNET_ACCESS;

DWORD dwHttpRequestFlags =
	INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;

const TCHAR szHeaders[] =
	_T("Accept: text/*\r\nUser-Agent: MFC_Tear_Sample\r\n");

/////////////////////////////////////////////////////////////////////////////
// CTearSession object

// TEAR wants to use its own derivative of the CInternetSession class
// just so it can implement an OnStatusCallback() override.

CTearSession::CTearSession(LPCTSTR pszAppName, int nMethod)
	: CInternetSession(pszAppName, 1, nMethod)
{
}

void CTearSession::OnStatusCallback(DWORD /* dwContext */, DWORD dwInternetStatus,
	LPVOID /* lpvStatusInformation */, DWORD /* dwStatusInformationLen */)
{
	if (!bProgressMode)
		return;

	if (dwInternetStatus == INTERNET_STATUS_CONNECTED_TO_SERVER)
		_t_cerr << _T("Connection made!") << endl;
}

/////////////////////////////////////////////////////////////////////////////
// CTearException -- used if something goes wrong for us

// TEAR will throw its own exception type to handle problems it might
// encounter while fulfilling the user's request.

IMPLEMENT_DYNCREATE(CTearException, CException)

CTearException::CTearException(int nCode)
	: m_nErrorCode(nCode)
{
}

void ThrowTearException(int nCode)
{
	CTearException* pEx = new CTearException(nCode);
	throw pEx;
}

/////////////////////////////////////////////////////////////////////////////
// Routines

WEB_API void ShowBanner()
{
	_t_cerr << _T("SPIDER - Read a file of URLs.") << endl;
	_t_cerr << endl;
}

WEB_API void ShowUsage()
{
	_t_cerr << _T("Usage:  SPIDER [options] <URL file>") << endl << endl;
	_t_cerr << _T("\t<URL file> each line starts with an HTTP address") << endl;
	_t_cerr << _T("\t[options] are any of:") << endl;
	_t_cerr << _T("\t\t/F force reload of requested pages") << endl;
	_t_cerr << _T("\t\t/P show detailed progress information") << endl;
	_t_cerr << _T("\t\t/S strip HTML tags (crudely)") << endl << endl;
	_t_cerr << _T("\t\t/L use local Internet access") << endl;
	_t_cerr << _T("\t\t/D use pre-configured Internet access (default)") << endl;

	_t_cerr << endl;
}

// ParseOptions() looks for options on the command line and sets global
// flags so the rest of the program knows about them.  ParseOptions()
// also initializes pszURL to point at the URL the user wanted.

WEB_API BOOL ParseOptions(int argc, _TCHAR* argv[],
	_TCHAR* &urlfile				// 03/02/99 AM.
	)
{
urlfile = NULL;				//03/02/99 AM.

if (argc < 2)				// 02/28/99 AM.
	return false;			// 02/28/99 AM.

	int nIndex;
	for (nIndex = 1; nIndex < argc; nIndex++)
	{
		// an option or a URL?

		if (*argv[nIndex] == '-' || *argv[nIndex] == '/')
		{
			if (argv[nIndex][1] == 'D' || argv[nIndex][1] == 'd')
				dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
			else if (argv[nIndex][1] == 'L' || argv[nIndex][1] == 'l')
				dwAccessType = LOCAL_INTERNET_ACCESS;
			else if (argv[nIndex][1] == 'S' || argv[nIndex][1] == 's')
				bStripMode = TRUE;
			else if (argv[nIndex][1] == 'P' || argv[nIndex][1] == 'p')
				bProgressMode = TRUE;
			else if (argv[nIndex][1] == 'F' || argv[nIndex][1] == 'f')
				dwHttpRequestFlags |= INTERNET_FLAG_RELOAD;
			else
			{
				_t_cerr << _T("Error: unrecognized option: ") << argv[nIndex] << endl;
				return FALSE;
			}
		}
		else
		{
		// can't have too many URLs
		// Now taking URL file name as input. 02/99 AM.

		if (urlfile != NULL)
		{
			_t_cerr << _T("Error: can only specify one URL file!") << endl;
			return FALSE;
		}
		else
			urlfile = argv[nIndex];
		}
	}

if (urlfile == NULL)			// 02/28/99 AM.
	return false;				// 02/28/99 AM.

	return TRUE;
}

// StripTags() rips through a buffer and removes HTML tags from it.
// The function uses a static variable to remember its state in case
// a HTML tag spans a buffer boundary.

void StripTags(LPTSTR pszBuffer)
{
	static BOOL bInTag = FALSE;
	LPTSTR pszSource = pszBuffer;
	LPTSTR pszDest = pszBuffer;

	while (*pszSource != '\0')
	{
		if (bInTag)
		{
			if (*pszSource == '>')
				bInTag = FALSE;
			pszSource++;
		}
		else
		{
			if (*pszSource == '<')
				bInTag = TRUE;
			else
			{
				*pszDest = *pszSource;
				pszDest++;
			}
			pszSource++;
		}
	}
	*pszDest = '\0';
}


/********************************************
* FN:		INIT_MFC
* CR:		02/11/99 AM.
* SUBJ:	Process a file of URLs.
********************************************/

WEB_API bool init_MFC()
{
if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_t_cerr << _T("MFC Failed to initialize.\n");
		return false;
	}
return true;
}

/********************************************
* FN:		TEAR_URL
* CR:		02/11/99 AM.
* SUBJ:	Fetch one URL into a file.
********************************************/

WEB_API int tear_url(
	LPCTSTR url,			// URL to lookup.
	_TCHAR *outfile			// File path for placing fetched webpage.
	)
{

	CTearSession session(_T("SPIDER Program"), dwAccessType);

	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000);	// 05/08/03 AM.
	session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 5000);	// 05/08/03 AM.

	int nRetCode = 0;
	CHttpConnection* pServer = NULL;
	CHttpFile* pFile = NULL;
	try
	{
		// check to see if this is a reasonable URL

		CString strServerName;
		CString strObject;
		INTERNET_PORT nPort;
		DWORD dwServiceType;

		if (!AfxParseURL(url, dwServiceType, strServerName, strObject, nPort) ||
			dwServiceType != INTERNET_SERVICE_HTTP)
		{
			_t_cerr << _T("Error: can only use URLs beginning with http://") << endl;
			ThrowTearException(1);
		}

		if (bProgressMode)
		{
			_t_cerr << _T("Opening Internet...");
			VERIFY(session.EnableStatusCallback(TRUE));
		}

		pServer = session.GetHttpConnection(strServerName, nPort);

		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
			strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
		pFile->AddRequestHeaders(szHeaders);
		pFile->SendRequest();

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);

		// if access was denied, prompt the user for the password

		if (dwRet == HTTP_STATUS_DENIED)
		{
			DWORD dwPrompt;
			dwPrompt = pFile->ErrorDlg(NULL, ERROR_INTERNET_INCORRECT_PASSWORD,
				FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, NULL);

			// if the user cancelled the dialog, bail out

			if (dwPrompt != ERROR_INTERNET_FORCE_RETRY)
			{
				_t_cerr << _T("Access denied: Invalid password\n");
				ThrowTearException(1);
			}

			pFile->SendRequest();
			pFile->QueryInfoStatusCode(dwRet);
		}

		CString strNewLocation;
		pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);

		// were we redirected?
		// these response status codes come from WININET.H

		if (dwRet == HTTP_STATUS_MOVED ||
			dwRet == HTTP_STATUS_REDIRECT ||
			dwRet == HTTP_STATUS_REDIRECT_METHOD)
		{
			CString strNewLocation;
			pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);

			int nPlace = strNewLocation.Find(_T("Location: "));
			if (nPlace == -1)
			{
				_t_cerr << _T("Error: Site redirects with no new location") << endl;
				ThrowTearException(2);
			}

			strNewLocation = strNewLocation.Mid(nPlace + 10);
			nPlace = strNewLocation.Find('\n');
			if (nPlace > 0)
				strNewLocation = strNewLocation.Left(nPlace);

			// close up the redirected site

			pFile->Close();
			delete pFile;
			pFile = NULL;				// 02/11/99 AM.
			pServer->Close();
			delete pServer;
			pServer = NULL;			// 02/11/99 AM.

			if (bProgressMode)
			{
				_t_cerr << _T("Caution: redirected to ");
				_t_cerr << (LPCTSTR) strNewLocation << endl;
			}

			// figure out what the old place was
			if (!AfxParseURL(strNewLocation, dwServiceType, strServerName, strObject, nPort))
			{
				_t_cerr << _T("Error: the redirected URL could not be parsed.") << endl;
				ThrowTearException(2);
			}

			if (dwServiceType != INTERNET_SERVICE_HTTP)
			{
				_t_cerr << _T("Error: the redirected URL does not reference a HTTP resource.") << endl;
				ThrowTearException(2);
			}

			// try again at the new location
			pServer = session.GetHttpConnection(strServerName, nPort);
			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
				strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
			pFile->AddRequestHeaders(szHeaders);
			pFile->SendRequest();

			pFile->QueryInfoStatusCode(dwRet);
			if (dwRet != HTTP_STATUS_OK)
			{
				_t_cerr << _T("Error: Got status code ") << dwRet << endl;
				ThrowTearException(2);
			}
		}

		_t_cerr << _T("Status Code is ") << dwRet << endl;

		/////////////////////
		/////////////////////
		// 02/11/99 AM. Use the user's URL string, even if it was
		// redirected.
		// SHOULD CHECK TO SEE IF FILE ALREADY EXISTS?
		_TCHAR outf[256];	// Name for output file for this URL.
		if (outfile && *outfile)	// 02/11/03 AM.
			{
			_stprintf(outf, _T("%s"), outfile);	// 02/11/03 AM.
			}
		else if (!url_to_fname(url, outf))	// Build a filename for URL. 
			{
			_t_cerr << _T("Error: Couldn't build file for URL=") << url << endl;
			ThrowTearException(2);
			}

		// 12/12/98 AM.  Read the file into a buffer, then call NLP Lite
		// on it!
//		_t_ofstream *fout = 0;						// 09/28/05 AM.
//		fout = new _t_ofstream(TCHAR2CA(outf), ios::out);	// 09/28/05 AM.
		CFile outFile;									// 09/28/05 AM.
		outFile.Open(outf,CFile::modeWrite | CFile::modeCreate); // 09/28/05 AM.
		TCHAR sz[1024];
#ifdef OLD_050928_
		while (pFile->ReadString(sz, 1023))
		{
			if (bStripMode)
				StripTags(sz);
			*fout << sz;
		}
#endif
// Using the binary style read-write.			// 09/28/05 AM.
		DWORD nRead = pFile->Read(sz, 1023);	// 09/28/05 AM.
		while (nRead > 0)								// 09/28/05 AM.
		{
			sz[nRead] = '\0';							// 09/28/05 AM.
			if (bStripMode)							// 09/28/05 AM.
				StripTags(sz);							// 09/28/05 AM.
//			*fout << sz;								// 09/28/05 AM.
			outFile.Write(sz,nRead);				// 09/28/05 AM.
			nRead = pFile->Read(sz, 1023);		// 09/28/05 AM.
		}

//		delete fout;									// 09/28/05 AM.
//		fout = 0;										// 09/28/05 AM.
		outFile.Close();								// 09/28/05 AM.
		/////////////////////
		/////////////////////

#ifdef ORIGINAL_
		TCHAR sz[1024];
		while (pFile->ReadString(sz, 1023))
		{
			if (bStripMode)
				StripTags(sz);
			_t_cout << sz;
		}
#endif

	// NOTE: Since HTTP servers normally spit back plain text, the
	// above code (which reads line by line) is just fine.  However,
	// other data sources (eg, FTP servers) might provide binary data
	// which should be handled a buffer at a time, like this:

#if 0
		while (nRead > 0)
		{
			sz[nRead] = '\0';
			if (bStripMode)
				StripTags(sz);
			_t_cout << sz;
			nRead = pFile->Read(sz, 1023);
		}
#endif

		pFile->Close();
		pServer->Close();
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet

		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);

		_t_cerr << _T("Error: (") << pEx->m_dwError << _T(") ");
		_t_cerr << szErr << endl;

		nRetCode = 2;
		pEx->Delete();
	}
	catch (CTearException* pEx)
	{
		// catch things wrong with parameters, etc

		nRetCode = pEx->m_nErrorCode;
		TRACE1("Error: Exiting with CTearException(%d)\n", nRetCode);
		pEx->Delete();
	}

	if (pFile != NULL)
		{
		delete pFile;
		pFile = NULL;			// 02/11/99 AM.
		}
	if (pServer != NULL)
		{
		delete pServer;
		pServer = NULL;		// 02/11/99 AM.
		}
	session.Close();

	return nRetCode;
}

/******************   END OF FILE   **************************/
