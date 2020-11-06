 ###############################################
 # FILE: XML SubSchema.pat                     #
 # SUBJ: Put together the major blocks of an   #
 #       XML Document                          #
 # AUTH: Paul Deane                            #
 # CREATED: 14/Jan/01 13:39:31                 #
 # DATE OF THIS VERSION: 31/Aug/01             #
 # Copyright 2001 Text Analysis International  #
 ###############################################

@NODES _ROOT

@RULES

_Prolog [unsealed] <-
	_declSep [opt]
	_XMLDecl [opt] ### (1)
	_xWILD [star matches=("_declSep" "_Comment" "ProcessingInstruction" "_whiteSpace")] ### (2)
	_doctypedecl [one] ### (3)
	_xWILD [star matches=("_declSep" "_Comment" "ProcessingInstruction" "_whiteSpace")] ### (4)
@@	

_Prolog [unsealed] <-
	_declSep [opt]
	_XMLDecl [one] ### (1)
	_xWILD [star matches=("_declSep" "_Comment" "ProcessingInstruction" "_whiteSpace")] ### (2)
	_doctypedecl [opt] ### (3)
	_xWILD [star matches=("_declSep" "_Comment" "ProcessingInstruction" "_whiteSpace")] ### (4)
@@	

_Misc <-
	_xWILD [plus matches=("_declSep" "_Comment" "ProcessingInstruction" "_whiteSpace")] ### (1)
@@


