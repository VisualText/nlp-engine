/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	RFASEM.H
* FILE:	include/API/lite/rfasem.h
* CR:		11/05/98 AM.
* SUBJ:	Class for RFA node's semantic data.
* NOTE:	Once again will use an apocryphal union.
*
*******************************************************************************/

#ifndef RFASEM_H_
#define RFASEM_H_

#include "slist.h"
//#include "lite/dlist.h"
//#include "irule.h"
#include "sem.h"
//#include "iregion.h"
//#include "irecurse.h"
//#include "ifile.h"
//#include "lite/node.h"

#include "consh/libconsh.h"
#include "consh/cg.h"
#include "htab.h"
#include "kb.h"

// Taken from cg.h .  // 02/22/00 AM.
#ifndef CONCEPT
typedef void CONCEPT;				// A concept.
typedef void ATTR;					// An attribute.
typedef void VAL;						// An attribute value.
typedef void PHRASE;					// A concept's phrase of nodes.
#endif


enum RFASemtype
	{
	RSNULL,
	RSFILE,
	RSGLOBAL,
	RSRECURSES,							// 11/24/98 AM.
	RSRECURSE,							// 11/24/98 AM.
	RSREGIONS,							// 11/24/98 AM.
	RSREGION,
	RSNODES,					// Select specifier.		// 03/25/99 AM.
	RSMULTI,					// Select specifier.		// 03/25/99 AM.
	RSPATH,					// Select specifier.		// 10/19/99 AM.
	RSRULES,
	RSRULE,
	RSSUGG,
	RSPHRASE,
	RSARGS,								// 11/20/98 AM.
	RSPRES,
	RSPOSTS,
	RSRULELT,
	RSPAIRS,
	RSACTIONS,							// 11/23/98 AM.
	RSPAIR,
	RSACTION,							// 11/23/98 AM.
	RSARG,								// 11/20/98 AM.
	RSPRE,								// 11/29/98 AM.
	// More, for micro level.
	RSNAME,								// 11/10/98 AM.
	RSSTR,								// 11/20/98 AM.
	RSNUM,								// 11/21/98 AM.
	RSLONG,								// 11/10/99 AM. !
	RSOSTREAM,							// 12/31/99 AM.
	RSFLOAT,								// 08/16/01 AM.

	// NLP++								// 11/08/99 AM.
	RSSTMTS,								// 11/08/99 AM.
	RSSTMT,								// 11/11/99 AM.
	RSEXPR,								// 11/08/99 AM.
	RSVAR,								// 11/09/99 AM.
	RSOP,									// 11/09/99 AM.

	RSFUNC,	// Function def.		// 12/19/01 AM.

	// KB OBJECTS.						// 02/22/00 AM.
	RS_KBCONCEPT,						// 02/22/00 AM.
	RS_KBPHRASE,						// 02/22/00 AM.
	RS_KBATTR,							// 02/22/00 AM.
	RS_KBVAL,							// 02/22/00 AM.

	// PARSE TREE OBJECTS.			// 10/18/00 AM.
	RSPNODE,								// 10/18/00 AM.
	RSNODE,								// 10/18/00 AM.

	RSDUMMYTERMINAL
	};

// Forward references.
class Gen;								// 05/08/00 AM.
class Ifile;
class Irecurse;
class Iregion;
class Istmt;							// 11/11/99 AM.
class Iexpr;							// 11/09/99 AM.
class Ivar;								// 11/09/99 AM.
class Iop;								// 11/09/99 AM.
class Ipnode;							// 10/18/00 AM.
class Pn;								// 10/18/00 AM.
class Nlppp;							// 11/14/99 AM.
class Parse;							// 01/09/07 AM.
class Ipost;							// 02/27/01 AM.
class Irule;							// 02/27/01 AM.
class Ielt;								// 02/27/01 AM.
class Iarg;								// 02/27/01 AM.
class Ipre;								// 02/27/01 AM.
class Ipair;							// 02/27/01 AM.
class Iaction;							// 02/27/01 AM.
class Irule;							// 02/27/01 AM.
class Isugg;							// 02/27/01 AM.
class Ifunc;							// 12/19/01 AM.

