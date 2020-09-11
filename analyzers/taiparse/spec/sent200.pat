###############################################
# FILE: sent200.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 13/Jul/04 23:38:31
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

@POST
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
  # compose nps.
  singler(2,4);
@RULES
_np <-
	_vg
	_np
	_conj
	_np
	_xEND
	@@

# np , alpha seg , 
@CHECK
  if (N("seg type",4) != "np")
    fail();
  if (N("seg first",4) != "adj"
   && N("seg first",4) != "noun")
     fail();
@POST
  L("tmp3") = N(3);
  if (N("verb",3))
    {
	L("vc") = vconj(N(3));
	if (L("vc") != "-edn"
	 && L("vc") != "-en"
	 && L("vc") != "-ing")
	  ;
	else
	  {
	  group(3,3,"_adj");
	  pncopyvars(L("tmp3"),N(3));
	  fixadj(N(3));
	  L("done") = 1;
	  listadd(4,3,"false");
	  }
	}

  if (L("done"))
    ; # Twiddle thumbs.
  else if (N("adj",3))
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	listadd(4,3,"false");
	}
  else if (N("noun",3))
    {
	group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	fixnoun(N(3));
	listadd(4,3,"false");
	}
@RULES
_xNIL <-
	_np
	\,
	_xALPHA
	_seg
	\,
	@@

# Common sentential start...
# np , vg
@CHECK
  if (N("voice",7))
    fail();
@POST
  fixvg(N(7),"passive","VBN");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	\,
	_xWILD [star match=(_advl _adv)]
	_vg
	@@
