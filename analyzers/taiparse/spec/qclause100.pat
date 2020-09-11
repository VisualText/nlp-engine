###############################################
# FILE: qclause100.pat
# SUBJ: Go into a clause.
# AUTH: AM
# CREATED: 09/Jul/04 23:10:22
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause


# Validate well-formed clause.
@POST
  if (!N(3))
    X("no subj") = 1;
  X("vg node") = N(5);

  L("nm") = pnname(N(7));
  if (L("nm") == "_adjc")
    {
	if (N(3))
      X("pattern") = "nvj";
    else
      X("pattern") = "vj";
	}
  else
    {
	if (N(3))
      X("pattern") = "nvp";
    else
      X("pattern") = "vp";
	}

  X("vg") = 1;

  if (N(8))
    L("n") = lasteltnode(8);
  else if (N(7))
    L("n") = lasteltnode(7);
  else if (N(6))
    L("n") = lasteltnode(6);
  else
    L("n") = lasteltnode(5);
  X("last name") = pnname(L("n"));
  L("v") = N("first verb",5);	# 06/05/06 AM.
  if (N(3) || N(7)
   || X("vg count",3) == 1) # Lone verb in sent.
    {
	# Ambiguous VB or VBP.
	fixverb(L("v"),"active","VBP");
	X("voice") = N("voice",5) = "active";
    }
  # Other verb fixup: _aposS.	# 9/18/05 AM.
  fixaposs(X("vg node"),L("v"));	# 9/18/05 AM.

  X("id") = "qclause100 wfj";
#  semclausenvj(X(4),2,3,4,5,6,7,8);  # Domain semantics for clause.
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_adjc _particle)]
	_xWILD [star match=(_advl _adv)]
	@@

# Ellipted copula.
@POST
  X("ellipted-copula") = 1;

  L("nm") = pnname(N(5));
  if (L("nm") == "_adjc")
    X("pattern") = "nvj";
  else
    X("pattern") = "nvp";
  X("id") = "qclause100 n-j";
  X("voice") = "active";
#  semclausenj(X(4),2,3,4,5,6);  # Domain semantics for clause.
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _pro)]			# 3
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_adjc _particle)]	# 5
	_xWILD [star match=(_advl _adv)]
	@@

# Validate well-formed clause.
@POST
  if (!N(3))
    X("no subj") = 1;
  X("vg node") = N(5);

  if (N(3) && N(7))
    X("pattern") = "nvnp";
  else if (N(7))
    X("pattern") = "vnp";
  else if (N(3))
    X("pattern") = "nvp";
  else
    X("pattern") = "vp";

  X("vg") = 1;

  if (N(8))
    L("n") = lasteltnode(8);
  else if (N(7))
    L("n") = lasteltnode(7);
  else if (N(6))
    L("n") = lasteltnode(6);
  else
    L("n") = lasteltnode(5);
  X("last name") = pnname(L("n"));
  if (N(7))
    {
	# Ambiguous -edn or inf.
	fixvg(N(5),"active","VBP");
	X("voice") = "active";
    }
  else if (N(3)
   || X("vg count",3) == 1) # Only 1 verb in sentence.
    fixvg(N(5),"active","VBP");
  X("id") = "qclause100 wf";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_prep
	@@

# Validate well-formed clause.
@POST
  # Determine if lone clause.	# 01/06/05 AM.
  if (X("clauses",3) == 1
   && X("clause num") == 1)
    L("lone clause") = 1;

  if (!N(3))
    X("no subj") = 1;
  X("vg node") = N(5);

  if (N(3) && N(7))
    X("pattern") = "nvn";
  else if (N(7))
    X("pattern") = "vn";
  else if (N(3))
    X("pattern") = "nv";
  else
    X("pattern") = "v";

  X("vg") = 1;

  if (N(8))
    L("n") = lasteltnode(8);
  else if (N(7))
    L("n") = lasteltnode(7);
  else if (N(6))
    L("n") = lasteltnode(6);
  else
    L("n") = lasteltnode(5);
  X("last name") = pnname(L("n"));
  L("v") = N("verb node",5);
  L("firstv") = N("first verb",5);
  if (L("firstv"))
    L("v") = L("firstv");	# 01/08/05 AM.
  if (N(7))
    {
	# Ambiguous -edn or inf.
	fixvg(N(5),"active","VBP");
    }
  if (X("vg count",3) == 1) # Lone verb in sent.
    {
	if (N("voice",5) != "passive")
	  {
  	  fixvg(N(5),"active","VBP");		# 01/07/05 AM.
	  X("voice") = "active";
	  }
	}
  else if (N(3))
    {
	if (N("voice",5) != "passive" &&
	    (L("lone clause")
	     || N("voice",5) == "active" # Not necess resolved.
		 || X("first v in sent")	# First verb in sentence.
		 || verbfeat(L("v"),"T5")	# MBUILD type verb.
		 ))
	  {
	  fixvg(N(5),"active","VBP");
	  X("voice") = "active";
	  }
	
    if (L("v") && !pnvar(L("v"),"mypos"))
      {
	   if (pnvar(L("v"),"inf"))
	    chpos(L("v"),"VBP");
	  }
	}
