###############################################
# FILE: stuff.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 27/Oct/06 17:30:34
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
"output.txt" << "Printing from code region." << "\n";

addword("abc");
L("con") = dictgetword("abc");
"output.txt" << "Word con=" << conceptname(L("con")) << "\n";

# "output.txt" << "apppath=";
"output.txt" << G("$apppath") << "\n";
L("fn") = G("$apppath") + "/data/xxx.txt";
"output.txt" << L("fn") << "\n";
# L("oo") = openfile(L("fn"),"app");
L("oo") = openfile(L("fn"),0);
L("oo") << "XXX FILE" << "\n";

# Try relative unix path.
"../data/yyy.txt" << "hello" << "\n";
L("zz") = "../data/zzz.txt";
L("zz") << "zzz ok" << "\n";

# Try relative unix path with openfile.
L("qqf") = "../data/qqq.txt";
L("qqo") = openfile(L("qqf"),0);
L("qqo") << "QQQ openfile ok" << "\n";

@@CODE

@NODES _ROOT

@POST
  "output.txt" << "Found Linux system" << "\n";
  N("var",1) = "zzz";
  L("red") = group(1,3,"_RED");
  if (!L("red")) "output.txt" << "EMPTY RED" << "\n";
#  pncopyvars(N(1),N(3));
#  "output.txt" << "var3=" << N("var",3) << "\n";
#  single();
@RULES
_xNIL <-
	Linux
	_xWHITE [star]
	system
	@@
