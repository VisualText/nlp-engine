###############################################
# FILE: funs.pat
# SUBJ: Functions.
# AUTH: AM
# CREATED: 06/Jul/04 02:00:25
# MODIFIED:
###############################################

@DECL
########
# FUNC:   LOOKUPALPHA
# SUBJ:   Look up an alphabetic word.
# CR:     03/20/02 AM.
# INPUT:  Assumes there's a word-node to attach results to.
# OUTPUT: 
# WARN:   Modifies vars in the given node.
########

lookupalpha(
	L("text"),	# Lowercased text for word.
	L("node")	# The node representing the word.
	)
{
if (!(L("wordcon") = dictfindword(L("text"))))
	return;	# Not handling unknown words here.
pnreplaceval(L("node"),"wordcon", L("wordcon"));

# Grab some attributes from kb.	# 06/24/02 AM.
if (L("nsem") = conval(L("wordcon"),"nounsem"))	# 06/24/02 AM.
  {
  pnreplaceval(L("node"),"nounsem",L("nsem"));	# 06/24/02 AM.
  if (inhierarchy(L("nsem"),"event"))	# 06/26/02 AM.
      pnreplaceval(L("node"),"eventive",1);	# 06/24/02 AM.
  pnreplaceval(L("node"),"sem",conceptname(L("nsem")));
  }
#domobject(L("text"),L("node"),L("nsem"));

L("vals") = findvals(L("wordcon"), "pos");
L("pos num") = 0;
L("pos") = 0;
while (L("vals"))
	{
	L("val") = getstrval(L("vals"));
	if (L("val") == "noun")
		{
		pnreplaceval(L("node"),"noun", 1);
		L("pos") = "_noun";
#		eventivenoun(L("text"),L("node"));
		}
	else if (L("val") == "verb")
		{
		pnreplaceval(L("node"),"verb", 1);
		L("pos") = "_verb";
		
		# Any -ing verb can be an eventive noun.
		if (strendswith(L("text"),"ing"))
			pnreplaceval(L("node"),"eventive",1);
		}
	else if (L("val") == "adjective")
		{
		pnreplaceval(L("node"),"adj", 1);
		L("pos") = "_adj";
		}
	else if (L("val") == "adverb")
		{
		pnreplaceval(L("node"),"adv", 1);
		L("pos") = "_adv";
		}
	else if (L("val") == "prep")
		{
		pnreplaceval(L("node"),"prep", 1);
		L("pos") = "_prep";
		}
	else if (L("val") == "pronoun")
		{
		pnreplaceval(L("node"),"pro", 1);
		L("pos") = "_pro";
		}
	else if (L("val") == "conj"
	 || L("val") == "conjunction")	# 03/20/02 AM.
		{
		pnreplaceval(L("node"),"conj", 1);
		L("pos") = "_conj";
		}
	else if (L("val") == "interj")
		{
		pnreplaceval(L("node"),"interj", 1);
		L("pos") = "_interj";
		}
	else if (L("val") == "det")
		{
		pnreplaceval(L("node"),"det", 1);
		L("pos") = "_det";
		}
	else
	    {
		# Hmm.  Found "abbreviation" as one unhandled
		# pos.  # 12/08/01 AM.
#		"misc.txt" << "abbreviation=" << L("text") << "\n";
		pnreplaceval(L("node"),"abbr",1);
		# L("pos") = 0;
		}
	L("vals") = nextval(L("vals"));
	++L("pos num");
	}
pnreplaceval(L("node"),"pos num", L("pos num"));

if (L("pos") && !pnvar(L("node"),"pos")) # 06/05/05 AM.
   pnreplaceval(L("node"),"pos",L("pos"));
if (L("pos num") > 0 && !pnvar(L("node"),"stem"))
  {
  if (L("stem") = nvstem(L("text")))
  	pnreplaceval(L("node"),"stem",L("stem"));
  else
    pnreplaceval(L("node"),"stem",L("text"));
  }
}

########
# FUNC:   VGAGREE
# CR:	  05/03/02 AM.
# ARGS:	  Top-level nodes assumed to bear info.
# RET:	  L("ret")[0] - 1 if agreement, else 0.
#		  L("ret")[1] - 1 if passive voice, else 0.
#		  L("ret")[2] - "past", "present", "future", 0.
#		  L("ret")[3] = "inf","-en","-ed","-edn","-ing","-s", or 0. 
#				First element's conjugation. (see notes).
#		  May want progressive, etc. "was eating..."
#	Would like the FAILURE POINT, so that I know where to flag
#	no glomming verbs left and no glomming verbs right.
# NOTE:	For constructs like "Should John be eating", will
#	consider that "be eating" has good agreement.  But
#	may want to return a flag saying it is "incomplete".
#	Should John be eating => inf-start.
#	Has John eaten => en-start.
#	Was John eating => ing-start.
#	Was John eaten => en-start.
# Some hints:
#	modal => needs inf.
#	have  => needs en.
#	be    => needs en (passive) or ing.
#   being => needs en.
# OPT: May want to internalize this in NLP++ for speed, at some
#	point.
########

