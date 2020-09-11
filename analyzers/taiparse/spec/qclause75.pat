###############################################
# FILE: qclause75.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 03/Jun/06 13:31:00
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

# there vg
@POST
  if (pnname(N(3)) == "_np")
    L("n") = pndown(N(3));
  else
    L("n") = N(3);
  chpos(L("n"),"EX");
  if (pnname(N(5)) == "_vg")
    if (!N("voice",5))
	  X("voice") = N("voice",5) = "active";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	there [s]
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

# there
@CHECK
  if (N("there"))
    fail();	# Loop guard.
@POST
  if (pnname(N(1)) == "_np")
    L("n") = pndown(N(1));
  else
    L("n") = N(1);
  if (!pnvar(L("n"),"mypos"))
    chpos(L("n"),"RB");
  N("there") = 1;
@RULES
_xNIL <-
	there [s]
	@@

# up
# down
# out
# Not otherwise assigned.
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"RB");
@RULES
_xNIL <-
	_xWILD [s one match=(up down out)]
	_xWILD [star match=(_adv _advl)]
	_xEND
	@@

# vg advl
# vg by-actor
@PRE
<2,2> var("by-actor");
@POST
  fixvg(N(1),"passive","VBN");
@RULES
_xNIL <-
	_vg
	_advl
	@@
