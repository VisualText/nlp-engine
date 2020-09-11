###############################################
# FILE: clause300.pat
# SUBJ: Gather data.
# AUTH: AM
# CREATED: 22/Sep/04 22:41:27
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

@POST
 X("last chunk") = "v";
 ++X("vg count");
 ++X("vg count",3);	# Count in sentence also. # 01/07/05 AM.
 if (N("voice"))
   X("voice") = N("voice");	# Copy active/passive to clause.
 if (N("last vg"))
   X("last vg") = N("last vg");
 if (N("first vg"))
   X("first vg") = N("first vg");
 if (!X("vg node"))
   X("vg node") = N(1);
 if (!X("voice") && N("voice"))
   X("voice") = N("voice");
 X("last") = N(1);
 if (!X("first name"))
   X("first name") = "_vg";
 if (!X("first vclause",3))
   {
   X("first vclause",3) = X(4);	# First clause with vg.
   X("first v in sent",4) = 1;
   }
 # Todo: check for multiple vgs...
@RULES
_xNIL <-
	_vg
	@@

# np
@CHECK
  if (X("pattern"))
    fail();
@POST
  X("last chunk") = "n";
  X("pattern") = "n";
  X("np") = N(3);
  if (N(4))
    {
	L("tmp") = lasteltnode(4);
	X("last") = L("tmp");
	}
  else
    X("last") = N(3);
  if (!X("first name"))
    {
	if (N(2))
	  X("first name") = pnname(N(2));
	else
	  X("first name") = "_np";
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@

@POST
  X("last chunk") = "n";
  X("last") = N(1);
  if (!X("first name"))
    X("first name") = "_np";
@RULES
_xNIL <-
	_np
	@@

@POST
  X("last chunk") = "v";
  X("start to-vg") = 1;
 ++X("vg count");
 ++X("vg count",3);	# Count in sentence also. # 01/07/05 AM.
 if (N("voice"))
   X("voice") = N("voice");	# Copy active/passive to clause.
 if (N("last vg"))
   X("last vg") = N("last vg");
 if (N("first vg"))
   X("first vg") = N("first vg");
 if (!X("vg node"))
   X("vg node") = N(5);

  X("last") = N(5);
  if (!X("first name"))
    {
	if (N(2))
	  X("first name") = pnname(N(2));
	else
	  X("first name") = "_prep";
	}

  # Todo: check for multiple vgs...
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	to [s]
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

@POST
  X("last chunk") = "p";
@RULES
_xNIL <-
	_prep
	@@

@POST
  X("last") = N(1);
  if (!X("first name"))
    X("first name") = "_advl";
  if (N("by-actor"))	# 12/07/05 AM.
    {
	if (!X("by-actor"))
	  X("by-actor") = N("by-actor");
	++X("num by-actor");
	if (!X("voice"))
	  X("voice") = "passive"; # Fix voice up...
	}
@RULES
_xNIL <-
	_advl
	@@

@POST
  X("last") = N(1);
  if (!X("first name"))
    X("first name") = "_adjc";
  X("last chunk") = "j";
@RULES
_xNIL <-
	_adjc
	@@

@POST
  X("last") = N(1);
  if (!X("first name"))
    X("first name") = "_adv";
@RULES
_xNIL <-
	_adv
	@@

