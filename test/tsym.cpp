#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"	// Linux.	// 04/26/07 AM.
#include "lite/inline.h"	// Linux.	// 04/26/07 AM.
#include "io.h"	// Linux.	// 04/26/07 AM.
#include "node.h"				// 07/07/03 AM.
#include "tree.h"	// Linux.	// 04/26/07 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "slist.h"
#include "tsym.h"
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "thtab.h"
#include "irule.h"

// C++ has a yucky feature. The space between brackets is needed.
template<> int Slist<tSym<void> >::count_ = 0;
template<> int Selt<tSym<void> >::count_ = 0;
template<> int tSym<void> ::count_ = 0;



template<> int tHtab<void>::count_ = 0;
template<> Stab *tHtab<void>::gstab_ = 0;				// 12/12/98 AM.

template<> int tSym<Slist<Irule> >::count_ = 0;
template<> int Selt<tSym<Slist<Irule> > >::count_ = 0;
template<> int Slist<tSym<Slist<Irule> > >::count_ = 0;

template<> int tHtab<Slist<Irule> >::count_ = 0;

