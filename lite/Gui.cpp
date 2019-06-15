/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/Gui.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GUI::GUI()
{
	Init();
}

GUI::~GUI()
{

}

void GUI::Init()
{
	m_message = GUI_MESSAGE_IDLE;
	m_status = GUI_STATUS_NORMAL;
	SetLabelString(_T(""));
	SetValueString(_T(""));
}

LITE_API void GUI::Abort()
{
	m_message = GUI_MESSAGE_ABORT;
}

LITE_API void GUI::SetMessage(GUI_MESSAGE message)
{
	m_message = message;
}

LITE_API GUI_MESSAGE GUI::GetMessage()
{
	return m_message;
}

int GUI::IsMessage(GUI_MESSAGE message)
{
	return m_message == message;
}

LITE_API void GUI::SetStatus(GUI_STATUS status)
{
	m_status = status;
}

LITE_API GUI_STATUS GUI::GetStatus()
{
	return m_status;
}

LITE_API int GUI::IsStatus(GUI_STATUS status)
{
	return m_status == status;
}

LITE_API void GUI::SetLabelString(_TCHAR *labelString)
{
	_tcsnccpy(m_labelString,labelString,GUI_STRING_MAX-1);
}

void GUI::SetValueString(_TCHAR *valueString)
{
	_tcsnccpy(m_valueString,valueString,GUI_STRING_MAX-1);
}

#ifdef JUNK_
void GUI::SendMessage(_TCHAR *labelString, _TCHAR *valueString)
{
	SetLabelString(labelString);
	SetValueString(valueString);

	SetMessage(GUI_MESSAGE_WAITING);

	while (m_message != GUI_MESSAGE_DISPLAYED &&
		   m_message != GUI_MESSAGE_ABORT)
		PeekHere();

	SetMessage(GUI_MESSAGE_IDLE);

	PeekHere();
}
#endif

void GUI::ReceiveMessage(_TCHAR *labelString, _TCHAR *valueString)
{
	_tcsnccpy(labelString,m_labelString,GUI_STRING_MAX-1);
	_tcsnccpy(valueString,m_valueString,GUI_STRING_MAX-1);
}
