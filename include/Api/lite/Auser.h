/*******************************************************************************
Copyright (c) 2001-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AUSER.H
* FILE:	include/API/lite/Auser.h
* CR:		02/13/01 AM.
* SUBJ:	Auser class is for info transfer between Lite and User libraries.
*
*******************************************************************************/

#ifndef AUSER_H_
#define AUSER_H_

#ifndef CONCEPT
typedef void CONCEPT;				// A concept.
typedef void ATTR;					// An attribute.
typedef void VAL;						// An attribute value.
typedef void PHRASE;					// A concept's phrase of nodes.

typedef void DELTS;              // Doubly-linked list.  // 07/07/03 AM.
typedef void NODE;               // Parse tree node.     // 07/07/03 AM.
typedef void TREE;
#endif

enum Rettype
	{
	RETNULL,
	RETNODE,
	RETSTR,
	RETLONG,
	RETOSTR,
	RETBOOL,		// May as well start on this type.
	RETCONCEPT,
	RETPHRASE,
	RETATTR,
	RETVAL,
	RETDUMMY
	};

// Forward reference.
class Nlppp;
class RFASem;
class Fn;
class Pn;
class user;		// In USER.DLL.
class Iarg;
class Parse;
class Ana;

/********************************************
* CLASS:	AUSER
* CR:		02/13/01 AM.
* SUBJ:	Interpreted parse-tree node semantic object.
********************************************/

class LITE_API Auser
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Auser &);
	friend class Fn;
	friend class user;			// In USER.DLL.

public:
	Auser(											// Default constructor.
		Nlppp *nlppp = 0,
		void *vtrun = 0														// 08/28/02 AM.
		);
	~Auser();										// Destructor.

	Auser(Auser &orig);							// Copy constructor.

	void clear();
	void zero();
	void copy(const Auser *orig);

	const Auser &operator=(const Auser &);	// Assignment.

	// Access
	Nlppp *getNlppp();
	bool	 getOkret();
	bool	 getBadname();

	// Modify
	void setNlppp(Nlppp *);
	void setOkret(bool);
	void setBadname(bool);

	// Set return value and type.
	void retNode(NODE *);
	void retStr(_TCHAR *);
	void retLong(long);
	void retOstr(std::_t_ostream *);
	void retBool(bool);

	void retConcept(CONCEPT *);
	void retPhrase(PHRASE *);
	void retAttr(ATTR *);
	void retVal(VAL *);

	// General.


	// Class-wide member functions.
   static Iarg *getData(DELTS*);                               // 07/07/03 AM.
	static DELTS *Left(DELTS*);                                 // 07/07/03 AM.
	static DELTS *Right(DELTS*);                                // 07/07/03 AM.
	static Parse *getParse(Nlppp*);										// 07/07/03 AM.
	static Ana *getAna(Parse*);											// 07/07/03 AM.
	static Pn *getNodedata(NODE*);										// 07/07/03 AM.
	static NODE *getRoot(TREE*);											// 07/07/03 AM.
	static NODE *getNodedown(NODE*);										// 12/04/03 AM.
	static NODE *getNoderight(NODE*);									// 12/04/03 AM.
	static NODE *getNodeleft(NODE*);										// 12/04/03 AM.
	static NODE *getNodeup(NODE*);										// 12/04/03 AM.

	static NODE *redsingle(NODE*,NODE*,_TCHAR*,bool,bool,Nlppp*); // 12/04/03 AM.

	// PUBLIC MEMBERS.
	void *vtrun_;																// 08/28/02 AM.

protected:
	// Reference to current parse state.
	Nlppp *nlppp_;

	bool okret_;		// If function executed ok.
	bool badname_;		// If function was present in user project.

	// Hard-wired semantic object for function's return value.
	enum Rettype rettype_;					// Type of return value.
	union Retval
		{
		// Dummy variant, to be able to reset union to null.
		void *ptr_;

		CONCEPT *kbconcept_;
		PHRASE *kbphrase_;
		ATTR *kbattr_;
		VAL *kbval_;

		NODE *node_;

		_TCHAR *str_;
		long long long_;
		std::_t_ostream *ostr_;
		bool bool_;
		} retval_;

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;					// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
