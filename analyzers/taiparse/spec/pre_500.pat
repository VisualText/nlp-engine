###############################################
# FILE: pre_500.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 15/Jul/04 16:33:21
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@PATH _ROOT _LINE _pos _text

@POST
  noop();
@RULES
_xNIL <-
	_xWHITE [plus]
	@@

@POST
  if (X("posarr len",3))
    {
    N("posarr") = X("posarr",3);
	N("posarr len") = X("posarr len",3);
	}
  else if (X("nopos",3))
    N("nopos") = 1;
  # Tokenize multi-element texts.
  if (pnnext(N(1)))
    {
    xrename("_tok");
	X("posarr",4) = X("posarr",3);
	X("posarr len",4) = X("posarr len",3);
	# Still scoring on tokens!	# 10/16/04 AM.
#	X("nopos",4) = 1;
	}
@RULES
_xNIL <-
	_xANY
	@@
