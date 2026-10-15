/*******************************************************************************
Open Source
MIT License
********************************************************************************
*
* NAME: NLP Engine
* FILE: engine.cpp
* CR:   09/14/20
* SUBJ: Creates a simple function to the analyzer
*
*******************************************************************************/

#include "nlp_engine.h"

#include <filesystem>
#include <ctime>
#include <cstdarg>
#include <cstdio>

#ifdef LINUX
#include <unistd.h>
#include <dirent.h>
#endif

// Bounded formatting into a fixed-size member or static buffer (#486). The
// paths built here come from caller strings of any length; _stprintf had no
// bound and could overrun MAXPATH buffers. Returns false if the result did not
// fit -- the buffer then holds a truncated, terminated string, which callers
// that build a path treat as an error rather than open the wrong file.
template <size_t N>
static bool bprintf(_TCHAR (&buf)[N], const _TCHAR *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, N, fmt, ap);
    va_end(ap);
    return n >= 0 && (size_t)n < N;
}

////////// NLP_INIT /////////////
// CR:  09/23/20 AM.
// DESC:    Initialize the NLP framework.
// NOTE:    Uses the global variables VTRun_ptr and VTRun_count.
//          Create RFA and RFB analyzers, ie, generate the NLP++ parser.
//          (data/Rfb/spec folder holds definition of NLP++ parser.)
//          Create the list manager.  And whatever else the VTRUN
//          framework manages.
/////////////////////////////////
/////////////////////////////////

////////// NLP_FIN /////////////
// CR:  09/23/20 AM.
// DESC:    Shut down the NLP framework.
// NOTE:    Uses the global variables VTRun_ptr and VTRun_count.
/////////////////////////////////
/////////////////////////////////



NLP_ENGINE::NLP_ENGINE(
    std::string workingFolder,
	bool silent
	)
{
    NLP_ENGINE::zeroInit();    // [DEGLOB]	// 10/15/20 AM.

    static _TCHAR logfile[MAXSTR];
    static _TCHAR rfbdir[MAXSTR];
    if (!workingFolder.empty()) {
        // NLP-ENGINE-521: m_workingFolder is _TCHAR[MAXPATH]; the caller-
        // supplied std::string can be longer (CI tempdir paths,
        // deeply-nested project layouts). Bound the copy.
        _stprintf(m_workingFolder, _T("%.*s"), (int)(MAXPATH - 1),
                  workingFolder.c_str());
        bprintf(logfile,_T("%s%s%s"),workingFolder.c_str(),DIR_STR,_T("vtrun_logfile.out"));
        bprintf(rfbdir,_T("%s%sdata%srfb%sspec"),workingFolder.c_str(),DIR_STR,DIR_STR,DIR_STR);
    } else {
        bprintf(logfile,_T("%s"),_T("vtrun_logfile.out"));
        bprintf(rfbdir,_T("data/rfb/spec"));
    }
    std::_t_cerr << _T("[logfile: ") << logfile << _T("]") << std::endl;
 
    std::_t_cerr << _T("[rfbdir: ") << rfbdir << _T("]") << std::endl;
 
// if (!VTRun_Ptr)  // [DEGLOB]	// 10/15/20 AM.
    {
        m_vtrun = VTRun::makeVTRun(                               // 07/21/03 AM.
            logfile,                // Verbose/error log file.         // 08/28/02 AM.
            rfbdir,                 // VisualText dir for RFB spec.    // 08/28/02 AM.
            silent                      // Build silently.                 // 08/28/02 AM.
            );
    }
}

NLP_ENGINE::~NLP_ENGINE()
{
    close();    // 09/27/20 AM.
}

// Folder that holds analyzers/ and data/rfb/spec; init() resolves a bare
// analyzer name as <folder>/analyzers/<name>. Declared since 2020 but never
// defined, so calling it failed to link (#485). The runtime manager made by
// the constructor keeps the rfb folder it was built with; this changes only
// where later init() calls look for analyzers.			// 10/13/26 DD.
void NLP_ENGINE::setWorkingFolder(_TCHAR *workingFolder)
{
    _TCHAR folder[MAXPATH];
    if (!bprintf(folder, _T("%s"), workingFolder ? workingFolder : _T("")))
        {
        // Keep the old folder: a truncated one would resolve analyzers
        // somewhere else without saying so.
        std::_t_cerr << _T("[setWorkingFolder: path too long, not changed: ")
                     << workingFolder << _T("]") << std::endl;
        return;
        }
    _tcscpy(m_workingFolder, folder);
}

