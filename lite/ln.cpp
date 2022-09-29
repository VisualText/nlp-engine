/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	LN.CPP
* FILE:	lite\ln.cpp
* CR:		10/11/98 AM.
* SUBJ:	Parse tree of nodes, each dedicated to one line of text.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"

#include "node.h"	// 07/07/03 AM.
#include "parse.h"
#include "io.h"

#include "ln.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Tree<Ln>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Node<Ln>::count_ = 0;
#else
int Tree<Ln>::count_ = 0;
int Node<Ln>::count_ = 0;
#endif
int Ln::count_ = -1;					// 11/17/98 AM. One static Ln.


/********************************************
* FN:		Special Functions for Ln Class
* CR:		10/08/98 AM.
********************************************/

Ln::Ln()
{
Start = End = -1;
#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ln::Ln(long start, long end, _TCHAR *text)
{
Start = start;
End = end;
Text = text;
#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ln::~Ln()
{
#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, const Ln &ln)
{
_TCHAR *buf;
long oend;
//buf = Current_Parse->getText();
buf = ln.Text;

// Print the text for this node.
_TCHAR *ptr, *end;
ptr = buf;
oend = ln.End - ln.Start;
end = &(buf[oend]);
//ptr = &(buf[ln.Start]);
//end = &(buf[ln.End]);
while (ptr != end)
	output << pretty_char(*ptr++);
output << pretty_char(*ptr);		// Output end char.

output
	<< _T(" [")
	<< ln.Start << _T(",")
	<< ln.End
	<< _T("]")
	;
return output;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
* CR:		10/08/98 AM.
********************************************/

long  Ln::getStart()			{ return Start; }
long  Ln::getEnd()			{ return End;   }
_TCHAR *Ln::getText()			{ return Text;  }


/********************************************
* FN:		Modify Functions
* CR:		10/08/98 AM.
********************************************/

void Ln::setStart(long x)			{ Start = x; }
void Ln::setEnd(long x)				{ End   = x; }
void Ln::setText(_TCHAR *x)			{ Text  = x; }


/********************************************
* FN:		SETSTARTEND
* CR:		10/08/98 AM.
********************************************/

void Ln::setStartEnd(long start, long end)
{
if (start < 0 || end < start)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[setStartEnd: Given bad value. start=")
		  << start << _T(" , end=") << end << _T("]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
Start = start;
End = end;
}


/********************************************
* FN:		GETCOUNT
* CR:		10/11/98 AM.
********************************************/
#ifndef STABLE_
int Ln::getCount() { return count_; }
#endif


/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ln::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ln count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ln count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		makeNode
* CR:		10/11/98 AM.
* SUBJ:	Create a node with appropriate data.
* NOTE:	10/12/98 AM. Making into a class function. Renamed from
*			makeLnode.
*			Must be a more elegant way to initialize the node.
********************************************/
static Ln p;

Node<Ln> *Ln::makeNode(long start, long end, _TCHAR *text)
{
Node<Ln> *node;

p.Start = start;
p.End = end;
p.Text = text;
node = new Node<Ln>(p);
return node;
}

/********************************************
* FN:		makeTree
* CR:		10/12/98 AM.
* SUBJ:	Create a tree with appropriate root data.
* NOTE:	Class member function. (Can be called without an instance existing).
********************************************/

Tree<Ln> *Ln::makeTree(
	long ostart,
	long oend,
	_TCHAR *text
	)
{
Node<Ln> *root;
root = makeNode(ostart, oend, text);
return new Tree<Ln>(root);
}
