###############################################
# FILE: sent250.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 08/Jan/05 10:05:40
# MODIFIED:
###############################################

@CODE
G("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

# Moved here from sent300.	# 01/08/05 AM.
@POST
  splice(1,1);
@RULES
_xNIL <-
	_clausesep
	@@

# Merging clauses with split construct:
#  vg to vg 
@CHECK
  if (!N("start to-vg",2))
    fail();
  if (N("last chunk",1) != "v")
    fail();
  if (N("voice",1))
    fail();
@POST
  if (X("vg count") == 2) # These are the only verbs.
    {
	L("vg1") = N("vg node",1);
	fixvg(L("vg1"),"active","VBP");
	N("voice",1) = "active";
	}
  # Probably best to just note that they are bound up
  # together...
  # Combine clauses.
  # (Perhaps need to renumber clauses...)
#  S("vg count") = N("vg count",1) + N("vg count",2);
#  S("voice") = "active";
#  S("vg node") = N("vg node",1);
#  S("clause num") = N("clause num",1);
#  S("last chunk") = N("last chunk",2);
#  merge();
  noop();	# 01/31/05 AM.
@RULES
_clause <-
	_clause
	_clause
	@@
