###############################################
# FILE: dictfix.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 10/Jul/04 17:06:24
# NOTES:
#	once = adv, adj, noun, conj
#	  adj "the once and future king" - Karin.
###############################################

@CODE
L("hello") = 0;

# Remove spurious words from the dictionary.
L("c") = dictfindword("nondurables");
if (L("c"))
  rmconcept(L("c"));
	
@@CODE

@NODES _TEXTZONE

#####
## CORRECTIONS TO DICTIONARY HERE.
#####

@POST
  N("pos num") = 2;
  N("noun") = 1;		# MISSING.
  N("verb") = 1;
  N("stem") = "affiliate";
  N("pos") = "_noun";
@RULES
_xNIL <- affiliate @@
_xNIL <- affiliates @@

# bar - listed as preposition, which is ok, but
# extremely rare.
@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;
  N("stem") = "bar";
  N("pos") = "_noun";
@RULES
_xNIL <- bar @@

@POST
  N("pos num") = 2;
  N("adj") = 1;		# MISSING.
  N("noun") = 1;
  N("stem") = "giant";
  N("pos") = "_noun";
@RULES
_xNIL <- giant @@

# own - pronoun pos useless (Conform treebank and otherwise).
# Only adj and verb.
@POST
  N("adj") = 1;
  N("verb") = 1;
  N("sem") = N("stem") = "own";
  N("pos num") = 2;
  N("pos") = "_adj";
@RULES
_xNIL <- own @@

# even - getting rid of rare/archaic noun sense. (evening).
@POST
  N("adv") = 97;	# Conform treebank.
  N("adj") = 1;
  N("verb") = 1;
  N("noun") = 0;  # Effectively zero except in poetry.
  N("sem") = N("stem") = "even";
  N("pos num") = 3;
  N("pos") = "_adv";
@RULES
_xNIL <- even @@

# disparate - Noun sense looks erroneous.
@POST
  N("adj") = 1;
  N("noun") = 0;
  N("sem") = N("stem") = "disparate";
  N("pos num") = 1;
  N("pos") = "_adj";
@RULES
_xNIL <- disparate @@

# goes - Erroneously listed as noun.
@POST
  N("verb") = 1;
  N("noun") = 0;
  N("sem") = N("stem") = "go";
  N("pos num") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- goes @@

# Note: exclusively in "single-handedly".
@POST
  N("adv") = 1;
  N("sem") = N("stem") = "handed";
  N("pos num") = 1;
  N("pos") = "_adv";
@RULES
_xNIL <- handedly @@

# protecting
# dict lists as noun, adj.
@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("sem") = N("stem") = "protect";
  N("-ing") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- protecting @@

# provided - special word.
# dict lists as conj. (handle in grammar).
@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("sem") = N("stem") = "provided";
  N("-edn") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- provided @@

# providing - special word.
# dict lists as conj. (handle in grammar).
@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;	# 10/09/06 AM.
  N("sem") = N("stem") = "provide";
  N("-ing") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- providing @@

# pending listed as prep, which messes up lookup.
# (MW lists it as prep.)
# pending is a special word.  Treebank lists it as VBG
# usually, which is wrong.
@POST
  N("pos num") = 2;
  N("adj") = 1;
  N("verb") = 1; # WRONG. Conforming to Treebank.
  N("stem") = N("sem") = "pending"; # !
  N("pos") = "_adj";
  chpos(N(1),"VBG"); # WRONG. Conforming to Treebank.
@RULES
_xNIL <- pending @@

# failing listed as prep, which messes up lookup.
@POST
  N("pos num") = 3;
  N("adj") = 1;
  N("verb") = 1;
  N("noun") = 1;
  N("stem") = N("sem") = "fail";  # AMBIG, could have failings...
  N("pos") = "_verb";
  N("pos_np") = "VBG";
@RULES
_xNIL <- failing @@ # 10

# directly as conj is chiefly British, not quite a conj.
@POST
  N("pos num") = 1;
  N("adv") = 1;
  N("stem") = N("sem") = "direct";
  N("pos") = "_adv";
