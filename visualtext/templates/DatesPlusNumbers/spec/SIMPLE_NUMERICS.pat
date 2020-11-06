 ##################################################
 # FILE: SIMPLE_NUMERICS.pat                      #
 # SUBJ: Analyse simple numeric words as numerals #
 # AUTH: Paul Deane                               #
 # CREATED: 04/Jan/01 11:29:43                    #
 # DATE OF THIS VERSION: 31/Aug/01                #
 # Copyright 2001, Text Analysis International    #
 ##################################################

@CODE
	G("word") = addword("January");
	addnumval(G("word"),"Month",1);
	G("word") = addword("Jan");
	addnumval(G("word"),"Month",1);
	G("word") = addword("February");
	addnumval(G("word"),"Month",2);
	G("word") = addword("Feb");
	addnumval(G("word"),"Month",2);
	G("word") = addword("March");
	addnumval(G("word"),"Month",3);
	G("word") = addword("Mar");
	addnumval(G("word"),"Month",3);
	G("word") = addword("April");
	addnumval(G("word"),"Month",4);
	G("word") = addword("Apr");
	addnumval(G("word"),"Month",4);
	G("word") = addword("May");
	addnumval(G("word"),"Month",5);
	G("word") = addword("June");
	addnumval(G("word"),"Month",6);
	G("word") = addword("Ju");
	addnumval(G("word"),"Month",6);
	G("word") = addword("July");
	addnumval(G("word"),"Month",7);
	G("word") = addword("Jy");
	addnumval(G("word"),"Month",7);
	G("word") = addword("August");
	addnumval(G("word"),"Month",8);
	G("word") = addword("Aug");
	addnumval(G("word"),"Month",8);
	G("word") = addword("September");
	addnumval(G("word"),"Month",9);
	G("word") = addword("Sept");
	addnumval(G("word"),"Month",9);
	G("word") = addword("Sep");
	addnumval(G("word"),"Month",9);
	G("word") = addword("October");
	addnumval(G("word"),"Month",10);
	G("word") = addword("Oct");
	addnumval(G("word"),"Month",10);
	G("word") = addword("November");
	addnumval(G("word"),"Month",11);
	G("word") = addword("Nov");
	addnumval(G("word"),"Month",11);
	G("word") = addword("December");
	addnumval(G("word"),"Month",12);
	G("word") = addword("Dec");
	addnumval(G("word"),"Month",12);
	

@@CODE

@NODES _ROOT

@POST
G("stem") = strtolower(N("$text",5));
if (strequal(strtolower(G("stem")),"hundred")) {
	G("Numeral Value") = 100;
}
else if (strequal(strtolower(G("stem")),"thousand")) {
	G("Numeral Value") = 1000;
}
else if (strequal(strtolower(G("stem")),"million")) {
	G("Numeral Value") = 1000000;
}
else if (strequal(strtolower(G("stem")),"billion")) {
	G("Numeral Value") = 1000000000;
}
else if (strequal(strtolower(G("stem")),"trillion")) {
	G("Numeral Value") = 1000000000000;
}
S("Numeral Value") = num(N("$text",1)) * G("Numeral Value");
single();
@@POST
@RULES
_cardinalNumeral <-
    _xNUM [one] ### (1)
    \. [opt] ### (2)
    _xNUM [opt] ### (3)
    _xWILD [star match=(_xWHITE "_whiteSpace")] ### (4)
    _xWILD [one match=("hundred" "thousand" "million" "billion" "trillion")] ### (5)
    @@
@@RULES

#######################
# Ordinal with integer#
#######################

@POST
G("numeralT") = N("$text",1);
S("Numeral Value") = num(G("numeralT"));
single();
@@POST
@RULES
_ordinalNumeral <-
    _xNUM [s one]	### (1)
    _xWILD [s min=1 max=1 match=("st" "nd" "rd" "th")]	### (2)
    @@
@@RULES