#  domnvn(N(2),N(3),N(4),N(5),N(6),N(7),N(8));
#  semclausenvn(X(4),2,3,4,5,6,7,8,9);  # Domain semantics for clause.
  X("id") = "qclause100 wf1";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [plus fail=(_prep _np _vg)]
	@@

@POST
  X("pattern") = "nvnn";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _nps)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _nps)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _nps)]
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@

# Query patterns.
# (Could be ditransitive also...)
@POST
  X("pattern") = "vnn";
  X("query") = 1;
  domvnn(X(4),N(3),N(5),N(7));
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _nps)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _nps)]
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@

# Singleton uncharacterized alpha.
@POST
  if (vconjq(N(2),"-en"))
   {
   L("tmp2") = N(2);
   group(2,2,"_verb");
   L("v") = N(2);
   pncopyvars(L("tmp2"),N(2));
   chpos(N(2),"VBN");
   group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
   pncopyvars(L("tmp2"),N(2));
   X("voice") = N("voice",2) = "passive";
   clearpos(N(2),1,0);	# Zero out token info.
   X("pattern") = "v";
   X("vg node") = N(2);
   }
  else
    X("pattern") = "alpha";
#  pncopyvars(N(2),X(4));	# 03/01/05 AM.
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_xEND
	@@

# Patterns with segments.
# np vg np prep seg
@POST
  X("vg count") = 1; # 1 verb in clause.
  if (N(3)
   || X("vg count",3) == 1)
    {
    if (!N("voice",5))
	  X("voice") = N("voice",5) = "active";
	}
  else
    X("no subj") = 1;
  X("vg node") = N(5);
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_prep
	_xWILD [star match=(_advl _adv)]
	_seg
	@@
	
# Component level segments.
# det alpha
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xEND
	@@

@POST
  if (N(3))
    {
    if (!N("voice",5))
	  X("voice") = N("voice",5) = "active";
	}
  else
    X("no subj") = 1;
  X("vg node") = N(5);
  noop();
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [opt match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@

# Decide on some active voice.
@POST
  L("v") = N("verb node",5);
  if (L("v") && !pnvar(L("v"),"mypos") )
    {
    if (pnvar(L("v"),"-edn"))
	  chpos(L("v"),"VBD");
	else if (pnvar(L("v"),"inf"))
	  chpos(L("v"),"VBP");
	}
  if (!N("voice",5))
	  X("voice") = N("voice",5) = "active";
  if (!N(3))
    X("no subj") = 1;

  X("vg node") = N(5);
  X("id") = "qclause100 nv";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

# lone vg at clause start.
@POST
  L("v") = N("verb node",3);
  L("vc") = vconj(L("v"));
  if (L("v") && !pnvar(L("v"),"mypos") )
    {
    if (L("vc") == "-edn")
	  {
	  chpos(L("v"),"VBN");
      if (!N("voice",3))
	    X("voice") = N("voice",3) = "passive";
	  }
	else if (L("vc") == "inf")
	  {
	  chpos(L("v"),"VBP");
      if (!N("voice",3))
	    X("voice") = N("voice",3) = "active";
	  }
	}
  X("no subj") = 1;
  X("vg node") = N(3);
  X("id") = "qclause100 v";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_xEND _prep _qEOS)]
	@@

# prep np conj alpha alpha noun
@CHECK
  if (!N("adj",4) && !N("noun",4))
    fail();
  if (!N("adj",5) && !N("noun",5))
    fail();
@POST
  L("tmp4") = N(4);
  L("tmp5") = N(5);
  if (N("adj",4))
    {
	group(4,4,"_adj");
	pncopyvars(L("tmp4"),N(4));
	fixadj(N(4));
	}
  else
    {
	group(4,4,"_noun");
	pncopyvars(L("tmp4"),N(4));
	fixnoun(N(4));
	}
  if (N("adj",5))
    {
	group(5,5,"_adj");
	pncopyvars(L("tmp5"),N(5));
	fixadj(N(5));
	}
  else
    {
	group(5,5,"_noun");
	pncopyvars(L("tmp5"),N(5));
	fixnoun(N(5));
	}
  L("tmp6") = N(6);
  group(4,6,"_np");
  pncopyvars(L("tmp6"),N(6));
  clearpos(N(4),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _prep)]
	_np
	_conj
	_xALPHA
	_xALPHA
	_noun
	_xWILD [one lookahead match=(_xEND)]
	@@

# np
@CHECK
  if (X("pattern"))
    fail();
@POST
  X("pattern") = "n";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@

# Pure adverbial.
@POST
  X("pattern") = "x";
#  semclauseadvls(X(4),2);
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus match=(_advl _adv)]
	_xEND
	@@
