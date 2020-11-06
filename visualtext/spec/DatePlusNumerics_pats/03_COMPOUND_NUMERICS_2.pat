###############################################
# FILE: COMPOUND_NUMERICS_2.pat               #
# SUBJ: Analyse complex word-based numerals   #
#       and extract the numeric values        #         
# AUTH: Paul Deane                            #
# CREATED: 27/Feb/01 22:18:53                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@CHECK
  if ( N("Numeral Value",1) <1000000)
  	fail();
  if ( N("Numeral Value",3) > 999999 )
       fail();
  if ( N("Numeral Value",5) > 999 )
       fail();
  if ( N("Numeral Value",9) > 99 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",3) ;
   G("num3") = N("Numeral Value",5) ;
   G("num4") = N("Numeral Value",9) ;
   S("Numeral Value") =  G("num1") + G("num2") + G("num3") + G("num4") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (4)
    _cardinalNumeral [s one] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _xWILD [s opt match=("and" "&")] ### (7)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (8)
    _cardinalNumeral [s one] ### (9)
    @@

_ordinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (4)
    _cardinalNumeral [s one] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _xWILD [s opt match=("and" "&")] ### (7)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (8)
    _ordinalNumeral [s one] ### (9)
    @@

@@RULES

@CHECK
  if ( N("Numeral Value",1) <1000)
  	fail();
  if ( N("Numeral Value",3) > 999 )
       fail();
  if ( N("Numeral Value",7) > 99 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",3) ;
   G("num3") = N("Numeral Value",7) ;
   S("Numeral Value") =  G("num1") + G("num2") + G("num3") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt match=("and" "&")] ### (5)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (6)
    _cardinalNumeral [s one] ### (7)
    @@

_ordinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt match=("and" "&")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _ordinalNumeral [s min=1 max=1] ### (7)
    @@

@@RULES

@CHECK
  if ( N("Numeral Value",1) <1000000)
  	fail();
  if ( N("Numeral Value",3) > 999999 )
       fail();
  if ( N("Numeral Value",7) > 99 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",3) ;
   G("num3") = N("Numeral Value",7) ;
   S("Numeral Value") =  G("num1") + G("num2") + G("num3") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt match=("and" "&")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _cardinalNumeral [s one] ### (7)
    @@

_ordinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt match=("and" "&")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _ordinalNumeral [s one] ### (7)
    @@

@@RULES

@CHECK
  if ( N("Numeral Value",1) <1000)
  	fail();
  if ( N("Numeral Value",3) > 999 )
       fail();
  if ( N("Numeral Value",7) > 99 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",3) ;
   G("num3") = N("Numeral Value",7) ;
   S("Numeral Value") =  G("num1") + G("num2") + G("num3") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt match=("and" "&")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _cardinalNumeral [s one] ### (7)
    @@

_ordinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt match=("and" "&")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _ordinalNumeral [s one] ### (7)
    @@

@@RULES


@CHECK
  if ( N("Numeral Value",1) <1000000 )
  	fail();
  if ( N("Numeral Value",3) > 999999 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",3) ;
   S("Numeral Value") =  G("num1") + G("num2") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    @@

_ordinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _ordinalNumeral [s one] ### (3)
    @@

@@RULES

@CHECK
  if ( N("Numeral Value",1) <1000 )
  	fail();
  if ( N("Numeral Value",3) > 999 )
       fail();
@@CHECK
@POST
   G("num1") = N("Numeral Value",1) ;
   G("num2") = N("Numeral Value",3) ;
   S("Numeral Value") =  G("num1") + G("num2") ;
   single();
@@POST
@RULES
_cardinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    @@

_ordinalNumeral <-
    _cardinalNumeral [s one] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _ordinalNumeral [s one] ### (3)
    @@

@@RULES


@CHECK
  if ( N("Numeral Value",1) <100 )
  	fail();
  if ( N("Numeral Value",3) > 99 )
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