vgagree(L("m"),L("h"),L("b"),L("being"),L("v"))
{
#if (L("m"))
#  "vg.txt" << "m=" << pnvar(L("m"),"$text") << "\n";
#if (L("v"))
#  "vg.txt" << "v=" << pnvar(L("v"),"$text") << "\n";

L("ret")[0] = 1; # Agreement till proved otherwise.
L("ret")[3] = 0; # Set return vals to zero.

L("need") = 0;	# What's needed next.
	# Vals = "inf","en","en-ing".
L("first") = 0; # First element seen.
L("last") = 0;  # Last element seen.

if (L("m")) # MODAL
  {
  # Need to set can-could type of modality here, or "future".

    L("need") = "inf";
  if (!L("first"))
    L("first") = L("m");
  L("last") = L("m");
  }

if (L("h"))	# HAVE
  {
  if (L("need") == "inf"
   && !pnvar(L("h"),"inf") )
    {
	# eg, "WILL HAD".
	L("ret")[0] = 0;	# NO AGREEMENT.
	return L("ret");
	}

  L("need") = "en";
  if (!L("first"))
    L("first") = L("h");
  L("last") = L("h");
  }

if (L("b"))	# BE
  {
  if (L("need") == "inf")
    {
    if (!pnvar(L("b"),"inf"))
      {
	  # eg, "WILL BEEN".
	  L("ret")[0] = 0;	# NO AGREEMENT.
	  return L("ret");
	  }
	}
  else if (L("need") == "en"  # BUG FIX.	# 07/10/02 AM.
   && !pnvar(L("b"),"-en") )
    {
	# eg, "HAVE ARE".
	L("ret")[0] = 0;	# NO AGREEMENT.
	return L("ret");
	}
  
  L("need") = "en-ing";
  if (!L("first"))
    L("first") = L("b");
  L("last") = L("b");
  }

if (L("being"))	# BEING
  {
  if (L("need") == "inf")
    {
	# eg, "WILL BEING".
	L("ret")[0] = 0;	# NO AGREEMENT.
	return L("ret");
	}
  else if (L("need") == "en")	# BUG FIX.	# 07/10/02 AM.
    {
	# eg, "HAVE BEING".
	L("ret")[0] = 0;	# NO AGREEMENT.
	return L("ret");
	}
  # else 0 or "en-ing" are OK.

  L("need") = "en";
  if (!L("first"))
    L("first") = L("being");
  L("last") = L("being");
  }

if (L("v"))	# VERB
  {
  L("vsuff") = vconj(L("v"));
  if (!L("vsuff"))
      {
	  # eg, "WAS BEEN".
	  L("ret")[0] = 0;	# NO AGREEMENT.
	  return L("ret");
	  }

  if (L("need") == "inf")
    {
	if (!vconjq(L("v"),"inf"))
      {
	  # eg, "WILL ATE".
	  L("ret")[0] = 0;	# NO AGREEMENT.
	  return L("ret");
	  }
	}
  else if (L("need") == "en")	# BUG FIX.	# 07/10/02 AM.
    {
	if (!vconjq(L("v"),"-en"))
      {
	  # eg, "HAVE ATE".
	  L("ret")[0] = 0;	# NO AGREEMENT.
	  return L("ret");
	  }
	}
  else if (L("need") == "en-ing")
    {
    if (L("vsuff") == "-edn"
	 || L("vsuff") == "-en" )
	  {
	  L("ret")[1] = 1;  # SETS PASSIVE VOICE FOR VG.
	  }
	else if (L("vsuff") == "-ing")
	  {
	  }
	else
      {
	  # eg, "WAS ATE".
	  L("ret")[0] = 0;	# NO AGREEMENT.
	  return L("ret");
	  }
	}
  # else 0 is OK.
  
  if (!L("first"))
    L("first") = L("v");
  L("last") = L("v");
  }

# TODO: Use first and last to determine need to the left
# and need to the right of this verb group.  eg, for
# resolving question syntax to left, unknown words to right.

return L("ret");
}

########
# FUNC:	VCONJQ
# SUBJ:	Get needed verb conjugation.
# ARGS:	Verb node.
# RET:	1 if matched need, else 0.
# ASS:	Assumes all irregular verb forms are in (l_)common.pat
#		(Could place this knowledge in kb also, for easy
#		reference...)
# NOTE:	VARIANT OF VCONJ TO HANDLE AMBIGUOUS VERBS LIKE "set".
#	For need == "-edn", requiring exact match. This will enable
#	user to tailor calls more flexibly.
########

vconjq(L("v"), L("need"))
{
if (!L("v") || !L("need"))
  return 0;

# Exact match.
if (pnvar(L("v"),L("need")))
  return 1;

# OPT: May want to collapse all this to one var!
if (pnvar(L("v"),"-edn"))	# Irregular verb +edn ending.
  {
  if (L("need") == "-en"
   || L("need") == "-ed")
    return 1;
  return 0;
  }
if (L("need") == "-edn")
  {
  if (pnvar(L("v"),"-en")
   && pnvar(L("v"),"-ed"))
    return 1;
  }

# If any nonmatching conjugation present, assume failure.
if (pnvar(L("v"),"-en"))	# Irregular +en
  return 0;
if (pnvar(L("v"),"inf"))	# Exception like "need".
  return 0;
if (pnvar(L("v"),"-ed"))	# Irregular like "ate".
  return 0;
if (pnvar(L("v"),"-ing"))
  return 0;
if (pnvar(L("v"),"-s"))
  return 0;

# Moved this down here. # 06/03/06 AM.
if (pnvar(L("v"),"stem") == "be") # Special case.
  {
  # Todo: Can now handle all these....
  return 0;
  }

# NOT IRREGULAR, SO USE LITERAL TEXT.
# Need a convention for getting just the verb's text.
# eg in something like "haven't".
L("vtext") = nodetext(L("v"));
#"vg.txt" << "vconj text=" << L("vtext") << "\n";
if (!L("vtext"))
  return 0;
if (strendswith(L("vtext"),"ed"))
  {
  if (L("need") == "-edn"
   || L("need") == "-en"
   || L("need") == "-ed")
    return 1;	# "Normal" verb with ambiguous ed ending.
  return 0;
  }
if (strendswith(L("vtext"),"ing"))
  return L("need") == "-ing";
#if (suffix(L("vtext"),"s"))	# "s" is a proper suffix.
if (nvsuff(L("vtext")) == "-s")	# 06/15/06 AM.
  return L("need") == "-s";
return L("need") == "inf";	# Assume found uninflected form.
}

########
# FUNC:	VCONJ
# SUBJ:	Get verb conjugation.
# ARGS:	Verb node.
# RET:	"inf","-s","-ing","-edn","-ed","-en", or 0.
# ASS:	Assumes all irregular verb forms are in (l_)common.pat
#		(Could place this knowledge in kb also, for easy
#		reference...)
#		May not work as desired for verbs like be.
#		Need to handle highly ambiguous, eg, "set".
########

vconj(L("v"))
{
if (!L("v"))
  return 0;

# OPT: May want to collapse all this to one var!
if (pnvar(L("v"),"-edn"))	# Irregular verb +edn ending.
  return "-edn";
if (pnvar(L("v"),"-en"))	# Irregular +en
  return "-en";
if (pnvar(L("v"),"inf"))	# Exception like "need".
  return "inf";
if (pnvar(L("v"),"-ed"))	# Irregular like "ate".
  return "-ed";
if (pnvar(L("v"),"-ing"))
  return "-ing";
if (pnvar(L("v"),"-s"))
  return "-s";

# Moved this down here. # 06/03/06 AM.
if (pnvar(L("v"),"stem") == "be") # Special case.
  return 0;

# NOT IRREGULAR, SO USE LITERAL TEXT.
# Need a convention for getting just the verb's text.
# eg in something like "haven't".
L("vtext") = nodetext(L("v"));
#"vg.txt" << "vconj text=" << L("vtext") << "\n";
if (!L("vtext"))
  return 0;
if (strendswith(L("vtext"),"ed"))
  return "-edn";	# "Normal" verb with ambiguous ed ending.
if (strendswith(L("vtext"),"ing"))
  return "-ing";
#if (suffix(L("vtext"),"s"))	# "s" is a proper suffix.
if (nvsuff(L("vtext")) == "-s")	# 06/15/06 AM.
  return "-s";
return "inf";	# Assume found uninflected form.
}

