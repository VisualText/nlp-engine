/*******************************************************************************
Copyright � 2002-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:  VTRUN.H
* FILE:  include/API/lite/vtrun.h
* CR:    08/27/02 AM.
*
* SUBJ:  CLASS TO MANAGE VISUALTEXT RUNTIME
*
* NOTE:  Support for multiple and embedded analyzers.
*
* API:   User should create only one instance of this class, prior to loading
*        any analyzer instances (i.e., creating NLP objects).  Additional
*        instances will be ignored.
*
*******************************************************************************/

#ifndef VTRUN_H_
#define VTRUN_H_

typedef void NLPS;	// 07/07/03 AM.

// Forward reference.
class NLP;	// 01/06/03 AM.
class CG;	// 07/18/03 AM.


/********************************************
* CLASS: VTRUN
* CR:    08/27/02 AM.
* SUBJ:  API Class for the VisualText Runtime System.
* NOTE:  This class may eventually manage actively loaded analyzer definitions
*        and instances.  For now, it stores global, one-time only data
*        structures.  (An analyzer should be able to use a private htab and
*        stab, when desired, rather than the globally shared ones here.)
* OPT:   Will probably speed up analyzer switching in VisualText, since
*        global structures won't be deleted and recreated (e.g., rfb).
********************************************/

class LITE_API VTRun
{
	friend class Aseq;	// 09/21/20 AM.
	friend class Nlp;	// 09/21/20 AM.
public:
   VTRun(                   // Default constructor.
      _TCHAR *logfile = 0,    // Log file for verbose output.
      _TCHAR *rfbspecdir = 0,
      bool silent = true
		);
   ~VTRun();                // Destructor. 

	void init(_TCHAR*,_TCHAR*,bool);  // Initialize ds.
	void clear();                 // Delete ds.
	void zero();                  // Zero out ds.

	void *getHtab();	// 06/24/03 AM.

   bool isBuiltin(_TCHAR *name);

	// API: Create runtime environmnet.	// 07/19/03 AM.
	static VTRun *makeVTRun(	// 07/19/03 AM.
      _TCHAR *logfile = 0,    // Log file for verbose output.
      _TCHAR *rfbspecdir = 0,
      bool silent = true
		);

	// API: Delete runtime environment.	// 07/19/03 AM.
	static bool deleteVTRun(VTRun*);	// 07/19/03 AM

	// API: Register analyzer INSTANCE.
	bool addAna(NLP*);		// 01/06/03 AM.

	// API: Unregister analyzer INSTANCE.
	// Does NOT remove NLP object.
	bool rmAna(NLP*);			// 01/06/03 AM.

	// Delete all the analyzers from VTRUN.	// 09/28/20 AM.
	bool deleteAnas();						// 09/28/20 AM.

	// API: Find registered analyzer INSTANCE by name.
	NLP *findAna(_TCHAR*);		// 01/06/03 AM.

	// API: Create and load a knowledge base.
	CG *makeCG(         // 07/18/03 AM.
		_TCHAR *appdir,    // Folder housing the kb folder.
		bool compiled,   // If loading the compiled kb.
		NLP *nlp         // Associated analyzer, if any.
		);

	// API: Delete a knowledge base.
	// (And remove references to it from analyzers.)
	bool deleteCG(CG*);		// 07/18/03 AM.

	// API: Create an analyzer INSTANCE.
	// (Defaults as in NLP object creation.)
	NLP *makeNLP(        // 07/18/03 AM.
		_TCHAR *appdir,     // Analyzer app directory.
		_TCHAR *name,       // Unique name for analyzer.
		bool develop=false,
		bool silent=false,
		bool compiled=false
		);

	// API: Delete analyzer INSTANCE.
	bool deleteNLP(NLP*); 	// 07/18/03 AM.

   // API: Get KB root concept.
	static CONCEPT *getKBroot(CG*); // 12/19/03 AM.

	// Internal use only.
	static void *create_rfa(void*,_TCHAR*,NLP*);
	static void *create_rfb(_TCHAR*,_TCHAR*,bool,bool,void*,void*,NLP*);

//////////////////////////////
// DATA (keeping these public)
//////////////////////////////

   void *rfa_;          // NLP++ bootstrap analyzer ("rulefile analyzer").
   void *rfb_;          // NLP++ full analyzer.
   void *rfc_;          // NLP++ compiled analyzer. [OPT: unimplemented]

	// Dummy environments for rfa,rfb,rfc.
	NLP *nlpa_;				// 06/30/03 AM.
	NLP *nlpb_;				// 06/30/03 AM.
	NLP *nlpc_;				// 06/30/03 AM.

   void *htab_;         // The hash table.
   void *stab_;         // The string table.

	void *htfunc_;			// Hash table for NLP++ builtin functions.

   void *alist_;        // List manager (used by KBs only).

	// Free list for parse tree nodes.  To improve analyzer speed
	// and reduce crashes on referencing an excised node.			// 06/18/10 AM.
	void *pnodefreelist_;	// Free list of parse tree nodes.	// 06/18/10 AM.

   _TCHAR *logfile_;      // Logging verbose messages.	// 10/29/02 AM.

	// Managing list of analyzers in memory.							// 01/06/03 AM.
//	Slist<NLP> *anas_;														// 01/06/03 AM.
	NLPS *anas_;																// 07/07/03 AM.

	// The global VTRun::count_ variable now part of class.	// [DEGLOB]	// 10/15/20 AM.
	int anacount_;	// [DEGLOB]	// 10/15/20 AM.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif

};

//extern LITE_API VTRun *VTRun_Ptr;	// 09/21/20 AM.	// [DEGLOB]	// 10/15/20 AM.

#endif
/*************************      END OF FILE      ******************************/
