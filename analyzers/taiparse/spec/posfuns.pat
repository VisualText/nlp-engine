###############################################
# FILE: posfuns.pat
# SUBJ: Functions re part of speech.
# AUTH: AM
# CREATED: 01/Sep/04 08:36:17
# MODIFIED:
###############################################

@DECL
########
# FUNC:	CLEARPOS
# SUBJ:	Mark as not receiving a part of speech.
# NOTE: Find parts-of-speech below this node, if any.
########
clearpos(
	L("n"),	# Node to clear.
	L("ignorepos"),	# If putting the ignore feature on.
	L("bracket")	# If putting noun phrase bracket on.
	)
{
if (!L("n"))
  return;

if (L("ignorepos") != pnvar(L("n"),"ignorepos"))
  pnreplaceval(L("n"),"ignorepos",L("ignorepos"));
if (L("bracket") != pnvar(L("n"),"bracket"))
  pnreplaceval(L("n"),"bracket",L("bracket"));

if (pnvar(L("n"),"posarr len"))
  pnreplaceval(L("n"),"posarr len",0);
if (pnvar(L("n"),"nopos"))
  pnreplaceval(L("n"),"nopos",0);
if (pnvar(L("n"),"mypos"))
  pnreplaceval(L("n"),"mypos",0);
}
########
# FUNC:	SCLEARPOS
# SUBJ:	Mark sugg as not receiving a part of speech.
# NOTE: Find parts-of-speech below this node, if any.
########
sclearpos(
	L("ignorepos"),	# If putting the ignore feature on.
	L("bracket")	# If putting noun phrase bracket on.
	)
{
if (L("ignorepos") != S("ignorepos"))
  S("ignorepos") = L("ignorepos");
if (L("bracket") != S("bracket"))
  S("bracket") = L("bracket");

if (S("posarr len"))
  S("posarr len") = 0;
if (S("nopos"))
  S("nopos") = 0;
if (S("mypos"))
  S("mypos") = 0;
}

########
# FUNC:	SAMEPOSRANGE
# SUBJ:	Intersection of pos for a range of nodes.
# RET:	L("posarr") = array of intersected pos values.
########

sameposrange(L("start"),L("end"))
{
if (!L("start"))
  return 0;
if (L("end"))
  L("end") = pnnext(L("end"));	# Easy end boundary.
L("n") = L("start");
L("posarr") = pnvar(L("n"),"posarr");	# Start with first.
L("n") = pnnext(L("n"));
while (L("n") != L("end"))
  {
  # Intersect pos arrays.
  L("posarr") = intersect(L("posarr"),pnvar(L("n"),"posarr"));
  if (!L("posarr"))	# No intersection.
    return 0;
  L("n") = pnnext(L("n"));
  }

return L("posarr");
}

########
# FUNC:	POSACCTINI
# SUBJ:	Accounting for rule performance.
########

posacctini()
{
if (!G("posacct"))
  return;
# Set up a global area.
G("posacct root") = getconcept(findroot(),"posacct");
}

########
# FUNC:	POSACCT
# SUBJ:	Accounting for rule performance.
########

posacct(L("n"))
{
if (!G("posacct"))
  return;
if (!L("n"))
  return;

L("rule") = str(G("$rulenum"));
if (!L("rule"))
  return;	# Not inside a rule.

L("nm") = pnname(L("n"));
L("ln") = strlength(L("nm"));

if (strpiece(L("nm"),0,0) == "_")
  L("nm") = strpiece(L("nm"),1,L("ln")-1);
L("npos") = nodepos(L("n"),L("nm"));

# Get kb concept for pass.
L("pass") = str(G("$passnum"));
L("cpass") = getconcept(G("posacct root"),L("pass"));

# Get kb concept for rule.
L("crule") = getconcept(L("cpass"),L("rule"));

# Keep global count.
L("fired") = numval(L("crule"),"fired");
replaceval(L("crule"),"fired",++L("fired"));

# Score
L("arr") = pnvar(L("n"),"posarr");
#"posdump.txt" << str(L("npos")) << "\t" << L("arr")[0] << "\n";
if (L("arr")[0] == L("npos"))	# Correct.
  {
  L("good") = numval(L("crule"),"good");
  replaceval(L("crule"),"good",++L("good"));
#  ++G("good arr")[G("$rulenum")];
  }
}

########
# FUNC:	POSACCTDUMP
# SUBJ:	Dump pos rule accounting for a pass.
########

posacctdump()
{
if (!G("posacct") || !G("posacct root"))
  return;

L("fname") = G("$apppath") + "\\data\\posdump.txt";
L("out") = openfile(L("fname"));

L("out")
  << "pass   rule        good        total      pct" <<"\n"
  << "=============================================" << "\n";

# Now storing in kb.
# For each pass.
L("cpass") = down(G("posacct root"));
while (L("cpass"))
  {
  # For each rule in pass.
  L("crule") = down(L("cpass"));
  while (L("crule"))
    {
	# Print rule stats.
	L("gd") = numval(L("crule"),"good");
	L("tt") = numval(L("crule"),"fired");
	if (L("tt"))
	  L("pct") = 100 * L("gd") / L("tt");
	else
	  L("pct") = 0;
	L("out")
		<< rightjustifynum(num(conceptname(L("cpass"))),4) << "\t"
		<< rightjustifynum(num(conceptname(L("crule"))),4) << "\t"
		<< rightjustifynum(L("gd"),9) << "\t"
		<< rightjustifynum(L("tt"),9) << "\t"
		<< rightjustifynum(L("pct"),4)
		<< "\n"
		;

	L("crule") = next(L("crule"));
	}
  L("cpass") = next(L("cpass"));
  }

if (L("out"))
  closefile(L("out"));
}

########
# FUNC:	CHPOS
# SUBJ:	Rudimentary set pos.
# NOTE:	For accounting convenience.
# 	Note there's a SETPOS function in this pass, more complex.
########

