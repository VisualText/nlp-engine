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

@NODES _TEXTZONE


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
  N("mass") = 1;
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
#  if (!G("conform treebank"))
#    fail();
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
  N("sem") = N("stem") = "abide";
  N("-edn") = 1;
@RULES
_xNIL <- abided @@
#_xNIL <- abode @@	# Obsolete, if at all. # 05/07/10 AM.
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
  N("sem") = N("stem") = "awake";
  N("-edn") = 1;
@RULES
_xNIL <- awaked @@
@POST
  N("sem") = N("stem") = "awake";
  N("-ed") = 1;
@RULES
_xNIL <- awoke @@
@POST
  N("sem") = N("stem") = "awake";
  N("-en") = 1;
@RULES
_xNIL <- awoken @@
@POST
  N("sem") = N("stem") = "befall";
  N("-ed") = 1;
@RULES
_xNIL <- befell @@
@POST
  N("sem") = N("stem") = "befall";
  N("-en") = 1;
@RULES
_xNIL <- befallen @@
@POST
  N("sem") = N("stem") = "beget";
  N("-ed") = 1;
@RULES
_xNIL <- begot @@
@POST
  N("sem") = N("stem") = "beget";
  N("-en") = 1;
@RULES
_xNIL <- begotten @@
@POST
  N("sem") = N("stem") = "behold";
  N("-edn") = 1;
@RULES
_xNIL <- beheld @@
@POST
  # Ambiguous bear/bore
  N("sem") = N("stem") = "bear";
  N("inf") = 1;
  N("-ed") = 1;
@RULES
_xNIL <- bore @@
@POST
  N("sem") = N("stem") = "bear";
  N("-en") = 1;
@RULES
_xNIL <- borne @@
@POST
  N("sem") = N("stem") = "beat";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- beat @@
@POST
  N("sem") = N("stem") = "beat";
  N("inf") = 1;
  N("-en") = 1;
@RULES
_xNIL <- beaten @@
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
@POST
  N("sem") = N("stem") = "bend";
  N("-edn") = 1;
@RULES
_xNIL <- bent @@
@POST
  N("sem") = N("stem") = "bereave";
  N("-edn") = 1;
@RULES
_xNIL <- bereaved @@
_xNIL <- bereft @@
@POST
  N("sem") = N("stem") = "beseech";
  N("-edn") = 1;
@RULES
_xNIL <- besought @@
_xNIL <- beseeched @@
@POST
  N("sem") = N("stem") = "beset";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- beset @@
@POST
  N("sem") = N("stem") = "bestride";
  N("-edn") = 1;
@RULES
_xNIL <- bestrode @@
@POST
  N("sem") = N("stem") = "bestride";
  N("-en") = 1;
@RULES
_xNIL <- bestridden @@
_xNIL <- bestrid @@
@POST
  N("sem") = N("stem") = "bet";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- bet @@
@POST
  N("sem") = N("stem") = "bet";
  N("-edn") = 1;
@RULES
_xNIL <- betted @@
@POST
  N("sem") = N("stem") = "betake";
  N("-ed") = 1;
@RULES
_xNIL <- betook @@
@POST
  N("sem") = N("stem") = "betake";
  N("-en") = 1;
@RULES
_xNIL <- betaken @@
@POST
  N("sem") = N("stem") = "bid";
  N("noun") = 1;
  N("verb") = 1;
  N("pos num") = 2;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- bid @@
# bad - also a past of bid, but rare. omitting for now.
@POST
  N("sem") = N("stem") = "bid";
  N("-edn") = 1;
@RULES
_xNIL <- bade @@
@POST
  N("sem") = N("stem") = "bid";
  N("-en") = 1;
@RULES
_xNIL <- bidden @@
@POST
  N("sem") = N("stem") = "bind";
  N("-edn") = 1;
@RULES
_xNIL <- bound @@
@POST
  N("sem") = N("stem") = "bite";
  N("-edn") = 1;
@RULES
_xNIL <- bit @@
@POST
  N("sem") = N("stem") = "bite";
  N("-en") = 1;
@RULES
_xNIL <- bitten @@
@POST
  N("sem") = N("stem") = "bleed";
  N("-edn") = 1;
@RULES
_xNIL <- bled @@
@POST
  N("sem") = N("stem") = "blow";
  N("-ed") = 1;
@RULES
_xNIL <- blew @@
@POST
  N("sem") = N("stem") = "blow";
  N("-en") = 1;
@RULES
_xNIL <- blown @@
@POST
  N("stem") = N("sem") = "break";
  N("pos num") = 2;
  N("verb") = 1;
  N("adj") = 1;
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- broke @@
@POST
  N("stem") = N("sem") = "break";
  N("pos num") = 2;
  N("verb") = 1;
  N("adj") = 1;
  N("-en") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- broken @@
@POST
  N("sem") = N("stem") = "breed";
  N("-edn") = 1;
@RULES
_xNIL <- bred @@
@POST
  N("sem") = N("stem") = "bring";
  N("-edn") = 1;
@RULES
_xNIL <- brought @@
@POST
  N("sem") = N("stem") = "broadcast";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- broadcast @@
@POST
  N("sem") = N("stem") = "build";
  N("-edn") = 1;
@RULES
_xNIL <- built @@
@POST
  N("sem") = N("stem") = "burn";
  N("-edn") = 1;
@RULES
_xNIL <- burnt @@
@POST
  N("sem") = N("stem") = "burst";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- burst @@
@POST
  N("sem") = N("stem") = "bust";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- bust @@
@POST
  N("sem") = N("stem") = "buy";
  N("-edn") = 1;
@RULES
_xNIL <- bought @@
@POST
  N("sem") = N("stem") = "chide";
  N("-edn") = 1;
