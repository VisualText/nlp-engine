###############################################
# FILE: HTML Character Patterns.pat           #
# SUBJ: Deal with HTML special characters     #
#       like &nbsp
# AUTH: Paul Deane                            #
# CREATED: 07/Feb/01 14:08:51                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

##################################################
# Lookup of character codes in &ALPHABETIC; format
# (but not whitespace chars -- these are handled
# in the rule block for whitespace
##################################################
@POST
	G("root") = findroot() ;
	G("character_codes") = findconcept(G("root"),"character_codes") ;
	if (G("character_codes")) {
		G("current code") = findconcept(G("character_codes"),N("$text",2)) ;
	}
	else G("current code") = 0;
	if (G("current code")) {
		G("new text") = strval(G("current code"),"textValue") ;
		if (G("new text")) {
			S("textValue") = G("new text") ;
		}
	}
	single();
@@POST
@RULES
_specialCharacter <-
    \& [trig one] ### (1)
    _xALPHA [one] ### (2)
    \; [one] ### (3)
    @@
@@RULES

##################################################
# Lookup of character codes in &#NUMERIC; format
# (but not whitespace chars -- these are handled
# in the rule block for whitespace
##################################################
@POST
	G("root") = findroot() ;
	G("character_codes") = findconcept(G("root"),"character_codes") ;
	G("current code") = findconcept(G("character_codes"),N("$text",3)) ;
	if (G("current code")) {
		G("new text") = strval(G("current code"),"textValue") ;
		if (G("new text")) {
			S("textValue") = G("new text") ;
		}
	}
	single();
@@POST
@RULES
_specialCharacter <-
    \& [one] ### (1)
    \# [trig one] ### (2)
    _xNUM [one] ### (3)
    \; [one] ### (4)
    @@
@@RULES

##################################################
# Lookup of character codes in &#xHEX; format
# (but not whitespace chars -- these are handled
# in the rule block for whitespace
##################################################
@POST
	G("root") = findroot() ;
	G("character_codes") = findconcept(G("root"),"character_codes") ;
	G("current code") = findconcept(G("character_codes"),N("$text",4)) ;
	if (G("current code")) {
		G("new text") = strval(G("current code"),"textValue") ;
		if (G("new text")) {
			S("textValue") = G("new text") ;
		}
	}
	single();
@@POST
@RULES
_specialCharacter <-
    \& [one] ### (1)
    \# [one] ### (2)
    x [trig one] ### (3)
    _xWILD [min=1 max=4 matches=("0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "A" "B" "C" "D" "E" "F" "a" "b" "c" "d" "e" "f")] ### (4)
    \; [one] ### (5)
    @@
@@RULES
