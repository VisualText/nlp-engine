###############################################
# FILE: qclause50.pat
# SUBJ: Clause starts and separations.
# AUTH: AM
# CREATED: 11/Feb/05 22:40:43
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

# vg
@CHECK
  if (N("voice",3))
    fail();
  S("v") = N("verb node",3);
  S("vc") = vconj(S("v"));
  if (S("vc") != "-edn")
    fail();
@POST
  fixvg(N(3),"passive","VBN");
  N("voice",3) = "passive";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_vg
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one fail=(_noun _np)]
	@@
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_vg
	_xWILD [star lookahead match=(_adv _advl)]
	_xEND
	@@


# np conj vg
# vg conj np
@POST
  group(2,2,"_clausesep");	# Clause separator!
@RULES
_xNIL <-
	_np
	_conj
	_vg [lookahead]
	@@
_xNIL <-
	_vg
	_conj
	_np [lookahead]
	@@

# vg adj conj vg
@POST
  group(3,3,"_clausesep");
@RULES
_xNIL <-
	_vg
	_adj
	_conj
	_vg [lookahead]
	@@

# to vg
@POST
  group(1,1,"_clausestart");
@RULES
_xNIL <-
	to [s except=(_clausestart)]
	_adv [star]
	_vg [lookahead]
	@@

# parallel construction.
# ellipted copula
@PRE
<1,1> varz("qc50 nvacna");
@POST
  N("qc50 nvacna",1) = 1;
  group(7,7,"_clausesep");
  if (N(8))
    group(8,8,"_clausestart");
  else
    group(9,9,"_clausestart");
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_adj
	_xWILD [star match=(_advl _adv)]
	_conj
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_adj
	@@

##########################################################
# Mechanism to split up clauses.


# np vg vg
# ex: people bitten by mosquitos reacted violently.
@PRE
<3,3> varz("qc50 s-nvv");
@POST
  N("qc50 s-nvv",3) = 1;
  L("v5") = N("verb node",5);
  L("vc5") = vconj(L("v5"));
  if (L("vc5") == "-ed"
   || L("vc5") == "-edn"
   || L("vc5") == "-en"
   )
    {
	if (N(4))
      group(4,4,"_clausestart");
	else
	  group(5,5,"_clausestart");
	group(7,7,"_clausestart");
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_np
	_xWILD [star match=(_adv _advl)]
	_vg
	_xWILD [star match=(_adv)]
	_vg
	@@

@PRE
<1,1> varz("qc50 v-n-v");
@CHECK
  if (N("q-q remote",1) == "right"
   || N("q-q remote",5) == "left")
    fail();
@POST
  if (N("glom",3) == "right"
   || N("glom",1) == "left"
   || N("prosubj",3) )	# 01/08/05 AM.
    {
	group(3,3,"_clausestart");
	}
  else if (N("glom",3) == "left"
  	|| N("proobj",3))
    {
	# If more than one adverbial, need a _clauseend
	# to be implemented...
	}
  else
    {
#	if (!N("voice",5))
#      fixvg(N(5),"passive");
    group(5,5,"_clausestart");
	}
  N("qc50 v-n-v",1) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(_vg)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _adjc)]
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

# Redundant with the above. # 06/02/07 AM.
@PRE
<1,1> varz("qclause50: vg-adv-vg");	# Loop guard.	# 04/24/10 AM.
@CHECK
  ++N("qclause50: vg-adv-vg",1);	# Loop guard.	# 04/24/10 AM.
  # Look for bound verb groups.
  # As in "is john eating".
#  if (N("q-q remote",1) == "right"
#   || N("q-q remote",5) == "left")
#     fail();
#@POST
#  group(3,3,"_clausestart");
#@RULES
#_xNIL <-
#	_vg
#	_xWILD [star match=(_advl _adv)]
#	_np
#	_xWILD [star match=(_advl _adv)]
#	_vg
#	@@

# A third one! # 06/02/07 AM.
# vg np vg
#@CHECK
#  if (N("q-q remote",1) == "right"
#   || N("q-q remote",5) == "left")
#     fail();
#@POST
#  group(5,5,"_clausestart");
#@RULES
#_xNIL <-
#	_vg [s]
#	_xWILD [star match=(_advl _adv)]
#	_np [opt]
#	_xWILD [star match=(_advl _adv)]
#	_vg
#	@@

# vg vg
@POST
#  if (N("voice",1) == "active")
#    {
   if (N(2))	# 04/24/10 AM.
	group(2,2,"_clausestart");	# FIX	# 04/14/10 AM.
