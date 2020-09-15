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

NLP_ENGINE::NLP_ENGINE(
	_TCHAR *analyzer,
    bool develop,
	bool silent,
    bool compiled
	)
{
    NLP_ENGINE::init(analyzer,develop,silent,compiled);
}

NLP_ENGINE::~NLP_ENGINE()
{
}

void NLP_ENGINE::zeroInit()
{ 
    m_input = 0;
    m_output = 0;
    m_sequence = 0;

    m_anadir[0] = '\0';
    m_ananame[0] = '\0';
    m_rfbdir[0] = '\0';
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

    #ifdef TEST_RUG_
    m_gram = 0;
    m_rug = 0;
    #endif
}
 
int NLP_ENGINE::init(
    _TCHAR *analyzer,
	bool develop,
	bool silent,
    bool compiled
)
{   
    NLP_ENGINE::zeroInit();
    m_analyzer = analyzer;
    m_develop = develop;
    m_silent = silent;
    m_compiled = compiled;

    struct stat st;

    if (stat(analyzer,&st) != 0) {
	#ifdef LINUX
        char cwd[MAXSTR];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            _t_cout << _T("[current directory: ") << cwd << _T("]") << endl;
            _stprintf(m_anadir, _T("%s%sanalyzers%s%s"),cwd,DIR_STR,DIR_STR,analyzer);
            _stprintf(m_ananame, _T("%s"),analyzer);
        } else {
            _t_cerr << _T("[couldn't get current directory") << analyzer << _T("]") << endl;
            return 0;
        }
	#endif
    } else {
        _stprintf(m_anadir, _T("%s"),analyzer);	
        _TCHAR *ana = _tcsrchr(m_anadir,_T('/'));
        ++ana;
        _stprintf(m_ananame, _T("%s"), ana); 
    }
	_t_cout << _T("[analyzer directory: ") << m_anadir << _T("]") << endl;
    _t_cout << _T("[analyzer name: ") << m_ananame << _T("]") << endl; 

    _stprintf(m_rfbdir, _T("./data%crfb%cspec"),DIR_CH,DIR_CH);  
    _t_cout << _T("[rfb file: ") << m_rfbdir << _T("]") << endl;

    _TCHAR *tmp = _T("./tmp");
    _stprintf(m_logfile, _T("%s%cvisualtext.log"),tmp,DIR_CH);
       NLP_ENGINE::createDir(tmp);
    _t_cout << _T("[log file: ") << m_logfile << _T("]") << endl;

    _stprintf(m_specdir, _T("%s%sspec"), m_anadir, DIR_STR);
    _t_cout << _T("[spec directory: ") << m_specdir << _T("]") << endl;

    _stprintf(m_seqfile, _T("%s%sanalyzer.seq"),m_specdir,DIR_STR);
    _t_cout << _T("[spec file: ") << m_seqfile << _T("]") << endl;

    _stprintf(m_outdir, _T("%s%s%s"), m_anadir,DIR_STR,_T("output"));
    NLP_ENGINE::createDir(m_outdir);
    _t_cout << _T("[output directory: ") << m_outdir << _T("]") << endl;

    /////////////////////////////////////////////////
    // INITIALIZE VISUALTEXT RUNTIME SYSTEM                        // 08/27/02 AM.
    /////////////////////////////////////////////////
    m_vtrun = VTRun::makeVTRun(                               // 07/21/03 AM.
        m_logfile,                // Verbose/error log file.         // 08/28/02 AM.
        m_rfbdir,                 // VisualText dir for RFB spec.    // 08/28/02 AM.
        true                      // Build silently.                 // 08/28/02 AM.
        );

    /////////////////////////////////////////////////
    // INITIALIZE ANALYZER RUNTIME ENGINE
    /////////////////////////////////////////////////
    // Create and initialize an NLP object to manage text analysis.
    // NOTE: This init will dynamically load the user extensions dll at
    // appdir\user\debug\user.dll
    m_nlp = m_vtrun->makeNLP(m_anadir,m_analyzer,m_develop,m_silent,m_compiled);  // 07/21/03 AM.

    /////////////////////////////////////////////////
    // SET UP THE KNOWLEDGE BASE
    /////////////////////////////////////////////////

    m_cg = m_vtrun->makeCG(                                        // 07/21/03 AM.
            m_anadir,
            true,      // LOAD COMPILED KB IF POSSIBLE.
            m_nlp);      // Associated analyzer object.              // 07/21/03 AM.

    if (!m_cg)                                                       // 07/21/03 AM.
    {
        _t_cerr << _T("[Couldn't make knowledge base.]") << endl;  // 07/21/03 AM.
        m_vtrun->deleteNLP(m_nlp);                                     // 07/21/03 AM.
        VTRun::deleteVTRun(m_vtrun);                                 // 07/21/03 AM.
        return -1;
    }

    _t_cerr << _T("[Loaded knowledge base.]") << endl;             // 02/19/19 AM.

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
        _t_cerr << _T("[Couldn't build analyzer.]") << endl;
        m_vtrun->deleteNLP(m_nlp);                                     // 07/21/03 AM.
        VTRun::deleteVTRun(m_vtrun);                                 // 07/21/03 AM.
        return -1;
    }

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
}
 
