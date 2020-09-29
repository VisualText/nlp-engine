#include "nlplib.h"
#include "lite/nlp_engine.h"
#include <map>
#include <string>

typedef std::map<std::string, NLP_ENGINE *> EngineMap;
EngineMap engines;

class Global {

public:

    Global() {
    }

    ~Global() {
        for (EngineMap::const_iterator itr = engines.begin(); itr != engines.end(); ++itr) {
            itr->second->close();
            delete itr->second;
        }
        engines.clear();
    }
};

LIBNLPLIB_API int analyse(const char * workingFolder, const char * analyzer, const char * in, int inLen, char * out, int outLen)
{
    /*
    try {
        if (engines.find(analyzer) == engines.end()) {
            engines[analyzer] = new NLP_ENGINE(const_cast<char *>(analyzer), false, true, false, workingFolder);
        }
        engines[analyzer]->analyze(const_cast<char *>(in), inLen, out, outLen);
    } catch (const std::exception& e) {
        strncpy(out, e.what(), outLen);
        return -1;
    } catch(...) {
        strncpy(out, "Unknown exception ...", outLen);
        return -2;
    }
    */
    return 0;
}
