###############################################
# FILE: traverse_parse_tree.pat
# SUBJ: A parse tree traversal pass.
# AUTH: AM
# CREATED: 12/3/01
# NOTE: An iterative traversal sample.
###############################################

@CODE
			        
# Initialize traversal data.
G("parseRoot") = pnroot();
G("node")      = G("parseRoot");
G("depth")     = 0;
G("path")      = 0;                     # Store the current path in the parse tree.
G("next")      = 0;                     # 09/24/13 AM.

while (G("node"))                       # Traverse the parse tree.
  {
  G("path")[G("depth")] = G("node");	# Update path in the parse tree.
  G("childs") = pndown(G("node"));
  if (G("depth") > 0) # FIX. # 09/24/13 AM.
    G("next") = pnnext(G("node"));
  
  # Printing out only leaf nodes, as an example.
  if (!G("childs"))
	 "tags.txt" << pnvar(G("node"),"$treeraw");

  if (G("childs"))                     # If node has children, traverse them.
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
