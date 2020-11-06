###############################################
# FILE: XML attdefs3.pat                      #
# SUBJ: XML attribute definitions ctd. for doc#
# AUTH: Paul Deane                            #
# CREATED: 12/Jan/01 08:07:01                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _doctypedecl _AttlistDecl

@POST
	S("buffer1") = str(N("$text",2)) ;
	S("buffer2") = str(N("$text",3)) ;
	if (N("$text",2) && N("$text",3)) {
		S("AttributeName") = S("buffer1") + S("buffer2") ;
	}
	else if (N("$text",2))
		S("AttributeName") = S("buffer1") ;
	else if (N("$text",3))
		S("AttributeName") = S("buffer2") ;
	S("AttribType") = S("AttributeName") + "Type" ;
	G("currentConcept") = findconcept(G("Elements"),X("ElementName",3));
	G("currentattr") = findattr(G("currentConcept"),S("AttributeName")) ;
	if (G("currentattr")==0) {
		addattr(G("currentConcept"),S("AttributeName")) ;
		addstrval(G("currentConcept"),S("AttributeName"),N("$text",7)) ;
		addattr(G("currentConcept"),S("AttribType")) ;
		addstrval(G("currentConcept"),S("AttribType"),N("$text",5)) ;
	}
	single() ;
@@POST
@RULES
_AttDef <-
    _whiteSpace [opt] ### (1)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _AttType [one] ### (5)
    _whiteSpace [opt] ### (6)
    _DefaultDecl [one] ### (7)
    @@
@@RULES	