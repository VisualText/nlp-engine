###############################################
# FILE: qsent100.pat
# SUBJ: Look in sentences.
# AUTH: AM
# CREATED: 09/Jul/04 23:46:20
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

@CHECK
  if (N("pattern",1) == "alpha"
   && N("pattern",3) == "alpha")
     fail();
  if (N("pattern",1) != "alpha"
   && N("pattern",3) != "alpha")
     fail();
@POST
  if (N("pattern",3) == "alpha")
    {
	# Assume no evidence for local glom around conj.
	if (N("verb",3))
	  {
	  L("tmp") = N(3);
	  pnrename(N(3),"_verb");
	  group(3,3,"_vg");
	  mhbv(N(3),L("neg"),0,0,0,0,L("tmp"));
	  pncopyvars(L("tmp"),N(3));
#	  N("voice",3) = ?
	  pnreplaceval(N(3),"pattern",0);
	  group(3,3,"_clause");
	  setunsealed(3,"true");	# 07/10/12 AM.
	  pncopyvars(L("tmp"),N(3));
	  N("pattern",3) = "v";
	  }
	}
  else # pattern 1 == alpha
    {
	}
@RULES
_xNIL <-
	_clause
	_conj
	_clause
	@@


# that clause
# Assigning role to "that" based on following clause.
#@CHECK
#  if (N("mypos",1))	# Fix. 06/09/06 AM.
#    fail();
#  if (!N("pattern",2))
#    fail();
#@POST
#  L("ch") = strpiece(N("pattern",2),0,0);
#  if (L("ch") == "n")
#   chpos(N(1),"IN");
#  else if (L("ch") == "v")
#   {
#   chpos(N(1),"WDT");	# that/WDT
#   N("bracket",1) = 1;
#   }
#@RULES
#_xNIL <-
#	that [s]
#	_clause [lookahead]
#	@@


# that
# clause that clause
# NOTE: New and better handling of that-clause.
#	Looking for "incomplete" second clause, eg, nv
#	with a transitive verb.
@CHECK
  if (N("mypos",2))
    fail();
  if (N("voice",3) == "passive")
    fail();	# Need a separate handler for passive.
  L("p1") = N("pattern",1);
  L("p") = N("pattern",3);

  if (L("p1") == "nv" || L("p1") == "v")
	{
	if (N("last name",1) != "_advl")	# 05/27/07 AM.
	  {
	  # Should check for pp etc.
	  S("pos") = "IN";
      succeed();
	  }
	}

  # If first clause ends in np.
  # If second starts with vg or is incomplete-transitive.
  # Then WDT.  Else IN.
#  "cl.txt" << phrasetext() << "\n";
#  "cl.txt" << clauselastnp(N(1)) << "\n";
#  "cl.txt" << clausecomplete(N(3)) << "\n";
  if (!clauselastnp(N(1)))
    fail();


  if (clauseincomplete(N(3))	# Fits incomplete patterns.
   && !clausecomplete(N(3)))	# Can't fit complete patterns.
    {
	S("pos") = "WDT";
    succeed();
	}
  # Todo: Need better checking on prepositional/phrasal verbs.
  fail();
@POST
# "cl.txt" << "active" << "\n";
 if (S("pos") == "IN")
 	{
	chpos(N(2),"IN");	# that/IN
	}
 else
    {
    chpos(N(2),"WDT");	# that/WDT
     N("bracket",2) = 1;
	 }
@RULES
_xNIL <-
	_clause
	that [s lookahead]
	_clause
	@@

# that
# clause that clause
# To simplify logic, separated passive voice handler.
@CHECK
  if (N("mypos",2))
    fail();
  if (N("voice",3) != "passive")
    fail();
  L("p1") = N("pattern",1);
  if (L("p1") == "nv" || L("p1") == "v")
  	{
	S("pos") = "IN";
	succeed();
	}
  S("pos") = "WDT";
  L("p") = N("pattern",3);
  if (!L("p"))
    fail();
  if (strpiece(L("p"),0,0) == "v")
    succeed();
  if (strpiece(L("p"),0,0) == "n")
    {
	# Require lone "by" or other prep at end of clause.
	if (L("p") == "nvp")
	  succeed();
	}
  fail();
@POST
  if (S("pos") == "IN")
  	{
	chpos(N(2),"IN");	# that/IN
	}
  else
	{
#  "cl.txt" << "passive" << "\n";
  chpos(N(2),"WDT");	# that/WDT
  N("bracket",2) = 1;
	}
@RULES
_xNIL <-
	_clause
	that [s lookahead]
	_clause
	@@

