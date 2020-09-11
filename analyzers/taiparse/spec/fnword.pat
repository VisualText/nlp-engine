###############################################
# FILE: fnword.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 11/Jul/04 01:57:24
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

# Idioms....
@POST
 L("tmp1") = N(1);
 L("tmp3") = N(3);
 group(1,1,"_det");
 pncopyvars(L("tmp1"),N(1));
 group(3,3,"_caps");
 group(3,3,"_noun");
 pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(a the these) gp=_det]
	_xWHITE [star]
	us
	@@

@POST
  S("sem") = "person";
  S("stem") = strtolower(N("$text"));
  S("need-vconj") = "-s";
  pncopyvars();
  single();
@RULES
_proSubj [layer=(_pro)] <- _xWILD [one match=(
	he she
	)] @@

@POST
  S("sem") = "person";
  S("stem") = strtolower(N("$text"));
  S("need-vconj") = "inf";	# Except for verb to be...
  pncopyvars();
  single();
@RULES
_proSubj [layer=(_pro)] <- _xWILD [one match=(
	I we they
	)] @@
_proObj [layer=(_proSubj _pro)] <- you [s] @@
_proObj [layer=(_pro)] <- _xWILD [s one match=(
	me him them us
	)] @@
#@POST
#  pncopyvars();
#  S("mypos") = "NN";
#  single();
#@RULES
#_pro <- _xWILD [one match=(
#	anything	# 06/24/06 AM.
#	)] @@

@POST
  pncopyvars();
  S("mypos") = "NN";
  S("nounpro") = 1;	# Give this class of words a name.
  single();
@RULES
_noun <- _xWILD [one match=(
	anybody anyone anything
	everybody everyone everything
	somebody someone something
	) ] @@
@POST
  pncopyvars();
  S("neg") = 1;
  S("mypos") = "NN";
  S("nounpro") = 1;	# Give this class of words a name.
  single();
@RULES
_nounpro [layer=_noun] <- _xWILD [one match=(
	nobody noone nothing
	)] @@

@POST
  L("tmp1") = N(1);
  group(1,1,"_proPoss");
  group(1,1,"_pro");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"PP$");
  N("proposs",1) = 1;
@RULES
_xNIL <- _xWILD [s one match=(
	our my your his their its)] @@

# Lone "it".
@POST
  L("tmp1") = N(1);
  group(1,1,"_pro");
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"PP");
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  N("pro",1) = 1;	# 01/08/05 AM.
  N("sem",1) = N("stem",1) = "it";
  clearpos(N(1),1,1);
