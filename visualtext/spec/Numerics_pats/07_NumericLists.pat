 ###############################################
 # FILE: Numeric Lists.pat                     #
 # SUBJ: Recognized coordinate NPs built from  #
 #       numbers                               #
 # AUTH: Paul Deane                            #
 # CREATED: 01/Mar/01 15:52:37                 #
 # DATE OF THIS VERSION: 31/Aug/01             #
 # Copyright 2001, Text Analysis International #
 ###############################################

@NODES _ROOT

@RULES

@POST
	S("Numeral Value")[0] = N("Numeral Value",1);
	S("MaxArrayPos") = 1;
	S("MaxValue") = N("MaxArrayPos",4)+1 ;
	S("CurrentValue") = 0 ;
	while ( S("MaxArrayPos") < S("MaxValue") ) {
		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",4)[S("CurrentValue")] ;
		S("MaxArrayPos")++;
		S("CurrentValue")++;
	}
	S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",4)[S("CurrentValue")] ;
	S("MaxArrayPos") = S("MaxArrayPos");
	single();
@@POST
@RULES
_cardinalList <-
	_cardinalNumeral [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalList [one]
	@@
@@RULES
@POST
	S("Numeral Value")[0] = num(N("$text",1));
	S("MaxArrayPos") = 1;
	S("MaxValue") = N("MaxArrayPos",4) ;
	S("CurrentValue") = 0 ;
	while ( S("MinValue") < S("MaxValue") ) {
		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value")[S("CurrentValue")] ;
		S("MaxArrayPos")++;
		S("MinValue")++;
		S("CurrentValue")++;
	}
	S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",4)[S("CurrentValue")] ;
	S("MaxArrayPos") = S("MaxArrayPos");
	single();
@@POST
@RULES
_cardinalList <-
	_xNUM [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalList [one]
	@@
@@RULES
@POST
	#add the sequence of values explicitly to the array for the first sequence
	S("Numeral Value")[0] = N("Numeral Value",1)[0];
	S("MaxArrayPos") = 1;
	S("MinValue") = N("Numeral Value",1)[0] ;
	S("CurrentValue") = N("Numeral Value",1)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",1)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
 		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",1)[1] ;
       }
	else {
		S("Numeral Value")[1] = N("Numeral Value",1)[1];
		S("MaxArrayPos") = 1;
	}
	S("FirstMax") = S("MaxArrayPos");
	#add the sequence of values to the array for the list
	S("MaxArrayPos") = 0;
	S("MinValue") = 0 ;
	S("MaxValue") = N("MaxValue",4)+1;
	while ( S("MaxArrayPos") < S("MaxValue") ) {
			S("Numeral Value")[S("FirstMax") + S("MaxArrayPos")] =  N("Numeral Value",4)[S("MaxArrayPos")] ;
			S("MaxArrayPos")++;
		}
 		S("Numeral Value")[S("FirstMax") + S("MaxArrayPos")] = N("Numeral Value",4)[S("MaxArrayPos")] ;
	S("MaxArrayPos") = S("FirstMax") +  S("MaxArrayPos");
	single();
@@POST
@RULES
_cardinalList <-
	_cardinalSequence [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalList [one]
	@@
@@RULES
@POST
	#add the sequence of values explicitly to the array for the first sequence
	S("Numeral Value")[0] = N("Numeral Value",1)[0];
	S("MaxArrayPos") = 1;
	S("MinValue") = N("Numeral Value",1)[0] ;
	S("CurrentValue") = N("Numeral Value",1)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",1)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
 		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",1)[1] ;
       }
	else {
		S("Numeral Value")[1] = N("Numeral Value",1)[1];
		S("MaxArrayPos") = 1;
	}
	S("MaxArrayPos") = S("MaxArrayPos")+1;

	#add the sequence of values explicitly to the array for the second sequence
	S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",6)[0];
	S("MaxArrayPos")++;
	S("MinValue") = N("Numeral Value",6)[0] ;
	S("CurrentValue") = N("Numeral Value",6)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",6)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
 		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",6)[1] ;
       }
	else {
		S("Numeral Value")[2] = N("Numeral Value",6)[1];
		S("MaxArrayPos") = 2;
	}
	S("MaxArrayPos") = S("MaxArrayPos")+1;
	single();
