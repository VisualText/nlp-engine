###############################################
# FILE: punct200.pat
# SUBJ: More eos handling, etc.
# AUTH: AM
# CREATED: 08/Jul/04 01:00:30
# MODIFIED:
###############################################

@CODE
if (!G("unhandled periods"))
  exitpass();
@@CODE

@PATH _ROOT _TEXTZONE

@POST
  listadd(5,6,"false");
@RULES
_xNIL <-
	_qEOS
	_xWHITE [star]
	_xCAP [s]
	_xWHITE [star]
	_letabbr
	\.
	@@

# Rearguard.
@POST
  ++G("unhandled periods");
  if (G("error"))
  "err.txt" << "[Unhandled period]" << "\n";
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
