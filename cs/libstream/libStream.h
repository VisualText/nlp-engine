
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBSTREAM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBSTREAM_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef LINUX
#define LIBSTREAM_API
#else
#ifdef LIBSTREAM_EXPORTS
#define LIBSTREAM_API __declspec(dllexport)
#else
#define LIBSTREAM_API __declspec(dllimport)
#endif
#endif

#ifndef _LIBSTREAM_H_
#define _LIBSTREAM_H_

// This class is exported from the libStream.dll
class LIBSTREAM_API CLibStream {
public:
	CLibStream(void);
	// TODO: add your methods here.
};

extern LIBSTREAM_API int nLibStream;

LIBSTREAM_API int fnLibStream(void);

#endif
