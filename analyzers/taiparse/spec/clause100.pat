###############################################
# FILE: clause100.pat
# SUBJ: Roughing out clauses.
# AUTH: AM
# CREATED: 09/Jul/04 01:26:33
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

@POST
  group(1,2,"_clause");
@RULES
_xNIL <-
	_clausestart [opt]
	_xWILD [plus fail=(\, _fnword _qEOS _dbldash \" _dblquote _clause _clausestart _clausesep)]
	_xWILD [plus lookahead match=(\, _fnword _qEOS _dbldash \" _dblquote _clause
		_clausestart _clausesep _xEND)]
	@@

@POST
  group(1,1,"_clause");
@RULES
_xNIL <-
	_clausestart
	@@
