/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:   NLP.H
* FILE:   include/API/lite/nlp.h
* CR:     10/19/98 AM.
*
* SUBJ:   PRIMARY API FILE FOR RUNNING VISUALTEXT ANALYZERS.
*
* NOTE:   NLP class defines an ENVIRONMENT for a text analyzer.
*
* API:    THE MAIN API FUNCTIONS ARE: (See templates below for args).
*
*         API CALLS TO SET UP THE TEXT ANALYZER:
*
*          NLP               = Create the NLP object to manage analyzer.
*          make_analyzer     = Create and load the text analyzer.
*
*         API CALL FOR NORMAL RUNNING OF THE ANALYZER ON A TEXT:
*
*          analyze           = Run analyzer on a single text.
*
*
*         API CALLS FOR STEPPING THROUGH ONE PASS AT A TIME:
*   		 (Note: These are used by VisualText, for example)
*
*          apiInitAnalyzer   = Initialize stepwise interp analysis of a text.
*          apiStepAnalyzer   = Execute next pass in the analysis of a text.
*          apiCleanAnalyzer  = Clean up the stepwise analysis of a text.
*
*         INTERNAL CALLS (NOT part of API)
*
*         The ones below are used internally by analyze()
*            initAnalyze     = Initialize analysis of a single text.
*            runAnalyzer     = Run compiled analyzer on a single text.
*            cleanAnalyze    = Clean up analysis of a single text.
*
*
*******************************************************************************/

#include <string>
#include <iostream>
#include <strstream>
#include <sstream>

#ifndef NLP_H_
#define NLP_H_
#include <assert.h>

#include "Aseq.h"			// 05/17/99 AM.
#include "lite.h"
#include "global.h"		// 02/05/00 AM. (for FNAME_SIZE).
#include "vtrun.h"		// [DEGLOB]	// 10/14/20 AM.

LITE_API void object_counts(
	std::_t_ofstream* =0	// 07/18/03 AM.
	);

// Forward references
class Ana;
class Htab;
class Stab;
class Parse;				// 05/13/99 AM.
//class Eana;				// 10/13/99 AM.
class GUI;					// 12/09/99 AM.
class CG;					// 02/15/00 AM.
class Eana;					// 04/04/00 AM.
class Nlppp;				// 06/25/03 AM.
class Tok;					// 06/25/03 AM.

/********************************************
* CLASS:	NLP
* CR:		12/14/98 AM.
* SUBJ:	API Class for the NLP Lite library.
********************************************/

class LITE_API NLP
{
	//friend ostream &operator<<(ostream &, Var &);
	friend class Parse;				// 06/25/03 AM.
	friend class Nlppp;				// 06/25/03 AM.
	friend class Tok;				// 06/25/03 AM.

public:
	NLP(										// Default constructor.
		_TCHAR *appdir = 0,     // ANALYZER APPLICATION TOP DIR.
		bool develop = false,   // INTERMEDIATE FILES.             // 12/25/98 AM.
		bool silent = false,    // LOGGING/DEBUG OUTPUT.           // 06/16/02 AM.
		bool compiled = false,  // RUN COMPILED ANALYZER (run.dll) // 07/05/00 AM.
		_TCHAR *outdir = 0,     // DIRECTORY FOR OUTPUT.			  // 03/10/99 AM.
//		char *datadir = 0			// VISUALTEXT DATA DIRECTORY		  // 12/08/99 AM.
		_TCHAR *name = 0,       // UNIQUE NAME FOR ANALYZER.       // 01/06/03 AM.
		_TCHAR *logfile = 0,      // VTLOG	// 05/06/13 AM.
		VTRun *vtrun = 0			// VT Runtime Environment.	// [DEGLOB]	// 10/15/20 AM.
		);
	NLP(bool);                 // Dummy constructor.              // 06/30/03 AM.
	~NLP();									// Destructor.

