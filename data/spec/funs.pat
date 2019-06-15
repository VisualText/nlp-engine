###############################################
# FILE: funs.pat
# SUBJ: Miscellaneous functions.
# AUTH: AM
# CREATED: 26/Mar/03
# MODIFIED:
###############################################

@DECL
######
# FN:  PROSIFYSIMPLE
# SUBJ: Convert subtree to a prose-like string.
# CR:  03/26/03 AM.
# NOTE: Assuming all blanks are gone.
#	Simple one-level traverse of children.
#####

prosifysimple(L("root"))
{
if (!L("root"))
  return 0;
L("node") = pndown(L("root"));
if (!L("node"))
  return pnvar(L("root"),"$text");

# Traverse nodes, sticking in spaces.
L("noafter") = 0;
L("str") = pnvar(L("node"),"$text");
while (L("node") = pnnext(L("node")) )
  {
  # Need ispunct.
  L("txt") = pnvar(L("node"),"$text");
  if (L("txt") != "."
   && L("txt") != ","
   && L("txt") != "-"
   && L("txt") != "'"
   && !L("noafter")
   )
    L("str") = L("str") + " ";  # Add space BEFORE current word.
  if (L("txt") == "-"
   || L("txt") == "'")
    L("noafter") = 1;	# No space after, also.
  else
    L("noafter") = 0;
  L("str") = L("str") + pnvar(L("node"),"$text");
  }
return L("str");
}

######
# FN:	PROSIFY
# SUBJ: Convert subtree to a prose-like string.
# CR:	03/26/03 AM.
# NOTE: Assuming all blanks are gone.
#		A crude function.
# RET:	catenated string.
#		RECURSIVE.
#####

prosify(L("root"))
{
if (!L("root"))
  return 0;

# Global var to pass up and down in recursion.
# If spacing after current token.
G("prosify sp") = 0;

return prosifyrec(L("root"),0,1);
}

######
# FN:  PROSIFYREC
# SUBJ: Convert subtree to a prose-like string.
# CR:	05/01/03 AM.
# NOTE: Assuming all blanks are gone.
#		Recursive part of prosify.
# RET:	catenated string.
#####

prosifyrec(
	L("root"),
	L("str"),	# The glommed string so far.
	L("flag")	# Flag if root or not.
	)
{
if (!L("root"))
  return L("str");
L("nm") = pnname(L("root"));

# If literal vs nonliteral.
L("ch") = strpiece(L("nm"),0,0);
if (L("ch") == "."
 || L("ch") == ","
 )
  {
  L("str") = L("str") + L("nm");
  G("prosify sp") = 1;
  }
else if (L("ch") == "-"
 )
  {
  L("str") = L("str") + L("nm");
  G("prosify sp") = 0;
  }
else if (L("ch") != "_")  # Alpha, num, etc.
  {
  if (G("prosify sp"))
    L("str") = L("str") + " " + L("nm");
  else
    L("str") = L("str") + L("nm");
  G("prosify sp") = 1;
  }
else	# Nonliteral.
  {
  # Handle my subtree.
  L("str") = prosifyrec(pndown(L("root")),L("str"),0);
  }

if (pnprev(L("root")))
  return L("str");

# First in a list, handle the list.
if (L("flag"))
  return L("str");
while (L("root") = pnnext(L("root")))
  L("str") = prosifyrec(L("root"),L("str"),1);
return L("str");
}

########
# FUNC:   MONTHTONUM
# SUBJ:   Convert month to number.
# CR:     04/19/02 AM.
# INPUT:  
# OUTPUT: 
########

monthtonum(
	L("text")	# Lowercased text for word.
	)
{
if (L("text") == "january" || L("text") == "jan")
  return 1;
else if (L("text") == "february" || L("text") == "feb")
  return 2;
else if (L("text") == "march" || L("text") == "mar")
  return 3;
else if (L("text") == "april" || L("text") == "apr")
  return 4;
else if (L("text") == "may")
  return 5;
else if (L("text") == "june" || L("text") == "jun")
  return 6;
else if (L("text") == "july" || L("text") == "jul")
  return 7;
else if (L("text") == "august" || L("text") == "aug")
  return 8;
else if (L("text") == "september" || L("text") == "sep"
	|| L("text") == "sept")
  return 9;
else if (L("text") == "october" || L("text") == "oct")
  return 10;
else if (L("text") == "november" || L("text") == "nov")
  return 11;
else if (L("text") == "december" || L("text") == "dec")
  return 12;
else
  return 0;
}

@CODE
G("hello") = 0;
@@CODE
