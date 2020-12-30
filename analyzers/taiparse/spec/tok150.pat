###############################################
# FILE: tok150.pat
# SUBJ: Tokenize after looks at individual words.
# AUTH: AM
# CREATED: 03/Aug/04 22:11:05
# MODIFIED:
###############################################

@CODE
if (G("pretagged"))
  exitpass();
@@CODE

@NODES _TEXTZONE

# Sentinel.
@POST
  noop();
@RULES
_xNIL <-
	_xCAP
	_xWHITE [star]
	_xCAP
	_xWILD [star match=(_xWHITE _xCAP)]
	@@

@POST
  singler(2,5);
@RULES
_tok <-
	_xWILD [plus match=(_xSTART _xWHITE)]
	_xALPHA [s]
	_xWILD [plus match=(_xPUNCT _xNUM)]
	_xALPHA [s]
	_xWILD [star fail=(_xWHITE _xEND _qEOS \. \, \; \! \? \: )]
	@@

# 03/11/05 AM. Moved from fnretok to dom100.
# 05/23/05 AM. Moved from dom100 to tok150.
# Parentheticals.	# 01/12/05 AM.
# Zap for now.
@POST
  N("parenth",2) = 1;
  excise(3,3);
  excise(1,1);
@RULES
_xNIL <-
	\(
	_xALPHA
	\)
	@@
_xNIL <-
	\[
	_xALPHA
	\]
	@@
_xNIL <-
	\{
	_xALPHA
	\}
	@@
