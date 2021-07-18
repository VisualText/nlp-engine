################################################
# FILE: EntityDefs.pat                         #
# SUBJ: XML Entity Definitions                 #
# AUTH: Paul Deane                             #
# CREATED: 12/Jan/01 08:46:40                  #
# Date of this version: 31/Aug/01              #
# Copyright 2001, Text Analysis International  #
################################################

@PATH _ROOT _doctypedecl _EntityDecl


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
		_whiteSpace [opt]
		_xWILD [s trig min=1 max=1 matches=("\%")]
		_whiteSpace
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_ExternalID
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
		_whiteSpace [opt]
		_xWILD [s trig min=1 max=1 matches=("\%")]
		_whiteSpace
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_EntityRef
@@

_PEDecl <- 
		_whiteSpace [opt]
		_xWILD [s trig min=1 max=1 matches=("\%")]
		_whiteSpace
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_PEReference
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
		_whiteSpace [opt]
		_xWILD [s trig min=1 max=1 matches=("\%")]
		_whiteSpace
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_PubidLiteral
@@

_PEDecl <- 
		_whiteSpace [opt]
		_xWILD [s trig min=1 max=1 matches=("\%")]
		_whiteSpace
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_SystemLiteral
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
		_whiteSpace [opt]
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_PubidLiteral
@@
_GEDecl <- 
		_whiteSpace [opt]
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_SystemLiteral
@@
_GEDecl <- 
		_whiteSpace [opt]
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_EntityRef
@@

_GEDecl <- 
		_whiteSpace [opt]
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_PEReference
@@
	
_GEDecl <- 
		_whiteSpace [opt]
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_PubidLiteral
@@

_GEDecl <- 
		_whiteSpace [opt]
		_xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")]
		_xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]
		_whiteSpace
		_SystemLiteral
@@

@@RULES