// Onetime init of NLP_ENGINE and VTRun environment.    // [DEGLOB]	// 10/15/20 AM.
void NLP_ENGINE::zeroInit()
{ 
    m_input = 0;
    m_output = 0;
    m_sequence = 0;

    m_workingFolder[0] = '\0';
    m_anadir[0] = '\0';
    m_ananame[0] = '\0';
    m_rfbdir[0] = '\0';
    m_logfile[0] = '\0';	
    m_specdir[0] = '\0';
    m_infile[0] = '\0';
    m_outdir[0] = '\0';
    m_outfile[0] = '\0';
    m_seqfile[0] = '\0';

    m_vtrun = 0;
    m_nlp = 0;
    m_cg = 0;

    m_develop = false;
    m_silent = false;
    m_compile = false;
    m_compiled = false;
    m_compileKB = false;
    m_compileAna = false;

    #ifdef TEST_RUG_
    m_gram = 0;
    m_rug = 0;
    #endif
}


// Reset if calling an analyzer other than the current one.    // [DEGLOB]	// 10/15/20 AM.
void NLP_ENGINE::zeroAna()
{ 
    m_input = 0;
    m_output = 0;
    m_sequence = 0;

    m_anadir[0] = '\0';
    m_ananame[0] = '\0';
    m_rfbdir[0] = '\0';
    m_logfile[0] = '\0';	
    m_specdir[0] = '\0';
    m_infile[0] = '\0';
    m_outdir[0] = '\0';
    m_outfile[0] = '\0';
    m_seqfile[0] = '\0';

 //   m_vtrun = 0;
    m_nlp = 0;
    m_cg = 0;

}

// Canonical registry name for an analyzer: its last path component, accepting
// both DIR_CH and '/'. init() stores this as the NLP name and keys findAna on
// it; close() normalizes its argument the same way. Without a shared rule the
// same analyzer passed as "emailaddress" vs ".../analyzers/emailaddress" maps to
// two registry entries, so a re-entrant analyze() rebuilds from scratch and
// orphans the prior NLP (#658, #380). The MAXPATH-1 precision bound preserves
// the NLP-ENGINE-521 stack-overflow fix.
void NLP_ENGINE::anaRegName(const _TCHAR *path, _TCHAR *out)
{
    if (!path || !*path) { out[0] = '\0'; return; }
    const _TCHAR *ana = _tcsrchr(path, DIR_CH);
#ifndef LINUX
    const _TCHAR *fwd = _tcsrchr(path, '/');
    if (fwd > ana) ana = fwd;
#endif
    if (ana && *(ana + 1) != '\0')
        _stprintf(out, _T("%.*s"), (int)(MAXPATH - 1), ana + 1);
    else
        _stprintf(out, _T("%.*s"), (int)(MAXPATH - 1), path);
}

