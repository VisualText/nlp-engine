#ifndef LIBNLPLIB_H_
#define LIBNLPLIB_H_

#ifdef SHARED_LIB
 #ifdef _MSC_VER
  #define DECL_EXPORT __declspec(dllexport)
  #define DECL_IMPORT __declspec(dllimport)
 #elif __GNUC__ >= 4
  #define DECL_EXPORT __attribute__ ((visibility ("default")))
  #define DECL_IMPORT __attribute__ ((visibility ("default")))
 #endif
//  add MacOS
#else
 #define DECL_EXPORT
 #define DECL_IMPORT
#endif

#ifdef LIBNLPLIB_EXPORTS
#define LIBNLPLIB_API DECL_EXPORT
#else
#define LIBNLPLIB_API DECL_IMPORT
#endif

LIBNLPLIB_API int analyse(const char * workingFolder, const char * analyzer, const char * in, int inLen, char * out, int outLen);

#endif
