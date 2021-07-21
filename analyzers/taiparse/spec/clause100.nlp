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

@NODES _sent

@POST
  group(1,2,"_clause");
  setunsealed(1,"true");	# 07/10/12 AM.
@RULES
_xNIL <-
	_clausestart [opt]
	_xWILD [plus fail=(\, _fnword _qEOS _dbldash \" _dblquote _clause _clausestart _clausesep)]
	_xWILD [plus lookahead match=(\, _fnword _qEOS _dbldash \" _dblquote _clause
		_clausestart _clausesep _xEND)]
	@@

@POST
  group(1,1,"_clause");
  setunsealed(1,"true");	# 07/10/12 AM.
@RULES
_xNIL <-
	_clausestart
	@@