int NLP_ENGINE::init(
    _TCHAR *analyzer,
	bool develop,
	bool silent,
    bool compile,
    bool compiled,
    bool compileKB,
    bool compileAna
)
{
//     NLP_ENGINE::zeroInit();    // [DEGLOB]	// 10/15/20 AM.
    // If calling a different analyzer, clear out info. // [DEGLOB]	// 10/15/20 AM.
    // Todo: The NLP object should store all analyzer information.  // [DEGLOB]	// 10/15/20 AM.
    NLP_ENGINE::zeroAna();  // [DEGLOB]	// 10/15/20 AM.
    // This engine's string table must be the process default while it
    // loads analyzers, or their names land in another engine's (#791).
    if (m_vtrun)
        m_vtrun->useStab();
 
    // Convenience ptr.
    //m_vtrun = VTRun_Ptr;    // [DEGLOB]	// 10/15/20 AM.

    // The analyzer string is borrowed, not owned: it is read during this call
    // only and m_analyzer is cleared below, so the caller may free it after.
    m_analyzer = analyzer;
    m_develop = develop;
    m_silent = silent;
    m_compile = compile;
    m_compiled = compiled;
    m_compileKB = compileKB;
    m_compileAna = compileAna;

    if (!analyzer || !*analyzer) {
        std::_t_cerr << _T("[Error: no analyzer given.]") << std::endl;
        return -1;
    }

    std::error_code ec;		// #323.
    if (!bprintf(m_anadir,_T("%s"),analyzer)) {
        std::_t_cerr << _T("[Error: analyzer path too long: ") << analyzer << _T("]") << std::endl;
        m_analyzer = 0;
        return -1;
    }

    if (m_workingFolder[0] != '\0') {
        if (!std::filesystem::exists(std::filesystem::path(analyzer), ec)) {
            if (!bprintf(m_anadir, _T("%s%sanalyzers%s%s"),m_workingFolder,DIR_STR,DIR_STR,analyzer)) {
                std::_t_cerr << _T("[Error: analyzer path too long: ") << analyzer << _T("]") << std::endl;
                m_analyzer = 0;
                return -1;
            }
            // NLP-ENGINE-521: m_ananame is MAXPATH bytes; the analyzer is an
            // unbounded caller-supplied _TCHAR*. Bound the copy so we can't
            // overflow the stack buffer on long analyzer paths.
            _stprintf(m_ananame, _T("%.*s"), (int)(MAXPATH - 1), analyzer);
        }
    }
    m_analyzer = 0;

    if (!std::filesystem::exists(std::filesystem::path(m_anadir), ec)) {
        // A failure, not a success: returning 0 here let analyze() go on and
        // a compile run of a missing analyzer report success (#484).
        std::_t_cerr << _T("[analyzer directory not found: ") << m_anadir << _T("]") << std::endl;
        return -1;
    }
    
    if (m_ananame[0] == '\0') {
        // Canonical registry name = basename of the resolved analyzer dir.
        // Shared with close() via anaRegName() so different path-forms of the
        // same analyzer share one registry entry (#658, #380). The helper also
        // carries the NLP-ENGINE-521 overflow-safe bounding.
        anaRegName(m_anadir, m_ananame);
    }

	std::_t_cerr << _T("[analyzer directory: ") << m_anadir << _T("]") << std::endl;
    std::_t_cerr << _T("[analyzer name: ") << m_ananame << _T("]") << std::endl; 

    bprintf(m_rfbdir, _T("%s%cdata%crfb%cspec"),m_workingFolder,DIR_CH,DIR_CH,DIR_CH);
    std::_t_cerr << _T("[rfb file: ") << m_rfbdir << _T("]") << std::endl;

#ifdef LINUX
    _TCHAR *tmp = _T("./tmp");
#else
    _TCHAR* tmp = _T(".\\tmp");
#endif
    bprintf(m_logfile, _T("%s%cvisualtext.log"),tmp,DIR_CH);
    if (!silent) {
        NLP_ENGINE::createDir(tmp);
        std::_t_cerr << _T("[log file: ") << m_logfile << _T("]") << std::endl;
    }

    // m_anadir fit in MAXPATH*2, and these buffers are larger, so they fit.
    bprintf(m_specdir, _T("%s%sspec"), m_anadir, DIR_STR);
    std::_t_cerr << _T("[spec directory: ") << m_specdir << _T("]") << std::endl;

    bprintf(m_seqfile, _T("%s%sanalyzer.seq"),m_specdir,DIR_STR);
    std::_t_cerr << _T("[spec file: ") << m_seqfile << _T("]") << std::endl;

    bprintf(m_outdir, _T("%s%s%s"), m_anadir,DIR_STR,_T("output"));
    if (!silent) {
        NLP_ENGINE::createDir(m_outdir);
        std::_t_cerr << _T("[output directory: ") << m_outdir << _T("]") << std::endl;
    }

    /////////////////////////////////////////////////
    // INITIALIZE VISUALTEXT RUNTIME SYSTEM                        // 08/27/02 AM.
    /////////////////////////////////////////////////

    /////////////////////////////////////////////////
    // INITIALIZE ANALYZER RUNTIME ENGINE
    /////////////////////////////////////////////////
    // Key the lookup on the canonical name (m_ananame), NOT the raw path, so a
    // re-entrant analyze() of an already-loaded analyzer -- even when the caller
    // spells the path differently -- reuses the loaded NLP instead of rebuilding
    // and orphaning it (#658, #380).
    if ((m_nlp = m_vtrun->findAna(m_ananame)))
        {
        // Reuse the already-loaded analyzer (this IS the "RELOAD ANALYZER" path
        // the old TODO referred to): grab its concept graph and skip the rebuild.
        std::_t_cerr << _T("[Reusing loaded analyzer: ") << m_ananame << _T("]") << std::endl;
        m_cg = m_nlp->getCG();
        }
    else
        {
        // Create and initialize an NLP object to manage text analysis.
        // NOTE: This init will dynamically load the user extensions dll at
        // appdir\user\debug\user.dll
        // Build under the canonical name so findAna() above can match it on a
        // later re-entry. makeNLP -> NLP::NLP already registers the instance via
        // VTRun::addAna (nlp.cpp), so no explicit addAna is needed here -- a
        // second call only produced the spurious "Named analyzer already
        // present" noise (#632).
        m_nlp = m_vtrun->makeNLP(m_anadir,m_ananame,m_develop,m_silent,m_compiled);  // 07/21/03 AM.


        /////////////////////////////////////////////////
        // SET UP THE KNOWLEDGE BASE
        /////////////////////////////////////////////////

        // Interpreted run: defer reading kb/user until Parse::execute reaches
        // the first tokenizer/grammar pass, so any pre-tokenizer pass (eg a
        // python pass that generates kbb/dict files) runs first and the load
        // then sees what it produced. Compile modes and compiled-analyzer runs
        // still load eagerly here (they don't go through Parse::execute).	// 07/11/26 DD.
        bool deferUserKB = !m_compile && !m_compileKB && !m_compileAna && !m_compiled;

         clock_t kb_s_time = clock();                                  // KB read timing.
         m_cg = m_vtrun->makeCG(                                        // 07/21/03 AM.
                m_anadir,
                true,      // LOAD COMPILED KB IF POSSIBLE.
                m_nlp,       // Associated analyzer object.              // 07/21/03 AM.
                deferUserKB);                                           // 07/11/26 DD.


        if (!m_cg)                                                       // 07/21/03 AM.
        {
            std::_t_cerr << _T("[Couldn't make knowledge base.]") << std::endl;  // 07/21/03 AM.
            m_vtrun->rmAna(m_nlp);  // 09/27/20 AM.
            m_vtrun->deleteNLP(m_nlp);                                     // 07/21/03 AM.
            m_nlp = 0;  // avoid dangling ptr; analyze() must not deref a deleted NLP.
    //        VTRun::deleteVTRun(m_vtrun);                                 // 07/21/03 AM.
            return -1;
        }

        double kb_tot = (double)(clock() - kb_s_time) / CLOCKS_PER_SEC;
        if (!deferUserKB)                                                // 07/11/26 DD.
            std::_t_cerr << _T("[Loaded knowledge base: ") << kb_tot << _T(" sec]") << std::endl;             // 02/19/19 AM.

        // Compile the KB if requested. Analyzer-only compile skips this.
        if (m_compile || m_compileKB)
            {
            std::_t_cerr << _T("[Compiling knowledge base.]") << std::endl;
            m_cg->genKB();
            m_cg->compileKB();
            }

        // KB-only compile: skip building the analyzer.
        if (m_compileKB && !m_compile)
            return 0;

        /////////////////////////////////////////////////
        // BUILD ANALYZER APPLICATION
        /////////////////////////////////////////////////
        // Create an analyzer dynamically using the sequence file and rules files
        // under appdir\spec.

        clock_t ana_s_time = clock();                                 // Analyzer load timing.
        if (!m_nlp->make_analyzer(m_seqfile, m_anadir, m_develop,
            silent,              // Debug/log file output.             // 06/16/02 AM.
            0,
            m_compile || m_compileAna, // Compile analyzer while loading.
            compiled))           // Compiled/interp analyzer.
            {
            std::_t_cerr << _T("[Couldn't build analyzer.]") << std::endl;
            m_vtrun->rmAna(m_nlp);  // 09/27/20 AM.
            m_vtrun->deleteNLP(m_nlp);                                     // 07/21/03 AM.
            m_nlp = 0;  // avoid dangling ptr; analyze() must not deref a deleted NLP.
    //        VTRun::deleteVTRun(m_vtrun);                                 // 07/21/03 AM.
            return -1;
            }

        // Report analyzer load time. `compiled` loads bin/run.<ext>; otherwise
        // the analyzer is built/interned from spec/ rules.            // 06/18/26 DD.
        double ana_tot = (double)(clock() - ana_s_time) / CLOCKS_PER_SEC;
        std::_t_cerr << _T("[Loaded ")
                     << (compiled ? _T("compiled analyzer: ") : _T("analyzer: "))
                     << ana_tot << _T(" sec]") << std::endl;
        }   // end ELSE...

    /////////////////////////////
    // TEST RULE GENERATION.
    /////////////////////////////
    #ifdef TEST_RUG_
    CONCEPT *root = VTRun::getKBroot(cg);
    m_gram = cg->findConcept(root, _T("gram"));
    if (!m_gram)
    gram = m_cg->makeConcept(root, _T("gram"));
    RUG::rugInit(gram,m_cg);

    // Execute rule generation.
    m_rug = new RUG(gram, m_nlp, m_anapath, m_cg);
    m_rug->rugGenerate(false,false);      // Generate ALL.
    delete rug;
    rug = 0;
    #endif

    return 0;
}
int NLP_ENGINE::analyze(
    _TCHAR *analyzer,
    _TCHAR *infile,
    _TCHAR *outdir,
	bool develop,
	bool silent,
    bool compile,
    bool compiled,
    bool compileKB,
    bool compileAna
	)
{
    if (m_vtrun)
        m_vtrun->useStab();     // #791.
    // Respect init() failure. On a re-entrant analyze() in a long-lived process
    // (e.g. the node/python addon), a name-collision in addAna can leave a fresh
    // NLP unregistered; if make_analyzer/makeCG then fails, init() deletes m_nlp
    // and returns -1. Charging ahead used to deref the freed m_nlp below and
    // SIGSEGV (deterministic on Linux). Bail out cleanly.
    if (NLP_ENGINE::init(analyzer,develop,silent,compile,compiled,compileKB,compileAna) < 0)
        return -1;

    // Compile mode: C++ code generation is done in init(); do not run analysis.
    if (m_compile || m_compileKB || m_compileAna)
        return 0;

    if (!m_nlp)  // defensive: never analyze without a live analyzer object.
        return -1;

    // #639: a missing input-file argument (easy to hit with -COMPILED, whose
    // syntax differs from -COMPILE only by the trailing input path) used to
    // reach readFiles(NULL) -> std::filesystem::is_directory(NULL) and SIGSEGV.
    // Fail with a clear diagnostic instead of crashing.
    if (!infile || !*infile)
        {
        std::_t_cerr << _T("[Error: no input file given. Usage: nlp [-COMPILED] ")
                     << _T("-ANA <analyzer> -WORK <dir> <input-file>]") << std::endl;
        return -1;
        }

    readFiles(infile);
    std::string file;

    std::filesystem::path fname(analyzer);
    fname /= _T("logs");
    fname /= _T("file.log");
    std::_t_ofstream *fp = new std::_t_ofstream(fname.string(), std::ios::out);
    m_nlp->setIsLastFile(false);
    m_nlp->setIsFirstFile(true);
    m_nlp->setFinteractive(true);

    for (std::vector<std::filesystem::path>::iterator it = m_files.begin() ; it != m_files.end(); ++it) {
        file = it->string();
        if (std::next(it) == m_files.end()) {
            m_nlp->setIsLastFile(true);
        }
        analyzeFile(file, outdir, fp);
    }

    delete fp;  // Closes file.log; it was never flushed before.
//    NLP_ENGINE::close();  // NO.  // 09/27/20 AM.
    return 0;
}

