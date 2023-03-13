/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STR.H
* FILE:	lite\str.h
* CR:		11/04/98 AM.
* SUBJ:	String and char functions.
*
*******************************************************************************/

#ifndef STR_H_
#define STR_H_

#include "prim/unicu.h"	// [UNICODE]

#define UINT long	// 09/26/19 AM.

class Parse;	// Forward reference.

#define SPELL_BUF_LEN 10001

LITE_API bool alphabetic(_TCHAR ch);										// 09/22/99 AM.
LITE_API bool is_upper(_TCHAR ch);											// 12/16/01 AM.
LITE_API bool is_lower(_TCHAR ch);											// 12/16/01 AM.
LITE_API _TCHAR to_upper(_TCHAR ch);											// 12/16/01 AM.
LITE_API _TCHAR to_lower(_TCHAR ch);											// 12/16/01 AM.

bool str_to_int(_TCHAR *, /*UP*/ int &);
bool str_to_long(_TCHAR *, /*UP*/ long long &);
bool str_to_long(_TCHAR *, /*UP*/ long &);
inline int char_to_digit(_TCHAR ch);
bool all_uppercase(_TCHAR *str);
bool all_lowercase(_TCHAR *str);										// 01/03/00 AM.
bool mixcap(_TCHAR *str);													// 01/15/00 AM.

_TCHAR *str_to_lower(_TCHAR *str, _TCHAR *buf);
_TCHAR *str_to_lower(_TCHAR *str);                   // 12/15/14 AM.
_TCHAR *str_to_upper(_TCHAR *str, _TCHAR *buf);							// 11/04/99 AM.
_TCHAR *str_to_upper(_TCHAR *str);                   // 12/15/14 AM.
bool zap_final_white(_TCHAR *str);										// 02/12/99 AM.

bool str_equal(_TCHAR *str1, _TCHAR *str2);							// 06/07/99 AM.

bool long_to_str(long num, /*UP*/ _TCHAR *buf);					// 11/03/99 AM.

bool str_to_cstr(															// 12/15/99 AM.
	_TCHAR *str,			// String to convert.
	/*UP*/
	_TCHAR *buf			// Supplied buffer to fill.
	);
inline void strcpy_e(														// 12/30/99 AM.
	_TCHAR* &ptr,			// Pointer to buffer for copying into.
							// Points to last FILLED char.
	_TCHAR *str			// Terminated string to copy.
	);
//inline void strcpy_ee(														// 09/27/00 AM.
//	char* &ptr,			// First EMPTY spot.
//	char *str
//	);
bool clean_str(
	_TCHAR *str,			// String to clean.
	/*UP*/
	_TCHAR *buf			// Supplied buffer to fill.
	);

bool find_str(_TCHAR *, const _TCHAR **);									// 05/17/00 AM.
bool find_str_nocase(_TCHAR *, const _TCHAR **);							// 06/07/00 AM.

bool strlen_eq(_TCHAR *, long);												// 05/24/00 AM.
bool strlen_eq(long, _TCHAR *);												// 05/24/00 AM.
bool strlen_eq(_TCHAR *, _TCHAR *);											// 05/24/00 AM.
long binary_search(															// 06/09/00 AM.
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long len					// Active length of array (excluding 0th elt).
	);
long binary_search(															// 06/09/00 AM.
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long ii,					// "Left" end of array.
	long jj					// "Right" end of array.
	);

// FUNCTIONS TO CONVERT FILES WITH JUNK IN THEM.					// 07/18/00 AM.
// Trying much longer lines by default.                              // 10/07/05 AM.
//bool readConvert(_TCHAR *,_TCHAR* &,long &,long=MAXSTR);				// 07/18/00 AM.
//bool readConvertU(_TCHAR *,_TCHAR* &,long &,long=MAXSTR);          // 03/03/05 AM.
bool readConvert(_TCHAR *,_TCHAR* &,long &,long=MAXLINE);				// 10/07/05 AM.
bool readConvertU(_TCHAR *,_TCHAR* &,long &,long=MAXLINE);           // 10/07/05 AM.
bool UTL_GetUnicodeFromMBCS(                                         // 03/03/05 AM.
	wchar_t* &pszUnicodeString,
	LPCTSTR lpszMBCS,
	long &len,
	UINT nCodePage
	);


bool writeConvert(_TCHAR *,_TCHAR *,long=0,long=MAXSTR);				// 07/18/00 AM.

#ifndef LPCWSTR
// Hack. 09/16/20 AM.
#define LPCWSTR double
#endif
bool UTL_GetMBCSFromUnicodeString(                                   // 03/06/05 AM.
	LPCWSTR lpwUnicode,
	UINT nCodePage,
	LPCTSTR* &pszMultiByteString
	);


