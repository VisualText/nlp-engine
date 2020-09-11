###############################################
# FILE: dom200.pat
# SUBJ: Domain patterns.
# AUTH: AM
# CREATED: 18/Jul/04 22:36:58
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@POST
  L("start") = N(2);
  L("end") = lasteltnode(2);
  pnreplacevalrange("mypos","NP",L("start"),L("end"));
  singler(2,3);
@RULES
_company [layer=_noun] <-
	_xWILD [one match=(_dbldash \, _conj)]
	_xWILD [s plus match=(_xCAP) except=(_companyDESIG)]
	_companyDESIG [s]
	@@

# Idiom...
# look forward to
@POST
  N("particle",3) = 1; # Flag particle for use someday.

  L("tmp1") = N(1);
  L("tmp2") = N(2);
  N("verb",1) = 1;
  N("noun",1) = N("adj",1) = N("adv",1) = 0;
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));

  N("adv",2) = 1;
  N("adj",2) = 0;
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(look looks looking looked)]
	forward
	to [s]
	@@

# 10 day old
# num noun old
@CHECK
  S("number") = number(N(2));
  if (S("number") != "singular")
    fail();
@POST
  pncopyvars(3);
  single();
@RULES
_ambig <-
	_xWILD [one match=(_quan _num _xNUM)]
	_noun
	old
	@@

# days old
# noun old
@CHECK
  S("number") = number(N(1));
  if (S("number") != "plural")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  group(2,2,"_adjc");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"JJ");
@RULES
_xNIL <-
  _noun
  old
  @@
 
# about $
# about num
# around num	# 04/21/07 AM.
@POST
  pnrename(N(1),"_adv");
  if (G("conform treebank"))
	chpos(N(1),"IN");  # treebank: about
@RULES
_xNIL <-
	_xWILD [s one match=(around about)]
	_xWILD [one lookahead match=(_quan _num _xNUM)]
	@@
_xNIL <-
	_xWILD [s one match=(around about)]
	_xWILD [s one lookahead match=(_num _money _percent)]
	@@

# about noun
# around noun
# 04/12/07 AM.
@PRE
<2,2> var("num");
@POST
  pnrename(N(1),"_adv");
  if (G("conform treebank"))
	chpos(N(1),"IN");  # treebank: about
@RULES
_xNIL <-
	_xWILD [s one match=(around about)]
	_noun [lookahead]
	@@

# none of
@POST
  L("tmp1") = N(1);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"NN");
@RULES
_xNIL <-
	none
	of [s lookahead]
	@@

# noun num
@POST
  if (pnname(N(1)) == "_np")
    {
	N("bracket",1) = 0;	# CLEAR.
	group(1,2,"_money");
	group(1,1,"_np");
	N("bracket",1) = 1;
	N("money",1) = 1;
	}
  else
    {
	group(1,2,"_money");
	group(1,1,"_noun");
	N("money",1) = 1;
	}
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [s one match=(thousand million mln billion trillion)]
	@@

# Normalize tagging.
# chief executive officer
# chief operating officer
@POST
  pncopyvars(3);
  single();
@RULES
_noun <-
	chief [s]
	_xWILD [s one match=(executive operating financial)]
	_xWILD [s one match=(officer officers)]
	@@

# got
@POST
  N("-edn",3) = 0;
  N("-en",3) = 1;
@RULES
_xNIL <-
	_have [s]
	_adv [star]
	got [s]
	@@

@POST
  N("-edn") = 0;
  N("-ed") = 1;
@RULES
_xNIL <-
	got [s]
	@@

# for now
# Idiom, fairly arbitrary tagging.
@POST
 if (nonliteral(N(2)))
   pnrename(N(2),"_noun");
  chpos(N(2),"RB");
 group(1,2,"_advl");
@RULES
_xNIL <-
	for [s]
	now [s]
	@@

# Need a designator for default pos, original pos, etc.
# in Eastern
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_prep
	_xWILD [one match=(northern southern eastern western)]
	@@

# still hope
# Sometimes, only semantics will solve a thing.
# Or at least collocation knowledge...
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("txt") = strtolower(N("$text",2));
  if (strendswith(L("txt"),"s"))
    fixverb(N(2),"active","VBZ");
  else
    fixverb(N(2),"active",0); # Don't know yet...
@RULES
_xNIL <-
	still
	_xWILD [one match=(
		hope hopes
		)] @@


