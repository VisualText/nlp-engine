###############################################
# FILE: XML declSep.pat                       #
# SUBJ: IDentify declaration separators       #
#       inside an XML document                #
# AUTH: Paul Deane                            #
# CREATED: 14/Jan/01 20:00:39                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@RULES

_DeclSep <-
    _PEReference [one] ### (1)
    @@

_DeclSep <-
    _whiteSpace [one] ### (1)
    @@

@@RULES
