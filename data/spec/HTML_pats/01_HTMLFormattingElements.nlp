 #######################################################
 # FILE: FORMATTING_ELEMENTS.pat                       #
 # SUBJ: Recognize formatting elements such as are     #
 #       used to build tags. Includes:                 #
 #       Extract the pieces of HTML tagging            #
 #       which enable HTML tags to be reconstructed.   #
 #       Strategy: just get the cues in generic form.  #
 #       The analyzer doesn't care if the tags are     #
 #       part of the standard HTML set.                #
 # AUTH: Paul Deane                                    #
 # CREATED: 02/Jan/01 18:25:01                         #
 # DATE OF CURRENT VERSION: 31/Aug/01                  #
 # Copyright 2001, Text Analysis International         #
 #######################################################

@NODES _ROOT

 ###########################################################
 # CONTENT INDEX                                           #
 # 1. Parameter=Value expressions"                         #
 #    a. alphanumeric-alphanumeric="value"                 #
 #    b. alphanumeric-alphanumeric='value'                 #
 #    c. alphanumeric-alphanumeric=value wildcard          #
 #    d. alphanumeric="value"                              #
 #    e. alphanumeric='value'                              #
 #    f. alphanumeric=value wildcard                       #
 # 2. <TAGNAME expressions                                 #
 #	 a. < + alphanumeric + numeral (elements like <h2)  #
 #    b. < + alphanumeric (elements like <LI               #
 #    c. </ + alphanumeric + numeral (elements like </h2   #
 #    d. </ + alphanumeric + numeral (elements like /LI    #
 # 3. Pure syntactic elements                              #
 #    a. <!-- (comment start)                              #
 #    b. --> (comment end)                                 #
 #    c. <! (other special tags like doctype)              #
 #    d. <% (start of ASP scripts)                         #
 #    e. >  (close of a tag)                               #
 ###########################################################

 ###########################################################
 #                                                         #
 #   SECTION 1: PARAMETER=VALUE                            #
 #                                                         #
 ###########################################################


 #####################################################################
 # RULE 1                                                           #
 # This is the generic pattern using double quotation marks          #
 # It should pick up most cases. We have the other rules as cleanup  #
 # or to handle cases with single quotes or no quotes at all         #
 #####################################################################
@POST
	#We want to collect the parameter names and values
	#so that we can pass them up to the tag for later
	#processing
	S("parameterName") = N("$text",1);
	S("parameterValue") = N("$text",4);
	single();
@@POST
@RULES
_paramValue <- 
	 _xALPHA [one] ###1
	 \= [one] ###2
	 \" [one] ###3
	 _xWILD [plus fail=("\"")] ###4
	 \" [one] ###5
	 @@
@@RULES

 ###################################################
 #                                                 #
 # (rule 1a)                                       #
 #                                                 #
 # Identify HTML parameter="value" expressions     #
 # and pass the parameter name and value to the    #
 # dominating node. In this version, the parameter #
 # name is an expression like http-equiv           #
 # with double quotes                              #
 ###################################################
@POST
	#We want to collect the parameter names and values
	#so that we can pass them up to the tag for later
	#processing
	S("parameterName") = N("$text",2) + N("$text",3) + N("$text",4) ;
	S("parameterValue") = N("$text",9) ;
	single() ;
@@POST
@RULES
_paramValue <-
	_xWHITE [star] ###1
	_xALPHA \- [one trig] ###2
	_xALPHA [one] ###3
	_xWHITE [star] ###4
	\= [one] ###5
	_xWHITE [star] ###6
	\" [one] ###7
	_xWILD [plus fail=("\<" "\"" "\>")] ###8
	\" [one] ###9
	@@
@@RULES

 ##################################################
 #                                                #
 # (rule 1b)                                      #
 #                                                #
 # Identify HTML parameter='value' expressions    #
 # and pass the parameter name and value to the   #
 # dominating node. In this version the parameter #
 # is a hyphenated expression like http-equiv     #
 # with single quotes                             #
 ##################################################
@POST
	#We want to collect the parameter names and values
	#so that we can pass them up to the tag for later
	#processing
	S("parameterName") = N("$text",2) + N("$text",3) + N("$text",4) ;
	S("parameterValue") = N("$text",9) ;
	single() ;
@@POST
@RULES
_paramValue <-
	_xWHITE [star] ###1
	_xALPHA [one] ###2
	\- [one trig] ###3
	_xALPHA [one] ###4
	_xWHITE [star] ###5
	\= [one] ###6
	_xWHITE [star] ###7
	\' [one] ###8
	_xWILD [plus fail=("\<" "\'" "\>")] ###9
	\' [one] ###10
	@@
