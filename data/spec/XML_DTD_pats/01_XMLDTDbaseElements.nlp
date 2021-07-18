################################################
# FILE: XML DTD baseElements.pat               #
# SUBJ: Identify base elements such as entity  #
#       references and start and ends of tags  #
#       inside an XML DTD                      #
# AUTH: Paul Deane                             #
# CREATED: 11/Jan/01 06:53:04                  #
# Date of Current Version: 31/Aug/01           #
# Copyright 2001, Text Analysis International  #
################################################


 ###############################################
 # CONTENT INDEX                               #
 # 1. Rules for special items like ampersands  #
 #    greater than etc. plus tag elements      #
 # 2. Signals for special tag types including  #
 #    comments and entity references           #
 ###############################################

@CODE
	#Now we need to make sure that the doc type is in the KB
	#and properly identified for this document
	G("root") = findroot() ;
	G("tmp") = findconcept(G("root"),"tmp");
	G("gramtab") = getconcept(G("tmp"),"gram") ;
	G("ElementName") = "Elements" ;
	G("EntityName") = "Entities" ;
	#we get the doc type name from the name of the DTD file
	G("DocTypeName") =  G("$inputhead") ;
	G("CurrentDocType") = findconcept(G("gramtab"),G("DocTypeName")) ;
	if (G("CurrentDocType") == 0 ) {
		makeconcept(G("gramtab"),G("DocTypeName")) ;
		G("CurrentDocType") = findconcept(G("gramtab"),G("DocTypeName")) ;
	}
	G("Entities") = findconcept(G("CurrentDocType"),G("EntityName")) ;
	if (G("Entities") == 0 ) {
		makeconcept(G("CurrentDocType"),G("EntityName")) ;
		G("Entities") = findconcept(G("CurrentDocType"),G("EntityName")) ;
	}
	G("Elements") = findconcept(G("CurrentDocType"),G("ElementName")) ;
	if (G("Elements") == 0 ) {
		makeconcept(G("CurrentDocType"),G("ElementName")) ;
		G("Elements") = findconcept(G("CurrentDocType"),G("ElementName")) ;
	}
@@CODE

@PATH _ROOT

 ###################################
 # Rule set 1                      #
 # Special syntactic elements      #
 ################################### 

@RULES

_Ampersand <-
	\& [one] ### (1)
	_xWILD [one matches=("amp")] ### (2)
	\; [one] ### (3)
	@@

_LessThan <-
	\& [one] ### (1)
	_xWILD [one matches=("lt")] ### (2)
	\; [one] ### (3)
	@@

_GreaterThan <-
	\& [one] ### (1)
	_xWILD [one matches=("gt")] ### (2)
	\; [one] ### (3)
	@@

_APos <-
	\& [one] ### (1)
	_xWILD [one matches=("apos")] ### (2)
	\; [one] ### (3)
	@@

_Quote <-
	\& [one] ### (1)
	_xWILD [one matches=("quot")] ### (2)
	\; [one] ### (3)
	@@

_CommentStart <-
    \< [one] ### (1)
    \! [one] ### (2)
    \- [one] ### (3)
    \- [one] ### (4)
    @@
_CommentEnd <-
    \- [one] ### (1)
    \- [one] ### (2)
    \> [one] ### (3)
    @@
_DoubleHyphen <-
    \- [one]	### (1)
    \- [one]	### (2)
    @@
	
_StartXML <-
    \< [one] ### (1)
    \? [one] ### (2)
    _xALPHA [s one matches=("xml")] ### (3)
    @@

@@RULES

 ##############################################
 # Rule set 2 -- Signals for specially tagged #
 # items like processing instructions and     #
 # comments                                   #
 ##############################################
@RULES	
_StartProcessingInstruction <- ### (5)
    \<	[one] ### (1)
    \? [one trig]	### (2)
@@
	
_EndProcessingInstruction <- ### (10)
    \?	[one] ### (1)
    \>	[one] ### (2)
@@
	
