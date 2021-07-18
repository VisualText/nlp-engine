###############################################
# FILE: caps50.pat
# SUBJ: Capitalized phrases.
# AUTH: AM
# CREATED: 07/Aug/04 21:22:35
# NOTE:	Assuming we're in a mixed case prose region.
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE

# ^ cap
@PRE
<3,3> cap();
@CHECK
  if (N("quoted",3))
  	{
	# Probably a caps region...
	if (X("quoted caps") > X("quoted prose"))
		fail();
	}
@POST
  N("newsent",3) = 1;
  noop();
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_xPUNCT _qEOS _dbldash)]
	_xWILD [one match=(_det _pro _prep _conj _fnword)]
	@@

# Defer decisions on start of sentence.
@POST
  N("newsent",2) = 1;
  if (N("unknown",2))
  	fixnphead(2);
  else
    noop();
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_prep
	@@
_xNIL <-
	_qEOS
	_xALPHA
	_prep
	@@
# Interrogative forms...
_xNIL <-
	_xSTART
	_verb
	I [s]
	@@

# ^ alpha
@CHECK
  if (N("verb",2) && !vconjq(N(2),"inf"))
	succeed();	# Inflected verb.
  fail();
@POST
  N("newsent") = 1;
  noop();	# Hide from cap glomming.
@RULES
_xNIL <-
	_xSTART
	_xCAP
	@@

# Some chaff.
# Four Americans ...
@POST
  noop();
@RULES
_xNIL <-
	_xWILD [plus match=(_num _quan)]
	_adj
	@@

# Don't know what to do with "I" yet.
@PRE
<1,1> vareq("stem","i");
@POST
  noop();
@RULES
_xNIL <-
	_pro
	@@

# ^ cap date
@CHECK
  if (N("sem",3) != "date"
	 && !N("acronym",3))	# 04/23/07 AM.
    fail();
@POST
  N("newsent",2) = 1;
  noop();	# Don't throw proper noun on first guy.
@RULES
_xNIL <-
	_xSTART
	_xCAP
	_noun
	@@

# cap prep cap
@POST
  cappos(N(1),0);
  cappos(N(3),0);
@RULES
_xNIL <-
	_xCAP [s]
	_prep [lookahead]
	_xCAP [s]
	@@

# Trying out the cool feature-based matching.	# 06/26/06 AM.
@PRE
<1,1> lowercase();
<2,2> var("cap");
@POST
#  chpos(N(3),"POS");	# Don't really know yet.
  L("fst") = N(2);
  L("lst") = lasteltnode(2);
  setposrange(L("fst"),L("lst"),"NP");
  group(2,2,"_caps");
  setunsealed(2,"true");	# 07/10/12 AM.
  group(2,2,"_noun");
  setunsealed(2,"true");	# 07/10/12 AM.
  N("ne",2) = 1;
#  group(2,3,"_adj");
#  clearpos(N(2),1,0);
#  N("possessive",2) = 1;
@RULES
_caps [layer=_noun] <-
	_xWILD [s one match=(_xALPHA _aposS \, _dbldash)]
	_xWILD [plus fail=(_xPUNCT _xNUM _qEOS)]
	_aposS
	@@


# Trying a more abstract method.	# 06/26/06 AM.
@PRE
<1,1> var("cap");
@POST
  L("fst") = N(1);
  L("lst") = lasteltnode(1);
  setposcaps(L("fst"),L("lst"));
  group(1,1,"_caps");
  setunsealed(1,"true");	# 07/10/12 AM.
  group(1,1,"_noun");
  setunsealed(1,"true");	# 07/10/12 AM.
  clearpos(N(1),1,0);
  N("cap",1) = 1;
  N("number",1) = number(L("lst"));
@RULES
_xNIL <-
	_xWILD [min=2 fail=(_xPUNCT _qEOS _xNUM)]
	@@

# cap cap apos
@POST
  N("cap",3) = 1;
  N("noun",3) = 1;
  L("tmp3") = N(3);
  group(3,3,"_noun");
  setunsealed(3,"true");	# 07/10/12 AM.
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xSTART
	_xCAP [s]
	_xCAP
	_aposS [lookahead]
	_xWILD [s one fail=(_xCAP)]
	@@

# city , state
@POST
  L("lst") = lasteltnode(2);
  setposrange(N(2),L("lst"),"NP");	# 05/03/05 AM.
  L("tmp4") = N(4);
  group(2,2,"_city");
  group(2,5,"_citystate");
  group(2,2,"_noun");
  pncopyvars(L("tmp4"),N(2));
  chpos(N(2),0);		# 05/03/05 AM.
  clearpos(N(2),1,0);