########
# FUNC:   NODETEXT
# ARGS:	  Get text of a node.
# RET:	  text or 0.
# ASS:	Using variable called "text" if node's text was
#	created explicitly.
# WARN:	GETTING LOWERCASED TEXT.
########

nodetext(L("n"))
{
if (!L("n"))
  return 0;
L("text") = pnvar(L("n"),"text"); # If var present, use it.
if (!L("text"))
  return strtolower(pnvar(L("n"),"$text")); # Get from tokens.
return L("text");
}


########
# FUNC: NUMBER
# SUBJ:	Check singular/plural/any for noun.
# ARGS: Noun node.
# RET:	"singular", "plural", "any", or 0.
# ASS:	Should return any for mass nouns, things like sheep...
# TODO:	Irregular plurals.  radii, cherubim, etc.
########

number(L("n"))
{
if (!L("n"))
  {
  return 0;
  }
if (pnvar(L("n"),"-s"))
  return "plural";
L("num") = pnvar(L("n"),"number");
if (L("num"))
  return L("num");

L("ntext") = nodetext(L("n"));
if (!L("ntext"))
  {
  return 0;
  }
L("stem") = pnvar(L("n"),"stem");
if (!L("stem"))
  L("stem") = nvstem(L("ntext"));
#"stem.txt" << "stem,text=" << L("stem") << "," << L("ntext") <<"\n";
if (strendswith(L("ntext"),"s")
 && L("stem") != L("ntext") )
  return "plural";
return "singular";
}


########
# FUNC:	SINGULAR
# SUBJ:	True if noun can be singular.
# ARGS: Noun node.
# RET:	1 or 0.
# ASS:	Should return true for mass nouns, things like sheep...
# TODO:	Irregular plurals.  radii, cherubim, etc.
########

singular(L("n"))
{
if (!L("n"))
  return 0;
if (pnvar(L("n"),"-s"))
  return 0;
L("pl") = pnvar(L("n"),"number");
if (L("pl") == "singular")
  return 1;
else if (L("pl") == "plural")
  return 0;
if (pnvar(L("n"),"number") == "singular")
  return 1;

L("ntext") = nodetext(L("n"));
if (!L("ntext"))
  return 0;
L("stem") = pnvar(L("n"),"stem");
if (!L("stem"))
  L("stem") = nvstem(L("ntext"));
if (strendswith(L("ntext"),"s")
 && L("stem") != L("ntext") )
  return 0;
return 1;
}


########
# FUNC:	PLURAL
# SUBJ:	True if noun can be plural.
# ARGS: Noun node.
# RET:	1 or 0.
# ASS:	Should return true for mass nouns, things like sheep...
# TODO:	Irregular plurals.  radii, cherubim, etc.
########

plural(L("n"))
{
if (!L("n"))
  return 0;
if (pnvar(L("n"),"-s"))
  return 1;
if (pnvar(L("n"),"number") == "plural")
  return 1;

L("ntext") = nodetext(L("n"));
if (!L("ntext"))
  return 0;
L("stem") = pnvar(L("n"),"stem");
if (!L("stem"))
  L("stem") = nvstem(L("ntext"));
if (strendswith(L("ntext"),"s")
 && L("stem") != L("ntext") )
  return 1;
return 0;
}

########
# FUNC:	NUMBERSAGREE
# SUBJ:	True if range of nodes agree in numbering.
# RET:	Agreed on number or 0 if fail.
# NOTE: Changing this to be for a noun phrase.  "Is this NP complete" (sic)
#	If no singular indicators, then end of noun phrase must be plural.
########