chpos(L("n"),L("pos"))
{
if (!L("n"))
  return;
if (L("pos"))
  {
  pnreplaceval(L("n"),"mypos",L("pos"));
  posacct(L("n"));
  }
else if (pnvar(L("n"),"mypos"))
  pnreplaceval(L("n"),"mypos",0);
}

########
# FUNC:	NODEPOS
# SUBJ:	Get part of speech for node.
########

nodepos(L("n"),L("name"))
{
if (!L("n") || !L("name"))
  return 0;
if (pnvar(L("n"),"ignorepos"))
  return 0;
L("pos") = pnvar(L("n"),"mypos");
if (L("pos"))
  return L("pos");
if (L("name") == "conj")
  return "CC";
if (L("name") == "det")
  return "DT";
if (L("name") == "num")
  return "CD";
if (L("name") == "interj")
  return "UH";
if (L("name") == "prep")
  {
  L("txt") = strtolower(pnvar(L("n"),"$text"));
  if (L("txt") == "to")
    return "TO";
  return "IN";
  }
if (L("name") == "adj")
  {
  L("txt") = strtolower(pnvar(L("n"),"$text"));
  L("myp") = adjconj(L("txt"));
  if (L("myp"))
    return L("myp");
  return "JJ";
  }
if (L("name") == "adv")
  {
  if (pnvar(L("n"),"comparative"))
    return "RBR";
  if (pnvar(L("n"),"superlative"))
    return "RBS";
  fixadv(L("n"));	# 06/19/06 AM.
  L("pos") = pnvar(L("n"),"mypos");
  if (L("pos"))
    return L("pos");
  return "RB";
  }
if (L("name") == "pro")
  return "PP";
if (L("name") == "noun")
  {
#  "dump.txt" << pnvar(L("n"),"$text") << " ";
  L("cap") = pnvar(L("n"),"cap");
  if (pnvar(L("n"),"newsent"))
    L("cap") = 0;	# 07/11/06 AM.
  if (!(L("num") = pnvar(L("n"),"number") ))
    L("num") = number(L("n"));
#  "dump.txt" << L("num") << "\n";
  if (L("num") == "singular")
    {
	if (L("cap"))
	  return "NP";
	else
      return "NN";
	}
  else if (L("num") == "plural")
    {
	if (L("cap"))
	  return "NPS";
	else
      return "NNS";
	}
  return "noun";
  }
if (L("name") == "verb")
  {
  if (pnvar(L("n"),"-en"))
    return "VBN";
  if (pnvar(L("n"),"-s"))
    return "VBZ";
  if (pnvar(L("n"),"inf"))
    return "VB";
  if (pnvar(L("n"),"-ing"))
    return "VBG";
  if (pnvar(L("n"),"-ed"))
    return "VBD";
  # VBP ... don't have a marker for this yet.
  return 0;
  }
return 0;
}

########
# FUNC:	SCORENODEPOS
# SUBJ:	Score a single part-of-speech against key.
########

scorenodepos(L("n"),L("npos"))
{
if (!G("scorepos") || !L("n"))  # FIX. # 09/09/05 AM.
	return 0;

L("arr") = pnvar(L("n"),"posarr");

# Update score.
L("num") = numval(G("scorepos"),"currtot");
replaceval(G("scorepos"),"currtot",++L("num"));

if (L("arr")[0] == L("npos"))
  {
  # Update score.
  L("num") = numval(G("scorepos"),"currgood");
  replaceval(G("scorepos"),"currgood",++L("num"));
  return 1;
  }
return 0;
}

########
# FUNC:	FINDVEN
# SUBJ:	Search inside a vg for first verb, which must be ven.
# RET:	L("ven") = ven-able verb, if found.
########

findven(L("n"))
{
# Traverse down to find verb.
L("done") = 0;
while (!L("done"))
  {
  if (!L("n"))
    return 0;
  L("nm") = pnname(L("n"));
  if (L("nm") == "_vg")
    L("n") = pndown(L("n"));
  else
    L("done") = 1;
  }

# Here we're within a vg.  Todo: traverse...
L("done") = 0;
while (!L("done"))
  {
  if (L("nm") == "_verb")
    L("done") = 1;
  else
    {
	if (L("n") = pnnext(L("n")))
	  L("nm") = pnname(L("n"));
	else
	  L("done") = 1;
	}
  }
# At a verb.
# Could be "having (been) eaten..."
# Else should not be "have" or "be".
if (vconjq(L("n"),"-en"))
  return L("n");

return 0;
}

########
# FUNC:	VGAMBIGEDN
# SUBJ:	See if verb group is ambiguous wrt -edn.
# CR:	06/13/06 AM.
# RET:	Verb node, if ambiguous wrt -edn.
########

vgambigedn(L("vg"))
{
if (!L("vg"))
  return 0;
if (pnvar(L("vg"),"voice") == "active")
  return 0;

# NOT the first verb in the group.
L("v") = pnvar(L("vg"), "verb node");
if (!L("v"))
  {
  "err.txt" << "[No verb node ptr: " << pnvar(L("vg"),"$text")
  	<< "\n";
  return 0;
  }

if (pnvar(L("v"),"mypos"))
  return 0;

L("vc") = vconj(L("v"));
if (L("vc") == "-edn")
  return L("v");
return 0;
}

########
# FUNC:	MHBV
# SUBJ:	Handle verb group in a rule.
# CR:	11/26/04 AM.
# RET:	
# NOTE:	Update the group node.
########