@@RULES

 ####################################################
 #                                                  #
 # (rule 1c)                                        #
 #                                                  #
 # Identify HTML parameter=value expressions        #
 # and pass the parameter name and value to the     #
 # dominating node. In this version the parameter   #
 # is a hyphenated expression like http-equiv       #
 # and the value is a complex wild card to pick up  #
 # anything missed by the simpler rules             #
 ####################################################
@POST
	#We want to collect the parameter names and values
	#so that we can pass them up to the tag for later
	#processing
	S("parameterName") = N("$text",2) + N("$text",3) + N("$text",4) ;
	S("parameterValue") = N("$text",8) ;
	single() ;
@@POST
@RULES
_paramValue <- 
	_xWHITE [star] ##1 
	_xALPHA [one]  ##2
	\- [one trig]  ##3
	_xALPHA [one]  ##4
	_xWHITE [star] ##5
	\= [one]  ##6
	_xWHITE [star] ##7 
	_xWILD [plus fail=("\"" "\<" "\>" "\'" "\ " "/n" "/t")] ##8 
	@@
@@RULES

 #####################################################
 #                                                   #
 # (rule 1d)                                         #
 #                                                   #
 # Identify HTML parameter="value" expressions       #
 # and pass the parameter name and value to the      #
 # dominating node. In this version, the parameter   #
 # name is a simple alphanumeric expression          #
 #####################################################
@POST
	#We want to collect the parameter names and values
	#so that we can pass them up to the tag for later
	#processing
	S("parameterName") = N("$text",2) ;
	S("parameterValue") = N("$text",7) ;
single() ;
@@POST
@RULES
_paramValue <-
	_xWHITE [star] ###1
	_xALPHA [one] ###2
	_xWHITE [star] ###3 
	\= [one trig] ###4
	_xWHITE [star] ###5
	\" [one] ###6
	_xWILD [plus fail=("\<" "\"" "\>")] ###7
	\" [one] ##8 
	@@
@@RULES

 ######################################################
 #                                                    #
 # (rule 1e)                                          #
 #                                                    #
 # Identify HTML parameter='value' expressions        #
 # and pass the parameter name and value to the       #
 # dominating node. In this version, the parameter    #
 # name is a simple alphanumeric expression           #
 ######################################################
@POST
	#We want to collect the parameter names and values
	#so that we can pass them up to the tag for later
	#processing
	S("parameterName") = N("$text",2) ;
	S("parameterValue") = N("$text",7) ;
	single() ;
@@POST
@RULES
_paramValue <-
	_xWHITE [star] ###1
	_xALPHA [one] ###2
	_xWHITE [star] ###3
	\= [one] ###4
	_xWHITE [star] ###5 
	\' [one] ###6
	_xWILD [plus fail=("\<" "\'" "\>")] ###7 
	\' [one] ###8
	@@
@@RULES

 #####################################################
 #                                                   #
 # (rule 1f)                                         #
 #                                                   #
 # Identify HTML parameter=value expressions         #
 # and pass the parameter name and value to the      #
 # dominating node. In this version, the parameter   #
 # name is a simple alphanumeric expression          #
 # and the value is broadly defined -- this is a     #
 # cleanup rule using _xWILD to pick up anything     #
 # earlier rules may have missed.                    #
 #####################################################
@POST
	#We want to collect the parameter names and values
	#so that we can pass them up to the tag for later
	#processing
	S("parameterName") = N("$text",2) ;
	S("parameterValue") = N("$text",6) ;
	single() ;
@@POST
@RULES
_paramValue <-
	_xWHITE [star] ###1 
	_xALPHA [one] ###2
	_xWHITE [star] ###3
	\= [one trig] ###4
	_xWHITE [star] ###5
	_xWILD[plus fail=("\"" "\<" "\>" "\'" "\ " "/n" "/t")] ###6
	@@
@@RULES

 ###################################################################
 #                                                                 #
 # SECTION 2: INITIAL TAG FRAGMENTS (opening brace plus tag name)  #
 #                                                                 #
 ###################################################################

 ############################################
 #                                          #
 # (rule 2a)                                #
 #                                          #
 # Rule for opening tags of the form alpha  #
 # plus numeral                             #
 ############################################ 
@POST
    #We just collect information about the heading
    #here so we know if we're at a heading tag for
    #later analysis of the text structure
    G("buffer") = N("$text",2) + N("$text",3) ; 
    #standardize tag names to uppercase
	S("tagName") = strtoupper(G("buffer")) ;
	if (strequal(S("tagName"),"H1")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 1 ;
	}
	else if (strequal(S("tagName"),"H2")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 2 ;
	}
	else if (strequal(S("tagName"),"H3")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 3 ;
	}
	else if (strequal(S("tagName"),"H4")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 4 ;
	}
	else if (strequal(S("tagName"),"H5")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 5 ;
	}
	else if (strequal(S("tagName"),"H6")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 6 ;
	}
	single() ;
