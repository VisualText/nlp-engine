/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	DSEM.H
* FILE:	lite/dsem.h
* CR:		08/03/99 AM.
* SUBJ:	Class for dynamic semantics.
* NOTE:	Dynamic semantics refers to data inside parse tree nodes.  The
*			user can update this data via code actions and rule actions.
*
*******************************************************************************/

#ifndef DSEM_H_
#define DSEM_H_

//#include <iostream>											// Upgrade.	// 01/24/01 AM.
//using namespace std;											// Upgrade.	// 01/24/01 AM.
#include <assert.h>

// Forward references
class Parse;

/********************************************
* CLASS:	DSEM
* CR:		08/03/99 AM.
* SUBJ:	Modularity class for node's dynamic semantics.
********************************************/

class Dsem
{
	//friend ostream &operator<<(ostream &, Dsem &);

public:
	Dsem();									// Default constructor.
	~Dsem();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static bool create(_TCHAR *name, Iarg *val, Pn *pn, Parse *parse);
	static bool createstrs(_TCHAR *name, Pn *pn, Parse *parse);
	static bool find(
		_TCHAR *name,					// Variable name to find.
		Pn *pn,
		Parse *parse,
		/*DU*/
		Ipair* &pair				// Pair found or null.
		);
	static bool inc(_TCHAR *name, Pn *pn, Parse *parse);
	static bool addStrs(
		_TCHAR *name,			// Variable name.
		_TCHAR *str,			// String value to add.
		Pn *pn,
		Parse *parse
		);
	static bool setVal(
		_TCHAR *name, Pn *pn, Parse *parse, _t_ostream *ostr);
	static bool val(
		_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ _TCHAR* &str);
	static bool val(
		_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ _t_ostream* &ostr);
	static bool vals(
		_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ Delt<Iarg>* &values);
	static bool vals(
		_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ Dlist<Iarg>* &list);
	static bool sortVals(
		_TCHAR *name, Pn *pn, Parse *parse);

protected:
};



#endif
/*************************      END OF FILE      ******************************/
