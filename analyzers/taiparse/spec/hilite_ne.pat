###############################################
# FILE: hilite_ne.pat
# SUBJ: Highlight named entities.
# AUTH: AM
# CREATED: 24/May/05 12:23:03
# MODIFIED:
###############################################

@CODE
if (G("pretagged"))
  exitpass();
G("hello") = 0;
@@CODE

# Traverse the whole tree.
@MULTI _ROOT

@PRE
<1,1> var("ne");	# FEATURE-BASED MATCH IN NLP++ ! 6/16/05 AM.
@POST
  if (G("verbose"))
    "multi.txt" << pnname(N(1)) << "\t" << phrasetext() << "\n";
  noop();	# Merely matching the rule will set text to green.
@RULES
_xNIL <- 
	_xANY
	@@

