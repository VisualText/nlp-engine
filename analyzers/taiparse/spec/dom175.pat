###############################################
# FILE: dom175.pat
# SUBJ: Capitalized phrases.
# AUTH: AM
# CREATED: 23/May/05 11:50:01
# MODIFIED:
###############################################

@CODE
G("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@CHECK
  if (!N("ne type",1))
    fail();
@POST
  L("tmp2") = N(2);
  if (!N(3))
    L("one") = 1;
  group(2,3,"_caps");
  group(2,2,"_noun");
  if (L("one"))
    pncopyvars(L("tmp2"),N(2));
  N("sem",2) = "name";
  N("ne",2) = 1;
  N("ne type",2) = N("ne type",1);
  N("stem",2) = N("$text",2);
  N("posarr",2) = pnvar(L("tmp2"),"posarr");	# 06/14/06 AM.
  N("posarr len",2) = pnvar(L("tmp2"),"posarr len");
  chpos(N(2),"NP");	# 06/14/06 AM.
  if (N("ne type",1) == "person")
    # Some name semantics here.
    registerpersnames(N("$text",2));
@RULES
_xNIL <-
	_noun
	_xCAP
	_xWILD [star match=(_xWHITE _xCAP _letabbr)]
	@@

# Title capitalization...
@POST
  group(2,4,"_caps");
  group(2,2,"_noun");
  N("ne",2) = 1;
  N("ne type",2) = "title";
@RULES
_xNIL <-
	\"
	_xWILD [s plus match=(_xCAP _det _prep _fnword)]
	_xCAP [s trigger]
	_xWILD [s star match=(_xCAP _det _prep _fnword)]
	_qEOS [opt]
	\"
	@@

# Semi-capitalized phrase...
# dqan cap noun 's
#@PRE
#<3,3> lowercase();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"NP");

  group(2,3,"_caps");
  group(2,2,"_noun");
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro _adj _prep)]
	_xCAP
	_noun
	_aposS [lookahead]
	@@


# neither cap nor
# neither alpha nor
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(1),"CC");	# neither/CC.
  chpos(N(3),"CC"); # nor/CC.
@RULES
_xNIL <-
	neither [s]
	_xCAP
	nor [s]
	@@

# Miscellany.

# ^ "
@POST
  ++X("dblquote");
  group(2,2,"_qEOS");
@RULES
_xNIL <-
	_xSTART
	_dblquote
	@@

# , "
@POST
  ++X("dblquote");
  group(1,2,"_qEOS");
@RULES
_xNIL <-
	\,
	_dblquote
	@@

# Hard-wired or "lexicalized" for now.
@PRE
<1,1> cap();
@CHECK
  if (N("mypos",1))
    fail();
@POST
  if (G("conform treebank"))
    chpos(N(1),"NP");
@RULES
_xNIL <-
	_xWILD [s one match=(
		American
		British
		Congress
		Continental
		Economic
		Environmental
		Mercantile
		Data
		Digital
		Eastern
		Financial
		Ford
		Foreign General Great
		Industrial
		Jaguar
		Japanese
		Minister
		Monetary
		Moody
		Paramount
		Philippines
		Poor President Prime
		SEC
		Secretary
		Southern
		State
		Supreme
		TV
		Warner

		Airlines Americans
		Associates
		Brothers
		Communications
		Containers
		Futures
		Gardens
		Holdings
		Industries
		Investors
		Machines
		Manufacturers
		Markets
		Motors
		Resources
		Savings
		Securities
		Services
		Stores
		Systems
		)]
	_xCAP [s lookahead]
	@@

@PRE
<2,2> cap();
@CHECK
  if (N("mypos",2))
    fail();
@POST
  if (G("conform treebank"))
    chpos(N(2),"NP");
@RULES
_xNIL <- # 8
	_xCAP [s]
	_xWILD [s one lookahead match=(
		American
		British
		Congress
		Continental
		Economic
		Environmental
		Mercantile
		Data
		Digital
		Eastern
		Financial
		Ford
		Foreign General Great
		Industrial
		Jaguar
		Japanese
		Minister
		Monetary
		Moody
		Paramount
		Philippines
		Poor President Prime
		SEC
		Secretary
		Southern
		State
		Supreme
		TV
		Warner

		Airlines Americans
		Associates
		Brothers
		Communications
		Containers
		Futures
		Gardens
		Holdings
		Industries
		Investors
		Machines
		Manufacturers
		Markets
		Motors
		Resources
		Savings
		Securities
		Services
		Stores
		Systems
		)]
	@@

