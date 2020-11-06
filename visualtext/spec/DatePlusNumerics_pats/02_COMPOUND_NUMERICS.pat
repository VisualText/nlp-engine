###############################################
# FILE: COMPOUND_NUMERICS.pat                 #
# SUBJ: Analyse word-based numerals           #
#       and extract the numeric values        #         
# AUTH: Paul Deane                            #
# CREATED: 27/Feb/01 17:16:33                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@POST
noop();
@@POST
@RULES
_xNIL <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    \- [opt] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _cardinalNumeral [one] ### (5)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (6)
    \- [one] ### (7)
    @@
_xNIL <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    \- [opt] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _ordinalNumeral [one] ### (5)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (6)
    \- [one] ### (7)
    @@
@@RULES

@CHECK
  if ( N("Numeral Value",1) != 100 && N("Numeral Value",1) != 200 && N("Numeral Value",1) != 300 && N("Numeral Value",1) != 400 && N("Numeral Value",1) != 500 && N("Numeral Value",1) != 600 && N("Numeral Value",1) != 700 && N("Numeral Value",1) != 800 && N("Numeral Value",1) != 900)
    	fail();
  if ( N("Numeral Value",5) >99)
       fail();
@@CHECK
@POST
   S("Numeral Value") = N("Numeral Value",1) + N("Numeral Value",3);
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [one] ### (5)
    @@

_ordinalNumeral <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _ordinalNumeral [one] ### (5)
    @@

@@RULES

@CHECK
  if ( N("Numeral Value",1) != 20 && N("Numeral Value",1) != 30 && N("Numeral Value",1) != 40 && N("Numeral Value",1) != 50 && N("Numeral Value",1) != 60 && N("Numeral Value",1) != 70 && N("Numeral Value",1) != 80 && N("Numeral Value",1) != 90)
  	fail();
  if ( N("Numeral Value",5) >9)
       fail();
@@CHECK
@POST
   S("Numeral Value") = N("Numeral Value",1) + N("Numeral Value",5);
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    \- [opt] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _cardinalNumeral [one] ### (5)
    @@

_ordinalNumeral <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    \- [opt] ### (3)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (4)
    _ordinalNumeral [one] ### (5)
    @@

@@RULES


@CHECK
  if ( N("Numeral Value",1) >100 )
  	fail();
  if ( N("Numeral Value",3) < 99 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",5) ;
   S("Numeral Value") =  G("num1") + G("num2") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [s opt match=("and" "&")] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _cardinalNumeral [s one] ### (5)
    @@

_ordinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [s opt match=("and" "&")] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _ordinalNumeral [s one] ### (5)
    @@

@@RULES

@CHECK
  if ( N("Numeral Value",1) <1 || N("Numeral Value",1)>999)
  	fail();
  if ( N("Numeral Value",3) != 100 && N("Numeral Value",3) != 1000 && N("Numeral Value",3) != 1000000 && N("Numeral Value",3) != 1000000000 && N("Numeral Value",3) != 1000000000000 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",3) ;
   S("Numeral Value") =  G("num1") * G("num2") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [one] ### (3)
    @@

_ordinalNumeral <-
    _cardinalNumeral [one] ### (1)
    _xWILD [opt match=(_xWHITE "_whiteSpace")] ### (2)
    _ordinalNumeral [one] ### (3)
    @@
@@RULES


  ######################
  #  (rule 4a)         #
  #  Decimal numbers   #
  ######################

  ############################
  # Rule 5                   #
  # Dotted telephone numbers #
  ############################
@CHECK
	if (strlength(N("$text",5)) != 4 ||
	    strlength(N("$text",3)) != 3 ||
	    strlength(N("$text",1)) != 3
	) {
		fail() ;
	}
	else succeed() ;
@@CHECK
@POST
	S("area code") = N("$text",1);
	S("local number") = N("$text",3) + N("$text",5);
	single() ;
@@POST
@RULES
_phoneNumber <-
    _xNUM [one]	### (1)
    \. [one]	### (2)
    _xNUM [one]	### (3)
    \. [one]	### (4)
    _xNUM [one]	### (5)
    @@
@@RULES

@POST
S("NumberOfFields") = 3;
single();
@@POST
@RULES
_dottedNumber <-
    _xNUM [one] ### (1)
    \. [one] ### (2)
    _xNUM [one] ### (3)
    \. [one] ### (2)
    _xNUM [one] ### (3)
    @@
@@RULES

@POST
S("NumberOfFields") = N("NumberOfFields",1)+1;
listadd(1,3,"true");
@@POST
@RULES
_dottedNumber <-
	_dottedNumber [one]
	\. [one]
	_xNUM [one]
	@@
@@RULES

@POST
	S("Numeral Value") = num(N("$text",1)) ;
	S("fractional value") = num(N("$text",3)) ;
	single() ;
@@POST
@RULES
_decimalNumber <-
    _xNUM [one] ### (1)
    \. [one] ### (2)
    _xNUM [one] ### (3)
    @@
@@RULES

  ######################
  #  (rule 4b)         #
  # Numbers with commas#
  ######################

@POST
	S("Numeral Value") = 1000*num(N("$text",1)) + num(N("$text",3)) ;
	single() ;
@@POST
@RULES
_decimalNumber <-
    _xNUM [one] ### (1)
    _xWILD [one match=(",")] ### (2)
    _xNUM [one] ### (3)
    @@
@@RULES

@POST
	S("Numeral Value") = 1000*N("Numeral Value",1) + num(N("$text",3));	
	single();
@@POST
@RULES
_decimalNumber <-
    _decimalNumber ### (1)
    \, [one] ### (2)
    _xNUM [one] ### (3)
    @@
@@RULES

