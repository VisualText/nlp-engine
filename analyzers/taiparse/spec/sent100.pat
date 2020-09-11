###############################################
# FILE: sent100.pat
# SUBJ: Initial assignment to sentences.
# AUTH: AM
# CREATED: 06/Jul/04 00:52:50
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@POST
  if (N("sent end",3))
    S("sent end") = N("sent end",3);

  if (pnname(N(3)) == "_qEOS")
    {
    setlookahead(3);
    singler(1,2);
	}
  else # _EOS
    singler(1,3);  # Include the _EOS node.
@RULES
_sent <-
	_xWILD [one fail=(_qEOS \" \( \< \[ _EOS)]
	_xWILD [star fail=(_qEOS _EOS)]
	_xWILD [one match=(_qEOS _EOS)]
	_xWILD [star match=( \) \] \> \" \' )]
	@@

@POST
  noop();
@RULES
_xNIL <- _xWILD [plus match=(
	\" \[ \( \< _qEOS \) \] \> _EOS
	)] @@

@RULES
_sent <-
	_xANY [plus]
	@@
