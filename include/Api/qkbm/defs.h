/*******************************************************************************
Copyright (c) 2007-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DEFS.H
* FILE:	include/cs/qkbm/defs.h
* CR:		02/07/07 AM.
* SUBJ:	Basic items for LIBQKBM.
*******************************************************************************/

#ifndef DEFS_H_
#define DEFS_H_

// Changing these to long.  Presumably the QDBM or other underlying system
// should enable 64-bit identifiers.  QDBM now uses 32-bit or "int.
// 02/13/07 AM.
typedef __int64 CON_ID;
typedef __int64 PTR_ID;
typedef __int64 STR_ID;

#define CNULL ((XCON_S *) NULL)

#endif
/*************************      END OF FILE      ******************************/