numbersagree(L("start"),L("end"))
{
if (!L("start") || !L("end"))
  return 0;
if (L("start") == L("end"))
  {
  L("need") = number(L("start"));
  if (L("need"))
    return L("need");
  return "any";	# Node agrees with itself.
  }
L("last") = L("end");
L("end") = pnnext(L("end"));
L("n") = L("start");
L("agree") = 1; # Agreement so far.
L("need") = "any"; # Constraint to be satisfied.
while (L("n") != L("end"))
  {
  L("nm") = pnname(L("n"));
  L("number") = 0; # Reset.
  if (L("nm") == "_adj"
   || L("nm") == "_adv"
   || (L("nm") == "_noun" && L("n") != L("last"))
   )
    ; # IGNORE
  else if (pnvar(L("n"),"number"))
    L("number") = pnvar(L("n"),"number");
  else if (pnvar(L("n"),"num"))
    {
	L("num") = pnvar(L("n"),"num");
	if (L("num") == 1)
	  L("number") = "singular";
	else
	  L("number") = "plural";
	}
#  else if (pnvar(L("n"),"noun"))
#    L("number") = number(L("n"));
  else if (strisdigit(pnvar(L("n"),"$text"))) # 10/19/04 AM.
    {
	L("nm") = num(pnvar(L("n"),"$text"));
	if (L("nm") == 1)
	  L("number") = "singular";
	else
	  L("number") = "plural";
	}
  # Should also check final alpha (ie as noun...) # 10/19/04 AM.
  else if (literal(L("n")) && L("n") == L("last"))
    {
	L("number") = number(L("n"));
	}

  if (L("number"))
    {
	if (L("need") == "any")
	  L("need") = L("number");
	else if (L("need") != L("number"))
	  return 0;	# Disagreement.
	}
  L("n") = pnnext(L("n"));
  }
if (L("need"))
  return L("need");
return "any";	# Agreement.
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

########
# FUNC:	INTERSECT
# SUBJ:	Intersection of two string-valued arrays.
# RET:	Array representing intersection.
########
intersect(L("arr1"),L("arr2"))
{
if (!L("arr1") || !L("arr2"))
  return 0;
L("len1") = arraylength(L("arr1"));
L("len2") = arraylength(L("arr2"));
L("ii") = 0;
L("newarr") = 0;
L("len") = 0; # Length of new array.
while (L("ii") < L("len1"))
  {
  L("jj") = 0;
  L("test") = L("arr1")[L("ii")];
  while (L("jj") < L("len2"))
    {
	if (L("test") == L("arr2")[L("jj")])
	  {
	  L("newarr")[L("len")] = L("test");	# Add new val.
	  ++L("len");
	  }
	++L("jj");
    }
  ++L("ii");
  }
return L("newarr");
}

########
# FUNC:	NONLITERAL
# SUBJ:	Check if node is nonliteral (should be NLP++ fn...).
########

nonliteral(L("n"))
{
if (!L("n"))
  return 0;
L("nm") = pnname(L("n"));
if (strpiece(L("nm"),0,0) == "_")
  return 1;
return 0;
}

########
# FUNC:	LITERAL
# SUBJ:	Check if node is literal (should be NLP++ fn...).
########

literal(L("n"))
{
if (!L("n"))
  return 0;
L("nm") = pnname(L("n"));
if (strpiece(L("nm"),0,0) == "_")
  return 0;
return 1;
}

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
L("space after") = pnvar(L("root"),"space after");
L("nospace after") = pnvar(L("root"),"nospace after");
#"crash.txt" << L("nm") << "\n";

# If literal vs nonliteral.
L("ch") = strpiece(L("nm"),0,0);
if (L("ch") == "."
 || L("ch") == ","
 )
  {
  L("str") = L("str") + L("nm");
  if (L("nospace after"))
    G("prosify sp") = 0;
  else
    G("prosify sp") = 1;
  }
else if (L("ch") == "-"
 || L("ch") == "'"
 || L("ch") == "\""
 || L("ch") == "`"
 )
  {
  L("str") = L("str") + L("nm");
  if (L("space after"))
    G("prosify sp") = 1;
  else
    G("prosify sp") = 0;
  }
else if (L("ch") == " "
 || L("ch") == "\t"
 || L("ch") == "\n"
 || L("ch") == "\r")
  ; # Ignore whitespace.
else if (L("ch") != "_")  # Alpha, num, etc.
  {
  if (G("prosify sp"))
    L("str") = L("str") + " " + L("nm");
  else
    L("str") = L("str") + L("nm");
  if (L("nospace after"))
    G("prosify sp") = 0;
  else
    G("prosify sp") = 1;
  }
else	# Nonliteral.
  # Handle my subtree.
  L("str") = prosifyrec(pndown(L("root")),L("str"),0);

if (pnprev(L("root")))
  return L("str");

# First in a list, handle the list.
if (L("flag"))
  return L("str");
while (L("root") = pnnext(L("root")))
  L("str") = prosifyrec(L("root"),L("str"),1);
return L("str");
}

######
# FN:  SAMEVCONJ
# SUBJ: See if compatible verb conjugations.
# CR:	10/17/04 AM.
# RET:	Most constrained fit.
# TODO: Handle ambiguous verb conjugations.
#####

samevconj(L("v1"),L("v2"))
{
if (!L("v1") || !L("v2"))
  return 0;
#"samevc.txt" << pnvar(L("v1"),"$text")
#	<< "," << pnvar(L("v2"),"$text") << "\n";
L("vc1") = vconj(L("v1"));
L("vc2") = vconj(L("v2"));
if (L("vc1") == L("vc2"))
  return L("vc1");

# For ambiguous cases...
# Todo: Doesn't necessarily work if BOTH are ambig.
if (vconjq(L("v1"),L("vc2")))
  return 1;
if (vconjq(L("v2"),L("vc1")))
  return 1;

# Now check out -edn -ed -en.
if (L("vc1") != "-edn" && L("vc2") != "-edn")
  return 0;
if (L("vc1") == "-en" || L("vc2") == "-en")
  return "-en";
if (L("vc1") == "-ed" || L("vc2") == "-ed")
  return "-ed";
return 0;	# Error.
}

######
# FN:  ISNEWSENTENCE
# SUBJ: See if node starts a new sentence.
# CR:	01/15/05 AM.
# RET:	1 if true, else 0.
#####

isnewsentence(L("node"))
{
if (!L("node"))
  return 0;
L("prev") = pnprev(L("node"));
if (!L("prev"))
  return 1;
L("name") = pnname(L("prev"));
if (L("name") == "_qEOS"
 || L("name") == "_dbldash"
 || L("name") == "\"")
  return 1;  # Tentatively...
return 0;
}

######
# FN:  ELLIPTEDPASSIVE
# SUBJ: See if node is passive subclause.
# CR:	01/25/05 AM.
# RET:	1 if true, else 0.
# EG:	John, bitten by lice, is mad...
######

elliptedpassive(L("clause"))
{
if (!L("clause"))
  return 0;
L("vg") = pnvar(L("clause"),"vg node");
if (!L("vg"))
  return 0;
if (pnvar(L("clause"),"voice") != "passive")
  return 0;
L("v") = pnvar(L("vg"),"first verb");
if (!L("v"))
  return 0;
if (pnvar(L("v"),"mypos") != "VBN")
  return 0;
return 1;
}

######
# FN:  ELLIPTEDACTIVE
# SUBJ: See if node is ving.
# CR:	03/02/05 AM.
# RET:	1 if true, else 0.
# EG:	John, biting nails, is mad...
######
elliptedactive(L("clause"))
{
if (!L("clause"))
  return 0;
L("vg") = pnvar(L("clause"),"vg node");
if (!L("vg"))
  return 0;
if (pnvar(L("clause"),"voice") != "active")
  return 0;
L("v") = pnvar(L("vg"),"first verb");
if (!L("v"))
  return 0;
if (pnvar(L("v"),"mypos") == "VBG")
  return 1;
if (vconjq(L("v"),"-ing"))
  return 1;
return 0;
}

######
# FN:  PNNAMEINRANGE
# SUBJ: See if list of nodes has name.
# CR:	02/12/05 AM.
# RET:	first node found.
######

pnnameinrange(L("name"),L("start"),L("end"))
{
if (!L("name") || !L("start"))
  return 0;
if (L("end"))
  L("end") = pnnext(L("end"));
L("node") = L("start");
while (L("node"))
  {
  if (pnname(L("node")) == "_advl")
    return L("node");
  if (L("node") == L("end"))
    return 0;
  L("node") = pnnext(L("node"));
  }
}

