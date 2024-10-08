/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	FN.H
* FILE:	lite/fn.h
* CR:		01/14/00 AM.
* SUBJ:	NLP++ functions.
*
*******************************************************************************/

#ifndef FN_H_
#define FN_H_
//#include <iostream.h>
#include <assert.h>
#include "lite/lite.h"
#ifdef _ODBC
#include "sql.h"				// ODBC	// 05/29/02 Dd.
#include "sqlext.h"			// ODBC	// 05/23/02 AM.
#endif

// Readability definitions.
typedef void FN;

// Forward reference.
class RFASem;
class Nlppp;																// 02/22/00 AM.
class CG;																	// 02/22/00 AM.

/********************************************
* CLASS:	FN
* CR:		01/14/00 AM.
* SUBJ:	Modularity class for NLP++ generation.
********************************************/

class LITE_API Fn
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Fn();									// Default constructor.
	~Fn();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static bool fnCall(													// 01/14/00 AM.
		Iaction *action,
		Nlppp *nlppp,														// 02/22/00 AM.
		/*UP*/
		bool &badname,		// If unknown independent function name.
		RFASem* &sem		// Function return value.
		);

	static bool userFn(														// 02/09/01 AM.
		_TCHAR *func,
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem,			// Returned sem object.
		bool &fnret,			// Return of called function.
		bool &badname			// If no matching function found.
		);

	static bool fnFactorial(											// 01/14/00 AM.
		Delt<Iarg> *args,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFindroot(												// 02/22/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFindconcept(											// 02/23/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFindattr(												// 02/23/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFindattrs(											// 02/23/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAttrname(												// 02/23/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAttrvals(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAttrtype(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFindvals(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnNumval(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFltval(												// 12/27/06 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnStrval(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnConval(												// 08/12/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnInheritval(											// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnConceptname(											// 02/22/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnConceptpath(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnWordpath(													// 03/06/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFindwordpath(											// 05/30/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnWordindex(												// 05/31/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem		// Function return value.
		);
	static bool fnPathconcept(												// 04/25/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFindhierconcept(									// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAttrexists(											// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAttrwithval(											// 05/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAttrchange(											// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnDown(													// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnUp(														// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnNext(													// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPrev(													// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnNextattr(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnNextval(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnGetsval(												// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnGetstrval(											// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnGetfltval(
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnGetnumval(											// 03/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnGetconval(											// 08/12/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnMakeconcept(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddconcept(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddattr(												// 02/22/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddsval(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddstrval(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddnumval(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddconval(											// 08/11/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnGetconcept(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmconcept(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmchild(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmvals(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmval(													// 05/11/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmattr(													// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmattrval(												// 05/17/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmchildren(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPrunephrases(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmword(													// 07/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool fnReplaceval(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRenameconcept(										// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRenamechild(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRenameattr(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool fnMovecleft(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnMovecright(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool fnFindphrase(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSortphrase(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSortchilds(                                // 02/20/09 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSorthier(                                  // 02/20/09 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPhraselength(										// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnNodeconcept(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnNodeowner(Delt<Iarg>*,Nlppp*,RFASem*&);		// 12/03/02 AM.
	static bool fnFindnode(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnListnode(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFirstnode(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnLastnode(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnMakephrase(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddcnode(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnAddnode(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmnode(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmphrase(												// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmcphrase(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRenamenode(											// 03/03/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
  static bool fnStrcontains(											// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrcontainsnocase(									// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrequal(												// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrnotequal(											// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrequalnocase(										// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrnotequalnocase(									// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrgreaterthan(										// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrlessthan(											// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnDictfirst(Delt<Iarg>*,Nlppp*,RFASem*&);		// 03/21/03 AM.
  static bool fnDictnext(Delt<Iarg>*,Nlppp*,RFASem*&);		// 03/21/03 AM.
  static bool fnDictfindword(											// 04/18/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
	static bool fnDictgetword(Delt<Iarg>*,Nlppp*,RFASem*&);	// 06/29/03 AM.
  static bool fnStrtolower(											// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrtoupper(											// 04/25/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnAddword(												// 05/09/00 AM.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrlength(												// 05/05/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrpiece(												// 05/05/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrchar(													// 05/05/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrisalpha(												// 05/05/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrisdigit(												// 05/05/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
	static bool fnStrhaspunct(
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrclean(													// 11/09/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrtrim(													// 11/09/00 Dd.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
static bool fnStruniquechars(
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrchr(														// 11/18/00 AM.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrchrcount(Delt<Iarg>*,Nlppp*,RFASem*&);		// 12/15/02 AM.
  static bool fnStrrchr(													// 11/18/00 AM.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrstartswith(												// 08/05/21 DD.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnStrendswith(												// 11/20/00 DD.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
  static bool fnTake(														// 05/09/00 AM.
	  Delt<Iarg> *args,
	  Nlppp *nlppp,
	  /*UP*/
	  RFASem* &sem
	  );
	static bool fnSucceed(													// 06/10/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFail(														// 06/10/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSystem(													// 06/20/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnStr(														// 07/26/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnNum(														// 07/26/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnFlt(Delt<Iarg>*,Nlppp*,RFASem*&);					// 08/22/01 AM.
	static bool fnStrtotitle(												// 08/21/00 DD.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
  static bool fnStrwrap(													// 09/22/00 DD.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
  static bool fnStrsubst(													// 09/22/00 DD.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnOpenfile(													// 09/23/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool fnClosefile(Delt<Iarg>*,Nlppp*,/*UP*/RFASem*&);	// 05/21/01 AM.

	static bool fnStrspellcompare(										// 09/26/00 DD.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnStrspellcandidate(										// 09/26/00 DD.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSpellcandidates(										// 09/27/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSpellword(												// 09/27/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSpellcorrect(											// 09/27/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnLj(															// 10/02/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRightjustifynum(										// 09/30/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPercentstr(												// 10/01/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnHitconf(													// 10/08/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnBatchstart(												// 10/19/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnTopdir(													// 10/20/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnUnpackdirs(												// 10/20/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSplit(Delt<Iarg>*,Nlppp*,/*UP*/RFASem*&);		// 03/14/03 AM.
	static bool fnArraylength(												// 10/20/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnRmattrs(													// 10/31/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	// NLP++ PARSE TREE FNS!
	static bool fnPnvar(														// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnsingletdown(											// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnname(													// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnroot(													// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPndown(													// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnup(														// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnnext(													// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnprev(													// 10/18/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnrename(													// 11/21/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPndeletechilds(											// 11/25/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnVarinlist(												// 11/21/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnGroup(														// 11/23/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnSplice(Delt<Iarg>*,Nlppp*,RFASem*&);				// 07/19/02 AM.
	static bool fnPhrasetext(												// 11/20/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPhraseraw(												// 11/20/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnLasteltnode(												// 11/25/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnEltnode(                                      // 09/12/06 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool fnDebug(														// 12/07/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnExitpass(													// 02/06/01 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnToday(														// 02/08/01 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPncopyvars(												// 05/16/01 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool fnPnmakevar(Delt<Iarg>*,Nlppp*,/*UP*/RFASem*&);		// 06/26/01 AM.
	static bool fnPnremoveval(Delt<Iarg>*,Nlppp*,/*UP*/RFASem*&);
	static bool fnPnreplaceval(Delt<Iarg>*,Nlppp*,/*UP*/RFASem*&);	// 06/27/01 AM.
	static bool fnPnpushval(Delt<Iarg>*,Nlppp*,/*UP*/RFASem*&);		// 12/12/14 AM.
	static bool fnPnrpushval(Delt<Iarg>*,Nlppp*,/*UP*/RFASem*&);	// 12/12/14 AM.

	static bool fnStrescape(Delt<Iarg>*,Nlppp*,RFASem*&);			// 07/24/01 DD.
	static bool fnStrunescape(Delt<Iarg>*,Nlppp*,RFASem*&);			// 07/24/01 DD.
	static bool fnKbdumptree(Delt<Iarg>*,Nlppp*,RFASem*&);			// 08/06/01 AM.
	static bool fnWriteKB(Delt<Iarg>*,Nlppp*,RFASem*&);
	static bool fnStriscaps(Delt<Iarg>*,Nlppp*,RFASem*&);
	static bool fnStrisupper(Delt<Iarg>*,Nlppp*,RFASem*&);			// 11/20/01 AM.
	static bool fnStrislower(Delt<Iarg>*,Nlppp*,RFASem*&);			// 11/20/01 AM.
	static bool fnSuffix(Delt<Iarg>*,Nlppp*,RFASem*&);				// 11/21/01 AM.
	static bool fnMkdir(Delt<Iarg>*,Nlppp*,RFASem*&);				// 12/06/01 AM.
	static bool fnStem(Delt<Iarg>*,Nlppp*,RFASem*&);				// 12/28/01 AM.
	static bool fnCout(Delt<Iarg>*,Nlppp*,RFASem*&);				// 05/05/02 AM.
	static bool fnCbuf(Delt<Iarg>*,Nlppp*,RFASem*&);				// 05/11/02 AM.
	static bool fnCoutreset(Delt<Iarg>*,Nlppp*,RFASem*&);			// 05/06/02 AM.
	static bool fnInteractive(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/06/02 AM.
	static bool fnPnvarnames(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/13/02 AM.
	static bool fnPnvartype(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/13/02 AM.
	static bool fnPnsetfired(Delt<Iarg>*,Nlppp*,RFASem*&);

	static bool fnDbopen(Delt<Iarg>*,Nlppp*,RFASem*&);				// 05/23/02 AM.
	static bool fnDbclose(Delt<Iarg>*,Nlppp*,RFASem*&);			// 05/23/02 AM.
	static bool fnDbexec(Delt<Iarg>*,Nlppp*,RFASem*&);				// 05/23/02 AM.
	static bool fnDballocstmt(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/25/02 AM.
	static bool fnDbbindcol(Delt<Iarg>*,Nlppp*,RFASem*&);			// 05/26/02 AM.
	static bool fnDbexecstmt(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/25/02 AM.
	static bool fnDbfetch(Delt<Iarg>*,Nlppp*,RFASem*&);			// 05/26/02 AM.
	static bool fnDbfreestmt(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/25/02 AM.

	static bool fnExittopopup(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/24/02 AM.
	static bool fnGetpopupdata(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/25/02 AM.
	static bool fnSortconsbyattr(Delt<Iarg>*,Nlppp*,RFASem*&);	// 12/02/02 AM.
	static bool fnInputrange(Delt<Iarg>*,Nlppp*,RFASem*&);		// 12/15/02 AM.
	static bool fnInputrangetofile(Delt<Iarg>*,Nlppp*,RFASem*&);// 05/29/03 AM.

	static bool fnFindana(Delt<Iarg>*,Nlppp*,RFASem*&);			// 02/03/03 AM.

	static bool fnUrltofile(Delt<Iarg>*,Nlppp*,RFASem*&);			// 02/11/03 AM.
	static bool fnUrlbase(Delt<Iarg>*,Nlppp*,RFASem*&);			// 03/16/03 AM.

	static bool fnXmlstr(Delt<Iarg>*,Nlppp*,RFASem*&);				// 05/10/03 AM.
	static bool fnResolveurl(Delt<Iarg>*,Nlppp*,RFASem*&);		// 05/14/03 AM.
	static bool fnSqlstr(Delt<Iarg>*,Nlppp*,RFASem*&);				// 07/22/03 AM.
	static bool fnDeaccent(Delt<Iarg>*,Nlppp*,RFASem*&);			// 09/07/03 AM.
	static bool fnDejunk(Delt<Iarg>*,Nlppp*,RFASem*&);          // 09/09/11 AM.
	static bool fnLextagger(Delt<Iarg>*,Nlppp*,RFASem*&);			// 12/11/03 AM.
	static bool fnSetlookahead(Delt<Iarg>*,Nlppp*,RFASem*&);		// 12/14/03 AM.
	static bool fnLevenshtein(Delt<Iarg>*,Nlppp*,RFASem*&);		// 03/06/07 AM.
	static bool fnPninsert(Delt<Iarg>*,Nlppp*,RFASem*&);        // 07/14/08 AM.
	static bool fnPnmove(Delt<Iarg>*,Nlppp*,RFASem*&);          // 10/06/13 AM.

	static bool fnPermuten(Delt<Iarg>*,Nlppp*,RFASem*&);			// 03/10/04 AM.
	static bool fnLogten(Delt<Iarg>*,Nlppp*,RFASem*&);          // 04/29/04 AM.

#ifdef WORDNET
	// WordNet Functions
	static bool fnWNInit(Delt<Iarg>*,Nlppp*,RFASem*&);          // 07/07/04 Dd.
	static bool fnWNSensesToConcept(Delt<Iarg>*,Nlppp*,RFASem*&);          // 07/07/04 Dd.
	static bool fnWNHypnymsToConcept(Delt<Iarg>*,Nlppp*,RFASem*&);          // 07/07/04 Dd.
	static bool fnAdoptConcepts(Delt<Iarg>*,Nlppp*,RFASem*&);          // 07/07/04 Dd.
	static bool fnMakeParentConcept(Delt<Iarg>*,Nlppp*,RFASem*&);          // 07/07/04 Dd.
#endif

	// Utility function.
	static Dlist<Iarg> *unpackdirs(_TCHAR *, Parse *);				// 10/20/00 AM.
	static Dlist<Iarg> *split_util(_TCHAR*,_TCHAR*,Parse*);			// 03/14/03 AM.

#ifdef _ODBC
	static SQLSMALLINT SeqlTypeFromString(_TCHAR *typeStr);			// 05/29/02 DD.
	static bool SeqlValidType(_TCHAR *typeStr);							// 05/29/02 DD.

	static bool dbbind2str(Nlppp*,RFASem*,SQLSMALLINT,
												long,long,RFASem*);			// 06/08/02 AM.
	static bool dbbind2long(Nlppp*,RFASem*,SQLSMALLINT,
												long,RFASem*);					// 06/08/02 AM.
	static bool dbbind2float(Nlppp*,RFASem*,SQLSMALLINT,
												long,RFASem*);					// 06/08/02 AM.
	static long *bindvar(Nlppp*,RFASem*);								// 06/12/02 AM.
#endif

	static bool prosify(													// 10/01/10 AM.
		Node<Pn> *node,
		bool root,				// If this is the top-level node of subtree.
		_TCHAR *buf,				// Buffer to fill.
		_TCHAR* &ptr,				// First empty spot in buffer.
		long &siz				// Empty space left in buffer.
		);

	static bool fnDICTphraselookup( // 12/15/14 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool DICTphraselookup_match( // 12/16/14 AM.
		Node<Pn> *nn,	// Overall start node for current walk.
		Node<Pn> *xx,	// Matched parse tree node.
		CONCEPT *cc,		// Matched kb phrase concept.
		_TCHAR *match_str, // Concept attribute that denotes a "MATCH".
		_TCHAR *list_str,	// Node attribute for gathering match values.
		CG *cg,
		Htab *ht         // Current hash table
		);

protected:

};



#endif
/*************************      END OF FILE      ******************************/
