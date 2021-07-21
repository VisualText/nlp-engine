
 #################################################
 # FILE: HTML Text Functions.pat                 #
 # SUBJ: Distinguish between formatting elements #
 #       in HTML tags and other tags             #
 # AUTH: Paul Deane                              #
 # CREATED: 07/Jun/01 16:12:03                   #
 # DATE OF THIS VERSION: 31/Aug/01               #
 # Copyright 2001, Text Analsis International    #
 #################################################

@NODES _ROOT

##########################
# HTML horizontal rules  #
##########################
@CHECK
	if ( N("horizBreak",2) == 1)
		succeed() ;
	else fail();
@@CHECK
@POST
pncopyvars(2);
single();
@@POST
@RULES
_horizRule <-
    _xWHITE [star]	### (1)
    _HTMLOpenTag [trig]	### (2)
    _xWHITE [star] ### (3)
    @@
@@RULES

@RULES
\& <-
    \& [one] ### (1)
    amp [one] ### (2)
    \; [one] ### (3)
    @@
@@RULES

#######################################################
# HTML paragraph break indicated by double linebreak  #
#######################################################
@CHECK
	if ( N("lineBreak",2)==1 &&
			  N("lineBreak",4)==1 )
		succeed() ;
	else fail();
@@CHECK
@POST
pncopyvars(2);
single();
@@POST
@RULES
_paraBreak <-
    _xWHITE [star] ### (1)
    _HTMLOpenTag [trig] ### (2)
    _xWHITE [star] ### (3)
    _HTMLOpenTag ### (4)
    _xWHITE [star] ### (5)
    @@
@@RULES


###########################
# HTML paragraph breaks   #
###########################
@CHECK
	if ( N("paraBreak",2)==1 &&
			  N("paraBreak",4)==1 )
		succeed() ;
	else fail();
@@CHECK
@POST
pncopyvars(2);
single();
@@POST
@RULES
_paraBreak <-
    _xWHITE [star] ### (1)
    _HTMLEndTag [trig] ### (2)
    _xWHITE [star] ### (3)
    _HTMLOpenTag ### (4)
    _xWHITE [star] ### (5)
    @@
@@RULES

###########################
# HTML paragraph breaks   #
###########################
@CHECK
	if ( N("paraBreak",2)==1)
		succeed() ;
	else fail();
@@CHECK
@POST
pncopyvars(2);
single();
@@POST
@RULES
_paraBreak <-
    _xWHITE [star] ### (1)
    _HTMLOpenTag [trig] ### (2)
    _xWHITE [star] ### (3)
    @@
_paraBreak <-
    _xWHITE [star] ### (1)
    _HTMLEndTag [trig] ### (2)
    _xWHITE [star] ### (3)
    @@
@@RULES

#####################################################
# Section breaks.                                   #
#                                                   #
# DIV tags are one major indication of section      #
# information in an HTML document, as are the start #
# of a heading.                                     #
#####################################################
@CHECK
	if (N("sectionBreak",2)==1)
		succeed() ;
	else fail();
@@CHECK
@POST
	pncopyvars(2);
	single();
@@POST
@RULES
_sectionBreak <-
    _xWHITE [star] ### (1)
    _HTMLOpenTag [trig] ### (2)
    _xWHITE [star] ### (3)
    @@
@@RULES

################################################
# Eliminate repeated whitespace, which isn't   #
# significant in an HTML file                  #
################################################
@POST
single();
@@POST
@RULES
_whiteSpace <-
    _xWHITE [star] ### (1)
    \& [one] ### (2)
    nbsp [one] ### (3)
    \; [one] ### (4)
    _xWHITE [star] ### (5)
    @@
_whiteSpace <-
    _xWHITE [one] ### (1)
    _xWHITE [star] ### (2)
    @@
@@RULES


#####################################################
# Mapping of <BR> and similar tags to line breaks   #
# for compatibility with non-html handling of text  #
#####################################################
@CHECK
	if (N("lineBreak",2) != 1)
		fail();
	else succeed();