// Long-running batch (#548): load the analyzer and KB once, then analyze one
// input file per line of `in` until a blank line or EOF. Input arrives one
// file at a time, but otherwise this is a folder run: $isdirrun is true and
// the KB and its changes carry over from one input to the next, so an
// analyzer resets on $isfirstfile and wraps up on $islastfile as it would for
// a folder.
//
// $islastfile has to be set before the last input runs, but only the line
// after it (blank or EOF) says it was last. So the stream holds one input
// back: each is analyzed once the following line has been read.
int NLP_ENGINE::analyzeStream(
    _TCHAR *analyzer,
    std::istream &in,
    _TCHAR *outdir,
	bool develop,
	bool silent,
    bool compiled
	)
{
    if (m_vtrun)
        m_vtrun->useStab();     // #791.
    if (NLP_ENGINE::init(analyzer,develop,silent,false,compiled,false,false) < 0)
        return -1;

    if (!m_nlp)  // defensive: never analyze without a live analyzer object.
        return -1;

    std::filesystem::path fname(analyzer);
    fname /= _T("logs");
    fname /= _T("file.log");
    std::_t_ofstream *fp = new std::_t_ofstream(fname.string(), std::ios::out);
    m_nlp->setIsDirRun(true);
    m_nlp->setIsLastFile(false);
    m_nlp->setIsFirstFile(true);
    m_nlp->setFinteractive(true);

    const char *space = " \t\r\n";
    std::string line, held;
    long count = 0;
    while (std::getline(in, line)) {
        size_t beg = line.find_first_not_of(space);
        if (beg == std::string::npos)
            break;  // A blank line ends the stream, like EOF.
        line = line.substr(beg, line.find_last_not_of(space) - beg + 1);

        // Resolve the same way analyzeFile() will, but here, so a bad path is
        // reported and skipped instead of taking the $islastfile slot.
        std::filesystem::path p(line);
        if (!std::filesystem::is_regular_file(p))
            p = std::filesystem::path(m_anadir) / _T("input") / line;
        if (!std::filesystem::is_regular_file(p)) {
            std::_t_cerr << _T("[stream: no such input file: ") << line << _T("]") << std::endl;
            continue;
        }

        if (!held.empty()) {
            analyzeFile(held, outdir, fp);
            ++count;
        }
        held = p.string();  // Resolved, so <file>_log lands beside the input.
    }
    if (!held.empty()) {
        m_nlp->setIsLastFile(true);
        analyzeFile(held, outdir, fp);
        ++count;
    }

    delete fp;
    std::_t_cerr << _T("[stream: analyzed ") << count << _T(" input(s)]") << std::endl;
    return 0;
}

