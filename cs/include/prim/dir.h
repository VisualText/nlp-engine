#ifndef NLP_CS_INCLUDE_PRIM_DIR_H
#define NLP_CS_INCLUDE_PRIM_DIR_H

/*******************************************************************************
Copyright (c) 1999 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	DIR.H
* FILE:	include/prim/dir.cpp
* CR:		05/06/99 AM.
* SUBJ:	Functions for handling directories.
*
*******************************************************************************/

LIBPRIM_API bool
make_dir(_TCHAR *dir);
LIBPRIM_API bool
rm_dir(_TCHAR *dir);

#endif // NLP_CS_INCLUDE_PRIM_DIR_H
