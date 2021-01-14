###############################################
# FILE: pos_out_noscore.pat
# SUBJ: Output POS tags without scoring.
# AUTH: AM
# CREATED: 16/Mar/05 23:35:02
# MODIFIED:
###############################################

@DECL
printblurb(L("out"))
{
L("out") << "*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*\n";
L("out") << "*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*\n";
L("out") << "*x*                                                                     *x*\n";
L("out") << "*x*            Copyright (C) 1990 University of Pennsylvania            *x*\n";
L("out") << "*x*                                                                     *x*\n";
L("out") << "*x*    The data in this file are part of a preliminary version of the   *x*\n";
L("out") << "*x*    Penn Treebank Corpus.  Any research using this corpus or based   *x*\n";
L("out") << "*x*    on it should acknowledge that fact, as well as the preliminary   *x*\n";
L("out") << "*x*    nature of the corpus.                                            *x*\n";
L("out") << "*x*                                                                     *x*\n";
L("out") << "*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*\n";
L("out") << "*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*\n";
L("out") << "*x*            Copyright (C) 2005 Text Analysis International, Inc.     *x*\n";
L("out") << "*x*            All rights reserved.                                     *x*\n";
L("out") << "*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*\n";
}

############ POSTOKB
## NOTE:	Send pos to kb.
######################
postokb(L("nm"),L("txt"))
{
if (!L("nm") || !L("txt"))
  return;

if (strisdigit(L("txt")))
  return;

# CONFIGURE: TO DISTINGUISH LETTER CASE, COMMENT THIS:  # 09/24/13 AM.
L("txt") = strtolower(L("txt"));	# 09/24/13 AM.

L("con") = getconcept(G("kbpos"),L("nm"));
L("wcon") = getconcept(L("con"),L("txt"));
}

############ POSTOFILE
## NOTE:	Send pos to file.
######################
postofile()
{
if (!G("kbpos")) return;

sorthier(G("kbpos"));
L("o") = "pos.txt";
L("c") = down(G("kbpos"));
while (L("c"))
  {
  L("o") << "\n" << strtoupper(conceptname(L("c"))) << "\n";
  # Words for this pos.
  L("w") = down(L("c"));
  while (L("w"))
    {
	L("o") << conceptname(L("w")) << "\n";
	L("w") = next(L("w"));
	}
  L("c") = next(L("c"));
  }
}

###################################################################

@CODE
# If not outputting tags, exit.
if (!G("verbose"))  # Todo: another flag here.
  exitpass();

if (G("pretagged"))
  exitpass();

if (L("con") = findconcept(findroot(),"pos"))	# 09/19/13 AM.
  rmconcept(L("con"));	# CLEAR LIST FOR CURRENT DOC.	# 09/19/13 AM.

G("kbpos") = getconcept(findroot(),"pos");	# 09/16/13 AM.

  
G("para") = "======================================";

if (G("treebank"))
  printblurb("tags.txt");

# Initialize traversal data.
G("parseRoot") = pnroot();
G("node")      = G("parseRoot");
G("depth")     = 0;
G("path")      = 0;                     # Store the current path in the parse tree.
L("newline") = 1;		# If on a new empty line.
L("newpar") = 0;		# If at new paragraph.

