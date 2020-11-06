#################################################
# FILE: XML StartingTag.pat                     #
# SUBJ: Find attributes within an XML start tag #
# AUTH: Paul Deane                              #
# CREATED: 14/Jan/01 14:32:27                   #
# DATE OF THIS VERSION: 31/Aug/01               #
# Copyright 2001, Text Analysis International   #
#################################################

@NODES _StartingTag

@RULES

_Attribute <-
    _xWILD [s one matches=("_xALPHA" "_" ":")]	### (1)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]	### (2)
    _whiteSpace [opt] ### (3)
    \= [one] ### (4)
    _whiteSpace [opt] ### (5)
    _PubidLiteral [one] ### (6)
    @@
_Attribute <-
    _xWILD [s one matches=("_xALPHA" "_" ":")]	### (1)    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]	### (2)
    _whiteSpace [opt] ### (3)
    \= [one] ### (4)
    _whiteSpace [opt] ### (5)
    _SystemLiteral [one] ### (6)
    @@


@@RULES