@RULES
_xNIL <- chid @@
@POST
  N("sem") = N("stem") = "chide";
  N("-en") = 1;
@RULES
_xNIL <- chidden @@

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
  N("sem") = N("stem") = "cleave";
  N("-edn") = 1;
@RULES
_xNIL <- cleft @@
@POST
  N("sem") = N("stem") = "cleave";
  N("-ed") = 1;
@RULES
_xNIL <- clove @@
@POST
  N("sem") = N("stem") = "cleave";
  N("-en") = 1;
@RULES
_xNIL <- cloven @@
@POST
  N("sem") = N("stem") = "cling";
  N("-edn") = 1;
@RULES
_xNIL <- clung @@

@POST
  N("sem") = N("stem") = "come";
  N("pos num") = 1;
  N("verb") = 1;
  N("-ed") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- came @@
@POST
  N("sem") = N("stem") = "catch";
  N("-edn") = 1;
@RULES
_xNIL <- caught @@
@POST
  N("sem") = N("stem") = "come";
  N("pos num") = 1;
  N("verb") = 1;
  N("inf") = 1;
  N("-en") = 1;
  N("pos") = "_verb";
@RULES
_xNIL <- come @@
@POST
  N("sem") = N("stem") = "cast";
  N("noun") = 1;
  N("adj") = 1;
  N("verb") = 1;
  N("pos num") = 3;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- cast @@
@POST
  N("sem") = N("stem") = "cost";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- cost @@
@POST
  N("sem") = N("stem") = "creep";
  N("-edn") = 1;
@RULES
_xNIL <- crept @@

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
  N("sem") = N("stem") = "deal";
  N("-edn") = 1;
@RULES
_xNIL <- dealt @@
@POST
  N("sem") = N("stem") = "deepfreeze";
  N("-ed") = 1;
@RULES
_xNIL <- deepfroze @@
@POST
  N("sem") = N("stem") = "deepfreeze";
  N("-en") = 1;
@RULES
_xNIL <- deepfrozen @@
@POST
  N("sem") = N("stem") = "develop";
  N("-edn") = 1;
@RULES
_xNIL <- developed @@
@POST
  N("sem") = N("stem") = "develop";
  N("-ing") = 1;
@RULES
_xNIL <- developing @@
@POST
  N("sem") = N("stem") = "dig";
  N("-edn") = 1;
@RULES
_xNIL <- dug @@
@POST
  N("sem") = N("stem") = "dive";
  N("-ed") = 1;	# AmerEng.
@RULES
_xNIL <- dove @@

@POST
  N("sem") = N("stem") = "draw";
  N("-en") = 1;
@RULES
_xNIL <- drawn @@
@POST
  N("sem") = N("stem") = "draw";
  N("-edn") = 1;
@RULES
_xNIL <- drew @@
@POST
  N("sem") = N("stem") = "dream";
  N("-edn") = 1;
@RULES
_xNIL <- dreamt @@
@POST
  N("sem") = N("stem") = "drink";
  N("-ed") = 1;
@RULES
_xNIL <- drank @@
@POST
  N("sem") = N("stem") = "drink";
  N("-en") = 1;
@RULES
_xNIL <- drunk @@
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
@POST
  N("sem") = N("stem") = "dwell";
  N("-edn") = 1;
@RULES
_xNIL <- dwelt @@
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
  N("sem") = N("stem") = "employ";
  N("-edn") = 1;
@RULES
_xNIL <- employed @@
@POST
  N("sem") = N("stem") = "employ";
  N("-ing") = 1;
@RULES
_xNIL <- employing @@
@POST
  N("sem") = N("stem") = "fall";
  N("-ed") = 1;
@RULES
_xNIL <- fell @@
@POST
  N("sem") = N("stem") = "fall";
  N("-en") = 1;
@RULES
_xNIL <- fallen @@

@POST
  N("sem") = N("stem") = "feed";
  N("-edn") = 1;
@RULES
_xNIL <- fed @@
@POST
  N("sem") = N("stem") = "feel";
  N("-edn") = 1;
@RULES
_xNIL <- felt @@
@POST
  N("sem") = N("stem") = "fight";
  N("-edn") = 1;
@RULES
_xNIL <- fought @@
@POST
  N("sem") = N("stem") = "find";
  N("-edn") = 1;
@RULES
_xNIL <- found @@
@POST
  N("sem") = N("stem") = "fix";
  N("-edn") = 1;
@RULES
_xNIL <- _xWILD [one match=(fixed fixt)] @@
@POST
  N("sem") = N("stem") = "fit";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- fit @@
@POST
  N("sem") = N("stem") = "fit";
  N("-edn") = 1;
@RULES
_xNIL <- fitted @@
@POST
  N("sem") = N("stem") = "flee";
  N("-edn") = 1;
@RULES
_xNIL <- fled @@
@POST
  N("sem") = N("stem") = "fling";
  N("-edn") = 1;
@RULES
_xNIL <- flung @@
@POST
  N("sem") = N("stem") = "fly";
  N("-ed") = 1;
@RULES
_xNIL <- flew @@
@POST
  N("sem") = N("stem") = "fly";
  N("-edn") = 1;
@RULES
_xNIL <- flied @@ # baseball...
@POST
  N("sem") = N("stem") = "fly";
  N("-en") = 1;
@RULES
_xNIL <- flown @@
@POST
  N("sem") = N("stem") = "forbear";
  N("-ed") = 1;
@RULES
_xNIL <- forbore @@
@POST
  N("sem") = N("stem") = "forbear";
  N("-en") = 1;