// Analyze one input file of a folder run or stream with the analyzer that
// init() loaded. The caller sets $isfirstfile/$islastfile beforehand.
void NLP_ENGINE::analyzeFile(
    const std::string &file,
    _TCHAR *outdir,
    std::_t_ofstream *fp
	)
{
    // file.log names each file by its path after "input". A path without
    // "input" used to compute npos+6 -- and substr() threw out_of_range,
    // aborting the engine, for any path shorter than 5 characters.	// 10/13/26 DD.
    size_t at = file.find("input");
    std::string filepath = (at == std::string::npos) ? file : file.substr(at + 6);
    *fp << _T("File=") << filepath << std::endl;

    bool fits;
    if (std::filesystem::exists(file))
        fits = bprintf(m_infile, _T("%s"),file.c_str());
    else
        fits = bprintf(m_infile, _T("%s%sinput%s%s"),m_anadir,DIR_STR,DIR_STR,file.c_str());
    if (!fits) {
        std::_t_cerr << _T("[Error: input path too long, skipped: ") << file << _T("]") << std::endl;
        return;
    }
    std::_t_cerr << _T("[infile path: ") << m_infile << _T("]") << std::endl;

    bprintf(m_outfile, _T("%s%soutfile.txt"),m_anadir,DIR_STR);
    std::_t_cerr << _T("[outfile path: ") << m_outfile << _T("]") << std::endl;

    bool create = true;
    if (outdir) {
        if (std::filesystem::is_directory(outdir)) {
            if (!bprintf(m_outdir, _T("%s"), outdir)) {
                std::_t_cerr << _T("[Error: output folder path too long: ") << outdir << _T("]") << std::endl;
                return;
            }
            create = false;
        }
    }
    if (!m_silent && create) {
        if (!bprintf(m_outdir, _T("%s_log"),file.c_str())) {
            std::_t_cerr << _T("[Error: input path too long, skipped: ") << file << _T("]") << std::endl;
            return;
        }
        NLP_ENGINE::createDir(m_outdir);
        // Start each run with an empty <file>_log. The engine rewrites its own
        // logs and trees, but an analyzer's "file" << ... output opens for
        // append, so a second run left two copies in out.txt, two JSON
        // documents in one .json file, and so on. Only files go, and only in
        // the folder the engine made itself: a caller-supplied outdir is
        // never touched, nor are subfolders an analyzer may keep there.
        NLP_ENGINE::clearDirFiles(m_outdir);
    }
    std::_t_cerr << _T("[outdir path: ") << m_outdir << _T("]") << std::endl;

    // Write missing-words.log into this file's output "*_log" folder.
    if (m_cg)
        m_cg->setMissingLogDir(m_outdir);

    // The analyzer's cout() writes to standard output.

    // Testing output to buffer.
    _TCHAR obuf[MAXSTR];

    m_nlp->analyze(m_infile, m_outfile, m_anadir, m_develop,
        m_silent,        // Debug/log output files.                  // 06/16/02 AM.
        m_outdir,             // Outdir.
        0,           // Input buffer.
        0,        // Length of input buffer, or 0.
        m_compiled,      // If running compiled analyzer.
        &std::_t_cout,	   // Stream for cout() in NLP++
        obuf,       // 05/11/02 AM.
        MAXSTR,	   // 05/11/02 AM.
        datumArg()  // G("$datum").
        );

    // A caller reading a stream's cout() output needs each input's share now,
    // not when the process exits.
    std::_t_cout.flush();

    m_nlp->setIsFirstFile(false);
}

