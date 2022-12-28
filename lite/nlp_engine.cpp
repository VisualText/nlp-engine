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

#ifdef LINUX
#include <unistd.h>
#include <dirent.h>
#endif

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

    std::filesystem::path logfile;
    std::filesystem::path rfbdir;
    if (!workingFolder.empty()) {
        m_workingFolder = workingFolder;
        logfile = workingFolder;
        rfbdir = workingFolder;
    }
    logfile /= _T("vtrun_logfile.out");
    rfbdir /= _T("data");
    rfbdir /= _T("rfb");
    rfbdir /= _T("spec");
    std::_t_cout << _T("[logfile: ") << logfile.string() << _T("]") << std::endl;
    std::_t_cout << _T("[rfbdir: ") << rfbdir.string() << _T("]") << std::endl;
 
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

// Onetime init of NLP_ENGINE and VTRun environment.    // [DEGLOB]	// 10/15/20 AM.
void NLP_ENGINE::zeroInit()
{ 
    m_input = 0;
    m_output = 0;
    m_sequence = 0;

    m_workingFolder.clear();
    m_anadir.clear();
    m_ananame[0] = '\0';
    m_rfbdir.clear();
    m_logfile.clear();	
    m_specdir.clear();
    m_infile.clear();
    m_outdir.clear();
    m_outfile.clear();;
    m_seqfile.clear();

    m_vtrun = 0;
    m_nlp = 0;
    m_cg = 0;

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

    m_anadir.clear();
    m_ananame[0] = '\0';
    m_rfbdir.clear();
    m_logfile.clear();	
    m_specdir.clear();
    m_infile.clear();
    m_outdir.clear();
    m_outfile.clear();
    m_seqfile.clear();

 //   m_vtrun = 0;
    m_nlp = 0;
    m_cg = 0;

}

