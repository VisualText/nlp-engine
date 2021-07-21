 ###############################################
 # FILE: XML baseElements.pat                  #
 # SUBJ: Collect the smallest syntactic pieces #
 #       of an XML file ... starts and ends of #
 #       tags, entity references, and the like #
 # AUTH: Paul Deane                            #
 # CREATED: 11/Jan/01 06:53:04                 #
 # DATE OF CURRENT VERSION: 31/Aug/01          #
 # Copyright 2001, Text Analysis International #
 ###############################################

 ###############################################
 # CONTENT INDEX                               #
 # 1. Rules for special items like ampersands  #
 #    greater than etc. plus tag elements      #
 # 2. Doctype declaration                      #
 # 3. Signals for special tag types including  #
 #    comments and entity references           #
 ###############################################

@CODE
	G("root") = findroot() ;
	G("tmp") = getconcept(G("root"),"tmp");
	G("gramtab") = getconcept(G("tmp"),"gram") ;

	#in case someone is so thoughtless as not to specify a doc type
	G("DocTypeName") = "XML";
	G("EntityName") = "Entities";
	G("ElementName") = "Elements";
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
	\& [one] ###(1)
	_xWILD [one matches=("amp")] ###(2)
	\; [one] ###(3)
	@@

_LessThan <-
	\& [one] ###(1)
	_xWILD [one matches=("lt")] ###(1)
	\; [one] ###(3)
	@@

_GreaterThan <-
	\& [one] ###(1)
	_xWILD [one matches=("gt")] ###(2)
	\; [one] ###(3)
	@@

_APos <-
	\& [one] ###(1)
	_xWILD [one matches=("apos")] ###(2)
	\; [one] ###(3)
	@@

_Quote <-
	\& [one] ###(1)
	_xWILD [one matches=("quot")] ###(2)
	\; [one] ###(3)
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
 # Rule set 2 -- Doc Type Declaration         #
 ##############################################
@POST
	#get the name of the document type we're working on here
	#and attach that to the tag we're bulding for the doctype
	#statement
	G("buffer1") = str(0) ;
	G("buffer2") = str(0) ;
	G("ElementName") = "Elements" ;
	G("EntityName") = "Entities" ;
	if (N("$text",5))
		G("buffer1") = str(N("$text",5)) ;
	if (N("$text",6))
		G("buffer2") = str(N("$text",6)) ;
	if (N("$text",5) && N("$text",6)) {
		G("DocTypeName") = G("buffer1") + G("buffer2") ;
	}
	else if (N("$text",5))
		G("DocTypeName") = G("buffer1") ;
	else if (N("$text",6))
		G("DocTypeName") = G("buffer2") ;
	S("DocTypeName") = G("DocTypeName");
		
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
	single() ;
@@POST
@RULES
_StartDocType <-
    \<	[one] ### (1)
    \! [one trig]	### (2)
    _xWILD [s one match=("DOCTYPE")]	### (3)
    _xWHITE [plus] ### (4)
    _xWILD [one matches=("_xALPHA" "_" ":")] ### (5)
    _xWILD [star matches=("_xALPHA" "_xNUM" "." "-" "_" ":")] ### (6)
@@
@@RULES


 ##############################################
 # Rule set 3 -- Signals for specially tagged #
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
    \[ ### (5)
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
	#Get the name of the element we are declaring here
	S("buffer1") = N("$text",5) ;
	S("buffer2") = N("$text",6) ;
	if (S("buffer1") != 0 && S("buffer2") != 0 ) {
		S("ElementName") = S("buffer1") + S("buffer2") ;
	}
	else if (S("buffer1") !=0)
		S("ElementName") = S("buffer1") ;
	else
		S("ElementName") = S("buffer2") ;

	#record the elements we've identified as part of the DTD
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
