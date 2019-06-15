/*******************************************************************************
Copyright © 2001-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*/
// Gui.h: interface for the Gui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUI_H__24407ED1_A1D4_11D3_A4D4_00105A99059F__INCLUDED_)
#define AFX_GUI_H__24407ED1_A1D4_11D3_A4D4_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lite.h"

enum GUI_MESSAGE {
	GUI_MESSAGE_IDLE = 0,
	GUI_MESSAGE_BLOCKED,
	GUI_MESSAGE_WAITING,
	GUI_MESSAGE_RECEIVED,
	GUI_MESSAGE_DISPLAYED,
	GUI_MESSAGE_ABORT,
};

enum GUI_STATUS {
	GUI_STATUS_NORMAL = 0,
	GUI_STATUS_INITIALIZING,
	GUI_STATUS_ANALYZING,
	GUI_STATUS_GENERATING,
};

#define GUI_STRING_MAX 1001

class GUI  
{
public:
	GUI();
	~GUI();

	LITE_API GUI_MESSAGE GetMessage();
	LITE_API void SetMessage(GUI_MESSAGE message);
	LITE_API void Abort();
//	LITE_API void SendMessage(char *labelString, char *valueString);
	LITE_API void ReceiveMessage(_TCHAR *labelString, _TCHAR *valueString);
	LITE_API int IsMessage(GUI_MESSAGE message);
	LITE_API GUI_STATUS GetStatus();
	LITE_API void SetStatus(GUI_STATUS status);
	LITE_API int IsStatus(GUI_STATUS status);
	LITE_API void Init();

private:
	GUI_MESSAGE m_message;
	GUI_STATUS m_status;
	_TCHAR m_labelString[GUI_STRING_MAX];
	_TCHAR m_valueString[GUI_STRING_MAX];

	LITE_API void SetLabelString(_TCHAR *labelString);
	LITE_API void SetValueString(_TCHAR *valueString);
};

#endif // !defined(AFX_GUI_H__24407ED1_A1D4_11D3_A4D4_00105A99059F__INCLUDED_)
