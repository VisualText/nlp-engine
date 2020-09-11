###############################################
# FILE: incap100.pat
# SUBJ: Work inside caps.
# AUTH: AM
# CREATED: 24/May/05 15:14:46
# MODIFIED:
###############################################

@CODE
G("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _noun _caps

# Want an _xFEAT match!
@CHECK
  if (N("sem",3) != "person title")
    fail();
  if (N("ne",4) || N("ne",5))
    fail();
@POST
  L("tmp3") = N(3);
  group(4,5,"_np");
  N("ne",4) = 1;
  N("ne type",4) = N("sem",4) = "person";
  N("ne type conf",4) = 80;
  N("stem",4) = N("$text",4);
  registerpersnames(N("$text",4));
  group(1,3,"_np");
  pncopyvars(L("tmp3"),N(1));
  
  X("ne",3) = X("ne",4) = 0;
  # Need POS.
@RULES
_xNIL <-
	_xWILD [one match=(_xCAP _noun)]
	_xWILD [one match=(_xCAP _noun)]
	_noun
	_xWILD [one match=(_xCAP _noun)]
	_xWILD [one match=(_xCAP _noun)]
	@@

@POST
  X("ne",3) = 1;
  X("ne type",3) = X("sem",3) = "organization";
  X("ne type conf",3) = 90;
  X("stem",3) = phrasetext();
@RULES
_xNIL <-
	_xWILD [plus match=(_xCAP _noun)]
	_xWILD [s one trigger match=(division department
		administration
		international
		ministry affairs hospital bank
		news tv
		school college university
		centre center)]
	_xEND
	@@

@CHECK
  if (!N("ne type",3))
    fail();
@POST
  X("ne",3) = 1;
  X("ne type",3) = N("ne type",3);
  X("ne type conf",3) = N("ne type conf",3);
  X("stem",3) = N("stem",3);
  X("sem",3) = N("sem",3);
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus match=(_xCAP)]
	_noun
	_xEND
	@@