	// Access
	void *getAna();							// 01/13/99 AM.
//	void *getRFA();							// 01/13/99 AM.
//	void *getRFB();							// 11/05/99 AM.
	void *getHtab();							// 01/18/99 AM.
	void *getStab();							// 05/26/02 AM.
//	void *getHtfunc();						// 12/20/01 AM.
	VTRun *getVTRun();	// [DEGLOB]	// 10/15/20 AM.
#ifndef LINUX
	HINSTANCE getHdll();						// 01/29/99 AM.
	HINSTANCE getHrundll();					// 05/14/00 AM.
#endif
//	char *getDatadir();						// 12/08/99 AM.
	CG   *getCG();								// 02/15/00 AM.
	bool getFbatchstart();					// 10/19/00 AM.
	bool getFinteractive();					// 05/06/02 AM.
	_TCHAR *getPopupmsg();						// 05/24/02 AM.
	_TCHAR *getPopuptyp();						// 05/24/02 AM.
	_TCHAR *getPopupdat();						// 05/24/02 AM.
	_TCHAR *getName();							// 01/06/03 AM.
//	bool  getDevelop();                 // 07/18/03 AM.
//	bool  getSilent();                  // 07/18/03 AM.
//	bool  getCompiled();                // 07/18/03 AM.
	bool  getIsDirRun();
	bool  getIsLastFile();
	bool  getIsFirstFile();

	// Modify
//	void	setDatadir(char *);				// 12/08/99 AM.
	void	setCG(CG *);						// 02/15/00 AM.
	void	setHtab(Htab*);					// 07/03/03 AM.
	void	setStab(Stab*);					// 07/03/03 AM.
	void	setFbatchstart(bool);			// 10/19/00 AM.
	void	setFinteractive(bool);			// 05/06/02 AM.
//	void	setHtfunc(void*);					// 12/20/01 AM.
	void	setVTRun(VTRun*);	// [DEGLOB]	// 10/15/20 AM.

	// Internals for analyzer invoking a GUI popup window.
	void	setPopupmsg(_TCHAR*);				// 05/24/02 AM.
	void	setPopuptyp(_TCHAR*);				// 05/24/02 AM.
	void	setPopupdat(_TCHAR*);				// 05/24/02 AM.

	void	setName(_TCHAR*);					// 01/06/03 AM.
//	void	setDevelop(bool);             // 07/18/03 AM.
//	void	setSilent(bool);              // 07/18/03 AM.
//	void	setCompiled(bool);            // 07/18/03 AM.


	// CREATE AND LOAD A TEXT ANALYZER.
	bool make_analyzer(
		_TCHAR *sequence,			// NAME OF ANA SEQUENCE FILE.
		_TCHAR *appdir,				// ANALYZER APPLICATION TOP DIR.
		bool develop,				// INTERMEDIATE FILES.				  // 12/25/98 AM.
		bool silent = false,    // LOGGING/DEBUG OUTPUT.           // 06/16/02 AM.
		_TCHAR *outdir = 0,			// DIRECTORY FOR OUTPUT.			  // 03/10/99 AM.
		bool compile = false,		// COMPILE WHILE LOADING.       // 07/15/02 AM.
		bool compiled = false,	// RUN COMPILED ANALYZER (run.dll) // 07/05/00 AM.
		bool fromkb = false		// GET ANALYZER SEQUENCE FROM KB.  // 11/11/00 AM.
		);

	// RUN ANALYZER ON A SINGLE TEXT OR BUFFER.
	void analyze(
		_TCHAR *input,		// INPUT FILE PATH.
		_TCHAR *output,		// OUTPUT FILE PATH.
		_TCHAR *appdir,		// ANALYZER APPLICATION TOP DIR.
		bool flogfiles,	// OUTPUT INTERMEDIATE LOGS				  // 10/13/99 AM.
		bool silent = false, // LOGGING/DEBUG OUTPUT.              // 06/16/02 AM.
		_TCHAR *outdir = 0,	// DIRECTORY FOR OUTPUT.					  // 03/10/99 AM.
		_TCHAR *inbuf = 0,	// IF ANALYZING A BUFFER, SUPPLY IT HERE // 02/11/00 AM.
		long len = 0,		// LENGTH OF INPUT BUFFER					  // 02/11/00 AM.
		bool compiled=false,	// RUN COMPILED ANALYZER				  // 07/05/00 AM.
		std::_t_ostream *os = 0,	// STANDARD OUTPUT STREAM					  // 05/05/02 AM.
		_TCHAR *outbuf = 0,	// OUTPUT BUFFER TO BIND TO cbuf STREAM  // 05/07/02 AM.
		long outlen = 0,	// MAXIMUM LENGTH OF cbuf OUTPUT BUFFER  // 05/07/02 AM.
		_TCHAR *datum = 0   // PASS INFO TO G("$datum").             // 03/13/03 AM.
		);

