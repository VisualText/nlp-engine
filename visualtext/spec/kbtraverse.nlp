###############################################
# FILE: kbtraverse.pat
# SUBJ: Iterative traversal of knowledge base.
# AUTH: AM
# CREATED: 24/Dec/03 15:54:47
# NOTE: Postorder actions print each node, as an example.
# WARN:	Recursive functions in NLP++ have a nesting limit,
#		especially in the interpreted code.
###############################################

@DECL

#################
## FN: POSTORDER
## SUBJ: Postorder action for kb traversal.
#################
postorder(L("con"))
{
if (!L("con"))
  return;	# Error.
"postorder.txt" << "postorder: " << conceptname(L("con")) << "\n";
}

#################
## FN: PREORDER
## SUBJ: Preorder action for kb traversal.
#################
preorder(L("con"),L("depth"))
{
if (!L("con"))
  return;	# Error.
indent(L("depth"),"preorder.txt");
"preorder.txt" << conceptname(L("con")) << "\n";
}

#################
## FN: INDENT
## SUBJ: Output tabs.
#################
indent(L("num"),L("out"))
{
while (L("num") > 0)
  {
  L("out") << "\t";
  --L("num");
  }
}

#################
## FN: KBTRAVERSE
## SUBJ: Traverse a subhierarchy in knowledge base.
#################
kbtraverse(L("root"))
{
L("con") = L("root");
L("depth")     = 0;
L("path")      = 0;                     # Store the current path in the parse tree.

while (L("con"))                       # Traverse the parse tree.
  {
  preorder(L("con"),L("depth"));
  L("path")[L("depth")] = L("con");	# Update path in the parse tree.
  L("childs") = down(L("con"));
  if (L("con") != L("root"))	# FIX.	# 10/03/13 AM.
    L("next") = next(L("con"));
  else	# FIX.	# 10/03/13 AM.
    L("next") = 0;	# FIX.	// 10/03/13 AM.

  if (L("childs"))                     # If node has children, traverse them.
     {
	 L("con") = L("childs");
	 ++L("depth");
	 }
  else if (L("next"))                  # If node has right sibling, get it.
     {
	 postorder(L("con"));
	 L("con") = L("next");
	 }
  else                                 # Try continuing from a parent.
     {
	 postorder(L("con"));
	 L("done") = 0;
	 L("con") = 0;
	 while (!L("done"))                # Find a continuation or quit.
	    {
		if (--L("depth") > 0)
		   {
		   L("con") = L("path")[L("depth")];	# Move up to parent.
		   if (L("con"))
			 postorder(L("con"));
		   if (L("con") = next(L("con")))	# Try to move right.
		     L("done") = 1;
		   }
		else
		   L("done") = 1;
		}
	 }
  }

# Traverse the given root
postorder(L("root"));
}

@CODE
L("hello") = 1;
#kbtraverse(findroot());	# Traverse the kb.
@@CODE
