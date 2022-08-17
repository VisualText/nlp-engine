/*******************************************************************************
Copyright (c) 2002-2009 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	AKBM.CPP
* FILE:	c:/cs/libkbm/Akbm.cpp
* CR:		06/11/02 AM.
* SUBJ:	API class for the KB Manager module.
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
#include "prim/list_s.h"	// 08/15/02 AM.
#include "prim/list.h"		// 08/15/02 AM.

#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/Akbm.h"

int AKBM::count_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

AKBM::AKBM(
	)
{
st_	= 0;
con_	= 0;
ptr_	= 0;

// Create knowledge base tables.
ast_  = new AST();
asym_ = new ASYM();
aptr_ = new APTR();
acon_ = new ACON();

// Set up all the backpointers among tables.
aptr_->acon_ = acon_;
aptr_->asym_ = asym_;

asym_->ast_ = ast_;

acon_->akbm_ = this;
acon_->asym_ = asym_;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
/*******************************************/

AKBM::~AKBM()
{
if (st_)
	delete [] (_TCHAR *) st_;
if (con_)
	delete [] (CON *) con_;
if (ptr_)
	delete [] (PTR *) ptr_;

// Todo: Each table should take care of its cleanup,
// then remove this call.
kbm_clean();

if (acon_)
	delete acon_;
if (asym_)
	delete asym_;
if (aptr_)
	delete aptr_;
if (ast_)
	delete ast_;

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
/*******************************************/
void *AKBM::getSt()	{return st_;}
void *AKBM::getCon()	{return con_;}
void *AKBM::getPtr()	{return ptr_;}


void	AKBM::setSt(void	*x)	{st_	= x;}
void	AKBM::setCon(void *x)	{con_	= x;}
void	AKBM::setPtr(void *x)	{ptr_	= x;}

///////////
