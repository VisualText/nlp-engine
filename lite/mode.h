/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MODE.H
* FILE:	lite/mode.h
* CR:		07/19/99 AM.
* SUBJ:	Mode data for Gram concept hierarchy.
* NOTE:	Mode data passes information down the Gram hierarchy. It includes
*			flags for the levels of generalization of rules from samples.
*			Rules for each flagged level will be written to rules files and
*			retained internally.
*
*******************************************************************************/

#ifndef MODE_H_
#define MODE_H_
//#include <iostream.h>
#include <assert.h>
#include "lite/lite.h"
//#include "slist.h"
//#include "lite/dlist.h"						// 12/10/99 AM.


// Forward references
class Iarg;									// 12/10/99 AM.


/********************************************
* CLASS:	MODE
* CR:		07/19/99 AM.
* SUBJ:	Rule-generation information object.
********************************************/

class LITE_API Mode
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Mode &);

public:
	Mode(											// Default constructor.
		);
	~Mode();						// Destructor.

	Mode(Mode &orig);		// Copy constructor.

	void clear();
	void zero();
	void copy(const Mode *orig);
	//void Mode::copy(const Mode *orig);

	// Assignment.
	const Mode &operator=(const Mode &);

	// Access
	int getFlags();
	bool getRaw() const;				// 10/29/99 AM.
	bool getLit() const;
	bool getConstrain() const;
	bool getSplit() const;
	bool getGen() const;
	bool getOpt() const;
	bool getClosed() const;
	bool getXwhite() const;			// 08/09/99 AM.
	bool getLocked() const;			// 08/25/99 AM.
	bool getDirty() const;			// 08/27/99 AM.
	Dlist<Iarg>	*getLayer();		// 12/10/99 AM.
	bool getQuicksem() const;		// 12/27/99 AM.
	bool getCounter() const;		// 11/27/00 AM.
	bool getNotrig() const;			// 12/08/00 AM.

	// Modify
	void setFlags(int);
	void setRaw(bool);				// 10/29/99 AM.
	void setLit(bool);
	void setConstrain(bool);
	void setSplit(bool);
	void setGen(bool);
	void setOpt(bool);
	void setClosed(bool);
	void setXwhite(bool);			// 08/09/99 AM.
	void setLocked(bool);			// 08/25/99 AM.
	void setDirty(bool);				// 08/27/99 AM.
	void setLayer(Dlist<Iarg> *);	// 12/10/99 AM.
	void setQuicksem(bool);			// 12/27/99 AM.
	void setCounter(bool);			// 11/27/00 AM.
	void setNotrig(bool);			// 12/08/00 AM.

	// General

protected:

	int flags_;					// Bit-flag vector.

	// List of names to LAYER onto a generated rule.			// 12/10/99 AM.
	Dlist<Iarg> *layer_;													// 12/10/99 AM.

#ifdef REF_
//	bool lit_;						// Retain literal rules.
//	bool constrain_;				// Retain constrained rules.
//	bool split_;					// Retain split rules (ie, generalized rules
										// with containing label information.)
//	bool gen_;						// Retain the most generalized rules.
//	bool opt_;						// Retain rules that contain optionality info.
										// UNIMPLEMENTED rule generation stuff.

//	bool closed_;					// If closed-set applies to all rule elements
										// of all rules in the current subhierarchy.
#endif

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