@RULES
_xNIL <- forborne @@
@POST
  N("sem") = N("stem") = "forbid";
  N("inf") = 1;
  N("-en") = 1;
@RULES
_xNIL <- forbid @@
@POST
  N("sem") = N("stem") = "forbid";
  N("-ed") = 1;
@RULES
_xNIL <- forbad @@
@POST
  N("sem") = N("stem") = "forbid";
  N("-ed") = 1;
@RULES
_xNIL <- forbade @@
@POST
  N("sem") = N("stem") = "forbid";
  N("-en") = 1;
@RULES
_xNIL <- forbidden @@
@POST
  N("sem") = N("stem") = "forecast";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- forecast @@

@POST
  N("sem") = N("stem") = "forsake";
  N("-ed") = 1;
@RULES
_xNIL <- forsook @@
@POST
  N("sem") = N("stem") = "forsake";
  N("-en") = 1;
@RULES
_xNIL <- forsaken @@
@POST
  N("sem") = N("stem") = "foresee";
  N("-ed") = 1;
@RULES
_xNIL <- foresaw @@
@POST
  N("sem") = N("stem") = "foresee";
  N("-en") = 1;
@RULES
_xNIL <- foreseen @@
@POST
  N("sem") = N("stem") = "foretell";
  N("-edn") = 1;
@RULES
_xNIL <- foretold @@
@POST
  N("sem") = N("stem") = "forget";
  N("-edn") = 1;
@RULES
_xNIL <- forgot @@
@POST
  N("sem") = N("stem") = "forget";
  N("-en") = 1;
@RULES
_xNIL <- forgotten @@
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
  N("sem") = N("stem") = "forgo";
  N("-ed") = 1;
@RULES
_xNIL <- forwent @@
@POST
  N("sem") = N("stem") = "forgo";
  N("-en") = 1;
@RULES
_xNIL <- forgone @@
@POST
  N("sem") = N("stem") = "forswear";
  N("-ed") = 1;
@RULES
_xNIL <- forswore @@
@POST
  N("sem") = N("stem") = "forswear";
  N("-en") = 1;
@RULES
_xNIL <- forsworn @@
@POST
  N("sem") = N("stem") = "freeze";
  N("-ed") = 1;
@RULES
_xNIL <- froze @@
@POST
  N("sem") = N("stem") = "freeze";
  N("-en") = 1;
@RULES
_xNIL <- frozen @@
@POST
  N("sem") = N("stem") = "gainsay";
  N("-edn") = 1;
@RULES
_xNIL <- gainsaid @@
# getting listed as noun, can't corroborate.
@POST
  N("pos num") = 1;
  N("verb") = 1;
  N("sem") = N("stem") = "get";
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
_xNIL <- _xWILD [one match=(get gets)] @@
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
# goes - Erroneously listed as NOUN.
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
@POST
  # Ambiguous grind/ground.
  N("sem") = N("stem") = "grind";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- ground @@
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
  N("sem") = N("stem") = "hamstring";
  N("-edn") = 1;
@RULES
_xNIL <- hamstrung @@
@POST
  N("sem") = N("stem") = "hang";
  N("-edn") = 1;
@RULES
_xNIL <- hanged @@
@POST
  N("sem") = "hang";
  N("stem") = "hang";
  N("-edn") = 1;
@RULES
_xNIL <- hung @@
@POST
  N("sem") = N("stem") = "hear";
  N("-edn") = 1;
@RULES
_xNIL <- heard @@
@POST
  N("sem") = N("stem") = "heave";
  N("-edn") = 1;
@RULES
_xNIL <- hove @@
@POST
  N("sem") = N("stem") = "hew";
  N("-en") = 1;
@RULES
_xNIL <- hewn @@
@POST
  N("sem") = N("stem") = "hide";
  N("-edn") = 1;
@RULES
_xNIL <- hid @@
@POST
  N("sem") = N("stem") = "hide";
  N("-en") = 1;
@RULES
_xNIL <- hidden @@
@POST
  N("sem") = N("stem") = "hit";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- hit @@
@POST
  N("sem") = N("stem") = "hold";
  N("-edn") = 1;
@RULES
_xNIL <- held @@
@POST
  N("sem") = N("stem") = "hurt";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- hurt @@
@POST
  N("sem") = N("stem") = "inset";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- inset @@
@POST
  N("sem") = N("stem") = "keep";
  N("-edn") = 1;
@RULES
_xNIL <- kept @@
@POST
  N("sem") = N("stem") = "kneel";
  N("-edn") = 1;
@RULES
_xNIL <- knelt @@
@POST
  N("sem") = N("stem") = "knit";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- knit @@
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
  N("sem") = N("stem") = "lead";
  N("-edn") = 1;
@RULES
_xNIL <- led @@
@POST
  N("sem") = N("stem") = "lean";
  N("-edn") = 1;
@RULES
_xNIL <- leant @@
@POST
  N("sem") = N("stem") = "leap";
  N("-edn") = 1;
@RULES
_xNIL <- leapt @@
@POST
  N("sem") = N("stem") = "learn";
  N("-edn") = 1;
@RULES
_xNIL <- learnt @@
@POST
  N("sem") = N("stem") = "lend";
  N("-edn") = 1;
@RULES
_xNIL <- lent @@
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
  # Note: Three overlapping verbs here..to lie to lay.
#  N("sem") = N("stem") = "lie";
  N("inf") = 1;
  N("-ed") = 1;
@RULES
_xNIL <- lay @@
@POST
  N("sem") = N("stem") = "lay";
  N("-edn") = 1;
@RULES
_xNIL <- laid @@
@POST
  N("sem") = N("stem") = "leave";
  N("-edn") = 1;
@RULES
_xNIL <- left @@	# left
@POST
  N("sem") = N("stem") = "lie";
  N("-en") = 1;
