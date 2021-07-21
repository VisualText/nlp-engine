###############################################
# FILE: tags100.pat
# SUBJ: Find stuff in tags.
# AUTH: AM
# CREATED: 14/Mar/06 10:22:54
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _ROOT

@POST
  if (!G("title"))
    G("title") = N("$treetext",2);
  N("header",2) = 1;	# Flag header zone.
@RULES
_xNIL <-
	_title
	_xWILD [plus gp=_TEXTZONE]
	_Etitle
	@@
_xNIL <-
	_ha
	_xWILD [plus gp=_TEXTZONE]
	_Eha
	@@
_xNIL <-
	_hb
	_xWILD [plus gp=_TEXTZONE]
	_Ehb
	@@
_xNIL <-
	_hc
	_xWILD [plus gp=_TEXTZONE]
	_Ehc
	@@
_xNIL <-
	_hd
	_xWILD [plus gp=_TEXTZONE]
	_Ehd
	@@
_xNIL <-
	_he
	_xWILD [plus gp=_TEXTZONE]
	_Ehe
	@@
_xNIL <-
	_hf
	_xWILD [plus gp=_TEXTZONE]
	_Ehf
	@@
_xNIL <-
	_hx
	_xWILD [plus gp=_TEXTZONE]
	_Ehx
	@@

