###############################################
# FILE: lex_unambig2.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 08/Aug/04 22:05:07
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@POST
	if (N("pos num") == 1)	# UNAMBIGUOUS PART OF SPEECH.
		{
		if (N("pos"))
		   {
		   G("node") = N(1);
		   group(1,1,N("pos"));
		   pncopyvars(G("node"),N(1));	# 12/14/01 AM.
		   if (!N("sem"))	# 01/01/02 AM.
		     {
			 if (N("text"))
			   S("tmp") = N("text");
			 else
			   S("tmp") = strtolower(N("$text"));
		     S("stem") = nvstem(S("tmp"));
		     if (S("stem"))
		        N("sem") = S("stem");
		     else
		        N("sem") = S("tmp");
			 }
		   }
		}
	# STARTING TO USE CONFIDENCE, FREQUENCY.	# 06/02/02 AM.
	else if (N("verb") >= 90)
	  {
      S("tmp") = N(1);
      group(1,1,"_verb");
      pncopyvars(S("tmp"),N(1));
	  }
	else if (N("adv") >= 90)
	  {
      S("tmp") = N(1);
      group(1,1,"_adv");
      pncopyvars(S("tmp"),N(1));
	  }

@RULES
_xNIL <-
	_xALPHA	# Uncharacterized, unreduced.
	@@