/********************************************
* CLASS:	RFASEM
* CR:		11/05/98 AM.
* SUBJ:	Class for RFA node's semantic information.
********************************************/

class LITE_API RFASem : public Sem
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, RFASem &);	// 11/09/99 AM.

public:
	// If any of these become ambiguous, then add a type argument.
	RFASem(enum RFASemtype = RSNULL);	// Default constructor.
	RFASem(Ifile *);						// 11/24/98 AM.
	RFASem(Slist<Irecurse> *);			// 11/24/98 AM.
	RFASem(Irecurse *);					// 11/24/98 AM.
	RFASem(Slist<Iregion> *);			// 11/24/98 AM.
	RFASem(Iregion *);					// 11/24/98 AM.
	RFASem(Dlist<Irule> *);
	RFASem(Irule *);
	RFASem(Isugg *);
	RFASem(Dlist<Ielt> *);
	RFASem(Dlist<Iarg> *, enum RFASemtype = RSARGS);	// 03/25/99 AM.
	RFASem(Dlist<Ipre> *);
	RFASem(Dlist<Ipost> *);
	RFASem(Ielt *);
	RFASem(Iarg *);						// 11/20/98 AM.
	RFASem(Dlist<Ipair> *);
	RFASem(Dlist<Iaction> *);			// 11/23/98 AM.
	RFASem(Ipair *);
	RFASem(Iaction *);					// 11/23/98 AM.
	RFASem(Ipre *);						// 11/29/98 AM.
	RFASem(_TCHAR *, enum RFASemtype = RSNAME);	// 11/20/98 AM.
	RFASem(long long);										// 11/10/99 AM. !
	RFASem(float);										// 08/16/01 AM.
	RFASem(std::_t_ostream *);								// 12/31/99 AM.
	RFASem(Ifunc*);									// 12/19/01 AM.

	// NLP++ Semantics.								// 11/09/99 AM.
	RFASem(Slist<Istmt> *);							// 11/11/99 AM.
	RFASem(Istmt *);									// 11/11/99 AM.
	RFASem(Iexpr *);									// 11/09/99 AM.
	RFASem(Ivar *);									// 11/09/99 AM.
	RFASem(Iop *);										// 11/09/99 AM.

	// KB HANDSHAKE.									// 02/22/00 AM.
	RFASem(void *, enum RFASemtype = RSNULL, CG * = NULL);

	// PARSE TREE OBJECTS.							// 10/18/00 AM.
	RFASem(Ipnode *);									// 10/18/00 AM.
	RFASem(Node<Pn> *);								// 10/18/00 AM.

	// DESTRUCTOR.
	~RFASem();

	// COPY CONSTRUCTOR.								// 06/27/01 AM.
	RFASem *copy_rfasem(Parse* = 0,bool=false);	// Copy constructor

	// Access
	enum RFASemtype getType();

	Ifile *getRulesfile();							// 11/24/98 AM.
	Slist<Irecurse> *getRecurses();				// 11/24/98 AM.
	Irecurse *getRecurse();							// 11/24/98 AM.
	Slist<Iregion> *getRegions();					// 11/24/98 AM.
	Iregion *getRegion();							// 11/24/98 AM.
	Dlist<Irule> *getRules();
	Irule *getRule();
	Isugg *getSugg();
	Dlist<Ielt> *getPhrase();
	Dlist<Iarg> *getArgs();							// 11/20/98 AM.
	Dlist<Ipre> *getPres();
	Dlist<Ipost> *getPosts();
	Ielt *getRulelt();
	Dlist<Ipair> *getPairs();
	Dlist<Iaction> *getActions();					// 11/23/98 AM.
	Iarg *getArg();									// 11/20/98 AM.
	Ipair *getPair();
	Iaction *getAction();							// 11/23/98 AM.
	Ipre *getPre();									// 11/29/98 AM.
	_TCHAR *getName();
	_TCHAR *getNum();									// 11/21/98 AM.
	long long getLong();									// 11/10/99 AM.
	float getFloat();									// 08/16/01 AM.
	std::_t_ostream *getOstream();							// 12/31/99 AM.

	// NLP++ Semantics.								// 11/09/99 AM.
	Slist<Istmt> *getStmts();						// 11/11/99 AM.
	Istmt *getStmt();									// 11/11/99 AM.
	Iexpr *getExpr();									// 11/09/99 AM.
	Ivar *getVar();									// 11/09/99 AM.
	Iop  *getOp();										// 11/09/99 AM.
	Ifunc *getFunc();									// 12/20/01 AM.

	// KB Handshake.									// 02/22/00 AM.
	CONCEPT *getKBconcept();						// 02/22/00 AM.
	CG *getCG();
	PHRASE  *getKBphrase();							// 02/22/00 AM.
	ATTR	  *getKBattr();							// 02/22/00 AM.
	VAL	  *getKBval();								// 02/22/00 AM.

	// Parse tree objects.							// 10/18/00 AM.
	Ipnode  *getPnode();								// 10/18/00 AM.
	Node<Pn> *getNode();								// 10/18/00 AM.


	// Modify
	void setType(enum RFASemtype);

	void setRulesfile(Ifile *);					// 11/24/98 AM.
	void setRecurses(Slist<Irecurse> *);		// 11/24/98 AM.
	void setRecurse(Irecurse *);					// 11/24/98 AM.
	void setRegions(Slist<Iregion> *);			// 11/24/98 AM.
	void setRegion(Iregion *);						// 11/24/98 AM.
	void setRules(Dlist<Irule> *);
	void setRule(Irule *);
	void setSugg(Isugg *);
	void setPhrase(Dlist<Ielt> *);
	void setArgs(Dlist<Iarg> *);					// 11/20/98 AM.
	void setPres(Dlist<Ipre> *);
	void setPosts(Dlist<Ipost> *);
	void setRulelt(Ielt *);
	void setPairs(Dlist<Ipair> *);
	void setActions(Dlist<Iaction> *);			// 11/23/98 AM.
	void setArg(Iarg *);								// 11/20/98 AM.
	void setPair(Ipair *);
	void setAction(Iaction *);						// 11/23/98 AM.
	void setPre(Ipre *);								// 11/29/98 AM.
	void setName(_TCHAR *);
	void setNum(_TCHAR *);								// 11/21/98 AM.
	void setLong(long long);								// 11/10/99 AM. !
	void setFloat(float);							// 08/16/01 AM.
	void setOstream(std::_t_ostream *);					// 12/31/99 AM.

	// NLP++ Semantics.								// 11/09/99 AM.
	void setStmts(Slist<Istmt> *);				// 11/11/99 AM.
	void setStmt(Istmt *);							// 11/11/99 AM.
	void setExpr(Iexpr *);							// 11/09/99 AM.
	void setVar(Ivar *);								// 11/09/99 AM.
	void setOp(Iop *);								// 11/09/99 AM.
	void setFunc(Ifunc *);							// 12/20/01 AM.

	// KB Handshake.									// 02/22/00 AM.
	void setKBconcept(CONCEPT *);					// 02/22/00 AM.
	void setKBphrase(PHRASE *);					// 02/22/00 AM.
	void setKBattr(ATTR *);							// 02/22/00 AM.
	void setKBval(VAL *);							// 02/22/00 AM.

	// Parse tree objects.							// 10/18/00 AM.
	void setPnode(Ipnode *);						// 10/18/00 AM.
	void setNode(Node<Pn> *);						// 10/18/00 AM.


	bool eval(Nlppp *nlppp, /*UP*/ RFASem* &val);					// 11/14/99 AM.

	bool genEval(Gen *,bool = false);									// 05/06/00 AM.
	bool truth();																// 05/26/00 AM.
	std::_t_ostream *out(std::_t_ostream *ostr);											// 05/26/00 AM.

	// Use this to fill Sem object from parse tree and rule match data.
	virtual void Fill();
	static bool same(RFASem *sem1, RFASem *sem2);	// 11/09/99 AM.

	// Conversions!															// 04/28/01 AM.
	long long sem_to_long(bool &);												// 04/28/01 AM.
	float sem_to_float(bool&);												// 08/18/01 AM.
	Node<Pn> *sem_to_node();												// 04/28/01 AM.
	_TCHAR *sem_to_str();														// 04/28/01 AM.

	long long sem_set_long(long long,bool&);						            // 07/05/06 AM.

