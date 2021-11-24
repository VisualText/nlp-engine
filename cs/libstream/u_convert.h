
#ifdef UNICODE

#ifndef _U_CONVERT_H_
#define _U_CONVERT_H_

//extern LIBSTREAM_API bool u_isascii(const _TCHAR lpwStr, int typeASCII);
extern LIBSTREAM_API bool u_to_mbcs(
	const _TCHAR* lpwUnicode,
	UINT nCodePage,
	const _TCHAR* &mbcsstr
	);
extern LIBSTREAM_API bool u_delete(const _TCHAR* &mbcsstr);


#endif

#endif