@RULES
_xNIL <- lain @@
@POST
  N("sem") = N("stem") = "lie";
  N("-edn") = 1;
@RULES
_xNIL <- lied @@
@POST
  N("sem") = N("stem") = "light";
  N("-edn") = 1;
@RULES
_xNIL <- lit @@

@POST
  N("sem") = N("stem") = "lose";
  N("noun") = 1;
  N("adj") = 1;
  N("verb") = 1;
  N("pos num") = 3;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- lost @@
@POST
  N("sem") = N("stem") = "make";
  N("-edn") = 1;
@RULES
_xNIL <- made @@
@POST
  N("sem") = N("stem") = "mean";
  N("-edn") = 1;
@RULES
_xNIL <- meant @@
@POST
  N("sem") = N("stem") = "meet";
  N("-edn") = 1;
@RULES
_xNIL <- met @@
@POST
  N("sem") = N("stem") = "miscast";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- miscast @@
@POST
  N("sem") = N("stem") = "misdeal";
  N("-edn") = 1;
@RULES
_xNIL <- misdealt @@
@POST
  N("sem") = N("stem") = "misgive";
  N("-ed") = 1;
@RULES
_xNIL <- misgave @@
@POST
  N("sem") = N("stem") = "misgive";
  N("-en") = 1;
@RULES
_xNIL <- misgiven @@
@POST
  N("sem") = N("stem") = "mishear";
  N("-edn") = 1;
@RULES
_xNIL <- misheard @@
@POST
  N("sem") = N("stem") = "mislay";
  N("-edn") = 1;
@RULES
_xNIL <- mislaid @@
@POST
  N("sem") = N("stem") = "mislead";
  N("-edn") = 1;
@RULES
_xNIL <- misled @@
@POST
  N("sem") = N("stem") = "misspell";
  N("-edn") = 1;
@RULES
_xNIL <- misspelt @@
@POST
  N("sem") = N("stem") = "misspend";
  N("-edn") = 1;
@RULES
_xNIL <- misspent @@
@POST
  N("sem") = N("stem") = "mistake";
  N("-ed") = 1;
@RULES
_xNIL <- mistook @@
@POST
  N("sem") = N("stem") = "mistake";
  N("-en") = 1;
@RULES
_xNIL <- mistaken @@
@POST
  N("sem") = N("stem") = "misunderstand";
  N("-edn") = 1;
@RULES
_xNIL <- misunderstood @@
@POST
  N("sem") = N("stem") = "mow";
  N("-en") = 1;
@RULES
_xNIL <- mown @@
@POST
  N("sem") = N("stem") = "offset";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- offset @@
@POST
  N("sem") = N("stem") = "outbid";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- outbid @@
@POST
  N("sem") = N("stem") = "outbid";
  N("-en") = 1;
@RULES
_xNIL <- outbidden @@
@POST
  N("sem") = N("stem") = "outdo";
  N("-ed") = 1;
@RULES
_xNIL <- outdid @@
@POST
  N("sem") = N("stem") = "outdo";
  N("-en") = 1;
@RULES
_xNIL <- outdone @@
@POST
  N("sem") = N("stem") = "outfight";
  N("-edn") = 1;
@RULES
_xNIL <- outfought @@
@POST
  N("sem") = N("stem") = "outgrow";
  N("-ed") = 1;
@RULES
_xNIL <- outgrew @@
@POST
  N("sem") = N("stem") = "outgrow";
  N("-en") = 1;
@RULES
_xNIL <- outgrown @@
@POST
  N("sem") = N("stem") = "outrun";
  N("-ed") = 1;
@RULES
_xNIL <- outran @@
@POST
  N("sem") = N("stem") = "outrun";
  N("-en") = 1;
@RULES
_xNIL <- outrun @@
@POST
  N("sem") = N("stem") = "outshine";
  N("-edn") = 1;
@RULES
_xNIL <- outshone @@
@POST
  N("sem") = N("stem") = "overbear";
  N("-ed") = 1;
@RULES
_xNIL <- overbore @@
@POST
  N("sem") = N("stem") = "overbear";
  N("-en") = 1;
@RULES
_xNIL <- overborne @@
@POST
  N("sem") = N("stem") = "overcast";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- overcast @@
@POST
  N("sem") = N("stem") = "overdo";
  N("-ed") = 1;
@RULES
_xNIL <- overdid @@
@POST
  N("sem") = N("stem") = "overdo";
  N("-en") = 1;
@RULES
_xNIL <- overdone @@
@POST
  N("sem") = N("stem") = "overeat";
  N("-ed") = 1;
@RULES
_xNIL <- overate @@
@POST
  N("sem") = N("stem") = "overeat";
  N("-en") = 1;
@RULES
_xNIL <- overeaten @@
@POST
  N("sem") = N("stem") = "overfeed";
  N("-edn") = 1;
@RULES
_xNIL <- overfed @@
@POST
  N("sem") = N("stem") = "overhang";
  N("-edn") = 1;
@RULES
_xNIL <- overhung @@
@POST
  N("sem") = N("stem") = "override";
  N("-ed") = 1;
@RULES
_xNIL <- overrode @@
@POST
  N("sem") = N("stem") = "override";
  N("-en") = 1;
@RULES
_xNIL <- overridden @@
@POST
  N("sem") = N("stem") = "overrun";
  N("inf") = 1;
  N("-en") = 1;
@RULES
_xNIL <- overrun @@
@POST
  N("sem") = N("stem") = "overrun";
  N("-ed") = 1;
@RULES
_xNIL <- overran @@
@POST
  N("sem") = N("stem") = "oversee";
  N("-ed") = 1;
