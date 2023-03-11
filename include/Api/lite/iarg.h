/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IARG.H
* FILE:	include/API/lite/iarg.h
* CR:		10/24/98 AM.
* SUBJ:	Class representing a generic rule-action's argument.
* NOTE:	Being used generically in many places.
*
*******************************************************************************/

#ifndef IARG_H_
#define IARG_H_

//#include "lite/dlist.h"	// 11/24/98 AM.
#include "lite/lite.h"

// Forward reference.
class Parse;	// For interning strings.		// 01/09/07 AM.
class RFASem;											// 02/16/00 AM.
class Starr;											// 05/06/00 AM.

enum LITE_API Iargtype
	{
	IANULL,				// No type set.
	IASTR,				// String type.
	IANUM,				// Numeric type.
	IAOSTREAM,			// Output stream type.	// 02/09/99 AM.
	IASEM,				// Semantic object.		// 02/16/00 AM.
	IAFLOAT,				// Float type.				// 08/17/01 AM.
	IAREF					// Call by reference		// 05/26/02 AM.
	};

union LITE_API Iargval
	{
	_TCHAR *str_;
	long long num_;
	std::_t_ostream *out_;		// 02/09/99 AM.
	RFASem *sem_;		// 02/16/00 AM.
	float float_;		// 08/17/01 AM.
	};

/********************************************
* CLASS:	IARG
* CR:		10/24/98 AM.
* SUBJ:	Class for rule-action's arguments.
********************************************/

class LITE_API Iarg
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iarg &);					// 11/22/98 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &output, Delt<Iarg> &);	// 11/22/98 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Iarg> &);			// 11/22/98 AM.
public:
	Iarg(enum Iargtype = IANULL);	// Default constructor.
	Iarg(_TCHAR *);
	Iarg(long long);
	Iarg(RFASem *);															// 02/16/00 AM.
	Iarg(RFASem *, enum Iargtype);										// 05/26/02 AM.
	Iarg(float);																// 08/17/01 AM.
	Iarg(std::_t_ostream *);															// 02/09/99 AM.
	~Iarg();								// Destructor.

	Iarg(Iarg &orig);					// Copy constructor.				// 07/02/99 AM.

	void clear();																// 07/02/99 AM.
	void zero();																// 07/02/99 AM.
	void copy(const Iarg *orig);

	// Assignment.																// 07/02/99 AM.
	const Iarg &operator=(const Iarg &);

	// Special copy and delete of lists.
	// Copy list,delt,iarg, but not anything the iarg points to.
	// Same for delete.
	static Dlist<Iarg> *copy_top(Dlist<Iarg> *);						// 02/18/00 AM.
	static void delete_top(Dlist<Iarg> *);								// 02/18/00 AM.
	static Dlist<Iarg> *copy_args(Dlist<Iarg> *,						// 12/05/00 AM.
								Parse* = 0, bool = false);					// 01/09/07 AM.
	static Iarg *copy_arg(Iarg*);											// 04/08/02 AM.
	static bool intern_strings(Dlist<Iarg> *args,Parse *parse);	// 07/23/07 AM.

	// Access.
	// <TYPE> getData();				// Each derived class should define this.
	enum Iargtype getType();
	_TCHAR *getStr();
	long long getNum();
	std::_t_ostream *getOstream();													// 02/09/99 AM.
	RFASem *getSem();															// 02/16/00 AM.
	float getFloat();															// 08/17/01 AM.

	long long *getNumaddr();														// 06/08/02 AM.
	float *getFloataddr();													// 06/08/02 AM.

	// Modify
//	virtual void setData(void) = 0;	// Let derived class override.
	void setType(enum Iargtype);
	void setStr(_TCHAR *);
	void setNum(long long);
	void setOstream(std::_t_ostream *);											// 02/09/99 AM.
	void setSem(RFASem *);													// 02/16/00 AM.
	void setFloat(float);													// 08/17/01 AM.

	// General

	// Fetch a number from arg.  If string, try to convert.		// 11/09/99 AM.
	bool fetchNum(/*UP*/ long long &num);										// 11/09/99 AM.

	// Class-wide

	// Find given string value in a list of args.					// 05/21/99 AM.
	static Delt<Iarg> *findArg(Dlist<Iarg> *dlist, _TCHAR *key);
	static Delt<Iarg> *findArgcase(Dlist<Iarg>*, _TCHAR*);			// 08/10/03 AM.
	static Delt<Iarg> *whiteArg(Dlist<Iarg> *dlist);				// 08/10/99 AM.
	static void genArgs(														// 06/15/99 AM.
		Dlist<Iarg> *args,
		_TCHAR *sep,
		std::_t_ostream &ofile,
		bool trunc = false);													// 06/05/00 AM.
	static void genArg(Iarg *arg, std::_t_ostream &ofile,					// 06/15/99 AM.
								bool = false);		// CAST OFF.			// 11/06/01 AM.

	static void genName(_TCHAR *name, std::_t_ostream &ofile);				// 07/11/99 AM.

	static bool same(Dlist<Iarg> *list1, Dlist<Iarg> *list2);	// 06/07/99 AM.
	static bool same(Iarg *arg1, Iarg *arg2);							// 06/07/99 AM.

	static Starr *strings_to_starr(Dlist<Iarg> *);					// 05/06/00 AM.

private:
	enum Iargtype type;				// Type of data that class represents.
	union Iargval val_;				// Arg's data.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);		// Pretty-print the count.
private:
	static int count_;				// Count nodes currently allocated.
#endif
};


#endif