# come true.
# Seems highly idiomatic to me.
# come seems a copula, linking verb here.
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(come comes coming came)]
	true
	@@

# much of
# 2:1 jj:rb in corpus, so use that.
# "much of the food".
# Seems more like a quantifier/predeterminer, so silly to
# fret about whether it's a jj or rb here, or nn.
@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"JJ");
  group(1,2,"_advl");
@RULES
_xNIL <-
	much
	of [s]
	@@

# alpha of
@CHECK
  if (!N("verb",1))
    fail();	# Verb + one other.
  if (N("pos num",1) != 2)
    fail();
  L("txt") = nodesem(N(1));
  if (finddictattr(L("txt"),"verb-of"))
    fail();
@POST
  # Choose the non-verb.
  L("tmp1") = N(1);
  if (N("noun",1))
    {
	group(1,1,"_noun");
	pncopyvars(L("tmp1"),N(1));
	fixnoun(N(1));
	}
  else if (N("adj",1))
    {
	group(1,1,"_adj");
	pncopyvars(L("tmp1"),N(1));
	fixadj(N(1));
	}
  else if (N("adv",1))
    {
	group(1,1,"_adv");
	pncopyvars(L("tmp1"),N(1));
	}
  else
    {
	if (G("error"))
	"err.txt" << "alpha-of: " << phrasetext() << "\n";
	noop();
	}
@RULES
_xNIL <-
	_xALPHA
	of [s lookahead]
	@@

# well prep
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  group(2,3,"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_qEOS _xSTART \, )]
	well
	_prep
	@@

# both X and Y
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"CC");
@RULES
_xNIL <-
	both [s]
	_xWILD [one]
	and [s]
	@@

# what alpha is
@CHECK
  if (!N("verb",2))
    fail();
  S("number") = number(N(2));
  if (S("number") != "plural"
   || !N("-s",3))
   fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("verb node",2) = L("v");
  clearpos(N(2),1,0);
  fixvg(N(2),"active","VBP");
@RULES
_xNIL <-
	what [s]
	_xALPHA
	_be [s lookahead]
	@@

# what
# If not claimed elsewhere, give it a default here.
#@CHECK
#  if (N("mypos"))
#    fail();
#@POST
#  chpos(N(1),"WP");
#@RULES
#_xNIL <-
#	what [s]
#	@@

# Many adverbs are special.
# new alpha
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  fixadj(N(1));
  if (N("adj",2) && N("noun",2))
    ; # Don't know which.
  else if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
@RULES
_xNIL <-
	new
	_xALPHA
	@@

# det while
@POST
  chpos(N(3),"NN");
  pnrename(N(3),"_noun");
@RULES
_xNIL <-
	_xWILD [s one match=(a an the)]
	_xWILD [star match=(_adv)]
	while [s]
	@@

# while not alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	while [s]
	not [s]
	_xALPHA
	@@

# while alpha
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("noun",2) || !N("verb",2))
    fail();
  S("vc") = vconj(N(2));
  if (S("vc") != "inf"
   && S("vc") != "-s")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
@RULES
_xNIL <-
	while [s]
	_xALPHA
	@@

# while
@CHECK
  if (N("mypos",1) == "IN")
    fail();
@POST
  chpos(N(1),"IN");
@RULES
_xNIL <-
	while [s]
	_xWILD [s one fail=(away)]
	@@

# alpha
@CHECK
  # noun vs adj.
  if (N("pos num",2) != 2)
    fail();
  if (!N("noun",2) || !N("adj",2))
    fail();
@POST
  # Could be wrong, but choose adj for now.
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_xALPHA
	@@

# Idiom.
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	vice [s]
	_xALPHA
	_xWILD [one lookahead match=(_prep \, _qEOS _xEND)]
	@@

# idiom: after all
@POST
  group(1,2,"_advl");
@RULES
_xNIL <-
	after [s]
	all [s]
	_xWILD [one lookahead fail=(_det _quan _num _xNUM _adj _noun)]
	@@

# by then
# by fnword
# I disagree with pos assignment of RB here.  Should be NN.
@CHECK
  if (N("by-then",2))
    fail();
@POST
  L("tmp2") = N(2);
  pnrename(N(2),"_noun");
  chpos(N(2),"RB"); # Debatable.
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
  N("by-then",2) = 1;

@RULES
_xNIL <-
	by [s]
	_xWILD [s one match=(now then)]
	@@

