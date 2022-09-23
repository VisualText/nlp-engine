/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	GEN.H
* FILE:	lite/igen.h
* CR:		05/08/00 AM.
* SUBJ:	Class for code generation data.
*******************************************************************************/

#ifndef GEN_H_
#define GEN_H_
//#include "lite/nlppp.h"

// Forward references.
class Iaction;
class Ielt;
class Iexpr;
class Iexprstmt;
class Ifile;
class Iifstmt;
class Irule;
class Istmt;
class Ivar;
class RFASem;
class Isugg;
class Irecurse;
class Htab;
class Iwhilestmt;				// 08/31/00 AM.
class Ipnode;					// 10/18/00 AM.
class Ifunc;					// 12/19/01 AM.
class NLP;						// 12/22/01 AM.
class Ireturn;					// 03/07/02 AM.
//class tHtab;

/********************************************
* CLASS:	GEN
* CR:		05/08/00 AM.
* SUBJ:	Code generation object.
********************************************/

class Gen
{
	// Enable optimized access of code generation data.
	friend class Iaction;
	friend class Ielt;
	friend class Iexpr;
	friend class Iexprstmt;
	friend class Ifile;
	friend class Iifstmt;
	friend class Irule;
	friend class Istmt;
	friend class Ivar;
	friend class RFASem;
	friend class Isugg;
	friend class Irecurse;
	friend class Htab;
	friend class Iwhilestmt;			// 08/31/00 AM.
	friend class Ipnode;					// 10/18/00 AM.
	friend class Ifunc;					// 12/19/01 AM.
	friend class Ireturn;				// 03/07/02 AM.
//	friend class tHtab;

	friend std::_t_ostream &operator<<(std::_t_ostream &, Gen &);

public:
	Gen(										// Default constructor.
		_TCHAR *indent		 = _T(""),		// Amount to indent code.
		std::_t_ofstream *fcode	 = 0,				// Main output file.
		std::_t_ofstream *fhead  = 0,				// Header output file.
		std::_t_ofstream *fdata  = 0,				// Data output file.
		std::_t_ofstream *faux   = 0,				// Auxiliary output.		// 06/05/00 AM.
		int id = 0,							// Id for numbering.
		NlpppRegion region = REGNULL	// Pass file region being gen'd.
		);
	~Gen();										// Destructor.

	Gen(Gen &orig);							// Copy constructor.

	virtual void clear();
	virtual void zero();
#ifdef LINUX
	virtual void copy(const Gen *orig);
#else
	virtual void Gen::copy(const Gen *orig);
#endif

	const Gen &operator=(const Gen &);	// Assignment.


	// Access
	_TCHAR *getIndent();
	std::_t_ofstream *getFcode();
	std::_t_ofstream *getFhead();
	std::_t_ofstream *getFdata();
	std::_t_ofstream *getFaux();
	std::_t_ofstream *getEhash();
	std::_t_ofstream *getEdata();
	std::_t_ofstream *getEhead();
	std::_t_ofstream *getRhash();
	std::_t_ofstream *getRchain();
	std::_t_ofstream *getRdata();
	std::_t_ofstream *getRhead();
	std::_t_ofstream *getPassc();   // 04/03/09 AM.
	std::_t_ofstream *getPassh();   // 04/03/09 AM.
	int getId();
	int getRuleid();															// 05/12/00 AM.
	int getEltid();															// 05/12/00 AM.
	int getRecid();															// 05/30/00 AM.
	enum NlpppRegion getRegion();
	bool getLabel();															// 06/06/00 AM.
	NLP *getNLP();																// 12/22/01 AM.

	// Modify
	void setIndent(_TCHAR *);
	void setFcode(std::_t_ofstream *);
	void setFhead(std::_t_ofstream *);
	void setFdata(std::_t_ofstream *);
	void setFaux(std::_t_ofstream *);
	void setEhash(std::_t_ofstream *);
	void setEdata(std::_t_ofstream *);
	void setEhead(std::_t_ofstream *);
	void setRhash(std::_t_ofstream *);
	void setRchain(std::_t_ofstream *);
	void setRdata(std::_t_ofstream *);
	void setRhead(std::_t_ofstream *);
	void setPassc(std::_t_ofstream *);  // 04/03/09 AM.
	void setPassh(std::_t_ofstream *);  // 04/03/09 AM.
	void setId(int);
	void setRuleid(int);														// 05/12/00 AM.
	void setEltid(int);														// 05/12/00 AM.
	void setRecid(int);														// 05/30/00 AM.
	void setRegion(NlpppRegion);
	void setLabel(bool);														// 06/06/00 AM.
	void setNLP(NLP*);														// 12/22/01 AM.

	// General.
	bool writeHeaders();														// 05/10/00 AM.
	bool reset();				// Finish up, set up for next time.	// 05/16/00 AM.

	// Class-wide member functions.
	static void eol(std::_t_ofstream*);												// 04/04/03 AM.
	static void nl(std::_t_ofstream*);												// 04/04/03 AM.

protected:
	_TCHAR *indent_;							// How much to indent code.
	std::_t_ofstream *fcode_;						// Main output file.
	std::_t_ofstream *fhead_;						// Header output file.
	std::_t_ofstream *fdata_;						// Data output file.

	// An auxiliary code file, because analyzer.cpp is getting too long
	// for the Visual C++ compiler (65K limit on line numbers).
	std::_t_ofstream *faux_;						// Pres and other funs.		// 06/05/00 AM.

	// File for elt hash tables and associated string tables.	// 06/14/00 AM.
	std::_t_ofstream *ehash_;															// 06/14/00 AM.
	std::_t_ofstream *edata_;															// 06/14/00 AM.
	std::_t_ofstream *ehead_;		// Header file for ehash.					// 06/14/00 AM.

	// For total rule hashing.												// 06/16/00 AM.
	std::_t_ofstream *rhash_;		// For rule hash table.						// 06/16/00 AM.
	std::_t_ofstream *rchain_;	// For conflict chain.						// 06/17/00 AM.
	std::_t_ofstream *rdata_;		// For rule number arrays.					// 06/16/00 AM.
	std::_t_ofstream *rhead_;		// Header file.								// 06/16/00 AM.

	// GENERATING EACH PASS IN SEPARATE FILE.                      // 04/03/09 AM.
	std::_t_ofstream *passc_;    // For code of current pass.           // 04/03/09 AM.
	std::_t_ofstream *passh_;    // Header file only for current pass.  // 04/03/09 AM.

	int id_;									// Numbering id (eg, pass number).
	int ruleid_;							// Numbering id (rule number).
	int eltid_;								// Numbering id (elt within rule).
	int recid_;	// Numbering id (for recurse minipasses.)			// 05/30/00 AM.

	// Tell execution code what REGION (eg, post vs check)
	// the code is executing in.  Affects execution....
	enum NlpppRegion region_;

	// Flag that a label is to be generated after @CHECK region.
	bool label_;																// 06/06/00 AM.

	// Need info about current analyzer, NLP++ builtin funcs.	// 12/22/01 AM.
	NLP *nlp_;																	// 12/22/01 AM.


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;					// Count objects currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
