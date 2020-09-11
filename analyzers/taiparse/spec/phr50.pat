###############################################
# FILE: phr50.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 09/Oct/04 15:56:35
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

# vg np conj vg
@POST
  # Align verb assignments.
  L("pos") = vgpos(N(1));
  L("voice") = N("voice",1);
  fixvg(N(4),L("voice"),L("pos"));
  "ps.txt" << L("voice") << " " << L("pos") << "\n";
@RULES
_xNIL <-
	_vg
	_np [opt]
	_conj
	_vg
	@@

# vg np prep np vg
# threw people in a mine filled ....
@POST
 fixvg(N(5),"passive","VBN");
@RULES
_xNIL <-
	_vg
	_np [lookahead]
	_prep
	_np
	_vg
	@@

# np vg np vg
# need to check for helping verbs etc.
# WEAK RULE.	# 10/22/04 AM.
#@CHECK
#  if (N("voice",4) != "active")
#   fail();
#@POST
#  N("glom",2) = "left";
#  N("glom",3) = "right";
#@RULES
#_xNIL <-
#	_np
#	_vg
#	_np
#	_vg
#	@@

# In a larger context.
# prep dqan alpha
@CHECK
  if (!N("noun",7))
    fail();
  if (N("verb",7))
    {
    if (!vconjq(N(7),"inf"))
	  fail();
	}
@POST
  L("tmp7") = N(7);
  group(7,7,"_noun");
  pncopyvars(L("tmp7"),N(7));
  fixnoun(N(7));
@RULES
_xNIL <-
	_xSTART
	_np
	\,
	_xWILD [one match=(_prep)]
	_proPoss [s opt]
	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun)]
	_xALPHA
	@@

# np conj np
@POST
  group(3,5,"_np");
@RULES
_xNIL <-
	_xWILD [one match=(_vg)]
	_xWILD [opt match=(_prep)]
	_np
	_conj
	_np
	_xWILD [lookahead one match=(_qEOS _dbldash _xEND)]
	@@


# np vg adj
@CHECK
  S("v") = N("verb node",2);
  if (!S("v"))
    fail();
  if (pnvar(S("v"),"mypos"))
    fail();
@POST
  fixvg(N(2),"active","VBP");
@RULES
_xNIL <-
	_np
	_vg
	_adj [lookahead]
	@@

# np apos
@POST
  chpos(N(2),"POS");
@RULES
_xNIL <-
	_np
	_aposX
	@@

