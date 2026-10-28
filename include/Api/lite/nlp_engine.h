#ifndef LINUX
#include <windows.h>
#include <fstream>
#include <tchar.h>
#include <my_tchar.h>
#include <streamclasses.h>
#else
#include <sys/stat.h>
#include "machine.h"
#endif
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strstream>
#include <time.h>
#include <vector>

#include "machine.h"

#ifndef LINUX
#include <tchar.h>
#include <my_tchar.h>
#include <streamclasses.h>
#endif

#include "lite/lite.h"
#include "prim/libprim.h"                                      // 08/14/02 AM.
#include "prim/list_s.h"                                       // 08/14/02 AM.
#include "prim/list.h"                                         // 08/14/02 AM.
#include "lite/mach.h"
#include "lite/global.h"
#include "lite/inline.h"
#include "lite/nlp.h"    // ANALYZER API.
#include "lite/slist.h"                                        // 01/06/03 AM.
#include "lite/vtrun.h"                                        // 08/27/02 AM.
#include "consh/libconsh.h"
#include "consh/cg.h"   // KNOWLEDGE BASE API.
#include "lite/dir.h"
#include "lite/rug.h"   // RULE GENERATION API.
#include "lite/Aseq.h"

/********************************************
* CLASS:  NLP ENGINE
* CR:	  09/14/20 Dd.
* SUBJ:	  API Class for running the NLP engine
********************************************/

// String arguments: every _TCHAR* passed to this API is borrowed for the
// duration of the call. The engine copies what it needs into its own bounded
// buffers and keeps no pointer to the caller's string, so the caller owns it
// and may free or reuse it as soon as the call returns. A path too long for
// those buffers makes the call fail (init() returns -1) rather than truncate.
class LITE_API NLP_ENGINE
{

public:
	NLP_ENGINE(
        std::string workingFolder,
        bool silent=false
 	);

	// -DICTSOURCE (#432): dictionary entries record their file and line on
	// the nodes dicttok builds. Set before the analyzer is loaded.
	static void setDictSource(bool on);

	~NLP_ENGINE();

    _TCHAR *m_analyzer;
    _TCHAR *m_input;
    _TCHAR *m_output;
    _TCHAR *m_sequence;

    _TCHAR m_workingFolder[MAXPATH];
    _TCHAR m_anadir[MAXPATH*2];
    _TCHAR m_ananame[MAXPATH];
    _TCHAR m_rfbdir[MAXPATH*2];
    _TCHAR m_logfile[MAXPATH];	
    _TCHAR m_specdir[MAXPATH*3];
    _TCHAR m_infile[MAXPATH*3];
    _TCHAR m_outdir[MAXPATH*3];
    _TCHAR m_outfile[MAXPATH*3];
    _TCHAR m_seqfile[MAXPATH*4];

    std::vector<std::filesystem::path> m_files;

    VTRun *m_vtrun;
    NLP *m_nlp;
    CG *m_cg;

    #ifdef TEST_RUG_
    CONCEPT *m_gram;
    RUG *m_rug
    #endif

    bool m_develop;
    bool m_silent;
    bool m_compile;
    bool m_compiled;
    bool m_compileKB;
    bool m_compileAna;

    // Folder holding analyzers/ (a bare analyzer name resolves there).
    void setWorkingFolder(_TCHAR *workingFolder);
    void zeroInit();
    void zeroAna(); // [DEGLOB]	// 10/15/20 AM.

    int init(
        _TCHAR *analyzer = NULL,
        bool develop=false,
        bool silent=false,
        bool compile=false,
        bool compiled=false,
        bool compileKB=false,
        bool compileAna=false
    );

    int analyze(
        _TCHAR *analyzer,
        _TCHAR *infile,
        _TCHAR *outfile,
        bool develop=false,
        bool silent=false,
        bool compile=false,
        bool compiled=false,
        bool compileKB=false,
        bool compileAna=false
    );

    int analyze(
        _TCHAR *analyzer,
        _TCHAR *inbuf,
        long len,
        _TCHAR *outbuf,
        long outlen,
        bool develop=false,
        bool silent=false,
        bool compile=false,
        bool compiled=false,
        bool compileKB=false,
        bool compileAna=false
    );

    int analyze(
        _TCHAR *analyzer,
        std::istringstream *iss,
        std::ostringstream *oss,
        bool develop=false,
        bool silent=false,
        bool compile=false,
        bool compiled=false,
        bool compileKB=false,
        bool compileAna=false
    );

    // Load once, then analyze one input file per line of `in` until a blank
    // line or EOF. KB state carries over between inputs as in a folder run.
    int analyzeStream(
        _TCHAR *analyzer,
        std::istream &in,
        _TCHAR *outdir=NULL,
        bool develop=false,
        bool silent=false,
        bool compiled=false
    );

    int close();
    int close(_TCHAR *analyzer);

    // A string every later analysis sees as G("$datum"), eg the output
    // format the caller wants. Empty (the default) means none.
    void setDatum(const _TCHAR *datum);
    const _TCHAR *getDatum();

    // A time limit on each text's analysis, in milliseconds; 0 (the
    // default) is none. A run past it is stopped -- at its next while turn
    // or pass -- instead of looping until something kills the process, and
    // lastRunStopped() is true after the analyze() call that stopped one.
    void setRunLimit(long long ms);
    long long getRunLimit();
    bool lastRunStopped();

    // Zone mode: run only these zones of each input, eg "40-45" or
    // "3,40-45" (zone numbers from 1, as in z040-ana###.tree). The others
    // get only the passes before the zone line. Empty or 0 is every zone.
    // False, and no change, if the spec is not that form.
    bool setZoneRange(const _TCHAR *spec);
    const _TCHAR *getZoneRange();

private:
    void analyzeFile(const std::string &file, _TCHAR *outdir, std::_t_ofstream *fp);
    int createDir(_TCHAR *dirPath);
    int clearDirFiles(_TCHAR *dirPath);
    int readFiles(_TCHAR *directoryPath);
    // Canonical registry name for an analyzer (its path basename). init() and
    // close() must key the VTRun analyzer registry on this so the same analyzer
    // passed as different path-forms maps to one entry -- see #658 / #380.
    void anaRegName(const _TCHAR *path, _TCHAR *out);
    _TCHAR *datumArg();
    void armRunLimit();

    // At the end of the class, so its layout is unchanged before it.
    std::basic_string<_TCHAR> m_datum;    // G("$datum") for analyses.
    long long m_runLimitMs = 0;           // setRunLimit; 0 = none.
    std::basic_string<_TCHAR> m_zoneRange; // setZoneRange; empty = every zone.
};