####################################################################
# FILE: Phone Numbers.pat                                          #
# AUTH: Paul Deane                                                 #
# CREATED: 04/Jan/01 11:29:43                                      #
# DATE OF THIS VERSION: 31/Aug/01                                  #
# Copyright 2001, Text Analysis International                      #
####################################################################

@NODES _ROOT


@CHECK
   if (strlength(N("$text",2)) != 2)
   	fail();
@@CHECK
@RULES
_yearAbbr <-
    \' [s one] ### (1)
    _xNUM [s one] ### (2)
    @@
@@RULES

###############################
#       (rule 20)                #
# Unambiguous telephone numbers  #
# of the form 1-905-555-5050     #
################################
@CHECK
	if (N("$length",5) != 3 ||
	    N("$length",9) != 3 ||
		N("$length",13) != 4
		) fail() ;
	else
		succeed() ; 
@@CHECK
@POST
S("area code") = N("$text",5) ;
S("local number") = N("$text",9) + N("$text",13) ;
single();
@@POST
@RULES
_phoneNumber [base] <- 
	_xWILD [s matches=("1" "0")] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\- [one trig] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [s one] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\- [one] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [one]
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\- [one]
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [one]
	@@
@@RULES


###############################
#       (rule 21)                 #
# Unambiguous telephone numbers  #
# of the form 905-555-5050       #
################################
@CHECK
	if (N("$length",1) != 3 ||
	    N("$length",5) != 3 ||
		N("$length",9) != 4
		) fail() ;
	else
		succeed() ; 
@@CHECK
@POST
S("area code") = N("$text",1) ;
S("local number") = N("$text",5) + N("$text",9) ;
single();
@@POST
@RULES
_phoneNumber [base] <-
	_xNUM [s one]
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\- [one trig] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [s one] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\- [one]
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [s one] 
	@@
@@RULES

###############################
#       (rule 22)                 #
# Unambiguous telephone numbers  #
# of the form (919) 555-5050     #
################################
@CHECK
	if (N("$length",3) != 3 ||
	    N("$length",7) != 3 ||
		N("$length",11) != 4
		) fail() ;
	else
		succeed() ; 
@@CHECK
@POST
S("area code") = N("$text",3) ;
S("local number") = N("$text",7) + N("$text",11) ;
single();
@@POST
@RULES
_phoneNumber [base] <-
	\( [s trig]
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [s one] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\) [one]
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [s one] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\- [one]
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [s one] 
	@@
@@RULES

###############################
#       (rule 23)                #
# Unambiguous telephone numbers  #
# of the form 1-905-555-5050     #
################################
@CHECK
	if (N("$length",1) != 3 ||
	    N("$length",5) != 4 
		) fail() ;
	else
		succeed() ; 
@@CHECK
@POST
S("local number") = N("$text",1) + N("$text",5) ;
single();
@@POST
@RULES
_phoneNumber [base] <- 
	_xNUM [s one] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	\- [one trig] 
     _xWILD [s min=0 max=2 match=(_xBLANK "_whiteSpace")]
	_xNUM [s one] 
	@@
@@RULES


#####################
# Rule 26              #
# Nonrecursive monetary#
######################
@POST
if (strequal(N("$text",3),"$"))
       S("currency") = "dollars";
if (strequal(N("$text",3),"£"))
       S("currency") = "pounds";
if (N("$text",1))
	  S("nationality") = N("$text",1);
S("Numeral Value") = num(N("$text",4));
S("fractional value") = num(N("$text",6));
single();
@@POST
@RULES
_money <-
    _xWILD [opt match=("US" "Canadian" "British")]	### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")]	### (2)
    _xWILD [s one matches=("\$" "£")]				### (3)
    _xNUM [opt]										### (4)
    \. [one]										### (5)
    _xNUM [one]										### (6)
    @@
@@RULES

@POST
if (strequal(N("$text",3),"$"))
       S("currency") = "dollars";
if (strequal(N("$text",3),"£"))
       S("currency") = "pounds";
if (N("$text",1))
	  S("nationality") = N("$text",1);
S("Numeral Value") = N("Numeral Value",4);
single();
@@POST
@RULES
_money <-
    _xWILD [opt match=("US")]						### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")]	### (2)
    _xWILD [s one matches=("\$" "£")]				### (3)
    _xWILD [one match=("_cardinalNumeral")]	### (4)
    @@
@@RULES


@POST
if (strequal(N("$text",3),"$"))
       S("currency") = "dollars";
