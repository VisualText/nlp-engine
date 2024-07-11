/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	VAR.H
* FILE:	lite\var.h
* CR:		12/07/98 AM.
* SUBJ:	Interpretive var management.  Part of semantic processing.
* NOTE:	Just provides functions.  Should not create objects of this class.
*			Reusing Ipair for the variable object.  May want to build a similar
*			thing for Var.  For one thing, Ipair allows multiple values for a
*			key (i.e., variable name).
*
*******************************************************************************/

#ifndef VAR_H_
#define VAR_H_
//#include <iostream.h>
#include <assert.h>


// Forward references
class Parse;
//class Seqn;
class Iarg;
class Ipair;
class RFASem;			// 02/22/00 AM.

/********************************************
* CLASS:	VAR
* CR:		12/07/98 AM.
* SUBJ:	Class for interpretive vars.
********************************************/

class Var
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Var();									// Default constructor.
	~Var();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static bool create(					// 08/03/99 AM.
		_TCHAR *name, Iarg *val, /*DU*/ Dlist<Ipair>* &dlist, Ipair* &pair);
	static bool create(					// 11/03/99 AM.
		_TCHAR *name, Dlist<Iarg> *vals, /*DU*/ Dlist<Ipair>* &dlist);
	static bool createstrs(				// 08/03/99 AM.
		_TCHAR *name, /*DU*/ Dlist<Ipair>* &dlist);
	static bool inc(						// 08/03/99 AM.
		_TCHAR *name, Parse *parse, /*DU*/ Dlist<Ipair>* &dlist);
	static bool addNum(					// 08/05/99 AM.
		_TCHAR *name,
		long long num,
		Parse *parse,
		/*DU*/
		Dlist<Ipair>* &dlist
		);
	static bool addStrs(					// 08/03/99 AM.
		_TCHAR *name,			// Variable name.
		_TCHAR *str,			// String value to add.
		/*DU*/
		Dlist<Ipair>* &dlist
		);
	static bool setVals(					// 11/03/99 AM.
		_TCHAR *name,
		Dlist<Iarg> *vals,
		/*DU*/
		Dlist<Ipair>* &dlist
		);
	static bool setVal(					// 10/31/99 AM.
		_TCHAR *name,
		_TCHAR *str,
		bool,
		bool,
		/*DU*/
		Dlist<Ipair>* &dlist
		);
	static bool setVal(
		_TCHAR *name,
		std::_t_ostream *ostr,
		/*DU*/
		Dlist<Ipair>* &dlist
		);
	static bool setVal(				// 02/22/00 AM.
		_TCHAR *name,
		RFASem *sem,
		bool,
		bool,
		/*DU*/
		Dlist<Ipair>* &dlist
		);
	static bool delVar(				// 02/22/00 AM.
		_TCHAR *name,
		Dlist<Ipair>* &dlist
		);


	static bool setVal(_TCHAR*,long long,bool,bool,/*DU*/Dlist<Ipair>*&);			// 06/26/01 AM.
	static bool setVal(_TCHAR*,float,bool,bool,/*DU*/Dlist<Ipair>*&);			// 08/18/01 AM.
	static bool rmVal(_TCHAR*,Dlist<Ipair>*&,int&);

	static bool setVal(Ipair*,long long);										// 11/15/99 AM.
	static bool setVal(Ipair*,float);									// 08/18/01 AM.
	static bool setVal(Ipair*,_TCHAR*);									// 11/15/99 AM.
	static bool setVal(Ipair*,std::_t_ostream*);								// 12/31/99 AM.
	static bool setVal(Ipair*,RFASem*);									// 02/22/00 AM.
	static bool setVal(Ipair*,Dlist<Iarg>*);							// 08/10/02 AM.

	static bool pushVal( // 12/14/14 AM.
		_TCHAR *name,
		_TCHAR *str,
		bool,
		bool,
		/*DU*/
		Dlist<Ipair>* &dlist
		);
	static bool pushVal(_TCHAR*,RFASem*,bool,bool,/*DU*/Dlist<Ipair>*&); // 12/15/14 AM.
	static bool pushVal(_TCHAR*,long long,bool,bool,/*DU*/Dlist<Ipair>*&);    // 12/15/14 AM.
	static bool pushVal(_TCHAR*,float,bool,bool,/*DU*/Dlist<Ipair>*&);   // 12/15/14 AM.

	static bool find(
		_TCHAR *name,					// Variable name to find.
		Dlist<Ipair> *dlist,		// List of variable-value pairs.
		/*DU*/
		Ipair* &pair				// Pair found or null.
		);
	static bool remove(
		_TCHAR *name,					// Variable name to find.
		Dlist<Ipair> *dlist		// List of variable-value pairs.
		);
	static bool val(															// 08/03/99 AM.
		_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ _TCHAR *str);
	static bool val(															// 08/06/99 AM.
		_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ long long &num);
	static bool val(															// 08/03/99 AM.
		_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ std::_t_ostream* &ostr);
	static bool vals(															// 08/03/99 AM.
		_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ Delt<Iarg>* &values);
	static bool vals(															// 08/03/99 AM.
		_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ Dlist<Iarg>* &list);
	static bool sortVals(													// 08/03/99 AM.
		_TCHAR *name, Dlist<Ipair> *dlist);
	static bool val(Ipair *, /*DU*/ _TCHAR *str);
	static bool val(Ipair *, /*DU*/ long long &num);						// 08/06/99 AM.
	static bool val(Ipair*,/*DU*/ float&);								// 08/17/01 AM.
	static bool val(Ipair *pair, /*DU*/ std::_t_ostream* &ostr);			// 02/09/99 AM.

	static bool clean_vars(Dlist<Ipair> *dlist);						// 09/26/00 AM.

	// PARSE-SPECIFIC  (The old stuff).
	static bool create(_TCHAR *name, Iarg *val, Parse *parse
		//, Seqn *seqn														// 01/14/00 AM.
		);
	static bool createstrs(_TCHAR *name, Parse *parse
		//, Seqn *seqn														// 01/14/00 AM.
		);
	static bool find(
		_TCHAR *name,					// Variable name to find.
		Parse *parse,
		/*DU*/
		Ipair* &pair				// Pair found or null.
		);
	static bool inc(_TCHAR *name, Parse *parse);
	static bool addStrs(			// 01/24/99 AM.
		_TCHAR *name,			// Variable name.
		_TCHAR *str,			// String value to add.
		Parse *parse
		);
	static bool setVal(_TCHAR *name, Parse *parse, std::_t_ostream *ostr);	//2/9/99AM
	static bool val(_TCHAR *name, Parse *parse, /*DU*/ _TCHAR *str);
	static bool val(_TCHAR *name, Parse *parse, /*DU*/ std::_t_ostream* &ostr); // 2/9/99AM
	static bool val(_TCHAR *, Parse *, /*UP*/ long long &);				// 08/31/00 AM.
	static bool vals(_TCHAR *name, Parse *parse, /*DU*/ Delt<Iarg>* &values);
	static bool vals(_TCHAR *name, Parse *parse, /*DU*/ Dlist<Iarg>* &list);
	static bool sortVals(_TCHAR *name, Parse *parse);

	static bool tolowerVals(												// 12/15/99 AM.
		_TCHAR *name,
		Parse *parse
		);
	static bool tolowerVals(												// 12/15/99 AM.
		_TCHAR *name,
		Dlist<Ipair> *dlist,
		Parse *parse
		);
	static bool uniqVals(													// 12/15/99 AM.
		_TCHAR *name,
		Parse *parse
		);
	static bool uniqVals(													// 12/15/99 AM.
		_TCHAR *name,
		Dlist<Ipair> *dlist,
		Parse *parse
		);
	static bool filevar(												// 12/31/99 AM.
		_TCHAR *fname,				// Filename to open or use.
		Parse *parse,
		/*UP*/
		Ipair* &pair,				// The variable-value pair.
		std::_t_ostream* &ostr,			// Var's ostream value.
		bool &exists,				// True if ostream var was already present.
		bool &open,					// True if ostream already open.
		bool &inuse,				// True if var in use for a non-ostream value.
		bool &badname				// If bad filename.
		);

	static _TCHAR *getStr(Delt<Iarg> *value);
	static int   compare( const void *arg1, const void *arg2 );
	static bool dump(										// 12/13/99 AM.
		Dlist<Ipair> *vars,
		std::_t_ostream *ostr
		);
	static bool copy_vars(Dlist<Ipair> *, Dlist<Ipair>* &);		// 05/16/01 AM.
	static bool copy_vars(Dlist<Ipair> *, RFASem *sem1, CG *cg);

protected:
};



#endif
/*************************      END OF FILE      ******************************/
