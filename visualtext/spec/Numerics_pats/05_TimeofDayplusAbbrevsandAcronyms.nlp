##############################################################
# FILE: Time of Day plus Abbrevs and Acronyms.pat            #
# SUBJ: Various types of entities that can be                #
#       recognized solely by combinations of                 #
#       character patterns, with minimal access              #
#       to grammatical information - where recursive def.    #
#       is necessary                                         #
# AUTH: Paul Deane                                           #
# CREATED: 04/Jan/01 13:35:48                                #
# DATE OF THIS VERSION: 31/Aug/01                            #
# Copyright 2001, Text Analysis International                #
##############################################################

@NODES _ROOT

###############################################
#
# CONTENT INDEX
#
#  1. Internet domain names formed with alphanumerics
#     separated by dots, ending in the common domains
#     .com, .edu etc.
#  2. email address: alphanumeric @ domain name
#  3. Time ranges: from 5:00 to 7:00 p.m., between 5:00 and 7:00 p.m.
#  4. Time abbrevs: a.m., p.m.
#  5. Simple colon numerals, possible time expressions 5:00, 7:00
#  6. Longer colon numerals 5:65:79:988
#  7. Time of day expressions, 5:00 p.m., 5:01 p.m. CDT, etc.
#  8. More time of day expressions
#  9. Abbreviations of the form U.S., U.S.A. etc.
#  10. Alphanumeric expressions

  ############################
  #    (rules 1)             #
  # Internet Domain Names    #
  ############################
@POST
single() ;
@@POST
@RULES
_domainName <-
    _alphaNumeric [one] ### (1)
    \. [one] ### (2)
    _domainName [one] ### (3)
    @@

_domainName <-
    _alphaNumeric [one] ### (1)
    \. [one] ### (2)
    _xWILD [one matches=("com" "edu" "net" "gov" "org" "ca" "uk")] ### (3)
    @@

_domainName <-
    _xWILD [plus matches=(_xALPHA _xNUM)] ### (1)
    \. [one] ### (2)
    _domainName [one] ### (3)
    @@
	
_domainName <-
    _xWILD [plus matches=(_xALPHA _xNUM)] ### (1)
    \. [one] ### (2)
    _xWILD [one matches=("com" "edu" "net" "gov" "org" "ca" "uk")] ### (3)
    @@
@@RULES

@RULES
_url <-
    _xWILD [one matches=("http" "ftp" "gopher" "mailto" "telnet")] ### (1)
    \: [one] ### (2)
    \/ [one] ### (3)
    \/ [one] ### (4)
    _domainName [one] ### (5)
    @@

_url <-
    _url [one] ### (1)
    _xWILD [min=1 max=1 match=("\\" "/")] ### (2)
    _alphaNumeric [opt] ### (3)
    @@
@@RULES

  ############################
  #      (rule 2)            #
  #  Email addresses         #
  ############################
@POST
S("user name") = N("$text",1) ;
S("domain name") = N("$text",3) ;
single() ;
@@POST
@RULES
_emailAddress <-
    _xWILD [plus matches=(_xALPHA _xNUM)] ### (1)
    \@ [one] ### (2)
    _domainName [one] ### (3)
    @@
@@RULES

  ##################################################
  # Times of Day & the like
  ##################################################

  ##################################
  #   rule 3a                      #
  # from time of day to time of day#
  ##################################
@POST
	S("start hour") = N("hour",3) ;
	S("start minute") = N("minute",3) ;
	S("start second") = N("second",3) ;
	S("start meridian") = N("meridian",3) ;
	S("end hour") = N("hour",7) ;
	S("end minute") = N("minute",7) ;
	S("end second") = N("second",7) ;
	S("end meridian") = N("meridian",7) ;
	S("timeZone") = N("timeZone",7) ;
	single() ;
@@POST
@RULES
_timeRange <-
    _xWILD [star matches=("from" "From" "FROM")] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _timeOfDay [one] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [one matches=("-" "_to" "to" "TO" "To" "through" "THROUGH")] ### (5)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (6)
    _timeOfDay [one] ### (7)
    @@
@@RULES


  ##################################
  #   rule 3b                      #
  # from possible time of day      #
  # to time of day                 #
  ##################################