# idiom
# those of them
@POST
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  group(3,3,"_np");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,1);
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,1);
  group(1,3,"_np");
@RULES
_xNIL <-
	those [s]
	of [s]
	_proObj [s]
	@@

# those who
@POST
  L("tmp1") = N(1);
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,1);
@RULES
_xNIL <-
	those [s]
	_xWILD [s one lookahead match=(who which that whom _prep)]
	@@

# because of
# because
# Note: pretagged has inconsistencies
# "because of" = sometimes 1st is RB, sometimes IN.
# DON'T CARE!
@CHECK
  if (N("mypos",1))
    fail();
@POST
  if (N(2))
    {
	L("tmp2") = N(2);
	chpos(N(1),"IN");
	group(1,2,"_prep");
	pncopyvars(L("tmp2"),N(2));
	N("because-of",1) = 1;
	clearpos(N(1),1,0);
	}
  else
    chpos(N(1),"IN");
@RULES
_xNIL <-
	because [s]
	of [s opt]
	@@

# even though
@CHECK
  if (N("mypos",1))
    fail();
@POST
  L("tmp2") = N(2);
  chpos(N(1),"RB");
  group(1,1,"_adv");
  group(1,2,"_fnword");
  pncopyvars(L("tmp2"),N(1));
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	even [s]
	though [s lookahead]
	@@

# there 's
@POST
  pnrename(N(2),"_verb");
  chpos(N(2),"VBZ");	# 's/VBZ
@RULES
_xNIL <-
	_xWILD [s one match=(there _nounpro)]
	_aposS
	@@
@POST
  if (!N("mypos",1))
    chpos(N(1),"WP");
  if (pnname(N(1)) == "_fnword")
    {
	group(1,1,"_np");
	N("fnword",1) = "whword";
	}
  pnrename(N(2),"_verb");
  chpos(N(2),"VBZ");
  N("apos-s",2) = 1;
@RULES
_xNIL <-
	_whword [s]
	_aposS
	@@

# that 's
@POST
#  L("tmp1") = N(1);
#  group(1,1,"_noun");
#  pncopyvars(L("tmp1"),N(1));
#  chpos(N(1),"WDT");

  pnrename(N(2),"_verb");
  chpos(N(2),"VBZ");
  N("verb",2) = 1;
@RULES
_xNIL <-
	that [s]
	_aposS
	@@

# pro 's
@POST
  pnrename(N(2),"_verb");
  chpos(N(2),"VBZ");	# 's/VBZ
  N("verb",2) = 1;
@RULES
_xNIL <-
	_pro
	_aposS
	@@

# noun 's and
@POST
  L("tmp1") = N(1);
  chpos(N(2),"POS");	# 's/POS
  group(1,2,"_adj");
  N("apos-s",1) = 1;
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	_noun
	_aposS
	_xWILD [one match=(_conj \) \} \] \, _dbldash _xEND)]
	@@

# 's and
@POST
  chpos(N(1),"POS");	# 's/POS
@RULES
_xNIL <-
	_aposS
	_xWILD [one match=(_conj \) \} \] \, _dbldash _xEND)]
	@@

# than predicted
# than alpha
@CHECK
  S("vc") = vconj(N(2));
  if (S("vc") != "-en"
   && S("vc") != "-edn"
   && S("vc") != "-ing")
   fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  if (S("vc") == "-ing")
    {
    L("voice") = "active";
	chpos(N(2),"VBG");
	}
  else if (S("vc") == "-en") # 02/19/05 AM.
    {
    L("voice") = "passive";
	chpos(N(2),"VBN");
	}
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = L("voice");
  N("first verb",2) = N("verb node",2) = L("v");
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	than [s]
	_xALPHA
	@@

# there is
# existential there.
@CHECK
  if (pnname(N(2)) == "_np")
    S("n") = pndown(N(2)); # Don't put mypos on an _np.
  else
    S("n") = N(2);
  if (pnvar(S("n"),"mypos"))
    fail();
@POST
  pnreplaceval(S("n"),"mypos","EX");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART \, _qEOS)]
	there [s]
	_be [s]
	@@