int NLP_ENGINE::analyze(
    _TCHAR *analyzer,
    _TCHAR *inbuf,
    long len,
    _TCHAR *outbuf,
    long outlen,
	bool develop,
	bool silent,
    bool compile,
    bool compiled,
    bool compileKB,
    bool compileAna
	)
{
    if (m_vtrun)
        m_vtrun->useStab();     // #791.

    // Respect init() failure (see file-I/O overload above): bail instead of
    // dereferencing a freed m_nlp.
    if (NLP_ENGINE::init(analyzer,develop,silent,compile,compiled,compileKB,compileAna) < 0)
        return -1;

    if (m_compile || m_compileKB || m_compileAna)
        return 0;

    if (!m_nlp)  // defensive: never analyze without a live analyzer object.
        return -1;

    // The analyzer's cout() writes to standard output.
    
    std::_t_cerr << _T("[infile path: ") << m_infile << _T("]") << std::endl;
    std::_t_cerr << _T("[outfile path: ") << m_outfile << _T("]") << std::endl;

//std::_t_cout << _T("BEFORE ANALYSIS: ") << std::endl; // 09/27/20 AM.
//object_counts();    // TESTING analysis cleanup.    // 09/27/20 AM.

    m_nlp->analyze(m_infile, m_outfile, m_anadir, m_develop,
        m_silent,        // Debug/log output files.                  // 06/16/02 AM.
        0,             // Outdir.
        inbuf,           // Input buffer.
        len,        // Length of input buffer, or 0.
        m_compiled,      // If running compiled analyzer.
        &std::_t_cout,	   // Stream for cout() in NLP++
        outbuf,       // 05/11/02 AM.
        outlen,	   // 05/11/02 AM.
        datumArg()  // G("$datum").
        );

//std::_t_cout << _T("AFTER ANALYSIS: ") << std::endl; // 09/27/20 AM.
//object_counts();    // TESTING analysis cleanup.    // 09/27/20 AM.
    return 0;
}