@POST
	S("start hour") = N("hour",3) ;
	S("start minute") = N("minute",3) ;
	S("start second") = N("second",3) ;
	S("start meridian") = N("meridian",3) ;
	S("end hour") = N("hour",7) ;
	S("end minute") = N("minute",7) ;
	S("end second") = N("second",7) ;
	S("end meridian") = N("meridian",7) ;
	S("timeZone") = N("timeZone",7) ;
	single() ;
@@POST
@RULES
_timeRange <-
    _xWILD [opt matches=("from" "From" "FROM")] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _colonNumeral [one] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [one matches=("-" "_to" "to" "TO" "To" "through" "THROUGH")] ### (5)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (6)
    _timeOfDay [one] ### (7)
    @@
@@RULES

  ######################################
  #   rule 3c                          #
  # between time of day and time of day#
  ######################################
@POST
	S("start hour") = N("hour",3) ;
	S("start minute") = N("minute",3) ;
	S("start second") = N("second",3) ;
	S("start meridian") = N("meridian",3) ;
	S("end hour") = N("hour",7) ;
	S("end minute") = N("minute",7) ;
	S("end second") = N("second",7) ;
	S("end meridian") = N("meridian",7) ;
	S("timeZone") = N("timeZone",7) ;
	single() ;
@@POST
@RULES
_timeRange <-
    _xWILD [opt matches=("between" "BETWEEN" "Between")] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _timeOfDay [one] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [one matches=("&" "and" "AND" "And")] ### (5)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (6)
    _timeOfDay [one] ### (7)
    @@
@@RULES


  ######################################
  #   rule 3d                          #
  # between time of day and time of day#
  ######################################
@POST
	S("start hour") = N("hour",3) ;
	S("start minute") = N("minute",3) ;
	S("start second") = N("second",3) ;
	S("start meridian") = N("meridian",3) ;
	S("end hour") = N("hour",7) ;
	S("end minute") = N("minute",7) ;
	S("end second") = N("second",7) ;
	S("end meridian") = N("meridian",7) ;
	S("timeZone") = N("timeZone",7) ;
	single() ;
@@POST
@RULES
_timeRange <-
    _xWILD [opt matches=("between")] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _colonNumeral [one] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [one matches=("&" "and")] ### (5)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (6)
    _timeOfDay [one] ### (7)
    @@
@@RULES


  #######################
  # rule 4a             #
  # time abbreviations  #
  #######################
@POST
	S("meridian") = "a.m." ;
	single() ;
@@POST
@RULES
_timeAbbrev <-
    _xWILD [one matches=("a")] ### (1)
    \. [one] ### (2)
    _xWILD [one matches=("m")] ### (3)
    \. [opt] ### (4)
    @@
@@RULES

  #######################
  # rule 4b             #
  # time abbreviations  #
  #######################
@POST
	S("meridian") = "p.m." ;
	single();
@@POST
@RULES
_timeAbbrev <-
    _xWILD [one matches=("p" "P")] ### (1)
    \. [one] ### (2)
    _xWILD [one matches=("m" "M")] ### (3)
    \. [one] ### (4)
    @@
@@RULES

  ##########################################
  # rule 5                                 #
  # number colon number -- possible time of#
  # day expressions                        #
  ##########################################
@POST
	S("firstlevel") = 1 ;
    G("num1T") = N("$text",1) ;
	G("num1") = num(G("num1T"));
	G("num2T") = N("$text",3) ;
	G("num2") = num(G("num2T"));
    if ( G("num1") > 0 && G("num1") < 25 && G("num2") >= 0 && G("num2") < 61 ) {
	  	S("hour") = N("$text",1) ;
		S("minute") = N("$text",3) ;
		S("possible time") = 1 ;
	}
	single() ;
@@POST
@RULES
_colonNumeral <-
    _xNUM [one] ### (1)
    _xWILD [one match=(":")] ### (2)
    _xNUM [one] ### (3)
    @@
@@RULES


  #######################
  # rule 6              #
  # longer colon number #
  # expressions - case  #
  # with three numbers  #
  # is possible time    #
  # expression.         #
  #######################
