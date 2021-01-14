###############################################
# FILE: study100.pat
# SUBJ: Study and rule generation.
# AUTH: AM
# CREATED: 30/Jun/06 08:59:35
# MODIFIED:
###############################################

@CODE
L("hello") = 0;

if (!G("verbose"))
  exitpass();

L("fname") = G("$apppath") + "\\data\\study100.txt";
G("studyout") = openfile(L("fname"),"app");
@@CODE

@NODES _sent

# Find every alpha cluster.
@POST
  if (N(2))
    L("lst") = lasteltnode(2);
  else
    L("lst") = N(1);
  # Count the alphas.
  L("count") = nodecount(N(1),L("lst"));
  G("studyout") << L("count") << " ";
  L("pre") = pnprev(N(1));
  L("post") = pnnext(L("lst"));
  if (L("pre"))
    L("first") = L("pre");
  else
    L("first") = N(1);

  if (!L("pre"))
    G("studyout") << " START";
  L("flag") = printruleelts(L("first"),L("post"),
									L("flag"),G("studyout"));
    if (!L("post"))
    G("studyout") << " END";

  # Now, text comment.
  G("studyout") << "\t#";
  nodestextout(L("first"),L("post"),G("studyout"));

  G("studyout") << "\n";
  noop();
@RULES
_xNIL <-
	_xALPHA
	_xWILD [star match=(_xALPHA _xPUNCT)]
	@@
