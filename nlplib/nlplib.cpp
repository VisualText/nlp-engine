#include "nlplib.h"

#include "lite/nlp_engine.h"

NLP_ENGINE *nlpEngine = NULL;

class Global {

public:

    Global() {
    }

    ~Global() {
        if (nlpEngine!= NULL){
            delete nlpEngine;
        }
    }
};

LIBNLPLIB_API int test(const char * in, int inLen, char * out, int outLen)
{
    // _TCHAR analyzer[1001];
    // _TCHAR input[1001];
    // _TCHAR output[1001];

    // _stprintf(analyzer,"%s",_T("taiparse"));
    // _stprintf(input,"%s",_T("The quick brown fox jumped over the lazy dog."));
    // output[0] = '\0';

    try {
        if (nlpEngine == NULL) {
            nlpEngine = new NLP_ENGINE("taiparse");
        }
        nlpEngine->analyze(const_cast<char *>(in), inLen, out, outLen);
        // nlpEngine->analyze(input, 1000, output, 1000);
    } catch (const std::exception& e) {
        strncpy(out, e.what(), outLen);
        return -1;
    } catch(...) {
        strncpy(out, "Unknown exception 2", outLen);
        return -1;
    }
    return 0;
}

LIBNLPLIB_API int test2() {
    _TCHAR analyzer[1001];
    _TCHAR input[1001];
    _TCHAR output[1001];

    _stprintf(analyzer,"%s",_T("taiparse"));
    _stprintf(input,"%s",_T("The quick brown fox jumped over the lazy dog."));
    output[0] = '\0';

    _t_cout << _T("[analyzer name: ") << analyzer << _T("]") << endl;
    _t_cout << _T("[input: ") << input << _T("]") << endl;
     
    NLP_ENGINE *nlpEngine = new NLP_ENGINE(analyzer);
    nlpEngine->analyze(input,1000,output,1000);

    _t_cout << _T("[output: ") << output << _T("]") << endl;
    return 0;
}
