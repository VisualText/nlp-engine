/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#ifndef LINUX

#if !defined(AFX_STDAFX_H__1AE1F752_88BC_11D2_9E65_000000000000__INCLUDED_)
#define AFX_STDAFX_H__1AE1F752_88BC_11D2_9E65_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <tchar.h>
#include <my_tchar.h>

#include <streamclasses.h>

#ifdef UNICODE
//#define gerrStr(a,b,c) gerrStr(a,c)
//#define gerrStr1(a,b,c) gerrStr1(a,c)
//#define gerrStr2(a,b,c) gerrStr2(a,c)
//#define gerrStr3(a,b,c) gerrStr3(a,c)
#define outStr(a,b,c) outStr(a,c)
#define ocbuf(a,b,c) ocbuf(a,c)
#elif LINUX
#define outStr(a,b,c) outStr(a,c)
#define ocbuf(a,b,c) ocbuf(a,c)
#endif

#ifndef LINUX
// Better C++ debug info. // 09/22/08 AM.
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1AE1F752_88BC_11D2_9E65_000000000000__INCLUDED_)

#endif