mhbv(L("n"),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"))
{
if (!L("n"))
  return 0;

if (L("neg"))
  pnreplaceval(L("n"),"neg",L("neg"));

if (!L("m") && !L("h") && !L("b") && !L("being") && !L("v"))
  {
  if (G("error"))
  "err.txt" << "[No verb parts: " << pnvar(L("n"),"$text")
  	<< "]\n";
  return 0;
  }

L("voice") = mhbvfix(L("n"),L("m"),L("h"),L("b"),L("being"),L("v"));

if (L("voice"))
  pnreplaceval(L("n"),"voice",L("voice"));

semvg(L("n"),L("v"));

if (L("v"))
  L("vn") = L("v");
else if (L("b"))
  L("vn") = L("b");
else if (L("h"))
  L("vn") = L("h");

if (L("vn"))
  {
  pnreplaceval(L("n"),"verb node",L("vn"));
  L("stem") = pnvar(L("vn"),"stem");
  if (L("stem"))
    pnreplaceval(L("n"),"stem",L("stem"));
  else
    pnreplaceval(L("n"),"stem",pnvar(L("vn"),"$text"));
  }
else
  {
  # Get stem.
  if (L("being"))
    L("sv") = L("being");
  else
    L("sv") = L("m");
  L("stem") = pnvar(L("sv"),"stem");
  if (L("stem"))
    pnreplaceval(L("n"),"stem",L("stem"));
  else
    pnreplaceval(L("n"),"stem",pnvar(L("sv"),"$text"));
  }

# First verb.	# 01/04/05 AM.
if (L("m"))
  {
  L("mstem") = pnvar(L("m"),"stem");	# 06/05/06 AM.
  if (L("mstem") == "do")	# 06/05/06 AM.
    L("first") = L("m");	# 06/05/06 AM.
  else
    L("first") = 0;	# NO "first" verb to conjugate. # 02/26/05 AM.
  if (L("h"))
    chpos(L("h"),"VB");
  else if (L("b"))
    chpos(L("b"),"VB");
  else if (L("v"))
    chpos(L("v"),"VB");
  }
else if (L("h"))
  {
  L("first") = L("h");
  if (pnvar(L("h"),"-edn")) # had
    chpos(L("h"),"VBD");
  }
else if (L("b"))
  L("first") = L("b");
else if (L("v"))
  L("first") = L("v");

if (L("first"))
  pnreplaceval(L("n"),"first verb",L("first"));

# Finite vs nofinite.
# (shallow assignment based on mhbv alone, not clause).

return L("voice");
}

########
# FUNC:	MHBVFIX
# SUBJ:	Fix up a verb group.
# RET:	"active", "passive", or 0 if ambiguous.
# NOTE: 02/20/05 AM. Handling ambiguous words like "set".
########

mhbvfix(L("n"),L("m"),L("h"),L("b"),L("being"),L("v"))
{
if (!L("n"))
  {
  if (G("error"))
  "err.txt" << "[mhbvfix error:] " << phrasetext() << "\n";
  return 0;
  }

if (!L("v"))
  {
  if (G("error"))
  "err.txt" << "[mhbvfix error(2):] " << phrasetext() << "\n";
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),0);
  return 0;
  }

# If assigned already, handle that...
L("assig") = 0;	# Track if verb has hard-wired assignments.

if (pnvar(L("v"),"-s"))
  {
  ++L("assig");
  if (!L("m") && !L("h") && !L("b") && !L("being"))
    {
    chpos(L("v"),"VBZ");
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-s");
    return "active";	# eats.
	}
  # will/have/been/being eats.
  # Look for an alternative...
  }
 
if (pnvar(L("v"),"-ing"))
  {
  ++L("assig");
  if (L("b") || L("being"))
    {
    chpos(L("v"),"VBG");
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-ing");
    return "active";	# been/being eating.
	}
  if (!L("m") && !L("h"))
    {
    chpos(L("v"),"VBG");
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-ing");
    return "active";	# eating.
	}
  # will/have eating.
  # Check alternative...
  }

if (pnvar(L("v"),"-en"))
  {
  ++L("assig");
  if (L("b") || L("being"))
    {
    chpos(L("v"),"VBN");
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-en");
    return "passive"; 	# been eaten.
	}
  if (L("h"))
    {
    chpos(L("v"),"VBN");
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-en");
    return "active"; 	# have eaten.
	}
  # will eaten ... so look for alternative.
  }

if (pnvar(L("v"),"-ed"))
  {
  ++L("assig");
  if (!L("b") && !L("being") && !L("h") && !L("m"))
    {
    chpos(L("v"),"VBD");	# ate.
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-ed");
    return "active";
	}
  # will/have/been ate.  Informal, disagree, or error.
  # Look for alternative...
  }

if (pnvar(L("v"),"-edn"))
  {
  ++L("assig");
  if (L("b") || L("being"))
    {
    chpos(L("v"),"VBN");
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-en");
    return "passive"; 	# been worked.
	}
  if (L("h"))
    {
    chpos(L("v"),"VBN");
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-en");
    return "active"; 	# have worked.
	}
  if (!L("m"))
    {
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-edn");
    return 0;	# worked.	Ambiguous.
	}
  # will worked ... so look for alternative.
  }

if (pnvar(L("v"),"inf"))	# 01/05/05 AM.
  {
  ++L("assig");
  # Unambiguous infinitive.
  if (!L("h") && !L("b") && !L("being"))
    {
	if (L("m"))	# will work.
	  {
      chpos(L("v"),"VB");  # will eat.
	  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"inf");
	  return "active";
	  }
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"inf");
	return "active";	# be or am.
	}
  # have/been/being work...
  }

if (L("assig"))	# Hard-wired conjugations.
	{
	fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"inf");
	return 0;	# Didn't work out.
	}

# Should assume regular verbs below here.	# 02/20/05 AM.
# Irregs should all be hardwired, handled above.

L("x") = vconj(L("v"));

if (L("x") == "inf")
  {
  pnreplaceval(L("v"),"inf",1);

  # Unambiguous infinitive.
  if (L("m") && !L("h") && !L("b") && !L("being"))
    chpos(L("v"),"VB");
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"inf");
  return "active";
  }
if (L("x") == "-s")
  {
  pnreplaceval(L("v"),"-s",1);
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-s");
  return "active";
  }
if (L("x") == "-ed")
  {
  pnreplaceval(L("v"),"-ed",1);
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-ed");
  return "active";
  }