	void analyze(
		std::istringstream *iss,	// Input strstream.
		std::ostringstream *oss,	// Output strstream.
		_TCHAR *appdir = 0,		// Directory holding analyzer.
		bool flogfiles = false,	// Changing the meaning of this.
		bool silent = false,	// Silent run mode.
		_TCHAR *outdir = 0,		// Intermed files.
		bool compiled = false,	// If running compiled analyzer.
		std::_t_ostream *os = 0,		// Rebinding of output stream.				// 05/05/02 AM.
		_TCHAR *datum = 0		// Pass info to G("$datum").
		);

	// Create an internalized version of a string, so you don't have to
	// worry about deleting it
	bool internStr(_TCHAR *, /*UP*/ _TCHAR* &); // FIX					// 12/11/99 AM.

	// Read named input file as binary.
	// WARN: User responsible for deleting the returned BUF.		// 08/01/00 AM.
	bool readConvert(															// 07/24/00 AM.
		_TCHAR *fname,
		_TCHAR* &buf,		/*UP*/  // WARN: DELETE WITH deleteBuf().
		long &len,		/*UP*/
		long maxline=MAXSTR
		);

	// Delete a buffer created by the API functions.
	bool deleteBuf(_TCHAR *);													// 08/02/00 AM.

	///////////////////////
	///////////////////////
	// API CALLS.																// 07/23/01 AM.
	// A SET OF CALLS FOR STEPPING THROUGH ONE PASS OF AN ANALYZER AT A TIME.
	// ENABLES STOPPING THE ANALYSIS OF A TEXT IN THE MIDDLE.
	///////////////////////

	// INITIALIZE FOR STEPPING THROUGH INTERPRETED ANALYSIS OF A TEXT.
	void *apiInitAnalyze(													// 07/23/01 AM.
		_TCHAR *input,		// INPUT FILE PATH.
		_TCHAR *output,		// OUTPUT FILE PATH.
		_TCHAR *appdir,		// ANALYZER APPLICATION TOP DIR.
		bool flogfiles,	// OUTPUT INTERMEDIATE LOGS
		_TCHAR *outdir = 0,	// DIRECTORY FOR OUTPUT.
		_TCHAR *inbuf = 0,	// IF ANALYZING A BUFFER, SUPPLY IT HERE
		long len = 0,		// LENGTH OF INPUT BUFFER
		std::_t_ostream *os = 0,	// STANDARD OUTPUT STREAM					  // 05/13/02 AM.
		_TCHAR *outbuf = 0,	// OUTPUT BUFFER TO BIND TO cbuf STREAM  // 05/13/02 AM.
		long outlen = 0	// MAXIMUM LENGTH OF cbuf OUTPUT BUFFER  // 05/13/02 AM.
		);

	// EXECUTE THE NEXT PASS IN ANALYZING CURRENT TEXT.
	// Call repeatedly till it returns false.
	bool apiStepAnalyze(														// 07/24/01 AM.
		void *parse
		);

	// CLEAN UP STEPWISE ANALYSIS OF CURRENT TEXT.
	bool apiCleanAnalyze(													// 07/23/01 AM.
		void *parse
		);

	// Is given name an NLP++ builtin function.						// 12/24/01 AM.
//	bool isBuiltin(char*);													// 08/28/02 AM.

	// Transfer string from GUI popup to analyzer.					// 05/24/02 AM.
	bool apiSetpopupdata(_TCHAR*);											// 05/24/02 AM.


	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	///////////////////////////////
	//   INTERNAL FUNCTIONS
	///////////////////////////////

	// RUN COMPILED ANALYZER ON A SINGLE TEXT OR BUFFER.
	void runAnalyzer(
		_TCHAR *input,		// INPUT FILE PATH.
		_TCHAR *output,		// OUTPUT FILE PATH.
		_TCHAR *appdir,		// ANALYZER APPLICATION TOP DIR.
		bool flogfiles,	// OUTPUT INTERMEDIATE LOGS
		bool silent = false, // LOGGING/DEBUG OUTPUT.              // 07/03/02 AM.
		_TCHAR *outdir = 0,	// DIRECTORY FOR OUTPUT.
		_TCHAR *inbuf = 0,	// IF ANALYZING A BUFFER, SUPPLY IT HERE
		long len = 0,		// LENGTH OF INPUT BUFFER
		std::_t_ostream *os = 0,	// STANDARD OUTPUT STREAM					  // 05/13/02 AM.
//		std::ostringstream * = 0,	// Output buffer stream, if any.         // 10/10/03 AM.
	   void * = 0,       // For .NET compatibility.               // 10/10/03 AM.
		long outlen = 0,	// MAXIMUM LENGTH OF cbuf OUTPUT BUFFER  // 05/13/02 AM.
		_TCHAR *datum = 0   // PASS INFO TO G("$datum").             // 03/13/03 AM.
		);

