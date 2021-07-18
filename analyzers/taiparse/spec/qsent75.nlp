###############################################
# FILE: qsent75.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 22/Oct/04 10:35:55
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

# np that clause clause
# that
# Need a verb in last clause.
# "The snail that ate Manhattan, our favorite town, is green."
# that
@CHECK
  if (N("vg count",7))
    fail();
@POST
  N("need verb",7) = 1;
  N("need voice",7) = "active";	# If ambiguous.
  N("need inf",7) = "VBP";
  N("need edn",7) = "VBD";
@RULES
_xNIL <-
	_clause
	that [s]
	_clause
	_xWILD [one match=( \, _dbldash)]
	_clause # Embedded clause.
	_xWILD [one match=( \, _dbldash)]
	_clause # Needs verb.
	@@

# OBSOLETE.	# 05/29/07 AM.
# clause clause
#@PRE
#<2,2> varz("ellipted-that");
#@CHECK
#  if (N("voice",2))
#    fail();
#  if (N("voice",1) != "passive")
#    fail();
#  S("vg") = N("vg node",2);
#  if (!S("vg"))
#    {
#	"err.txt" << "Clause has no vg=" << N("$text",2) << "\n";
#	fail();
#	}
#  S("v") = pnvar(S("vg"),"verb node");
#  if (!S("v"))
#    {
#	if (G("error"))
#	"err.txt" << "Vg has no verb=" << N("$text",2) << "\n";
#	fail();
#	}
#@POST
#  N("qsent75 c-c",1) = 1;
#  if (!pnvar(S("v"),"mypos"))
#    {
#	L("vc") = vconj(S("v"));
#	"vc.txt" << pnvar(S("v"),"$text") << ","<< L("vc") << "\n";
#	if (L("vc") == "-edn")
#	  {
#	  fixvg(S("vg"),"passive","VBN");
#	  N("voice",2) = "passive";
#	  }
#	}
#@RULES
#_xNIL <-
#	_clause
#	_clause [lookahead]
#	@@

# This rule is too loose.  Need constraints before and after.
@CHECK
  S("vg1") = N("vg node",1);
  S("vg3") = N("vg node",3);
  if (!S("vg1") || !S("vg3"))
    {
#	"err.txt" << "no vg: " << phrasetext() << "\n";
    fail();
	}
  S("v1") = pnvar(S("vg1"),"verb node");
  S("v3") = pnvar(S("vg3"),"verb node");
  if (!S("v1") || !S("v3"))
    {
	if (G("error"))
	"err.txt" << "no v: " << phrasetext() << "\n";
	fail();
	}
  if (pnvar(S("v3"),"mypos"))
    fail();
  if (!pnvar(S("v1"),"mypos"))
    fail();
@POST
  L("pos") = pnvar(S("v1"),"mypos");
  L("voice") = pnvar(S("vg1"),"voice");
  if (L("pos") == "VB" && N("first name",3) == "_np")
	L("pos") = "VBP";
  if (samevconj(S("v1"),S("v3")))
  	{
	fixvg(S("vg3"),L("voice"),L("pos"));
	N("voice",3) = pnvar(S("vg3"),"voice");
	}
  N("qsent75 c-and-c",1) = N("qsent75 c-and-c",3) = 1;
@RULES
_xNIL <-
	_clause
	_conj
	_clause
	_xWILD [one lookahead fail=(_dbldash)]
	@@

# clause , clause , clause
@CHECK
  if (N("vg count",6))
    fail();
  if (N("vg count",2))
    fail();
  if (N("pattern",2) != "n")
    fail();
  if (!elliptedpassive(N(4))
   && !elliptedactive(N(4)) )
    fail();
@POST
  N("need verb",6) = 1;
  N("need voice",6) = "active";	# If ambiguous.
  N("need inf",6) = "VBP";
  N("need edn",6) = "VBD";
@RULES
_xNIL <-
	_xSTART
	_clause
	\,
	_clause
	\,
	_clause
	@@

# Try simple stupid.
@CHECK
  if (N("vg count"))
    fail();
  if (N("pattern") != "ving")
    fail();
@POST
  N("need verb") = 1;
@RULES
_xNIL <-
	_clause
	@@

