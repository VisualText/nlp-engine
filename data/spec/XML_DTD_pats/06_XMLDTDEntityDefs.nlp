###############################################
# FILE: XML DTD EntityDefs.pat                #
# SUBJ: Entity definitions for xml dtds       #
# AUTH: Paul Deane                            #
# CREATED: 12/Jan/01 08:46:40                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _EntityDecl

@POST
	if (N("$text",4))
		S("buffer1") = str(N("$text",4)) ;
	if (N("$text",5))
		S("buffer2") = str(N("$text",5)) ;
	if (S("buffer1") && S("buffer2")) {
		S("EntityName") = S("buffer1") + S("buffer2") ;
	}
	else if (S("buffer1"))
		S("EntityName") = S("buffer1") ;
	else if (S("buffer2"))
		S("EntityName") = S("buffer2") ;
	G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	if (G("CurrentEntity") == 0 ) {
		makeconcept(G("Entities"),S("EntityName")) ;
		G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	}
	addattr(G("CurrentEntity"),"parameter") ;
	addsval(G("CurrentEntity"),"parameter",1) ;
	addattr(G("CurrentEntity"),"internal") ;
	addsval(G("CurrentEntity"),"internal",0) ;
	addattr(G("CurrentEntity"),"textValue") ;
	addattr(G("CurrentEntity"),"URI") ;
	addstrval(G("CurrentEntity"),"URI",str(N("URI",7))) ;
      single() ;
@@POST
@RULES
_PEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s trig min=1 max=1 matches=("\%")] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")] ### (4)
    _xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (5)
    _whiteSpace [opt] ### (6)
    _ExternalID [one] ### (7)
    @@
@@RULES

@POST
	if (N("$text",4))
		S("buffer1") = str(N("$text",4)) ;
	if (N("$text",5))
		S("buffer2") = str(N("$text",5)) ;
	if (S("buffer1") && S("buffer2")) {
		S("EntityName") = S("buffer1") + S("buffer2") ;
	}
	else if (S("buffer1"))
		S("EntityName") = S("buffer1") ;
	else if (S("buffer2"))
		S("EntityName") = S("buffer2") ;
	G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	if (G("CurrentEntity") == 0 ) {
		makeconcept(G("Entities"),S("EntityName")) ;
		G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	}
	addattr(G("CurrentEntity"),"parameter") ;
	addsval(G("CurrentEntity"),"parameter",1) ;
	addattr(G("CurrentEntity"),"internal") ;
	addsval(G("CurrentEntity"),"internal",1) ;
	addattr(G("CurrentEntity"),"textValue") ;
	addstrval(G("CurrentEntity"),"textValue",str(N("textValue",7))) ;
      single() ;
@@POST
@RULES
_PEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s trig one matches=("\%")] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (4)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (5)
    _whiteSpace [opt] ### (6)
    _EntityRef [one] ### (7)
    @@

_PEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s trig min=1 max=1 matches=("\%")] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")] ### (4)
    _xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (5)
    _whiteSpace [opt] ### (6)
    _PEReference [one] ### (7)
    @@

@@RULES

@POST
	if (N("$text",4))
		S("buffer1") = str(N("$text",4)) ;
	if (N("$text",5))
		S("buffer2") = str(N("$text",5)) ;
	if (S("buffer1") && S("buffer2")) {
		S("EntityName") = S("buffer1") + S("buffer2") ;
	}
	else if (S("buffer1"))
		S("EntityName") = S("buffer1") ;
	else if (S("buffer2"))
		S("EntityName") = S("buffer2") ;
	G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	if (G("CurrentEntity") == 0 ) {
		makeconcept(G("Entities"),S("EntityName")) ;
		G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	}
	addattr(G("CurrentEntity"),"parameter") ;
	addsval(G("CurrentEntity"),"parameter",1) ;
	addattr(G("CurrentEntity"),"internal") ;
	addsval(G("CurrentEntity"),"internal",1) ;
	addattr(G("CurrentEntity"),"textValue") ;
	addstrval(G("CurrentEntity"),"textValue",str(N("textValue",7))) ;
      single() ;
@@POST
@RULES

_PEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s trig one matches=("\%")] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")] ### (4)
    _xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (5)
    _whiteSpace [opt] ### (6)
    _PubidLiteral [one] ### (7)
    @@

_PEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s trig min=1 max=1 matches=("\%")] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")] ### (4)
    _xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (5)
    _whiteSpace [opt] ### (6)
    _SystemLiteral [one] ### (7)
    @@

@@RULES
@POST
	if (N("$text",2))
		S("buffer1") = str(N("$text",2)) ;
	if (N("$text",3))
		S("buffer2") = str(N("$text",3)) ;
	if (N("$text",2) && N("$text",3)) {
		S("EntityName") = S("buffer1") + S("buffer2") ;
	}
	else if (N("$text",2))
		S("EntityName") = S("buffer1") ;
	else if (N("$text",3))
		S("EntityName") = S("buffer2") ;
	G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	if (G("CurrentEntity") == 0 ) {
		makeconcept(G("Entities"),S("EntityName")) ;
		G("CurrentEntity") = findconcept(G("Entities"),S("EntityName")) ;
	}
	addattr(G("CurrentEntity"),"parameter") ;
	addsval(G("CurrentEntity"),"parameter",0) ;
	addattr(G("CurrentEntity"),"internal") ;
	addsval(G("CurrentEntity"),"internal",1) ;
	addattr(G("CurrentEntity"),"textValue") ;
	addstrval(G("CurrentEntity"),"textValue",str(N("textValue",5))) ;
      single() ;
@@POST
@RULES

_GEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _PubidLiteral [one] ### (5)
    @@
	
_GEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _SystemLiteral [one] ### (5)
    @@
	
_GEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _EntityRef [one] ### (5)
    @@

_GEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _PEReference [one] ### (5)
    @@
	
_GEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s opt matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _PubidLiteral [one] ### (5)
    @@

_GEDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s opt matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _SystemLiteral [one] ### (5)
    @@

@@RULES