	void runAnalyzer(
		std::istringstream *iss,
		std::ostringstream *oss,
		_TCHAR *appdir,
		bool flogfiles,			// Changing the meaning of this.
		bool silent = false,	// Silent run mode.
		_TCHAR *outdir = 0,		// Interned files.
		std::_t_ostream *os = 0,		// Rebinding of output stream.
		_TCHAR *datum = 0		// Pass info to G("$datum").
		);


	// Create the RFB (NLP++ compiler/interpreter).
	static                // MAKING THIS STATIC.                  // 08/27/02 AM.
	void *                // CHANGING TYPE.                       // 08/27/02 AM.
//	bool
	     make_rfb(						// 11/05/99 AM.
		_TCHAR *appdir,
		_TCHAR *rfbspecdir,						// 12/08/99 AM.
		bool develop,
		bool silent,								// 06/16/02 AM.
		void*,                                                     // 08/27/02 AM.
		void*                                                      // 08/27/02 AM.
		);

	// INITIALIZE ANALYSIS OF A SINGLE TEXT.
	Parse *initAnalyze(                                              // 05/13/99 AM.
		_TCHAR *input,       // INPUT FILE PATH.
		_TCHAR *output,      // OUTPUT FILE PATH.
		_TCHAR *appdir,	   // ANALYZER APPLICATION TOP DIR.
		Eana *eana,          // FLAGS,MODES FOR ANALYSIS				  // 10/13/99 AM.
		_TCHAR *outdir = 0,  // DIRECTORY FOR OUTPUT.
		_TCHAR *inbuf = 0,   // IF ANALYZING A BUFFER, SUPPLY IT HERE.// 02/06/00 AM.
		long len = 0,        // LENGTH OF INPUT BUFFER.					  // 02/06/00 AM.
		std::_t_ostream * = 0,		// Output stream, if any.                // 05/11/02 AM.
//		std::ostringstream * = 0,    // Output buffer stream, if any.         // 10/10/03 AM.
	   void * = 0,          // For .NET compatibility.               // 10/10/03 AM.
		long = 0,            // Output buffer length, if any.         // 05/11/02 AM.
		bool retain = false  // If retaining hash,sym tables.         // 05/21/09 AM.
		);

	Parse *initAnalyze(
		std::istringstream *iss,
		std::ostringstream *oss,
		_TCHAR *appdir,
		Eana *eana,
		_TCHAR *outdir = 0,
		std::_t_ostream *os = 0,	// Rebinding of output stream.
		bool retain = false		// If retaining hash,sym tables.
		);

	// CLEAN UP ANALYSIS OF A SINGLE TEXT.
	bool cleanAnalyze(														  // 05/13/99 AM.
		Parse *parse,
      bool retain=false // If retaining hash,sym tables.         // 07/03/03 AM.
		);

	// Special functions related to the USER Project.				// 01/23/02 AM.
	// (For internal use by analyzer engine.)							// 01/23/02 AM.
	bool user_ini();															// 01/23/02 AM.
	bool user_fin();															// 01/23/02 AM.

	bool load_compiled(_TCHAR *);			// 09/13/00 AM.

	void fileDbg(_TCHAR *,std::_t_ostream*&);										// 08/26/02 AM.
	void resetDbg(std::_t_ostream*&);												// 08/26/02 AM.
	bool logOut(bool);														// 08/26/02 AM.
	bool logOut(std::_t_strstream*,bool);                            // 02/25/05 AM.
	void setIsDirRun(bool val);
	void setIsLastFile(bool val);
	void setIsFirstFile(bool val);

