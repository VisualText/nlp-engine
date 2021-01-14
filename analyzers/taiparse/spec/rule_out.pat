###############################################
# FILE: rule_out.pat
# SUBJ: A sample pass to output rules.
# AUTH: AM
# CREATED: 07/Oct/04 08:44:12
# MODIFIED:
###############################################

@DECL
#############
# PRINTRULE
#############
printrule(L("clause"))
{
if (!L("clause"))
  return;
L("n") = pndown(L("clause"));
if (!L("n"))
  return;

# Assume we have a good clause.
G("rout") << "\n_clause <-";

# Traverse.
L("a flag") = 0; # Flag if working on adverbial.
while (L("n"))
  {
  L("a flag") = printruleelt(L("n"),L("a flag"),G("rout"));
  L("n") = pnnext(L("n"));
  }

G("rout") << " @@";
}

#############
# PRINTRULEELTS
#############
printruleelts(
	L("start"),	# 1st node.
	L("end"),	# last node.
	L("a flag"),	# If last was an adverbial.
	L("rout")		# Stream to print to.
	)
{
if (!L("start") || !L("rout"))
  return L("a flag");
if (L("end"))
  L("end") = pnnext(L("end"));
while (L("start") && L("start") != L("end"))
  {
  L("a flag") = printruleelt(L("start"),L("a flag"),L("rout"));
  L("start") = pnnext(L("start"));
  }
return L("a flag");
}

#############
# PRINTRULEELT
#############
printruleelt(
	L("n"),	# Current node.
	L("a flag"),	# If last was an adverbial.
	L("rout")		# Stream to print to.
	)
{
if (!L("n") || !L("rout"))
  return L("a flag");

  L("name") = pnname(L("n"));
  if (L("name") == "_adv" || L("name") == "_advl")
    {
	if (!L("a flag")) # First adverbial in group.
	  {
	  L("rout") << " ";
	  L("a flag") = 1;
#	  G("rout") << "_xWILD [star match=(_adv _advl)]";
	  L("rout") << "_advl";  # For readability...
	  }
	}
  else
    {
	L("rout") << " ";
	L("a flag") = 0; # Reset.
	L("ch") = strpiece(L("name"),0,0); # First char.
	if (L("ch") == "_") # nonliteral.
	  L("rout") << L("name");
#	else if (strisdigit(L("ch")) || strisalpha(L("ch")))
#	  L("rout") << L("name");
	else if (strisdigit(L("ch")))
	  L("rout") << "num";
	else if (strisalpha(L("ch")))
	  {
	  if (pnvar(L("n"),"posarr len"))
	    {
		L("arr") = pnvar(L("n"),"posarr");
		L("pos") = L("arr")[0];
		L("pos2") = strpiece(L("pos"),0,1);
		if (L("pos2") == "NN"
		 || L("pos2") == "VB"
		 || L("pos2") == "JJ"
		 || L("pos2") == "RB")
		  L("rout") << L("pos2");
		else
		  L("rout") << L("pos");
		}
	  else
	    L("rout") << "alpha";
	  }
	else  # punct
	  L("rout") << "\\" << L("name");
	}
return L("a flag");
}
#############
# PRINTRULEALPHAS
#############

printrulealphas(L("count"),L("rout"))
{
if (!L("count") || !L("rout"))
  return;
while (L("count") > 0)
  {
  L("rout") << " " << "alpha";
  --L("count");
  }
}

@CODE
if (!G("verbose"))
  exitpass();

G("rout") = "rule.txt";
G("rout") << "\n# Automatically gen'd rule file." << "\n";
G("rout") << "@PATH _ROOT _TEXTZONE _sent" << "\n";
# Keep it simple for now, with one rule region.
G("rout") << "\n@RULES\n";
@@CODE

#@PATH _ROOT _TEXTZONE _sent _clause
#@PATH _ROOT _TEXTZONE _sent	# Comment.	# 07/13/12 AM.
@NODES _sent	# 07/13/12 AM.

# Every time I see a clause with a verb group,
#traverse it and print out a grammar rule for it.
#@POST
#  printrule(X(4));  # Supply the clause node.
#@RULES
#_xNIL <-
#	_xANY [plus]
#	_vg [trigger]
#	_xANY [plus]
#	@@

@POST
  printrule(N(1));
@RULES
_xNIL <-
	_clause
	@@