// ANALYZE VARIANT FOR STRSTREAM I/O.  11/24/20 AM.
int NLP_ENGINE::analyze(
    _TCHAR *analyzer,
    std::istringstream *iss,
    std::ostringstream *oss,
	bool develop,
	bool silent,
    bool compile,
    bool compiled,
    bool compileKB,
    bool compileAna
	)
{
    if (m_vtrun)
        m_vtrun->useStab();     // #791.

    // Respect init() failure (see file-I/O overload above): bail instead of
    // dereferencing a freed m_nlp.
    if (NLP_ENGINE::init(analyzer,develop,silent,compile,compiled,compileKB,compileAna) < 0)
        return -1;

    if (m_compile || m_compileKB || m_compileAna)
        return 0;

    if (!m_nlp)  // defensive: never analyze without a live analyzer object.
        return -1;

    // The analyzer's cout() writes to standard output.

//std::_t_cout << _T("BEFORE ANALYSIS: ") << std::endl; // 09/27/20 AM.
//object_counts();    // TESTING analysis cleanup.    // 09/27/20 AM.

    m_nlp->analyze(
        iss,
        oss,
        m_anadir, // appdir
        m_develop, // flogfiles
        m_silent,
        m_outdir,
        m_compiled,      // If running compiled analyzer.
        &std::_t_cout,	   // Stream for cout() in NLP++
        datumArg()  // G("$datum"); was always NULL.
        );


//std::_t_cout << _T("AFTER ANALYSIS: ") << std::endl; // 09/27/20 AM.
//object_counts();    // TESTING analysis cleanup.    // 09/27/20 AM.
    return 0;
}

