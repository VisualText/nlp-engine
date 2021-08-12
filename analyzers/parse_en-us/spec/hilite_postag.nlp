###############################################
# FILE: hilite_postag.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 29/Dec/20 20:50:41
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
if (!G("hilite"))
  exitpass();
@@CODE

# Traverse the whole tree.
@MULTI _ROOT

@PRE
<1,1> var("mypos");
#<1,1> var("pos_np");
@POST
  noop();	# Merely matching the rule will set text to green.
@RULES
_xNIL <- _xANY @@