@RULES
_xNIL <- oversaw @@
@POST
  N("sem") = N("stem") = "oversee";
  N("-en") = 1;
@RULES
_xNIL <- overseen @@
@POST
  N("sem") = N("stem") = "overshoot";
  N("-edn") = 1;
@RULES
_xNIL <- overshot @@
@POST
  N("sem") = N("stem") = "oversleep";
  N("-edn") = 1;
@RULES
_xNIL <- overslept @@
@POST
  N("sem") = N("stem") = "overtake";
  N("-ed") = 1;
@RULES
_xNIL <- overtook @@
@POST
  N("sem") = N("stem") = "overtake";
  N("-edn") = 1;
@RULES
_xNIL <- overtaken @@

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
  N("sem") = N("stem") = "partake";
  N("-ed") = 1;
@RULES
_xNIL <- partook @@
@POST
  N("sem") = N("stem") = "partake";
  N("-en") = 1;
@RULES
_xNIL <- partaken @@
@POST
  N("stem") = N("sem") = "pay";
  N("-edn") = 1;
@RULES
_xNIL <- paid @@
@POST
  N("sem") = N("stem") = "plead";
  N("-edn") = 1;
@RULES
_xNIL <- pled @@
@POST
  N("sem") = N("stem") = "prove";
  N("-edn") = 1;
@RULES
_xNIL <- proved @@
@POST
  N("sem") = N("stem") = "prove";
  N("-en") = 1;
@RULES
_xNIL <- proven @@
@POST
  N("sem") = N("stem") = "put";
  N("noun") = 1;
  N("verb") = 1;
  N("pos num") = 2;
  N("-edn") = 1;
  N("inf") = 1;
@RULES
_xNIL <- put @@
@POST
  N("sem") = N("stem") = "quit";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- quit @@
@POST
  N("sem") = N("stem") = "read";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- read @@
@POST
  N("sem") = N("stem") = "rebind";
  N("-edn") = 1;
@RULES
_xNIL <- rebound @@
@POST
  N("sem") = N("stem") = "rebuild";
  N("-edn") = 1;
@RULES
_xNIL <- rebuilt @@
@POST
  N("sem") = N("stem") = "recast";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- recast @@
@POST
  N("sem") = N("stem") = "redo";
  N("-ed") = 1;
@RULES
_xNIL <- redid @@
@POST
  N("sem") = N("stem") = "redo";
  N("-en") = 1;
@RULES
_xNIL <- redone @@
@POST
  N("sem") = N("stem") = "relay";
  N("-edn") = 1;
@RULES
_xNIL <- relaid @@
@POST
  N("sem") = N("stem") = "remake";
  N("-edn") = 1;
@RULES
_xNIL <- remade @@
@POST
  # Ambiguous rend/rent.
  N("sem") = N("stem") = "rend";
  N("-edn") = 1;
@RULES
_xNIL <- rent @@
@POST
  N("stem") = N("sem") = "repay";
  N("-edn") = 1;
@RULES
_xNIL <- repaid @@
@POST
  N("sem") = N("stem") = "reread";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- reread @@
@POST
  N("sem") = N("stem") = "rerun";
  N("inf") = 1;
  N("-en") = 1;
@RULES
_xNIL <- rerun @@
@POST
  N("sem") = N("stem") = "rerun";
  N("-ed") = 1;
@RULES
_xNIL <- reran @@
@POST
  N("sem") = N("stem") = "restring";
  N("-edn") = 1;
@RULES
_xNIL <- restrung @@
@POST
  N("sem") = N("stem") = "retell";
  N("-edn") = 1;
@RULES
_xNIL <- retold @@
@POST
  N("sem") = N("stem") = "rethink";
  N("-edn") = 1;
@RULES
_xNIL <- rethought @@
@POST
  N("sem") = N("stem") = "rewind";
  N("-edn") = 1;
@RULES
_xNIL <- rewound @@
@POST
  N("sem") = N("stem") = "rewrite";
  N("-ed") = 1;
@RULES
_xNIL <- rewrote @@
@POST
  N("sem") = N("stem") = "rewrite";
  N("-en") = 1;
@RULES
_xNIL <- rewritten @@
@POST
  N("sem") = N("stem") = "rid";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- rid @@
@POST
  N("sem") = N("stem") = "ride";
  N("-ed") = 1;
@RULES
_xNIL <- rode @@
@POST
  N("sem") = N("stem") = "ride";
  N("-en") = 1;
@RULES
_xNIL <- ridden @@
@POST
  N("sem") = N("stem") = "ring";
  N("-ed") = 1;
@RULES
_xNIL <- rang @@
@POST
  # Noun too..
  N("nsem") = N("nstem") = "rung";
  N("vsem") = N("vstem") = "ring";
  N("-en") = 1;
@RULES
_xNIL <- rung @@
@POST
  N("sem") = N("stem") = "reset";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- reset @@
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

# SAW -- ambiguous word...
@POST
  N("sem") = N("stem") = "see";
  N("-ed") = 1;
@RULES
_xNIL <- saw @@
@POST
  N("sem") = N("stem") = "saw";
  N("-edn") = 1;
@RULES
_xNIL <- sawn @@
@POST
  N("sem") = N("stem") = "say";
  N("-edn") = 1;
@RULES
_xNIL <- said @@
@POST
  N("sem") = N("stem") = "see";
  N("-en") = "seen";
@RULES
_xNIL <- seen @@
@POST
  N("sem") = N("stem") = "sell";
  N("-edn") = 1;
@RULES
_xNIL <- sold @@
@POST
  N("sem") = N("stem") = "send";
  N("-edn") = 1;