#  chpos(N(2),"NP");	# 05/03/05 AM.
  N("cap",2) = 1;
  N("glom comma",2) = 1; # Can glom comma to the right.
  N("sem",2) = "geoloc";	# 04/21/07 AM.
@RULES
_xNIL <-
	_xWILD [s opt fail=(_xCAP)]
	_xCAP [s plus except=(_det)]
	\,
	_usstate [s]
	\, [opt]
	@@


# Uknown word at start of sentence etc.
@CHECK
  if (!N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  N("cap",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_qEOS _conj)]
	_xCAP
	_xWILD [one lookahead match=( \, _verb _vg )]
	@@
_xNIL <-
	_xSTART
	_xCAP
	_xWILD [one lookahead match=( \, )]
	@@

@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  N("cap",2) = 1;
  N("ne",2) = 1;
  cappos(N(2),0);

  # Some domain semantics.
  L("lc") = strtolower(N("$text",2));
  if (finddictattr(L("lc"),"persname"))
    {
    N("ne type",2) = "person";
	N("sem",2) = "name";
    N("ne type conf",2) = 50;
	}
@RULES
_xNIL <-
	_xWILD [one match=( \, _conj )]
	_xCAP
	_xWILD [one lookahead match=( \, _verb _vg )]
	@@


# alpha num
# cap num
@CHECK
  if (!N("unknown",1))
    fail();
@POST
  chpos(N(1),"NP");
  group(1,2,"_noun");
  clearpos(N(1),1,0);
  N("cap",1) = 1;
  N("ne",1) = 1;
@RULES
_xNIL <- #10
	_xCAP
	_xNUM
	@@

	

# alpha cap alpha
@PRE
<1,1> lowercase();
<3,3> lowercase();
@CHECK
  if (N("adj",2) && N("noun",2))
    fail();	# 01/04/05 AM.
@POST
  L("tmp2") = N(2);
  if (N("noun",2) || !N("adj",2))
    {
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
 	cappos(N(2),0);
	}
  else
    {
    group(2,2,"_adj");
    pncopyvars(L("tmp2"),N(2));
	}
  N("cap",2) = 1;
  N("ne",2) = 1;
  N("stem",2) = N("$text",2);

  # Some domain semantics.
  L("lc") = strtolower(N("$text",2));
  if (finddictattr(L("lc"),"persname"))
    {
    N("ne type",2) = "person";
    N("ne type conf",2) = 50;
	N("sem",2) = "name";
	N("ne",2) = 1;
	}
  else if (N("unknown",2))
    {
	# A bit of morph.
	if (strendswith(L("lc"),"tv"))
	  {
	  N("ne type",2) = "organization";
      N("ne type conf",2) = 50;
	  N("sem",2) = "organization";
	  N("ne",2) = 1;
	  }
	}
@RULES
_xNIL <-
	_xALPHA [s]
	_xCAP
	_xALPHA [s lookahead]
	@@

# cap & cap
@POST
  L("tmp1") = lasteltnode(1);
  setposrange(N(1),L("tmp1"),"NP");
  L("tmp3") = lasteltnode(3);
  setposrange(N(3),L("tmp3"),"NP");
  pncopyvars(L("tmp3"));
  sclearpos(1,0);
  single();
@RULES
_caps [layer=_noun unsealed] <-
	_xCAP [plus]
	\& [s]
	_xCAP [plus]
	@@

@PRE
<1,1> lowercase();
@POST
  chpos(N(4),"POS");	# 5/24/06 AM.
  if (N(3))
    L("last") = lasteltnode(3);
  else
    L("last") = N(2);
  fixnouns(N(2),L("last"));
  group(2,3,"_caps");
  setunsealed(2,"true");	# 07/10/12 AM.
  group(2,2,"_noun");
  setunsealed(2,"true");	# 07/10/12 AM.
#  N("posarr",2) = "NP";	# 06/26/06 AM.
#  N("posarr len",2) = 1;	# 06/26/06 AM.
#  chpos(N(2),"NP");		# 06/26/06 AM.
  N("ne",2) = 1;
  group(2,3,"_adj");
  clearpos(N(2),1,0);
  N("possessive",2) = 1;
@RULES
_caps [layer=_noun] <-
	_xWILD [s one match=(_xALPHA _xSTART _aposS \, _dbldash)]
	_xCAP
	_xWILD [s star match=(_xCAP _letabbr)]
	_aposS
	@@