@RULES
_xNIL <- directly @@

# 'plus' is a prep.
# plus

@POST
  N("pos num") = 5;
  N("noun") = 1;
  N("conj") = 1;
  N("adj") = 1;
  N("adv") = 1;
  N("prep") = 1;
  N("stem") = "plus";
  N("pos") = "_adj";
@RULES
_xNIL <- plus @@

# 'plus' is a prep.

# past
@POST
  N("pos num") = 4;
  N("noun") = 24;
  N("adj") = 71;	# Conform Treebank.
  N("adv") = 1;
  N("prep") = 4;
  N("stem") = "past";
  N("pos") = "_adj";
  N("sem") = "date";	# 07/13/06 AM.
  ++X("date ref");
  ++X("date=past");
@RULES
_xNIL <- past @@

# recessionary listed only as noun.
@POST
  N("pos num") = 1;
  N("adj") = 1;
  N("stem") = N("sem") = "recession";
  N("pos") = "_adj";
@RULES
_xNIL <- recessionary @@


@POST
  N("pos num") = 5;
  N("noun") = 1;
  N("verb") = 1;
  N("adj") = 1;
  N("adv") = 1;
  N("prep") = 1;
  N("stem") = "round";
  N("pos") = "_adj";
@RULES
_xNIL <- round @@


@POST
  N("pos num") = 2;
  N("adj") = 86;	# Conform Treebank.
  N("adv") = 14;
#  N("prep") = 5;
  N("stem") = "next";
  N("pos") = "_adj";
@RULES
_xNIL <- next @@

# 'go' missing from dict!
@POST
  N("pos num") = 3;
  N("verb") = 95;	# Putting in some preferences!
  N("noun") = 4;
  N("adj") = 1;
  N("stem") = "go";
  N("pos") = "_verb";
@RULES
_xNIL <- go @@

# 'offer' listed as adj in dict, but can't corroborate.
@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;
  N("stem") = "offer";
  N("pos") = "_noun";
@RULES
_xNIL <- offer @@

# used
@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("adj") = 1;
  N("sem") = N("stem") = "use";
  N("-edn") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- used @@

# 'seek' listed as noun in dict, but can't corroborate.
@POST
  N("pos num") = 1;
  N("verb") = 100;
  N("stem") = "seek";
  N("pos") = "_verb";
@RULES
_xNIL <- seek @@
_xNIL <- seeks @@

# resolving listed as noun, can't corroborate.
@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("stem") = "resolve";
  N("pos") = "_verb";
@RULES
_xNIL <- resolving @@ # 20

# listed as noun, can't corroborate.
@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("stem") = "comprise";
  N("pos") = "_verb";
@RULES
_xNIL <- comprising @@

@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("adv") = 1;		# MISSING.
  N("stem") = "please";
  N("pos") = "_verb";
@RULES
_xNIL <- please @@

# To avoid mis-stemming.
@POST
  N("pos num") = 1;
  N("noun") = 1;
  N("stem") = "thing";
  N("sem") = "thing";
  N("pos") = "_noun";
@RULES
_xNIL <- thing @@

@POST
  N("pos num") = 1;
  N("adj") = 1;
  N("stem") = "unirradiated";
  N("pos") = "_adj";
@RULES
_xNIL <- unirradiated @@

# Consecutive is ONLY an adj.
@POST
  N("pos num") = 1;
  N("adj") = 1;
  N("stem") = "consecutive";
  N("pos") = "_adj";
@RULES
_xNIL <- consecutive @@

@POST
  N("pos num") = 2;
  N("adj") = 1;		# MISSING.
  N("noun") = 1;
  N("stem") = "miotic";
  N("pos") = "_adj";
@RULES
_xNIL <- miotic @@

@POST
# BUG: "do" is listed only as a noun in the dictionary!
  N("pos num") = 2;
  N("verb") = 1;	# MISSING.
  N("noun") = 1;
  N("stem") = "do";
  N("pos") = "_verb";
