###############################################
# FILE: ini.pat
# SUBJ: Initializations.
# AUTH: AM
# CREATED: 05/Jul/04 20:07:12
# ANALYZER: TAIPARSE is a general analyzer of English text.
###############################################

@CODE

L("hello") = 0;

# If verbose outputs.
G("verbose") = 1;

# If error outputs.
G("error") = 1;

# If expecting and zapping HTML/XML input.
G("find html") = 0;

if (G("find html"))	# Some html setup.
  {
  }

# Flag allcaps text.
# This could also be sleuthed by the analyzer and checked
# on a local basis (eg, group of five all-cap words).
G("allcap") = 0;

# If dealing with pretagged input texts.
G("pretagged") = 1;

# Confidence threshold for outputting tags 0-100.
# For now, using 0 to "guess" if nothing has been assigned.
# Note: Computing confidence is not implemented.
G("thresh") = 0;

# If gathering global data on rule performance.
G("posacct") = 1;

if (!G("pretagged"))
  G("posacct") = 0;
if (G("posacct"))
  posacctini();

# If printing out an embedded tagged output file.
# (Assuming input is plain text.)
G("xml recursive") = 0;

# If printing out xml tags only (excluding embedded text).
G("xml tags only") = 0;

# If bracketing noun phrases.
G("bracket") = 1;

# If printing a treebank header.
G("treebank") = 1;

# If pos tagging to conform as well as possible to Treebank
# assignments that I disagree with.
G("conform treebank") = 1;

# If printing filenames with mismatch diagnostic output.
G("mismatch verbose") = 1;

if (G("pretagged"))
  {
  # Set up scoring.
  G("scorepos") = getconcept(findroot(),"scorepos");
  
  # Reset scoring for current document.
  replaceval(G("scorepos"),"currgood",0);
  replaceval(G("scorepos"),"currtot",0);
  
  # Accumulate for total document set.
  # allgood
  # alltot
  
  # Collect single-word mismatches.	# 06/14/06 AM.
  L("fname") = G("$apppath") + "\\data\\mismatch.txt";
  G("mismatch out") = openfile(L("fname"),"app");
  
  # Collect single-word zeros. # 06/23/06 AM.
  L("fname") = G("$apppath") + "\\data\zero.txt";
  G("zero out") = openfile(L("fname"),"app");
  }

#### Initialize some general semantic handling in KB.
G("semantic processing") = 1;

if (G("semantic processing"))
  seminit();	# See semfuns pass.

@@CODE

# DEV NOTE: _adjc will be a useful concept for adj constituent
# analogous to _np and _vg.
