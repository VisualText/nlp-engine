###############################################
# FILE: entity100.pat
# SUBJ: Named entities.
# AUTH: AM
# CREATED: 10/Oct/04 15:39:08
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE

@POST
  S("sem") = S("ne type") = "organization";
  S("ne type conf") = 95;
  S("stem") = phrasetext();
  S("ne") = 1;
  single();
@RULES
_caps [layer=_noun] <-
	united [s]
	_xWHITE [star]
	nations [s]
	@@

_caps [layer=_noun] <-
	tamil [s]
	_xWHITE [star]
	_xANY
	@@

@PRE
<1,1> cap();
@POST
  pncopyvars();
  S("sem") = S("ne type") = "organization";
  S("ne type conf") = 95;
  S("stem") = phrasetext();
  S("ne") = 1;
  S("number") = "singular";
  single();
@RULES

_caps [layer=_noun] <- _xWILD [s one match=(
	un unicef bloomberg reuters
	bbc nbc cnn
	)] @@

@POST
  S("sem") = S("ne type") = "organization";
  S("ne type conf") = 95;
  S("stem") = "EU";
  S("ne") = 1;
  single();
@RULES
_caps [layer=_noun] <-
	eu
	@@

# US States
# n.h.
@POST
  S("ne") = 1;
  S("ne type") = "location";
  S("ne type conf") = 85;
  S("sem") = "us_state";
  S("stem") = phrasetext();
  S("mypos") = "NP";
  if (N(5))
    S("pos eos") = 1;
  S("cap") = 1;
  single();
@RULES
_usstate [layer=_noun] <-
	n
	\.
	_xWHITE [star]
	h
	\. [opt]
	@@


# US States
@PRE
<1,1> cap();
@POST
 S("sem") = "us_state";
 S("ne type") = "location";
 S("ne type conf") = 85;
 S("ne") = 1;
 pncopyvars(1);
 S("stem") = phrasetext();
 S("mypos") = "NP";
 if (N(2))
   S("poss eos") = 1;
single();
@RULES
_usstate [layer=_noun] <-
	_xWILD [s one match=(
AA
AE
AK
AL
Ala
Alb
Alabama
#Alas
Alaska
AP
AR
Ari
Ariz
Arizona
#Ark
Arkan
Arkansas
#AS
AZ
CA
Cal
Calif
California
#CO
Col
Colo
Colorado
Columbia
Conn
Connecticut
CT
Dak
Dakota
DC
#DE
Del
Dela
Delaware
FL
Fla
Flo
Flor
Florida
#FM
GA
Geo
Georg
Georgia
GU
Guam
Haw
Hawaii
#HI
IA
#ID
Ida
Idaho
#IL
#Ill
Illin
Illinois
#IN
Ind
Indiana
Iowa
Kan
Kans
Kansas
#Ken
#Kent
Kentucky
KS
KY
#LA
#Louis
Louisiana
#MA
Maine
#Mary
Maryland
#Mass
Massachusetts
#MD
#ME
MH
MI
Mich
Michigan
Micronesia
Minn
Minnesota
#Miss
Mississippi
Missouri
MN
MO
Mon
Mont
Montana
MP
#MS
MT
NC
ND
NE
Neb
Nebraska
Nev
Nevada
NH
NJ
NM
NSW
NV
NY
#OH
Ohio
#OK
Okl
Okla
Oklahoma
#OR
#Ore
Oreg
Oregon
Orpalau
#PA
Penn
Pennsyl
Pennsylvania
PR
PW
RI
Samoa
SC
SD
Tenn
Tennessee
Tex
Texas
TN
TX
UT
Utah
VA
Ver
Verm
Vermont
#VI
Vir
Virg
Virgin
Virginia
VT
WA
Wash
Washington
WI
Wis
Wisc
Wiscon
Wisconsin
WV
WY
Wyo
Wyoming
)]
\. [opt]
	@@

# Hard-wiring some plural proper names. # 05/27/07 AM.
@PRE
<1,1> cap();
@POST
  N("mypos") = "NPS";	# cap/NPS
@RULES
_xNIL <-
	_xWILD [s one match=(
americans
beatles
communists
germans
jews
russians
soviets
virginians
		)]
	@@

# Hard-wiring some development stuff so I don't keep
# stumbling across it as an error.	# 05/28/07 AM.

# composer name name
# jobtype cap cap
@POST
  if (N("cap",1))
    {
  	N("cap",1) = 0;
	setbase(1,"true");	# Don't look for the capitalization here.
	}
  N("mypos",2) = N("mypos",3) = "NP";
  group(2,3,"_noun");
  N("ne type",2) = "person";
  N("ne type conf",2) = 95;
  N("ne",2) = 1;
  N("sem",2) = "person";
@RULES
_xNIL <-
	_xWILD [s one match=(composer)]
	_xCAP [s]
	_xCAP [s]
	_xWILD [s one lookahead fail=(_xCAP)]
	@@
