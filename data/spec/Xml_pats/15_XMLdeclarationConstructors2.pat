###############################################
# FILE: XML declarationConstructors2.pat      #
# SUBJ: Build entity declarations in an XML   #
#       document type declaration             #
# AUTH: Paul Deane                            #
# CREATED: 11/Jan/01 13:14:36                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _doctypedecl

@POST
	S("buf") = "Grammar" ;
	S("Grammar") = str(N("$text",3)) ;
	G("CurrentConcept") = findconcept(G("Elements"),N("ElementName",1)) ;
	G("attr") = findattr(G("CurrentConcept"),S("buf")) ;
       if (G("attr") == 0 ) {
		addattr(G("CurrentConcept"),S("buf")) ;
		addstrval(G("CurrentConcept"),S("buf"),S("Grammar")) ;
	}
	G("attrName") = "sortedYet" ;
	G("attr") = findattr(G("CurrentConcept"),G("attrName")) ;
       if (G("attr") == 0 ) {
		addattr(G("CurrentConcept"),G("attrName")) ;
		addsval(G("CurrentConcept"),G("attrName"),0) ;
	}
	single() ;
@@POST
@RULES
_ElementDecl <-
    _ElementDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [star matches=("EMPTY" "ANY")] ### (3)
    _whiteSpace [opt] ### (4)
    _EndTag [one] ### (5)
    @@

_ElementDecl <-
    _ElementDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _Mixed [one] ### (3)
    _whiteSpace [opt] ### (4)
    _EndTag [one] ### (5)
    @@

_ElementDecl <-
    _ElementDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _choice [one] ### (3)
    _xWILD [opt matches=("?" "*" "+")] ### (4)
    _whiteSpace [opt] ### (5)
    _EndTag [one] ### (6)
    @@


_ElementDecl <-
    _ElementDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _seq [one] ### (3)
    _xWILD [opt matches=("?" "*" "+")] ### (4)
    _whiteSpace [opt] ### (5)
    _EndTag [one] ### (6)
    @@
@@RULES
@RULES
_seq  <-
    \( [one] ### (1)
    _whiteSpace [opt] ### (2)
    _cp [one] ### (3)
    _seqElement [plus] ### (4)
    _whiteSpace [opt] ### (5)
    \) [one] ### (6)
    _xWILD [opt matches=("*" "+" "?")] ### (7)
    @@

_seq <-
    \( [one] ### (1)
    _whiteSpace [opt] ### (2)
    _choice [one] ### (3)
    _seqElement [plus] ### (4)
    _whiteSpace [opt] ### (5)
    \) [one] ### (6)
    _xWILD [opt matches=("*" "+" "?")] ### (7)
    @@
	
_seq <-
    \( ### (1)
    _whiteSpace [opt] ### (2)
    _seq [one] ### (3)
    _seqElement [plus] ### (4)
    _whiteSpace [opt] ### (5)
    \) [one] ### (6)
	_xWILD [opt matches=("*" "+" "?")] ### (7)
    @@

_seqElement <-
    _whiteSpace [opt] ### (1)
    \, [one] ### (2)
    _whiteSpace [opt] ### (3)
    _choice [one] ### (4)
    @@

_choice <-
	\( [one] ### (1)
	_whiteSpace [opt] ### (2)
	_cp [one] ### (3)
	_choiceElement [star] ### (4)
	_whiteSpace [opt] ### (5)
	\) [one] ### (6)
	_xWILD [opt matches=("*" "+" "?")] ### (7)
	@@

_choice <- 
    \( [one] ### (1)
    _whiteSpace [opt] ### (2)
    _choice [one] ### (3)
    _choiceElement [plus] ### (4)
    _whiteSpace [opt] ### (5)
    \) [one] ### (6)
    _xWILD [opt matches=("*" "+" "?")] ### (7)
    @@
	
_choice <-
    \( [one] ### (1)
    _whiteSpace [opt] ### (2)
    _seq [one] ### (3)
    _choiceElement [plus] ### (4)
    _whiteSpace [opt] ### (5)
    \) [one] ### (6)
    _xWILD [opt matches=("*" "+" "?")] ### (7)
    @@

_choiceElement <-
    _whiteSpace [opt] ### (1)
    \| [one] ### (2)
    _whiteSpace [opt] ### (3)
    _seq [one] ### (4)
    @@

_seqElement <-
    _whiteSpace [opt] ### (1)
    \, [one] ### (2)
    _whiteSpace [opt] ### (3)
    _seq [one] ### (4)
    @@

_choiceElement <-
    _whiteSpace [opt] ### (1)
    \| [one] ### (2)
    _whiteSpace [opt] ### (3)
    _choice [one] ### (4)
    @@

@@RULES
