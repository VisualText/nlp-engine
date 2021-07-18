###############################################
# FILE: XML PCDATA.pat                        #
# SUBJ: Identify a PCDATA section in XML doc  #
# AUTH: Paul Deane                            #
# CREATED: 11/Jan/01 15:25:54                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _doctypedecl _Mixed

@RULES
_childItem <-
    _whiteSpace [opt] ### (1)
    \| [one]	### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [s one matches=("_xALPHA" "_" ":")]	### (4)
    _xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")]	### (5)
    @@

_childItem <-
    _whiteSpace [opt] ### (1)
    \| [one]	### (2)
    _whiteSpace [opt] ### (3)
    _PEReference [one] ### (4)
    @@

@@RULES

@POST
	noop() ;
@@POST
@RULES
_xNIL <-
    _whiteSpace [one] ### (1)
    @@
@@RULES

@POST
	"output.txt" << "Error: stray characters in PCDATA section\n";
	noop() ;
@@POST
@RULES
_xNIL <-
    _xWILD ### (1)
    @@
@@RULES
