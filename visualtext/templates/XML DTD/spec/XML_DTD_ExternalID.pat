###############################################
# FILE: XML DTD ExternalID.pat                #
# SUBJ: Rules for external IDs in XML dtd     #
# AUTH: Paul Deane                            #
# CREATED: 16/Jan/01 11:00:24                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _EntityDecl

@POST
	S("public")=1 ;
	S("URI") = N("textValue",3) ;
	single() ;
@@POST
@RULES
_ExternalID <-
    _xWILD [one matches=("PUBLIC")] ### (1)
    _whiteSpace [one] ### (2)
    _PubidLiteral [one] ### (3)
    _whiteSpace [one] ### (4)
    _PubidLiteral [one] ### (5)
    @@
	
_ExternalID <-
    _xWILD [one matches=("PUBLIC")] ### (1)
    _whiteSpace [one] ### (2)
    _PubidLiteral [one] ### (3)
    _whiteSpace [one] ### (4)
    _SystemLiteral [one] ### (5)
    @@
@@RULES
@POST
	S("public")=0 ;
	S("URI") = N("textValue",3) ;
	single() ;
@@POST
@RULES
_ExternalID <-
    _xWILD [min=1 max=1 matches=("SYSTEM")] ### (1)
    _whiteSpace [opt] ### (2)
    _PubidLiteral [one] ### (3)
    @@
	
_ExternalID <-
    _xWILD [min=1 max=1 matches=("SYSTEM")] ### (1)
    _whiteSpace [opt] ### (2)
    _SystemLiteral [one] ### (3)
    @@
@@RULES
		