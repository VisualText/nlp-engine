/*******************************************************************************
Copyright © 2003-2009 Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME: VTAPI.H
* FILE: include/Api/vtapi.h
* CR:   01/31/03 AM.
* SUBJ: API include files for running an analyzer standalone or embedded.
* NOTE: Taken from user/main.cpp.
*
*******************************************************************************/

#ifndef VTAPI_H_
#define VTAPI_H_

#include "machine.h"

#ifndef LINUX
#include <tchar.h>
#include <my_tchar.h>
#include <streamclasses.h>
#endif

#include "prim/libprim.h"                                      // 08/14/02 AM.
#include "prim/list_s.h"                                       // 08/14/02 AM.
#include "prim/list.h"                                         // 08/14/02 AM.
#include "lite/mach.h"
#include "lite/global.h"
#include "lite/inline.h"
#include "lite/nlp.h"    // ANALYZER API.
#include "lite/slist.h"                                        // 01/06/03 AM.
#include "lite/vtrun.h"                                        // 08/27/02 AM.
#include "consh/libconsh.h"
#include "consh/cg.h"   // KNOWLEDGE BASE API.
#include "lite/dir.h"
#include "lite/rug.h"   // RULE GENERATION API.
#include "lite/Aseq.h"

#endif