@@POST
@RULES
_startOpenTag <-
	\< [one trig] ###1
	_xALPHA [one] ###2
	_xNUM [one] ###3
	@@
@@RULES

 ############################################
 #                                          #
 # (rule 2b)                                #
 #                                          #
 # Rule for simple opening tags, which will #
 # have the tag name specified as the first #
 # alphanumeric element after open bracket  #
 ############################################ 
@POST
    #Collect information on tag types that cause a paragraph level
    #or division level break int he text
    G("buffer") = N("$text",2) ;
    #standardize tag names to uppercase
    S("tagName") = strtoupper(G("buffer")) ;
	if (strequal(S("tagName"),"HR")) {
	     S("horizBreak") = 1;
	}
	else if (strequal(S("tagName"),"P")) {
		S("paraBreak") = 1 ;
	}
	else if (strequal(S("tagName"),"DIV")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 0 ;
	}
	else if (strequal(S("tagName"),"BR")) {
		S("lineBreak") = 1 ;
	}
	single() ;
@@POST
@RULES
_startOpenTag <-
	 \< [one trig] ###1
	 _xALPHA [one] ###2 
	 @@
@@RULES

 ############################################
 #                                          #
 # (rule 2c)                                #
 #                                          #
 # Rule for ending tags of the form alpha   #
 # plus numeral                             #
 ############################################ 
@POST
    #We collect information about heading levels here for later use
    G("buffer") = N("$text",3) + N("$text",4) ;
    #standardize tag names to uppercase
    S("tagName") =  strtoupper(G("buffer")) ; 
	if (strequal(S("tagName"),"H1")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 1 ;
	}
	else if (strequal(S("tagName"),"H2")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 2 ;
	}
	else if (strequal(S("tagName"),"H3")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 3 ;
	}
	else if (strequal(S("tagName"),"H4")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 4 ;
	}
	else if (strequal(S("tagName"),"H5")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 5 ;
	}
	else if (strequal(S("tagName"),"H6")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 6 ;
	}
	single() ;
@@POST
@RULES
_startEndTag <-
	 \< [one] ###1
	 \/ [one trig] ###2 
	 _xALPHA [one] ###3
	 _xNUM [one] ###4
	 @@
@@RULES

 ############################################
 #                                          # 
 # (rule 2d)                                #
 #                                          #
 # Rule for simple ending tags, which will  #
 # have the tag name specified as the first #
 # alphanumeric element after open bracket  #
 ############################################ 
@POST
    #Collect information on tag types that cause a paragraph level
    #or division level break int he text
    G("buffer") = N("$text",3) ;
    #standardize tag names to uppercase
    S("tagName") = strtoupper(G("buffer")) ;
     if (strequal(S("tagName"),"HR")) {
	     S("horizBreak") = 1;
	}
	else if (strequal(S("tagName"),"P")) {
		S("paraBreak") = 1 ;
	}
	else if (strequal(S("tagName"),"DIV")) {
		S("sectionBreak") = 1 ;
		S("heading level") = 0 ;
	}
	else if (strequal(S("tagName"),"BR")) {
		S("lineBreak") = 1 ;
	}
	single() ;
@@POST
@RULES
_startEndTag <-
	\< [one] ###1
	\/ [one trig] ###2 
	_xALPHA [one] ##3
	@@
@@RULES


#################################################
# These are rules for various odds and ends     #
# where we don't need to pass any information   #
# up the parse tree -- e.g., syntactic elements #
# like the start and end of a comment, or the   #
# close bracket at the end of a tag             #
#################################################
@RULES

 # (rule 3a)
_startComment <-
	\< [one] ###1 
	\! [trig] ###2
	\- [one] ###3
	\- [one] ###4
	@@

# (rule 3b)
_endComment <-
	\- [one] ###1 
	\- [one] ###2
	\> [one trig] ###3 
	@@
	
_endComment <-
    \- [one] ### (1)
    \- [one] ### (2)
    \! [one] ### (3)
    \> [one] ### (4)
    @@

# (rule 3c)
_startSpecialTag <-
	\< [one] ###1
	\! [one trig] ###2 
	_xALPHA [one] ###3
	@@

# (rule 3d)
_startASPScript <-
	\< [one] ###1
	\% [one trig] ###2 
	@@

# (rule 3e)
_endTag <- 
	\> [one] ###1 
	@@
@@RULES
