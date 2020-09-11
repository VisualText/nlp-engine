###############################################
# FILE: common.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 09/Jul/04 01:31:54
# MODIFIED:
###############################################
@CODE
L("hello") = 0;
@@CODE
@PATH _ROOT _TEXTZONE


#### NUM stuff
# num %
@POST
  pncopyvars(2);
  chpos(N(1),"CD");
  chpos(N(2),"NN");
  S("percent") = N("$text",1);
  S("sem") = "quantity";
  S("sem detail") = "percent";
  sclearpos(1,0);
  single();
@RULES
_percent [layer=(_noun)] <-
	_xWILD [one match=(_num _xNUM)]
	\%
	@@

#### FNWORD100 PASS ####
# That is, follow-ons to fnword pass... # 06/18/06 AM.
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"JJ");
@RULES
_xNIL <-
	_xWILD [s one match=(
		an the my his her their our your its
		)]
	only
	@@

#### DICTFIX 2nd pass # 06/18/06 AM.

# best
@POST
  N("pos_np_head") = "JJS";
@RULES
_xNIL <- _xWILD [s one match=(
	best
	)] @@

# data
@POST
  N("pos_np_head") = "NNS";
@RULES
_xNIL <- _xWILD [s one match=(
	data
	)] @@

# one
@POST
  N("pos_np_head") = "NN";
@RULES
_xNIL <- _xWILD [s one match=(
	one
	part	# Try this.	# 06/24/06 AM.
	)] @@

# first
@POST
  N("pos_np_head") = "JJ";
@RULES
_xNIL <- _xWILD [s one match=(
	first
	)] @@

# Default.
@POST
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"RB");
@RULES
_xNIL <-
	only
	@@

# here
@CHECK
  if (!G("conform treebank"))
    fail();
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"RB");
@RULES
_xNIL <- here @@

# willing
@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"JJ");
@RULES
_xNIL <-
	willing
	@@


#### IRREGULAR VERB FORMS #####
@POST
  N("sem") = N("stem") = "come";
  N("pos num") = 1;
  N("verb") = 1;
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- came @@
@POST
  N("sem") = N("stem") = "come";
  N("pos num") = 1;
  N("verb") = 1;
  N("inf") = 1;
  N("-en") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- come @@
# Highly ambiguous conjugation, how to handle....
@POST
  N("sem") = N("stem") = "set";
  N("noun") = 1;
  N("adj") = 1;
  N("verb") = 1;
  N("pos num") = 3;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- set @@
@POST
  N("sem") = N("stem") = "cut";
  N("noun") = 1;
  N("adj") = 1;
  N("verb") = 1;
  N("pos num") = 3;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- cut @@
@POST
  N("sem") = N("stem") = "bid";
  N("noun") = 1;
  N("verb") = 1;
  N("pos num") = 2;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- bid @@
@POST
  N("sem") = N("stem") = "let";
  N("noun") = 1;
  N("verb") = 1;
  N("pos num") = 2;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- let @@

@POST
  N("sem") = N("stem") = "choose";
  N("-ed") = 1;
@RULES
_xNIL <- chose @@
@POST
  N("sem") = N("stem") = "choose";
  N("-en") = 1;
@RULES
_xNIL <- chosen @@
@POST
  N("sem") = N("stem") = "overthrow";
  N("-ed") = 1;
@RULES
_xNIL <- overthrew @@
@POST
  N("sem") = N("stem") = "overthrow";
  N("-en") = 1;
@RULES
_xNIL <- overthrown @@
@POST
  N("sem") = N("stem") = "throw";
  N("-ed") = 1;
@RULES
_xNIL <- threw @@
@POST
  N("sem") = N("stem") = "throw";
  N("-en") = 1;
@RULES
_xNIL <- thrown @@

@POST
  N("sem") = N("stem") = "take";
  N("-ed") = 1;
@RULES
_xNIL <- took @@
@POST
  N("sem") = N("stem") = "take";
  N("-en") = 1;
@RULES
_xNIL <- taken @@
@POST
  N("sem") = N("stem") = "shake";
  N("-ed") = 1;
@RULES
_xNIL <- shook @@
@POST
  N("sem") = N("stem") = "shake";
  N("-en") = 1;
@RULES
_xNIL <- shaken @@

@POST
  N("sem") = N("stem") = "go";
  N("-s") = 1;
@RULES
_xNIL <- goes @@
@POST
  N("sem") = N("stem") = "go";
  N("-ed") = 1;
