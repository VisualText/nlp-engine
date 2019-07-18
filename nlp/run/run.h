
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RUN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RUN_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef LINUX
#define RUN_API
#else

#ifdef RUN_EXPORTS
#define RUN_API __declspec(dllexport)
#else
#define RUN_API __declspec(dllimport)
#endif

#endif

// Forward references.
class Parse;

// This class is exported from the run.dll
#ifdef SAMPLE_
class RUN_API CRun {
public:
	CRun(void);
	// TODO: add your methods here.
};

extern RUN_API int nRun;

RUN_API int fnRun(void);
#endif

extern "C" bool run_analyzer(Parse *);

extern "C" void testy();