######
# FN:  ATTRINRANGE
# SUBJ: See if list of nodes has attr.
# RET:	First value found.
######

attrinrange(L("name"),L("start"),L("end"))
{
if (!L("name") || !L("start"))
  return 0;
if (L("end"))
  L("end") = pnnext(L("end"));
L("node") = L("start");
while (L("node"))
  {
  L("val") = pnvar(L("node"),L("name"));
  if (L("val"))
    return L("val");
  if (L("node") == L("end"))
    return 0;
  L("node") = pnnext(L("node"));
  }
}

######
# FN:  PNREPLACEVALRANGE
# SUBJ: Replace value in range of nodes.
# CR:	02/18/05 AM.
######
pnreplacevalrange(L("var"),L("val"),L("start"),L("end"))
{
if (!L("var") || !L("start"))
  return;
if (L("end"))
  L("end") = pnnext(L("end"));
L("node") = L("start");
while (L("node"))
  {
  if (L("node") == L("end"))
    return;
  pnreplaceval(L("node"),L("var"),L("val"));
  L("node") = pnnext(L("node"));
  }
}

######
# FN:  VVAGREE
# SUBJ: See if tenses of 2 verbs agree.
# NOTE:
######
vvagree(L("n1"),L("n2"))
{
if (!L("n1") || !L("n2"))
  fail();

L("nm1") = pnname(L("n1"));
L("nm2") = pnname(L("n2"));
if (L("nm1") == "_vg" && L("nm2") == "_vg")
  return vgvgagree(L("vg1"),L("vg2"));

if (L("nm1") == "_vg")
  L("v1") = pnvar(L("n1"),"first verb");
else
  L("v1") = L("n1");

if (L("nm2") == "_vg")
  L("v2") = pnvar(L("n2"),"first verb");
else
  L("v2") = L("n2");

return verbverbagree(L("v1"),L("v2"));
}


######
# FN:  VGVGAGREE
# SUBJ: See if tenses of 2 vgs agree.
# CR:	04/15/05 AM.
######
vgvgagree(L("vg1"),L("vg2"))
{
if (!L("vg1") || !L("vg2"))
  return 0;
L("first1") = pnvar(L("vg1"),"first verb");
L("first2") = pnvar(L("vg2"),"first verb");

return verbverbagree(L("first1"),L("first2"));
}

######
# FN:  VERBVERBAGREE
# SUBJ: See if tenses of 2 verbs agree.
######
verbverbagree(L("first1"),L("first2"))
{
if (!L("first1") || !L("first2"))
  return 0;
L("vc1") = vconj(L("first1"));
L("vc2") = vconj(L("first2"));
#"vgvg.txt" << pnvar(L("first1"),"$text") << "," << pnvar(L("first2"),"$text") << "\n";
#"vgvg.txt" << L("vc1") << "," << L("vc2") << "\n";
if (L("vc1") == L("vc2"))
  return 1;
if (pnvar(L("first1"),"inf")
 && pnvar(L("first2"),"inf"))
  return 1;
if (pnvar(L("first1"),"-s")
 && pnvar(L("first2"),"-s"))
  return 1;
if (pnvar(L("first1"),"-ing")
 && pnvar(L("first2"),"-ing"))
  return 1;
if (pnvar(L("first1"),"-ed")
 && pnvar(L("first2"),"-ed"))
  return 1;
if (pnvar(L("first1"),"-en")
 && pnvar(L("first2"),"-en"))
  return 1;
# Check compatibles.
# Check ambiguous.
if (pnvar(L("first1"),"-edn"))
  {
  if (pnvar(L("first2"),"-edn")
   || pnvar(L("first2"),"-en")
   || pnvar(L("first2"),"-ed") )
   return 1;
  }
if (pnvar(L("first2"),"-edn"))
  {
  if (pnvar(L("first1"),"-en")
   || pnvar(L("first1"),"-ed") )
   return 1;
  }

return 0;
}

########
# FUNC:	PNADDSTR
# SUBJ:	Add string to a node's variable.
# EX:	pnaddstr(N(2),"hello","newstr");
# NOTE:	For adding multiple values to a variable.
########
pnaddstr(
	L("node"),	# Node we are adding info to.
	L("field"),
	L("str")
	)
{
if (!L("node") || !L("field") || !L("str"))
  return;
L("vals") = pnvar(L("node"),L("field"));
if (!L("vals"))
  L("len") = 0;
else
  L("len") = arraylength(L("vals"));

# Can't directly append a new value onto node.
# Need something like pnaddval(L("node"),L("field"),L("str")).
L("vals")[L("len")] = L("str");
pnreplaceval(L("node"),L("field"),L("vals"));
}

########
# FUNC:   INHIERARCHY
# SUBJ:   See if given concept is under a concept by name.
# CR:     06/26/02 AM.
# INPUT:  
# OUTPUT: 
########

inhierarchy(L("con"),L("parent_str"))
{
if (!L("parent_str"))
  return 0; # false.
while (L("con"))
  {
  if (conceptname(L("con")) == L("parent_str"))
    return 1; # true.
  L("con") = up(L("con"));
  }
return 0; # false.
}

########
# FUNC:   GETDATE
# SUBJ:   Compute a date string.
########

getdate(L("n"))
{
if (!L("n"))
  return 0;
L("yr") = pnvar(L("n"),"yr");
L("dy") = pnvar(L("n"),"dy");
L("mo") = pnvar(L("n"),"mo");
if (!L("yr") && !L("mo"))
  return 0;

if (!L("yr"))
  {
  if (L("mo") == 11 || L("mo") == 12)
    L("yr") = 2004;
  else
    L("yr") = 2005;
  }

if (!L("mo"))
  return str(L("yr")) + "-00-00";

if (L("mo") < 10)
  L("mo") = "0" + str(L("mo"));
if (L("dy") < 10)
  L("dy") = "0" + str(L("dy"));

return str(L("yr")) + "-"
  	+ str(L("mo")) + "-"
	+ str(L("dy"));
}

########
# FUNC:   NVSTEM
# SUBJ:   Compute stem for word.
# NOTE:	  NLP++ stem() sucks, should do more of the below.
########

nvstem(L("txt"))
{
if (!L("txt"))
  return 0;
L("lc") = strtolower(L("txt"));
# Can do more accurate if it's a known word.
if (spellword(L("lc")))
  return nvstemknown(L("lc"));
else
  return nvstemunk(L("lc"));
}

