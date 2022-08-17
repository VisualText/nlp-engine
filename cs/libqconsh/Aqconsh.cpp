/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AQCONSH.CPP
* FILE:	cs/libqconsh/Aqconsh.cpp
* CR:		02/08/07 AM.
* SUBJ:	API class for the Conceptual Grammar KBMS based on QDBM.
* NOTE:	There is a test_libqconsh program in the dev\projects area.
*
*******************************************************************************/

#include "StdAfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "machine-min.h"

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aqconsh.h"

int AQCONSH::count_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

AQCONSH::AQCONSH(
	)
{

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

AQCONSH::~AQCONSH()
{

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
/*******************************************/
// void *AQCONSH::getSt()	{return st_;}


//void	AQCONSH::setSt(void	*x)	{st_	= x;}

///////////