@POST
	G("secT") = N("$text",3);
	G("sec") = num(G("secT"));
	if ( N("possible time",1) == 1 &&
	     N("firstlevel",1) == 1 &&
	     G("sec") >= 0 &&
		 G("sec") <= 60
	   ) {
	   		S("hour") = N("hour",1) ;
			S("minute") = N("minute",1) ;
			S("second") = N("$text",3) ;
			S("possible time") = 1 ;
	   }	
  single() ;
@@POST
@RULES
_colonNumeral <-
    _colonNumeral [one] ### (1)
    _xWILD [one match=(":")] ### (2)
    _xNUM [one] ### (3)
    @@
@@RULES

  ####################################
  #  Rule 7a                        #
  #  explicit time of day with abbrev#
  #################################### 
@CHECK
	if (N("possible time",1)) {
		succeed() ;
	}
	else fail() ;
@@CHECK
@POST
	S("hour") = N("hour",1) ;
	S("minute") = N("minute",1) ;
	S("second") = N("second",1) ;
       S("meridian") = strtolower(N("$text",3)) ;
	S("timeZone") = N("$text",5) ;
	single() ;
@@POST
@RULES
_timeOfDay <-
    _colonNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _timeAbbrev [one] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [opt matches=("EST" "EDT" "PST" "PDT" "CST" "CDT" "PST" "PDT" "GMT")] ### (5)
    @@
@@RULES

  ####################################
  #  Rule 8b                        #
  #  explicit time of day no periods #
  #################################### 
@CHECK
	if (N("possible time",1)) {
		succeed() ;
	}
	else fail() ;
@@CHECK
@POST
	S("hour") = N("hour",1) ;
	S("minute") = N("minute",1) ;
	S("second") = N("second",1) ;
	if (strequal(N("$text",3),"am"))
       S("meridian") = "a.m." ;
	if (strequal(N("$text",3),"pm"))
       S("meridian") = "p.m." ;
	if (strequal(N("$text",3),"AM"))
       S("meridian") = "a.m." ;
	if (strequal(N("$text",3),"PM"))
       S("meridian") = "p.m." ;
	S("timeZone") = N("$text",5) ;
	single() ;
@@POST
@RULES
_timeOfDay <-
    _colonNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [one matches=("am" "pm")] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [opt matches=("EST" "EDT" "PST" "PDT" "CST" "CDT" "PST" "PDT" "GMT")] ### (5)
    @@
@@RULES


  ###############################################
  # Rule 9                                      #
  # Abbreviations (capital + period alternating #
  ###############################################
@CHECK
G("word") = N("$text",2);
G("L1") = strlength(G("word")); 
if (G("L1") > 2)
 	fail();
@@CHECK
@RULES
_abbrev <-
    _abbrev [one] ### (1)
    _xALPHA [one] ### (2)
    \. [opt] ### (3)
    @@
@@RULES

@CHECK
G("word") = N("$text",1);
G("LL") = num(strlength(G("word")));
if (G("LL") > 2)
	fail();
G("firstword") = N("$text",1) + N("$text",2);
G("fwordConcept") = dictfindword(G("firstword"));
if (G("fwordConcept") && G("LL")==2 && N("$text",3))
     fail();	
G("word") = N("$text",4);
G("LL") = num(strlength(G("word")));
if (G("LL") > 2)
	fail(); 
@@CHECK
@RULES
_abbrev <-
    _xALPHA [one] ### (1)
    \. [one] ### (2)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (3)
    _xALPHA [one] ### (4)
    \. [one] ### (5)
    @@
@@RULES


################################
#   Rule 10                    #
#  alphanumeric                #
################################
@POST
S("noun") = 1;
S("adjective") = 1;
single();
@@POST
@RULES
_alphaNumeric <-
    _xALPHA [s one]	### (1)
    _xNUM [s one] ### (2)
    @@
	
_alphaNumeric <-
    _xNUM [s one] ### (1)
    _xALPHA [s one] ### (2)
    @@

@@RULES

@POST
	S("noun")=1;
	S("adjective")=1;
	single();
@@POST
@RULES	
_alphaNumeric <-
    _alphaNumeric [one] ### (1)
    _xNUM [one] ### (2)
    @@

_alphaNumeric <-
    _alphaNumeric [one] ### (1)
    _xALPHA [one] ### (2)
    @@
@@RULES