########
# FUNC:   NVSUFF
# SUBJ:   Compute suffix for noun or verb.
# NOTE:	  NLP++ stem() sucks, should do more of the below.
########

nvsuff(L("txt"))
{
if (!L("txt"))
  return L("txt");
L("lc") = strtolower(L("txt"));
L("stem") = nvstem(L("txt"));
if (L("stem") == L("lc"))
  return "inf";
# Got some kind of stemming.
if (strendswith(L("lc"),"s"))
  return "-s";
if (strendswith(L("lc"),"d"))
  return "-edn";
if (strendswith(L("lc"),"g"))
  return "-ing";
if (strendswith(L("lc"),"n"))
  return "-en";
}

########
# FUNC:   NVSTEMKNOWN
# SUBJ:   Compute stem for known word.
# NOTE:	NOT HANDLING IRREGULAR VERBS.
########

nvstemknown(L("lc"))
{
if (!L("lc"))
  return 0;

# EXCEPTIONS HERE.
# (They should be in the kb, etc.)
if (L("lc") == "ironed"
 || L("lc") == "ironing")
  return "iron";

# Look for EXTENSIONS in dictionary.
# Try adding an "-ing".
#L("x") = L("lc") + "ing";
#if (spellword(L("x")))
#  return L("lc");
#
# Try adding an "-s".
#L("x") = L("lc") + "s";
#if (spellword(L("x")))
#  return L("lc");

# Look for suffixes.
# Check doubled letters...

# -ss
if (strendswith(L("lc"),"ss"))
  return L("lc");


L("len") = strlength(L("lc"));
if (L("len") <= 3)
  return L("lc");	# As is.

if (L("len") > 4)
  {
  # -ies => -y
  # -ied => -y
  if (strendswith(L("lc"),"ies")
   || strendswith(L("lc"),"ied"))
    {
	# Try lopping off the s.
	L("x") = strpiece(L("lc"),0,L("len")-2);
	if (spellword(L("x")))
	  return L("x");

	# Lop off ies, add y.
	L("x") = strpiece(L("lc"),0,L("len")-4) + "y";
	if (spellword(L("x")))
	  return L("x");

	# Lop off es. (-ies => -i)
	L("x") = strpiece(L("lc"),0,L("len")-3);
	if (spellword(L("x")))
	  return L("x");

	# Nothing worked. Return as is.
	return L("lc");
	}

  # -ing =>
  # -ing => -e
  # doubled
  if (strendswith(L("lc"),"ing"))
    {
	L("x") = strpiece(L("lc"),0,L("len")-4);

	# Check doubling.
	if (strpiece(L("lc"),L("len")-4,L("len")-4)  ==
	    strpiece(L("lc"),L("len")-5,L("len")-5) )
	  {
	  # Check as is.
	  if (spellword(L("x")))
	    return L("x");

	  # Remove doubling.
	  L("y") = strpiece(L("x"),0,L("len")-5);
	  if (spellword(L("y")))
	    return L("y");

	  return L("lc");
	  }

	# Check -ing => -e
	L("y") = L("x") + "e";
	if (spellword(L("y")))
	  return L("y");

	# -ing => 0
	if (spellword(L("x")))
	  return L("x");

	# Nothing worked, return as is.
	return L("lc");
	}

  # -es  => -e
  # -es  => 0
  if (strendswith(L("lc"),"es"))
    {
	L("x") = strpiece(L("lc"),0,L("len")-3);
	
	L("y") = L("x") + "e";
	if (spellword(L("y")))
	  return L("y");

	if (spellword(L("x")))
	  return L("x");

	return L("lc");
	}

  # -ed => -e
  # doubled
  # -ed =>
  if (strendswith(L("lc"),"ed"))
    {
	L("x") = strpiece(L("lc"),0,L("len")-3);

	# Check doubling.
	if (strpiece(L("x"),L("len")-3,L("len")-3) ==
	    strpiece(L("x"),L("len")-4,L("len")-4))
	  {
	  L("y") = strpiece(L("x"),0,L("len")-4);
	  if (spellword(L("y")))
	    return L("y");
	  }

	# Add an e.
	L("y") = L("x") + "e";
	if (spellword(L("y")))
	  return L("y");

	if (spellword(L("x")))
	  return L("x");

	return L("lc");
	}

  }

# -s   =>
if (strendswith(L("lc"),"s"))
  {
  L("x") = strpiece(L("lc"),0,L("len")-2);
  if (spellword(L("x")))
    return L("x");
  }

return L("lc");
}

########
# FUNC:   NVSTEMUNK
# SUBJ:   Compute possible stem for unknown word.
########

nvstemunk(L("lc"))
{
if (!L("lc"))
  return 0;

# Look for suffixes.
# -ss
# -ies => -y
# -es  => -e
# -s   =>

# -ing =>
# -ing => -e

# -ied => -y
# -ed => -e
# -ed =>


}

########
# FUNC:   NODEFEAT
# SUBJ:   If node's stem has feature.
########

nodefeat(L("n"),L("feat"))
{
if (!L("n") || !L("feat"))
  return 0;

L("lctxt") = nodestem(L("n"));
if (!L("lctxt"))
  {
  "blupp.txt" << pnvar(L("n"),"$text") << "\n";
  return 0;
  }
L("lctxt") = strtolower(L("lctxt"));
return finddictattr(L("lctxt"),L("feat"));
}

########
# FUNC:   VERBFEAT
# SUBJ:   If verb's stem has feature.
########

verbfeat(L("n"),L("feat"))
{
if (!L("n") || !L("feat"))
  return 0;
L("nm") = pnname(L("n"));
if (L("nm") == "_vg")
  L("v") = pnvar(L("n"),"verb node");
else
  L("v") = L("n");

if (!L("v"))
  return 0;

L("lctxt") = nodestem(L("v"));
if (!L("lctxt"))
  {
  "blup.txt" << pnvar(L("n"),"$text") << "\n";
  return 0;
  }
L("lctxt") = strtolower(L("lctxt"));
return finddictattr(L("lctxt"),L("feat"));
}


########
# FUNC:   PHRPREPVERBQ
# SUBJ:   If possible phrasal/prepositional verb + particle.
########

