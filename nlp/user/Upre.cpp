/*******************************************************************************
Copyright (c) 1998,1999 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	UPRE.CPP
* FILE:	c:\apps\webco\upre.cpp
* CR:		12/04/98 AM.
* SUBJ:	User-defined pre actions.
* NOTE:	1/29/02 AM. This may work, not used in years.
*
*******************************************************************************/
#ifndef LINUX
#include <windows.h>	// 01/10/06 AM.
#endif

#include "machine.h"

/*

#include "global.h"
#include "str.h"
#include "io.h"
#include "chars.h"
#include "string.h"
#include "irule.h"
#include "parse.h"
#include "pat.h"
#include "pn.h"

#include "arg.h"
#include "pre.h"
*/

#include "Upre.h"

/********************************************
* FN:		Special functions for the class
********************************************/

Upre::Upre()
	: Pre()
{
//cout << "[Creating Upre class.]" << endl;
}

Upre::~Upre()
{
}

/*******************************************/

/*******************************************/


/*******************************************/

/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

/********************************************
* FN:		Modify Functions
********************************************/


/********************************************
* FN:		EXECUTE
* CR:		12/04/98 AM.
* SUBJ:	Execute one pre action on current node.
* RET:	True if successful match, else false.
********************************************/

bool Upre::execute(
	_TCHAR *func,					// Name of pre action.
	DELTS *args,			// Action's arguments.
	Pn *pn						// Current node being matched.
	)
{
static bool warned = false;	// Todo: make this a list of strings!

// Just a simple test of pre actions.
#ifndef LINUX
if (!_tcsicmp(func, _T("lowercase")))
	return Upre::upreLowercase(args, pn);
else if (!_tcsicmp(func, _T("hello")))
	return Upre::upreHello(args, pn);
#else
if (!strcasecmp(func, _T("lowercase")))
	return Upre::upreLowercase(args, pn);
else if (!strcasecmp(func, _T("hello")))
	return Upre::upreHello(args, pn);
#endif

if (!warned)
	{
	cerr << _T("[Pre action: Unknown action=") << func << _T("]") << endl;
	warned = true;
	}
return false;
}

/********************************************
* FN:		PRE ACTIONS.
********************************************/


/********************************************
* FN:		UPRELOWERCASE
* CR:		12/04/98 AM.
* SUBJ:	See if node has lowercase text.
* RET:	True if pre succeeded.
********************************************/

bool Upre::upreLowercase(
	DELTS *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
static bool warned = false;
static bool warnedi = false;
bool ok = true;
if (args && !warned)
	{
	cerr << _T("[Lowercase pre action: Ignoring arguments.]") << endl;
	warned = true;			// Don't repeat this message.
	}
if (!warnedi)
	{
	cerr << _T("[Lowercase pre action: Not implemented.]") << endl;
	warnedi = true;
	}
return false;

// Get the node's text.  If alphabetic chars are all lowercase, then
// success.
#ifdef IMPLEMENT_
_TCHAR *buf;
buf = pn->pnStr();			// Create buffer for node's text.
ok = all_lowercase(buf);
Chars::destroy(buf);			// Free up buffer.
#endif

return ok;
}


/********************************************
* FN:		UPREHELLO
* CR:		04/14/00 AM.
* SUBJ:	Hello world.
* RET:	True if pre succeeded.
********************************************/

bool Upre::upreHello(
	DELTS *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
static bool warned = false;
static bool warnedi = false;
bool ok = true;
if (args && !warned)
	{
	cerr << _T("[Lowercase pre action: Ignoring arguments.]") << endl;
	warned = true;			// Don't repeat this message.
	}
if (!warnedi)
	{
	cerr << _T("[Lowercase pre action: Not implemented.]") << endl;
	warnedi = true;
	}
return false;

// Get the node's text.  If alphabetic chars are all lowercase, then
// success.
#ifdef IMPLEMENT_
_TCHAR *buf;
buf = pn->pnStr();			// Create buffer for node's text.
ok = all_lowercase(buf);
Chars::destroy(buf);			// Free up buffer.
#endif

cerr << _T("[HELLO WORLD PRE ACTION!]") << endl;

return ok;
}

/*************************      END OF FILE      ******************************/