@RULES
_xNIL <- sent @@
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
  N("sem") = N("stem") = "seek";
  N("-edn") = 1;
@RULES
_xNIL <- sought @@
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
  N("sem") = N("stem") = "shave";
  N("-en") = 1;
@RULES
_xNIL <- shaven @@
@POST
  N("sem") = N("stem") = "shear";
  N("-en") = 1;
@RULES
_xNIL <- shorn @@
@POST
  N("sem") = N("stem") = "shed";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- shed @@
@POST
  N("sem") = N("stem") = "shew";
  N("-ed") = 1;
@RULES
_xNIL <- shewed @@
@POST
  N("sem") = N("stem") = "shew";
  N("-en") = 1;
@RULES
_xNIL <- shewn @@
@POST
  N("sem") = N("stem") = "shit";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- shit @@
@POST
  N("sem") = N("stem") = "shit";
  N("-ed") = 1;
@RULES
_xNIL <- shat @@
@POST
  N("sem") = N("stem") = "shine";
  N("-edn") = 1;
@RULES
_xNIL <- shone @@
@POST
  N("sem") = N("stem") = "shoe";
  N("-edn") = 1;
@RULES
_xNIL <- shod @@
@POST
  N("sem") = N("stem") = "shoot";
  N("-edn") = 1;
@RULES
_xNIL <- shot @@
@POST
  N("sem") = N("stem") = "show";
  N("-en") = 1;
@RULES
_xNIL <- shown @@
@POST
  N("sem") = N("stem") = "shred";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- shred @@
@POST
  N("sem") = N("stem") = "shrink";
  N("-ed") = 1;
@RULES
_xNIL <- shrank @@
@POST
  N("sem") = N("stem") = "shrink";
  N("-edn") = 1;	# -ed informal.
@RULES
_xNIL <- shrunk @@
@POST
  N("sem") = N("stem") = "shrive";
  N("-ed") = 1;
@RULES
_xNIL <- shrove @@
@POST
  N("sem") = N("stem") = "shrive";
  N("-en") = 1;
@RULES
_xNIL <- shriven @@
@POST
  N("sem") = N("stem") = "shut";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- shut @@
@POST
  N("sem") = N("stem") = "sing";
  N("-ed") = 1;
@RULES
_xNIL <- sang @@
@POST
  N("sem") = N("stem") = "sing";
  N("-en") = 1;
@RULES
_xNIL <- sung @@
@POST
  N("sem") = N("stem") = "sink";
  N("-en") = 1;
@RULES
_xNIL <- sank @@
@POST
  N("sem") = N("stem") = "sink";
  N("-en") = 1;
@RULES
_xNIL <- sunk @@
@POST
  N("sem") = N("stem") = "sit";
  N("-edn") = 1;
@RULES
_xNIL <- sat @@
@POST
  N("sem") = N("stem") = "slay";
  N("-ed") = 1;
@RULES
_xNIL <- slew @@
@POST
  N("sem") = N("stem") = "slay";
  N("-en") = 1;
@RULES
_xNIL <- slain @@
@POST
  N("sem") = N("stem") = "sleep";
  N("-edn") = 1;
@RULES
_xNIL <- slept @@
@POST
  N("sem") = N("stem") = "slide";
  N("-edn") = 1;
@RULES
_xNIL <- slid @@
@POST
  N("sem") = N("stem") = "sling";
  N("-edn") = 1;
@RULES
_xNIL <- slung @@
@POST
  N("sem") = N("stem") = "slink";
  N("-edn") = 1;
@RULES
_xNIL <- slunk @@
@POST
  N("sem") = N("stem") = "slit";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- slit @@
@POST
  # Ambig: smelt is a noun too.
  N("sem") = N("stem") = "smell";
  N("-edn") = 1;
@RULES
_xNIL <- smelt @@
@POST
  N("sem") = N("stem") = "smite";
  N("-ed") = 1;
@RULES
_xNIL <- smote @@
@POST
  N("sem") = N("stem") = "smite";
  N("-en") = 1;
@RULES
_xNIL <- smitten @@
@POST
  N("sem") = N("stem") = "soft";
  N("-er") = 1;
@RULES
_xNIL <- softer @@
@POST
  N("sem") = N("stem") = "soft";
  N("-est") = 1;
@RULES
_xNIL <- softest @@
@POST
  N("sem") = N("stem") = "sow";
  N("-en") = 1;
@RULES
_xNIL <- sown @@
@POST
  # NOTE: spoke is also a noun.....
  N("nsem") = N("nstem") = "spoke";
  N("vsem") = N("vstem") = "speak";
  N("-ed") = 1;
@RULES
_xNIL <- spoke @@
@POST
  N("sem") = N("stem") = "speak";
  N("-en") = 1;
@RULES
_xNIL <- spoken @@
@POST
  N("sem") = N("stem") = "speed";
  N("-edn") = 1;
@RULES
_xNIL <- sped @@
@POST
  N("sem") = N("stem") = "spell";
  N("-edn") = 1;
@RULES
_xNIL <- spelt @@
@POST
  N("sem") = N("stem") = "spend";
  N("-edn") = 1;
@RULES
_xNIL <- spent @@
@POST
  N("sem") = N("stem") = "spill";
  N("-edn") = 1;
@RULES
_xNIL <- spilt @@
@POST
  # Maybe BrE.
  # Ambig: span is noun.
  N("sem") = N("stem") = "spin";
  N("-ed") = 1;
@RULES
_xNIL <- span @@
@POST
  N("sem") = N("stem") = "spin";
  N("-edn") = 1;
@RULES
_xNIL <- spun @@
@POST # Ambig.
  N("sem") = N("stem") = "spit";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- spit @@
