
#include "stdafx.h"
#include "libStream.h"
#include "u_convert.h"

#ifdef UNICODE
#define ASCII_7 0 // 0 - 0x7f
#define ASCII_EX 1 // 0 - 0xff
LIBSTREAM_API bool u_isascii(LPCWSTR lpwStr, int typeASCII)
{
	int maxVal = ASCII_7 == typeASCII ? 0x7f : 0xff;
	int len = lstrlen(lpwStr);
	for (int n = 0; n < len; n++)
		if ((int)lpwStr[n] > maxVal)
			return false;
	return true;
}

/********************************************
* FN:		u_to_mbcs
* CR:		03/06/05 AM.
* SUBJ:	Convert Unicode to MBCS.
* NOTE:	From Scott's functions in VisualText.
********************************************/

LIBSTREAM_API bool u_to_mbcs(
	LPCWSTR lpwUnicode,
	UINT nCodePage,
	LPCTSTR* &mbcsstr
	)
{
	int nChars = wcslen(lpwUnicode) + 1;
	int nMultiByteBufferSize = nChars * 6;  // possible 6 bytes max per character for UTF-8
	wchar_t* pszUnicodeString = new wchar_t[nChars]; 
	char *pszMultiByteString = new char[nMultiByteBufferSize];  

	lstrcpy(pszUnicodeString, lpwUnicode);

	int nCharsWritten = 0;
	if (pszUnicodeString && pszMultiByteString)
		{
		ZeroMemory(pszMultiByteString, nMultiByteBufferSize);
		if (nCodePage == -1)
			{
//			BOOL bASCII_Ex = u_isascii(lpwUnicode, ASCII_EX);
//			nCodePage = !bASCII_Ex ? CP_UTF8 : CP_ACP;
			nCodePage = CP_UTF8;
			}

		try
			{
			nCharsWritten = WideCharToMultiByte(nCodePage, 0,
				pszUnicodeString,-1, pszMultiByteString, nMultiByteBufferSize, NULL, NULL);
			}
		catch(...)
			{
//			TRACE(_T("Controlled exception in WideCharToMultiByte!\n"));
			}
		} 
//	CStringA strMBCS = CStringA((CP_UTF8 == nCodePage ? BOM_UTF8 : "")) + pszMultiByteString;
	if (pszUnicodeString)
		delete [] pszUnicodeString;
//	if (pszMultiByteString)
//		delete [] pszMultiByteString;
//	return strMBCS;
mbcsstr = (LPCTSTR *)pszMultiByteString;
return true;
}


/********************************************
* FN:		u_delete
* CR:		03/10/05 AM.
* SUBJ:	Delete buffer allocated in this library.
* NOTE:	From Scott's functions in VisualText.
********************************************/

LIBSTREAM_API bool u_delete(
	LPCTSTR* &mbcsstr
	)
{
if (mbcsstr)
  delete [] mbcsstr;
return true;
}

/********************************************
* FN:		<< OVERRIDE FOR UNICODE
* CR:		03/24/05 AM.
* SUBJ:	Unicode strings automatically converted to multibyte.
********************************************/


// MOVE TO LIBSTREAM								// 01/24/06 AM.
// Define these outside of any class!
/*
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
*/


#endif
