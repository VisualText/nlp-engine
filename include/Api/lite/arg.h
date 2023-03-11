/*******************************************************************************
Copyright (c) 2001-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ARG.H
* FILE:	lite\arg.h
* CR:		11/29/98 AM.
* SUBJ:	Low-level utilities for analysis algorithms.
* NOTE:	Just provides functions.  Should not create objects of this class.
*
*******************************************************************************/

#ifndef ARG_H_
#define ARG_H_
//#include <iostream.h>
#include <assert.h>
//#include "pn.h"
//#include "tree.h"

typedef void TREE;	// 07/07/03 AM.
typedef void NODE;	// 07/07/03 AM.


// Forward references
class RFASem;					// 02/22/00 AM.
class Iarg;						// 02/26/01 AM.
class Pn;						// 02/26/01 AM.
class Nlppp;					// 11/14/02 AM.
class Parse;					// 06/12/03 AM.

/********************************************
* CLASS:	ARG
* CR:		11/29/98 AM.
* SUBJ:	Convenience class for fetching pass file arguments.
********************************************/

class LITE_API Arg															// 08/29/02 AM.
{
	//friend ostream &operator<<(ostream &, Arg &);

public:
	Arg();										// Default constructor.
	~Arg();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static bool done(DELTS*,_TCHAR*,									// 05/26/02 AM.
								Parse* = 0);									// 06/12/03 AM.
	static bool elt1(
		TREE *collect,		// Tracking for current rule match.
		_TCHAR *errstr,				// String for reporting errors.
		/*DU*/
		DELTS* &args,		// Remaining arguments in list.
		NODE* &coll,			// Collect for rule element, if any.
		NODE* &start,			// First node for rule element, if any.
		NODE* &end
		);

	static bool num1(_TCHAR*,/*DU*/DELTS*&,long long&,bool=false);// 05/26/02 AM.
   static bool float1(_TCHAR*,/*DU*/DELTS*&,float&,bool=false);// 04/29/04 AM.
	static bool str1(_TCHAR*,/*DU*/DELTS*&,_TCHAR*&);				// 05/26/02 AM.
	static bool sem1(_TCHAR*,Nlppp*,/*DU*/DELTS*&,RFASem*&,	// 05/26/02 AM.
																bool=false);	// 05/04/03 AM.
	static bool ref1(_TCHAR*,/*DU*/DELTS*&,RFASem*&);			// 05/26/02 AM.
	static bool str_or_num1(
							_TCHAR*,/*DU*/DELTS*&,_TCHAR*&,long long&);	// 05/26/02 AM.
	static bool any1(
				_TCHAR*,Parse*,/*DU*/DELTS*&,_TCHAR*&,long long&,float&,RFASem*&);	// 05/26/02 AM.
	static bool any1(_TCHAR*,Parse*, /*DU*/ DELTS*&,enum Iargtype &,// 05/29/02 AM.
		_TCHAR*&,long long&,float&,RFASem*&);									// 05/29/02 AM.
	static bool arg1(_TCHAR*,/*DU*/DELTS*&,Iarg*&);				// 05/26/02 AM.

protected:
};



#endif
/*************************      END OF FILE      ******************************/