if (L("x") == "-en")
  {
  pnreplaceval(L("v"),"-en",1);
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-en");
  return "passive";
  }
if (L("x") == "-ing")
  {
  pnreplaceval(L("v"),"-ing",1);
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-ing");
  return "active";
  }

# -edn
 
if (!L("h") && !L("b") && !L("being"))
  {
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-edn");
  return 0; # ambiguous.
  }
if (L("x") != "-edn")
  return 0; # error.
pnreplaceval(L("v"),"-en",1);
pnreplaceval(L("v"),"-edn",0);
chpos(L("v"),"VBN");	# 06/02/06 AM.
if (L("b") || L("being"))
  {
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-en");
  return "passive";
  }
else if (L("h"))
  {
  fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-en");
  return "active";
  }
fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),"-edn");
return 0;
}

########
# FUNC:	FIXVGATTRS
# SUBJ:	Fix up tense and aspect for a verb group.
# NOTE: Will overwrite even if exists.
########

fixvgattrs(L("n"),L("m"),L("h"),L("b"),L("being"),L("vconj"))
{
if (!L("vconj")|| !L("n"))
  {
  if (G("error"))
  "err.txt" << "[fixvgattrs error: " << phrasetext() << "]\n";
  return 0;
  }

if (L("m"))
  pnreplaceval(L("n"),"tense","future");
else if (L("h"))
  pnreplaceval(L("n"),"tense",pnvar(L("h"),"tense"));
else if (L("b"))
  pnreplaceval(L("n"),"tense",pnvar(L("b"),"tense"));
else if (L("being"))
  pnreplaceval(L("n"),"tense","present");
else if (L("vconj") == "-ed"
 || L("vconj") == "-en"
 || L("vconj") == "-edn")
  pnreplaceval(L("n"),"tense","past");
else
  pnreplaceval(L("n"),"tense","present");

if (L("vconj") == "-s")
  pnreplaceval(L("n"),"aspect","none");
else if (L("b") && L("vconj") == "-en") # Passive.
  {
  if (L("being") && L("h"))
    pnreplaceval(L("n"),"aspect","perfective-progressive");
  else if (L("h"))
    pnreplaceval(L("n"),"aspect","perfective");
  else if (L("being"))
    pnreplaceval(L("n"),"aspect","progressive");
  else
    pnreplaceval(L("n"),"aspect","none");
  }
else # Active.
  {
  if (L("b") && L("h"))
    pnreplaceval(L("n"),"aspect","perfective-progressive");
  else if (L("h"))
    pnreplaceval(L("n"),"aspect","perfective");
  else if (L("b") || L("vconj") == "-ing")
    pnreplaceval(L("n"),"aspect","progressive");
  else
    pnreplaceval(L("n"),"aspect","none");
  }
}

########
# FUNC:	FIXVG
# SUBJ:	Fix up a verb group.
# RET:	"active", "passive", or 0 if ambiguous.
# NOTE: 04/21/07 AM. Allowing for list of vgs.
########

fixvg(L("vg"),L("active/passive"),L("pos"))
{
if (!L("vg"))
  return;
L("fst") = pnvar(L("vg"),"first vg");
if (!L("fst"))
 return fixvgsimple(L("vg"),L("active/passive"),L("pos"));

# Traverse the list of vgs.
L("lst") = pnvar(L("vg"),"last vg");
if (L("lst"))
   L("lst") = pnnext(L("lst")); # End mark.

L("vg") = L("fst");
while (L("vg") && (L("vg") != L("lst")))
  {
  fixvg(L("vg"),L("active/passive"),L("pos"));
  L("vg") = pnnext(L("vg"));
  }
}

########
# FUNC:	FIXVGSIMPLE
# SUBJ:	Fix up a verb group.
# RET:	"active", "passive", or 0 if ambiguous.
# NOTE: 04/21/07 AM. Allowing for list of vgs.
########

fixvgsimple(L("vg"),L("active/passive"),L("pos"))
{
if (!L("vg"))
  return;
if (L("tmp") = pnvar(L("vg"),"first vg"))
  return;	# Too nested to bother with, for now.

pnreplaceval(L("vg"),"voice",L("active/passive"));
L("v") = pnvar(L("vg"),"first verb");
if (!L("v"))
  L("v") = pnvar(L("vg"),"verb node");
if (L("v"))
  fixverb(L("v"),L("active/passive"),L("pos"));
}

########
# FUNC:	FIXVERB
# SUBJ:	Fix up a lone verb.
########

fixverb(L("v"),
	L("active/passive"), # What to do if ambiguous.
	L("pos")	# A reference pos, mainly from previous verb.
	)
{
if (!L("v"))
  return;

L("dn") = pndown(L("v"));
#if (pnname(L("dn")) == "_verb")
# "grunk.txt" << "_verb: " << pnname(pndown(L("dn"))) << "\n";
#else
# "grunk.txt" << pnname(L("dn")) << "\n";
#if (pnname(L("dn")) == "believes")
#  exitpass();	# DEBUGGING!!!!

# No defaults. 
#if (!L("active/passive"))
#  L("active/passive") = "active";
if (pnvar(L("v"),"mypos"))
  return;

# If assigned already, done.
if (pnvar(L("v"),"-en"))
  {
  chpos(L("v"),"VBN");
  return;
  }
if (pnvar(L("v"),"-ed"))
  {
  chpos(L("v"),"VBD");
  return;
  }
if (pnvar(L("v"),"-s"))
  {
  chpos(L("v"),"VBZ");
  return;
  }
if (pnvar(L("v"),"-ing"))
  {
  chpos(L("v"),"VBG");
  return;
  }

L("x") = vconj(L("v"));
if (L("x") == "inf")
  {
# "grunk.txt" << "inf=" << pnname(L("dn")) << "," << L("pos") << "\n";
  pnreplaceval(L("v"),"inf",1);
  if (L("pos") == "VB" || L("pos") == "VBP")
    chpos(L("v"),L("pos"));
  return;
  }
if (L("x") == "-s")
  {
  pnreplaceval(L("v"),"-s",1);
  chpos(L("v"),"VBZ");
  return;
  }
if (L("x") == "-ed")
  {
  pnreplaceval(L("v"),"-ed",1);
  chpos(L("v"),"VBD");
  return;
  }
if (L("x") == "-en")
  {
  pnreplaceval(L("v"),"-en",1);
  chpos(L("v"),"VBN");
  return;
  }
if (L("x") == "-ing")
  {
  pnreplaceval(L("v"),"-ing",1);
  chpos(L("v"),"VBG");
  return;
  }

if (L("x") != "-edn")
  return; # Some kind of error or special case...
if (L("active/passive") == "passive" || L("pos") == "VBN")
  {
  pnreplaceval(L("v"),"-en",1);
  pnreplaceval(L("v"),"-edn",0);
  chpos(L("v"),"VBN");
  }
else if (L("active/passive") == "active" # 10/09/04 AM.
 || L("pos") == "VBD" )
  {
  pnreplaceval(L("v"),"-ed",1);
  pnreplaceval(L("v"),"-edn",0);
  chpos(L("v"),"VBD");
  }

}

