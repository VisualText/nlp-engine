###############################################
# FILE: tok50.pat
# SUBJ: Token sleuthing.
# AUTH: AM
# CREATED: 20/Jul/04 15:07:16
# NOTE:	Because pos-tagged corpora gives us tokenized text,
#	we may want to gather them the same way.
###############################################

@CODE
L("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _tok
@NODES _tok

# num . num
@POST
  xrename("_num");
  X("num") = N("$text",2) + "." + N("$text",4);
@RULES
_xNIL <-
	_xSTART
	_xNUM
	\.
	_xNUM
	_xEND
	@@

# num / num
@POST
  xrename("_num");
  X("num") = 1;
  X("fraction") = 1;
@RULES
_xNIL <-
	_xSTART
	_xNUM
	\/
	_xNUM
	_xEND
	@@

# Note: Can redo pos in later pass for street numbers and
#	proper nouns.
@POST
  xrename("_num");
  X("num") = 1;	# singular.
  X("ordinal") = 1;
  chpos(X(),"JJ");	# Conform Treebank.
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus fail=(st nd rd th)]
	_xWILD [one match=(st nd rd th)]
	_xEND
	@@

# cap & cap
# cap / cap
@POST
  xrename("_noun");
  chpos(X(),"NP");
  X("cap") = 1;
@RULES
_xNIL <-
	_xSTART
	_xCAP
	_xWILD [one match=( \& \/ )]
	_xCAP
	_xWILD [star match=(_xPUNCT _xCAP)]
	_xEND
	@@

# - num
# Could be suffix of alpha-num type noun.
@POST
  xrename("_num");
  X("num") = N("$text",3);
@RULES
_xNIL <-
	_xSTART
	\-
	_xNUM
	_xEND
	@@

# Note: some misparsed treebank tokens.
@POST
  if (strisupper(N("$text",9)))
    xrename("_noun");
  else
    xrename("_adj");
  X("sem") = "us_state";
  X("cap") = 1;
@RULES
_xNIL <-
	_xSTART
	_xCAP
	\.
	_xCAP [opt]
	\. [opt]
	_xCAP [opt]
	\. [opt]
	\-
	_xALPHA
	_xEND
	@@

# US $
@POST
  xrename("_money");
@RULES
_xNIL <-
	_xSTART
	US
	\$
	_xEND
	@@

# ``
# ''
@POST
  ++G("dbl quotes");
  xrename("_dblquote");
  X("nopos") = 1;