_CDStart <-
    \< [one] ### (1)
    \! [one] ### (2)
    \[ [one] ### (3)
    _xALPHA [s one matches=("CDATA")] ### (4)
    \[ [one] ### (5)
@@
	
_CDEnd <-
    \] [one] ### (1)
    \] [one] ### (2)
    \> [one] ### (3)
@@

_EndDocType <-
	\] [one] ### (1)
	_xWHITE [star] ### (2)
	\> [one] ### (3)
@@

_EndEmptyTag <-
    \/ [one] ### (1)
    \> [one] ### (2)
@@

						
_EndTag <-
    \> [one] ### (1)
    @@

_CharRef <-
	\& [one] ### (1)
	\# [one] ### (2)
	_xNUM [one] ### (3)
	\; [one] ### (4)
@@

_CharRef <-
	\& [one] ### (1)
	\# [one] ### (2)
	x [one] ### (3)
	_xWILD [one matches=("xNUM" "A" "a" "B" "b" "C" "c" "D" "d" "E" "e" "F" "f")] ### (4)
	\; [one] ### (5)
@@
					
_EntityRef <-
	\& [one] ### (1)
	_xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
	_xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
	\; [one] ### (4)
@@
	
_PEReference <-
	\% [one] ### (1)
	_xWILD [s one matches=("_xALPHA" "_" ":")] ### (2)
	_xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (3)
	\; [one] ### (4)
@@
@@RULES

@POST
	#find the name of the entity we're declaring
	S("buffer1") = N("$text",5) ;
	S("buffer2") = N("$text",6) ;
	if (S("buffer1") != 0 && S("buffer2") != 0 ) {
		S("ElementName") = S("buffer1") + S("buffer2") ;
	}
	else if (S("buffer1") !=0)
		S("ElementName") = S("buffer1") ;
	else
		S("ElementName") = S("buffer2") ;
	S("CurrentElement") = findconcept(G("Elements"),S("ElementName")) ;
	if (S("CurrentElement") == 0) {
		makeconcept(G("Elements"),S("ElementName")) ;
		S("CurrentElement") = findconcept(G("Elements"),S("ElementName")) ;
	}	
	single() ;
@@POST
@RULES	

_ElementDeclStart <-
	\< [one] ### (1)
	\! [one] ### (2)
	_xWILD [s one matches=("ELEMENT")] ### (3)
	_xWHITE [plus] ### (4)
	_xWILD [s one matches=("_xALPHA" "_" ":")] ### (5)
	_xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (6)
@@
@@RULES

@RULES
_NotationDeclStart <-
	\< [one] ### (1)
	\! [one] ### (2)
	_xWILD [s one matches=("NOTATION")] ### (3)
	_xWHITE [plus] ### (4)
	_xWILD [s one matches=("_xALPHA" "_" ":")] ### (5)
	_xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (6)
@@
@@RULES

@POST
	#find the name of the element we're declaring
	S("buffer1") = str(N("$text",5)) ;
	S("buffer2") = str(N("$text",6)) ;
	if (N("$text",5) && N("$text",6)) {
		S("ElementName") = S("buffer1") + S("buffer2") ;
	}
	else if (N("$text",5)) {
		S("ElementName") = N("$text",5) ;
	}
	else if (N("$text",6)) {
		S("ElementName") = N("$text",6) ;
	}
	S("CurrentElement") = findconcept(G("Elements"),S("ElementName")) ;
	if (S("CurrentElement") == 0) {
		makeconcept(G("Elements"),S("ElementName")) ;
		S("CurrentElement") = findconcept(G("Elements"),S("ElementName")) ;
	}	
	single() ;
@@POST
@RULES
_AttlistDeclStart <-
	\< [one] ### (1)
	\! [one] ### (2)
	_xWILD [s one matches=("ATTLIST")] ### (3)
	_xWHITE [plus] ### (4)
	_xWILD [s one matches=("_xALPHA" "_" ":")] ### (5)
	_xWILD [s star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (6)
	@@	
@@RULES
@RULES

_EntityDeclStart <-
	\< [one] ### (1)
	\! [one] ### (2)
	_xWILD [s one matches=("ENTITY")] ### (3)
	_xWHITE [plus] ### (4)
@@	
@@RULES