########
# FUNC:	FIXAPOSS
# SUBJ:	Fix up an apostrophe-S to a verb
########

fixaposs(L("vg"),L("v"))
{
if (!L("vg") || !L("v"))
  return;
if (!pnvar(L("v"),"apos-s"))
  return;

pnreplaceval(L("vg"),"stem","be");
pnreplaceval(L("vg"),"sem","be");
pnreplaceval(L("vg"),"person",3);
pnreplaceval(L("vg"),"number","singular");
pnreplaceval(L("vg"),"tense","present"); # Maybe.
pnreplaceval(L("vg"),"voice","active");

}

########
# FUNC:	CLAUSEPOS
# SUBJ:	Get verb pos for clause.
# NOTE:	For pos scoring, etc.
########

clausepos(L("clause"))
{
if (!L("clause"))
  return 0;

	if (N("last vg",1))
	  L("vg") = N("last vg",1);
	else if (N("vg node",1))
	  L("vg") = N("vg node",1);
return vgpos(L("vg"));
}

########
# FUNC:	VGPOS
# SUBJ:	Get verb pos for vg.
# NOTE:	For pos scoring, etc.
########

vgpos(L("vg"))
{
if (!L("vg"))
  return 0;
	  L("v") = pnvar(L("vg"),"verb node");
	if (L("v"))
	  L("pos") = pnvar(L("v"),"mypos");
return L("pos");
}

########
# FUNC:	FIXNOUNS
# SUBJ:	Fixup nouns in an np.
# NOTE:	For pos scoring, etc.
########

fixnouns(
	L("first"),
	L("last"))
{
if (!L("first") || !L("last"))
  return;
L("n") = L("first");
while (L("n") && (L("n") != L("last")))
  {
  L("pos") = pnvar(L("n"),"pos_np");
  if (L("pos"))
    chpos(L("n"),L("pos"));
  else
    fixnoun(L("n"));
  L("n") = pnnext(L("n"));
  }

if (L("n") != L("last"))
  {
  if (G("error"))
  "err.txt" << "[Error in fixnouns.]" << "\n";
  exitpass();
  }
# Fix head noun.
#fixnoun(L("last"));
fixnounhead(L("last"));
}

########
# FUNC:	FIXNOUN
# SUBJ:	Fixup noun info.
# NOTE:	For pos scoring, etc.
#	Need to account for first word of sentence.
#	Need to account for mass nouns.
########

fixnoun(L("n"))
{
if (!L("n"))
  return;
if (pnvar(L("n"),"mypos"))
  return;	# 01/15/05 AM.
if (pnvar(L("n"),"ignorepos"))
  return;	# 06/10/06 AM.

L("cap") = pnvar(L("n"),"cap");
L("newsent") = pnvar(L("n"),"newsent");
L("unknown") = pnvar(L("n"),"unknown");
if (!(L("num") = pnvar(L("n"),"number") ))
   L("num") = number(L("n"));

if (L("num") == "singular")
    {
	if (L("cap") && (L("unknown") || !L("newsent")))
	  chpos(L("n"),"NP");
	else
      chpos(L("n"),"NN");
	}
else if (L("num") == "plural")
    {
	if (L("cap") && (L("unknown") || !L("newsent")))
	  chpos(L("n"),"NPS");
	else
      chpos(L("n"),"NNS");
	}
}

########
# FUNC:	FIXNOUNNONHEAD
# SUBJ:	Fixup nonhead noun in an np.
# NOTE:	For pos scoring, etc.
########

fixnounnonhead(
	L("noun"))
{
if (!L("noun"))
  return;
L("pos") = pnvar(L("noun"),"pos_np");
if (L("pos"))
  chpos(L("noun"),L("pos"));
else
  fixnoun(L("n"));
}

########
# FUNC:	FIXNOUNHEAD
# SUBJ:	Fixup head noun in an np.
# NOTE:	For pos scoring, etc.
########

fixnounhead(
	L("noun"))
{
if (!L("noun"))
  return;
L("pos") = pnvar(L("noun"),"pos_np_head");
if (L("pos"))
  chpos(L("noun"),L("pos"));
else
  fixnoun(L("noun"));
}

########
# FUNC:	FIXADJS
# SUBJ:	Fixup adjs in an np.
# CR:	01/20/05 AM.
# NOTE:	For pos scoring, etc.
#	Assumes adj is not head of noun phrase...
########

fixadjs(
	L("first"),
	L("last"))
{
if (!L("first") || !L("last"))
  return;
L("n") = L("first");
if (L("last"))
  L("last") = pnnext(L("last"));
while (L("n") != L("last"))
  {
  L("pos") = pnvar(L("n"),"pos_np");
  if (pnname(L("n")) != "_adj")
    ;  # Skip it.
  else if (pnvar(L("n"),"ignorepos"))
    ;  # Skip it.
  else if (L("pos"))
    chpos(L("n"),L("pos"));
  else
    fixadj(L("n"));
  L("n") = pnnext(L("n"));
  }
}

