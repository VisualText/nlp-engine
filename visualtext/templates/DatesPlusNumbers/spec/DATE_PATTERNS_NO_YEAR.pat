 ###############################################
 # FILE: DATE_PATTERNS_NO_YEAR.pat             #
 # SUBJ: Recognize common date expressions w/o #
 #       a year specified                      #
 # AUTH: Paul Deane                            #
 # CREATED: 28/Feb/01 15:42:34                 #
 # DATE OF THIS VERSION: 31/Aug/01             #
 # Copyright 2001, Text Analysis International #
 ###############################################

@NODES _ROOT

@CHECK
   G("N1") = N("Numeral Value",3);
   if ( G("N1") < 1 || G("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",3);
	G("WordStr") = N("$text",9);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
       the [s one]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_ordinalNumeral")]
	_xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xWILD [s opt matches=("day" "days")] ### (5)
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	of [s opt]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	@@
@@RULES


@CHECK
   G("N1") = N("Numeral Value",3)[0];
   if ( G("N1") < 1 || G("N1") > 31) {
		fail();
	}
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",3);
	S("MaxArrayPos") = N("MaxArrayPos",3);
	G("WordStr") = N("$text",9);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
       the [s one]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_ordinalList" "_ordinalSequence")]
	_xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xWILD [s opt matches=("day" "days")] ### (5)
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	of [s opt]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	@@
@@RULES

@CHECK
   G("N1") = N("Numeral Value",5);
   G("lower") = 1;
   G("upper") = 31;
   if ( G("N1") < G("lower") || G("N1") > G("upper")) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",5);
	S("MaxArrayPos") = 0;
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	the [s opt]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_ordinalNumeral")]
	@@
@@RULES


@CHECK
   G("N1") = N("Numeral Value",5)[0];
   G("lower") = 1;
   G("upper") = 31;
   if ( G("N1") < G("lower") || G("N1") > G("upper")) {
		fail();
	}
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",5);
	S("MaxArrayPos") = N("MaxArrayPos",5);
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	the [s opt]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_ordinalList" "_ordinalSequence")]
	@@
@@RULES


@CHECK
   if ( !( N("Numeral Value",1) >= 1 && N("Numeral Value",1) <= 31)) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",1);
	G("WordStr") = N("$text",3);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"));
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
	single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one matches=("_cardinalNumeral")]
	_xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xWILD [s one match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	@@
@@RULES


@CHECK
   if ( !( N("Numeral Value",1)[0] >= 1 && N("Numeral Value",1)[0] <= 31)) {
		fail();
	}
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",1);
	S("MaxArrayPos") = N("MaxArrayPos",1);
	G("WordStr") = N("$text",3);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one matches=("_cardinalList" "_cardinalSequence")]
	_xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xWILD [s one match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	@@
@@RULES


@CHECK
   if ( !( num(N("$text",1)) >= 1 && num(N("$text",1)) <= 31)) {
		fail();
	}
@@CHECK
@POST
	S("Date Value") = num(N("$text",1));
	G("WordStr") = N("$text",3);
	S("MaxArrayPos") = 0;
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
   	_xNUM [s one]
	_xWILD [s one match=(_xWHITE "_whiteSpace")]
	_xWILD [s one match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	@@
@@RULES


@CHECK
   if ( N("Numeral Value",3) < 1) {
		fail();
	}
   else if ( N("Numeral Value",3) > 31) {
   		fail();
   }
@@CHECK
@POST
	S("Date Value") = N("Numeral Value",3);
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	S("MaxArrayPos") = 0;
single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_cardinalNumeral")]
	@@
@@RULES


@CHECK
   if ( N("Numeral Value",3)[0] < 1) {
		fail();
	}
   else if ( N("Numeral Value",3)[0] > 31) {
   		fail();
   }
@@CHECK
@POST
	S("Date Value List") = N("Numeral Value",3);
	S("MaxArrayPos") = N("MaxArrayPos",3);
	G("WordStr") = N("$text",1);
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s one match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	_xWILD [s opt match=(_xWHITE "_whiteSpace")]
	_xWILD [s one matches=("_cardinalList" "_cardinalSequence")]
	@@
@@RULES


@CHECK
   if ( num(N("$text",3)) < 1) {
		fail();
	}
   else if ( num(N("$text",3)) > 31) {
   		fail();
   }
@@CHECK
@POST
	S("Date Value") = num(N("$text",3));
	G("WordStr") = N("$text",1);
	S("MaxArrayPos") = 0;
	G("Word") = dictfindword(G("WordStr"));
	if (G("Word"))
       S("Month Value") = numval(G("Word"),"Month");
	single();
@@POST
@RULES
   _dateExpr [base] <-
	_xWILD [s min=1 max=1 match=("January" "Jan." "Jan" "February" "Feb." "Feb" "March" "April" "Apr." "Apr" "May" "June" "Ju." "July" "Jy." "Jy" "Jul." "Jul" "August" "Aug." "Aug" "September" "Sept." "Sept" "Sep." "Sep" "October" "Oct." "Oct" "November" "Nov." "Nov" "December" "Dec." "Dec")]
	_xWILD [s one match=(_xWHITE "_whiteSpace")]
   	_xNUM [s min=1 max=1]
	@@
@@RULES



