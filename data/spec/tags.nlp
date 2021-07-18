###############################################
# FILE: tags.pat
# SUBJ: Process html constructs.
# AUTH: AM
# CREATED: 08/Jun/05 21:49:49
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _ROOT

@POST
  L("nm") = strtolower(N("$text",3));
  L("name") = strsubst(L("nm"),"_","uu");
  if (N(2))
     L("tag") = "_E" + L("name");
  else
     L("tag") = "_" + L("name");
  group(1,5,L("tag"));
  N("name",1) = L("name");
@RULES
_xNIL <-
	\<
	\/ [opt]
	_xWILD [plus match=(_xALPHA \_ ) gp=_name]
	_xWILD
	\>
	@@

@POST
 group(1,4,"_xmltag");
@RULES
_xNIL <-
	\<
	\?
	_xWILD
	\>
	@@

