###############################################
# FILE: hilite_undone.pat
# SUBJ: Highlight alphas and segments to show unmarked pos.
# AUTH: AM
# CREATED: 08/Jul/04 01:28:25
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

@POST
 X("stuff-in-clause") = 1;
 noop();
@RULES
_xNIL <- _xWILD [one match=(
	_xALPHA _seg _tok _aposS
	_noun _verb _adj _adv _premod
	_interj
#	_prep
	_xNUM _xPUNCT
	)] @@

# Funny things to check on.
_xNIL <-
	_vg
	_vg
	@@