protected:					// Accessible by derived classes.

private:
	enum RFASemtype type_;
	union Semval
		{
		// KEEP THIS FIRST.
		void *ptr_;

		// Rules file structure.
		// File-wide actions, declares, etc.
		// Pre-post-rules region structure.
		Ifile *rulesfile_;				// 11/24/98 AM.
		Slist<Irecurse> *recurses_;	// 11/24/98 AM.
		Irecurse *recurse_;				// 11/24/98 AM.
		Slist<Iregion> *regions_;		// 11/24/98 AM.
		Iregion *region_;					// 11/24/98 AM.

		Dlist<Irule> *rules_;
		Irule *rule_;
		Isugg *sugg_;
		Dlist<Ielt> *phrase_;
		Dlist<Iarg> *args_;				// 11/20/98 AM.
		Dlist<Ipre> *pres_;
		Dlist<Ipost> *posts_;
		Ielt *rulelt_;
		Dlist<Ipair> *pairs_;
		Dlist<Iaction> *actions_;		// 11/23/98 AM.
		Iarg *arg_;							// 11/20/98 AM.
		Ipair *pair_;
		Iaction *action_;					// 11/23/98 AM.
		Ipre *pre_;							// 11/29/98 AM.

		// NLP++ Semantics.				// 11/09/99 AM.
		Slist<Istmt> *stmts_;			// 11/11/99 AM.
		Istmt *stmt_;						// 11/11/99 AM.
		Iexpr *expr_;						// 11/09/99 AM.
		Ivar *var_;							// 11/09/99 AM.
		Iop  *op_;							// 11/09/99 AM.

		Ifunc *func_;						// 12/19/01 AM.

		// KB OBJECTS.						// 02/22/00 AM.
		CONCEPT *kbconcept_;
		CG *cg_;
		PHRASE *kbphrase_;
		ATTR *kbattr_;
		VAL *kbval_;

		// Parse tree objects.			// 10/18/00 AM.
		Ipnode *pnode_;					// 10/18/00 AM.
		Node<Pn> *node_;					// 10/18/00 AM.



		// 11/10/98 AM.
		// This is the name to use for the associated node.  It may differ
		// from the text or the rule element name.  For example, we might
		// have a _LIT node that came from "\<".  The correct name should be
		// "<".
		// 11/21/98 AM. Collecting most atomics as strings, for simplicity.
		// (strings, numbers, whatever.)
		_TCHAR *name_;

		// 11/10/99 AM. (one year later!)
		// Want a way to store numbers as numbers!
		// Naming this way to ease into this....
		long long long_;															// 11/10/99 AM.

		// FLOAT DATA TYPE.												// 08/16/01 AM.
		float float_;														// 08/16/01 AM.

		// 12/31/99 AM. For output operator in NLP++.
		std::_t_ostream *ostr_;													// 12/31/99 AM.

		// 11/15/99 AM.
		// For handling multiple values, eg, as stored in code variables.
		// Use RSARGS!

		} val_;
};


#endif