@RULES
_xNIL <- do @@

@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;	# MISSING.
  N("stem") = "whale";
  N("pos") = "_noun";
@RULES
_xNIL <- whaling @@

@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;
  N("stem") = "reprocess";
  N("pos") = "_noun";
@RULES
_xNIL <- reprocessing @@

@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;
  N("stem") = "restructure";
  N("eventive") = 1;
  N("pos") = "_noun";
  N("pos_np") = "NN";
@RULES
_xNIL <- restructuring @@

@POST
  N("pos num") = 1;
  N("noun") = 1;
  N("stem") = "pc";
  N("sem") = "pc";
  N("pos") = "_noun";
  N("number") = "singular";
  N("mypos") = "NN";
  N("acronym") = 1;
@RULES
_xNIL <- pc @@

@POST
  N("pos num") = 1;
  N("noun") = 1;
  N("stem") = "pc";
  N("sem") = "pc";
  N("pos") = "_noun";
  N("number") = "plural";
  N("mypos") = "NNS";
  N("acronym") = 1;
@RULES
_xNIL <- pcs @@

@POST
  S("pos num") = 2;
  S("verb") = 1;
  S("noun") = 1;
  S("text") = "cochair";
  S("stem") = "cochair";
  S("pos") = "_noun";
  single();
@RULES
cochair <- # 30
	co [s]
	\- [s]
	chair [s]
	@@

@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;
  N("stem") = "cochair";
  N("pos") = "_noun";
@RULES
_xNIL <- cochair @@

# Stemmer incorrectly does improving => improv!
@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("adj") = 1;
  N("stem") = "improve";
  N("pos") = "_verb";
@RULES
_xNIL <- improving @@
_xNIL <- improved @@

@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;	# Add this definition.
  N("stem") = "compute";
  N("eventive") = 1;
  N("pos") = "_noun";
  N("pos_np") = "NN";
@RULES
_xNIL <- computing @@	# 04/23/07 AM.

# Stemmer incorrectly does pointed => pointe
@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("adj") = 1;
  N("stem") = "point";
  N("pos") = "_verb";
@RULES
_xNIL <- pointed @@

@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("stem") = "locate";
  N("pos") = "_verb";
@RULES
_xNIL <- locating @@

# Dict had this as adj also.	# 9/18/04 AM.
@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("stem") = "hear";
  N("pos") = "_verb";
  N("-edn") = 1;
@RULES
_xNIL <- heard @@

@POST
  N("pos num") = 2;
  N("verb") = 50;
  N("noun") = 50;
  N("stem") = "interest";
  N("pos") = "_noun";
@RULES
_xNIL <- interest @@	# 07/17/03 AM.

@POST
  N("pos num") = 2;
  N("verb") = 1;
  N("noun") = 1;
@RULES
_xNIL <- sideline @@
_xNIL <- sidelines @@ # 40

@RULES
_verb <- sidelining @@
_verb <- sidelined @@

@POST
  N("pos num") = 1; # Dict has verb also.
  N("noun") = 1;
  N("stem") = "aide";
  pncopyvars();
  single();
@RULES
_noun <- aides @@

# modest is not superlative!
@POST
  N("stem") = N("sem") = "modest";
  N("pos num") = 1;
  N("adj") = 1;
  chpos(N(1),"JJ");
  pncopyvars();
  single();
@RULES
_adj <- modest @@

@POST
  N("pos num") = 1;
  N("noun") = 1;
  N("stem") = "man";
  N("number") = "plural";
  chpos(N(1),"NNS");
  pncopyvars();
  single();
@RULES
_noun <- men @@

@POST
  N("pos num") = 1;
  N("noun") = 1;
  N("stem") = "penny";
  N("number") = "plural";
  chpos(N(1),"NNS");
  pncopyvars();
  single();
@RULES
_noun <- pence @@	# 46