int NLP_ENGINE::close()
{
    /////////////////////////////////////////////////
    // CLEANUP VISUALTEXT RUNTIME
    /////////////////////////////////////////////////


    // Compiled analyzers: need to close the user.dll for the application also.
    // Shutdown the runtime manager.

    // NLP-ENGINE-523: make close() idempotent so it can be called explicitly
    // before ~NLP_ENGINE without double-tearing-down. The Python binding in
    // py-package-nlpengine exposes this as Engine.close() so callers (and
    // Engine.__del__ / __exit__) can deterministically release the open
    // <workfolder>/logs/cgerr.log file handle before TemporaryDirectory
    // cleanup runs at interpreter shutdown — on Windows that handle blocks
    // the temp-dir delete and surfaces as PermissionError, currently
    // band-aided in NLPPlus via TemporaryDirectory(ignore_cleanup_errors=True).
    if (!m_vtrun)
        return 0;

//    VTRun::deleteVTRun(VTRun_Ptr);                      // 9/27/20 AM.
//    VTRun_Ptr = 0;      // Clear out static var.        // 09/27/20 AM.
    VTRun::deleteVTRun(m_vtrun);    // [DEGLOB]	// 10/15/20 AM.
    m_vtrun = 0;    // [DEGLOB]	// 10/15/20 AM.
    std::_t_cerr << _T("[AFTER VTRUN DELETE: ]") << std::endl;    // 09/27/20 AM.

    // Report memory leaks to standard output.
    // Disabled: deleteVTRun above tears down the runtime, after which
    // `gout` (used by every prettyCount writer) points at a deleted stream
    // and *gout AVs. The other four object_counts() call sites in this file
    // are already commented out for the same reason; this one was missed.
    // object_counts();    // 09/27/20 AM.

    return 0;
}

// Remove a single named analyzer.
int NLP_ENGINE::close(_TCHAR *analyzer)
{
    /////////////////////////////////////////////////
    // CLEANUP VISUALTEXT RUNTIME
    /////////////////////////////////////////////////

    //NLP_ENGINE::close();

    if (!m_vtrun)
        return 0;

    // Look up by the same canonical name init() registered under (basename via
    // anaRegName), so a path-form mismatch can't leave the analyzer registered
    // -- and thus leaked -- on teardown (#658, #380). Only rm/delete if actually
    // found; the old code passed a possibly-NULL findAna result straight into
    // rmAna/deleteNLP, which logged errors and could dangle m_nlp.
    _TCHAR name[MAXPATH];
    anaRegName(analyzer, name);
    NLP *nlp = m_vtrun->findAna(name);
    if (nlp)
        {
        m_vtrun->rmAna(nlp);  // 09/27/20 AM.
        m_vtrun->deleteNLP(nlp);   // Remove analyzer from manager.    // 09/27/20 AM.
        if (m_nlp == nlp)
            m_nlp = 0;  // current-analyzer pointer now dangling; clear it.
        }

    return 0;
}

void NLP_ENGINE::setDictSource(bool on) {
    CG::setDictSource(on);	// #432.
}

void NLP_ENGINE::setDatum(const _TCHAR *datum) {
    m_datum = datum ? datum : _T("");
}

const _TCHAR *NLP_ENGINE::getDatum() {
    return m_datum.c_str();
}

// The datum to pass to an analysis: 0 when unset, as before.
_TCHAR *NLP_ENGINE::datumArg() {
    return m_datum.empty() ? 0 : (_TCHAR *) m_datum.c_str();
}

int NLP_ENGINE::createDir(_TCHAR *dirPath) {
    std::error_code ec;		// #323.
    if (!std::filesystem::exists(std::filesystem::path(dirPath), ec)) {
	    std::filesystem::create_directories(std::filesystem::path(dirPath), ec);
	    std::_t_cerr << _T("[Creating output directory: ") << dirPath << _T("]") << std::endl;
	}
    return 0;
}

int NLP_ENGINE::clearDirFiles(_TCHAR *dirPath) {
    int removed = 0;
    std::error_code ec;
    std::filesystem::directory_iterator it(dirPath, ec), end;
    for (; !ec && it != end; it.increment(ec)) {
        std::error_code fec;
        if (it->is_regular_file(fec) && std::filesystem::remove(it->path(), fec))
            ++removed;
    }
    return removed;
}

int NLP_ENGINE::readFiles(_TCHAR *dir)
{
    m_files.clear();

    if (std::filesystem::is_directory(dir)) {
        m_nlp->setIsDirRun(true);
        read_files(dir,_T(""),m_files);
        return 1;
    } else {
        m_nlp->setIsDirRun(false);
        m_files.push_back(dir);
        return 1;
    }
}