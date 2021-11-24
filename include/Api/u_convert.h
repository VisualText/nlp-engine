/*******************************************************************************
Copyright (c) 2006-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#ifdef UNICODE

#ifndef _U_CONVERT_H_
#define _U_CONVERT_H_

LIBSTREAM_API bool u_isascii(const _TCHAR lpwStr, int typeASCII);
LIBSTREAM_API bool u_to_mbcs(
	const _TCHAR* lpwUnicode,
	UINT nCodePage,
	const _TCHAR* &mbcsstr
	);
LIBSTREAM_API bool u_delete(const _TCHAR* &mbcsstr);


#endif

#endif