@POST # Ambig.
  N("sem") = N("stem") = "spit";
  N("-edn") = 1;
@RULES
_xNIL <- spat @@
@POST
  N("sem") = N("stem") = "split";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- split @@
@POST
  N("sem") = N("stem") = "spoil";
  N("-edn") = 1;
@RULES
_xNIL <- spoilt @@
@POST
  N("sem") = N("stem") = "spread";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- spread @@
@POST
  N("sem") = N("stem") = "spring";
  N("-ed") = 1;
@RULES
_xNIL <- sprang @@
@POST
  N("sem") = N("stem") = "spring";
  N("-edn") = 1;
@RULES
_xNIL <- sprung @@
@POST
  N("sem") = N("stem") = "stand";
  N("-edn") = 1;
@RULES
_xNIL <- stood @@
@POST # Ambig.
  N("sem") = N("stem") = "steal";
  N("-ed") = 1;
@RULES
_xNIL <- stole @@
@POST
  N("sem") = N("stem") = "steal";
  N("-en") = 1;
@RULES
_xNIL <- stolen @@
@POST
  N("sem") = N("stem") = "stick";
  N("-edn") = 1;
@RULES
_xNIL <- stuck @@
@POST
  N("sem") = N("stem") = "sting";
  N("-edn") = 1;
@RULES
_xNIL <- stung @@
@POST
  N("sem") = N("stem") = "stink";
  N("-ed") = 1;
@RULES
_xNIL <- stank @@
@POST
  N("sem") = N("stem") = "stink";
  N("-edn") = 1;
@RULES
_xNIL <- stunk @@
@POST
  N("sem") = N("stem") = "strew";
  N("-en") = 1;
@RULES
_xNIL <- strewn @@
@POST
  N("sem") = N("stem") = "stride";
  N("-en") = 1;
@RULES
_xNIL <- strid @@
@POST
  N("sem") = N("stem") = "stride";
  N("-en") = 1;
@RULES
_xNIL <- stridden @@
@POST
  N("sem") = N("stem") = "stride";
  N("-edn") = 1;
@RULES
_xNIL <- strode @@
@POST
  N("sem") = N("stem") = "strike";
  N("-edn") = 1;
@RULES
_xNIL <- struck @@
@POST
  N("sem") = N("stem") = "string";
  N("-edn") = 1;
@RULES
_xNIL <- strung @@
@POST
  N("sem") = N("stem") = "strive";
  N("-ed") = 1;
@RULES
_xNIL <- strove @@
@POST
  N("sem") = N("stem") = "strive";
  N("-en") = 1;
@RULES
_xNIL <- striven @@
@POST
  N("sem") = N("stem") = "swear";
  N("-ed") = 1;
@RULES
_xNIL <- swore @@
@POST
  N("sem") = N("stem") = "swear";
  N("-en") = 1;
@RULES
_xNIL <- sworn @@
#@POST
#  N("sem") = N("stem") = "sweat";
#  N("inf") = 1;
#  N("-edn") = 1; # Maybe BrE
#@RULES
#_xNIL <- sweat @@
@POST
  N("sem") = N("stem") = "sweep";
  N("-edn") = 1;
@RULES
_xNIL <- swept @@
@POST
  N("sem") = N("stem") = "swell";
  N("-edn") = 1;
@RULES
_xNIL <- swollen @@
@POST
  N("sem") = N("stem") = "swim";
  N("-ed") = 1;
@RULES
_xNIL <- swam @@
@POST
  N("sem") = N("stem") = "swim";
  N("-en") = 1;	# -ed maybe BrE
@RULES
_xNIL <- swum @@
@POST
  N("sem") = N("stem") = "swing";
  N("-edn") = 1;
@RULES
_xNIL <- swung @@
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
  N("sem") = N("stem") = "teach";
  N("-edn") = 1;
@RULES
_xNIL <- taught @@
@POST
  N("sem") = N("stem") = "tear";
  N("-ed") = 1;
@RULES
_xNIL <- tore @@
@POST
  N("sem") = N("stem") = "tear";
  N("-en") = 1;
@RULES
_xNIL <- torn @@
@POST
  N("sem") = N("stem") = "telecast";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- telecast @@
@POST
  N("sem") = N("stem") = "tell";
  N("-edn") = 1;
@RULES
_xNIL <- told @@
@POST
  N("sem") = N("stem") = "think";
  N("-edn") = 1;
@RULES
_xNIL <- thought @@
@POST
  N("sem") = N("stem") = "thrive";
  N("-ed") = 1;
@RULES
_xNIL <- throve @@	# Maybe BrE
@POST
  N("sem") = N("stem") = "thrive";
  N("-en") = 1;
@RULES
_xNIL <- thriven @@	# Maybe BrE
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
  N("sem") = N("stem") = "thrust";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- thrust @@
@POST
  N("sem") = N("stem") = "tread";
  N("-edn") = 1;
@RULES
_xNIL <- trod @@
@POST
  N("sem") = N("stem") = "tread";
  N("-en") = 1;
@RULES
_xNIL <- trodden @@
@POST
  N("sem") = N("stem") = "unbend";
  N("-edn") = 1;
@RULES
_xNIL <- unbent @@
@POST
  N("sem") = N("stem") = "unbind";
  N("-edn") = 1;
@RULES
_xNIL <- unbound @@
@POST
  N("sem") = N("stem") = "underbid";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- underbid @@
@POST
  N("sem") = N("stem") = "underbid";
  N("-en") = 1;
@RULES
_xNIL <- underbidden @@
@POST
  N("sem") = N("stem") = "undergo";
  N("-ed") = 1;
@RULES
_xNIL <- underwent @@
@POST
  N("sem") = N("stem") = "undergo";
  N("-en") = 1;
