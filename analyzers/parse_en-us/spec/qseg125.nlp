###############################################
# FILE: qseg125.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 14/Jun/06 08:27:30
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _sent _seg
@NODES _seg

# alpha alpha alpha.
# Could still want to allow first or last to be separated as
# a verb.
@CHECK
  if (X("resolve") != "np")
    fail();
@POST
  fixnpnonhead(2);
  fixnpnonhead(3);
  xrename("_np");

  # fixnphead
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnoun(N(4));

@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xALPHA
	_xEND
	@@