@RULES
_xNIL <-
  it [s]
  _xWILD [s one lookahead match=(_xWHITE _xPUNCT _xALPHA)
     except=( \' )]
  @@

@POST
  pncopyvars();
  S("proposs",1) = 1;
  single();
@RULES # 10
_proObj [layer=(_proPoss _pro)] <- her @@
@POST
  pncopyvars();
  single();
@RULES
_pro <- _xWILD [one match=(
	mine ours yours hers theirs
	)] @@

@POST
  pncopyvars();
  S("pro-reflexive") = 1;
  single();
@RULES
_proObj [layer=(_pro)] <- _xWILD [one match=(	# 06/02/02 AM.
	myself yourself himself herself itself ourselves
	yourselves themselves
	)] @@

##### VERB "TO BE" #####
@POST
  pncopyvars();
  S("inf") = 1;
  S("sem") = "be";
  S("stem") = "be";
  S("mypos") = "VB";
  S("tense") = "future";
  S("verb") = 1;
  single();
@RULES
_be [layer=(_verb)] <- be @@

@POST
  pncopyvars();
  S("-en") = 1;
  S("sem") = "be";
  S("stem") = "be";
  S("verb") = 1;
  single();
@RULES
_be [layer=(_verb)] <- been @@

@POST
  pncopyvars();
  S("person") = "1";
  S("number") = "singular";
  S("tense") = "present";
  S("sem") = "be";
  S("stem") = "be";
  S("verb") = 1;
  single();
@RULES
_be [layer=(_verb)] <- am @@

@POST
  pncopyvars();
  S("person") = "2s-123p";
  S("number") = "2s-123p";
  S("tense") = "present";
  S("sem") = "be";
  S("stem") = "be";
  S("mypos") = "VBP";
  S("verb") = 1;
  single();
@RULES
_be [layer=(_verb)] <- are @@

@POST
  pncopyvars();
  S("person") = "13s";
  S("number") = "singular";
  S("tense") = "past";
  S("-ed") = 1;
  S("sem") = "be";
  S("stem") = "be";
  S("verb") = 1;
  single();
@RULES
_be [layer=(_verb)] <- was @@

@POST
  pncopyvars();
  S("person") = "2s-123p";
  S("number") = "2s-123p";
  S("tense") = "past";
  S("-ed") = 1;
  S("sem") = "be";
  S("stem") = "be";
  S("verb") = 1;
  single();
@RULES
_be [layer=(_verb)] <- were @@

@POST
  pncopyvars();
  S("person") = "3";
  S("number") = "singular";
  S("tense") = "present";
  S("-s") = 1;
  S("sem") = "be";
  S("stem") = "be";
  S("verb") = 1;
  single();
@RULES
_be [layer=(_verb)] <- is @@

# some of
@POST
  pncopyvars(1);
  S("mypos") = "DT";
  singler(1,1);
@RULES
_noun <- # 20
	_xWILD [s one match=(any some)]
	_xWHITE [star]
	of [s lookahead]
	@@

@POST
  pncopyvars();
  S("number") = "any";
  S("stem") = strtolower(N("$text"));
  S("needs-np") = 1;
  single();
@RULES
_det <- the [s] @@
@POST
  pncopyvars();
  S("number") = "any";
  S("stem") = strtolower(N("$text"));
  single();
@RULES
_det <- _xWILD [s one match=(
	any some)] @@

@POST
  pncopyvars();
  S("number") = "singular";
  S("needs-np") = 1;
  single();
@RULES
_det <- _xWILD [one match=(a an every) except=(_letabbr)] @@

@POST
  pncopyvars();
  S("number") = "singular";
  S("stem") = strtolower(N("$text"));
  single();
@RULES
#_det [layer=(_fnword)] <- this @@
_det <- this @@
_det <- another @@ # 10/13/04 AM.
_det <- each @@

# all
@POST
  pncopyvars(1);
  S("number") = "plural";
  S("mypos") = "PDT";
  S("stem") = strtolower(N("$text"));
  singler(1,1);
@RULES
_det <-
	_xWILD [s one match=(all half)]
	_xWHITE [star]
	_xWILD [s one lookahead match=(
		the this these those
		my his her their our your its
		)]
	@@

@POST
  pncopyvars();
  S("number") = "plural";
  S("mypos") = "DT";
  S("stem") = strtolower(N("$text"));
  single();
@RULES
_det <- _xWILD [s one match=(all)] @@
_det [layer=(_fnword)] <- _xWILD [one match=(these those)] @@

@POST
  pncopyvars();
  single();
@RULES
_adj <- few @@ # Empirically... # rule30
_adj <- several @@ # Empirically...

@POST
  pncopyvars();
  S("number") = "plural";
  single();
@RULES
_fnword <- both @@

@POST
  N("stem",1) = "there";
  L("tmp1") = N(1);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,0);
  N("bracket",1) = 1;
@RULES
_xNIL <- there @@

@POST
  pncopyvars();
  S("stem") = strtolower(N("$text"));
  single();
@RULES

# Ambiguous - can also be a verb, etc.
_fnword <- while @@


# Special phrase separation properties.
# fnword except where shown to be a verb infinitive or advl.
_fnword <- please @@

_fnword <- once @@	# 09/08/04 AM.

# Want to watch for prepositional "like".
_fnword <- like @@ # 10/12/04 AM.

# adv or conj.
_fnword <- yet @@	# 10/18/04 AM.

@POST
  pncopyvars();
  S("stem") = strtolower(N("$text"));
  if (G("conform treebank"))
    S("mypos") = "IN";	# unless/IN
  single();
@RULES
# Supposedly a conjunction, but prob more global than "and".
_fnword <- unless @@

# no one
# one
@POST
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  chpos(N(3),"NN");
  
  group(1,1,"_det");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <- # 40
	no
	_xWHITE [star]
	one
	@@

# one
@POST
  N("pos_np_head") = "NN";
@RULES
_xNIL <-
	one
	@@

@POST
  pncopyvars();
  S("neg") = 1;
  single();
@RULES
_det <- no @@

@POST
  pncopyvars();
  ++X("func words");
  S("mypos") = "WP$";
  single();
@RULES

# Treating this first and foremost as English fn word.
_fnword <- whose @@	# 02/26/02 AM.

@POST
  pncopyvars();
  ++X("func words");
  S("mypos") = "RB";
  single();
@RULES
_fnword <- then @@	# 05/23/02 AM.

@POST
  pncopyvars();
  ++X("func words");
  single();
@RULES
# Except can be a verb, conj, prep.
_fnword <- except @@

@POST
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	rather
	_xWILD [one lookahead fail=(_adj _adv)]
	@@

_xNIL <-
	just
	@@

# "Adverbs" that premodify adjectives.
@POST
  pncopyvars();
  S("mypos") = "RB";
  single();
@RULES
_premod <- _xWILD [s one match=(
	very extremely totally wildly highly
	quite rather fairly
	considerably
#	most
	least
	)] @@

# Doesn't increase degree.
_premod <- slightly @@

@POST
  pncopyvars();
  single();
@RULES
_fnword <- most @@ # 50

@POST
  pncopyvars();
  S("mypos") = "MD";
  single();
@RULES
_modal <- _xWILD [s one match=(
	will would can could may might shall should ought
	must
	)] @@

@POST
  pncopyvars();
  S("inf") = 1;
  S("sem") = "have";
  S("tense") = "present";
  single();
@RULES
_have [layer=(_verb)] <- have @@
@POST
  pncopyvars();
  S("-s") = 1;
  S("sem") = "have";
  S("tense") = "present";
  single();
@RULES
_have [layer=(_verb)] <- has @@
@POST
  pncopyvars();
  S("-ing") = 1;
  S("sem") = "have";
  S("tense") = "present";
  single();
@RULES
_have [layer=(_verb)] <- having @@
@POST
  # Note that -en is idiomatic. "I've been had".
 pncopyvars();
 S("-edn") = 1;
  S("sem") = "have";
  S("tense") = "present";
  single();
@RULES
_have [layer=(_verb)] <- had @@

@POST
  ++X("func words");
  pncopyvars();
  S("inf") = 1;
  S("sem") = S("stem") = "do";
  single();
@RULES
#_do [layer=(_modal _verb)] <- do @@	# 06/09/06 AM.
_do [layer=(_verb)] <- do @@

@POST
  ++X("func words");
  pncopyvars();
  S("-s") = 1;
  S("sem") = "do";
  S("stem") = "do";
  single();
@RULES
_do [layer=(_verb)] <- does @@	# 06/09/06 AM.

@POST
  ++X("func words");
  pncopyvars();
  S("-ing") = 1;
  S("sem") = "do";
  S("stem") = "do";
  single();
@RULES
_do [layer=(_verb)] <- doing @@

@POST
  ++X("func words");
  pncopyvars();
  S("-ed") = 1;
  S("sem") = "do";
  S("stem") = "do";
  single();
@RULES
_do [layer=(_verb)] <- did @@

@POST
  ++X("func words");
  pncopyvars();
  S("-en") = 1;
  S("sem") = "do";
  S("stem") = "do";
  single();
@RULES
_do [layer=(_verb)] <- done @@ # 60

@POST
  pncopyvars();
  S("neg") = 1;	# Negative.
  single();
@RULES
_neg [layer=(_adv)] <- _xWILD [s one match=(
	not never
	)] @@

# idiom: as alpha as
# as far as
# as long as
# as well as
# so long as
@POST
  chpos(N(1),"RB");
  chpos(N(3),"RB");  # far: Conformity (105/25 = RB/JJ).
  chpos(N(5),"IN");
  single();
@RULES
_fnword <-
	_xWILD [s one match=(as so)]
	_xWHITE [s star]
	_xWILD [s match=(far long well)]
	_xWHITE [s star]
	as [s]
	@@
@POST
  chpos(N(1),"RB");
  chpos(N(3),"JJ");  # Conformity
  chpos(N(5),"IN");
  single();
@RULES
_fnword <- # 63
	_xWILD [s one match=(as so)]
	_xWHITE [s star]
	_xWILD [s one match=(many much)]
	_xWHITE [s star]
	as [s]
	@@

# as well
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"RB");
  chpos(N(3),"RB");
  group(1,3,"_adv");