@PRE
<1,1> lowercase();
@POST
  L("n") = N(2);
  L("posarr") = sameposrange(N(2),L("n"));
  if (!L("posarr"))
    "err.txt" << "[More than one POS in caps list.]\n";
  pncopyvars(L("n"));
  L("last") = lasteltnode(3);
  forceposrange(N(2),L("last"),"NP");	# 04/21/07 AM.
  group(2,3,"_caps");
  setunsealed(2,"true");	# 07/10/12 AM.
  group(2,2,"_noun");
  setunsealed(2,"true");	# 07/10/12 AM.
  clearpos(N(2),1,0);
  N("cap",2) = 1;
  N("sem",2) = "name";
  N("ne",2) = 1;
@RULES
#_caps [layer=_noun] <-
_xNIL <-
	_xWILD [s one match=(_xALPHA _aposS)]
	_xCAP [s]	# 06/21/06 AM.
	_xWILD [s plus match=(_xCAP _letabbr)]
	@@
_xNIL <-
	_xSTART
	_xCAP
	_xWILD [s plus match=(_xCAP _letabbr)]
	@@

@POST
  L("first") = N(1);
  L("last") = lasteltnode(2);
  L("sem") = pnvar(L("last"),"sem");
  if (!L("sem"))
    L("sem") = "name";
  setposrange(L("first"),L("last"),"NP");
  L("ne text") = phrasetext();
  group(1,2,"_caps");
  setunsealed(1,"true");	# 07/10/12 AM.
  group(1,1,"_noun");
  setunsealed(1,"true");	# 07/10/12 AM.
  clearpos(N(1),1,0);
  if (L("sem"))
  N("sem",1) = L("sem");
  N("stem",1) = phrasetext();
  N("ne text",1) = L("ne text");
  N("ne type",1) = "person";
  N("ne",1) = 1;
    N("ne type conf",1) = 50;
  N("cap",1) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_title _letabbr _letlet)]
	_xWILD [s plus match=(_xCAP _letabbr)]
	@@

# Two caps in a row, even if reduced.
@CHECK
  if (N("ne type",1))	# Don't glom known entities.
    fail();
@POST
  L("first") = N(1);
  L("last") = lasteltnode(2);
  L("sem") = pnvar(L("last"),"sem");
  if (!L("sem"))
    L("sem") = "name";
  setposrange(L("first"),L("last"),"NP");
  L("ne text") = phrasetext();
  L("caps arr")[0] = N("$text",1);
  L("caps arr")[1] = N("$text",2);
  group(1,2,"_caps");
  setunsealed(1,"true");	# 07/10/12 AM.
  group(1,1,"_noun");
  setunsealed(1,"true");	# 07/10/12 AM.
  clearpos(N(1),1,0);
  if (L("sem"))
  N("sem",1) = L("sem");
  N("ne text",1) = N("stem",1) = L("ne text");
  N("ne",1) = 1;
  N("cap",1) = 1;
  if (L("caps arr"))
    N("caps arr",1) = L("caps arr");
@RULES
_xNIL <-
	_xWILD [s one match=(_xCAP _title
		_letabbr _letlet _name _month _abbr _usstate)
		except=(_det _fnword _prep _conj _pro _verb _vg _modal)]
	_xWILD [s plus match=(_xCAP _letabbr)
		except=(_det _fnword _prep _conj _pro)]
	@@

@POST
  pncopyvars(1);
  S("mypos") = "NP";
  single();
@RULES
_caps [layer=_noun unsealed] <-
	_xCAP
	_xWILD [s plus match=(_xCAP _letabbr)]
	@@

@POST
  chpos(N(3),"POS");
  L("tmp2") = N(2);
  group(2,2,"_caps");
  setunsealed(2,"true");	# 07/10/12 AM.
  group(2,2,"_noun");
  setunsealed(2,"true");	# 07/10/12 AM.
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"NP");
@RULES
_xNIL <-
	_xWILD [s one fail=(_xCAP)]
	_xCAP
	_aposS
	@@

@PRE
<1,1> lowercase();
@POST
  chpos(N(2),"NP");
  chpos(N(3),"POS");
  pncopyvars(2);
  sclearpos(1,0);
  singler(2,3);
@RULES
_caps [layer=_noun unsealed] <-
	_xWILD [s one match=(_xALPHA _xSTART)]
	_xCAP
	_aposS
	@@

# cap apos eos
@POST
  chpos(N(2),"NP");
  chpos(N(3),"POS");
  pncopyvars(2);
  sclearpos(1,0);
  singler(2,3);
