###############################################
# FILE: pre_400.pat
# SUBJ: Gather pretagged pos values.
# AUTH: AM
# CREATED: 15/Jul/04 16:26:11
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@PATH _ROOT _LINE _pos _slash

@POST
  X("posarr",3)[X("posarr len",3)] = N("$text",1) + "$";
  ++X("posarr len",3);
@RULES
_xNIL <-
	_xALPHA
	\$
	@@

@POST
  X("posarr",3)[X("posarr len",3)] = N("$text");
  ++X("posarr len",3);
@RULES
_xNIL <-
	_xALPHA
	@@

# Transcribe the rest literally.
@POST
  X("nopos",3) = 1;
@RULES
_xNIL <-
	_xANY [plus]
	@@
