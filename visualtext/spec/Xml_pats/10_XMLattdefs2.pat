###############################################
# FILE: XML attdefs2.pat                      #
# SUBJ: XML attribute definitions ctd.        #
# AUTH: Paul Deane                            #
# CREATED: 12/Jan/01 07:38:44                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _doctypedecl _AttlistDecl

@RULES

_AttType [unsealed] <-
    _xWILD [s one matches=("NOTATION")] ### (1)
    _whiteSpace [opt] ### (2)
    \( [one]  ### (3)
    _whiteSpace [opt] ### (4)
    _xWILD [s min=1 max=1 matches=("_xALPHA" "_" ":")] ### (5)
    _xWILD [s min=0 max=0 matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (6)
    _xWHITE [opt] ### (7)
    _EnumNameElement [min=0 max=0] ### (8)
    \) [one] ### (9)
    @@

_AttType [unsealed] <-
    \( [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [s plus matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _EnumElement [star] ### (5)
    \) [one] ### (6)
    @@


_AttType [unsealed] <-
    \( [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [s plus matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
    _whiteSpace [opt] ### (4)
    _EnumNameElement [star] ### (5)
    \) [one] ### (6)
    @@