@RULES
_xNIL <- undergone @@
@POST
  N("sem") = N("stem") = "understand";
  N("-edn") = 1;
@RULES
_xNIL <- understood @@
@POST
  N("sem") = N("stem") = "undertake";
  N("-ed") = 1;
@RULES
_xNIL <- undertook @@
@POST
  N("sem") = N("stem") = "undertake";
  N("-en") = 1;
@RULES
_xNIL <- undertaken @@
@POST
  N("sem") = N("stem") = "underwrite";
  N("-ed") = 1;
@RULES
_xNIL <- underwrote @@
@POST
  N("sem") = N("stem") = "underwrite";
  N("-en") = 1;
@RULES
_xNIL <- underwritten @@
@POST
  N("sem") = N("stem") = "undo";
  N("-ed") = 1;
@RULES
_xNIL <- undid @@
@POST
  N("sem") = N("stem") = "undo";
  N("-en") = 1;
@RULES
_xNIL <- undone @@
@POST
  N("sem") = N("stem") = "unfreeze";
  N("-ed") = 1;
@RULES
_xNIL <- unfroze @@
@POST
  N("sem") = N("stem") = "unfreeze";
  N("-en") = 1;
@RULES
_xNIL <- unfrozen @@
@POST
  N("sem") = N("stem") = "unmake";
  N("-edn") = 1;
@RULES
_xNIL <- unmade @@
@POST
  N("sem") = N("stem") = "unwind";
  N("-edn") = 1;
@RULES
_xNIL <- unwound @@
@POST
  N("sem") = N("stem") = "uphold";
  N("-edn") = 1;
@RULES
_xNIL <- upheld @@
@POST
  N("sem") = N("stem") = "upset";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- upset @@
@POST
  N("sem") = N("stem") = "wake";
  N("-ed") = 1;
@RULES
_xNIL <- woke @@
@POST
  N("sem") = N("stem") = "wake";
  N("-en") = 1;
@RULES
_xNIL <- woken @@
@POST
  N("sem") = N("stem") = "waylay";
  N("-edn") = 1;
@RULES
_xNIL <- waylaid @@
@POST
  N("sem") = N("stem") = "wear";
  N("-ed") = 1;
@RULES
_xNIL <- wore @@
@POST
  N("sem") = N("stem") = "wear";
  N("-en") = 1;
@RULES
_xNIL <- worn @@
@POST
  N("sem") = N("stem") = "weave";
  N("-ed") = 1;
@RULES
_xNIL <- wove @@
@POST
  N("sem") = N("stem") = "weave";
  N("-en") = 1;
@RULES
_xNIL <- woven @@
@POST
  N("sem") = N("stem") = "wed";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- wed @@
@POST
  N("sem") = N("stem") = "weep";
  N("-edn") = 1;
@RULES
_xNIL <- wept @@
@POST
  N("sem") = N("stem") = "wet";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- wet @@
@POST
  N("sem") = N("stem") = "win";
  N("-edn") = 1;
@RULES
_xNIL <- won @@
@POST
  # Ambiguous: wind/wound.
  N("sem") = N("stem") = "wind";
  N("inf") = 1;
  N("-edn") = 1;
@RULES
_xNIL <- wound @@
@POST
  N("sem") = N("stem") = "wound";
  N("-edn") = 1;
@RULES
_xNIL <- wounded @@
@POST
  N("sem") = N("stem") = "withdraw";
  N("-en") = 1;
@RULES
_xNIL <- withdrawn @@
@POST
  N("sem") = N("stem") = "withdraw";
  N("-edn") = 1;
@RULES
_xNIL <- withdrew @@
@POST
  N("sem") = N("stem") = "withhold";
  N("-edn") = 1;
@RULES
_xNIL <- withheld @@
@POST
  N("sem") = N("stem") = "withstand";
  N("-edn") = 1;
@RULES
_xNIL <- withstood @@
@POST
  N("sem") = N("stem") = "wring";
  N("-edn") = 1;
@RULES
_xNIL <- wrung @@
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


#@POST
#  N("sem") = N("stem") = "xxx";
#  N("-edn") = 1;
#@RULES
#_xNIL <- xxx @@
#


@POST
  N("sem") = "person";
  N("stem") = "person";
  N("number") = "plural";
  N("inf") = 1;
@RULES
_xNIL <- people @@
@POST
  N("sem") = "salaryman";
  N("stem") = "salaryman";
  N("number") = "plural";
@RULES
_xNIL <- salarymen @@
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
  N("stem") = N("sem") = "tv";
  N("pos num") = 1;
  N("noun") = 1;
  N("pos") = "_noun";
  N("number") = "singular";
@RULES
_xNIL <- tv @@

@POST
  N("pos num") = 3;
  N("adj") = 1;
  N("noun") = 1;
  N("interjection") = 1;
  N("pos num") = 3;
  N("pos") = "_adj";
  N("number") = "singular";
@RULES
_xNIL <- ok @@

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
@PRE
<1,1> cap();
@POST
  pncopyvars(1);
  S("sem detail") = S("stem") = strtolower(phrasetext());
  S("sem") = "nationality";
  S("ne") = 1;
  S("ne type") = "location";
  S("ne type conf") = 95;
  S("cap") = 1;
  S("mypos") = "NP";
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
    	++N("cap",1);
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
  ++X("date ref");
@RULES	# FIX.  # 02/12/02 AM.
_xNIL <- _xWILD [s one match=(
	January February March April May June July August
	September October November December
	
	Jan Feb Mar Apr May June Jul Aug Sep Sept Oct Nov Dec
	)] @@
#!JPW TODO Need to do better with March/April/May/June/August ambiguity.