@RULES
_xNIL <- went @@
@POST
  N("sem") = N("stem") = "go";
  N("-en") = 1;
@RULES
_xNIL <- gone @@
# getting listed as noun, can't corroborate.
@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("stem") = "get";
  N("pos") = "_verb";
  N("-ing") = 1;
@RULES
_xNIL <- getting @@

@POST
  N("sem") = N("stem") = "get";
  N("-edn") = 1;	# 07/30/04 AM.
@RULES
_xNIL <- got @@
@POST
  N("sem") = N("stem") = "get";
  N("-en") = 1;
@RULES
_xNIL <- gotten @@
@POST
  N("sem") = N("stem") = "get";
@RULES
_xNIL <- _xWILD [one match=(get gets getting)] @@
@POST
  N("sem") = N("stem") = "make";
  N("-edn") = 1;
@RULES
_xNIL <- made @@
@POST
  N("sem") = N("stem") = "meet";
  N("-edn") = 1;
@RULES
_xNIL <- met @@
@POST
  N("sem") = N("stem") = "bend";
  N("-edn") = 1;
@RULES
_xNIL <- bent @@
@POST
  N("sem") = N("stem") = "lend";
  N("-edn") = 1;
@RULES
_xNIL <- lent @@
@POST
  N("stem") = N("sem") = "pay";
  N("-edn") = 1;
@RULES
_xNIL <- paid @@
@POST
  N("sem") = N("stem") = "fix";
  N("-edn") = 1;
@RULES
_xNIL <- _xWILD [one match=(fixed fixt)] @@
@POST
  N("sem") = N("stem") = "find";
  N("-edn") = 1;
@RULES
_xNIL <- found @@
@POST
  N("sem") = N("stem") = "uphold";
  N("-edn") = 1;
@RULES
_xNIL <- upheld @@
@POST
  N("sem") = N("stem") = "stand";
  N("-edn") = 1;
@RULES
_xNIL <- stood @@
@POST
  N("sem") = N("stem") = "know";
  N("-ed") = 1;
@RULES
_xNIL <- knew @@
@POST
  N("sem") = N("stem") = "know";
  N("-en") = 1;
@RULES
_xNIL <- known @@
@POST
  N("sem") = N("stem") = "begin";
  N("-ed") = 1;
  chpos(N(1),"VBD");
  N("pos num") = 1;
  N("verb") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- began @@
@POST
  N("sem") = N("stem") = "begin";
  N("-en") = 1;
@RULES
_xNIL <- begun @@
# SAW -- ambiguous word...
@POST
  N("sem") = N("stem") = "see";
  N("-ed") = 1;
@RULES
_xNIL <- saw @@
@POST
  N("sem") = N("stem") = "see";
  N("-en") = "seen";
@RULES
_xNIL <- seen @@
@POST
  N("sem") = N("stem") = "hear";
  N("-edn") = 1;
@RULES
_xNIL <- heard @@
@POST
  N("sem") = N("stem") = "build";
  N("-edn") = 1;
@RULES
_xNIL <- built @@
@POST
  N("sem") = N("stem") = "keep";
  N("-edn") = 1;
@RULES
_xNIL <- kept @@
@POST
  N("sem") = N("stem") = "bleed";
  N("-edn") = 1;
@RULES
_xNIL <- bled @@
@POST
  N("sem") = N("stem") = "win";
  N("-edn") = 1;
@RULES
_xNIL <- won @@
@POST
  N("sem") = N("stem") = "leave";
  N("-edn") = 1;
@RULES
_xNIL <- left @@

@POST
  N("sem") = N("stem") = "tell";
  N("-edn") = 1;
@RULES
_xNIL <- told @@
@POST
  N("sem") = N("stem") = "sell";
  N("-edn") = 1;
@RULES
_xNIL <- sold @@
@POST
  N("sem") = N("stem") = "spend";
  N("-edn") = 1;
@RULES
_xNIL <- spent @@
@POST
  N("sem") = N("stem") = "rise";
  N("-ed") = 1;
@RULES
_xNIL <- rose @@
@POST
  N("sem") = N("stem") = "rise";
  N("-en") = 1;
  chpos(N(1),"VBN");
@RULES
_xNIL <- risen @@
@POST
  N("sem") = N("stem") = "grow";
  N("-ed") = 1;
@RULES
_xNIL <- grew @@
@POST
  N("sem") = N("stem") = "grow";
  N("-en") = 1;
  chpos(N(1),"VBN");