phrprepverbq(L("nverb"),L("nprep"))
{
if (!L("nverb") || !L("nprep"))
  return 0;
L("tverb") = nodestem(L("nverb"));
L("tprep") = nodestem(L("nprep"));
return finddictattr(L("tverb"),L("tprep"));
}


########
# FUNC:   PHRASALVERBQ
# SUBJ:   If possible phrasal verb + particle.
########

phrasalverbq(L("nverb"),L("nprep"))
{
if (!L("nverb") || !L("nprep"))
  return 0;
L("tverb") = nodestem(L("nverb"));
L("tprep") = nodestem(L("nprep"));
L("num") = finddictattr(L("tverb"),L("tprep"));
if (L("num") == 1	# Phrasal
 || L("num") == 3)	# Both phrasal and prepositional.
 return 1;
return 0;
}

########
# FUNC:   PREPOSITIONALVERBQ
# SUBJ:   If possible prepositional verb + prep.
########

prepositionalverbq(L("nverb"),L("nprep"))
{
if (!L("nverb") || !L("nprep"))
  return 0;
L("tverb") = nodestem(L("nverb"));
L("tprep") = nodestem(L("nprep"));
L("num") = finddictattr(L("tverb"),L("tprep"));
if (L("num") == 2	# Prepositional.
 || L("num") == 3)	# Both phrasal and prepositional.
 return 1;
return 0;
}

########
# FUNC:   LJUST
# SUBJ:   Left-justify string in field.
########
ljust(L("str"),L("num"),L("out"))
{
if (!L("out"))
  return;
if (!L("str"))
  {
  indent(L("num"),L("out"));
  return;
  }
L("len") = strlength(L("str"));
L("out") << L("str");
L("diff") = L("num") - L("len");
indent(L("diff"),L("out"));
}

########
# FUNC:   INDENT
# SUBJ:   Print padding.
########
indent(L("num"),L("out"))
{
if (!L("num") || !L("out"))
  return;
while (L("num") > 0)
  {
  L("out") << " ";
  --L("num");
  }
}

########
# FUNC:   NODECOUNT
# SUBJ:   Count nodes in a list.
# NOTE:	  NLP++ todo: could be an internal like N("$count",1)
########

nodecount(L("start"),L("end"))
{
if (!L("start"))
  return 0;

L("ii") = 0;
if (L("end"))
  L("end") = pnnext(L("end"));
while (L("start") && L("start") != L("end"))
  {
  ++L("ii");
  L("start") = pnnext(L("start"));
  }
return L("ii");
}

########
# FUNC:   NODESTEXTOUT
# SUBJ:   Get text for nodes.
########

nodestextout(
	L("start"),
	L("end"),
	L("out")
	)
{
if (!L("start") || !L("out"))
  return;
if (L("end"))
  L("end") = pnnext(L("end"));
while (L("start") && L("start") != L("end"))
  {
  L("out") << " " << nodetext(L("start"));
  L("start") = pnnext(L("start"));
  }
}

########
# FUNC:   CLAUSELASTNP
# SUBJ:   See if clause ends in np.
########
clauselastnp(L("n"))
{
if (!L("n"))
  return 0;
L("nm") = pnname(L("n"));
if (L("nm") == "_np")
  return 1;
if (L("nm") == "_advl")
  {
  if (pnvar(L("n"),"pp")) # Prepositional phrase, np at end.
    return 1;
  return 0;
  }

L("last") = pnvar(L("n"),"last");
if (L("last"))
  return clauselastnp(L("last")); # Recursive call on component.

if (L("nm") != "_clause")
  return 0;

L("p") = pnvar(L("n"),"pattern");
if (!L("p"))
  return 0;

if (L("p") == "nvn"
 || L("p") == "vn"
 || L("p") == "n"
 )
  return 1;
return 0;
}

########
# FUNC:   CLAUSEINCOMPLETE
# SUBJ:   See if clause is transitive missing object.
# NOTE:	Eg, "John threw" which doesn't occur by itself, but
#	can follow in something like "There's the ball that John threw."
########
clauseincomplete(L("n"))
{
if (!L("n"))
  return 0;

# Todo: Need knowledge of transitive and intransitive verbs.
# Todo: Issues with prep/phrasal verb patterns also.
L("p") = pnvar(L("n"),"pattern");
if (!L("p"))
  return 0;

if (L("p") == "nv"
 || L("p") == "v"
 )
  return 1;
return 0;
}

########
# FUNC:   CLAUSECOMPLETE
# SUBJ:   See if clause can be standalone.
# NOTE:	Eg, (incomplete) "John threw" which doesn't occur by itself, but
#	can follow in something like "There's the ball that John threw."
########
clausecomplete(L("n"))
{
if (!L("n"))
  return 0;

# Todo: Need knowledge of transitive and intransitive verbs.
# Todo: Issues with prep/phrasal verb patterns also.
L("p") = pnvar(L("n"),"pattern");
if (!L("p"))
  return 0;

if (L("p") == "nvn"
 || L("p") == "vn"
 || L("p") == "nvnn"
 || L("p") == "vnn"
 || L("p") == "nvj"
 || L("p") == "vj"
 )
  return 1;

if (L("p") != "nv"
 && L("p") != "v"
 )
  return 0;

# See if can be intransitive.
L("vg") = pnvar(L("n"),"vg node");
if (!L("vg"))
  return 0;
L("v") = pnvar(L("vg"),"verb node");
if (!L("v"))
  return 0;
L("stem") = nodestem(L("v"));
"cl.txt" << "clcomplete=" << L("stem") << "\n";
if (!L("stem"))
  return 0;

if (finddictattr(L("stem"),"intr"))
  return 1;

# Todo: should be copula + necessary adverbial etc.
if (copulaq(L("v")))
  return 1;

return 0;
}

########
# FUNC:   MHBVADV
# SUBJ:   Look at adverbials of mhbv phrase.
########

mhbvadv(
	L("n x1"),
	L("n x2"),
	L("n x3"),
	L("n x4")
	)
{
if (L("n x1"))
  {
  L("a") = eltnode(L("n x1"));
  L("b") = lasteltnode(L("n x1"));
  L("neg") = attrinrange("neg",L("a"),L("b"));
  if (L("neg"))
    return 1;
  }
if (L("n x2"))
  {
  L("a") = eltnode(L("n x2"));
  L("b") = lasteltnode(L("n x2"));
  L("neg") = attrinrange("neg",L("a"),L("b"));
  if (L("neg"))
    return 1;
  }
if (L("n x3"))
  {
  L("a") = eltnode(L("n x3"));
  L("b") = lasteltnode(L("n x3"));
  L("neg") = attrinrange("neg",L("a"),L("b"));
  if (L("neg"))
    return 1;
  }
if (L("n x4"))
  {
  L("a") = eltnode(L("n x4"));
  L("b") = lasteltnode(L("n x4"));
  L("neg") = attrinrange("neg",L("a"),L("b"));
  if (L("neg"))
    return 1;
  }
return 0;
}