########
# FUNC:	FIXADJ
# SUBJ:	Fixup adj.
# NOTE:	For pos scoring, etc.
########

fixadj(L("n"))
{
if (!L("n"))
  return;
if (pnvar(L("n"),"mypos"))
  return;

fixvadj(L("n"));
if (pnvar(L("n"),"mypos"))
  return;

# Else get comparative, superlative.
L("txt") = nodetext(L("n"));
if (!L("txt"))
  return;
if (L("myp") = adjconj(L("txt")))
  chpos(L("n"),L("myp"));
}

########
# FUNC:	FIXVADJ
# SUBJ:	Fixup adj that's also a ven or ving.
# NOTE:	For pos scoring, etc.
########

fixvadj(L("n"))
{
if (!L("n"))
  return;
if (pnvar(L("n"),"mypos"))
  return;

# Use preference if any.	# 10/16/04 AM.
# Assuming we're in a noun phrase...
if (L("pos") = pnvar(L("n"),"pos_np"))
  {
  chpos(L("n"),L("pos"));
  return;
  }

if (pnvar(L("n"),"hyphenated"))
  {
  chpos(L("n"),"JJ");
  return;
  }

if (!pnvar(L("n"),"verb"))
  return;	# 10/09/06 AM.

L("vc") = vconj(L("n"));
if (L("vc") == "-en" || L("vc") == "-edn")
  chpos(L("n"),"VBN");
else if (L("vc") == "-ing")
  chpos(L("n"),"VBG");

}

########
# FUNC:	ADJCONJ
# SUBJ:	Get adjective form.
# NOTE:	For pos scoring, etc.
#	Short for adj conjugation.
########

adjconj(L("word"))
{
if (!L("word"))
  return 0;
if (finddictattr(L("word"),"adj-plain"))
  return "JJ";	# 01/15/05 AM.
if (strendswith(L("word"),"est"))
  return "JJS";
if (strendswith(L("word"),"er"))
  {
  # Watch for exceptions.
  L("len") = strlength(L("word"));
  if (L("len") < 3)
    return 0;
  if (strpiece(L("word"),
  	L("len")-3, L("len")-3) == "e")
    return 0;
  return "JJR";
  }
return "JJ";
}
########
# FUNC:	FIXADV
# SUBJ:	Fixup adverb.
# NOTE:	For pos scoring, etc.
########

fixadv(L("n"))
{
if (!L("n"))
  return;
if (pnvar(L("n"),"mypos"))
  return;

# Get comparative, superlative.
L("txt") = nodetext(L("n"));
if (!L("txt"))
  return;
if (L("myp") = advconj(L("txt")))
  chpos(L("n"),L("myp"));
}

########
# FUNC:	ADVCONJ
# SUBJ:	Get adverb form.
# NOTE:	For pos scoring, etc.
#	Short for adverb conjugation.
########

advconj(L("word"))
{
if (!L("word"))
  return 0;
if (finddictattr(L("word"),"adv-plain"))
  return "RB";
if (strendswith(L("word"),"est"))
  return "RBS";
if (strendswith(L("word"),"er"))
  {
  # Watch for exceptions.
  L("len") = strlength(L("word"));
  if (L("len") < 3)
    return 0;
  if (strpiece(L("word"),
  	L("len")-3, L("len")-3) == "e")
    return 0;
  return "RBR";
  }
return "RB";
}

########
# FUNC:	FIXNPNONHEAD
# SUBJ:	Fix adj,noun,alpha in np. Non-head node.
# NOTE:	For pos scoring, etc.
# EFFECT: Assumes we're in rule match.
########

fixnpnonhead(
	L("ord")	# Position in current rule.
	)
{
L("node") = eltnode(L("ord"));
if (!L("node") || !L("ord"))
  return;
L("name") = pnname(L("node"));
if (L("name") == "_adj")
  fixadjs(L("node"),L("node"));
else if (L("name") == "_noun")
  fixnouns(L("node"),L("node"));
else if (literal(L("node"))) # _xALPHA
  {
  # DO A REDUCE RIGHT HERE!
 	if (pnvar(L("node"),"adj"))
	  {
	  group(L("ord"),L("ord"),"_adj");
	  L("red") = pnup(L("node"));	# Sloppy get of new node.
	  pncopyvars(L("node"),L("red"));
	  fixadjs(L("red"),L("red"));
	  }
	else
	  {
	  group(L("ord"),L("ord"),"_noun");
	  L("red") = pnup(L("node"));	# Sloppy get of new node.
	  pncopyvars(L("node"),L("red"));
	  fixnounnonhead(L("red"));
	  }
  }
}

########
# FUNC:	FIXNPHEAD
# SUBJ:	Fix head noun of np. (alphatonoun)
# NOTE:	For pos scoring, etc.
# EFFECT: Assumes we're in rule match.
########

fixnphead(L("ord"))
{
L("n") = eltnode(L("ord"));
if (!L("n") || !L("ord"))
  return 0;

group(L("ord"),L("ord"),"_noun");
L("red") = pnup(L("n"));	# Sloppy get of new node.
pncopyvars(L("n"),L("red"));

fixnounhead(L("red"));

return L("red");
}

########
# FUNC:	SETPOS
# SUBJ:	Set POS if possible.
########

setpos(L("node"),L("pos"))
{
if (!L("node"))
  return;
if (pnvar(L("node"),"posarr len"))
  {
  chpos(L("node"),L("pos"));
  return;
  }
L("node") = pndown(L("node"));
while (L("node"))
  {
  if (pnnext(L("node")))
    return;
  if (pnvar(L("node"),"posarr len"))
    {
    chpos(L("node"),L("pos"));
    return;
    }
  L("node") = pndown(L("node"));
  }
}