@RULES
_xNIL <-  # 64
	as [s]
	_xWHITE [star]
	well [s]
	@@

# idiom: per capita.
@POST
  single();
@RULES
_adv [layer=_adj] <- per [s] _xWHITE [s star] capita [s] @@

@POST
  chpos(N(1),"IN");
  chpos(N(3),"JJS");
  single();
@RULES
_adv <- at [s] _xWHITE [s star] _xWILD [s one match=(most least)] @@

@POST
  pncopyvars();
  S("sem") = "date";
  S("mypos") = "RB";	# 01/13/05 AM.
  single();
@RULES
#_adv <-	# 10/22/04 AM.
#	now
#	@@
_fnword <- now @@	# 10/22/04 AM.

@POST
  pncopyvars();
  single();
@RULES
_adv <- nevertheless @@

@POST
  pncopyvars();
  single();
@RULES
# more
# Treating as an English function word.
# Note more as noun (mor-ay ...).
_fnword <-
	more
	@@
_fnword <- # 70
	less
	@@

# more than
# less than
#@POST
#  if (!nonliteral(N(5)))
#    {
#    L("tmp5") = N(5);
#	group(5,5,"_prep"); # subordinating conjunction, really.
#	pncopyvars(L("tmp5"),N(5));
#	}
#  if (!nonliteral(N(3)))
#    {
#    L("tmp3") = N(3);
#	group(3,3,"_adv");
#	pncopyvars(L("tmp3"),N(3));
#	N("comparative",3) = 1;
#	}
#  if (!nonliteral(N(1)))
#    {
#    L("tmp1") = N(1);
#	group(1,1,"_adv");
#	pncopyvars(L("tmp1"),N(1));
#	}
#  single();
#@RULES	# 08/12/04 AM.
#_prep <-
#	no [s]
#	_xWHITE [s star]
#	_xWILD [one match=(less more)]
#	_xWHITE [s star]
#	than [s]
#	@@
#@POST
#  if (!nonliteral(N(3)))
#    {
#    L("tmp3") = N(3);
#	group(3,3,"_prep"); # subordinating conjunction, really.
#	pncopyvars(L("tmp3"),N(3));
#	}
#  if (!nonliteral(N(1)))
#    {
#    L("tmp1") = N(1);
#	group(1,1,"_adv");
#	pncopyvars(L("tmp1"),N(1));
#	N("comparative",1) = 1;
#	}
#  single();
#@RULES
#_prep <-
#	_xWILD [one match=(less more)]
#	_xWHITE [s star]
#	than [s]
#	@@

