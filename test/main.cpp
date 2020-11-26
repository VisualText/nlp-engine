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
    _TCHAR anaCorporate[1001];
    _TCHAR workingFolder[1001];
    _TCHAR input[1001];
    _TCHAR output[1001];

    _stprintf(anaParse,"%s",_T("taiparse"));
    _stprintf(anaCorporate,"%s",_T("corporate"));
    _stprintf(anaNoop,"%s",_T("noop"));
    _stprintf(workingFolder,"%s",_T("./"));
 
    _t_cout << _T("[Create runtime manager.]") << endl;
    NLP_ENGINE *nlpEngine = new NLP_ENGINE(workingFolder);

#ifdef _TESTING_
    _stprintf(input,"%s",_T("The quick brown fox jumped over the lazy dog."));
    output[0] = '\0';
    _t_cout << _T("[analyzer: ") << anaParse << _T("]") << endl;
    _t_cout << _T("[input: ") << input << _T("]") << endl;
    nlpEngine->analyze(anaParse,input,1000,output,1000);
    _t_cout << _T("[output: ") << output << _T("]") << endl;


    _stprintf(input,"%s",_T("This doesn't matter"));
    output[0] = '\0';
    _t_cout << _T("[analyzer: ") << anaCorporate << _T("]") << endl;
    _t_cout << _T("[input: ") << input << _T("]") << endl;
    nlpEngine->analyze(anaCorporate,input,1000,output,1000);
    _t_cout << _T("[output: ") << output << _T("]") << endl;


    _stprintf(input,"%s",_T("Good trumps evil."));
    output[0] = '\0';
    _t_cout << _T("[analyzer: ") << anaParse << _T("]") << endl;
    _t_cout << _T("[input: ") << input << _T("]") << endl;
    nlpEngine->analyze(anaParse,input,1000,output,1000);
    _t_cout << _T("[output: ") << output << _T("]") << endl;
#endif


    istrstream ssi("Strstream test.");
    ostrstream sso;
    _t_cout << _T("[analyzer: ") << anaNoop << _T("]") << endl;
    _t_cout << _T("[input: ") << ssi.str() << _T("]") << endl;
    nlpEngine->analyze(anaNoop,&ssi,&sso);
    _t_cout << _T("[output: ") << sso.str() << _T("]") << endl;
    _t_cout << _T("ok") << endl;


    delete nlpEngine;
}
