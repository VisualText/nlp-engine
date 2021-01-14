###############################################
# FILE: mhbv100.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 09/Jul/04 01:59:30
# MODIFIED:
###############################################

@CODE
L("hello") = 0;

if (G("studyout"))
  closefile(G("studyout"));
@@CODE

@NODES _sent

@CHECK
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),N(9));
  if (!L("arr")[0])
    fail();
@POST
 L("m") = N(1);
 L("h") = N(3);
 L("b") = N(5);
 L("being") = N(7);
 L("v") = N(9);
 L("neg") = mhbvadv(2,4,6,8);
 if (N(7))
   {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
   }
 # Should be a more compact way to do the below...
 # At least group could return the grouped node.
 if (N(1))
   {
   group(1,9,"_vg");
   mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   }
 else if (N(3))
   {
   group(3,9,"_vg");
   mhbv(N(3),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   }
 else if (N(5))
   {
   group(5,9,"_vg");
   mhbv(N(5),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   }
 else if (N(7))
   {
   group(7,9,"_vg");
   mhbv(N(7),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   }
 else
   {
   group(9,9,"_vg");
   mhbv(N(9),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   }
 
@RULES
_xNIL <-
	_xWILD [s opt match=(_modal _do)]
	_xWILD [star match=(_adv _advl)]
	_have [s opt]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(_verb) except=(_modal _have _be _vg)]
	@@

####### MHB ALPHA

@CHECK
  if (!N("verb",9))
    fail();
  if (!N(1) && !N(3) && !N(5) && !N(7)) # 06/15/06 AM.
    fail();
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),N(9));
  if (!L("arr")[0])
    fail();
@POST
  L("tmp9") = N(9);
  group(9,9,"_verb");
  pncopyvars(L("tmp9"),N(9));
 L("m") = N(1);
 L("h") = N(3);
 L("b") = N(5);
 L("being") = N(7);
 L("v") = N(9);
 if (N(7))
   {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
   }
  L("neg") = mhbvadv(2,4,6,8);
 group(1,9,"_vg");
 mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
	_xWILD [s opt match=(_modal _do)]
	_xWILD [star match=(_adv _advl)]
	_have [s opt]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

@CHECK
  if (!N("verb",7))
    fail();
  # Agreement...
  L("arr") = vgagree(0,N(1),N(3),N(5),N(7));
  if (!L("arr")[0])
    fail();
@POST
  L("tmp7") = N(7);
  group(7,7,"_verb");
  pncopyvars(L("tmp7"),N(7));
 L("m") = 0;
 L("h") = N(1);
 L("b") = N(3);
 L("being") = N(5);
 L("v") = N(7);
 if (N(5))
   {
    N("sem",5) = N("stem",5) = "be";
    chpos(N(5),"VBG");
   }
  L("neg") = mhbvadv(2,4,6,0);
 group(1,7,"_vg");
 mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
	_have [s]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

@CHECK
  if (!N("verb",5))
    fail();
  # Agreement...
  L("arr") = vgagree(0,0,N(1),N(3),N(5));
  if (!L("arr")[0])
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_verb");
  pncopyvars(L("tmp5"),N(5));
 L("m") = 0;
 L("h") = 0;
 L("b") = N(1);
 L("being") = N(3);
 L("v") = N(5);
 if (N(3))
   {
    N("sem",3) = N("stem",3) = "be";
    chpos(N(3),"VBG");
   }
  L("neg") = mhbvadv(2,4,0,0);
 group(1,5,"_vg");
 mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
	_be [s]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

##### VERBLESS

@CHECK
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),0);
#  "err.txt" << L("arr") << phrasetext() << "\n";
  if (!L("arr")[0])
    fail();
  S("voice") = "active";
@POST
  L("m") = N(1);
  L("h") = N(3);
  L("b") = N(5);
  L("being") = N(7);
 if (N(7))
   {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
   }
  L("neg") = mhbvadv(2,4,6,0);
  group(1,7,"_vg");
  mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),0);
  N("voice",1) = S("voice");
@RULES
_xNIL <-
	_xWILD [s one match=(_modal _do) except=(_vg _np)]
	_xWILD [star match=(_adv _advl)]
	_have [s opt]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	@@

@CHECK
  # Agreement...
  L("arr") = vgagree(0,N(1),N(3),N(5),0);
  S("verb node") = N(1);
  if (!L("arr")[0])
    fail();
#  S("voice") = "active";
@POST
 L("h") = N(1);
 L("b") = N(3);
 L("being") = N(5);
 if (N(5))
   {
    N("sem",5) = N("stem",5) = "be";
    chpos(N(5),"VBG");
   }
  L("neg") = mhbvadv(2,4,0,0);
 if (N(5))
   group(1,5,"_vg");
 else if (N(3))
	group(1,3,"_vg");
 else
	group(1,1,"_vg");
 mhbv(N(1),L("neg"),0,L("h"),L("b"),L("being"),0);

@RULES
_xNIL <-
	_xWILD [s one match=(_have) except=(_vg)]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	@@

@CHECK
  # Agreement...
  L("arr") = vgagree(0,0,N(1),N(3),0);
  if (!L("arr")[0])
    fail();
  S("voice") = "active";

 if (N(3))
   {
    N("sem",3) = N("stem",3) = "be";
    chpos(N(3),"VBG");
   }
@RULES
_vg <-
	_be [s]
	_xWILD [star match=(_adv _advl)]
	being [s]
	@@

# be
@POST
  L("tmp1") = N(1);
  L("neg") = mhbvadv(2,0,0,0);
  group(1,1,"_vg");
  mhbv(N(1),L("neg"),0,0,L("tmp1"),0,0);
  pncopyvars(L("tmp1"),N(1));
  N("voice",1) = "active";
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_be [s except=(_vg)]
	_adv [star]
	_xWILD [one fail=(_xALPHA _verb)]
	@@
_xNIL <-
	_be [s except=(_vg)]
	_adv [star]
	_xEND
	@@


# noun alpha by
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-en"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("vb") = N(2);
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_vg");
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "passive";
  N("verb node",2) = L("vb");
  clearpos(N(2),1,0);	# Zero out token info.
  N("ellipted-rel",2) = 1;
@RULES
_xNIL <-
	_noun
	_xALPHA
	_prep [lookahead]
	@@

# to alpha
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  L("tmp") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp"),N(2));
  chpos(N(2),"VB");	# Infinitive.
  group(2,2,"_vg");
  pncopyvars(L("tmp"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	to [s]
	_xALPHA
	_xWILD [lookahead one match=(_conj _prep _fnword
		_det _quan _num _xNUM _adj _noun \,)]
	@@

# noun alpha noun
# to alpha noun
# prep alpha noun
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"inf"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  L("v") = N(3);
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"active","VB");
  L("neg") = mhbvadv(2,0,0,0);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("voice",3) = "active";
  clearpos(N(3),1,0);	# Zero out token info.
@RULES
#_xNIL <-
#	_xWILD [one match=(_noun _np _pro)]
#	_xWILD [star match=(_adv _advl)]
#	_xALPHA
#	_xWILD [star match=(_adv _advl)]
#	_xWILD [one lookahead match=(_noun _np _pro _det _prep _conj)]
#	@@

# to alpha
_xNIL <-
	_xWILD [s one match=(to)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_xWILD [one lookahead match=(_noun _np _pro _det _prep _conj
		_fnword _whword \, )]
	@@

# to alpha
@CHECK
  if (!N("verb",3))
   fail();
  if (!vconjq(N(3),"inf"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  L("neg") = mhbvadv(2,0,0,0);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
  fixvg(N(3),"active","VB");
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(to)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