#####################
# One word ordinals #
#####################
@POST
G("stem") = N("$text",1);
if (strequal(strtolower(G("stem")),"first")) {
	S("Numeral Value") = 1;
}
else if (strequal(strtolower(G("stem")),"second")) {
	S("Numeral Value") = 2;
}
else if (strequal(strtolower(G("stem")),"third")) {
	S("Numeral Value") = 3;
}
else if (strequal(strtolower(G("stem")),"fourth")) {
	S("Numeral Value") = 4;
}
else if (strequal(strtolower(G("stem")),"fifth")) {
	S("Numeral Value") = 5;
}
else if (strequal(strtolower(G("stem")),"sixth")) {
	S("Numeral Value") = 6;
}
else if (strequal(strtolower(G("stem")),"seventh")) {
	S("Numeral Value") = 7;
}
else if (strequal(strtolower(G("stem")),"eighth")) {
	S("Numeral Value") = 8;
}
else if (strequal(strtolower(G("stem")),"ninth")) {
	S("Numeral Value") = 9;
}
else if (strequal(strtolower(G("stem")),"tenth")) {
	S("Numeral Value") = 10;
}
else if (strequal(strtolower(G("stem")),"eleventh")) {
	S("Numeral Value") = 11;
}
else if (strequal(strtolower(G("stem")),"twelfth")) {
	S("Numeral Value") = 12;
}
else if (strequal(strtolower(G("stem")),"thirteenth")) {
	S("Numeral Value") = 13;
}
else if (strequal(strtolower(G("stem")),"fourteenth")) {
	S("Numeral Value") = 14;
}
else if (strequal(strtolower(G("stem")),"fifteenth")) {
	S("Numeral Value") = 15;
}
else if (strequal(strtolower(G("stem")),"sixteenth")) {
	S("Numeral Value") = 16;
}
else if (strequal(strtolower(G("stem")),"seventeenth")) {
	S("Numeral Value") = 17;
}
else if (strequal(strtolower(G("stem")),"eighteenth")) {
	S("Numeral Value") = 18;
}
else if (strequal(strtolower(G("stem")),"nineteenth")) {
	S("Numeral Value") = 19;
}
else if (strequal(strtolower(G("stem")),"twentieth")) {
	S("Numeral Value") = 20;
}
else if (strequal(strtolower(G("stem")),"thirtieth")) {
	S("Numeral Value") = 30;
}
else if (strequal(strtolower(G("stem")),"fortieth")) {
	S("Numeral Value") = 40;
}
else if (strequal(strtolower(G("stem")),"fiftieth")) {
	S("Numeral Value") = 50;
}
else if (strequal(strtolower(G("stem")),"sixtieth")) {
	S("Numeral Value") = 60;
}
else if (strequal(strtolower(G("stem")),"seventieth")) {
	S("Numeral Value") = 70;
}
else if (strequal(strtolower(G("stem")),"eightieth")) {
	S("Numeral Value") = 80;
}
else if (strequal(strtolower(G("stem")),"ninetieth")) {
	S("Numeral Value") = 90;
}
else if (strequal(strtolower(G("stem")),"hundredth")) {
	S("Numeral Value") = 100;
}
else if (strequal(strtolower(G("stem")),"thousandth")) {
	S("Numeral Value") = 1000;
}
else if (strequal(strtolower(G("stem")),"millionth")) {
	S("Numeral Value") = 1000000;
}
else if (strequal(strtolower(G("stem")),"billionth")) {
	S("Numeral Value") = 1000000000;
}
else if (strequal(strtolower(G("stem")),"trillionth")) {
	S("Numeral Value") = 1000000000000;
}
single();
@@POST
@RULES
_ordinalNumeral <-
    _xWILD [s min=1 max=1 match=("first" "second" "third" "fourth" "fifth" "sixth" "seventh" "eighth" "ninth" "tenth" "eleventh" "twelfth" "thirteenth" "fourteenth" "fifteenth" "sixteenth" "seventeenth" "eighteenth" "nineteenth" "twentieth" "thirtieth" "fortieth" "fiftieth" "sixtieth" "seventieth" "eightieth" "ninetieth" "hundredth" "thousandth" "millionth" "billionth" "trillionth")] ### (1)
    @@
@@RULES