_TCHAR *str_to_title(_TCHAR *str, _TCHAR *buf);								// 08/21/00 DD.
_TCHAR *str_wrap(_TCHAR *str, long wrapSize, _TCHAR *buf);           // 09/22/00 DD.
_TCHAR *str_subst(_TCHAR *str,													// 09/22/00 DD.
	_TCHAR *oldSubStr,
	_TCHAR *newSubStr,
	_TCHAR *buf);

int str_spell_compare(_TCHAR *str, _TCHAR *candidateStr);                           // 09/26/00 DD.
void str_spell_candidate(_TCHAR *str, _TCHAR *strList, _TCHAR *buf); // 09/26/00 DD.

long binary_spell(															// 09/27/00 AM.
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long len,					// Active length of array (excluding 0th elt).
	/*UP*/
	long &
	);
long binary_spell(															// 09/27/00 AM.
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long ii,					// "Left" end of array.
	long jj,					// "Right" end of array.
	/*UP*/
	long &
	);

_TCHAR *spell_candidates(														// 10/16/00 AM.
	_TCHAR *word,				// The word to be checked.
	_TCHAR *buf,				// Buffer for placing candidates.
	long maxbuf				// Maximum length of buffer.
	);
_TCHAR *spell_candidates(														// 09/27/00 AM.
	_TCHAR *word,				// The word to be checked.
	_TCHAR *arr[],			// Alpha-sorted, null terminated array of strings.
	long len,				// Length of array (excluding null terminator string).
	_TCHAR *buf,				// Buffer for placing candidates.
	long maxbuf,
	/*UP*/
	_TCHAR* &ptr				// End of active buffer.					// 10/16/00 AM.
	);

long prefix_match_len(_TCHAR *str1, _TCHAR *str2);						// 09/27/00 AM.
long set_arr_len(_TCHAR *arr[]);											// 09/27/00 AM.

_TCHAR *leftjustify(															// 10/02/00 AM.
	_TCHAR *str,
	long fieldsize
	);
_TCHAR *rightjustify(															// 10/01/00 AM.
	long num,
	long fieldsize
	);

inline bool spell_new_low(													// 10/16/00 AM.
	int weight,				// Current candidate weight
	long len,				// Current candidate length
	int low_weight,		// Best so far
	long low_len,			// Best so far
	long best_len			// Best possible length.
	);
bool str_trim(																	// 11/10/00 DD.
        _TCHAR *str,                      // String to clean.
        /*UP*/
        _TCHAR *buf                       // Supplied buffer to fill.
        );
bool str_ends_with(_TCHAR *str, _TCHAR *ending);							// 11/20/00 DD.
bool str_starts_with(_TCHAR *str, _TCHAR *startstr);							// 08/05/21 Dd.

bool str_unescape(_TCHAR*,_TCHAR*,_TCHAR,_TCHAR*);							// 07/26/01 DD.
bool str_escape(_TCHAR*,_TCHAR*,_TCHAR,_TCHAR*);								// 07/26/01 DD.
bool convert_to_float(_TCHAR *str1, _TCHAR *str2, /*UP*/ float &fnum);
bool s_to_f(_TCHAR *str, /*UP*/ float *fnum);							// 08/23/01 AM.

bool str_piece(_TCHAR*,long,long,long,/*UP*/_TCHAR*);					// 11/18/02 AM.
bool str_unique_chars(_TCHAR*,/*UP*/_TCHAR*);

// Should go in a morph module.
bool strsuffix(_TCHAR*,_TCHAR*);												// 11/23/01 AM.
_TCHAR *strstem(_TCHAR*);														// 12/28/01 AM.

bool url_base(_TCHAR*,/*UP*/_TCHAR*);										// 03/16/03 AM.
bool resolve_url(_TCHAR*,_TCHAR*,Parse*,/*DU*/_TCHAR*);					// 05/12/03 AM.
bool unpack_url(																// 05/13/03 AM.
	_TCHAR *url_str,
	Parse *parse,
	/*DU*/
	_TCHAR *scheme,
	_TCHAR *net_loc,
	bool &absflag,
	long &pathlen,
	Dlist<Iarg> *&path,
	bool &endslash,
	_TCHAR *params,
	_TCHAR *query,
	_TCHAR *fragment
	);
bool combine_url(																// 05/14/03 AM.
	_TCHAR *buf,
	_TCHAR *scheme,
	_TCHAR *net_loc,
	bool absflag,
	long pathlen,
	Dlist<Iarg> *path,
	bool endslash,
	_TCHAR *params,
	_TCHAR *query,
	_TCHAR *fragment
	);

long levenshtein_int(			// 03/06/07 AM.
	_TCHAR *s1,
	_TCHAR *s2
	);

long u_strlen(_TCHAR *name);	// [UNICODE]

#endif
