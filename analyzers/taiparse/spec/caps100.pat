###############################################
# FILE: caps100.pat
# SUBJ: Glom capitalized phrases.
# AUTH: AM
# CREATED: 09/Jul/04 21:02:43
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

@PRE
<1,1> lowercase();
@CHECK
  if (N("ne type",2))
    fail();
@POST
  L("tmp") = lasteltnode(3);
#  if (pnvar(L("tmp"),"apos-s"))
#    {
#    S("apos-s") = 1;
#	group(2,3,"_adj");
#	}
#  else
    group(2,3,"_noun");
  pncopyvars(L("tmp"),N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(_xALPHA _xPUNCT _xNUM)]
	_xWILD [one match=(_xCAP _caps)]
	_xWILD [s plus match=(_xCAP _caps \- )]
	@@

@POST
  L("tmp") = lasteltnode(3);
#  if (pnvar(L("tmp"),"apos-s"))
#    {
#	group(2,3,"_adj");
#	N("apos-s",2) = 1;
#	}
#  else
    group(2,3,"_noun");
  pncopyvars(L("tmp"),N(2));
@RULES
_xNIL <-
	_xSTART
	_abbr [s]
	_xWILD [s plus match=(_xCAP _caps \- )]
	@@

@CHECK
  if (!N("cap",3))
    fail();
  if (N("cap",1) || N("cap",5))
    fail();  # Need bigger handler...
@POST
  L("tmp4") = lasteltnode(4);
  group(2,4,"_caps");
  group(2,2,"_noun");
  pncopyvars(L("tmp4"),N(2));
  chpos(N(2),"NP");
@RULES
_xNIL <-
	_xANY
	_xWILD [s plus match=(_xCAP _caps)]
	_xANY
	_xWILD [s plus match=(_xCAP _caps)]
	_xANY
	@@
_xNIL <-
	_xSTART
	_xWILD [s plus match=(_xCAP _caps)]
	_xANY
	_xWILD [s plus match=(_xCAP _caps)]
	_xANY
	@@


@CHECK
  if (!N("unknown"))
    fail();
  if (N("cap"))
    fail();	# Already handled by caps.
@POST
  L("tmp1") = N(1);
  group(1,1,"_caps");
#  if (pnvar(L("tmp1"),"apos-s"))
#    group(1,1,"_adj");
#  else
    group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  N("cap") = 1;
  N("ne") = 1;

  # Some domain semantics.
  L("lc") = strtolower(N("$text"));
  if (finddictattr(L("lc"),"persname"))
    {
	N("sem") = "person";
    N("ne type") = "person";
	N("ne type conf") = 70;
	}
@RULES
_xNIL <-
	_xWILD [s one match=(_xCAP) except=(_np)]
	@@

@PRE
<1,1> lowercase();
<3,3> lowercase();
@CHECK
  if (N("pos num",2) > 0)
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_caps");
#  if (pnvar(L("tmp2"),"apos-s"))
#    group(2,2,"_adj");
#  else
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"NP");
  N("cap",2) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(_xALPHA _xPUNCT _xNUM)]
	_xWILD [one match=(_xCAP _abbr)]
	_xWILD [s one lookahead match=(_xALPHA _xPUNCT _xNUM _xEND)]
	@@

# alpha name
@POST
  group(1,1,"_name");
  group(1,1,"_noun");
@RULES
_xNIL <-
	_xALPHA
	_name [s]
	@@

#
@CHECK
  if (!N("glom",2) || !N("cap",2))
    fail();
  if (!N("ne",1))
    fail();
@POST
  L("tmp1") = N(1);
  group(2,3,"_caps");
  group(2,2,"_noun");
  N("cap",2) = 1;
  L("ne type") = pnvar(L("tmp1"),"ne type");
  if (L("ne type") == "person")
    {
	N("ne",2) = 1;
	N("ne type",2) = "person";
	N("ne type conf",2) = 50;
	N("stem",2) = N("$text",2);
	N("sem",2) = "person";
	registerpersnames(N("$text",2));
	}
  # Assign POS.
@RULES
_xNIL <-
	_noun
	_noun
	_xCAP [plus]
	_xANY [lookahead]
	@@

