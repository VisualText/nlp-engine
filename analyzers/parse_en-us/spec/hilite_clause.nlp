###############################################
# FILE: hilite_clause.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 12/Jan/05 12:57:30
# MODIFIED:
###############################################

@CODE
if (G("pretagged"))
  exitpass();
if (!G("hilite"))	# 10/25/10 AM.
  exitpass();		# 10/25/10 AM.
G("hello") = 0;
@@CODE

# Traverse the whole tree.
@MULTI _ROOT

@POST
  # Active/passive output with clause. # 04/08/05 AM.
  if (G("verbose"))
    {
    if (N("vg")) # If there are verbs in clause.
      {
	  if (N("voice") == "active")
	    "clause.txt" << "active:  ";
	  else if (N("voice") == "passive")
	    "clause.txt" << "passive: ";
	  else
	    "clause.txt" << "novoice: ";
	  }
    else
      "clause.txt" << "noverb:  ";
    "clause.txt" << N("$text") << "\n";
    }
  noop();	# Merely matching the rule will set text to green.
@RULES
_xNIL <- _xWILD [one match=(
	_clause
	)] @@