########
# FUNC:	SETPOSRANGE
# SUBJ:	Assign POS to a list of nodes.
# NOTE: Weaning away from dependence on the pre-tagged
#	stuff, like N("posarr len").
#	This doesn't wipe out existing tags and can't serve
#	to zero out tags.
# TODO:	A function like this to handle capitalized phrases more
#	generally.
########

setposrange(
	L("first"),
	L("last"),
	L("pos")
	)
{
if (!L("first") || !L("pos"))
  return;

if (L("last"))
  L("last") = pnnext(L("last"));	# Good end boundary.
L("node") = L("first");
while (L("node") != L("last"))
  {
  if (!L("node"))
    return;
  if (!pnvar(L("node"),"mypos"))
	{
	L("p") = pnvar(L("node"),"pos_np");	# 04/19/07 AM.
	if (L("p"))
	  chpos(L("node"),L("p"));	# 04/19/07 AM.
	else
      chpos(L("node"),L("pos"));
	}
  L("node") = pnnext(L("node"));
  }
}

########
# FUNC:	SETPOSCAPS
# SUBJ:	Assign POS to a list of caps.
########

setposcaps(
	L("first"),
	L("last")
	)
{
if (!L("first"))
  return;

if (L("last"))
  L("last") = pnnext(L("last"));	# Good end boundary.
L("node") = L("first");
while (L("node") != L("last"))
  {
  if (!L("node"))
    return;
  if (pnvar(L("node"),"hyphenated"))	# 04/22/07 AM.
	;	# Leave it alone.
  else if (pnvar(L("node"),"unknown"))
	chpos(L("node"),"NP");
  else if (plural(L("node")))
	chpos(L("node"),"NPS");
  else
    chpos(L("node"),"NP");

  L("node") = pnnext(L("node"));
  }
}


########
# FUNC:	FORCEPOSRANGE
# SUBJ:	Assign POS to a list of nodes.
# NOTE: Weaning away from dependence on the pre-tagged
#	stuff, like N("posarr len").
#	This doesn't wipe out existing tags and can't serve
#	to zero out tags.
# TODO:	A function like this to handle capitalized phrases more
#	generally.
########

forceposrange(
	L("first"),
	L("last"),
	L("pos")
	)
{
if (!L("first") || !L("pos"))
  return;

if (L("last"))
  L("last") = pnnext(L("last"));	# Good end boundary.
L("node") = L("first");
while (L("node") != L("last"))
  {
  if (!L("node"))
    return;
#
  chpos(L("node"),L("pos"));
#
  L("node") = pnnext(L("node"));
  }
}

########
# FUNC:	GROUPONE
# SUBJ:	Smart group of one node.
# NOTE: Do the right thing with tagging info.
########

groupone(
	L("n"),		# Node to be grouped.
	L("o"),		# Order in rule phrase.
	L("name"),	# Name to reduce node to.
	L("copy")	# 1 if copying variables up.
	)
{
if (!L("n") || !L("o") || !L("name"))
  return;

# Do the reduce here.
group(L("o"),L("o"),L("name"));
L("red") = pnup(L("n"));	# Sloppy get of new node.

if (L("copy"))
  {
  pncopyvars(L("n"),L("red"));
  if (pnvar(L("n"),"mypos"))
    clearpos(L("red"),1,0);
  }
}

########
# FUNC:	GROUPNP
# SUBJ:	Smart group of nodes to a noun phrase.
# NOTE: Do the right thing with tagging info.
# ASS:	Assumes dqaninfo and rule have set up S variables.
########

groupnp()
{
if (!S("ofirst") || !S("olast") || !S("first") || !S("last"))
  {
  "err.txt" << "groupnp fail" << "\n";
  return;
  }
L("name") = "_np";

# Look for possessive pronoun.
if (pnvar(S("first"),"proposs")
 && !pnvar(S("first"),"mypos"))
   chpos(S("first"),"PP$");

if (S("firstj"))
  fixadjs(S("firstj"),S("lastj"));

fixnouns(S("firstn"),S("lastn"));

L("ne arr") = nenoderange(S("firstan"),S("lastan"));

# Look for negation.
L("neg") = attrinrange("neg",S("first"),S("last"));

# Do the reduce here.
group(S("ofirst"),S("olast"),L("name"));
L("red") = pnup(S("first"));	# Sloppy get of new node.

#if (L("neg"))
#  "sem.txt" << "neg np=" << pnvar(L("red"),"$text") << "\n";

pncopyvars(S("last"),L("red"));
clearpos(L("red"),1,1);

if (S("firstn") && S("firstn") != S("last"))
  semrange(L("red"),S("firstn"),S("last"));

#if (S("firstan") && S("firstan") != S("last"))
#  semnpnouns(L("red"),S("firstan"),S("last"),L("neg"));

# Try to compose with context.
#nodevsarray(L("red"),G("context entities"));

# Try to get an np semantics.
L("sem") = pnvar(S("last"),"sem");
L("ne") = pnvar(S("last"),"ne");
L("typ") = pnvar(S("last"),"ne type");
if (!L("typ") && L("ne"))
  L("typ") = "name";
if (L("typ"))
  pnreplaceval(L("red"),"sem",L("typ"));
else if (L("sem"))
  pnreplaceval(L("red"),"sem",L("sem"));
else if (pnvar(L("red"),"sem"))
  pnreplaceval(L("red"),"sem",0);

if (pnvar(L("red"),"ne"))
  pnreplaceval(L("red"),"ne",0);
if (pnvar(L("red"),"ne text"))
  pnreplaceval(L("red"),"ne text",0);
if (pnvar(L("red"),"ne type"))
  pnreplaceval(L("red"),"ne type",0);
if (L("ne arr"))
  pnreplaceval(L("red"),"ne arr",L("ne arr"));
if (L("neg"))
  pnreplaceval(L("red"),"neg",L("neg"));
}

