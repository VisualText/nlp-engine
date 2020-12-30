###############################################
# FILE: punct100.pat
# SUBJ: Text punctuation
# AUTH: AM
# CREATED: 05/Jul/04 20:57:17
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE

# Non- eos cases also.
@PRE
<3,3> lowercase();
@POST
  noop();
@RULES
_xNIL <-
	\.
	_xWHITE [star lookahead]
	_xALPHA
	@@

# Some abbreviatons.
@POST
  pncopyvars(1);
  S("mypos") = "NP";
  single();
@RULES
_companyDESIG [layer=(_abbr _noun)] <-
	_xWILD [one match=(inc co corp ltd llc
		bancorp)]
	\. [opt]
	@@
@POST
  S("mypos") = "NP";
  single();
@RULES
#n.v.
_companyDESIG <-
	n
	\. [opt]
	v
	\. [opt]
	@@

@POST
  S("cap") = 1;
  singler(1,2);
@RULES
_abbr [layer=_noun] <-
	_xWILD [one match=(mr mrs dr messrs inc jr sen)]
	\. [opt]
	_xWHITE [star lookahead]
	_xCAP
	@@

# 10/07/06 AM.
@CHECK
  L("txt") = strtolower(N("$text",1));
  if (spellword(L("txt")))	# 04/22/07 AM.
	fail();
  L("len") = strlength(L("txt"));
  if (L("len") > 3)	# 04/22/07 AM.
	fail();
  if (finddictattr(L("txt"),"common-cap"))	# 04/23/07 AM.
 	fail();
@POST
  L("tmp1") = N(1);
  group(1,2,"_abbr");
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  N("cap",1) = 1;
@RULES
_xNIL <-
	_xCAP
	\.
	@@

# Too broad. # 04/10/06 AM.
#@PRE
#<1,1> length(2);
#@POST
#  S("cap") = 1;
#  singler(1,2);
#@RULES
#_abbr [layer=_noun] <-
#	_xCAP
#	\. [opt]
#	_xWHITE [star lookahead]
#	_xCAP
#	@@

# Period at end of letter abbreviations.
@PRE
<4,4> lowercase();
@POST
  listadd(1,2,"false");
@RULES
_xNIL <-
	_letabbr
	\.
	_xWHITE [star lookahead]
	_xWILD [one match=(_xALPHA _xPUNCT)]
	@@

# letter .
# alpha .
@PRE
<1,1> length(1);
@POST
  group(1,2,"_letabbr");
  N("cap",1) = 1;	# 04/21/07 AM.
@RULES
_xNIL <-
	_xCAP
	\.
	@@


# High confidence end-of-sentence.
#@PRE
#<1,1> lowercase();
@POST
  L("txt") = N("$text",2);
  if (L("txt") == "?")
    S("sent end") = "interrogative";
  else if (L("txt") == "!")
    S("sent end") = "exclamatory";
  singler(2,3);
@RULES
_qEOS <-
	_xWILD [one match=(_xALPHA _xNUM \] \) \> \% _noun)]
	_xWILD [plus match=( \. \: \; \? \! )]
	_xWHITE [star]
	_xWILD [one lookahead match=(_xCAP _letabbr \( \[ \< \" \' )]
	@@

# Period at end of text zone.
@POST
  L("txt") = N("$text",2);
  if (L("txt") == "?")
    S("sent end") = "interrogative";
  else if (L("txt") == "!")
    S("sent end") = "exclamatory";
  singler(2,3);
@RULES
_qEOS <-
	_xWILD [one match=(_xALPHA _xNUM \] \) \> \% _noun)]
	_xWILD [plus match=( \. \: \; \? \! )]
	_xWHITE [star]
	_xEND
	@@

# Sentinel.
@PRE
<5,5> lowercase();
@POST
  noop();
@RULES
_qEOS <-
	_xWILD [one match=(_xALPHA _xNUM )]
	\. [plus]
	_xWILD [one lookahead match=( \) \] \> )]
	_xWHITE [star]
	_xALPHA
	@@

@POST
  singler(2,2);
@RULES
_qEOS <-
	_xWILD [one match=(_xALPHA _xNUM )]
	\. [plus]
	_xWILD [one lookahead match=( \) \] \> )]
	@@

# Some may be followed by noncapitalized words.
@POST
  singler(2,2);
@RULES
_qEOS <-
	_xWILD [one match=(_xALPHA _xNUM _noun \] \) \> \% )]
	_xWILD [plus match=( \: \; )]
#	_xWHITE [plus]
	@@

@RULES
_qEOS <-
	\.
	\"
	@@

@POST
  listadd(3,4,"false");
@RULES
_xNIL <-
	_xWILD [s one match=(_xPUNCT)]
	_xWHITE [star]
	_letabbr
	\.
	@@

# Just a period at end of line, assume ok for now. # 03/04/10 AM.
@POST
  group(1,1,"_qEOS");
