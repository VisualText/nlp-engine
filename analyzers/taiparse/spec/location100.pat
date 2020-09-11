###############################################
# FILE: location100.pat
# SUBJ: Named entity knowledge.
# AUTH: AM
# CREATED: 19/Apr/05 12:17:58
# NOTE:	Generalize to named entities.
###############################################

@CODE
G("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@PRE
<2,2> cap();
@POST
  L("name") = N("$text",1);
  L("type") = N("$text",2);
  group(1,2,"_location");
  N("name",1) = L("name");
  N("type",1) = L("type");
  N("ne",1) = 1;
@RULES
_xNIL <-
	_xWILD [plus match=(_xCAP _xWHITE) gp=_name]
	_xWILD [one trigger match=(highway hwy fwy freeway trail) gp=_type]
	@@

@POST
  S("ne text") = phrasetext();
  S("ne") = 1;
  S("sem") = S("ne type") = "location";
  S("ne type conf") = 95;
  S("sem location") = "province";
  S("mypos") = "NP";
  S("stem") = phrasetext();
#  "city.txt" << phrasetext() << "\n";
  single();
@RULES

_loc [layer=(_noun)] <- _xWILD [one match=(
	Aceh
	Sumatra
	)] @@

@POST
  S("ne text") = phrasetext();
  S("ne") = 1;
  S("sem") = S("ne type") = "location";
  S("ne type conf") = 95;
  S("sem location") = "island";
  S("mypos") = "NP";
  S("stem") = phrasetext();
#  "city.txt" << phrasetext() << "\n";
  single();
@RULES

_loc [layer=(_noun)] <- 
	andaman [s]
	_xWHITE [star]
	_xWILD [s opt match=( and \& )]
	_xWHITE [star]
	nicobar [s]
	_xWHITE [star]
	_xWILD [s one match=(island islands)]
	@@

@POST
  S("ne text") = S("stem") = phrasetext();
  S("ne type") = "location";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("sem") = "location";
  S("sem location") = "ocean";
  single();
@RULES
_noun <-
	Indian
	_xWHITE [star]
	Ocean
	@@

@POST
  pncopyvars(5);
  S("ne text") = S("stem") = phrasetext();
  S("ne type") = "person";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("sem") = "person title";
  single();
@RULES
_noun <-
	Secretary
	_xWHITE [star]
	of [s]
	_xWHITE [star]
	State
	@@
@POST
  pncopyvars(3);
  S("ne text") = S("stem") = phrasetext();
  S("ne type") = "person";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("sem") = "person title";
  single();
@RULES
_noun <-
	_xWILD [one match=(Foreign Prime)]
	_xWHITE [star]
	Minister
	@@
@POST
  pncopyvars(1);
  S("ne text") = S("stem") = phrasetext();
  S("ne type") = "person";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("sem") = "person title";
  single();
@RULES
_noun <-
	Secretary
	_xWILD [star match=(_xWHITE \- )]
	General
	@@
@PRE
<1,1> cap();
@POST
  pncopyvars(1);
  S("ne text") = S("stem") = phrasetext();
  S("ne type") = "person";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("sem") = "person title";
  single();
@RULES
_noun <-
	_xWILD [one match=(President Governor Premier Commissioner
		Secretary General
		Coordinator
		Commander
	)]
	@@

### Some NEWS HEADERS here ####

@POST
  group(8,8,"_qEOS");
  L("date") = getdate(N(1));
  group(4,5,"_source");
  L("source") = N("$text",4);
  group(1,5,"_np");
  N("ne",1) = 1;
  N("ne type",1) = "newshead";
  S("ne type conf") = 95;
  N("timestamp",1) = L("date");
  N("source",1) = L("source");
@RULES
_xNIL <-
	_date [s]
	_xWHITE [star]
	_xWILD [one match=( \( \[ \{ )]
	_xCAP
	_xWILD [star match=(_xWHITE _xCAP)]
	_xWILD [one match=( \) \] \} )]
	_xWHITE [star]
	_xWILD [one match=( _dbldash \- )]
	_xWHITE [star]
	@@