# once used to
@CHECK
  if (!N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
#  fixverb(N(2),"passive","VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_fnword)]
	_xALPHA
	_xWILD [s one lookahead match=(to)]
	@@

# a more
# det more
# more
@CHECK
  if (N("mypos",2))
    fail();
@POST
  chpos(N(2),"RBR");
  pnrename(N(2),"_adj");	# Forms np more easily...
@RULES
_xNIL <-
	a [s]
	more [s]
	@@

# more like
@CHECK
  if (N("mypos",1) == "RBR")
    fail();	# Loop guard.
@POST
  pnrename(N(2),"_prep");
  pnrename(N(1),"_adv");
  chpos(N(1),"RBR");
@RULES
_xNIL <-
	_xWILD [s one match=(more less)]
	like [s]
	@@

# more verb than
@POST
  chpos(N(1),"RBR");	# more/RBR
  chpos(N(3),"IN");	# than/IN
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	more [s]
	_verb [lookahead]
	than [s]
	@@

# more alpha than
@POST
  chpos(N(3),"IN");	# than/IN
  pnrename(N(1),"_adv");
  L("tmp2") = N(2);
  if (N("adj",2))
    {
	chpos(N(1),"RBR");	# more/RBR
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (N("adv",2))
    {
	chpos(N(1),"RBR");	# more/RBR
	group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	fixadv(N(2));
	}
  else if (N("verb",2) && !vconjq(N(2),"inf"))
    {
	chpos(N(1),"JJR");	# more/JJR
	group(2,2,"_verb");
	pncopyvars(L("tmp2"),N(2));
	}
  else if (N("noun",2))
    {
	chpos(N(1),"JJR");	# more/JJR
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	}
@RULES
_xNIL <-
	more [s]
	_xALPHA
	than [s]
	@@

# be alpha
# be not alpha
@CHECK
  if (!N("adj",3) || !N("verb",3))
    fail();
  if (N("adv",3) || N("noun",3))
    fail();
  if (!vconjq(N(3),"inf"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
@RULES
_xNIL <-
	_be [s]
	not [s opt]
	_xALPHA
	@@

# be alpha
@CHECK
  if (!N("verb",2))
    fail();
  S("vc") = vconj(N(2));
  if (S("vc") != "-ing"
   && S("vc") != "-en"
   && S("vc") != "-edn")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_be [s]
	_xALPHA
	@@

# fnword not alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"active","VBG");
@RULES
_xNIL <-
	_xWILD [one match=(_fnword)]
	not [s]
	_xALPHA
	@@


# how long
# how alpha
@CHECK
  if (!N("adj",2) || !N("verb",2))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	how [s]
	_xALPHA
	@@

# how about
@POST
  if (!N("mypos",2))
    chpos(N(2),"IN");
@RULES
_xNIL <-
	how [s]
	about [s lookahead]
	@@

# how much
# how many
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  group(1,2,"_howmuch");
  group(1,1,"_fnword");
@RULES
_xNIL <-
	how [s]
	_xWILD [one match=(much many)]
	@@

# in return
# idiom
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	in [s]
	return
	@@

# This trend
# this alpha
@CHECK
  if (!N("noun",3))
    fail();
  if (!singular(N(3)))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_qEOS _xSTART)]
	this [s ren=_det]
	_xALPHA
	@@

# idiom: the most part.
# the most part
# det most noun
@POST
  pnrename(N(2),"_adj");
  chpos(N(2),"JJS");
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  chpos(N(3),"NN");
@RULES
_xNIL <-
	the [s]
	most [s]
	part
	@@

# idiom: most of.
# most of
@POST
  chpos(N(1),"JJS");
@RULES
_xNIL <-
	most [s]
	of [s lookahead]
	@@

# most alpha
@CHECK
  if (!N("noun",2))
    fail();
  L("txt") = nodetext(N(2));
  "text.txt" << L("txt") << "\n";
  if (!strendswith(L("txt"),"s"))
    fail();
@POST
  pnrename(N(1),"_adj");
  chpos(N(1),"JJS");
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"NNS");
@RULES
_xNIL <-
	most [s]
	_xALPHA
	@@

# most alpha
@CHECK
  if (N("mypos",1))
    fail();
  if (!N("noun",2) || !N("verb",2))
    fail();
  if (N("pos num",2) != 2)
    fail();
@POST
  chpos(N(1),"JJS");
  pnrename(N(1),"_adj");
@RULES
_xNIL <-
	most [s]
	_xALPHA [lookahead]
	@@

# very disappointing
# premod alpha
@CHECK
  if (!N("adj",2) && !N("verb",2))
    fail();
  S("vc") = vconj(N(2));
  if (S("vc") != "-ing"
   && S("vc") != "-en"
   && S("vc") != "-edn")
   fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
  group(1,2,"_adj");
#  pncopyvars(L("tmp2"),N(1));
@RULES
_xNIL <-
	_premod
	_xALPHA
	@@

# premod alpha
@CHECK
  if (!N("adj",2))
    fail();
@POST
  alphatoadj(2);
@RULES
_xNIL <-
	_premod
	_xALPHA
	@@

# ago
@POST
  pncopyvars(1);
  single();
@RULES
_noun <-
	_xALPHA
	ago [s]
	@@

# ," alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"-ed"))
   fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  L("v") = N(3);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  N("voice",3) = "active";
@RULES
_xNIL <-
	\, [s]
	\" [s]
	_xALPHA
	@@

# to alpha alpha .
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);

  if (N("noun",3) &&
      (N("mass",3) || plural(N(3)) || N("cap",3)) )
    {
    group(3,3,"_noun");
    pncopyvars(L("tmp3"),N(3));
    fixnoun(N(3));
    group(3,3,"_np");
    pncopyvars(L("tmp3"),N(3));
    clearpos(N(3),1,1);
    }
  else if (copulaq(N(2)) && N("adj",3))
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	}
  else if (N("adv",3))
    {
	group(3,3,"_adv");
	pncopyvars(L("tmp3"),N(3));
	}

  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  chpos(N(2),"VB");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  N("verb node",2) = L("v");
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	to [s]
	_xALPHA
	_xALPHA
	_xWILD [one match=(_qEOS _prep _xEND)]
	@@

