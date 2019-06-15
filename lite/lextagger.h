/*******************************************************************************
Copyright (c) 2002-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:  LEXTAGGER.H
* FILE:  lite/lextagger.h
* CR:    08/27/02 AM.
*
* SUBJ:	Class to manage UMIST LexTagger, a POS tagger.
*
*
*******************************************************************************/

#ifndef LEXTAGGER_H_
#define LEXTAGGER_H_


// Forward reference.
class Nlppp;

/********************************************
* CLASS:	LEXTAGGER
* CR:		12/11/03 AM.
* SUBJ:	Modularity class for LexTagger POS tagger.
********************************************/

class LITE_API LexTagger
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	LexTagger();									// Default constructor.
	~LexTagger();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static bool posParse(_TCHAR*,Nlppp*);
	static bool posUpdate(_TCHAR*,_TCHAR*,_TCHAR*,Nlppp*,/*DU*/Node<Pn>*&);
	static bool postagger(Nlppp*);

};

#endif
/*************************      END OF FILE      ******************************/
