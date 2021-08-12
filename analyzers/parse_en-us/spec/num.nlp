###############################################
# FILE: num.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 11/Jul/04 02:08:22
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE


######## MISC #########
# det modal
@POST
 splice(3,3);
@RULES
_xNIL <-
	_xWILD [s one match=(a an the)]
	_xWHITE [star]
	_xWILD [s one match=(can will may might should)]
	@@
################

# num %
@POST
  pncopyvars(2);
  chpos(N(1),"CD");
  chpos(N(2),"NN");
  S("percent") = N("$text",1);
  S("sem") = "quantity";
  S("sem detail") = "percent";
  S("num") = 1;
  sclearpos(1,0);
  single();
@RULES
_percent [layer=(_noun)] <-
	_xNUM
	\%
	@@

# Money
@POST
  L("tmp3") = N(3);
  L("num") = num(N("$text",1));
  if (L("num") < 10)
    L("money") = "$0.0" + N("$text",1);
  else if (L("num") < 100)
    L("money") = "$0." + N("$text",1);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
  group(1,3,"_money");
  group(1,1,"_noun");
  pncopyvars(L("tmp3"),N(1));
  N("money",1) = L("money");
  N("don't splice",1) = 1;
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_xNUM
	_xWHITE [star]
	_xWILD [s one match=(
		cent cents penny pennies)]
	@@
		
		
# Some phone numbers
@PRE
<1,1> length(3);
<3,3> length(3);
<5,5> length(4);
@RULES
_phone <-
	_xNUM
	_xWILD [s one match=( \- \.)]
	_xNUM
	_xWILD [s one match=(\- \.)]
	_xNUM
	@@
@PRE
<2,2> length(3);
<4,4> length(3);
<6,6> length(4);
@RULES
_phone <-
	\(
	_xNUM
	\)
	_xNUM
	_xWILD [s one match=(\- \.)]
	_xNUM
	@@

# Foreign phone
@RULES
_phone <-
	\+ [opt]
	_xNUM
	_xWHITE [opt]
	\(
	_xNUM
	\)
	_xWHITE [opt]
	_xNUM
	_xWHITE [opt]
	_xNUM
	_xWHITE [opt]
	_xNUM
	@@

@POST
  S("x-coordinate") = num(N("$text",2));
  S("y-coordinate") = num(N("$text",4));
  S("sem") = "location";
  single();
@RULES
_coord [layer=_noun] <-
	\( [s]
	_xNUM [s]
	\, [s]
	_xNUM [s]
	\) [s]
	@@

# num,num,num
# num , num , num
@CHECK
  if (N(5) && N("$length",5) != 3)
    fail();
  if (N("$length",3) != 3)
    fail();
@POST
  S("val") = num(N("$text",1)) * 1000.0
    + num(N("$text",3));
  if (N(5))
    S("val") = S("val") * 1000.0
	 + num(N("$text",5));
  S("mypos") = "CD";
  single();
@RULES
_num <-
	_xNUM [s]
	\, [s]
	_xNUM [s]
	\, [s opt]
	_xNUM [s opt]
	@@

@POST
#  pncopyvars(1);	# FIX.	# 01/16/13 AM.
  single();
@RULES
_num <-
	_xWILD [s min=4 match=(_xNUM)]
	@@

# num . num . num
# Note: Probable date.  Can do some validation.
@POST
  L("n1") = N("$text",1);
  L("n2") = N("$text",3);
  L("n3") = N("$text",5);
  group(1,5,"_num");
  group(1,1,"_noun");
  N("sem",1) = "date";
  N("year",1) = L("n3");
  N("num1",1) = L("n1");
  N("num2",1) = L("n2");
  N("mypos",1) = "NN";
  ++X("date ref");
@RULES
_xNIL <-
	_xNUM [s]
	\. [s]
	_xNUM [s]
	\. [s]
	_xNUM [s]
	_xWILD [one lookahead fail=( \. )]
	@@

# num . num
@POST
  N("nospace after",2) = 1;
  X("num") = N("$text",2) + N("$text",2) + N("$text",4);
  single();
@RULES
_num <-
	_xNUM [s]
	\. [s]
	_xNUM [s]
	@@
# num / num
_num <-
	_xNUM [s]
	\/ [s]
	_xNUM [s]
	@@