########
# FUNC:   ARRAYMERGE
# SUBJ:   Merge two arrays.
# NOTE:	  Only copying stuff if needed.
#	Does passing an array automatically copy it?
# WARN:	CALLER MUST DECIDE IF 0 MEANS EMPTY ARRAY
#		AND ACT ACCORDINGLY.
########

arraymerge(L("arr1"),L("arr2"))
{
#if (!L("arr1"))
#  return L("arr2");
#if (!L("arr2"))
#  return L("arr1");

L("len1") = arraylength(L("arr1"));
L("len2") = arraylength(L("arr2"));
L("merge") = L("arr1");	# COPY IT.
L("ii") = 0;
while (L("ii") < L("len2"))
  {
  L("merge")[L("len1") + L("ii")] = L("arr2")[L("ii")];
  ++L("ii");
  }
return L("merge");
}

########
# FUNC:   ARRAYREVERSE
# SUBJ:   Reverse order of elements in an array.
# NOTE:	  Only copying stuff if needed.
########

arrayreverse(L("arr"))
{
if (!L("arr"))
  return 0;
L("len") = arraylength(L("arr"));
if (L("len") == 1)
  return L("arr");
L("ii") = 0;
L("jj") = L("len") - 1;
L("rev") = 0;
while (L("ii") < L("len"))
  {
  L("rev")[L("ii")] = L("arr")[L("jj")];
  ++L("ii");
  --L("jj");
  }
return L("rev");
}

########
# FUNC:   PUSH
# SUBJ:   Push value onto array.
########
push(L("val"),L("arr"))
{
if (!L("arr"))
  return L("val");
L("len") = arraylength(L("arr"));
L("newarr")[0] = L("val");
L("ii") = 0;
while (L("ii") < L("len"))
  {
  L("newarr")[L("ii")+1] = L("arr")[L("ii")];
  ++L("ii");
  }
return L("newarr");
}

########
# FUNC:   RPUSH
# SUBJ:   Push value onto array end of array.
########

rpush(L("arr"),L("val"))
{
if (!L("arr"))
  return L("val");
L("len") = arraylength(L("arr"));
L("arr")[L("len")] = L("val");
return L("arr");
}
########
# FUNC:   PUSHZ
# SUBJ:   Push value onto array.
# NOTE:	 Treats zero as an array with one value.
########
pushz(L("val"),L("arr"))
{
#if (!L("arr"))
#  return L("val");
L("len") = arraylength(L("arr"));
L("newarr")[0] = L("val");
L("ii") = 0;
while (L("ii") < L("len"))
  {
  L("newarr")[L("ii")+1] = L("arr")[L("ii")];
  ++L("ii");
  }
return L("newarr");
}

########
# FUNC:   RPUSHZ
# SUBJ:   Push value onto array end of array.
# NOTE:	 Treats zero as an array with one value.
########

rpushz(L("arr"),L("val"))
{
#if (!L("arr"))
#  return L("val");
L("len") = arraylength(L("arr"));
L("arr")[L("len")] = L("val");
return L("arr");
}

######## PRINTCH
# SUBJ: Print given number of chars.
###################
printch(L("ch"),L("num"),L("out"))
{
if (!L("ch") || !L("num") || !L("out"))
  return;
while (L("num") > 0)
  {
  L("out") << L("ch");
  --L("num");
  }
}

######## RJX
# SUBJ: Right-justify something to print.
###################
rjx(
	L("item"),	# Item to print.
	L("field"),	# Length of field to print in.
	L("ch"),	# Leading char.
	L("out")
	)
{
if (!L("field") || !L("out"))
  return;
if (!L("ch"))
  L("ch") = " "; # Default is space.

L("str") = str(L("item"));  # Must be forced to string.
L("len") = strlength(L("str"));
L("ii") = L("field") - L("len");
while (L("ii") > 0)
  {
  L("out") << L("ch");
  --L("ii");
  }
L("out") << L("str");
}

######## CTR
# SUBJ: Pretty-print centered in a field.
###################
ctr(
	L("item"),	# Item to print.
	L("field"),	# Length of field to print in.
	L("ch"),	# Surrounding chars.
	L("out")
	)
{
if (!L("field") || !L("out"))
  return;
if (!L("ch"))
  L("ch") = " "; # Default is space.

L("str") = str(L("item"));  # Must be forced to string.
L("len") = strlength(L("str"));

# Figure out preceding chars.
L("pad") = L("field") - L("len");
L("pre") = L("pad")/2;
L("post") = L("pad") - L("pre");

L("ii") = L("pre");
while (L("ii") > 0)
  {
  L("out") << L("ch");
  --L("ii");
  }
L("out") << L("str");

if (!L("post"))
  return;
if (L("ch") == " ")
  return;

L("ii") = L("post");
while (L("ii") > 0)
  {
  L("out") << L("ch");
  --L("ii");
  }
}

########
# FUNC:   PNFINDCHILD
# SUBJ:   Find child node by name.
# NOTE:	  Fetch the first one found.
########
pnfindchild(L("n"),L("name"))
{
if (!L("n") || !L("name"))
  return 0;
L("n") = pndown(L("n"));
if (!L("n"))
  return 0;
while (L("n"))
  {
  if (L("name") == pnname(L("n")))
    return L("n");
  L("n") = pnnext(L("n"));
  }
return 0;
}

########
# FUNC:   KBCHILDRENTOARRAY
# SUBJ:   Copy kb children to an array.
# RET:	  Array.
########

kbchildrentoarray(
	L("parent")	# Parent concept.
	)
{
if (!L("parent"))
  return 0;
L("n") = down(L("parent"));
L("arr") = 0;
L("ii") = 0;
while (L("n"))
  {
  L("arr")[L("ii")] = L("n");
  ++L("ii");
  L("n") = next(L("n"));
  }
return L("arr");
}

@CODE
L("hello") = 0;
@@CODE