int NLP_ENGINE::analyze(
    _TCHAR *infile,
    _TCHAR *outfile,
	bool develop,
	bool silent,
    bool compiled
	)
{   
    struct stat st;
    if (stat(infile,&st) == 0)
        _stprintf(m_infile, _T("%s"),infile);
    else
        _stprintf(m_infile, _T("%s%sinput%s%s"),m_anadir,DIR_STR,DIR_STR,infile);
    _t_cout << _T("[infile path: ") << m_infile << _T("]") << endl;
    _stprintf(m_outfile, _T("%s%soutput.txt"),m_anadir,DIR_STR);
    _t_cout << _T("[outfile path: ") << m_outfile << _T("]") << endl;

    // Analyzer can output to a stream.
    _TCHAR ofstr[MAXSTR];
    #ifdef LINUX
    _stprintf(ofstr,_T("./dummy.txt"));
    #else
    _stprintf(ofstr,_T("e:\\dummy.txt"));
    #endif
    _t_ofstream os(TCHAR2CA(ofstr), ios::out);						// 08/07/02 AM.

    // Testing output to buffer.
    _TCHAR obuf[MAXSTR];

    m_nlp->analyze(m_infile, m_outfile, m_anadir, m_develop,
        m_silent,        // Debug/log output files.                  // 06/16/02 AM.
        0,             // Outdir.
        0,           // Input buffer.
        0,        // Length of input buffer, or 0.
        m_compiled,      // If running compiled analyzer.
        &os,	   // Rebind cout output stream in analyzer    // 08/07/02 AM.
        obuf,       // 05/11/02 AM.
        MAXSTR	   // 05/11/02 AM.
        );

    NLP_ENGINE::close();
}

int NLP_ENGINE::analyze(
    _TCHAR *inbuf,
    long len,
    _TCHAR *outbuf,
    long outlen,
	bool develop,
	bool silent,
    bool compiled
	)
{
    // Analyzer can output to a stream.
    _TCHAR ofstr[MAXSTR];
    #ifdef LINUX
    _stprintf(ofstr,_T("./dummy.txt"));
    #else
    _stprintf(ofstr,_T("e:\\dummy.txt"));
    #endif
    _t_ofstream os(TCHAR2CA(ofstr), ios::out);
    
    _t_cout << _T("[infile path: ") << m_infile << _T("]") << endl;
    _t_cout << _T("[outfile path: ") << m_outfile << _T("]") << endl;

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

    NLP_ENGINE::close();
}

int NLP_ENGINE::close()
{
    /////////////////////////////////////////////////
    // CLEANUP VISUALTEXT RUNTIME
    /////////////////////////////////////////////////

    // This will close the user.dll for the application also.
    m_vtrun->deleteNLP(m_nlp);                                         // 07/21/03 AM.
    VTRun::deleteVTRun(m_vtrun);                                     // 07/21/03 AM.
    object_counts();    // Report memory leaks to standard output.

    return 0;
}

int NLP_ENGINE::createDir(_TCHAR *dirPath) {
    struct stat st;

    if (stat(dirPath,&st) != 0) {
#ifdef LINUX
	    mkdir(dirPath, 777);
#else
	    CreateDirectory(dirPath,NULL);
#endif
	    _t_cout << _T("[Creating output directory: ") << dirPath << _T("]") << endl;
	}

}