@POST
  S("mypos") = "JJ";
  S("num") = num(N("$text",1));
  # Need float for the larger ones. Need MAXINT test...
  single();
@RULES
_adj <-
	_xNUM [s]
	th [s]
	@@

@POST
  S("mypos") = "CD";
  S("num") = num(N("$text",1));
  # Need float for the larger ones. Need MAXINT test...
  single();
@RULES
_noun <-
	_xNUM [s]
	\' [s]
	s [s]
	@@

@POST
  S("num") = 1;
  S("number") = "singular";
  S("stem") = "one";
  pncopyvars(1);
  single();
@RULES
_num <-
	_xWILD [s one match=(one) except=(_noun)] @@

@POST
  S("num") = 1;
  pncopyvars(1);
  single();
@RULES
#_adj <- first [s] @@	# Function word ...
_adj <- 1 [s] st [s] @@

@POST
  N("num") = 1;
@RULES
_xNIL <- first @@

@POST
  S("num") = 2;
  pncopyvars(1);
  single();
@RULES
_num <- two [s] @@
@POST
  S("num") = 2;
  pncopyvars(1);
  single();
@RULES
_adj <- second [s] @@
_adj <- 2 [s] nd [s] @@

@POST
  S("num") = 3;
  pncopyvars(1);
  single();
@RULES
_num <- three [s] @@
@POST
  S("num") = 3;
  pncopyvars(1);
  single();
@RULES
_adj <- third [s] @@
_adj <- 3 [s] rd [s] @@

@POST
  pncopyvars(1);
  S("num") = 4;
  single();
@RULES
_num <- four [s] @@
@POST
  pncopyvars(1);
  S("num") = 4;
  single();
@RULES
_adj <- fourth [s] @@

@POST
  pncopyvars(1);
  S("num") = 5;
  single();
@RULES
_num <- five [s] @@
@POST
  pncopyvars(1);
  S("num") = 5;
  single();
@RULES
_adj <- fifth [s] @@

@POST
  pncopyvars(1);
  S("num") = 6;
  single();
@RULES
_num <- six [s] @@
@POST
  pncopyvars(1);
  S("num") = 6;
  single();
@RULES
_adj <- sixth [s] @@

@POST
  pncopyvars(1);
  S("num") = 7;
  single();
@RULES
_num <- seven [s] @@
@POST
  pncopyvars(1);
  S("num") = 7;
  single();
@RULES
_adj <- seventh [s] @@

@POST
  pncopyvars(1);
  S("num") = 8;
  single();
@RULES
_num <- eight [s] @@
@POST
  pncopyvars(1);
  S("num") = 8;
  single();
@RULES
_adj <- eighth [s] @@

@POST
  pncopyvars(1);
  S("num") = 9;
  single();
@RULES
_num <- nine [s] @@
@POST
  pncopyvars(1);
  S("num") = 9;
  single();
@RULES
_adj <- ninth [s] @@

@POST
  pncopyvars();
  S("num") = 10;
  single();
@RULES
_num <- ten [s] @@
@POST
  pncopyvars();
  S("num") = 10;
  single();
@RULES
_adj <- tenth [s] @@

@POST
  pncopyvars();
  S("num") = 11;
  single();
@RULES
_num <- eleven [s] @@
@POST
  pncopyvars();
  S("num") = 11;
  single();
@RULES
_adj <- eleventh [s] @@

@POST
  pncopyvars();
  S("num") = 12;
  single();
@RULES
_num <- twelve [s] @@
@POST
  pncopyvars();
  S("num") = 12;
  single();
@RULES
_adj <- twelfth [s] @@

@POST
  pncopyvars();
  S("num") = 13;
  single();
@RULES
_num <- thirteen [s] @@
@POST
  pncopyvars();
  S("num") = 13;
  single();
@RULES
_adj <- thirteenth [s] @@

@POST
  S("num") = 14;
  single();
@RULES
_num <- fourteen [s] @@
@POST
  S("num") = 14;
  single();
@RULES
_adj <- fourteenth [s] @@

@POST
  pncopyvars();
  S("num") = 15;
  single();
@RULES
_num <- fifteen [s] @@
@POST
  pncopyvars();
  S("num") = 15;
  single();
@RULES
_adj <- fifteenth [s] @@

