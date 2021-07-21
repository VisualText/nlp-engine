###############################################
# FILE: country.pat
# SUBJ: Automatically generated.
# AUTH: file2pass
# CREATED: Thu Jan 13 11:39:58 2005
###############################################

# TODO: PUT YOUR CONTEXT IN HERE.
@NODES _TEXTZONE

# TODO: SET UP USER ACTIONS HERE.
@POST
  pncopyvars(1);	# 06/05/06 AM.
  S("ne text") = phrasetext();
  S("ne type") = "location";
  S("ne type conf") = 95;
  S("ne") = 1;
  S("sem") = "location";
  S("sem location") = "country";
  S("mypos") = "NP";
  S("country") = S("stem") = phrasetext();
  if (G("verbose"))
    "country.txt" << phrasetext() << "\n";
  single();
@RULES

_country [layer=(_noun)] <- 
	Bangladesh
	@@

_country [layer=(_noun)] <- 
	Burma
	@@

_country [layer=(_noun)] <- 
	Cambodia
	@@

_country [layer=(_noun)] <- 
	China
	@@

_country [layer=(_noun)] <- 
	India
	@@

_country [layer=(_noun)] <- 
	Indonesia
	@@

_country [layer=(_noun)] <- 
	Laos
	@@

_country [layer=(_noun)] <- 
	Malaysia
	@@

_country [layer=(_noun)] <- 
	Myanmar
	@@

_country [layer=(_noun)] <- 
	Singapore
	@@

_country [layer=(_noun)] <- 
	Sri
	_xWHITE
	Lanka
	@@

_country [layer=(_noun)] <- 
	Thailand
	@@

_country [layer=(_noun)] <- 
	Vietnam
	@@