@POST
  N("pos num") = 1;
  N("noun") = 1;
  N("stem") = "known";
  N("number") = "plural";
  chpos(N(1),"NNS");
  pncopyvars();
  single();
@RULES
_noun <- knowns @@

# news is singular only.
@POST
  N("pos num") = 1;
  N("noun") = 1;
  N("stem") = "news";
  N("number") = "singular";
#  chpos(N(1),"NN");
@RULES
_noun <- news @@

# noun in dict is spurious.
@POST
  N("pos num") = 1;
  N("adj") = 1;
  pncopyvars();
  single();
@RULES
_adj <-
	nondurable
	@@

@POST
  N("pos num") = 1;
  N("adj") = 1;
  pncopyvars();
  single();
@RULES
_adj <-
	nonexecutive
	@@

# listed as conj in dictionary.
@POST
  N("pos num") = 1;
  N("adv") = 1;
  pncopyvars();
  single();
@RULES
_adv <- immediately @@

@POST
  # Don't want this to automatically go to preposition.
  N("prep") = 0;
  pncopyvars();
  single();
@RULES
_noun <- vice @@

# Setting a preference for noun/adj/vbg, eg,
# if this ends up as non head noun in noun phrase.
# 10/16/04 AM.
@POST
  N("pos_np") = "JJ";
@RULES
_xNIL <- _xWILD [one match=(
	accelerated
	annual
	blue	# Colors...
	common
	composite
	computerized
	confused
	differential
	disaffected
	disembodied
	distorted
	disturbing
	domestic
	executive	# About 665/510 !
	facial
	first
	free
	further	# Treebank 329/356.
	general
	good
	human
	initial
	intellectual
	last
	lead	# ambig.
	local
	matching
	major
	member	#
	monthly
	open
	past
	personal
	private
	senior		# 06/04/06 AM.
	shaded
	simple
	specific
	standardized
	standing
	striking
	troubled
	worthy
	)] @@
@POST
  N("pos_np") = "JJS";
@RULES
_xNIL <- _xWILD [one match=(
	best
	)] @@
@POST
  N("pos_np") = "JJR";
@RULES
_xNIL <- _xWILD [one match=(
	lesser
	lower
	)] @@

@POST
  N("pos_np") = "NNS";
  N("-s") = 1;
@RULES
_xNIL <- _xWILD [one match=(
	alumni
	data
	)] @@

@POST
  N("pos_np") = "NN";
@RULES
_xNIL <- _xWILD [one match=(
	adult
	attack
	baby
	bond
	business
	capital
	cardboard
	centennial
	cheerleading	# new noun.
	compound
	copyright
	country
	crack	# usually.
	desktop
	district
	engineering
	guest
	holding
	home
	household
	managing
	manufacturing
	market
	material
	mathematics	# want singular...
	morning
	motor
	operating
	patent
 	power
	radio
	rate
	state
	stock
	summer
	test
	textile
	trade
	trading
	vice
	winter
	work
	world
	)] @@

# See treebank tagguide for vbg/jj, vbn/jj.
@POST
  N("pos_np") = "VBN";	# 04/20/07 AM.
@RULES
_xNIL <- _xWILD [one match=(
 	accused
 	announced
	expected
	imported
	)] @@

@POST
  N("pos_np") = "VBG";
@RULES
_xNIL <- _xWILD [one match=(
	amusing
	building
	competing
	developing
	failing
	growing		# 04/23/07 AM.
	remaining	# 04/20/07 AM.
	receiving
	slowing
	)] @@

@POST
  N("pos num") = 3;
  N("noun") = 1;
  N("adj") = 1;
#  N("sem") = "direction";
  N("stem") = strtolower(N("$text"));
#  if (N("$length") == 1)
#    N("adv") = 50;	# confidence.
#  else
#    N("adv") = 95;	# Ballworld domain.
  N("adv") = 1;
@RULES
_xNIL <- _xWILD [one match=(
	n s e w
	ne se nw sw
	nne ene sse ese nnw wnw ssw wsw
	)] @@