@RULES
_xNIL <- grown @@
@POST
  N("sem") = N("stem") = "write";
  N("-ed") = 1;
@RULES
_xNIL <- wrote @@
@POST
  N("sem") = N("stem") = "write";
  N("-en") = 1;
  chpos(N(1),"VBN");
@RULES
_xNIL <- written @@
@POST
  N("sem") = N("stem") = "buy";
  N("-edn") = 1;
@RULES
_xNIL <- bought @@
@POST
  N("sem") = N("stem") = "bring";
  N("-edn") = 1;
@RULES
_xNIL <- brought @@
@POST
  N("sem") = N("stem") = "catch";
  N("-edn") = 1;
@RULES
_xNIL <- caught @@
@POST
  N("sem") = N("stem") = "give";
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- gave @@
@POST
  N("sem") = N("stem") = "give";
  N("-en") = 1;
@RULES
_xNIL <- given @@
@POST
  N("sem") = N("stem") = "hold";
  N("-edn") = 1;
@RULES
_xNIL <- held @@
@POST
  N("sem") = N("stem") = "say";
  N("-edn") = 1;
@RULES
_xNIL <- said @@
@POST
  N("sem") = N("stem") = "fall";
  N("-ed") = 1;
@RULES
_xNIL <- fell @@
@POST
  N("sem") = N("stem") = "slide";
  N("-edn") = 1;
@RULES
_xNIL <- slid @@
@POST
  N("sem") = "rise";
  N("stem") = "rise";
  N("-en") = 1;
@RULES
_xNIL <- risen @@
@POST
  N("sem") = "hang";
  N("stem") = "hang";
  N("-edn") = 1;
@RULES
_xNIL <- hung @@
@POST
  N("sem") = "person";
  N("stem") = "person";
  N("number") = "plural";
  N("inf") = 1;
@RULES
_xNIL <- people @@
@POST
  N("sem") = "woman";
  N("stem") = "woman";
  N("number") = "plural";
@RULES
_xNIL <- women @@
@POST
  N("sem") = "child";
  N("stem") = "child";
  N("number") = "plural";
@RULES
_xNIL <- children @@
@POST
  N("sem") = "foot";
  N("stem") = "foot";
  N("number") = "plural";
@RULES
_xNIL <- feet @@
@POST
  N("sem") = "medium";
  N("stem") = "medium";
  N("number") = "plural";
@RULES
_xNIL <- media @@
# Very short plurals will need to be spelled out explicitly.
@POST
  N("sem") = "ad";
  N("stem") = "ad";
  N("number") = "plural";
  N("-s") = 1;
@RULES
_xNIL <- ads @@
@POST
  N("sem") = N("stem") = "become";
  N("inf") = 1;
  N("-en") = 1;
@RULES

_xNIL <- become @@
@POST
  N("sem") = N("stem") = "become";
  N("-ed") = 1;
@RULES
_xNIL <- became @@
@POST
  N("stem") = N("sem") = "eat";
  N("-ed") = 1;
@RULES
_xNIL <- ate @@
@POST
  N("stem") = N("sem") = "eat";
  N("pos num") = 2;
  N("verb") = 1;
  N("adj") = 1;
  N("-en") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- eaten @@
@POST
  N("stem") = N("sem") = "arise";
  N("pos num") = 1;
  N("verb") = 1;
  N("-en") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- arisen @@
@POST
  N("stem") = N("sem") = "arise";
  N("pos num") = 1;
  N("verb") = 1;
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- arose @@
@POST
  N("stem") = N("sem") = "forgive";
  N("pos num") = 1;
  N("verb") = 1;
  N("-en") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- forgiven @@
@POST
  N("stem") = N("sem") = "forgive";
  N("pos num") = 1;
  N("verb") = 1;
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- forgave @@

@POST
  N("stem") = N("sem") = "drive";
  N("pos num") = 1;
  N("verb") = 1;
  N("-en") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- driven @@
@POST
  N("stem") = N("sem") = "drive";
  N("pos num") = 1;
  N("verb") = 1;
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- drove @@
_xNIL <- drived @@	# sic

# Ambiguous.
@POST
  N("stem") = N("sem") = "run";
  N("pos num") = 2;
  N("noun") = 1;
  N("verb") = 1;
  N("-en") = 1;
  N("inf") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- run @@

@POST
  N("stem") = N("sem") = "run";
  N("pos num") = 1;
  N("verb") = 1;
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- ran @@

