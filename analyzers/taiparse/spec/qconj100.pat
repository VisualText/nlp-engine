###############################################
# FILE: qconj100.pat
# SUBJ: Conjunction study.
# AUTH: AM
# CREATED: 30/Sep/04 01:00:28
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

# np conj np
@POST
  group(2,4,"_np");
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_np
	_conj
	_np
	_xWILD [one lookahead match=(_fnword)]
	@@

@POST
  N("conj np",2) = 1; # Could bind nps.
@RULES
_xNIL <-
	_np
	_conj
	_np [lookahead]
	@@

@POST
  pncopyvars(1);
  if (N("glom",1) && N("glom",3))
    S("glom") = 0; # Can't decide.
  else if (N("glom",1) == "right")
    S("glom") = "right";
  else if (N("glom",3) == "left")
    S("glom") = "left";
  if (N("voice",1))
    S("voice") = N("voice",1); # Need voice agreement.
  S("first vg") = N(1);
  S("last vg") = N(3);
  single();
@RULES
_vg <-
	_vg
	_conj
	_vg
	@@

# parallel construction to resolve segs.	# 06/14/06 AM.
@CHECK
  if (N("resolve",7))
    fail();
@POST
  N("resolve",7) = "np";
@RULES
_xNIL <-
	_vg
	_prep
	_np
	_prep
	_np
	_conj
	_seg
	_prep [lookahead]
	_np
	@@