@RULES
_adj <-
	unaccounted [s]
	_xWHITE [s star]
	for [s]
	@@

@POST
  L("tmp3") = N(3);
  group(3,3,"_prep");
  pncopyvars(L("tmp3"),N(3));
  
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));

  group(1,3,"_prep");
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(due prior)]
	_xWHITE [s star]
	to [s]
	@@
#_prep <-
#	because [s]
#	_xWHITE [s star]
#	of [s]
#	@@

# as
# to
# as to
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_prep");
  pncopyvars(L("tmp2"),N(2));
  group(1,1,"_prep");
  pncopyvars(L("tmp1"),N(1));
  if (!N("mypos",1))
    chpos(N(1),"IN");
  if (!N("mypos",2))
    chpos(N(2),"TO");
@RULES
_xNIL <-
	as
	to
	@@

# according to
# owing to
@POST
  chpos(N(1),"VBG");	# Don't care.
  chpos(N(3),"TO");	# Don't care.
  S("verbal") = 1;	# A "verbal" preposition. # 06/08/06 AM.
  sclearpos(1,0);
  single();
@RULES
_prep <-
	_xWILD [s one match=(according owing)]
	_xWHITE [s star]
	to [s]
	@@

@RULES
_prep <-
	in [s]
	_xWHITE [s star]
	_xWILD [s one match=(keeping accordance)]
	_xWHITE [s star]
	with [s]
	@@

