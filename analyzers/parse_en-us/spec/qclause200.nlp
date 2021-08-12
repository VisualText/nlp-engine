###############################################
# FILE: qclause200.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 06/Oct/04 22:54:32
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _clause

@POST
  singler(1,2);
@RULES
_advl <-
	_prep
	_xWILD [s one match=(_np) except=(_proSubj)]
	_xWILD [one lookahead fail=(_conj)]
	@@

# Fixing up question/query/interrrogative patterns
# to look like declarative patterns.	# 05/10/06 AM.
@CHECK
  if (N("q-q remote",3) != "right")
   fail();
@POST
  X("q-q excised") = N("q-q excised",7) = 1;

  # Excise the discontinuous interrogative.
  # So that clause pattern can look the same
  # as declarative, eg, nvn.
  --X("vg count");
  X("vg node") = N(7);
  if (X("first vg") == N(3))
    X("first vg") = N(7);
  if (X("first name") == "_vg")
    X("first name") = "_np";  
  N("first verb",7) = 0;	# Avoid VT CRASH.

  # Rather than excising, convert to an adverbial! # 06/02/06 AM.
  # This enables pos tagger to record verb parts of speech.
#  excise(3,3);
  pnrename(N(3),"_advl");	# verb -> advl !
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