########
# FUNC:	ALPHATOVERB
# SUBJ:	Smart group of alpha to verb.
# NOTE: Do the right thing with tagging info.
########


alphatoverb(
	L("ord"),	# order of alpha elt in phrase.
	L("voice"),	# Verb voice.
	L("pos")	# Verb pos.
	)
{
L("n") = eltnode(L("ord"));
if (!L("n") || !L("ord"))
  return;

# Do the reduce here.
group(L("ord"),L("ord"),"_verb");
L("v") = pnup(L("n"));	# Sloppy get of verb node.

pncopyvars(L("n"),L("v"));
if (L("voice") || L("pos"))
  fixverb(L("v"),L("voice"),L("pos"));
return L("v");
}

########
# FUNC:	ALPHATOVG
# SUBJ:	Smart group of alpha to verb to vg.
# NOTE: Do the right thing with tagging info.
########


alphatovg(
	L("ord"),	# order of alpha elt in phrase.
	L("voice"),	# Verb voice.
	L("pos")	# Verb pos.
	)
{
L("n") = eltnode(L("ord"));
if (!L("n") || !L("ord"))
  return;

# Do the reduce here.
group(L("ord"),L("ord"),"_verb");
L("v") = pnup(L("n"));	# Sloppy get of verb node.

pncopyvars(L("n"),L("v"));
if (L("voice") || L("pos"))
  fixverb(L("v"),L("voice"),L("pos"));

# Do the reduce here.
group(L("ord"),L("ord"),"_vg");
L("vg") = pnup(L("v"));	# Sloppy get of vg node.

mhbv(L("vg"),0,0,0,0,0,L("v"));
pncopyvars(L("v"),L("vg"));
pnreplaceval(L("vg"),"voice",L("voice"));
pnreplaceval(L("vg"),"verb node",L("v"));
pnreplaceval(L("vg"),"first verb",L("v"));
clearpos(L("vg"),1,0);	# Zero out token info.
return L("vg");
}

########
# FUNC:	ALPHATONOUN
# SUBJ:	Smart group of alpha to noun.
# NOTE: Don't know if np head or not.
########

alphatonoun(L("ord"))
{
L("n") = eltnode(L("ord"));
if (!L("n") || !L("ord"))
  return 0;

# Do the reduce here.
group(L("ord"),L("ord"),"_noun");
L("noun") = pnup(L("n"));	# Sloppy get of node.

pncopyvars(L("n"),L("noun"));
return L("noun");
}

########
# FUNC:	ALPHATOADJ
# SUBJ:	Smart group of alpha to adj.
# NOTE: Don't know if in np or not.
########

alphatoadj(L("ord"))
{
L("n") = eltnode(L("ord"));
if (!L("n") || !L("ord"))
  return 0;

# Do the reduce here.
group(L("ord"),L("ord"),"_adj");
L("adj") = pnup(L("n"));	# Sloppy get of node.

pncopyvars(L("n"),L("adj"));
fixadj(L("adj"));
return L("adj");
}
########
# FUNC:	ALPHATOADV
# SUBJ:	Smart group of alpha to adv.
########

alphatoadv(L("ord"))
{
L("n") = eltnode(L("ord"));
if (!L("n") || !L("ord"))
  return 0;

# Do the reduce here.
group(L("ord"),L("ord"),"_adv");
L("adv") = pnup(L("n"));	# Sloppy get of node.

pncopyvars(L("n"),L("adv"));
fixadv(L("adv"));
return L("adv");
}

########
# FUNC:	DQANINFO
# SUBJ:	Set up some variables for dqan.
# ASS: Assume in dqan rule context.
########

dqaninfo(
	L("ndet"),
	L("nquan"),
	L("nadj"),
	L("nnoun")
	)
{
if (L("ndet"))
  {
  S("first") = S("firstd") = S("det") = eltnode(L("ndet"));
  S("last") = S("lastd") = lasteltnode(L("ndet"));
  S("ofirst") = S("olast") = L("ndet");
  }

if (L("nquan"))
  {
  S("firstq") = eltnode(L("nquan"));
  S("lastq") = lasteltnode(L("nquan"));
  if (!S("first"))
    S("first") = S("firstq");
  if (S("lastq"))
    {
	S("last") = S("lastq");
    S("olast") = L("nquan");
	}
  if (!S("ofirst"))
    S("ofirst") = L("nquan");
  }


if (L("nadj"))
  {
  S("firstj") = eltnode(L("nadj"));
  S("lastj") = lasteltnode(L("nadj"));
  if (!S("first"))
    S("first") = S("firstj");
  S("firstan") = S("firstj");
  if (S("lastj"))
    {
	S("last") = S("lastan") = S("lastj");
    S("olast") = L("nadj");
	}
  if (!S("ofirst"))
    S("ofirst") = L("nadj");
  }

if (L("nnoun"))
  {
  S("firstn") = eltnode(L("nnoun"));
  S("lastn") = lasteltnode(L("nnoun"));
  if (!S("first"))
    S("first") = S("firstn");
  if (!S("firstan"))
    S("firstan") = S("firstn");
  if (S("lastn"))
    {
	S("last") = S("lastan") = S("lastn");
    S("olast") = L("nnoun");
	}
  if (!S("ofirst"))
    S("ofirst") = L("nnoun");
  }

if (!S("ofirst") || !S("olast")
	|| !S("first") || !S("last"))
	"err.txt" << "dqaninfo fail." << "\n";
}

########
# FUNC:	COPULAQ
# SUBJ:	See if verbal noun is a copula.
########

copulaq(L("n"))
{
if (!L("n"))
  return 0;
if (pnname(L("n")) == "_vg")
  L("n") = pnvar(L("n"),"verb node");
if (!L("n"))
  return 0;

L("txt") = nodestem(L("n"));
L("sem") = nodesem(L("n"));
if (L("sem") == "be" || L("sem") == "being"
   || finddictattr(L("txt"),"copula"))
  return 1;
return 0;
}

@CODE
G("hello") = 0;
@@CODE
