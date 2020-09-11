###############################################
# FILE: anaphora100.pat
# SUBJ: Intra-sentential resolutions.
# AUTH: AM
# CREATED: 23/Nov/05 22:58:07
# NOTE: Need some clause reasoning functions...
#	12/07/05 AM. An idea is to write rules that grab
#	one clause at a time here ("traverse"). Then
#	store the prior in sentence node.
###############################################

@CODE
L("hello") = 0;

if (G("verbose"))
  "wsd.txt" << "\n" << "ANAPHORA100 PASS" << "\n";
@@CODE

@PATH _ROOT _TEXTZONE _sent

# that
@CHECK
  if (N("pattern",2) != "n")
    fail();
@POST
  L("np") = N("np",2);
  if (N("pattern",4) == "vn"
   || N("pattern",4) == "v"
  	)
	{
	# Resolve using earlier np.
	N("np1 remote",4) = L("np");
	}
  if (N("pattern",6) == "vn"
   || N("pattern",6) == "v"
  	)
	{
	# Resolve using earlier np.
	N("np1 remote",6) = L("np");
	}
@RULES
_xNIL <-
	_xSTART
	_clause
	_xWILD [s one match=(which that)]
	_clause [lookahead]
	_prep
	_clause
	@@

# that
@CHECK
  if (N("pattern",2) != "n")
    fail();
@POST
  L("np") = N("np",2);
  if (N("pattern",4) == "vn"
   || N("pattern",4) == "v"
  	)
	{
	# Resolve using earlier np.
	N("np1 remote",4) = L("np");
	}
@RULES
_xNIL <-
	_xSTART
	_clause
	_xWILD [s one lookahead match=(which that)]
	_clause
	@@

@CHECK
  L("pat3") = N("pattern",3);
  if (!L("pat3"))
    fail();
  if (strpiece(L("pat3"),0,0) != "v")
    fail();
  L("pat2") = N("pattern",2);
  if (!L("pat2"))
    fail();
  if (strpiece(L("pat2"),0,0) != "n")
    fail();
  if (vconjq(N(3),"-en"))
    fail();
@POST
  # Distribute np to second clause.
  L("np") = N("np1",2);
  if (!L("np"))
    L("np") = N("np",2);
  if (L("np"))
    {
	N("np1 remote",3) = L("np");

	# Distribute to 3rd clause.
	if (N("pattern",5))
	  {
	  if (strpiece(N("pattern",5),0,0) == "v")
	    N("np1 remote",5) = L("np");
	  }
	}
@RULES
_xNIL <-
	_xSTART
	_clause
	_clause [lookahead]
	_xWILD [s one match=(while when _conj)]
	_clause
	@@


# passive passive.
# X was processed by Y connected to Z.
# Y is the resolver.
@CHECK
  if (N("voice",1) != "passive"
   || N("voice",2) != "passive")
    fail();
  if (!N("pattern",2))
    fail();	# Flag this to debug file...
  if (strpiece(N("pattern",2),0,0) != "v")
    fail();
  # Get last np in clause.
  if (!N("last np",1))
    fail();
@POST
  N("np1 remote",2) = N("last np",1);
@RULES
_xNIL <-
	_clause
	_clause [lookahead]
	@@

@CHECK
  L("pat2") = N("pattern",2);
  if (!L("pat2"))
    fail();
  if (strpiece(L("pat2"),0,0) != "v")
    fail();
  L("pat1") = N("pattern",1);
  if (!L("pat1"))
    fail();
  if (strpiece(L("pat1"),0,0) != "n")
    fail();
  if (vconjq(N(2),"-en"))
    fail();
@POST
  # Distribute np to second clause.
  L("np") = N("np1",1);
  if (!L("np"))
    L("np") = N("np",1);
  if (L("np"))
    {
	N("np1 remote",2) = L("np");
	}
  N("id",2) = "anaph100 c-c";
@RULES
_xNIL <-
	_clause
	_clause [lookahead]
	@@