@POST
  N("stem") = N("sem") = "tv";
  N("pos num") = 1;
  N("noun") = 1;
  N("pos") = "_noun";
  N("number") = "singular";
@RULES
_xNIL <- tv @@

#### OTHER STUFF ####
@POST
  N("sem") = "color";
  N("stem") = strtolower(N("$text"));
@RULES
_xNIL <-
	_xWILD [s one match=(
		red orange yellow green blue indigo violet
		white black grey gray
		purple
	)] @@
###############
# Nationalities
###############
@POST
  pncopyvars(1);
  S("sem detail") = S("stem") = strtolower(phrasetext());
  S("sem") = "nationality";
  S("ne") = 1;
  S("ne type") = "location";
  S("ne type conf") = 95;
  L("country") = finddictsattr(S("sem detail"),"country");
  if (L("country"))
    S("country") = L("country");
  single();
@RULES
_adjNation [layer=(_adj)] <-
	Bangla [s]
	_xWHITE [s star]
	Deshi [s]
	@@
_adjNation [layer=(_adj)] <-
	Bosnia [s]
	_xWILD [star match=(_xWHITE \- )]
	Herzegovinan [s]
	@@
_adjNation [layer=(_adj)] <-
	Burkina [s]
	_xWHITE [s star]
	Fassoan [s]
	@@
_adjNation [layer=(_adj)] <-
	Cape [s]
	_xWHITE [s star]
	Verdean [s]
	@@
_adjNation [layer=(_adj)] <-
	Central [s]
	_xWHITE [s star]
	Africa [s]
	_xWHITE [s star]
	Republican [s]
	@@
_adjNation [layer=(_adj)] <-
	Congo [s]
	_xWHITE [s star]
	Democratic [s]
	_xWHITE [star]
	Republican [s]
	@@
_adjNation [layer=(_adj)] <-
	Costa [s]
	_xWHITE [s star]
	Rican [s]
	@@
_adjNation [layer=(_adj)] <-
	Czech [s]
	@@
_adjNation [layer=(_adj)] <-
	El [s]
	_xWHITE [s star]
	Salvadoran [s]
	@@
_adjNation [layer=(_adj)] <-
	Equatorial [s]
	_xWHITE [s star]
	Guinean [s]
	@@
_adjNation [layer=(_adj)] <-
	Guinea [s]
	_xWHITE [s star]
	Bissauan [s]
	@@
_adjNation [layer=(_adj)] <-
	Ivory [s]
	_xWHITE [s star]
	Coastan [s]
	@@
_adjNation [layer=(_adj)] <-
	New [s]
	_xWHITE [star]
	Guinean [s]
	@@
_adjNation [layer=(_adj)] <-
	New [s]
	_xWHITE [s star]
	Zealander [s]
	@@
_adjNation [layer=(_adj)] <-
	North [s]
	_xWHITE [s star]
	Korean [s]
	@@
_adjNation [layer=(_adj)] <-
	Papua [s]
	_xWHITE [s star]
	New [s]
	_xWHITE [star]
	Guinean [s]
	@@
_adjNation [layer=(_adj)] <-
	Puerto [s]
	_xWHITE [s star]
	Rican [s]
	@@
_adjNation [layer=(_adj)] <-
	San [s]
	_xWHITE [s star]
	Marinan [s]
	@@
_adjNation [layer=(_adj)] <-
	Saudi [s]
	_xWHITE [s star]
	Arabian [s]
	@@
_adjNation [layer=(_adj)] <-
	Sierra [s]
	_xWHITE [s star]
	Leonean [s]
	@@
_adjNation [layer=(_adj)] <-
	Slovak [s]
	@@
_adjNation [layer=(_adj)] <-
	South [s]
	_xWHITE [s star]
	African [s]
	@@
_adjNation [layer=(_adj)] <-
	Sri [s]
	_xWHITE [s star]
	Lankan [s]
	@@
_adjNation [layer=(_adj)] <-
	South [s]
	_xWHITE [s star]
	Korean [s]
	@@
###############
# Countries
###############
@POST
  S("ne type") = "location";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("stem") = S("country") = S("sem detail") = "usa";
  S("sem") = "country";
  single();
@RULES
_nounCountry [layer=_noun] <-
	united [s]
	_xWHITE [s star]
	states [s]
	_xWHITE [s star]
	of [s opt]
	_xWHITE [s star]
	america [s opt]
	@@
@POST
  pncopyvars(1); # For copying pretagged pos info.
  S("mypos") = "NP";
  S("ne type") = "location";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("stem") = S("sem detail") = strtolower(phrasetext());
  S("sem") = "country";
  single();