@RULES
_adv <-
	_xWILD [s one match=(as when where if)]
	_xWHITE [s star]
	appropriate [s] @@

@POST
  pncopyvars(1);
  S("mypos") = "RB";	# Conformity (955/336 RB/JJ).
  single();
@RULES
_adv <-
	ago
	@@

@POST
  chpos(N(1),"IN");
  chpos(N(3),"NN");
  chpos(N(5),"TO");
  single();
@RULES
_prep <-
	in [s] _xWHITE [s star] addition [s] _xWHITE [s star] to [s]
	@@

# such det
@POST
  L("tmp1") = N(1);
  group(1,1,"_det");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"PDT");
@RULES
_xNIL <- # 79
	such
	_xWHITE [star]
	_xWILD [s one lookahead match=(a an)]
	@@

@POST
  chpos(N(3),"IN");
  chpos(N(1),"JJ");	# Penn Treebank conform.
  sclearpos(1,0);
  single();
@RULES
_prep <- such _xWHITE [s star] as @@

@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"JJ");	# Penn Treebank conform.
@RULES
_xNIL <-
	such
	@@

@RULES
# "for up to 2 days"
_prep <- for [s] _xWHITE [s star] up [s] _xWHITE [s star] to [s] @@

@POST
  chpos(N(3),"IN");
  chpos(N(1),"IN");
  sclearpos(1,0);
  single();

@RULES
_prep <- except [s] _xWHITE [s star] for [s] @@

@POST
  chpos(N(5),"IN");
  chpos(N(3),"NN");
  chpos(N(1),"IN");
  sclearpos(1,0);
  single();

@RULES
_prep <- by [s] _xWHITE [s star] means [s] _xWHITE [s star] of [s] @@
_prep <- in [s] _xWHITE [s star] comparison [s] _xWHITE [s star] with [s] @@
_prep <- in [s] _xWHITE [s star] front [s] _xWHITE [s star] of [s] @@
_prep <- in [s] _xWHITE [s star] relation [s] _xWHITE [s star] to [s] @@
_prep <- on [s] _xWHITE [s star] top [s] _xWHITE [s star] of [s] @@

@POST
  chpos(N(1),"IN");
  chpos(N(3),"IN");
  chpos(N(7),"IN");
  if (N(5))
    {
	chpos(N(5),"RB");
	S("neg") = 1;
	}
  sclearpos(1,0);
  single();

@RULES
_prep <-
	so [s]
	_xWHITE [s star]
	as [s]
	_xWHITE [s star]
	not [s opt]
	_xWHITE [s star]
	to [s]
	@@
# Need to defer this, for prep/phrasal verbs, etc.
@POST
  chpos(N(1),"IN");
  chpos(N(3),"IN");
  S("text") = strtolower(N("$text",1) + " " + N("$text",3));
  single();
@RULES
_prep <- along [s] _xWHITE [s star] with [s] @@
_prep <- as [s] _xWHITE [s star] for [s] @@
_prep <- as [s] _xWHITE [s star] of [s] @@
_prep <- out [s] _xWHITE [s star] of [s] @@

#@POST
#  chpos(N(1),"IN");
#  chpos(N(3),"TO");
#  single();
#@RULES
#_prep <- up [s] _xWHITE [s star] to [s] @@

@POST
  chpos(N(1),"RB");
  chpos(N(3),"IN");
  single();
@RULES
_prep <- away [s] _xWHITE [s star] from [s] @@

# Note: should treat these uniformly as single unit and pos.
# so far as (to)
# so far
@POST
  chpos(N(1),"RB");	# Corpus based.
  chpos(N(3),"RB");
  if (N(5))
    {
	chpos(N(5),"IN");
    S("pattern") = "so-far-as";
	}
  else
	S("pattern") = "so-far";
  S("ignorepos") = 1;
  single();
@RULES
_advl <-
	so [s]
	_xWHITE [s star]
	far [s]
	_xWHITE [s star]
	as [s]
	@@
_advl <-
	so [s]
	_xWHITE [s star]
	far [s]
	@@

