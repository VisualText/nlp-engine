
// STATIC COMPILATION OF KB LIBRARY.	// 02/20/19 AM.
extern "C" bool kb_static_setup(void*);	// 02/20/19 AM.

#ifndef LINUX
// ENTRY POINT TO DYNAMICALLY LOADED DLL.								// 06/29/00 AM.
extern "C" bool kb_setup(void*);											// 08/16/02 AM.
#else
extern "C" bool kb_setup(void*);											// 08/16/02 AM.
#endif