@@POST
@RULES
_cardinalList <-
	_cardinalSequence [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalSequence [one]
	@@
@@RULES
@POST
	#add the lone numeral
	S("Numeral Value")[0] = N("Numeral Value",1);

	#add the sequence of values explicitly to the array
	S("Numeral Value")[1] = N("Numeral Value",6)[0];
	S("MaxArrayPos") = 2;
	S("MinValue") = N("Numeral Value",6)[0] ;
	S("CurrentValue") = N("Numeral Value",6)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",6)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
       }
	else {
		S("Numeral Value")[2] = N("Numeral Value",6)[1];
		S("MaxArrayPos") = 2;
	}
	single();
@@POST
@RULES
_cardinalList <-
	_cardinalNumeral [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalSequence [one]
	@@


@@RULES
@POST
	#add the lone numeral
	S("Numeral Value")[0] = num(N("$text",1));

	#add the sequence of values explicitly to the array
	S("Numeral Value")[1] = N("Numeral Value",6)[0];
	S("MaxArrayPos") = 2;
	S("MinValue") = N("Numeral Value",6)[0] ;
	S("CurrentValue") = N("Numeral Value",6)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",6)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
       }
	else {
		S("Numeral Value")[2] = N("Numeral Value",6)[1];
		S("MaxArrayPos") = 2;
	}
	S("MaxArrayPos") = S("MaxArrayPos")+1;
	single();
@@POST
@RULES
_cardinalList <-
	_xNUM [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalSequence [one]
	@@
@@RULES
@POST
	S("Numeral Value")[0] = N("Numeral Value",1)[0];
	S("MaxArrayPos") = 1;
	S("MinValue") = N("Numeral Value",1)[0] ;
	S("CurrentValue") = N("Numeral Value",1)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",1)[1];
	#add the sequence of values explicitly to the array
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
       }
	else {
		S("Numeral Value")[1] = N("Numeral Value",1)[1];
		S("MaxArrayPos") = 0;
	}
	#add the lone numeral
	S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",6);
	single();
@@POST
@RULES
_cardinalList <-
	_cardinalSequence [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalNumeral [one]
	@@
@@RULES
@POST
	S("Numeral Value")[0] = N("Numeral Value",1)[0];
	S("MaxArrayPos") = 1;
	S("MinValue") = N("Numeral Value",1)[0] ;
	S("CurrentValue") = N("Numeral Value",1)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",1)[1];
	#add the sequence of values explicitly to the array
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
 		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",1)[1] ;
       }
	else {
		S("Numeral Value")[1] = N("Numeral Value",1)[1];
		S("MaxArrayPos") = 1;
	}
	#add the lone numeral
	S("Numeral Value")[S("MaxArrayPos")] = num(N("$text",6));
	single();
