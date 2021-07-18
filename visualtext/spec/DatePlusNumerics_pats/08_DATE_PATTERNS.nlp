 ###############################################
 # FILE: DATE_PATTERNS.pat                     #
 # SUBJ: Recognize date expressions with year  #
 # AUTH: Paul Deane                            #
 # CREATED: 26/Feb/01 15:00:01                 #
 # DATE OF THIS VERSION: 31/Aug/01             #
 # Copyright 2001, Text Analysis International #
 ###############################################
	
@NODES _ROOT

@CHECK
   S("N1") = N("Numeral Value",3) ;
   if (S("N1")) {
		   if ( S("N1") < 1 && S("N1") > 31) {
			fail();
		}
   }
   if ( strlength(N("$text",12)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",3);
	S("Year Value") = num(N("$text",12));
	G("WordStr") = N("$text",9);
	S("MaxArrayPos") = 0;
	G("Word") = dictfindword(G("WordStr"));
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
_dateExpr [base] <-
    the [s opt]### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [s one matches=("_ordinalNumeral")] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt matches=("day" "days")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    of [s opt] ### (7)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (8)
    _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
    \, [s opt] ### (10)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (11)
    _xNUM [s min=1 max=1] ### (12)
    @@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",3)[0] ;
   if (S("N1")) {
		   if ( S("N1") < 1 && S("N1") > 31) {
			fail();
		}
   }
   if ( strlength(N("$text",12)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",3);
	S("Year Value") = num(N("$text",12));
	G("WordStr") = N("$text",9);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = N("MaxArrayPos",3);
	single();
@@POST
@RULES
_dateExpr [base] <-
    the [s opt]### (1)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [s one matches=("_ordinalList" "_ordinalSequence")] ### (3)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [s opt matches=("day" "days")] ### (5)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (6)
    of [s opt] ### (7)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (8)
    _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
    \, [s opt] ### (10)
    _xWILD [s opt match=(_xWHITE "_whiteSpace")] ### (11)
    _xNUM [s min=1 max=1] ### (12)
    @@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",3);
   if (S("N1")) {
	   if ( S("N1") < 1 || S("N1") > 31) {
			fail();
		}
   }
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",3);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",12);
	G("WordStr") = N("$text",9);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
_dateExpr [base] <-
       the [s opt]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
    	_xWILD [s one matches=("_ordinalNumeral")]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s opt matches=("day" "days")] ### (5)
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
	of [s opt]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
       _xWILD [s min=1 max=1 match=("January" "Jan" "February" "Feb" "March" "April" "Apr" "May" "June" "July" "Jy" "Jul" "August" "Aug" "September" "Sept" "Sep" "October" "Oct" "November" "Nov" "December" "Dec")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",3)[0];
   if (S("N1")) {
	   if ( S("N1") < 1 || S("N1") > 31) {
			fail();
		}
   }
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",3);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",12);
	G("WordStr") = N("$text",9);
	S("MaxArrayPos") = N("MaxArrayPos",3);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
_dateExpr [base] <-
       the [s opt]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
    	_xWILD [s one matches=("_ordinalList" "_ordinalSequence")]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s opt matches=("day" "days")] ### (5)
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
	of [s opt]
	_whiteSpace [s opt]
       _xWILD [s min=1 max=1 match=("January" "Jan" "February" "Feb" "March" "April" "Apr" "May" "June" "July" "Jy" "Jul" "August" "Aug" "September" "Sept" "Sep" "October" "Oct" "November" "Nov" "December" "Dec")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",5);
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",8)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",5);
	S("Year Value") = num(N("$text",8));
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	the [s opt]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
    	_xWILD [s one matches=("_ordinalNumeral")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xNUM [s one]
	@@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",5)[0];
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",8)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",5);
	S("Year Value") = num(N("$text",8));
	G("WordStr") = N("$text",1);
	S("MaxArrayPos") = N("MaxArrayPos",5);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	the [s opt] 
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
    	_xWILD [s one matches=("_ordinalList" "_ordinalSequence")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xNUM [s one]
	@@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",5);
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",5);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",8);
	G("WordStr") = N("$text",1);
	G("Word") = 0;
	G("Word") = dictfindword(G("WordStr"));
	if (G("word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan" "February" "Feb" "March" "April" "Apr" "May" "June" "Ju" "July" "Jy" "Jul" "August" "Aug" "September" "Sept" "Sep" "October" "Oct" "November" "Nov" "December" "Dec")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	the [s opt]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
    	_xWILD [s one matches=("_ordinalNumeral")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",5)[0];
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",5);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",8);
	G("WordStr") = N("$text",1);
	G("Word") = 0;
	S("MaxArrayPos") = N("MaxArrayPos",5);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan" "February" "Feb" "March" "April" "Apr" "May" "June" "Ju" "July" "Jy" "Jul" "August" "Aug" "September" "Sept" "Sep" "October" "Oct" "November" "Nov" "December" "Dec")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	the [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
    	_xWILD [s one matches=("_ordinalList" "_ordinalSequence")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES

@CHECK
   S("N1") = N("Numeral Value",1);
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",6)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",1);
	S("Year Value") = num(N("$text",6));
	G("WordStr") = N("$text",3);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
_dateExpr [base] <-
    _xWILD [s one matches=("_cardinalNumeral")]
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
    \, [s opt] ### (4)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (5)
    _xNUM [s one] ### (6)
    @@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",1)[0];
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",6)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",1);
	S("Year Value") = num(N("$text",6));
	G("WordStr") = N("$text",3);
	G("Word") = dictfindword(G("WordStr"));
	S("MaxArrayPos") = N("MaxArrayPos",1);
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
_dateExpr [base] <-
    _xWILD [s one matches=("_cardinalList" "_cardinalSequence")]
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
    \, [s opt] ### (4)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (5)
    _xNUM [s one] ### (6)
    @@
@@RULES

@CHECK
   S("N1") = N("Numeral Value",1);
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",1);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",6);
	G("WordStr") = N("$text",3);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       G("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one matches=("_cardinalNumeral")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES


@CHECK
   S("N1") = N("Numeral Value",1)[0];
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",1);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",6);
	G("WordStr") = N("$text",3);
	S("MaxArrayPos") = N("MaxArrayPos",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       G("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one matches=("_cardinalList" "_cardinalSequence")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES

@CHECK
   S("N1") = num(N("$text",1));
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",6)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value") = num(N("$text",1));
	S("Year Value") = num(N("$text",6));
	G("WordStr") = N("$text",3);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
_dateExpr [base] <-
    _xNUM [s one] ### (1)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (2)
    _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
    \, [s opt] ### (4)
    _xWILD [s one match=(_xWHITE "_whiteSpace")] ### (5)
    _xNUM [s one] ### (6)
    @@
@@RULES

@CHECK
   S("N1") = num(N("$text",1));
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = num(N("$text",1));
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",6);
	G("WordStr") = N("$text",3);
	G("Word") = dictfindword(G("WordStr"));
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
   	_xNUM [s one]
       _xWILD [s one match=(_xWHITE "_whiteSpace")]
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES

@CHECK
   S("N1") = N("Numeral Value",3);
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",6)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",3);
	S("Year Value") = num(N("$text",6));
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr <-
       _xWILD [s one match=("January" "Jan" "February" "Feb" "March" "April" "Apr" "May" "June" "July" "Jy" "Jul" "August" "Aug" "September" "Sept" "Sep" "October" "Oct" "November" "Nov" "December" "Dec")]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_cardinalNumeral")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xNUM [s one]
	@@
