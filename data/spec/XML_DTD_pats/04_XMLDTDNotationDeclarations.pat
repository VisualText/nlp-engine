###############################################
# FILE: XML DTD NotationDeclarations.pat      #
# SUBJ: XML notation declarations for xml dtd #
# AUTH: Paul Deane                            #
# CREATED: 14/Jan/01 12:13:12                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@RULES

_NotationDecl <-
    _NotationDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [min=1 max=0 fail=("_EndTag")] ### (3)
    _whiteSpace [opt] ### (4)
    _EndTag [one] ### (5)
    @@

_NDataDecl <-
    _whiteSpace [opt] ### (1)
    _xWILD [one matches=("NDATA")] ### (2)
    _whiteSpace [one] ### (3)
    _xWILD [s one matches=("_xALPHA" "_" ":")]	### (4)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]	### (5)
    @@
