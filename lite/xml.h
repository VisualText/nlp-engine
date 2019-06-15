/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XML.H
* FILE:	lite/xml.h
* CR:		12/27/99 AM.
* SUBJ:	Utilities for XML.
*
*******************************************************************************/

#ifndef XML_H_
#define XML_H_
//#include <iostream.h>
#include <assert.h>
#include "lite/lite.h"

// Readability definitions.
typedef void XML;

// Forward reference.
class Htab;

/********************************************
* CLASS:	XML
* CR:		12/29/99 AM.
* SUBJ:	Modularity class for XML stuff.
********************************************/

class LITE_API Xml
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Xml();									// Default constructor.
	~Xml();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static _TCHAR *text2xml(											// 12/29/99 AM.
		_TCHAR *text,
		_TCHAR *xml
		);
	static _TCHAR *text2xml(											// 12/29/99 AM.
		_TCHAR *text,
		long len,
		_TCHAR *xml
		);
	static _TCHAR *text2xml(
		_TCHAR *text,
		long len,
		Htab *htab
		);
	static inline void xmlchar(												// 03/16/10 AM.
		_TCHAR* &xml,				// Pointer to last filled char in xml buffer.
		_TCHAR* &text				// Pointer to next char to convert in text buffer.
		);
	static inline void htmlchar(												// 12/30/99 AM.
		_TCHAR* &xml,				// Pointer to last filled char in xml buffer.
		_TCHAR* &text				// Pointer to next char to convert in text buffer.
		);

	// Quick and dirty placement here.
	static _TCHAR *text2sql(_TCHAR*,_TCHAR*);							// 07/22/03 AM.
	static _TCHAR *text2sql(_TCHAR*,long,_TCHAR*);					// 07/22/03 AM.
	static _TCHAR *text2sql(_TCHAR*,long,Htab*);					// 07/22/03 AM.

	static _TCHAR *de_accent(_TCHAR*,_TCHAR*);									// 09/07/03 AM.
	static _TCHAR *de_accent(_TCHAR*,long,_TCHAR*);								// 09/07/03 AM.
	static _TCHAR *de_accent(_TCHAR*,long,Htab*);								// 09/07/03 AM.
	static inline void de_accent_ch(_TCHAR*&,_TCHAR*&);						// 09/06/03 AM.

	static _TCHAR *de_junk(_TCHAR*,_TCHAR*);          // 09/09/11 AM.
	static _TCHAR *de_junk(_TCHAR*,long,_TCHAR*);     // 09/09/11 AM.
	static _TCHAR *de_junk(_TCHAR*,long,Htab*);       // 09/09/11 AM.
	static inline void de_junk_ch(_TCHAR*&,_TCHAR*&); // 09/09/11 AM.

protected:
};



#endif
/*************************      END OF FILE      ******************************/
