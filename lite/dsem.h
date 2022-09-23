/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
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
#ifdef LINUX
	static bool create(_TCHAR *name, Iarg *val, Pn *pn, Parse *parse);
#else
	static bool create(_TCHAR *name, Iarg *val, Pn *pn, Parse *parse);
#endif
	static bool createstrs(_TCHAR *name, Pn *pn, Parse *parse);
#ifdef LINUX
	static bool find(
#else
	static bool find(
#endif
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
		_TCHAR *name, Pn *pn, Parse *parse, std::_t_ostream *ostr);
	static bool val(
		_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ _TCHAR* &str);
	static bool val(
		_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ std::_t_ostream* &ostr);
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