@RULES
_xNIL <-
	\`
	\`
	@@
_xNIL <-
	\'
	\'
	@@


# Mishandled by pretagged.
@POST
  xrename("_modal");
  chpos(X(),"VBP");
  X("neg") = 1;
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(
		can
		doesn
		don
		mayn
		mightn
		mustn
		shan
		won
		)]
	\'
	t
	_xEND
	@@
@POST
  xrename("_modal");
  chpos(X(),"VBD");
  X("neg") = 1;
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(
		couldn
		didn
		shouldn
		wouldn
		)]
	\'
	t
	_xEND
	@@

# Pre-tagged artifact.
# n't
# n ' t

@POST
  xrename("_adv");
  chpos(X(),"RB");
  X("neg") = 1;
  X("tok") = "n't";
@RULES
_xNIL <-
	n [s]
	\' [s]
	t [s]
	@@

# Some abbreviations.
@PRE
<2,2> cap();	# 10/14/06 AM.
@POST
  xrename("_noun");
  chpos(X(),"NP");
  X("abbrev") = 1;
  X("cap") = 1; # 5/25/06 AM.
  single();
@RULES
_month <-
	_xSTART
	_xWILD [one match=(Jan Feb Mar Apr May Jun Jul Aug
		Sep Sept Oct Nov Dec)]
	\. [opt]
	_xEND
	@@

_title <-
	_xSTART
	_xWILD [one match=(mr mrs dr messrs sen rep)]
	\.
	_xEND
	@@
_abbr <-
	_xSTART
	_xWILD [one match=(jr st)]
	\.
	_xEND
	@@

_companyDESIG <-
	_xSTART
	_xWILD [one match=(co corp inc ltd llc bancorp)]
	\. [opt]
	_xEND
	@@
#n.v.
_companyDESIG <- # 17
	_xSTART
	n
	\. [opt]
	v
	\. [opt]
	_xEND
	@@

@POST
  xrename("_noun");
  chpos(X(),"NN");
  X("abbrev") = 1;
  X("cap") = 1;
  single();
@RULES
_noabbr <-
	_xSTART
	_xWILD [one match=(
		No
		)]
	\. [opt]
	_xEND
	@@

# Some common contractions.
# 're
@POST
  X("stem") = X("sem") = "be";
  X("number") = 1;
#  X("-ed") = 1;
  group(2,3,"_be");
  xrename("_verb");
  chpos(X(),"VBP");
@RULES
_xNIL <-
	_xSTART
	\'
	re
	_xEND
	@@

# letter .
# alpha .
@PRE
<2,2> length(1);
@POST
  xrename("_letabbr");
  X("cap") = 1;	# 04/21/07 AM.
@RULES
_xNIL <- # 20
	_xSTART
	_xALPHA
	\.
	_xEND
	@@

# alpha *
@POST
#  xrename("_noun");
#  pncopyvars(N(2),X());
  X("text") = N("$text",2);
  X("punct end") = "*";
  excise(3,3);
#  if (strisupper(X("text")))
#    chpos(X(),"NP");
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	\*
	_xEND
	@@

# 1960s
# num s
@POST
  xrename("_noun");
  chpos(X(),"NNS");	# Treebank 5/2 NNS/CD.
  X("id") = "tok50 num-s";
@RULES
_xNIL <-
	_xSTART
	_xNUM
	s
	_xEND
	@@

# 1,000,000
# num , num , num
@PRE
<4,4> length(3);
<6,6> length(3);
@POST
  xrename("_num");
  chpos(X(),"CD");
  X("id") = "tok50 num,num,num";
@RULES
_xNIL <-
	_xSTART
	_xNUM
	\,
	_xNUM
	\,
	_xNUM
	_xEND
	@@

# 1,000
# num , num
@PRE
<4,4> length(3);
@POST
  xrename("_num");
  chpos(X(),"CD");
  X("id") = "tok50 num,num";
  X("number") = "plural";
@RULES
_xNIL <-
	_xSTART
	_xNUM
	\,
	_xNUM
	_xEND
	@@

# US States.
@POST
 xrename("_noun");
 X("sem") = "us_state";
  chpos(X(),"NP");
 if (N(4))
   X("pos eos") = 1;
 X("id") = "tok50 us-state";
 X("cap") = 1;
 single();
@RULES
_usstate <-
	n
	\.
	_xWILD [one match=(c d h j m y)]
	\. [opt]
	@@
@POST
 xrename("_noun");
 X("sem") = "us_state";
  chpos(X(),"NP");
 if (N(4))
   X("pos eos") = 1;
 X("id") = "tok50 us-state";
 X("cap") = 1;
 single();
@RULES
_usstate <-
	s
	\.
	_xWILD [one match=(c d)]
	\. [opt]
	@@


# US States
@POST
 xrename("_noun");
 X("sem") = "us_state";
  chpos(X(),"NP");
 if (N(2))
   X("pos eos") = 1;
 X("id") = "tok50 us-state";
 single();
@RULES
_usstate <-
	_xSTART
	_xWILD [one match=(
AA
AE
AK
AL
Ala
Alab
Alabama
Alas
Alaska
AP
AR
Ari
Ariz
Arizona
Ark
Arkan
Arkansas
AS
AZ
CA
Cal
Calif
California
CO
Col
Colo
Colorado
Columbia
Conn
Connecticut
CT
Dak
Dakota
DC
DE
Del
Dela
Delaware
FL
Fla	# 09/07/06 AM.
Flo
Flor
Florida
FM
GA
Geo
Georg
Georgia
GU
Guam
Haw
Hawaii
HI
IA
ID
Ida
Idaho
IL
Ill
Illin
Illinois
IN
Ind
Indiana
Iowa
Kan
Kans
Kansas
Ken
Kent
Kentucky
KS
KY
LA
Louis
Louisiana
MA
Maine
Mary
Maryland
Mass
Massachusetts
MD
ME
MH
MI
Mich
Michigan
Micronesia
Minn
Minnesota
Miss
Mississippi
Missouri
MN
MO
Mon
Mont
Montana
MP
MS
MT
NC
ND
NE
Neb
Nebraska
Nev
Nevada
NH
NJ
NM
NSW
NV
NY
OH
Ohio
OK
Okl
Okla
Oklahoma
OR
Ore
Oreg
Oregon
Orpalau
PA
Penn
Pennsyl
Pennsylvania
PR
PW
RI
Samoa
SC
SD
Tenn
Tennessee
Tex
Texas
TN
TX
UT
Utah
VA
Ver
Verm
Vermont
#VI
Vir
Virg
Virgin
Virginia
VT
WA
Wash
Washington
WI
Wis
Wisc
Wiscon
Wisconsin
WV
WY
Wyo
Wyoming
)]
	\. [opt]
	_xEND
	@@

# Note:	Some abbreviations.
@CHECK
  L("t") = strtolower(N("$text",2));
  if (spellword(L("t")))
	fail();
@POST
  xrename("_noun");
  X("cap") = 1;	# 04/21/07 AM.
@RULES
_xNIL <- # 20
	_xSTART
	_xCAP
	\.
	_xEND
	@@


# Put some semantics on.	# 04/15/05 AM.
@POST
  if (G("conform treebank"))
    {
    chpos(X(),"NN");
	N("bracket") = 1;
	}
  N("sem") = "date";
  N("advl") = 1;	# Possible standalone adverbial.
  L("xx") = pnparent(X());	# 07/10/12 AM.
  # Some temporal evidence.
  L("t") = strtolower(N("$text"));
  if (L("t") == "today")
	pnreplaceval(L("xx"),"date=present",1);	# 07/10/12 AM.
  else if (L("t") = "yesterday")
	pnreplaceval(L("xx"),"date=past",1);	# 07/10/12 AM.
  else
	pnreplaceval(L("xx"),"date=future",1);	# 07/10/12 AM.
  # General date references.
  pnreplaceval(L("xx"),"date ref",1);	# 07/10/12 AM.
@RULES
_xNIL <- _xWILD [s one match=(
	today yesterday tomorrow
	)] @@

@POST
  N("sem") = "date";
  L("xx") = pnparent(X());	# 07/10/12 AM.
  pnreplaceval(L("xx"),"date ref",1);	# 07/10/12 AM.
@RULES
_xNIL <- _xWILD [s one match=(
	millennium millennia millenniums
	century centuries
	decade decades
	year years
	month months
	week weeks
	day days
	evening evenings
	night nights
	morning mornings
	noon noons
	afternoon afternoons
	hour hours
	minute minutes
	second seconds
	time date season	# 01/12/05 AM.
	holiday holidays
	quarter quarters	# ambig, of course.
	)]
	@@

