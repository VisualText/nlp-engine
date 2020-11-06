###############################################
# FILE: XML Literals.pat                      #
# SUBJ: Define quoted strings in XML doc      #
# AUTH: Paul Deane                            #
# CREATED: 17/Jan/01 13:13:13                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@RULES

_Comment <-
    _CommentStart [one] ### (1)
    _xWILD [min=0 max=0 fail=("_CommentEnd" "_CommentEnd" "_DoubleHyphen")] ### (2)
    _CommentEnd [one] ### (3)
    @@

@POST
	S("textValue") = N("$text",2) ;
	single() ;
@@POST
@RULES
_PubidLiteral <-
    \" [one] ### (1)
    _xWILD [min=0 max=0 matches=( "_xALPHA"  "_xNUM" \ \- \' \( \) \+ \, \. \/ \: \= \? \; \! \* \# \@ \$ \_ \% )] ### (2)
    \" [one] ### (3)
    @@

_PubidLiteral <-
    \' [one] ### (1)
    _xWILD [min=0 max=0 matches=( "_xALPHA"  _xNUM \ \- \( \) \+ \, \. \/ \: \= \? \; \! \* \# \@ \$ \_ \% )] ### (2)
    \' [one] ### (3)
    @@

_SystemLiteral <-
    \" [one] ### (1)
    _xWILD [min=0 max=0 fails=("\"")] ### (2)
    \" [one] ### (3)
    @@

_SystemLiteral <-
    \' [one] ### (1)
    _xWILD [min=0 max=0 fails=("'")] ### (2)
    \' [one] ### (3)
    @@
@@RULES
@RULES
_whiteSpace <-
    _xWHITE [plus] ### (1)
    @@
	
@@RULES