	////////////////////////////
	// STEPPING THROUGH ANALYSIS			// 05/17/99 AM.
	// (Internal functions, not for API user.)
	////////////////////////////
	bool stepExecute(							// 05/17/99 AM.
		Parse *parse,				// Current parse instance.
		APASS *seq,					// Current step in analyzer sequence.
		int num						// Pass number.
		);
	bool partExecute(					// 05/18/99 AM.
		Parse *parse,				// Current parse instance.
		APASS *seq_s,				// Start pass.
		APASS *seq_e,				// End pass.
		/*DU*/
		int &num						// Start pass number.
		);
	bool internPass(				// 09/01/99 AM.
		APASS *seqn,
		Eana  *erfa					// 10/13/99 AM.
		);

	GUI *gui;																	// 11/29/99 DD.

	///////////////////////
	///////////////////////

	// Class-wide

private:
	void init(_TCHAR *appdir, bool develop,
		bool silent,															// 06/16/02 AM.
		bool compiled,			// Running compiled analyzer.			// 07/03/00 AM.
//		_TCHAR *outdir = _T("")														// 03/10/99 AM.
		_TCHAR *outdir = NULL														// 09/23/20 AM.
		);
	void clean();

	// Reference the runtime environment that this analyzer belongs to.	// [DEGLOB]	// 10/14/20 AM.
	VTRun *vtrun_;				// [DEGLOB]	// 10/14/20 AM.

//	Ana *rfa_;				// The rules-file analyzer.
//	Ana *rfb_;				// The RFA Blaster!							// 11/05/99 AM.
//	Cana *rfc_;				// The RFA Compiled! ....					// 11/05/99 AM.
	Htab *htab_;			// The hash table.
	Stab *stab_;			// The string table.
	Ana *ana_;				// The text analyzer.
	CG *cg_;					// The knowledge base.						// 02/15/00 AM.

#ifndef LINUX
	// USER DLL TO LOAD DYNAMICALLY.										// 01/29/99 AM.
	HINSTANCE hdll_;															// 01/29/99 AM.

	// COMPILED ANALYZER TO LOAD DYNAMICALLY.							// 05/14/00 AM.
	HINSTANCE hrundll_;														// 05/14/00 AM.
#endif

	// If not defined by user, initialize data directory to one
	// defined by the environment variable %TAI%.			// 12/08/99 AM.
	// Houses dict word file and RFB definition, for now.
//	char datadir_[FNAME_SIZE];	// Data dir for analyzer.	// 12/08/99 AM.

	// This flag is used to notify analyzer that the user has just started
	// a batch process.  Set to true when user selects a folder (or
	// hierarchy of files) to process.  Flag is passed to the parse of the
	// first file, and then turns itself off.							// 10/19/00 AM.
	// Used as a handshake between VisualText (ie, API user) and analyzer.
	// (Can also harmlessly be set when user is analyzing an individual
	// file.)
	bool fbatchstart_;

	// This flag is to notify the analyzer whether it's in an
	// interactive environment (e.g., within VisualText).			// 05/06/02 AM.
	// Can help control where the output goes from NLP++, e.g.	// 05/06/02 AM.
	// Default is FALSE.														// 05/06/02 AM.
	bool finteractive_;														// 05/06/02 AM.

	bool isLastFile_;
	bool isFirstFile_;
	bool isDirRun_;

	// Hash table for NLP++ builtin functions.						// 12/20/01 AM.
//	void *htfunc_;																// 12/20/01 AM.

	// Handshakes with GUI for setting up a popup window.			// 05/24/02 AM.
	_TCHAR *popupmsg_;	// Get popup message to GUI.					// 05/24/02 AM.
	_TCHAR *popuptyp_;	// Get popup type to GUI.						// 05/24/02 AM.
	_TCHAR *popupdat_;	// Get popup result from GUI.					// 05/24/02 AM.

	// Debugging and log output analyzer.								// 08/26/02 AM.
	std::_t_ostream *dbgout_;															// 08/26/02 AM.

	// Name for the current analyzer (may want to simply use
	// the analyzer's folder name).  Will enable one analyzer
	// to find another analyzer loaded into memory.)				// 01/06/03 AM.
	// (Note: not allowing multiple instances of same analyzer,
	// currently.)
	_TCHAR name_[MAXSTR];														// 01/06/03 AM.

	// Global logfile from VTRun environment.    // VTLOG	// 05/06/13 AM.
	// Intended for notable errors.              // VTLOG	// 05/06/13 AM.
	_TCHAR *logfile_;                            // VTLOG	// 05/06/13 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);				// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
