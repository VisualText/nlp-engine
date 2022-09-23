/*******************************************************************************
Copyright (c) 2005-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/


#ifndef _U_OFSTREAM_H_
#define _U_OFSTREAM_H_

#ifdef UNICODE

#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <iostream>
#include <fstream>
#include <sstream>
#endif


#include "u_convert.h"

class u_ofstream: public wofstream {

// Attributes
public:

// Operations
public:
//        u_ofstream();
//        ~u_ofstream();
        u_ofstream& operator<<(const _TCHAR &lpszBuffer);
};

#elif LINUX
#include <sstream>
#else
#include <strstream>
#endif

#endif
