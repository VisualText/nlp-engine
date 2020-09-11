###############################################
# FILE: xmlrecursive.pat
# SUBJ: Recursive traversal to output embedded tags.
# AUTH: AM
# CREATED: 16/Dec/02 20:30:09
# MODIFIED:
###############################################

@DECL
#############
# FN:   XMLRECURSION
# SUBJ: Traverse parse tree to output embedded tags.
# CR:   12/16/02 AM.
# NOTE: To minimize recursion, each first child will manage
#		its list of siblings.
#		Using the global G("oxml") stream for printout.
#############

xmlrecursion(
	L("n"),	# Current node.
	L("root"),	# 1 if a "root", else 0.
	L("inside") # 1 if "inside" an NP, for example.
	)
{
# Consider current node.
if (!L("n"))
  return;

L("childs") = pndown(L("n"));

# Check if printable.
L("name") = pnname(L("n"));
L("sem") = pnvar(L("n"),"sem");
if (pnvar(L("n"),"ne"))
  L("ne") = 1;
L("tag") = 0;
#if (G("NP"))	# Analyzer caller says to print out NP nodes.
#  {
# && (L("name") == "_nps" || L("name") == "_np") )
#  L("tag") = "np";
#  if (L("name") == "_phone")
#    L("tag") = "phone";
#  else if (L("name") == "_url")
#    L("tag") = "url";
#  }
#else if (G("NOUN")
# && (L("name") == "_noun") )

if (L("name") == "_np" && !pnvar(L("n"),"compound-np"))
  {
  if (!L("inside"))
    {
	if (L("ne"))
	  {
	  L("tag") = "NE";
	  L("childs") = 0;
	  }
	else
      L("tag") = "NP";
	L("nowinside") = 1;  # For traversing inside it.
	}
  else if (L("ne"))
    {
    L("tag") = "NE";
	L("childs") = 0;
	}
  }
else if (L("name") == "_vg")
  {
  if (pnvar(L("n"),"list"))
    L("tag") = "VGLIST";
  else
    {
    L("tag") = "VG";
	L("childs") = 0;
	}
  }
else if (L("name") == "_verb")
  L("tag") = "VG";
else if (L("name") == "_sent")
  L("tag") = "SENT";
else if (L("name") == "_clause")
  L("tag") = "CLAUSE";
else if (L("name") == "_prep")
  {
  if (!L("inside"))
    L("tag") = "PREP";
  }
else if (L("name") == "_advl") # Look for location case...
  {
  if (!L("inside"))
    L("tag") = "ADVL";
  }
else if (L("ne"))
  {
  L("tag") = "NE";
  L("childs") = 0;
  }

if (L("inside"))
  L("nowinside") = 1;

if (L("tag"))	# If printable.
  {
  if (!G("xml tags only"))
    printfiller(L("n"),G("oend"),G("oxml"));
  printtag(L("n"),L("tag"),G("oxml"));
#  G("oxml") << "<" << L("tag") << ">";
  if (L("tag") == "NE")
     {
     if (G("verbose"))
	 ljust(pnvar(L("n"),"$text"),30,"ne.txt");
	  if (G("verbose"))
	 "ne.txt"
		<< "\t" << pnvar(L("n"),"ne type")
		<< "\n";
	}
  
  # Update right edge so filler won't be printed again.
  G("oend") = pnvar(L("n"),"$ostart");
  }

# If a leaf, print it.
if (!L("childs"))
  {
  if (!G("xml tags only"))
    {
    if (!L("tag"))
      printfiller(L("n"),G("oend"),G("oxml"));
    G("oxml") << xmlstr(pnvar(L("n"),"$text"));
    }
  G("oend") = pnvar(L("n"),"$oend");
  }
else	# Else, traverse its subtree.
  xmlrecursion(L("childs"),0, L("nowinside"));

# If printable, print its end tag.
if (L("tag"))
  G("oxml") << "</" << L("tag") << ">";

# If first child and not a root, traverse siblings.
if (!pnprev(L("n")) && !L("root"))
  {
  while (L("n") = pnnext(L("n")) )
    xmlrecursion(L("n"),1,L("inside"));
  }
}

