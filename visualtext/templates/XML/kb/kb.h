
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef LINUX
#define KB_API
#else

#ifdef KB_EXPORTS
#define KB_API __declspec(dllexport)
#else
#define KB_API __declspec(dllimport)
#endif

#endif

// This class is exported from the kb.dll
class KB_API CKb {
public:
	CKb(void);
	// TODO: add your methods here.
};

extern KB_API int nKb;

KB_API int fnKb(void);