@RULES
_nounCountry [layer=(_noun)] <-
	Bangla [s]
	_xWHITE [s star]
	Desh [s]
	@@
_nounCountry [layer=(_noun)] <-
	Bosnia [s]
	_xWILD [star match=(_xWHITE \- )]
	Herzegovina [s]
	@@
_nounCountry [layer=(_noun)] <-
	Burkina [s]
	_xWHITE [s star]
	Fasso [s]
	@@
_nounCountry [layer=(_noun)] <-
	Cape [s]
	_xWHITE [s star]
	Verde [s]
	@@
_nounCountry [layer=(_noun)] <-
	Central [s]
	_xWHITE [s star]
	Africa [s]
	_xWHITE [s star]
	Republic [s]
	@@
_nounCountry [layer=(_noun)] <-
	Congo [s]
	_xWHITE [s star]
	Democratic [s]
	_xWHITE [star]
	Republic [s]
	@@
_nounCountry [layer=(_noun)] <-
	Costa [s]
	_xWHITE [s star]
	Rica [s]
	@@
_nounCountry [layer=(_noun)] <-
	Czech [s]
	_xWHITE [s star]
	Republic [s]
	@@
_nounCountry [layer=(_noun)] <-
	El [s]
	_xWHITE [s star]
	Salvador [s]
	@@
_nounCountry [layer=(_noun)] <-
	Equatorial [s]
	_xWHITE [s star]
	Guinea [s]
	@@
_nounCountry [layer=(_noun)] <-
	Guinea [s]
	_xWHITE [s star]
	Bissau [s]
	@@
_nounCountry [layer=(_noun)] <-
	Ivory [s]
	_xWHITE [s star]
	Coast [s]
	@@
_nounCountry [layer=(_noun)] <-
	New [s]
	_xWHITE [star]
	Guinea [s]
	@@
_nounCountry [layer=(_noun)] <-
	New [s]
	_xWHITE [s star]
	Zealand [s]
	@@
_nounCountry [layer=(_noun)] <-
	North [s]
	_xWHITE [s star]
	Korea [s]
	@@
_nounCountry [layer=(_noun)] <-
	Papua [s]
	_xWHITE [s star]
	New [s]
	_xWHITE [star]
	Guinea [s]
	@@
_nounCountry [layer=(_noun)] <-
	Puerto [s]
	_xWHITE [s star]
	Rico [s]
	@@
_nounCountry [layer=(_noun)] <-
	San [s]
	_xWHITE [s star]
	Marino [s]
	@@
_nounCountry [layer=(_noun)] <-
	Saudi [s]
	_xWHITE [s star]
	Arabia [s]
	@@
_nounCountry [layer=(_noun)] <-
	Sierra [s]
	_xWHITE [s star]
	Leone [s]
	@@
_nounCountry [layer=(_noun)] <-
	Slovak [s]
	_xWHITE [s star]
	Republic [s]
	@@
_nounCountry [layer=(_noun)] <-
	South [s]
	_xWHITE [s star]
	Africa [s]
	_xWHITE [s star]
	Republic [s]
	@@
_nounCountry [layer=(_noun)] <-
	Sri [s]
	_xWHITE [s star]
	Lanka [s]
	@@
_nounCountry [layer=(_noun)] <-
	South [s]
	_xWHITE [s star]
	Korea [s]
	@@
_nounCountry [layer=(_noun)] <-
	Soviet [s]
	_xWHITE [s star]
	Union [s]
	@@
_nounCountry [layer=(_noun)] <-
	United [s]
	_xWHITE [s star]
	Arab [s]
	_xWHITE [star]
	Emirates [s]
	@@
_nounCountry [layer=(_noun)] <-
	United [s]
	_xWHITE [s star]
	Kingdom [s]
	@@
_nounCountry [layer=(_noun)] <-
	Vatican [s]
	_xWHITE [s star]
	City [s]
	@@
#City of Vatican
#Sao Tome e Principe
#Dominicana, Rep.
#St. Christopher Nevis
#St. Lucia
#St. Vincent & Grenadines
#Trinidad y Tobago
#Western Timor
#Fed. St. of Micronesia

###############
# Nationalities
###############
@POST
  pncopyvars(1);
  S("sem detail") = S("stem") = strtolower(phrasetext());
  S("sem") = "nationality";
  S("ne") = 1;
  S("ne type") = "location";
  S("ne type conf") = 95;
  L("country") = finddictsattr(S("sem detail"),"country");
  if (L("country"))
    S("country") = L("country");
  single();
