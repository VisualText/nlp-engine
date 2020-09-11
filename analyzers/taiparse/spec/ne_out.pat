###############################################
# FILE: ne_out.pat
# SUBJ: comment
# AUTH: AM
# MODIFIED:
###############################################

@CODE
if (!G("verbose"))
  exitpass();

#L("dir") = G("$apppath") + "\\data\\ne";
#mkdir(L("dir"));
#L("fname") = L("dir") + "\\" + G("$inputname");
#G("kout") = openfile(L("fname"));
G("kout") = "clause_ne.txt";
G("kout") << "PART   SEM          STEM          NE_TEXT       ALL_TEXT" << "\n"
          << "========================================================" << "\n"
		  ;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

@POST
  if (!X("printed",3))
     {
    G("kout") << "\n" << "sentence: "
		<< X("$text",3)
		<< "\n";
	 X("printed",3) = 1;
	 }
  if (!X("printed",4))
     {
    G("kout") << "\n" << "clause: "
		<< X("$text",4)
		<< "\n";
	 if (X("voice",4))
	    G("kout") << "voice:\t" << X("voice",4) << "\n";
	 else
	    G("kout") << "voice:\tnil\n";
	 X("printed",4) = 1;
	 }

  if (!X("stuff-in-clause"))
    {
	L("nm") = pnname(N(1));
	L("len") = strlength(L("nm"));
	if (strpiece(L("nm"),0,0) == "_"
	 && L("len") > 1)
	  L("nm") = strpiece(L("nm"),1,L("len")-1);
	G("kout") << L("nm");
	indent(5-L("len"),G("kout"));

	L("sem") = N("sem");
	if (!L("sem"))
	  L("sem") = N("stem");
	if (!L("sem"))
	  L("sem") = "nil";
    G("kout") << "\t"
		<< L("sem")
		;
	L("len") = strlength(L("sem"));
	indent(8-L("len"),G("kout"));

	# NP head, etc.
	L("stem") = N("stem");
	if (!L("stem"))
	  L("stem") = "nil";
    G("kout") << "\t"
		<< L("stem")
		;
	L("len") = strlength(L("stem"));
	indent(8-L("len"),G("kout"));

	L("ne text") = N("ne text");
	if (!L("ne text"))
	  L("ne text") = "nil";
    G("kout") << "\t"
		<< L("ne text")
		;
	L("len") = strlength(L("ne text"));
	indent(12-L("len"),G("kout"));

    G("kout") << "\t" << N("$text") << "\n";
	}
@RULES
_xNIL <-
	_xANY
	@@
