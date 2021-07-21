###############################################
# FILE: XML baseDefs.pat                      #
# SUBJ: REcognizing processing instr's &      #
#       other base elements                   #
# AUTH: Paul Deane                            #
# CREATED: 10/Jan/01 22:43:13                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@POST
single() ;
@@POST
@RULES

_XMLDecl <-
    _StartXML	### (1)
	_xWILD [min=0 max=0 fail=("_EndProcessingInstruction")]
	_EndProcessingInstruction ### (3)
    @@

_doctypedecl [unsealed] <-
	_StartDocType
	_whiteSpace [opt]
	_xWILD [min=0 max=0 fail=("_EndTag" "[")]
	_EndTag
	@@

_doctypedecl <-
	_StartDocType
	_whiteSpace [opt]
	_xWILD [min=0 max=0 fail=("_EndDocType" "[")]
	_whiteSpace [opt]
	\[
	_whiteSpace [opt]
	_xWILD [min=0 max=0 fail=("_EndDocType")]
 	_EndDocType
	@@

_ProcessingInstruction <-
	_StartProcessingInstruction
	_xWILD [min=0 max=0 fail=("_EndProcessingInstruction")]
	_EndProcessingInstruction
	@@

_CDataSection <-
	_CDStart
	_xWILD [min=0 max=0 fail=("_CDEnd")]
	_CDEnd
	@@
		
@@RULES
