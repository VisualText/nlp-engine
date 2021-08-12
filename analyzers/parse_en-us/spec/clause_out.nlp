###############################################
# FILE: clause_out.pat
# SUBJ: Task-specific output at clause level
# AUTH: AM
# CREATED: 09/May/07 20:58:32
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

@POST
  if (N("sem texts") && N("sem vals"))
	{
	"output.txt" << "TEXT= " << phrasetext() << "\n";
	L("ii") = 0;
	L("max") = arraylength(N("sem texts"));
	L("key") = N("sem texts");
	L("val") = N("sem vals");
	while (L("ii") < L("max"))
		{
 		"output.txt" << strtoupper(L("key")[L("ii")])
			<< "\t"
			<< L("val")[L("ii")]
			<< "\n"
			;
		++L("ii");
		}
	}
@RULES
_xNIL <-
	_clause
	@@