@POST
  pncopyvars();
  S("mypos") = "IN";	# Treebank 94%.	# 04/23/07 AM.
  single();
@RULES
_fnword <- because @@

@POST
  pncopyvars();
  S("mypos") = "IN";
  single();
@RULES
_fnword <- _xWILD [one match=(
	since
	)] @@
@POST
  pncopyvars();
  S("mypos") = "RB";
  single();
@RULES
_fnword <- _xWILD [one match=(
	ever
	)] @@

@POST
  pncopyvars();
  single();
@RULES

_fnword <- _xWILD [one match=(
#	past	# prep, adj, noun ...	# 01/15/05 AM.
	before
	outside # highly ambig, could be prep also. # 10/20/04 AM.
	)]
	@@

@POST
  ++X("func words");
  pncopyvars();
  S("stem") = strtolower(N("$text"));
  if (S("stem") == "without")
    S("neg") = 1;
  single();
@RULES
_prep <- _xWILD [one match=(
	about above after along around
#	as	# 08/01/02 AM.
	at
#	before
	below beside between
	besides	# 04/22/07 AM.
	beyond
	by
	down
	during # acts like prep.
	for from
	in into
	near
	of off on over
	out	# adv etc.
#	past
#	since # fnword....
	till through to
	throughout
	under until up
	underneath
	with without within
	despite
	per upon
	)] @@
# worth can be a prep.	# 10/20/04 AM.

# Special preps.
_prep <- _xWILD [s one match=(
	regarding concerning)] @@

_fnword <- as @@

_conj <- _xWILD [s one match=(and or nor)] @@
_conj <- \& @@

_fnword <- _xWILD [s one match=(
	that
	so
	either neither
	)] @@

# Misc function words.
@POST
  pncopyvars(1);
  S("mypos") = "IN";
  single();
@RULES
_fnword <- _xWILD [s one match=(
	whatever wherever
    if than
	whether
	thus
	although though
	whereupon whereas
	)] @@

_fnword [layer=_adv] <- therefore [s] @@

@POST
  pncopyvars(1);
  S("mypos") = "WRB";
  single();
@RULES
_fnword <- _xWILD [s one match=(
	whenever
	)] @@

@POST
  pncopyvars(1);
  single();
@RULES
# Conjunction type thingy.
_conj <- but @@


@POST
  S("mypos",1) = "JJ";
  S("mypos",3) = "IN";
  single();
@RULES

# Other is tricky too.
# _adj [layer=(_fnword)] <- other @@
_fnword <- other [s] _xWHITE [s star] than [s] @@

@POST
  pncopyvars();
  single();
@RULES
_adj <- other @@

#_fnword [layer=(_adj _adv)] <-
#	only @@	# adj, adv, who knows what else.
	
_fnword [layer=(_adv)] <- _xWILD [s one match=(
	however)] @@

_whword [layer=(_fnword)] <- _xWILD [s one match=(
	what
	)] @@

@POST
  pncopyvars();
  S("mypos") = "WRB";
  single();
@RULES
_whword [layer=(_fnword)] <- _xWILD [s one match=(
	how why
	)] @@

# many
@POST
  chpos(N(1),"JJ");	# Conformity.
  N("number") = "plural";
@RULES
_xNIL <- many @@

@POST
  pncopyvars();
  S("mypos") = "WRB";
  single();
@RULES
_whword [layer=(_fnword)] <- when @@

@POST
  pncopyvars();
  S("mypos") = "WRB";
  single();
@RULES
_whword [layer=(_fnword)] <- 
	where
	@@

@POST
  pncopyvars();
  S("mypos") = "WDT";
  S("stem") = "which";
  S("bracket") = 1;
  single();
@RULES
_whword [layer=(_fnword)] <-
	which [s]
	@@

@POST
  pncopyvars();
  S("mypos") = "WP";
  single();
@RULES
_whword [layer=(_proSubj _proObj _pro _fnword)] <- who @@
_whword [layer=( _proObj _pro _fnword)] <- whom @@

@POST
  pncopyvars();
  S("temporal") = 1;
  S("unit") = "dy";
  S("sem") = "date";
  single();
@RULES
_day [layer=(_time _noun)] <-
	_xWILD [s one match=(day days)]
	@@
