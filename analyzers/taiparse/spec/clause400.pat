###############################################
# FILE: clause400.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 06/Jan/05 18:45:48
# MODIFIED:
###############################################

@CODE
G("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

# vg to vg
@POST
  L("tmp1") = N(1);
  L("tmp5") = N(5);
  group(1,5,"_vg");
  X("voice") = N("voice",1) = "active";
  # Syntax from 1st.
  N("verb node",1) = pnvar(L("tmp1"),"verb node");
  N("first verb",1) = pnvar(L("tmp1"),"first verb");
  # Semantics from 2nd.
  N("sem",1) = pnvar(L("tmp5"),"sem");
  N("stem",1) = pnvar(L("tmp5"),"stem");

  X("vg node") = N(1);
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_advl _adv)]
	to [s]
	_xWILD [star match=(_advl _adv)]
	_vg
	@@


@POST
  X("to-vg") = N("to-vg",3) = 1;
  listadd(3,1,"true");
@RULES
_xNIL <-
	to [s]
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

# alpha advl
@CHECK
  if (!N("verb",3))
    fail();
  if (N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  group(3,3,"_vg");
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
  clearpos(N(3),1,0);
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  if (vconjq(N(3),"-en"))
    {
	fixvg(N(3),"passive","VBN");
	X("voice") = "passive";
	}
  else
    {
    fixvg(N(3),"active","VBD");
	X("voice") = "active";
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv _np)]
	_xALPHA
	_xWILD [one lookahead match=(_advl _adv _np)]
	@@

# Characterize some clause patterns....
@POST
  if (N("verb",3))
    {
    if (vconjq(N(4),"-ing"))
	  X("pattern") = "ving";
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@