while (G("node"))                       # Traverse the parse tree.
  {
  G("path")[G("depth")] = G("node");	# Update path in the parse tree.
  G("childs") = pndown(G("node"));
  G("next") = pnnext(G("node"));
  
  # If there are prior deleted tokens, print those.
  
  # If bracketing this node, do the start.
  if (G("bracket") && pnvar(G("node"),"bracket"))
    {
	if (!L("newline") && !L("newpar"))
	  "tags.txt" << "\n";
    "tags.txt" << "[ ";
	L("newline") = 0;
	L("newpar") = 0;
	}

  # Stop at a node with a mypos variable.
  # Need some other way for default pos tags...
  L("nm") = pnname(G("node"));
  L("ln") = strlength(L("nm")) - 1;
  L("ch") = strpiece(L("nm"),0,0);
  if (L("ch") != "_") # Literal.
    {
	if (strisdigit(L("nm")))
	  pnreplaceval(G("node"),"mypos","CD");
	else if (L("ch") == " " || L("ch") == "\t"
	  || L("ch") == "\n" || L("ch") == "\r")
	   ;  # Ignore whitespace.
	else if (strisalpha(L("ch")))
	  {
	  if (!pnvar(G("node"),"mypos"))
	    pnreplaceval(G("node"),"mypos","UNKNOWN");
	  }
	else # Punctuation.
	  pnreplaceval(G("node"),"nopos",1);
    L("nm") = "NIL";
	}
  else
    {
	if (L("nm") == "_sent")
	  {
	  if (!G("bracket"))
	    "tags.txt" << "\n";
	  else
	    {
		if (!L("newline") && !L("newpar"))
		  "tags.txt" << "\n";
		if (!L("newpar"))
	      "tags.txt" << "\n";
		}
	  L("newline") = 1;
	  }
	else if (L("nm") == "_TEXTZONE")
	  {
	  if (!L("newline") && !L("newpar"))
	    "tags.txt" << "\n";
	  if (!L("newpar"))
	    "tags.txt" << "\n";
	  "tags.txt" << G("para") << "\n\n";
	  L("newline") = 1;
	  L("newpar") = 1;
	  }
	if (L("ln") >= 1)
    	L("nm") = strpiece(L("nm"),1,L("ln"));
	}

  L("npos") = nodepos(G("node"),L("nm"));
  L("ignorepos") = pnvar(G("node"),"ignorepos");
  L("nopos") = pnvar(G("node"),"nopos");

  if (L("npos") && !L("ignorepos"))
  	{
	L("newpar") = 0;
	G("gochilds") = 0; # Don't traverse children.
	L("txt") = prosify(G("node"),"text");	# 12/15/20 AM.
	 "tags.txt" << L("txt")
	 	<< "/"
		<< L("npos")
		;
# CONFIGURE: EACH POS TO ITS OWN FILE.			# 09/15/13 AM.
#	L("out") = L("nm") + ".txt";	# 09/16/13 AM.
#	L("out") << L("txt") << "\n";	# 09/15/13 AM.
# OR THIS, ALL IN ONE FILE:			# 09/24/13 AM.
	postokb(L("nm"),L("txt"));		# 09/16/13 AM.
	
	if (!G("bracket"))
	  {
	  "tags.txt" << "\n";
	  L("newline") = 1;
	  }
	else
	  {
	  "tags.txt" << " ";
	  L("newline") = 0;
	  }
	if (G("bracket") && pnvar(G("node"),"bracket"))
	  {
	  "tags.txt" << "]\n";	# End bracket.
	  L("newline") = 1;
	  }
	}
  else if (L("nopos"))	# Things like punctuation...
  	{
	L("newpar") = 0;
	G("gochilds") = 0; # Don't traverse children.
	L("txt") = pnvar(G("node"),"$text");
	"tags.txt" << L("txt")
	 	<< "/"
		<< L("txt");  # Just echo it.
	if (!G("bracket"))
	  {
	  "tags.txt" << "\n";
	  L("newline") = 1;
	  }
	else
	  {
	  "tags.txt" << " ";
	  L("newline") = 0;
	  }
	if (G("bracket") && pnvar(G("node"),"bracket"))
	  {
	  "tags.txt" << "]\n";	# End bracket.
	  L("newline") = 1;
	  }
	}
  else
	G("gochilds") = 1; # Traverse children.


  if (G("childs")                    # If node has children, traverse them.
   && G("gochilds"))
     {
	 G("node") = G("childs");
	 ++G("depth");
	 }
  else if (G("next"))                  # If node has right sibling, get it.
     {
	 G("node") = G("next");
	 }
  else                                 # Try continuing from a parent.
     {
	 G("done") = 0;
	 G("node") = 0;
	 while (!G("done"))                # Find a continuation or quit.
	    {
		if (--G("depth") > 0)
		   {
		   G("node") = G("path")[G("depth")];	# Move up to parent.
		   if (G("bracket") && pnvar(G("node"),"bracket"))
		     {
			 "tags.txt" << "]\n";	# End bracket.
			 L("newline") = 1;
			 }
		   if (G("node") = pnnext(G("node")))	# Try to move right.
		     G("done") = 1;
		   }
		else
		   G("done") = 1;
		}
	 }
  }

postofile();	# 09/16/13 AM.
@@CODE

