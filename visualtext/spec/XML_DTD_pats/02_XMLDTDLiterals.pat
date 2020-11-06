###############################################
# FILE: XML DTD Literals.pat                  #
# SUBJ: quoted elements in an XML DTD         #
# AUTH: Paul Deane                            #
# CREATED: 17/Jan/01 13:13:13                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@RULES
_Comment <-
    _CommentStart [one] ### (1)
    _xWILD [star fail=("_CommentEnd" "_CommentEnd" "_DoubleHyphen")] ### (2)
    _CommentEnd [one] ### (3)
    @@

@POST
	S("textValue") = N("$text",2) ;
	single() ;
@@POST
@RULES
_PubidLiteral <-
    \" [one] ### (1)
    _xWILD [star matches=( "_xALPHA"  "_xNUM" \ \- \' \( \) \+ \, \. \/ \: \= \? \; \! \* \# \@ \$ \_ \% )] ### (2)
    \" [one] ### (3)
    @@

_PubidLiteral <-
    \' [one] ### (1)
    _xWILD [star matches=( "_xALPHA"  _xNUM \ \- \( \) \+ \, \. \/ \: \= \? \; \! \* \# \@ \$ \_ \% )] ### (2)
    \' [one] ### (3)
    @@

_SystemLiteral <-
    \" [one] ### (1)
    _xWILD [star fails=("\"")] ### (2)
    \" [one] ### (3)
    @@

_SystemLiteral <-
    \' [one] ### (1)
    _xWILD [star fails=("'")] ### (2)
    \' [one] ### (3)
    @@
@@RULES
@RULES
_whiteSpace <-
    _xWHITE [plus] ### (1)
    @@
	
@@RULES


