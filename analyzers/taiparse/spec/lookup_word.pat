###############################################
# FILE: lookup_word.pat
# SUBJ: Lookup words in kb dictionary.
# AUTH: AM
# CREATED: 06/Jul/04 01:40:03
# MODIFIED:
###############################################

@DECL
##############
# LOOKUPWORD
# SUBJ: Lookup and record word information.
##############
lookupword(L("n"))
{
if (!L("n"))
  return;
L("txt") = pnvar(L("n"),"$treetext");
L("lctxt") = strtolower(L("txt"));

if (strisupper(strpiece(L("txt"),0,0)))
  pnreplaceval(L("n"),"cap",1);	# 06/26/06 AM.

L("wcon") = dictfindword(L("lctxt"));
if (!L("wcon"))
  {
  pnreplaceval(L("n"),"unknown",1);
  if (G("verbose"))
  "unknown.txt" << L("lctxt") << "\n";
  if (L("txt") == L("lctxt") && G("verbose"))
    "lcunknown.txt" << L("lctxt") << "\n";
  return;
  }

lookupalpha(L("lctxt"),L("n"));
if (!pnvar(L("n"),"pos num"))	# Not in dictionary.
  	{
    if (G("verbose"))
	"unknown.txt" << L("lctext") << "\n";
    if (L("txt") == L("lctxt") && G("verbose"))
      "lcunknown.txt" << L("lctxt") << "\n";
	pnreplaceval(L("n"),"unknown",1);
	pnreplaceval(L("n"),"stem",L("lctxt"));
#	unknownword(N("lctext"),N(1));
	}

}

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@CHECK
  if (N("pos num"))
    fail();	# Fixed dict earlier.
@POST
  lookupword(N(1));
  if (N("prep"))
    {
	L("tmp") = N(1);
    group(1,1,"_prep");
	pncopyvars(L("tmp"),N(1));
	}
  else if (N("det"))
    {
	L("tmp") = N(1);
    group(1,1,"_det");
	pncopyvars(L("tmp"),N(1));
	}
  else if (N("conj"))
    {
	L("tmp") = N(1);
    group(1,1,"_conj");
	pncopyvars(L("tmp"),N(1));
	}

@RULES
_xNIL <-
	_xALPHA
	@@