# Weak rule.
# alpha alpha ,
@CHECK
  if (N("pos50 a-a-comma",2))
    fail();	# Loop guard.
@POST
  N("pos50 a-a-comma",2) = 1;
  L("tmp3") = N(3);
  if (!N("verb",3))
    {
	if (N("noun",3))
	  {
	  group(3,3,"_noun");
	  pncopyvars(L("tmp3"),N(3));
	  }
	}
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=( \, )]
	@@

# alpha fnword
@CHECK
  if (!N("noun",2))
    fail();
  if (N("verb",2))
    {
	S("vc") = vconj(N(2));
	if (S("vc") != "-s"
	 && S("vc") != "inf")
	  fail(); # Too ambiguous.
	}
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xALPHA
	_xWILD [one lookahead match=(_fnword _prep)]
	@@

# too far
@CHECK
  if (N("mypos",2))
    fail();
@POST
  # Conformity. (967/35 RB/JJ).
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(too very extremely really)]
	far
	@@

# adv alpha
# too alpha
@CHECK
  if (!N("adj",2) && !N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  N("noun",2) = N("verb",2) = 0; # Zero these out.
  if (!N("adj",2))
     {
	 group(2,2,"_adv");
	 pncopyvars(L("tmp2"),N(2));
	 }
  else if (!N("adv",2))
     {
	 group(2,2,"_adj");
	 pncopyvars(L("tmp2"),N(2));
	 fixadj(N(2));
	 }
@RULES
_xNIL <-
	_xWILD [s one match=(too)]
	_xALPHA
	@@

# Catching a bad sentence non-break.
@POST
  N("eos",2) = 1;	# END OF SENTENCE.
  ++G("eos");		# Count new sentence breaks.
@RULES
_xNIL <-
	_prep
	_citystate [s]
	_xWILD [s one lookahead match=(_xCAP)]
	@@

# as adj as
# as adv as
# as verb as
@POST
  chpos(N(3),"IN");
  chpos(N(1),"RB");
  single();
@RULES
_fnword <-
	as [s]
	_xWILD [one match=(_adj _adv _verb)]
	as [s]
	@@

# TOO BROAD.
# as adj
#@CHECK
#  if (N("mypos",1))
#    fail();
#@POST
#  chpos(N(1),"RB");
#  pnrename(N(1),"_adv");
#@RULES
#_xNIL <-
#	as [s]
#	_xWILD [one lookahead match=(_adj)]
#	@@

# as alpha as
@POST
  L("tmp2") = N(2);
  chpos(N(3),"IN");
  chpos(N(1),"RB");
  if (N("adj",2) && !N("adv",2))
	group(2,2,"_adj");
  else if (N("adv",2) && !N("adj",2))
    group(2,2,"_adv");
  else if (N("adj",2) && N("adv",2))
    group(2,2,"_adj");	# Should check cases...
  else if (N("verb",2))
    group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
  S("as-x-as") = 1;
  single();
@RULES
_fnword <-
	as [s]
	_xALPHA
	as [s]
	@@


# noun - alpha
@CHECK
  if (!N("verb",3) || !N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),0,0);
  chpos(N(3),"JJ");
  group(1,3,"_adj");
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_noun
	\-
	_xALPHA
	@@

