###############################################
# FILE: pos_out.pat
# SUBJ: Output POS tags.
# AUTH: AM
# CREATED: 05/Aug/04 17:40:39
# NOTE:	This pass can also SCORE.
###############################################

@CODE
# If not outputting tags, exit.
if (!G("verbose"))  # Todo: another flag here.
  exitpass();

if (!G("pretagged"))
  exitpass();

# Initialize traversal data.
G("parseRoot") = pnroot();
G("node")      = G("parseRoot");
G("depth")     = 0;
G("path")      = 0;                     # Store the current path in the parse tree.

while (G("node"))                       # Traverse the parse tree.
  {
  G("path")[G("depth")] = G("node");	# Update path in the parse tree.
  G("childs") = pndown(G("node"));
  G("next") = pnnext(G("node"));

  # Stop at a node with a part-of-speech array.
  L("posarr len") = pnvar(G("node"),"posarr len");
  L("nopos") = pnvar(G("node"),"nopos");
  L("nm") = pnname(G("node"));
  L("ln") = strlength(L("nm")) - 1;
  if (strpiece(L("nm"),0,0) != "_")
    {
	if (strisdigit(L("nm")))
	  pnreplaceval(G("node"),"mypos","CD");
    L("nm") = "NIL";
	}
  else
    {
	if (L("nm") == "_sent")
	  "tags.txt" << "\n";
	if (L("ln") >= 1)
    	L("nm") = strpiece(L("nm"),1,L("ln"));
	}
  if (L("posarr len") > 0)
  	{
	G("gochilds") = 0; # Don't traverse children.
	L("npos") = nodepos(G("node"),L("nm"));
	L("txt") = prosify(G("node"));
	 "tags.txt" << L("txt")
	 	<< " / ("
		<< pnvar(G("node"),"posarr")
		<< ") "
		<< L("npos");
		;
	if (!scorenodepos(G("node"),L("npos"))) # mismatch
	  {
	  pnrename(G("node"),"_poserr");
	  "tags.txt" << "   *********************"; # 01/08/05 AM.

	  if (L("npos"))	# 06/14/06 AM.
	    {
	    G("mismatch out") << L("txt")	# 06/14/06 AM.
	 	  << " / ("
		  << pnvar(G("node"),"posarr")
		  << ") "
		  << L("npos")
		  ;
		if (G("mismatch verbose"))
		  {
		  G("mismatch out") << " (" << G("$inputhead") << ")";
		  }
		G("mismatch out") << "\n";
	    }
	  else
	    {
	    G("zero out") << L("txt")
	 	  << " / ("
		  << pnvar(G("node"),"posarr")
		  << ") "
		  << L("npos")
		  ;
		if (G("mismatch verbose"))
		  {
		  G("zero out") << " (" << G("$inputhead") << ")";
		  }
		G("zero out") << "\n";
	    }

	  }
	"tags.txt" << "\n";
	}
  else if (L("nopos"))	# Things like punctuation...
  	{
	G("gochilds") = 0; # Don't traverse children.
	 L("txt") = pnvar(G("node"),"$text");
	 "tags.txt" << L("txt")
	 	<< " / ()"
		<< "\n";
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
	 G("node") = G("next");
  else                                 # Try continuing from a parent.
     {
	 G("done") = 0;
	 G("node") = 0;
	 while (!G("done"))                # Find a continuation or quit.
	    {
		if (--G("depth") > 0)
		   {
		   G("node") = G("path")[G("depth")];	# Move up to parent.
		   if (G("node") = pnnext(G("node")))	# Try to move right.
		     G("done") = 1;
		   }
		else
		   G("done") = 1;
		}
	 }
  }

@@CODE

