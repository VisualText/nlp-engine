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

#@PATH _ROOT _TEXTZONE _sent _clause
@NODES _clause

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

# Note: Updated, but still cruddy logic.	# 05/25/07 AM.
# Note: Clause should record if by-np, by-actor seen.
# alpha advl
@CHECK
  if (!N("verb",3))
    fail();
  if (N("noun",3))
    fail();
@POST
  if (pnname(N(4)) == "_np")
  	L("np") = 1;
  else if (N(2))
  	{
	if (pnname(N(2)) == "_np")
		L("np") = 1;
	}
  if (L("np"))
   {
   alphatovg(3,"active","VBP");
   X("voice") = "active";
   }
  else if (!vconjq(N(3),"-en"))
   {
   alphatovg(3,"active","VBP");
   X("voice") = "active";
   }
  else
   {
   alphatovg(3,"passive","VBN");
   X("voice") = "passive";
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