@RULES
_adjNation [layer=(_adj)] <- _xWILD [s one match=(
African
American
Arab
Argentine
Argentinian
Asian
Australian
Belgian
Brazilian
British
Cambodian
Canadian #
Chinese
Danish
Dutch
#English
European
Finnish
French
German
Greek
Hungarian
Indian
Indonesian #
Iraqi
Irish
Israeli
Italian
Japanese
Korean #
Liberian #
Norwegian
Pakistani
Palestinian #
Polish
Portuguese
Russian
Saudi
Scots
Scottish
Siberian
Spanish
Swedish
Swiss
Thai	#
Ukrainian
Venezuelan #
Vietnamese
Welsh
	)] @@


@POST
  L("tmp1") = N(1);
  N("sem detail") = strtolower(N("$text"));
  N("sem") = "country";
#  S("apos-s") = N("apos-s");
  group(1,1,"_nounCountry");
#  if (S("apos-s"))
#    {
#    group(1,1,"_adj");
#	N("apos-s",1) = 1;
#	}
#  else
    group(1,1,"_noun");
	pncopyvars(L("tmp1"),N(1));
	chpos(N(1),"NP");
	N("ne") = 1;
	N("ne type") = "location";
    N("ne type conf") = 95;
	if (!N("stem"))
	 N("stem") = strtolower(N("$text"));
@RULES
_xNIL <- _xWILD [s one
	except=(_seg _np _noun)
	match=(
Afghanistan
Albania
Algeria
Andorra
Angola
Antigua
Argentina
Armenia
Australia
Austria
Azerbaijan
Bahamas
Bahrain
Barbados
Belarus
Belgium
Belize
Benin
Bhutan
Bolivia
Bosnia
Botswana
Brazil
Britain
Brunei
Bulgaria
Burma
Burundi
Cambodia
Cameroon
Canada
Chad
Chile
China
Colombia
Comoros
Croatia
Cuba
Cyprus
Denmark
Djibouti
Dominica
Ecuador
Egypt
England
Eritrea
Estonia
Ethiopia
Fidji
Finland
France
Gabon
Gambia
Georgia
Germany
Ghana
Greece
Grenada
Guatemala
Guinea
Guyana
Haiti
Holland
Honduras
Hungary
Iceland
India
Indonesia
Iran
Iraq
Ireland
Israel
Italy
Jamaica
Japan
Jordan
Kazajstan
Kenya
Kirgizistan
Kiribati
Kuwait
Laos
Latvia
Lebanon
Lesotho
Liberia
Libya
Liechtestein
Lithuania
Luxembourg
Macedonia
Madagascar
Malawi
Malaysia
Maldives
Mali
Malta
Marshall
Mauritania
Mauritius
Mexico
Moldova
Monaco
Mongolia
Morocco
Mozambique
Namibia
Nauru
Nepal
Netherlands
Nicaragua
Niger
Norway
Oman
Pakistan
Palau
Palestine
Panama
Paraguay
Peru
Philippines
Poland
Portugal
Qatar
Romania
Russia
Rwanda
Samoa
Scotland
Senegal
Seychelles
Singapore
Slovenia
Solomon
Somalia
Spain
Sudan
Surinam
Swaziland
Sweden
Switzerland
Syria
Taiwan
Tanzania
Tayijistan
Thailand
Togo
Tonga
Tunisia
Turkey
Turkmenistan
Tuvalu
Uganda
Ukraine
Uruguay
USA
Uzbekistan
Vanuatu
Venezuela
Vietnam
Wales
Yemen
Yugoslavia
Zambia
	Africa
	America
	Asia
	Europe
	Oceania
	)] @@
@PRE
<1,1> cap();	# 09/24/04 AM.
@POST
  L("tmp1") = N(1);
  group(1,1,"_moWord");
  N("sem") = "date";
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  N("sem") = "date";
  chpos(N(1),"NP");
  N("ne") = 1;
  N("ne type") = "date";
  N("ne type conf") = 95;
@RULES	# FIX.  # 02/12/02 AM.
_xNIL <- _xWILD [s one match=(
	January February March April May June July August
	September October November December
	
	Jan Feb Mar Apr May June Jul Aug Sep Sept Oct Nov Dec
	)] @@
#!JPW TODO Need to do better with March/April/May/June/August ambiguity.