@RULES
_xNIL <-
	\.
	_xBLANK [star]
	\n [lookahead]
	@@

# Rearguard.
@POST
  ++G("unhandled periods");
  if (G("error"))
  "err1.txt" << "[Unhandled period]" << "\n";
  L("n") = N(1);
  L("prev") = pnprev(L("n"));
  if (L("prev")) L("prev") = pnprev(L("prev"));
  L("next") = pnnext(L("n"));
  if (L("next")) L("next") = pnnext(L("next"));
  if (L("next")) L("next") = pnnext(L("next"));
  if (L("prev"))
    L("s") = pnvar(L("prev"),"$ostart");
  else
    L("s") = N("$ostart");
  if (L("next"))
    L("e") = pnvar(L("next"),"$oend");
  else
    L("e") = N("$oend");
  if (G("error"))
  "err1.txt" << " " << inputrange(L("s"),L("e"));
@RULES
_xNIL <-
	\. [plus]
	@@

# Pre-tagged issue.
# possessive apostrophe
@CHECK
  if (N("posarr") != "POS") # possessive.
    fail();
@POST
  pncopyvars();
  S("mypos") = "POS";
  single();
@RULES
_aposX <-
	\'
	@@

# ``
@POST
  S("dblquote start") = 1;
  S("nopos") = 1;
  single();
@RULES
_dblquote <-
	\`
	\`
	@@
# ''
@POST
  S("dblquote end") = 1;
  S("nopos") = 1;
  single();
@RULES
_dblquote <-
	\'
	\'
	@@


# alpha *
@POST
  L("tmp1") = N(1);
  group(1,2,"_noun");
  pncopyvars(L("tmp1"),N(1));
  N("text",1) = pnvar(L("tmp1"),"$text");
  N("punct end",1) = "*";
  if (strisupper(N("text",1)))
    chpos(N(1),"NP");
@RULES
_xNIL <- # 19
	_xALPHA
	\*
	@@

# num s
@POST
  N("nospace after",1) = 1;
  L("num") = N("$text",1);
  group(1,2,"_noun");
  N("num",1) = L("num");
  chpos(N(1),"CD");
@RULES
_xNIL <-
	_xNUM
	s
	@@

# Sentinel.
@POST
  noop();
@RULES
_xNIL <-
	_xCAP
	_xWHITE [star]
	_xCAP
	\-
	_xALPHA
	@@

# alpha - alpha
# alpha dash alpha
@POST
  L("tmp3") = N(3);
  L("txt") = strtolower(N("$text",3));
  if (strisupper(N("$text",1)) || strisupper(N("$text",3)))
    L("cap") = 1;
  if (strisupper(N("$text",3)))
    L("cap3") = 1;
  if (N("verb",3))
    {
	L("vc") = vconj(N(3));
	if (L("vc") == "-ing"
	 || L("vc") == "-en"
	 || L("vc") == "-edn")
	  L("inflected") = 1;
	}
  if (!spellword(L("txt")) || L("cap"))
    {
	group(3,3,"_noun");
	group(1,3,"_noun");
    chpos(N(1),"NP");
	}
  else if (N("adj",3) || L("inflected"))
    {
	group(3,3,"_adj");
	group(1,3,"_adj");
	}
  else if (N("noun",3))
    {
	# Default to adj for now.
	L("tmp3") = N(3);
	group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	fixnoun(N(3));
	L("pos") = N("mypos",3);
	group(1,3,"_adj");
	if (pnvar(L("tmp3"),"sem") == "date")
	  chpos(N(1),"JJ");  # eg., "five-year".
	else
	  chpos(N(1),L("pos")); # NN or NNS...
	}
  else
    {
	# Default to adj for now.
	group(3,3,"_adj");
	group(1,3,"_adj");
	}
  N("hyphenated",1) = 1;
  if (L("cap"))
    N("cap",1) = 1;
  if (L("cap3"))
	N("pos_np") = "NP";	# 06/08/06 AM.
  else
    N("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
@RULES
_xNIL <-
	_xALPHA # [s] buggy -- can't reduce nonlit to lit.
	\-
	_xALPHA # [s] buggy
	@@

# ( letter )
# List item, etc.
@PRE
<2,2> length(1);
@POST
  chpos(N(2),"NN");
  group(1,3,"_listitem");
  group(1,1,"_noun");
@RULES
_xNIL <-
	\(
	_xALPHA
	\)
	@@

# Zap single quotes for now.	# 05/10/07 AM.
@POST
  excise(2,2);
@RULES
_xNIL <-
	_xWHITE [star]
	_xWILD [one match=( \` \' )]
	@@
@POST
  excise(1,1);
@RULES
_xNIL <-
	_xWILD [one match=( \` \' )]
	_xWHITE [star]
	@@

# Zap all control characters	# 03/04/10 AM.
@POST
  excise(1,1);
@RULES
_xNIL <- _xCTRL @@
