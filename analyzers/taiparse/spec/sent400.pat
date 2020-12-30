###############################################
# FILE: sent400.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 30/Sep/04 01:18:22
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

# np , vg to vg , vg
# John , trying to work... , left the room...
@CHECK
  if (N("pattern",2) != "n")
    fail();
  if (!N("start to-vg",5))
    fail();
@POST
  L("vg") = N("vg node",7);
  if (L("vg"))
    fixvg(L("vg"),"active","VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_clause
	\, [lookahead]
	_clause
	_clause
	\,
	_clause
	@@

# List of clauses!
# Basis for parallel construction.
# Verbs should agree in passive/active, etc. whenever possible.
@CHECK
  if (!N("vg count",1)) # Verbless.
    fail();
@POST
  if (N("voice",1) == "active")
    {
	L("pos") = clausepos(N(1));

	# Fill in ambiguous clauses with active voice.
	if (!N("voice",3)  && N("no subj",3))
	  {
	  fixvg(N("vg node",3),"active",L("pos"));
	  N("voice",3) = "active";
	  }
	if (!N("voice",6)  && N("no subj",6))
	  {
	  fixvg(N("vg node",6),"active",L("pos"));
	  N("voice",6) = "active";
	  }
	}

  noop();
@RULES
_xNIL <-
	_clause
	\,
	_clause [lookahead]
	\, [opt]
	_conj
	_clause
	@@

# clause prep clause conj clause
# Case where first and third clause agree.
@CHECK
  if (N("voice",5))
    fail();
  if (N("voice",1) != "passive")
    fail();
  if (vgvgagree(N("vg node",3),N("vg node",5)))
    fail();
@POST
  fixvg(N("vg node",5),"passive","VBN");
  N("voice",5) = "passive";
@RULES
_xNIL <-
	_clause
	_prep [lookahead]
	_clause
	_conj
	_clause
	@@

# clause fnword clause
# clause than clause
@CHECK
  if (N("last chunk",1) != "j")
    fail();
  if (N("voice",3))
    fail();
  if (!N("vg node",3))
    fail();
@POST
  fixvg(N("vg node",3),"passive","VBN");
  N("voice",3) = "passive";
@RULES
_xNIL <-
	_clause
	than [s lookahead]
	_clause
	@@

# clause fnword clause.
@CHECK
  if (!N("vg count",1)) # Verbless.
    fail();
  if (!N("voice",1) && !N("voice",3))
    fail();
  if (N("voice",1) && N("voice",3)
   && N("voice",1) != N("voice",3))
    fail();
  if (N("pattern",3) == "nv"
   || N("pattern",1) == "nv")
    fail();	# Still not sure...
  # Check verb tense agreement here...
  S("vg1") = N("vg node",1);
  S("vg3") = N("vg node",3);
  "clause-fn-clause.txt" << pnvar(S("vg1"),"$text") << "\n";
  if (!vgvgagree(S("vg1"),S("vg3")))
    fail();
  "clause-fn-clause.txt" << pnvar(S("vg1"),"$text") << "\n";
@POST
  if (N("voice",1))
    L("voice") = N("voice",1);
  else
    {
    L("voice") = N("voice",3);
	N("voice",1) = L("voice");
	}
  if (!N("voice",3))
    N("voice",3) = L("voice");
  fixvg(S("vg3"),L("voice"),clausepos(N(1)));
  fixvg(S("vg1"),L("voice"),clausepos(N(3)));
  N("sent400 c-and-c",1) = 1;
  N("sent400 c-and-c",3) = 1;
@RULES
_xNIL <-
	_clause
	_xWILD [one lookahead match=(_conj \, _fnword )]
	_clause
	@@

# clause fnword clause.
@CHECK
  if (N("sent400 c-fun-c",1))
    fail();
  if (N("voice",1))
    fail();
  if (!N("voice",3))
    fail();
  if (N("pattern",1) != "nv")
    fail();
  if (!N("pattern",3))
    fail();
  S("vg1") = N("vg node",1);
  S("vg3") = N("vg node",3);
@POST
  L("voice") = N("voice",3);
  N("voice",1) = L("voice");
  if (strpiece(N("pattern",3),0,0) == "v")
    fixvg(S("vg1"),"active","VBD");
  else
    fixvg(S("vg1"),L("voice"),clausepos(N(3)));
  N("sent400 c-fun-c",1) = 1;
@RULES
_xNIL <-
	_clause
	_xWILD [one lookahead match=(_conj \, _fnword )]
	_clause
	_xWILD [one match=(_qEOS _xEND)]
	@@


# clause , clause
# verbless 2nd...
# apposition...
@CHECK
  if (!N("vg count",1)) # Verbless.
    fail();
  if (N("voice",1)
   || N("vg count",3) > 0)
    fail();
@POST
  fixvg(N("vg node",1),"active","VBP");
  N("voice",1) = "active";
@RULES
_xNIL <-
	_clause
	\,	[lookahead]
	_clause
	@@

# that clause
# that
#@POST
#  noop();
#@RULES
#_xNIL <-
#	that [s]
#	_clause [lookahead]
#	@@

# clause , that
# that
@CHECK
  if (N("mypos",3) == "IN")
    fail();
@POST
  chpos(N(3),"IN");	# that/IN
@RULES
_xNIL <-
	_clause
	\, [lookahead]
	that [s]
	@@

# clause , clause , clause
# Want first to be an np.  Appositive case.
@CHECK
  if (N("vg count",1))
    fail();	# Require verbless.
  if (N("pattern",1) != "n")
    fail();	# Require np.
@POST
	L("vg3") = N("vg node",3);
	if (L("vg3"))
	  L("v3") = pnvar(L("vg3"),"first verb");
	if (L("v3"))
	 if (!pnvar(L("v3"),"mypos")) # Unresolved.
	  {
	  L("vc3") = vconj(L("v3"));
	  if (L("vc3") == "-edn")
	    {
	    fixvg(L("vg3"),"passive","VBN");	# 01/25/05 AM.
		N("voice",3) = "passive";
		}
	  }
	
	L("vg") = N("vg node",5);
	fixvg(L("vg"),"active","VBP");
	N("voice",5) = "active";
	N("sent400 ccc",5) = 1;
@RULES
_xNIL <-
	_clause
	\, [lookahead]
	_clause
	\,
	_clause
	@@

# clause , clause , clause
# Want 1st to end with vg, last to start with vg.
@CHECK
  if (N("pattern",1) != "nv")
    fail();
  if (!N("pattern",5))
    fail();
  if (strpiece(N("pattern",5),0,0) != "v")
    fail();	# Need start with a verb.
  if (N("voice",1))
    fail();
  if (!N("vg node",1))
    fail();
@POST
  L("vg") = N("vg node",1);
  fixvg(L("vg"),"passive","VBN");
  N("voice",1) = "passive";
  N("id",1) = "sent400 c--c--c";
@RULES
_xNIL <-
	_clause
	_xWILD [one lookahead match=( \, _dbldash)]
	_clause
	_xWILD [one match=( \, _dbldash)]
	_clause
	@@


# clause clause clause
@CHECK
  if (N("pattern",2) != "n"
   && !N("ellipted-copula",2) )
    fail();
  if (N("voice",3))
    fail();
  if (N("voice",4) != "active")
    fail();
  S("vg3") = N("vg node",3);
  S("v3") = pnvar(S("vg3"),"first verb");
  S("vc3") = vconj(S("v3"));
  "blimp.txt" << S("vc3") << "\n";
  if (S("vc3") != "-edn")
    fail();
@POST
  if (N("ellipted-copula",2))
    {
	fixvg(S("vg3"),"active","VBD");
	N("voice",3) = "active";
	}
  else
    {
    fixvg(S("vg3"),"passive","VBN");
    N("voice",3) = "passive";
	}
  L("vg4") = N("vg node",4);
  if (L("vg4"))
    fixvg(L("vg4"),"active","VBP");
  N("sent400 c-c-c",2) = 1;
@RULES
_xNIL <-
	_xSTART
	_clause
	_clause [lookahead]
	_clause
#	_xEND
	@@

# clause clause
@CHECK
#  if (!N("start to-vg",2))
#    fail();
  if (N("voice",1) && N("voice",2))
    fail();
  if (!N("voice",1) && !N("voice",2))
    fail();
  if (!N("vg count",1) || !N("vg count",2))
  	fail();
@POST
  if (N("voice",1) && !N("voice",2))
    {
	if ((N("pattern",1) == "nvn" || N("pattern",1) == "vn"
		|| (N("pattern",1) == "v" && N("last name",1) == "_advl") )
	 && (N("pattern",2) == "v"   || N("pattern",2) == "vn"
	     || N("pattern",2) == "vp"	# 05/16/07 AM.
	))
	  {
	  fixvg(N("vg node",2),"passive","VBN");
	  N("voice",2) = "passive";
	  }
	else if (N("voice",1) == "active"
	 && N("last name",1) == "_advl"
	 && N("first name",2) == "_vg")
	   {
	   fixvg(N("vg node",2),"passive","VBN");
	   N("voice",2) = "passive";
	   }
	}
  else if (!N("voice",1) && N("voice",2))
    {
	L("pat1") = N("pattern",1);
	L("pat2") = N("pattern",2);
	L("vg") = N("vg node",2);
	L("v") = pnvar(L("vg"),"first verb");
	L("vc") = vconj(L("v"));
	if (L("pat1"))
	  {
	  if (N("to-vg",2)
	   || L("vc") == "-ing")
	    {
		# Tentative....
		fixvg(N("vg node",1),"active","VBP");
		N("voice",1) = "active";
		}
	  else if (L("pat1") == "nv")			# 06/21/06 AM.
	  	{
		fixvg(N("vg node",1),"active","VBD");
		N("voice",1) = "active";
		}
#	  else if (strendswith(L("pat1"),"v"))	# 06/21/06 AM.
	  else if (L("pat1") == "v")			# 06/21/06 AM.
	    {
		if (N("ellipted-copula",1))
		  {
		  fixvg(N("vg node",1),"active","VBD");
		  N("voice",1) = "active";
		  }
		else
		  {
		  fixvg(N("vg node",1),"passive","VBN");
		  N("voice",1) = "passive";
		  }
		}
	  }
	if (!N("voice",1) && L("pat2"))
	  if (strpiece(L("pat2"),0,0) == "v")
	    {
	    fixvg(N("vg node",1),"active","VBD");
		N("voice",1) = "active";
		}
	}
  N("sent400 c-c",1) = 1;
@RULES
_xNIL <-
	_clause
	_clause [lookahead]
	@@

# Last clause nv.
@CHECK
  if (N("voice",1))
    fail();
@POST
  N("sent400 last-clause",1) = 1;
  if (N("pattern",1) == "nv")
	{
    fixvg(N("vg node",1),"active","VBD");
    N("voice",1) = "active";
	}
  else if (N("pattern",1) == "v")
	{
    fixvg(N("vg node",1),"passive","VBN");
    N("voice",1) = "passive";
	}
@RULES
_xNIL <-
	_clause
	_xEND
	@@

# clause conj clause
# "one eating prunes, the other stuffed silly."
@PRE
<1,1> var("ellipted-that-is");
<3,3> vareq("pattern","nv");
<3,3> varz("voice");
@POST
  L("vg") = N("vg node",3);
  fixvg(L("vg"),"passive","VBN");
  N("voice",3) = "passive";
@RULES
_xNIL <-
	_clause
	_conj
	_clause
	@@

# ^ clause , clause $
@PRE
<2,2> vareq("voice","active");
<4,4> varz("voice");
<4,4> vareq("pattern","v");
@POST
  L("vg") = N("vg node",2);
  if (L("vg"))
    L("v") = pnvar(L("vg"),"verb node");
  if (L("v"))
    L("pos") = pnvar(L("v"),"mypos");
  L("vg4") = N("vg node",4);
  if (N("last name",2) == "_advl"
   || N("last name",4) == "_advl")
  	{
	fixvg(L("vg4"),"passive","VBN");
	N("voice",4) = "passive";
	}
  else
	{
    fixvg(L("vg4"),"active",L("pos"));
	N("voice",4) = "active";
	}
@RULES
_xNIL <-
	_xSTART
	_clause
	\,
	_clause
	_xEND
	@@

# clause conj clause
@PRE
<1,1> vareq("voice","active");
<3,3> varz("voice");
<3,3> vareq("pattern","v");
@POST
  L("vg") = N("vg node",1);
  if (L("vg"))
    L("v") = pnvar(L("vg"),"verb node");
  if (L("v"))
    L("pos") = pnvar(L("v"),"mypos");
  L("vg3") = N("vg node",3);
  fixvg(L("vg3"),"active",L("pos"));
  N("voice",3) = "active";
@RULES
_xNIL <-
	_clause
 	_xWILD [plus lookahead match=(_conj \, )]
 	_clause
	@@
 
# fnword clause , clause
# ex: when blah , say blah.
@CHECK
  S("vg4") = N("vg node",4);
  if (!S("vg4"))
    fail();
  if (vgassigned(S("vg4")))
   	fail();
  if (!vconjq(S("vg4"),"inf"))
  	fail();
  if (!N("pattern",4))
  	fail();
  L("ch") = strpiece(N("pattern",4),0,0);
  if (L("ch") != "v")
  	fail();
@POST
  fixvg(S("vg4"),"active","VB");	# Imperative.
@RULES
 _xNIL <-
 	_whword [s]
	_clause
	\, [opt]
	_clause
	@@

	 