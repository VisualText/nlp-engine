
 #################################################
 # FILE: HTML tag structure.pat                  #
 # SUBJ: The purpose of this pass is to assemble #
 #       the segments marked by beginning and    # 
 #       ending tags in an HTML document.        #
 # AUTH: Paul Deane                              #
 # CREATED: 7/Feb/01                             #
 # DATE OF CURRENT VERSION: 31/Aug/01            #
 # Copyright 2001 Text Analysis International    #
 #################################################


@NODES _ROOT
@CHECK
	#We only combine beginning and ending tags that have
	#the same tag name
	if (N("tagName",1) && N("tagName",3)) {
		if (!strequal(N("tagName",1),N("tagName",3))) {
			fail() ;
		}
	}
	if (!N("tagName",1))
		fail();
	if (!N("tagName",3))
		fail();
@@CHECK
@POST
       if (N("tagName",1))
		S("tagName") = str(N("tagName",1)) ;
	single() ;
@@POST
@RULES
_Element <-
	_startFormatElement [s one] ### (1)
	_xWILD [star fails=("_HTMLOpenTag" "_HTMLEndTag" "_startElement" "_endElement" "_startFormatElement" "_endFormatElement")] ### (2)
	_endFormatElement [s one] ### (3)
	@@
@@RULES

@CHECK
	#We only combine beginning and ending tags that have
	#the same tag name
	if (N("tagName",1) && N("tagName",3)) {
		if (!strequal(N("tagName",1),N("tagName",3))) {
			fail() ;
		}
	}
	if (!N("tagName",1))
		fail();
	if (!N("tagName",3))
		fail();
@@CHECK
@POST
       if (N("tagName",1))
		S("tagName") = str(N("tagName",1)) ;
	single() ;
@@POST
@RULES
_Element <-
	_HTMLOpenTag [s one] ### (1)
	_xWILD [star fails=("_HTMLOpenTag" "_HTMLEndTag" "_startElement" "_endElement" "_startFormatElement" "_endFormatElement")] ### (2)
	_HTMLEndTag [s one] ### (3)
	@@
@@RULES


@CHECK
	#We only combine beginning and ending tags that have
	#the same tag name
	if (N("tagName",1) && N("tagName",3)) {
		if (!strequal(N("tagName",1),N("tagName",3))) {
			fail() ;
		}
	}
	if (!N("tagName",1))
		fail();
	if (!N("tagName",3))
		fail();
@@CHECK
@POST
       if (N("tagName",1))
		S("tagName") = str(N("tagName",1)) ;
	single() ;
@@POST
@RULES
_Element <-
	_startElement [s one] ### (1)
	_xWILD [star fails=("_HTMLOpenTag" "_HTMLEndTag" "_startElement" "_endElement" "_startFormatElement" "_endFormatElement")] ### (2)
	_endElement [s one] ### (3)
	@@
@@RULES

@CHECK
	#We only combine beginning and ending tags that have
	#the same tag name -- in this case, section breaks are
	#really HTML tags with some special treatment
	if (N("tagName",1) && N("tagName",3)) {
		if (!strequal(N("tagName",1),N("tagName",3))) {
			fail() ;
		}
	}
	if (!N("tagName",1))
		fail();
	if (!N("tagName",3))
		fail();
@@CHECK
@POST
	S("tagName") = str(N("tagName",1)) ;
	S("heading level") = N("heading level",1) ;
	single() ;
@@POST
@RULES
_sectionHeading <-
	_sectionBreak [s one] ### (1)
	_xWILD [star fails=("_HTMLOpenTag" "_HTMLEndTag" "_startElement" "_endElement" "_startFormatElement" "_endFormatElement" "_sectionBreak")] ### (2)
	_HTMLEndTag [s one] ### (3)
	@@
@@RULES

@RULES
_HTMLDocument <-
	_openHTML [s one] ### (1)
	_xWILD [star fails=("_HTMLOpenTag" "_HTMLEndTag" "_startElement" "_endElement" "_startFormatElement" "_endFormatElement" "_closeHTML")] ### (2)
	_closeHTML [s one] ### (3)
	@@
@@RULES