@RULES
_caps [layer=_noun unsealed] <-
	_xWILD [one match=(_prep _fnword _verb _vg)]
	_xCAP
	_aposS
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# Flag all caps.
@PRE
<1,1> varz("mypos");
#<1,1> varz("adj");	# 04/20/07 AM. # Commented 05/19/07 AM.
@POST
  N("caps50 lonecap") = 1;
  # Todo: Fun to assess start of new sentence here.
  L("newsent") = isnewsentence(N(1));
  L("tmp1") = N(1);				# 06/27/06 AM.
  if (!L("newsent"))
    {
# Try restricting this.	# 05/22/07 AM.
	if (literal(N(1)) && N("unknown",1))
	  {
	  group(1,1,"_noun");		# 06/27/06 AM.
      setunsealed(1,"true");	# 07/10/12 AM.
	  pncopyvars(L("tmp1"),N(1));	# 06/27/06 AM.
	  }
    N("cap") = 1;
	N("ne") = 1;
#	if (pnname(N(1)) == "_noun" || pnname(N(1)) == "_adj")
#	if (N("noun",1) || N("adj",1))	# 05/22/07 AM.
   	  cappos(N(1),0);	# 05/10/07 AM.
#	  fixnoun(N(1));
	}
  else # First word of text zone is cap.
    {
	N("newsent") = 1;
	if (literal(N(1)) && N("unknown"))	# 06/27/06 AM.
	  {
	  group(1,1,"_noun");			# 06/27/06 AM.
      setunsealed(1,"true");	# 07/10/12 AM.
	  pncopyvars(L("tmp1"),N(1));	# 06/27/06 AM.
	  N("cap") = 1;					# 06/27/06 AM.
	  fixnoun(N(1));				# 06/27/06 AM.
	  }
	else if (pnname(N(1)) == "_noun" && !N("mypos"))
	  {
	  # Only unknown words will be NP, for now...
	  if (N("unknown"))
	    {
		N("cap") = 1;
		N("mypos",1) = "NP";	# 05/26/07 AM.
		fixnoun(N(1));
		}
	  # Else could collect tentative noncount/mass nouns here.
        if (G("verbose"))
	    "mass-noun.txt" << N("$text") << "\n";
	  }
#	else if (literal(N(1)) && N("unknown") && !N("mypos"))
#	  {
	  # 5/27/06 AM.
#	  N("cap") = 1;
#	  }
	}
@RULES
_xNIL <- #22. 83%
	_xWILD [s one match=(_xCAP)
	# Want all caps within a sentence handled here. # 06/02/07 AM.
	except=(_pro _det _fnword _prep) # 06/02/07 AM.
	]
	@@

# Common words that are capitalized.
@POST
  N("cap") = 1;	# Flag as caps.	# 06/06/07 AM.
@RULES
_xNIL <-
	_xCAP [s]
	@@

# simple name pattern
@POST
  pncopyvars(2);
  S("mypos") = "NP";
  singler(1,2);
@RULES
_caps [layer=_noun unsealed] <-
	_letlet [s]
	_xCAP
	_xWILD [one lookahead fail=(_xCAP)]
	@@

# Lone letter abbreviation.
# Could be bad answer key.
@POST
  L("tmp1") = N(1);
  group(1,1,"_caps");
  setunsealed(1,"true");	# 07/10/12 AM.
  group(1,1,"_noun");
  setunsealed(1,"true");	# 07/10/12 AM.
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"NP");
@RULES
_xNIL <-
	_letabbr
	@@


# Hal 9000
# alpha num noun
@POST
  L("tmp2") = N(2);
  if (literal(N(2)))
    {
  group(2,2,"_caps");
  setunsealed(2,"true");	# 07/10/12 AM.
  group(2,2,"_noun");
  setunsealed(2,"true");	# 07/10/12 AM.
  pncopyvars(L("tmp2"),N(2));
    }
  chpos(N(2),"NP");
  group(2,4,"_noun");
  setunsealed(2,"true");	# 07/10/12 AM.
@RULES
_xNIL <-
	_xWILD [one fail=(_xCAP)]
	_xCAP [s]
	_xNUM
	_listitem [s opt]
	_xWILD [one lookahead match=(_noun _verb _vg)]
	@@

# by cap prep cap ,
@PRE
<1,1> vareq("stem","by");
@POST
  group(2,4,"_np");
  setunsealed(2,"true");	# 07/10/12 AM.
  N("cap",2) = 1;
  N("ne",2) = 1;
@RULES
_xNIL <-
	_prep
	_xCAP [s]
	_prep
	_xCAP [s]
	_xWILD [one lookahead match=( \, )]
	@@

