###############################################
# FILE: fnretok.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 11/Jul/04 01:34:24
# MODIFIED:
###############################################

@CODE
if (G("pretagged"))
  exitpass();
@@CODE

@PATH _ROOT _TEXTZONE

@POST
  S("neg") = 1;
  S("mypos") = "VBP";
  single();
@RULES

_modal <- won \' t @@
_modal <- can \' t @@
_modal <- cannot @@
_modal <- mayn \' t @@
_modal <- mightn \' t @@
_modal <- shan \' t @@
_modal <- oughtn \' t @@
_modal <- mustn \' t @@
_modal <- don \' t @@
_modal <- doesn \' t @@

_have [layer=(_verb)] <- haven \' t @@
_have [layer=(_verb)] <- hasn \' t @@

_be [layer=(_verb)] <- aren \' t @@
_be [layer=(_verb)] <- isn \' t @@

@POST
  S("neg") = 1;
  S("mypos") = "VBD";
  single();
@RULES
_modal <- couldn \' t @@
_modal <- didn \' t @@
_modal <- shouldn \' t @@
_modal <- wouldn \' t @@

_have [layer=(_verb)] <- hadn \' t @@

_be [layer=(_verb)] <- wasn \' t @@
_be [layer=(_verb)] <- weren \' t @@
_be [layer=(_verb)] <- aren \' t @@
_be [layer=(_verb)] <- isn \' t @@

@POST
  group(1,1, "_proSubj");
  group(1,1, "_pro");
  group(2,3, "am");	# Retokenize.
@RULES
_xNIL <-
  I [s]
  \' [s]
  m [s]
  @@

# you're
@POST
  group(2,3,"_be");
  group(2,2,"_verb");
  N("sem",2) = N("stem",2) = "be";
  N("number",2) = "any";
  chpos(N(2),"VBP");
  L("tmp1") = N(1);
  group(1,1,"_proSubj");
  group(1,1,"_pro");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	you
	\'
	re
	@@
# you're
@POST
  group(2,3,"_be");
  group(2,2,"_verb");
  N("sem",2) = N("stem",2) = "be";
  N("number",2) = "plural";
  chpos(N(2),"VBP");
  L("tmp1") = N(1);
  group(1,1,"_proSubj");
  group(1,1,"_pro");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	we
	\'
	re
	@@
# you're
@POST
  group(2,3,"_be");
  group(2,2,"_verb");
  N("sem",2) = N("stem",2) = "be";
  N("number",2) = "plural";
  chpos(N(2),"VBP");
  L("tmp1") = N(1);
  group(1,1,"_proSubj");
  group(1,1,"_pro");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	they
	\'
	re
	@@

@RULES
_num <-
	_xNUM
	\' [s]
	s [s]
	@@

#@RULES
#_aposS <-
#	\' [s]
#	s [s]
#	@@

# alpha '
@POST
  N("apostrophe",1) = 1;
  excise(2,2);	# Trash the apostrophe.
@RULES
_xNIL <-
	_xALPHA [s]
	\' [trigger]
	@@


# Some mumbles.
@POST
  ++X("nmumbles");
  single();
@RULES
_mumble <-
	_xWILD [min=2 match=(_xALPHA _xNUM \_ )]
	@@

_mumble <-
	_xALPHA
	_xNUM
	@@

_noun <-
	_noun
	\/
	_noun
	@@

@RULES
_dbldash <- \- \- \- @@  # Triple!
_dbldash <- \- \- @@

@POST
  group(2,2,"_dbldash");
@RULES
_xNIL <-
	_xWHITE [s plus]
	\-
	_xWHITE [s plus]
	@@


