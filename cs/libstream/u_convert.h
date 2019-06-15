
#ifdef UNICODE

#ifndef _U_CONVERT_H_
#define _U_CONVERT_H_

LIBSTREAM_API bool u_isascii(LPCWSTR lpwStr, int typeASCII);
LIBSTREAM_API bool u_to_mbcs(
	LPCWSTR lpwUnicode,
	UINT nCodePage,
	LPCTSTR* &mbcsstr
	);
LIBSTREAM_API bool u_delete(LPCTSTR* &mbcsstr);


#endif

#endif