#####################
# One word cardinals #
#####################
@POST
G("stem") = N("$text",1);
if (strequal(strtolower(G("stem")),"one")) {
	S("Numeral Value") = 1;
}
else if (strequal(strtolower(G("stem")),"two")) {
	S("Numeral Value") = 2;
}
else if (strequal(strtolower(G("stem")),"three")) {
	S("Numeral Value") = 3;
}
else if (strequal(strtolower(G("stem")),"four")) {
	S("Numeral Value") = 4;
}
else if (strequal(strtolower(G("stem")),"five")) {
	S("Numeral Value") = 5;
}
else if (strequal(strtolower(G("stem")),"six")) {
	S("Numeral Value") = 6;
}
else if (strequal(strtolower(G("stem")),"seven")) {
	S("Numeral Value") = 7;
}
else if (strequal(strtolower(G("stem")),"eight")) {
	S("Numeral Value") = 8;
}
else if (strequal(strtolower(G("stem")),"nine")) {
	S("Numeral Value") = 9;
}
else if (strequal(strtolower(G("stem")),"ten")) {
	S("Numeral Value") = 10;
}
else if (strequal(strtolower(G("stem")),"eleven")) {
	S("Numeral Value") = 11;
}
else if (strequal(strtolower(G("stem")),"twelve")) {
	S("Numeral Value") = 12;
}
else if (strequal(strtolower(G("stem")),"thirteen")) {
	S("Numeral Value") = 13;
}
else if (strequal(strtolower(G("stem")),"fourteen")) {
	S("Numeral Value") = 14;
}
else if (strequal(strtolower(G("stem")),"fifteen")) {
	S("Numeral Value") = 15;
}
else if (strequal(strtolower(G("stem")),"sixteen")) {
	S("Numeral Value") = 16;
}
else if (strequal(strtolower(G("stem")),"seventeen")) {
	S("Numeral Value") = 17;
}
else if (strequal(strtolower(G("stem")),"eighteen")) {
	S("Numeral Value") = 18;
}
else if (strequal(strtolower(G("stem")),"nineteen")) {
	S("Numeral Value") = 19;
}
else if (strequal(strtolower(G("stem")),"twenty")) {
	S("Numeral Value") = 20;
}
else if (strequal(strtolower(G("stem")),"thirty")) {
	S("Numeral Value") = 30;
}
else if (strequal(strtolower(G("stem")),"forty")) {
	S("Numeral Value") = 40;
}
else if (strequal(strtolower(G("stem")),"fifty")) {
	S("Numeral Value") = 50;
}
else if (strequal(strtolower(G("stem")),"sixty")) {
	S("Numeral Value") = 60;
}
else if (strequal(strtolower(G("stem")),"seventy")) {
	S("Numeral Value") = 70;
}
else if (strequal(strtolower(G("stem")),"eighty")) {
	S("Numeral Value") = 80;
}
else if (strequal(strtolower(G("stem")),"ninety")) {
	S("Numeral Value") = 90;
}
else if (strequal(strtolower(G("stem")),"hundred")) {
	S("Numeral Value") = 100;
}
else if (strequal(strtolower(G("stem")),"thousand")) {
	S("Numeral Value") = 1000;
}
else if (strequal(strtolower(G("stem")),"million")) {
	S("Numeral Value") = 1000000;
}
else if (strequal(strtolower(G("stem")),"billion")) {
	S("Numeral Value") = 1000000000;
}
else if (strequal(strtolower(G("stem")),"trillion")) {
	S("Numeral Value") = 1000000000000;
}
single();
@@POST
@RULES
_cardinalNumeral <-
    _xWILD [s min=1 max=1 match=("one" "two" "three" "four" "five" "six" "seven" "eight" "nine" "ten" "eleven" "twelve" "thirteen" "fourteen" "fifteen" "sixteen" "seventeen" "eighteen" "nineteen" "twenty" "thirty" "forty" "fifty" "sixty" "seventy" "eighty" "ninety" "hundred" "thousand" "million" "billion" "trillion")] ### (1)
    @@
@@RULES
