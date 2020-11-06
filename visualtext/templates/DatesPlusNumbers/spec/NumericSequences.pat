 ################################################
 # FILE: Numeric Sequences.pat                  #
 # SUBJ: Recognize numeric sequence expressions #
 #       like 5-15, or between 5 and 15         #
 # AUTH: Paul Deane                             #
 # CREATED: 01/Mar/01 12:51:29                  #
 # DATE OF THIS VERSION: 31/Aug/01              #
 ################################################

@NODES _ROOT

@POST
	S("Numeral Value")[0] = num(N("$text",3));
	S("Numeral Value")[1] = num(N("$text",7));
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_cardinalSequence <-
    _xWILD [s one match=("between")] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _xNUM [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s one match=("&" "and")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _xNUM [s one] ### (7)
    @@
@@RULES
@POST
	S("Numeral Value")[0] = num(N("Numeral Value",3));
	S("Numeral Value")[1] = num(N("Numeral Value",7));
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_cardinalSequence <-
    _xWILD [s one match=("between")] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s one match=("&" "and")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _cardinalNumeral [s one] ### (7)
    @@
@@RULES

@POST
	S("Numeral Value")[0] = num(N("Numeral Value",5));
	S("Numeral Value")[1] = num(N("Numeral Value",11));
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_ordinalSequence <-
    _xWILD [s one match=("between")] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    the [s opt] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _ordinalNumeral [s one] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _xWILD [s one match=("&" "and")] ### (7)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (8)
    the [s opt] ### (9)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (10)
    _ordinalNumeral [s one] ### (11)
    @@
@@RULES

@POST
	S("Numeral Value")[0] = num(N("$text",3));
	S("Numeral Value")[1] = num(N("$text",7));
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_cardinalSequence <-
    _xWILD [s opt match=("from")] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _xNUM [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s one match=("-" "to" "through")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _xNUM [s one] ### (7)
    @@
@@RULES
@POST
	S("Numeral Value")[0] = num(N("Numeral Value",3));
	S("Numeral Value")[1] = num(N("Numeral Value",7));
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_cardinalSequence <-
    _xWILD [s opt match=("from")] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _cardinalNumeral [s one] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s one match=("to" "through")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _cardinalNumeral [s one] ### (7)
    @@
@@RULES

@POST
	S("Numeral Value")[0] = num(N("Numeral Value",5));
	S("Numeral Value")[1] = num(N("Numeral Value",11));
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_ordinalSequence <-
    _xWILD [s opt match=("from")] ### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    the [s opt] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _ordinalNumeral [s one] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    _xWILD [s one match=("-" "to" "through")] ### (7)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (8)
    the [s opt] ### (9)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (10)
    _ordinalNumeral [s one] ### (11)
    @@

@@RULES