@@CHECK
@POST
	pncopyvars(2);
	single();
@@POST
@RULES
_lineBreak <-
    _xWHITE [star] ### (1)
    _HTMLOpenTag [trig] ### (2)
    _xWHITE [star] ### (3)
    @@
@@RULES


#####################################################
# Mapping of <IMG> and similar tags to line breaks   #
# for compatibility with non-html handling of text  #
#####################################################
@CHECK
	if (strequal(N("tagName",2),"IMG") != 1 &&
	    strequal(N("tagName",2),"AREA") != 1
	   )
		fail();
	else succeed();
@@CHECK
@POST
	pncopyvars(2);
	single();
@@POST
@RULES
_imageLink <-
    _xWHITE [star] ### (1)
    _HTMLOpenTag [trig] ### (2)
    _xWHITE [star] ### (3)
    @@
@@RULES

#####################################################
# Mapping of <META> and similar tags to line breaks   #
# for compatibility with non-html handling of text  #
#####################################################
@CHECK
	if (strequal(N("tagName",2),"META") != 1 &&
	    strequal(N("tagName",2),"BASE") != 1
	   ) fail();
	else succeed();
@@CHECK
@POST
	pncopyvars(2);
	single();
@@POST
@RULES
_metaInfo <-
    _xWHITE [star] ### (1)
    _HTMLOpenTag [trig] ### (2)
    _xWHITE [star] ### (3)
    @@
@@RULES

#####################################################
# Mapping of <META> and similar tags to line breaks   #
# for compatibility with non-html handling of text  #
#####################################################
@CHECK
	if (strequal(N("tagName",2),"INPUT") != 1
	   ) fail();
	else succeed();
@@CHECK
@POST
	pncopyvars(2);
	single();
@@POST
@RULES
_formInfo <-
    _xWHITE [star] ### (1)
    _HTMLOpenTag [trig] ### (2)
    _xWHITE [star] ### (3)
    @@
@@RULES


@CHECK
G("tagName") = N("tagName",1);
if (!G("tagName"))
	fail();
if ( 	!strequalnocase(G("tagName"),"title") &&
     	!strequalnocase(G("tagName"),"h1") &&
	!strequalnocase(G("tagName"),"h2") &&
	!strequalnocase(G("tagName"),"h3") &&
	!strequalnocase(G("tagName"),"h4") &&
	!strequalnocase(G("tagName"),"h5") &&
	!strequalnocase(G("tagName"),"h6")
   ) fail();
@@CHECK
@POST
pncopyvars(1);
single();
@@POST
@RULES
_startElement <-
    _HTMLOpenTag [one] ### (1)
    @@

@CHECK
G("tagName") = N("tagName",1);
if (!G("tagName"))
	fail();
if ( 	!strequalnocase(G("tagName"),"title") &&
     	!strequalnocase(G("tagName"),"h1") &&
	!strequalnocase(G("tagName"),"h2") &&
	!strequalnocase(G("tagName"),"h3") &&
	!strequalnocase(G("tagName"),"h4") &&
	!strequalnocase(G("tagName"),"h5") &&
	!strequalnocase(G("tagName"),"h6")
   ) fail();
@@CHECK
@POST
pncopyvars(1);
single();
@@POST
@RULES
_endElement <-
    _HTMLEndTag [one] ### (1)
    @@

@CHECK
G("tagName") = N("tagName",1);
if (!G("tagName"))
	fail();
if ( 	!strequalnocase(G("tagName"),"b") &&
     	!strequalnocase(G("tagName"),"i") &&
	!strequalnocase(G("tagName"),"strong") &&
     	!strequalnocase(G("tagName"),"font") &&
     	!strequalnocase(G("tagName"),"center") &&
     	!strequalnocase(G("tagName"),"em")
   ) fail();
@@CHECK
@POST
pncopyvars(1);
single();
@@POST
@RULES
_startFormatElement <-
    _HTMLOpenTag [one] ### (1)
    @@

_endFormatElement <-
    _HTMLEndTag [one] ### (1)
    @@
