###############################################
# FILE: qline100.pat
# SUBJ: Examine lines.
# AUTH: AM
# CREATED: 07/Jul/04 23:23:39
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _LINE

# Looking for numbered lines.
@POST
  N("nopos",3) = 1;
  L("num") = num(N("$text",2));
  if (!G("num lines"))
    {
	if (L("num") == 1)
    G("num lines") = 1;
	xrename("_TEXTZONE");
	}
  else if (
      (L("num") == G("num lines"))
   || (L("num") == G("num lines") + 1) )
   {
   G("num lines") = L("num");
   xrename("_TEXTZONE");
   }
@RULES
_xNIL <-
	_xSTART
	_xNUM
	\. [gp=_qEOS]
	@@


@POST
  xrename("_HORIZLINE");
@RULES
_xNIL <-
	_xSTART
	_xWHITE [star]
	\- [plus]
	_xWHITE [star]
	_xEND
	@@