@POST
  pncopyvars();
  S("num") = 16;
  single();
@RULES
_num <- sixteen [s] @@
@POST
  pncopyvars();
  S("num") = 16;
  single();
@RULES
_adj <- sixteenth [s] @@

@POST
  pncopyvars();
  S("num") = 17;
  single();
@RULES
_num <- seventeen [s] @@
@POST
  pncopyvars();
  S("num") = 17;
  single();
@RULES
_adj <- seventeenth [s] @@

@POST
  pncopyvars();
  S("num") = 18;
  single();
@RULES
_num <- eighteen [s] @@
@POST
  pncopyvars();
  S("num") = 18;
  single();
@RULES
_adj <- eighteenth [s] @@

@POST
  pncopyvars();
  S("num") = 19;
  single();
@RULES
_num <- nineteen [s] @@
@POST
  pncopyvars();
  S("num") = 19;
  single();
@RULES
_adj <- nineteenth [s] @@

@POST
  pncopyvars();
  S("num") = 20;
  single();
@RULES
_num <- twenty [s] @@
@POST
  pncopyvars();
  S("num") = 20;
  single();
@RULES
_adj <- twentieth [s] @@

@POST
  pncopyvars();
  S("num") = 30;
  single();
@RULES
_num <- thirty [s] @@
@POST
  pncopyvars();
  S("num") = 30;
  single();
@RULES
_adj <- thirtieth [s] @@

@POST
  pncopyvars();
  S("num") = 40;
  single();
@RULES
_num <- forty [s] @@
@POST
  pncopyvars();
  S("num") = 40;
  single();
@RULES
_adj <- fortieth [s] @@

@POST
  pncopyvars();
  S("num") = 50;
  single();
@RULES
_num <- fifty [s] @@
@POST
  pncopyvars();
  S("num") = 50;
  single();
@RULES
_adj <- fiftieth [s] @@

@POST
  pncopyvars();
  S("num") = 60;
  single();
@RULES
_num <- sixty [s] @@
@POST
  pncopyvars();
  S("num") = 60;
  single();
@RULES
_adj <- sixtieth [s] @@

@POST
  pncopyvars();
  S("num") = 70;
  single();
@RULES
_num <- seventy [s] @@
@POST
  pncopyvars();
  S("num") = 70;
  single();
@RULES
_adj <- seventieth [s] @@

@POST
  pncopyvars();
  S("num") = 80;
  single();
@RULES
_num <- eighty [s] @@
@POST
  pncopyvars();
  S("num") = 80;
  single();
@RULES
_adj <- eightieth [s] @@

@POST
  pncopyvars();
  S("num") = 90;
  single();
@RULES
_num <- ninety [s] @@
@POST
  pncopyvars();
  S("num") = 90;
  single();
@RULES
_adj <- ninetieth [s] @@

@POST
  pncopyvars();
  S("num") = 100;
  single();
@RULES
_num <- hundred [s] @@
@POST
  pncopyvars();
  S("num") = 100;
  single();
@RULES
_adj <- hundredth [s] @@

@POST
  pncopyvars();
  S("num") = 1000;
  single();
@RULES
_num <- thousand [s] @@
@POST
  pncopyvars();
  S("num") = 1000;
  single();
@RULES
_adj <- thousandth [s] @@

@POST
  pncopyvars();
  S("num") = 1000000;
  single();
@RULES
_num <- million [s] @@
_num <- mln [s] @@
@POST
  pncopyvars();
  S("num") = 1000000;
  single();
@RULES
_adj <- millionth [s] @@

@POST
  pncopyvars();
  S("num") = 1000000000;
  single();
@RULES
_num <- billion [s] @@
@POST
  pncopyvars();
  S("num") = 1000000000;
  single();
@RULES
_adj <- billionth [s] @@

@POST
  pncopyvars();
  S("num") = 1000000000000.0;
  single();
@RULES
_num <- trillion [s] @@
@POST
  pncopyvars();
  S("num") = 1000000000000.0;
  single();
@RULES
_adj <- trillionth [s] @@


# For pos tagger.	# 06/04/06 AM.
@POST
  if (!N("mypos"))
    chpos(N(1),"CD");
  noop();
@RULES
_xNIL <-
	_xNUM
	@@