if (strequal(N("$text",3),"£"))
       S("currency") = "pounds";
if (N("$text",1))
	  S("nationality") = N("$text",1);
S("Numeral Value") = N("Numeral Value",4);
S("fractional value") = N("fractional value",4);
single();
@@POST
@RULES
_money <-
    _xWILD [opt match=("US")]						### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")]	### (2)
    _xWILD [s one matches=("\$" "£")]				### (3)
    _xWILD [one match=("_decimalNumber")]	### (4)
    @@
@@RULES

@POST
if (strequal(N("$text",3),"$"))
       S("currency") = "dollars";
if (strequal(N("$text",3),"£"))
       S("currency") = "pounds";
if (N("$text",1))
	  S("nationality") = N("$text",1);
S("Numeral Value") = 1000000*num(N("$text",4)) + 1000*num(N("$text",6)) + num(N("$text",8));
single();
@@POST
@RULES
_money <-
    _xWILD [opt match=("US")]						### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")]	### (2)
    _xWILD [s one matches=("\$" "£")]				### (3)
    _xNUM [one]										### (4)
    \, [opt]										### (5)
    _xNUM [one]										### (6)
    \, [one]										### (7)
    _xNUM [one]										### (8)
    @@
@@RULES

@POST
if (strequal(N("$text",3),"$"))
       S("currency") = "dollars";
if (strequal(N("$text",3),"£"))
       S("currency") = "pounds";
if (N("$text",1))
	  S("nationality") = N("$text",1);
S("Numeral Value") = 1000*num(N("$text",4)) + num(N("$text",6));
single();
@@POST
@RULES
_money <-
    _xWILD [opt match=("US" "Canadian" "British")]	### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")]	### (2)
    _xWILD [s one matches=("\$" "£")]				### (3)
    _xNUM [one]										### (4)
    \, [one]										### (5)
    _xNUM [one]										### (6)
    @@
@@RULES

@POST
if (strequal(N("$text",3),"$"))
       S("currency") = "dollars";
if (strequal(N("$text",3),"£"))
       S("currency") = "pounds";
if (N("$text",1))
	  S("nationality") = N("$text",1);
S("Numeral Value") = num(N("$text",4));
single();
@@POST
@RULES
_money <-
    _xWILD [opt match=("US" "Canadian" "British")] ### (2)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (3)
    _xWILD [s one matches=("\$" "£")]	### (1)
    _xNUM [one] ### (4)
    @@
@@RULES

#############################
# Rule 14                   #
# Percent expressions       #
#############################
@POST
S("Numeral Value") = N("Numeral Value",1);
S("fractional value") = N("fractional value",1);
single();
@@POST
@RULES
_percent <-
    _decimalNumber [one] ### (1)
    _xWILD [one match=("%")] ### (2)
    @@
@@RULES

@POST
S("Numeral Value") = num(N("$text",1));
S("fractional value") = 0;
single();
@@POST
@RULES
_percent <-
    _xNUM [one]	### (1)
    _xWILD [one match=("%")]	### (2)
    @@

@@RULES

##################################
# 15                             #
# basic currency                 #
##################################
@POST
S("fractional value") = num(N("$text",3));
S("Numeral Value") = N("Numeral Value",1);
S("currency") = N("currency",1);
single();
@@POST
@RULES
_money <-
    _money [one] ### (1)
    _xWILD [one matches=(".")] ### (2)
    _xNUM [one] ### (3)
    @@
@@RULES
		
##################################
# 16a                            #
# Currency expressions w dollar  #
# afterward                      #
##################################
@POST
S("Numeral Value") = N("Numeral Value",1);
S("fractional value") = N("fractional value",1);
S("currency") = N("$text",3);
single();
@@POST
@RULES
_money <-
    _decimalNumber [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [one matches=("dollar" "dollars" "pound" "pounds" "cent" "cents")] ### (3)
    @@
@@RULES
			
##################################
# 16b                            #
# Currency expressions           #
##################################
@POST
S("Numeral Value") = num(N("$text",1));
S("currency") = N("$text",3);
single();
@@POST
@RULES
_money <-
    _xNUM [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [one matches=("dollar" "dollars" "pound" "pounds" "cent" "cents")] ### (3)
    @@
@@RULES



@RULES
_item <-
    _xWILD [one match=("no")] ### (1)
    \. [one] ### (2)
    _whiteSpace [opt] ### (3)
    _xWILD [one match=(_xNUM "_decimalNumber" "_cardinalNumber")] ### (4)
    @@
@@RULES
