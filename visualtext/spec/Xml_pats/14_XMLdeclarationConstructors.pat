###############################################
# FILE: XML declarationConstructors.pat       #
# SUBJ: Build the basic elements of an entity #
#       declaration in an XML document        #
# AUTH: Paul Deane                            #
# CREATED: 11/Jan/01 13:11:50                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _doctypedecl

@RULES
_choiceElement [unsealed] <-
    _whiteSpace [opt]	### (1)
    \|	[one] ### (2)
    _whiteSpace [opt]	### (3)
    _cp [one]	### (4)
    _xWILD [opt matches=("*" "+" "?")]	### (5)
    @@

_seqElement [unsealed] <-
    _whiteSpace [opt] ### (1)
    \, [one] ### (2)
    _whiteSpace [opt] ### (3)
    _cp [one] ### (4)
    _xWILD [opt matches=("*" "+" "?")] ### (5)
    @@
