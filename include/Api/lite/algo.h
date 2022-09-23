/*******************************************************************************
Copyright � 2001-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ALGO.H
* FILE:	include/API/lite/algo.h
* CR:		10/12/98 AM.
* SUBJ:	Class representing the generic analyzer pass algorithm.
* NOTE:	Virtual class.
*			10/18/98 AM. Renamed from Pass to Algo.
*
*******************************************************************************/

#ifndef ALGO_H_
#define ALGO_H_

#include "lite.h"

// Forward references.
class Parse;					// 12/04/98 AM.
class Seqn;


/********************************************
* CLASS:	ALGO
* CR:		10/12/98 AM.
* SUBJ:	Class for generic analyzer pass.
********************************************/

class LITE_API Algo
{
public:
	Algo(_TCHAR * = 0);	// Constructor.
	virtual ~Algo();

	// Access
	_TCHAR *getName();
	bool Debug();

	// Modify
	void	setName(_TCHAR *);
	void  setDebug(bool);

	// General

	// Virtual function. Defines Algo as an ABSTRACT CLASS.
	// Getting error return from individual pass.					// 01/26/02 AM.
	virtual bool Execute(Parse *, Seqn *) = 0;	// Let the pass do its thing.
	virtual Algo &dup() = 0;		// Algo copies itself! 12/03/98 AM.
	virtual void setup(_TCHAR *s_data) = 0;	// Algo sets up as analyzer pass.

	// Class-wide
	static bool userAlgo(_TCHAR *s_algo, _TCHAR *s_data, /*DU*/ Algo* &algo);
	static bool regAlgo(Algo &);
	static void clean();					// 12/04/98 AM.

private:
protected:					// Accessible by derived pass classes.
	// Name of the algorithm.  For checking user-registered algorithm names,
	// and for pretty-printing.
	_TCHAR *name;

	bool debug_;

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream*ofstr=0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