# year was
@CHECK
  S("num") = num(N("$text",1));
  if (S("num") < 1990
   || S("num") > 2010)
     fail();
@POST
  group(1,1,"_noun");
  group(1,1,"_np");
  N("sem",1) = "date";
@RULES
_xNIL <-
	_xNUM
	_xWILD [s one lookahead match=(is was has does)]
	@@

# noun noun
# Domain semantics.
@CHECK
  if (!N("cap",2))
    fail();
  if (N("ne type",2))
    fail();
  N("ne",2) = 1;
  N("ne type conf",2) = 70; # A guess.
  if (!N("stem",2))
    N("stem",2) = N("$text",2);
  if (N("sem",2) == "date")
    N("ne type",2) = "date";
#  else if (N("ne type",1) == "location")
#    N("ne type",2) = "organization"; # Try this.
  else if (N("ne type",1) == "person")
    {
	registerpersnames(N("$text",2));
	N("ne type",2) = "person";
	}
  else if (N("ne type",1))
    {
    N("ne type",2) = N("ne type",1);
	N("ne type conf",2) = 50;
	}
  fail();	# So that other rules in this pass will still match.
@RULES
_xNIL <-
	_noun
	_noun
	_xWILD [one lookahead fail=(_xALPHA)]
	@@

# Domain semantics.
@CHECK
  if (!N("cap",1))
    fail();
  if (N("ne type",1))
    fail();
@POST
  N("ne type",1) = N("sem",1) = "location";
  N("ne type conf",1) = 90;
  N("ne",1) = 1;
@RULES
_xNIL <-
	_noun
	_xWILD [one s match=(
		province
		)]
	@@

# said X
@CHECK
  if (N("stem",1) != "say")
    fail();
  if (N("ne type",2))
    fail();
  if (N("sem",2) != "name")
    fail();
@POST
  N("ne type",2) = N("sem",2) = "person";
  N("ne type conf",2) = 50;
  N("stem",2) = N("$text",2);
@RULES
_xNIL <-
	_xWILD [one match=(_verb say says saying said)]
	_noun
	@@

# X said in its ...
# (not a person, but an org, perhaps news org)
@CHECK
  if (N("stem",2) != "say")
    fail();
  if (N("ne type",1))
    fail();
  if (N("sem",1) != "name"
   && !N("cap",1))
    fail();
@POST
  N("ne type",1) = N("sem",1) = "organization";
  N("ne type conf",1) = 95;	# Confidence that it's an org.
  N("stem",1) = N("$text",1);
  if (pnname(N(2)) != "_verb")
    {
    L("tmp2") = N(2);
    group(2,2,"_verb");
    pncopyvars(L("tmp2"),N(2));
    L("v") = N(2);
    group(2,2,"_vg");
    mhbv(N(2),L("neg"),0,0,0,0,L("v"));
    pncopyvars(L("tmp2"),N(2));
    N("verb node",2) = L("v");
    clearpos(N(2),1,0);
    fixvg(N(2),"active",0);
	}
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_verb say says saying said)]
	_prep
	its [s]
	@@

# X said
@CHECK
  if (N("stem",2) != "say")
    fail();
  if (N("ne type",1))
    fail();
  if (N("sem",1) != "name"
   && !N("cap",1))
    fail();
@POST
  N("ne type",1) = N("sem",1) = "person";
  N("ne type conf",1) = 60;
  N("stem",1) = N("$text",1);
  if (pnname(N(2)) != "_verb")
    {
    L("tmp2") = N(2);
    group(2,2,"_verb");
    pncopyvars(L("tmp2"),N(2));
    L("v") = N(2);
    group(2,2,"_vg");
    mhbv(N(2),L("neg"),0,0,0,0,L("v"));
    pncopyvars(L("tmp2"),N(2));
    N("verb node",2) = L("v");
    clearpos(N(2),1,0);
    fixvg(N(2),"active",0);
	N("glom",2) = "left";	# 04/21/07 AM.
	}
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_verb say says saying said)]
	@@

