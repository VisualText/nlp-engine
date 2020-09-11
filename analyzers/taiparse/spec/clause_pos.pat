###############################################
# FILE: clause_pos.pat
# SUBJ: Default pos tagging for clauses.
# AUTH: AM
# CREATED: 30/Jun/06 16:06:14
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

#### DEFAULTS
# Some default pos tagging.
@CHECK
  if (N("mypos"))
    fail();
  L("nm") = pnname(N(1));
  if (L("nm") == "_adv"
   || L("nm") == "_prep")
    fail();
@POST
  chpos(N(1),"IN");	# as/IN. DEFAULT.
@RULES
_xNIL <-
	as [s]
	@@

@CHECK
  if (N("mypos"))
    fail();
  if (pnname(N(1)) != "_fnword")
    fail();
@POST
  chpos(N(1),"IN");	# that/IN. DEFAULT.
@RULES
_xNIL <- that [s] @@

@CHECK
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"JJR");	# more/JJR. DEFAULT.
@RULES
_xNIL <- more [s] @@

# GUESS.
@PRE
<1,1> varz("mypos");
@CHECK
  if (G("threshold") > 0)
	fail();	# Not guessing.	# 04/24/07 AM.
@POST
  if (vconjq(N(1),"-ing"))
	alphatoverb(1,"active","VBG");
  else if (N("noun"))
	alphatonoun(1);
  else if (N("adj"))
	alphatoadj(1);
  else if (N("verb"))
	{
	if (vconjq(N(1),"-en"))
		alphatoverb(1,"passive","VBN");
	else if (vconjq(N(1),"-ed"))
		alphatoverb(1,"passive","VBD");
	else
		alphatoverb(1,"active","VBP");
	}
  else if (N("adv"))
	alphatoadv(1);
  else
    alphatonoun(1); # Wild-ass quess.
@RULES
_xNIL <-
	_xALPHA
	@@

