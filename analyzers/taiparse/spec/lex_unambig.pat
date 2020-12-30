###############################################
# FILE: lex_unambig.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 06/Jul/04 02:14:41
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE

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
		   if (N("pos") == "_noun")	# 09/04/04 AM.
		     {
			 L("num") = number(N(1));
			 if (L("num"))
			   N("number") = L("num");
			 }
		   # Doing this in nodepos().	# 06/22/06 AM.
#		   if (N("pos") == "_adj") # 09/15/04 AM.
#		     {
#			 L("txt") = strtolower(N("$text"));
#			 if (L("myp") = adjconj(L("txt")))
#			   N("mypos") = L("myp");
#			 }
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