@@RULES


@CHECK
   S("N1") = num(N("$text",3));
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",6)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",3);
	S("Year Value") = num(N("$text",6));
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	S("MaxArrayPos") = N("MaxArrayPos",3);
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr <-
       _xWILD [s one match=("January" "Jan" "February" "Feb" "March" "April" "Apr" "May" "June" "July" "Jy" "Jul" "August" "Aug" "September" "Sept" "Sep" "October" "Oct" "November" "Nov" "December" "Dec")]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_cardinalList" "_cardinalSequence")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xNUM [s one]
	@@
@@RULES

@CHECK
   S("N0") = N("Numeral Value",3);
   S("N1") = S("N0");
   if ( S("N1") < 1 )
        fail();
   if ( S("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",3);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",6);
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word")) {
	       S("Month Value") = numval(G("Word"),"Month");
	}
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
      _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
      _xWILD [s one matches=("_cardinalNumeral")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [ s one]
	@@
@@RULES


@CHECK
   S("N0") = N("$text",3);
   S("N1") = S("N0");
   if ( S("N1") < 1 )
        fail();
   if ( S("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",3);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",6);
	G("WordStr") = N("$text",1);
	S("MaxArrayPos") = N("MaxArrayPos",3);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word")) {
	       S("Month Value") = numval(G("Word"),"Month");
	}
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
      _xWILD [s opt match=(_xWHITE "_whiteSpace")]
       _xWILD [s one matches=("_cardinalList" "_cardinalSequence")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [ s one]
	@@
@@RULES


@CHECK
   S("N1") = num(N("$text",3));
   if ( S("N1") < 1 || S("N1") > 31) {
		fail();
	}
   if ( strlength(N("$text",6)) != 4 ) {
   		fail(); 
   }
@@CHECK
@POST
	S("Date Value") = num(N("$text",3));
	S("Year Value") = num(N("$text",6));
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
   	_xNUM [s one]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xNUM [s one]
	@@
@@RULES

@CHECK
if (N("$text",4)) {
   if ( strlength(N("$text",4)) != 4 ) {
   		fail(); 
   }
}
@@CHECK
@POST
	S("Date Value") = 0;
	S("Year Value") = num(N("$text",4));
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xNUM [s one]
	@@
@@RULES

@CHECK
   if (N("$text",3)) {
	   S("N1") = num(N("$text",3));
	   if ( S("N1") < 1 || S("N1") > 31) {
			fail();
		}
   }
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",3);
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",6);
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	G("attrname") = "Month";
	if (G("Word"))
       S("Month Value") = numval(G("Word"),G("attrname"));
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
   	_xNUM [s one]
	\, [s opt]
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES

@POST
	S("Date Value") = 0;
	S("Year Value") = 0;
	S("YearInCentury") = N("YearInCentury",4);
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	G("attrname") = "Month";
       S("Month Value") = numval(G("Word"),G("attrname"));
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
       _xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	\, [s opt] 
      _xWILD [s one match=(_xWHITE "_whiteSpace")]
	_yearAbbr [s one]
	@@
@@RULES
