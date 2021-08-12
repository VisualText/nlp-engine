###############################################
# FILE: feat.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 11/Jul/04 01:45:06
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE

@RULES
# Some non -ed exceptions.
# Some non -ing exceptions.
@POST
  N("inf") = 1;	# Not a verb+ed.
  S("stem") = strtolower(N("$text"));
  noop();
@RULES
_xNIL <- _xWILD [s one match=(
	need heed deed feed reed seed weed
	bleed breed speed
	exceed proceed overseed reseed screed succeed
	bed bobsled embed imbed inbreed outbreed overfeed
	refeed red shred sled sned ted woodshed
	shed wed  # These CAN be -ed.
	)] @@

_xNIL <- _xWILD [s one match=(
	bring cling ding fling king outring outsing ping
	respring restring ring sing sling spring sting
	string swing ting unsling unstring upfling
	upspring upswing wing wring zing
	)] @@
