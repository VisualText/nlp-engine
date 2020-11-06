###############################################
# FILE: XML tagElements.pat                   #
# SUBJ: Combine the elements of an XML tag    #
# AUTH: Paul Deane                            #
# CREATED: 13/Jan/01 23:27:37                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@RULES
_EmptyTag  <-
    \< [one] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _xWILD [star fails=("_EndEmptyTag" "_EndTag")] ### (5)
    _EndEmptyTag [one] ### (6)
    @@
@@RULES	

@POST
	S("buffer1") = str(N("$text",2)) ;
	S("buffer2") = str(N("$text",3)) ;
	if (N("$text",2) && N("$text",3)) {
		S("tagName") = S("buffer1") + S("buffer2") ;
	}
	else if (N("$text",2))
		S("tagName") = S("buffer1") ;
	else if (N("$text",3))
		S("tagName") = S("buffer2") ;
	G("ReferenceIDforConcept") = findconcept(G("Elements"),S("tagName")) ;
	if (G("ReferenceIDforConcept") == 0 )
		makeconcept(G("Elements"),S("tagName")) ;
	single() ;
@@POST
@RULES
_StartingTag  <-
    \< [one] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _xWILD [star fails=("_EndTag")] ### (5)
    _EndTag [one] ### (6)
    @@
@@RULES	
@POST
	S("buffer1") = str(N("$text",3)) ;
	S("buffer2") = str(N("$text",4)) ;
	if (N("$text",3) && N("$text",4)) {
		S("tagName") = S("buffer1") + S("buffer2") ;
	}
	else if (N("$text",3))
		S("tagName") = S("buffer1") ;
	else if (N("$text",4))
		S("tagName") = S("buffer2") ;
	G("ReferenceIDforConcept") = findconcept(G("Elements"),S("tagName")) ;
	if (G("ReferenceIDforConcept") == 0 )
		makeconcept(G("Elements"),S("tagName")) ;
	single() ;
@@POST
@RULES
_ClosingTag  <-
    \< [one] ### (1)
    \/ [one] ### (2)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (3)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (4)
    _whiteSpace [opt] ### (5)
    _xWILD [star fails=("_EndTag")] ### (6)
    _EndTag [one] ### (7)
    @@
@@RULES	