# alpha being
# "being" is important, needs careful handling.
# Often ambiguous: "The human being changed."
@CHECK
  if (!plural(N(1)))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  N("sem",3) = N("stem",3) = "be";
  chpos(N(3),"VBG");
@RULES
_xNIL <-
	_xWILD [one match=(_xALPHA _noun)]
	_xWILD [star match=(_adv _advl)]
	being
	@@

# carrier X
@CHECK
  if (N("stem",1) != "carrier")
    fail();
  if (!N("cap",2) && N("sem",2) != "name")
    fail();
  if (N("ne type",2))
    fail();
@POST
  N("ne type",2) = "naval vessel";
  N("ne type conf",2) = 75;
@RULES
_xNIL <-
	_noun
	_noun
	@@

# name of org
# noun of noun
@CHECK
  if (N("ne type",1))
    fail();
  if (N("ne type",3) != "organization")
    fail();
  if (!N("cap",1) && N("sem",1) != "name")
    fail();
@POST
  N("ne type",1) = "person";
  N("ne type conf",1) = 75;
  if (!N("stem",1))
    N("stem",1) = N("$text",1);
@RULES
_xNIL <-
	_noun
	of [s]
	_noun
	@@


# alpha .
# Transitive verbs with one other alternative.
@CHECK
  if (N("pos num",1) != 2)
    fail();
  if (!N("verb",1))
    fail();
  if (!finddictattr(N("$text",1),"trans"))
    fail();
@POST
  L("tmp1") = N(1);
  if (N("noun",1))
    {
	group(1,1,"_noun");
	pncopyvars(L("tmp1"),N(1));
	fixnoun(N(1));
	}
  else
    "trans.txt" << "Other alt: " << N("$text",1) << "\n";
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@


# what
# If not claimed elsewhere, give it a default here.
@CHECK
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"WP");
@RULES
_xNIL <-
	what [s]
	@@

# Parentheticals.	# 06/05/06 AM.
@POST
  excise(3,3);
  excise(1,1);
@RULES
_xNIL <-
	\{
	_xANY
	\}
	@@

# Fix up.	# 06/15/06 AM.
# 'd
# ' d
@POST
  L("tmp1") = N(1);
  if (vconjq(N(3),"inf"))
    {
	group(1,1,"_modal");
	pncopyvars(L("tmp1"),N(1));
	chpos(N(1),"MD");
	N("sem",1) = N("stem",1) = "will";
	N("text",1) = "would";
	}
  else # Only certain ones agree....
    {
	group(1,1,"_have");
	group(1,1,"_verb");
	pncopyvars(L("tmp1"),N(1));
    chpos(N(1),"VBD");
	N("sem",1) = N("stem",1) = "had";
	N("text",1) = "have";
	N("verb",1) = 1;
	N("pos num",1) = 1;
	}
@RULES
_xNIL <-
	_aposD
	_xWILD [star match=(_adv _advl)]
	_xWILD [one lookahead match=(_verb)]
	@@

# her
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"PP$");
@RULES
_xNIL <-
	her [s]
	_xWILD [one lookahead match=(_adj _noun)]
	@@

# 's own
# own
@CHECK
  if (N("mypos",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"JJ");
@RULES
_xNIL <-
	_aposS
	own
	@@

# due num
# (which is) due (in the year) 2007.
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_num");
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_np");
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  group(1,2,"_advl");	# Kind of an adverbial idiom.
@RULES
_xNIL <-
	due
	_xNUM
	@@

# from alpha to alpha
@CHECK
  if (!N("noun",2) || !N("noun",4))
    fail();
@POST
  alphatonoun(2);
  alphatonoun(4);
@RULES
_xNIL <-
	from [s]
	_xALPHA
	to [s]
	_xALPHA
	@@

# equal to
@POST
  alphatoadj(1);
@RULES
_xNIL <-
	equal
	to [s lookahead]
	@@

# all alpha
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  alphatoverb(2,"active","VBP");
@RULES
_xNIL <-
	all [s]
	_xALPHA
	@@

# det first
@POST
 L("tmp2") = N(2);
 group(2,2,"_quan");
 pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"JJ");
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	first
	_xWILD [one lookahead match=(_quan _num _xNUM _adj _noun)]
	@@

# each increased by 10.
@CHECK
  if (!N("verb",2))
    fail();
@POST
  alphatoverb(2,0,"VBP");
