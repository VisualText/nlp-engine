###############################################
# FILE: XML DTD declarationElements.pat       #
# SUBJ: Identify basic tagging elements in    #
#       an XML declaration                    #
# AUTH: Paul Deane                            #
# CREATED: 11/Jan/01 12:28:55                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@POST
	G("CurrentElementName") = str(N("ElementName",1)) ;
	noop() ;
@@POST
@RULES
_xNIL <-
    _ElementDeclStart ### (1)
    @@
@@RULES
@POST
	G("CurrentElementName") = 0 ;
	noop() ;
@@POST
@RULES
_xNIL <-
    _EndTag ### (1)
    @@
@@RULES


@RULES
		
_Mixed <-
		_PCDataStart
		_xWILD [min=1 max=0 fail=("\)")]
		_whiteSpace [opt]
		\)
		\*
		@@		

_Mixed <-
		_PCDataStart
		_whiteSpace [opt]
		\)
		@@	

@@RULES
@CHECK
	if (strequal(str(N("$text",1)),"EMPTY") ||
	    strequal(str(N("$text",1)),"ANY")) {
		fail() ;
	}
@@CHECK
@POST
	S("buffer1") = str(N("$text",1)) ;
	S("buffer2") = str(N("$text",2)) ;
	if (N("$text",1) && N("$text",2)) {
		S("ChildElementName") = S("buffer1") + S("buffer2") ;
	}
	else if ( N("$text",1)) {
		S("ChildElementName") = S("buffer1") ;
	}
	else if ( N("$text",2)) {
		S("ChildElementName") = S("buffer2") ;
	}
	G("CurrentConcept") = findconcept(G("Elements"),G("CurrentElementName")) ;
	G("CurrentChildConcept") = findconcept(G("CurrentConcept"),S("ChildElementName")) ;
	if (G("CurrentChildConcept") == 0 ) {
		G("CurrentChildConcept") = makeconcept(G("CurrentConcept"),S("ChildElementName")) ;
		G("ReferenceIDforConcept") = findconcept(G("Elements"),S("ChildElementName")) ;
		if (G("ReferenceIDforConcept")==0) {
			makeconcept(G("Elements"),S("ChildElementName")) ;
		}
	}
	single() ;
@@POST
@RULES
_cp <- 
	_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")] 
	_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
	_xWILD [s min=0 max=1 matches=("?" "*" "+")]
	@@

@@RULES

@RULES
_cp <-
	\( [one]							### (1)
    _PEReference [one]					### (2)
    \) [one]							### (3)
    _xWILD [opt match=("?" "+" "*")]	### (4)
    @@
@@RULES
	