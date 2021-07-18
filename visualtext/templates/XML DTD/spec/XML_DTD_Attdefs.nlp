###############################################
# FILE: XML DTD attdefs.pat                   #
# SUBJ: XML attribute definitions for dtd     #
# AUTH: Paul Deane                            #
# CREATED: 12/Jan/01 00:22:26                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _AttlistDecl

@RULES

_DefaultDecl <-
    \# [one] ### (1)
    _xWILD [s one matches=("REQUIRED" "IMPLIED")] ### (2)
    @@
	
_DefaultDecl <-
    \# [one] ### (1)
    _xWILD [s one matches=("FIXED")] ### (2)
    _whiteSpace [opt] ### (3)
    _PERefence [one] ### (4)
    @@
	
_DefaultDecl <-
    \# [one] ### (1)
    _xWILD [s one matches=("FIXED")] ### (2)
    _whiteSpace [opt] ### (3)
    _PubidLiteral [one] ### (4)
    @@
	
_DefaultDecl <-
    \# [one] ### (1)
    _xWILD [s one matches=("FIXED")] ### (2)
    _whiteSpace [opt] ### (3)
    _SystemLiteral [one] ### (4)
    @@
	
_DefaultDecl <-
    _PubidLiteral [one] ### (1)
    @@

_DefaultDecl <-
    _SystemLiteral [one] ### (1)
    @@
		

_AttType <-
    _xWILD [s one matches=("CDATA" "ID" "IDREF" "IDREFS" "ENTITY" "ENTITIES" "NMTOKEN" "NMTOKENS")] ### (1)
    @@
		
_EnumNameElement <-
    _whiteSpace [opt] ### (1)
    \| [one] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s one matches=("_xALPHA" "_" ":")] ### (4)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (5)
    @@

_EnumElement <-
    _whiteSpace [opt] ### (1)
    \| [one] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s plus matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (4)
    @@