@POST
  if (G("conform treebank"))
    N("mypos",1);
  chpos(N(2),"NP");
  if (N(3))
    chpos(N(3),"NP");
  group(1,3,"_noun");
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	American [s]
	Stock [s]
	Exchange [s]
	@@

@PRE
<1,1> cap();
@POST
  pnrename(N(1),"_noun");
  chpos(N(1),"NP");
  if (N(2))
    chpos(N(2),"NP");
  if (N(3))
    {
	if (G("conform treebank"))
	  chpos(N(3),"NP");
	else
	  chpos(N(3),"JJ");
	}
  if (N(4))
    chpos(N(4),"NP");
  group(1,4,"_noun");
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	Dow [s]
	Jones [s opt]
	Industrial [s opt]
	Average [s opt]
	@@

# money
# yen
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  if (N("num",1) != 1)	# NUMERIC VALUE of number.
    chpos(N(2),"NNS");
  else
    chpos(N(2),"NN");
@RULES
_xNIL <-
	_xWILD [one match=(_num _quan _xNUM)]
	yen
	@@

# idiom: data base
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	data [s]
	_xWILD [one match=(base bases)]
	@@

# much noun
@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  if (!N("mypos",1))
    chpos(N(1),"JJ");
@RULES
_xNIL <-
	much
	_xWILD [one lookahead match=(_noun)]
	@@

# much adv
@POST
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
  if (!N("mypos",1))
    chpos(N(1),"RB");
@RULES
_xNIL <-
	much
	_xWILD [one lookahead match=(_adv)]
	@@

# savings and loan
@POST
  if (!N("mypos",3))
    chpos(N(3),"NN");
  group(1,3,"_noun");
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	savings [s]
	and [s]
	loan [s]
	@@

# Standalone cap words are not necessarily NP.

@POST
  if (!N("mypos",2))
    chpos(N(2),"NN");
@RULES
_xNIL <-
	_xWILD [s one fail=(_xCAP)]
	_xWILD [s one match=(
		tv
		)]
	_xWILD [s one lookahead fail=(_xCAP)]
	@@

# adj 's
# Fix some bad assignments.
@POST
  pnrename(N(1),"_noun");
@RULES
_xNIL <-
	_adj
	_aposS [lookahead]
	@@

# someone else
# noun else
@PRE
<1,1> var("nounpro");
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"RB");	# Conform treebank.
@RULES
_xNIL <-
	_noun
	else
	@@

# which means
# that means
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(that which)]
	means
	@@

# money num
@POST
  group(1,2,"_num");
  N("number") = "any";
@RULES
_xNIL <-
	_money # eg, US$
	_num
	@@

# human being
# idiom. (not foolproof, of course.)
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  N("sem",2) = N("stem",2) = "being";
  chpos(N(2),"NN");	# being/NN.
@RULES
_xNIL <-
	human [s]
	being
	@@

# being
# DEFAULT (96% in Penn Treebank.)
@POST
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
  N("sem",1) = N("stem",1) = "be";
  chpos(N(1),"VBG");	# being/VBG.
@RULES
_xNIL <-
	being
	@@

# look like
# sound like
@POST
  alphatoverb(1,"active","VBD");
  if (!N("mypos",2))
    {
	chpos(N(2),"IN");	# like/IN.
	pnrename(N(2),"_prep");
	}
@RULES
_xNIL <-
	_xWILD [one match=(
		look sound
		looks sounds
		looking sounding
		looked sounded
		seem seems seeming seemed
		feel feels feeling felt
		)]
	like [s lookahead]
	@@

# like pro
@CHECK
  if (N("mypos",1))
    fail();
  if (pnname(N(1)) != "_fnword")
    fail();
@POST
  chpos(N(1),"IN");	# like/IN.
  pnrename(N(1),"_prep");
@RULES
_xNIL <-
	_xWILD [s one match=(like)]
	_proSubj [s lookahead]
	@@

# like to
@CHECK
  if (N("mypos",1))
    fail();
  if (pnname(N(1)) != "_fnword")
    fail();
@POST
  pnrename(N(1),"_verb");
@RULES
_xNIL <-
	like [s]
	_adv [star lookahead]
	to [s]
	@@

# det num
@CHECK
  N("num") = num(N("$text",2));
  if (N("num") > 1950 && N("num") < 2015)
    succeed();
  fail();
@POST
  group(2,2,"_adj");
  chpos(N(2),"CD");
@RULES
_xNIL <-
	_det
	_xNUM
	@@

# num th street
@PRE
<1,1> var("ordinal");
<2,2> var("cap");
@POST
  chpos(N(1),"NP");
  chpos(N(2),"NP");
@RULES
_xNIL <-
	_num
	_xWILD [s one match=(street avenue boulevard)]
	@@