@@POST
@RULES
_cardinalList <-
	_cardinalSequence [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xNUM [one]
	@@
@@RULES
@POST
	S("Numeral Value")[0] = N("Numeral Value",1);
	S("Numeral Value")[1] = N("Numeral Value",6);
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_cardinalList <-
	_cardinalNumeral [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_cardinalNumeral [one]
	@@
@@RULES
@POST
	S("Numeral Value")[0] = num(N("$text",1));
	S("Numeral Value")[1] = num(N("$text",6));
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_cardinalList <-
	_xNUM [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xNUM [one]
	@@
@@RULES
@POST
	S("Numeral Value")[0] = N("Numeral Value",1);
	S("MaxArrayPos") = 1;
	S("MaxValue") = N("MaxArrayPos",4) ;
	S("CurrentValue") = 0 ;
	while ( S("MinValue") < S("MaxValue") ) {
		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value")[S("CurrentValue")] ;
		S("MaxArrayPos")++;
		S("MinValue")++;
		S("CurrentValue")++;
	}
	S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value")[S("CurrentValue")] ;
	S("MaxArrayPos") = S("MaxArrayPos");
	single();
@@POST
@RULES
_ordinalList <-
	_ordinalNumeral [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_ordinalList [one]
	@@
@@RULES
@POST
	#add the sequence of values explicitly to the array for the first sequence
	S("Numeral Value")[0] = N("Numeral Value",1)[0];
	S("MaxArrayPos") = 1;
	S("MinValue") = N("Numeral Value",1)[0] ;
	S("CurrentValue") = N("Numeral Value",1)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",1)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
 		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",1)[1] ;
       }
	else {
		S("Numeral Value")[1] = N("Numeral Value",1)[1];
		S("MaxArrayPos") = 1;
	}
	S("FirstMax") = S("MaxArrayPos");
	#add the sequence of values to the array for the list
	S("MaxArrayPos") = 0;
	S("MinValue") = 0 ;
	S("MaxValue") = N("MaxValue",4)+1;
	while ( S("MaxArrayPos") < S("MaxValue") ) {
			S("Numeral Value")[S("FirstMax") + S("MaxArrayPos")] =  N("Numeral Value",4)[S("MaxArrayPos")] ;
			S("MaxArrayPos")++;
		}
 		S("Numeral Value")[S("FirstMax") + S("MaxArrayPos")] = N("Numeral Value",4)[S("MaxArrayPos")] ;
	S("MaxArrayPos") = S("FirstMax") +  S("MaxArrayPos");
	single();
@@POST
@RULES
_ordinalList <-
	_ordinalSequence [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_ordinalList [one]
	@@
@@RULES
@POST
	#add the sequence of values explicitly to the array for the first sequence
	S("Numeral Value")[0] = N("Numeral Value",1)[0];
	S("MaxArrayPos") = 1;
	S("MinValue") = N("Numeral Value",1)[0] ;
	S("CurrentValue") = N("Numeral Value",1)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",1)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
 		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",1)[1] ;
       }
	else {
		S("Numeral Value")[1] = N("Numeral Value",1)[1];
		S("MaxArrayPos") = 1;
	}
	S("MaxArrayPos") = S("MaxArrayPos")+1;

	#add the sequence of values explicitly to the array for the second sequence
	S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",6)[0];
	S("MaxArrayPos")++;
	S("MinValue") = N("Numeral Value",6)[0] ;
	S("CurrentValue") = N("Numeral Value",6)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",6)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
 		S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",6)[1] ;
       }
	else {
		S("Numeral Value")[2] = N("Numeral Value",6)[1];
		S("MaxArrayPos") = 2;
	}
	S("MaxArrayPos") = S("MaxArrayPos")+1;
	single();
@@POST
@RULES
_ordinalList <-
	_ordinalSequence [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_ordinalSequence [one]
	@@
@@RULES
@POST
	#add the lone numeral
	S("Numeral Value")[0] = N("Numeral Value",1);

	#add the sequence of values explicitly to the array
	S("Numeral Value")[1] = N("Numeral Value",6)[0];
	S("MaxArrayPos") = 2;
	S("MinValue") = N("Numeral Value",6)[0] ;
	S("CurrentValue") = N("Numeral Value",6)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",6)[1];
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
       }
	else {
		S("Numeral Value")[2] = N("Numeral Value",6)[1];
		S("MaxArrayPos") = 2;
	}
	S("MaxArrayPos") = S("MaxArrayPos")+1;
	single();
@@POST
@RULES
_ordinalList <-
	_ordinalNumeral [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_ordinalSequence [one]
	@@
@@RULES

@POST
	S("Numeral Value")[0] = N("Numeral Value",1)[0];
	S("MaxArrayPos") = 1;
	S("MinValue") = N("Numeral Value",1)[0] ;
	S("CurrentValue") = N("Numeral Value",1)[0] + 1 ;
	S("MaxValue") = N("Numeral Value",1)[1];
	#add the sequence of values explicitly to the array
       if ( S("MinValue") < S("MaxValue")) {
		while ( S("MinValue") < S("MaxValue") ) {
			S("Numeral Value")[S("MaxArrayPos")] = S("CurrentValue") ;
			S("MaxArrayPos")++;
			S("MinValue")++;
			S("CurrentValue")++;
		}
       }
	else {
		S("Numeral Value")[1] = N("Numeral Value",1)[1];
		S("MaxArrayPos") = 1;
	}
	#add the lone numeral
	S("Numeral Value")[S("MaxArrayPos")] = N("Numeral Value",6);
	single();
@@POST
@RULES
_ordinalList <-
	_ordinalSequence [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_ordinalNumeral [one]
	@@
@@RULES

@POST
	S("Numeral Value")[0] = N("Numeral Value",1);
	S("Numeral Value")[1] = N("Numeral Value",6);
	S("MaxArrayPos") = 1;
	single();
@@POST
@RULES
_ordinalList <-
	_ordinalNumeral [one]
	\, [one]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_xWILD [one matches=("&" "and" "or")]
      _xWILD [opt match=(_xWHITE "_whiteSpace")]
	_ordinalNumeral [one]
	@@
@@RULES