#	}
#  else
#    {
#    L("v") = N("verb node",1);
#    group(1,2,"_clause");
#	 setunsealed(1,"true");	# 07/10/12 AM.
#    }
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_adv _advl)]
	_vg [lookahead]
	@@

# clausal boundary
@POST
  group(5,5,"_clausestart");
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star lookahead match=(_advl _adv)]
	_vg
	@@

# vg prep pro
@POST
  group(3,3,"_clausestart");
@RULES
_xNIL <-
	_vg
	_prep
	_xWILD [s one match=(_proSubj) except=(_clausestart)]
	@@

# vg np conj np vg
@POST
  L("v1") = N("verb node",1);
  L("v5") = N("first verb",5);
  L("vc1") = vconj(L("v1"));
  L("vc5") = vconj(L("v5"));
  if (L("vc1") == "-ing"
   && L("vc5") == "-edn")
    {
    group(5,5,"_clausestart");
	group(2,4,"_np");
	}
  else
    group(3,3,"_clausesep");
@RULES
_xNIL <-
	_vg
	_np
	_conj
	_np
	_vg
	@@

# Default separations.
@POST
  group(7,7,"_clausestart");
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_advl _adv)]
	_prep
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

@POST
  group(5,5,"_clausestart");
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_vg
	@@


@POST
  group(3,3,"_clausestart");
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_advl _adv)]
	_vg
	@@


# Complex clausal patterns.
@CHECK
  if (N("voice",11) != "active")
    fail();
  if (N("voice",5)
   && N("voice",5) != "passive")
    fail();
@POST
  fixvg(N(5),"passive","VBN");
  if (N(4))
    group(4,4,"_clausestart");
  else
    group(5,5,"_clausestart");
  group(11,11,"_clausestart");
@RULES
_xNIL <-
    _xSTART								### (1)
    _xWILD [star match=(_advl _adv)]	### (2)
    _np									### (3)
    _xWILD [star match=(_advl _adv)]	### (4)
    _vg									### (5)
    to [s]								### (6)
    _vg									### (7)
    _xWILD [star match=(_advl _adv)]	### (8)
    _np									### (9)
    _xWILD [star match=(_advl _adv)]	### (10)
    _vg									### (11)
    @@


# np pro
@POST
  group(2,2,"_clausestart");
  N("relative-clause",2) = 1;
  # Maybe ellipted...
  # "book (that) she read..."
@RULES
_xNIL <-
	_np
	_xWILD [s one match=(_proPoss it) except=(_clausestart)]
	_vg [lookahead]
	@@

#####################################################
# , conj
@RULES
_clausesep <-
	\,
	_conj
	@@

# conj
@POST
  group(2,2,"_clausesep");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_conj
	@@

@CHECK
  if ((N("glom",1) != "left")
   && (N("glom",2) != "right") )
    fail();
@POST
  singler(2,2);
@RULES
_clausestart <-
	_xWILD [one fail=(_clausestart _clausesep)]
	_xWILD [one fail=(_clausestart _clausesep)]
	@@


# Conj at this point is a clause separator....
@RULES
_clausesep <-
	_conj
	@@

# ... than np np vg
@POST
  group(3,3,"_clausestart");
  N("glom",2) = "left";
  N("glom",3) = "right";
  fixvg(N(4),"active","VBP");
@RULES
_xNIL <-
	than [s]
	_np
	_np
	_vg
	_xWILD [one match=(_qEOS _xEND)]
	@@

# prep verb
@POST
  group(1,1,"_clausesep");
@RULES
_xNIL <-
	_prep
	_vg [lookahead]
	@@

# where
# when
@CHECK
  if (pnname(N(3)) == "_advl")
    fail();
@POST
  group(3,3,"_advl");
  N("fnword",3) = "whword";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_conj _adv _advl)]
	_xWILD [s one match=(where when)]
	@@

# Some kind of ellipted clause.
# , that verb adj
@CHECK
  if (pnname(N(2)) == "_np")
    fail();
@POST
  pnrename(N(2),"_np");
  if (!N("mypos",2))
    chpos(N(2),"DT");	# that/DT
  N("bracket",2) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(_xSTART \, _dbldash)]
	that [s]
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# Note: verbless.  np (who are) adj ... => NN
# np adj
@POST
  group(6,6,"_clausestart");
@RULES
_xNIL <-
	_xWILD [s star match=(_adv _advl)]
	_np [s]
	_xWILD [star match=(_adv _advl)]
	_adjc
	_xWILD [star match=(_adv _advl)]
	_xWILD [one fail=(_clausestart)]
	@@