@RULES
_xNIL <-
	each [s]
	_xALPHA
	_xWILD [s one match=(by to from)]
	_xWILD [one match=(_noun _np)]
	@@

# Semantics: scalar-verb followed by number.
@PRE
<1,1> var("verb");
<2,2> var("num");
@CHECK
  if (!nodefeat(N(1),"vscalar"))
    fail();
@POST
  alphatoverb(1,0,0);
@RULES
_xNIL <-
	_xALPHA
	_noun [lookahead]
	@@

# alpha to
# close to
# Note: adjective phrase idioms.
@POST
  alphatoadj(1);
  group(1,1,"_adjc");
@RULES
_xNIL <-
	_xWILD [one match=(close)]
	to [s lookahead]
	@@

# who verb
@POST
  if (vconjq(N(2),"-ed"))
 	fixverb(N(2),"active","VBD");
  else if (vconjq(N(2),"inf"))
	fixverb(N(2),"active","VBP");
@RULES
_xNIL <-
	who [s]
	_verb
	@@

# far higher
# much higher.
# alpha adj
@POST
  alphatoadv(1);
  group(1,2,"_adj");
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(far much)]
	_adj [lookahead]
	@@

# no sooner
@POST
  chpos(N(1),"RB");
  chpos(N(2),"RB");
@RULES
_xNIL <-
	no [s]
	sooner [s]
	@@

# sooner or later
@POST
  chpos(N(1),"RB");
  chpos(N(3),"RB");
@RULES
_xNIL <-
	sooner [s]
	or [s]
	later [s]
	@@

# based in geoloc
@PRE
<4,4> vareq("sem","geoloc");
@POST
  alphatovg(2,"passive","VBN");
  group(4,4,"_np");
  group(3,4,"_advl");
  group(1,4,"_advl");
@RULES
_xNIL <-
	\, [opt]
	based
	_prep
	_noun
	\, [opt]
	@@

# the other
# Note: det, like "another".
# Note: But conform to Treebank. # 04/21/07 AM.
@POST
  group(1,2,"_det");
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	_det
	other [s]
	@@

# to subject
@POST
  alphatoverb(3,"active","VB");
@RULES
_xNIL <-
	to [s]
	_xWILD [star match=(_adv _advl)]
	subject
	@@
 
# subject to
@POST
  alphatoadj(1);
@RULES
_xNIL <-
	subject
 	_xWILD [star lookahead match=(_adv _advl)]
	to [s]
	@@

# plus
@POST
  if (N("conj",3))
	{
	--N("pos num",3);
	N("conj",3) = 0;
	}
@RULES
_xNIL <-
	_det
	_adj [star]
	plus
	@@

# plus
@POST
  alphatoadj(2);
@RULES
_xNIL <-
	\-
	plus
	@@

# plus
@POST
  L("tmp1") = N(1);
  group(1,1,"_conj");
  pncopyvars(L("tmp1"),N(1));
  N("mypos",1) = "CC";	# plus/CC	# 04/22/07 AM.
@RULES
_xNIL <-
	plus
	@@

# up to
# 04/22/07 AM.
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"IN");	# up/IN
@RULES
_xNIL <-
	up [s]
	to [s lookahead]
	@@

# Note: Fix up a noun meaning for VING/VBG that need it.
# prep alpha verb verb
@PRE
<2,2> var("adj");
@CHECK
  if (!vconjq(N(3),"-ing"))
	fail();
  L("stem") = nodestem(N(2));
  L("core") = finddictsattr(L("stem"),"core");
  if (L("core") != "adj")
	fail();
@POST
  pnrename(N(3),"_noun");
  fixnoun(N(3));
  fixnpnonhead(2);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_verb
	_xWILD [s one lookahead match=(_be _have _modal)]
	@@

# 5 times stronger
@PRE
<2,2> var("noun");
<3,3> var("adj");
@CHECK
  # Need comparative adj.
  L("w") = strtolower(N("$text",3));
  L("c") = adjconj(L("w"));
  if (L("c") != "JJR")
	fail();
@POST
  alphatoadj(3);
  group(3,3,"_adjc");
  fixnphead(2);
  N("sem",2) = "scalar";
@RULES
 _xNIL <-
	_xWILD [one match=(_num _xNUM)]
	_xALPHA
	_xALPHA
 	@@

# together
@POST
	alphatoadv(1);
@RULES
_xNIL <-
	together
	@@
