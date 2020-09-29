#include "lite/nlp_engine.h"
#include "lite/vtrun.h"     // Include NLP++ runtime manager.   // 09/25/20 AM.
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
    _TCHAR workingFolder[1001];
    _TCHAR input[1001];
    _TCHAR output[1001];

    _stprintf(anaParse,"%s",_T("taiparse"));
    _stprintf(anaNoop,"%s",_T("noop"));
    _stprintf(workingFolder,"%s",_T("./"));
 
    _t_cout << _T("[Create runtime manager.]") << endl;
    NLP_ENGINE *nlpEngine = new NLP_ENGINE(workingFolder);

    _stprintf(input,"%s",_T("The quick brown fox jumped over the lazy dog."));
    output[0] = '\0';
    _t_cout << _T("[analyzer: ") << anaParse << _T("]") << endl;
    _t_cout << _T("[input: ") << input << _T("]") << endl;
    nlpEngine->analyze(anaParse,input,1000,output,1000);
    _t_cout << _T("[output: ") << output << _T("]") << endl;

    _stprintf(input,"%s",_T("This doesn't matter"));
    output[0] = '\0';
    _t_cout << _T("[analyzer: ") << anaNoop << _T("]") << endl;
    _t_cout << _T("[input: ") << input << _T("]") << endl;
    nlpEngine->analyze(anaNoop,input,1000,output,1000);
    _t_cout << _T("[output: ") << output << _T("]") << endl;

    _stprintf(input,"%s",_T("It is time for all good men to come to the aid of their country."));
    output[0] = '\0';
    _t_cout << _T("[analyzer: ") << anaParse << _T("]") << endl;
    _t_cout << _T("[input: ") << input << _T("]") << endl;
    nlpEngine->analyze(anaParse,input,1000,output,1000);
    _t_cout << _T("[output: ") << output << _T("]") << endl;

    delete nlpEngine;
}
