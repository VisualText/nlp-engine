/*******************************************************************************
Copyright (c) 1998-2003 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	UCODE.H
* FILE:	c:\dev\nlplite\ucode.h
* CR:		12/08/98 AM.
* SUBJ:	User-defined code actions for the Pat pass.
* NOTE:	This class is a convenience for registering user-defined code
*			actions. The user
*			class has to be registered with NLP Lite.
*
*******************************************************************************/

#ifndef UMISC_H_
#define UMISC_H_
#include "lite/dlist.h"
#include "lite/code.h"


// Forward references
class Nlppp;

// Parse the output of a 3rd party POS tagger.
bool posParse(_TCHAR*,Nlppp*);                                   // 12/03/03 AM.
bool posUpdate(_TCHAR*,_TCHAR*,_TCHAR*,Nlppp*,/*DU*/NODE*&);         // 12/04/03 AM.
#endif
/*************************      END OF FILE      ******************************/
