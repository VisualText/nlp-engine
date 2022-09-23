#include "lite/nlp_engine.h"
#include "lite/vtrun.h"     // Include NLP++ runtime manager.   // 09/25/20 AM.

#ifdef LINUX
#include <unistd.h>
#define GetCurrentDir getcwd
#else
#include <direct.h>
#define GetCurrentDir _getcwd
#endif
#include<iostream>

#ifdef LINUX
int main(
#else
int _tmain( // 09/23/20 AM.
#endif
   int argc,
   _TCHAR *argv[],
   _TCHAR *envp[]
   )
{
    _TCHAR anaParse[1001];
    _TCHAR anaNoop[1001];
    _TCHAR anaCorporate[1001];
    _TCHAR workingFolder[1001];

    _stprintf(anaParse,"%s",_T("taiparse"));
    _stprintf(anaCorporate,"%s",_T("corporate"));
    _stprintf(anaNoop,"%s",_T("noop"));
    _stprintf(workingFolder,"%s",_T("./"));
 
    std::_t_cout << _T("[Create runtime manager.]") << std::endl;
    GetCurrentDir(workingFolder, FILENAME_MAX);
    std::string current_working_dir(workingFolder);
    NLP_ENGINE *nlpEngine = new NLP_ENGINE(workingFolder);

    char *str = 
"LOS ANGELES (NWI) - The No. 1 text analysis company Text "
"Analysis International Inc. (TAI) has bought the American "
"Medical Records Processing (AMRP) for more than $130 "
"million dollars. Avi Meyers, CEO and president of TAI, "
"said on Friday that they bought the Los Angeles-based AMRP "
"for $120 million in cash and $14.3 million in assumed debt."
"\n"
"Founded in 1972, AMRP is the largest text processing firm "
"in the country for medical records and journals and has "
"been using TAI's VisualText technology almost exclusively "
"for 3 years."
"\n"
"The sale of the medical records processing company for more "
"than $134.3 million is above analysts' predictions but is "
"not surprising given TAI's strong position in the market "
"place."
"\n"
"In 2006 alone, TAI has acquired MD Files, New Hampshire "
"Medical Inc., and Carezani Med Company of Maryland for a "
"total of 160 million dollars."
"\n"
"With this last purchase, TAI has substantially strengthened "
"its hold on the medical text processing field and hopes to "
"centralize the processing of medical records nationally."
"\n"
"With its latest purchase of the medical records processing "
"company, TAI's stock is up 4% from $58.33 a share to "
"$60.66."
"\n"
"TAI's stock has recently remained steady due to its lawsuit "
"with MiasmaCraft whose text processing IDE Miasma-Text bears "
"a striking resemblance to TAI's VisualText."
"\n"
"TAI is vigorously defending its patent on VisualText, "
"NLP++, and the Conceptual Grammar."
"\n"
"More than they know.";

    std::istringstream ssi1(str);
    std::ostringstream sso1;
    std::_t_cout << _T("[analyzer: ") << anaParse << _T("]") << std::endl;
    std::_t_cout << _T("[input: ") << ssi1.str() << _T("]") << std::endl;
    nlpEngine->analyze(anaParse,&ssi1,&sso1);
    std::_t_cout << _T("[output: ") << sso1.str() << _T("]") << std::endl;
    std::_t_cout << _T("[end: ") << anaParse << _T("]") << std::endl;

    std::istringstream ssi2(str);
    std::ostringstream sso2;
    std::_t_cout << _T("[analyzer: ") << anaCorporate << _T("]") << std::endl;
    std::_t_cout << _T("[input: ") << ssi2.str() << _T("]") << std::endl;
    nlpEngine->analyze(anaCorporate,&ssi2,&sso2);
    std::_t_cout << _T("[output: ") << sso2.str() << _T("]") << std::endl;
    std::_t_cout << _T("[end: ") << anaCorporate << _T("]") << std::endl;

    std::istringstream ssi3("Strstream test.");
    std::ostringstream sso3;
    std::_t_cout << _T("[analyzer: ") << anaNoop << _T("]") << std::endl;
    std::_t_cout << _T("[input: ") << ssi3.str() << _T("]") << std::endl;
    nlpEngine->analyze(anaNoop,&ssi3,&sso3);
    std::_t_cout << _T("[output: ") << sso3.str() << _T("]") << std::endl;
    std::_t_cout << _T("ok") << std::endl;

    delete nlpEngine;
}