#############
## FN:   PRINTFILLER
## SUBJ: Print filler between prior and current node.
## CR:   12/15/02 AM.
#############
printfiller(
	L("node"),	# Current node to print.
	L("oend"),	# Right edge offset of last printed text.
	L("out")	# Output stream.
	)
{
	L("ostart") = pnvar(L("node"),"$ostart");
	if (L("oend") == L("ostart"))
	  return;
    if (L("oend")
	 && (L("ostart") - L("oend") - 1) )
	  {
	  # Get snippet of input text between last and current output node.
	  if (!(L("snip") = inputrange(L("oend")+1,L("ostart")-1) ))
	    {
		if (G("error"))
	    "err.txt" << "snip: " << L("oend") << "," << L("ostart") << "\n";
		}
	  if (L("n") = strchrcount(L("snip"),"\n"))	# If snippet has a newline.
	    {
		while (L("n")-- > 0)
	      L("out") << "\n";			# Preserve newline.
		}
	  else
	    # Note: Even if there wasn't a space in the snippet, we don't know what got
		# excised in there, so printing space for safety.  For example, if the parse excised
		# a paren, then no space means two alphabetics may be inappropriately glommed together.
		# (When excising, could flag if there is a need to separate what's left with a space...)
	    L("out") << " ";			# Preserve space.
	  }
}

#############
## FN:   PRINTTAG
## SUBJ: Print an XML tag with attribute data.
## CR:   05/18/05 AM.
#############
printtag(
	L("node"),	# Current node to print.
	L("name"),	# Tag name.
	L("out")	# Output stream.
	)
{
if (!L("node") || !L("name") || !L("out"))
  return; # Error.

L("out") << "<" << L("name");

# Print attributes here.
L("voice") = pnvar(L("node"),"voice");
if (L("voice"))
  L("out") << " voice=\"" << L("voice") << "\"";

L("sem") = pnvar(L("node"),"sem");
if (L("sem"))
  L("out") << " sem=\"" << L("sem") << "\"";
  
L("stem") = pnvar(L("node"),"stem");
if (L("stem"))
  L("out") << " stem=\"" << L("stem") << "\"";

L("tense") = pnvar(L("node"),"tense");
if (L("tense"))
  L("out") << " tense=\"" << L("tense") << "\"";

L("aspect") = pnvar(L("node"),"aspect");
if (L("aspect"))
  L("out") << " aspect=\"" << L("aspect") << "\"";

# Default to node's NE type.
L("type") = pnvar(L("node"),"ne type");

# But if we resolve to a concept, use that NE type.
L("kb ent") = pnvar(L("node"), "kb ent");

if (L("kb ent")) {
	L("con entity") = conval(L("kb ent"), "resolver");
	if (L("con entity")) {
	   L("type") = strval(L("con entity"), "ne type");
	}
}

if (L("type"))
  L("out") << " type=\"" << L("type") << "\"";

L("source") = pnvar(L("node"),"source");
if (L("source"))
  L("out") << " source=\"" << L("source") << "\"";

L("date") = pnvar(L("node"),"timestamp");
if (L("date"))
  L("out") << " date=\"" << L("date") << "\"";

L("country") = pnvar(L("node"),"country");
if (L("country"))
  L("out") << " country=\"" << L("country") << "\"";

L("out") << ">";
}

@CODE
if (!G("xml recursive"))
  exitpass();

# Set up the recursion here.
G("oend") = 0;	# End offset of previously printed node.

L("dir") = G("$apppath") + "\\data";
#L("fname") = L("dir") + "\\embed.xml";
#G("oxml") = openfile(L("fname"),"app");
L("fname") = L("dir") + "\\" + G("$inputhead") + ".xml";
G("oxml") = openfile(L("fname"));

#L("dir") = G("$apppath") + "\\data\\xml_out";
#mkdir(L("dir"));
#L("fname") = L("dir") + "\\" + G("$inputhead") + ".xml";
#G("oxml") = openfile(L("fname"));

#G("oxml") = "embed.xml";

#xmlheader(G("oxml"));
xmlstart("doc",G("oxml"));
xmlshort("input",G("$inputname"),G("oxml"));
xmlshort("output",G("$inputhead")+".xml",G("oxml"));
xmlrecursion(pnroot(), 1, 0);
xmlend("doc",G("oxml"));

if (G("oxml"))
  closefile(G("oxml"));
@@CODE