int NLP_ENGINE::init(
    _TCHAR *analyzer,
	bool develop,
	bool silent,
    bool compiled
)
{   
//     NLP_ENGINE::zeroInit();    // [DEGLOB]	// 10/15/20 AM.
    // If calling a different analyzer, clear out info. // [DEGLOB]	// 10/15/20 AM.
    // Todo: The NLP object should store all analyzer information.  // [DEGLOB]	// 10/15/20 AM.
    NLP_ENGINE::zeroAna();  // [DEGLOB]	// 10/15/20 AM.
 
    // Convenience ptr.
    //m_vtrun = VTRun_Ptr;    // [DEGLOB]	// 10/15/20 AM.

    m_analyzer = analyzer;
    m_develop = develop;
    m_silent = silent;
    m_compiled = compiled;

    struct stat st;
    m_anadir = analyzer;

    if (!m_workingFolder.empty()) {
        if (!std::filesystem::exists(m_analyzer)) {
            m_anadir = m_workingFolder;
            m_anadir /= _T("analyzers");
            m_anadir /= m_analyzer;
            m_ananame = m_analyzer;          
        }
    }
    
    if (!std::filesystem::exists(m_anadir)) {
        std::_t_cerr << _T("[analyzer directory not found: ") << m_anadir.string() << _T("]") << std::endl;
        return 0;
    }
    
    if (m_ananame.empty()) {
        m_ananame = m_anadir.filename().string();         
    }

	std::_t_cout << _T("[analyzer directory: ") << m_anadir.string() << _T("]") << std::endl;
    std::_t_cout << _T("[analyzer name: ") << m_ananame << _T("]") << std::endl;

    m_rfbdir = m_workingFolder;
    m_rfbdir /= _T("data");
    m_rfbdir /= _T("rfb");
    m_rfbdir /= _T("spec");

    std::_t_cout << _T("[rfb file: ") << m_rfbdir.string() << _T("]") << std::endl;

    m_logfile = _T("tmp");
    m_logfile /= _T("visualtext.log");
    if (!silent) {
        std::filesystem::create_directory(_T("tmp"));
        std::_t_cout << _T("[log file: ") << m_logfile << _T("]") << std::endl;
    }

    m_specdir = m_anadir;
    m_specdir /= _T("spec");
    std::_t_cout << _T("[spec directory: ") << m_specdir.string() << _T("]") << std::endl;

    m_seqfile = m_anadir;
    m_seqfile /= _T("spec");
    std::_t_cout << _T("[spec file: ") << m_seqfile.string() << _T("]") << std::endl;
 
    m_outdir = m_anadir;
    m_outdir /= _T("output");
    if (!silent) {
        std::filesystem::create_directory(m_outdir);
        std::_t_cout << _T("[output directory: ") << m_outdir.string() << _T("]") << std::endl;
    }

    /////////////////////////////////////////////////
    // INITIALIZE VISUALTEXT RUNTIME SYSTEM                        // 08/27/02 AM.
    /////////////////////////////////////////////////

    /////////////////////////////////////////////////
    // INITIALIZE ANALYZER RUNTIME ENGINE
    /////////////////////////////////////////////////
//    if (m_nlp = VTRun_Ptr->findAna(analyzer))   // [DEGLOB]	// 10/15/20 AM.
    if ((m_nlp = m_vtrun->findAna(analyzer)))   // [DEGLOB]	// 10/15/20 AM.
        {
        std::_t_cout << _T("Analyzer found: ") << analyzer << analyzer << std::endl;
        std::_t_cout << _T("[TODO: RELOAD ANALYZER (NLP) INTO NLPENGINE HERE.]") << analyzer << std::endl;
        // return 1;    // If reloading same analyzer, done init....
        m_cg = m_nlp->getCG();

        //    std::_t_cout << _T("Analyzer not found: ") << analyzer << std::endl;
        }
    else
        {
        // Create and initialize an NLP object to manage text analysis.
        // NOTE: This init will dynamically load the user extensions dll at
        // appdir\user\debug\user.dll
        m_nlp = m_vtrun->makeNLP(m_anadir,m_analyzer,m_develop,m_silent,m_compiled);  // 07/21/03 AM.

//        VTRun_Ptr->addAna(m_nlp);   // Add ana to runtime manager.    // [DEGLOB]	// 10/15/20 AM.
        m_vtrun->addAna(m_nlp); // Register analyzer.   // [DEGLOB]	// 10/15/20 AM.


        /////////////////////////////////////////////////
        // SET UP THE KNOWLEDGE BASE
        /////////////////////////////////////////////////

         m_cg = m_vtrun->makeCG(                                        // 07/21/03 AM.
                m_anadir,
                true,      // LOAD COMPILED KB IF POSSIBLE.
                m_nlp);      // Associated analyzer object.              // 07/21/03 AM.


        if (!m_cg)                                                       // 07/21/03 AM.
        {
            std::_t_cerr << _T("[Couldn't make knowledge base.]") << std::endl;  // 07/21/03 AM.
            m_vtrun->rmAna(m_nlp);  // 09/27/20 AM.
            m_vtrun->deleteNLP(m_nlp);                                     // 07/21/03 AM.
    //        VTRun::deleteVTRun(m_vtrun);                                 // 07/21/03 AM.
            return -1;
        }

        std::_t_cerr << _T("[Loaded knowledge base.]") << std::endl;             // 02/19/19 AM.

        /////////////////////////////////////////////////
        // BUILD ANALYZER APPLICATION
        /////////////////////////////////////////////////
        // Create an analyzer dynamically using the sequence file and rules files
        // under appdir\spec.

        if (!m_nlp->make_analyzer(m_seqfile, m_anadir, m_develop,
            silent,              // Debug/log file output.             // 06/16/02 AM.
            0,
            false,               // false == Don't compile during load.
            compiled))           // Compiled/interp analyzer.
            {
            std::_t_cerr << _T("[Couldn't build analyzer.]") << std::endl;
            m_vtrun->rmAna(m_nlp);  // 09/27/20 AM.
            m_vtrun->deleteNLP(m_nlp);                                     // 07/21/03 AM.
    //        VTRun::deleteVTRun(m_vtrun);                                 // 07/21/03 AM.
            return -1;
            }
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
    bool compiled
	)
{   
    NLP_ENGINE::init(analyzer,develop,silent,compiled);

    readFiles(infile);
    std::string file;
    struct stat st;

    std::filesystem::path fname(analyzer);
    fname /= _T("logs");
    fname /= _T("file.log");
    std::_t_ofstream *fp = new std::_t_ofstream(fname.string(), std::ios::out);
    m_nlp->setIsLastFile(false);
    m_nlp->setIsFirstFile(true);

    for (std::vector<std::filesystem::path>::iterator it = m_files.begin() ; it != m_files.end(); ++it) {
        file = it->string();
        if (std::next(it) == m_files.end()) {
            m_nlp->setIsLastFile(true);
        }
        std::string s = file;
        std::string filepath = s.substr(s.find("input")+6,s.length()-1);
        *fp << _T("File=") << filepath << std::endl;

        if (std::filesystem::exists(file))
            m_infile = file;
        else {
            m_infile = m_anadir;
            m_infile /= _T("input");
            m_infile = file;
        }
        std::_t_cout << _T("[infile path: ") << m_infile.string() << _T("]") << std::endl;

        m_outfile = m_anadir;
        m_outfile /= _T("outfile.txt");
        std::_t_cout << _T("[outfile path: ") << m_outfile.string() << _T("]") << std::endl;

        bool create = true;
        m_outdir = outdir;
        if (std::filesystem::is_directory(m_outdir)) {
            m_outdir = outdir;
            create = false;
        }
        if (!silent && create) {
            m_outdir = file;
            std::filesystem::create_directory(m_outdir);
        }
        std::_t_cout << _T("[outdir path: ") << m_outdir.string() << _T("]") << std::endl;

        // Analyzer can output to a stream.
        _TCHAR ofstr[MAXSTR];
        #ifdef LINUX
        _stprintf(ofstr,_T("./dummy.txt"));
        #else
        _stprintf(ofstr,_T("e:\\dummy.txt"));
        #endif
        std::_t_ofstream os(TCHAR2CA(ofstr), std::ios::out);						// 08/07/02 AM.

        // Testing output to buffer.
        _TCHAR obuf[MAXSTR];

        m_nlp->analyze(m_infile, m_outfile, m_anadir, m_develop,
            m_silent,        // Debug/log output files.                  // 06/16/02 AM.
            m_outdir,             // Outdir.
            0,           // Input buffer.
            0,        // Length of input buffer, or 0.
            m_compiled,      // If running compiled analyzer.
            &os,	   // Rebind cout output stream in analyzer    // 08/07/02 AM.
            obuf,       // 05/11/02 AM.
            MAXSTR	   // 05/11/02 AM.
            );

        m_nlp->setIsFirstFile(false);
    }

//    NLP_ENGINE::close();  // NO.  // 09/27/20 AM.
    return 0;
}

int NLP_ENGINE::analyze(
    _TCHAR *analyzer,
    _TCHAR *inbuf,
    long len,
    _TCHAR *outbuf,
    long outlen,
	bool develop,
	bool silent,
    bool compiled
	)
{
 
    NLP_ENGINE::init(analyzer,develop,silent,compiled);

    // Analyzer can output to a stream.
    _TCHAR ofstr[MAXSTR];
    #ifdef LINUX
    _stprintf(ofstr,_T("./dummy.txt"));
    #else
    _stprintf(ofstr,_T("e:\\dummy.txt"));
    #endif
    std::_t_ofstream os(TCHAR2CA(ofstr), std::ios::out);
    
    std::_t_cout << _T("[infile path: ") << m_infile.string() << _T("]") << std::endl;
    std::_t_cout << _T("[outfile path: ") << m_outfile.string() << _T("]") << std::endl;

//std::_t_cout << _T("BEFORE ANALYSIS: ") << std::endl; // 09/27/20 AM.
//object_counts();    // TESTING analysis cleanup.    // 09/27/20 AM.

    m_nlp->analyze(m_infile, m_outfile, m_anadir, m_develop,
        m_silent,        // Debug/log output files.                  // 06/16/02 AM.
        0,             // Outdir.
        inbuf,           // Input buffer.
        len,        // Length of input buffer, or 0.
        m_compiled,      // If running compiled analyzer.
        &os,	   // Rebind cout output stream in analyzer    // 08/07/02 AM.
        outbuf,       // 05/11/02 AM.
        outlen	   // 05/11/02 AM.
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
    bool compiled
	)
{
 
    NLP_ENGINE::init(analyzer,develop,silent,compiled);

    // Analyzer can output to a stream.
    _TCHAR ofstr[MAXSTR];
    #ifdef LINUX
    _stprintf(ofstr,_T("./dummy.txt"));
    #else
    _stprintf(ofstr,_T("e:\\dummy.txt"));
    #endif
    std::_t_ofstream os(TCHAR2CA(ofstr), std::ios::out);

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
        &os,	   // Rebind cout output stream in analyzer    // 08/07/02 AM.
        NULL   // datum
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

//    VTRun::deleteVTRun(VTRun_Ptr);                      // 9/27/20 AM.
//    VTRun_Ptr = 0;      // Clear out static var.        // 09/27/20 AM.
    VTRun::deleteVTRun(m_vtrun);    // [DEGLOB]	// 10/15/20 AM.
    m_vtrun = 0;    // [DEGLOB]	// 10/15/20 AM.
    std::_t_cout << _T("[AFTER VTRUN DELETE: ]") << std::endl;    // 09/27/20 AM.

    // Report memory leaks to standard output.
    object_counts();    // 09/27/20 AM.

    return 0;
}

// Remove a single named analyzer.
int NLP_ENGINE::close(_TCHAR *analyzer)
{
    /////////////////////////////////////////////////
    // CLEANUP VISUALTEXT RUNTIME
    /////////////////////////////////////////////////

    //NLP_ENGINE::close();

    // This will close the user.dll for the application also.
 //   m_vtrun->deleteNLP(m_nlp);                                         // 07/21/03 AM.
 //   VTRun::deleteVTRun(m_vtrun);                                     // 07/21/03 AM.
//    m_nlp = VTRun_Ptr->findAna(analyzer); // [DEGLOB]	// 10/15/20 AM.
    m_nlp = m_vtrun->findAna(analyzer); // [DEGLOB]	// 10/15/20 AM.
    m_vtrun->rmAna(m_nlp);  // 09/27/20 AM.
    m_vtrun->deleteNLP(m_nlp);   // Remove analyzer from manager.    // 09/27/20 AM.

    return 0;
}

int NLP_ENGINE::readFiles(_TCHAR *dir) 
{
    m_files.clear();

    if (std::filesystem::is_directory(dir)) {
        m_nlp->setIsDirRun(false);
        m_files.push_back(dir);
        return 1;
    } else {
        m_nlp->setIsDirRun(false);
        read_files(dir,_T("*"),m_files);
        return 1;
    }
}