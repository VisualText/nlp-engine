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

#@PATH _ROOT _TEXTZONE _noun _caps
@NODES _caps

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

  L("x3") = pnparent(X());		# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne",0);	# 07/10/12 AM.
  X("ne") = 0;
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
  L("x3") = pnparent(X());		# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne",1);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type","organization");	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type conf",90);		# 07/10/12 AM.
  pnreplaceval(L("x3"),"sem","organization");	# 07/10/12 AM.
  pnreplaceval(L("x3"),"stem",phrasetext());	# 07/10/12 AM.
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
  L("x3") = pnparent(X());		# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne",1);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type",N("ne type",3));	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type conf",N("ne type conf",3));		# 07/10/12 AM.
  pnreplaceval(L("x3"),"sem",N("sem",3));	# 07/10/12 AM.
  pnreplaceval(L("x3"),"stem",N("stem",3));	# 07/10/12 AM.
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus match=(_xCAP)]
	_noun
	_xEND
	@@

# Note: title name patterns.
@CHECK
  if (!N("unknown",2) && !N("unknown",3))
	fail();
@POST
  L("tt") = N("$text",3) + " " + N("$text",4);	# 07/10/12 AM.
  L("x3") = pnparent(X());		# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne",1);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type","person");	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type conf",90);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"sem","person");		# 07/10/12 AM.
  pnreplaceval(L("x3"),"stem",L("tt"));		# 07/10/12 AM.
#  X("ne",3) = 1;
#  X("ne type",3) = X("sem",3) = "person";
#  X("ne type conf",3) = 90;
#  X("stem",3) = N("$text",3) + " " + N("$text",4);
  N("mypos",3) = "NP";
  N("mypos",4) = "NP";
  # Todo: Register first and last name.
@RULES
_xNIL <-
	_xWILD [plus match=(_xCAP _noun)]
	_xWILD [s trigger plus match=(_nounCountry
	representative ceo coo cfo officer
	president senator congressman)]
	_xCAP
	_xCAP
	_xEND
	@@

# title alpha
@POST
  L("tt") = N("$text",3);		# 07/10/12 AM.
  L("x3") = pnparent(X());		# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne",1);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type","person");	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type conf",90);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"sem","person");		# 07/10/12 AM.
  pnreplaceval(L("x3"),"stem",L("tt"));		# 07/10/12 AM.

#  X("ne",3) = 1;
#  X("ne type",3) = X("sem",3) = "person";
#  X("ne type conf",3) = 90;
#  X("stem",3) = N("$text",3);
  N("mypos",3) = "NP";
  # Todo: Register last name.
@RULES
_xNIL <-
	_xSTART
	_title [s]
	_xALPHA
	_xEND
	@@

@CHECK
  if (!N("unknown",2))
	fail();
  if (!N("unknown",3))
	fail();
@POST
  L("tt") = N("$text",2) + " " + N("$text",3);	# 07/10/12 AM.
  L("x3") = pnparent(X());		# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne",1);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type","person");	# 07/10/12 AM.
  pnreplaceval(L("x3"),"ne type conf",90);	# 07/10/12 AM.
  pnreplaceval(L("x3"),"sem","person");		# 07/10/12 AM.
  pnreplaceval(L("x3"),"stem",L("tt"));		# 07/10/12 AM.

#  X("ne",3) = 1;
#  X("ne type",3) = X("sem",3) = "person";
#  X("ne type conf",3) = 90;
#  X("stem",3) = N("$text",2) + " " + N("$text",3);
  N("mypos",2) = "NP";
  N("mypos",3) = "NP";
  # Todo: Register first and last name.
@RULES
_xNIL <-
	_xSTART
	_xCAP
	_xCAP
	_xEND
	@@
