###############################################
# FILE: trend_init.pat
# SUBJ: Initialize emergent trend collection
# AUTH: AM
# CREATED: 02/Oct/00 12:34:12
# METHOD:	We'll use the dictionary system already in place
#	in the knowledge base (KB) in order to track words and
#	their frequencies.  We'll start simple, and move to more
#	complex things like phrases and concepts eventually.
#
#	1. Increment a counter for each word encountered in text.
#	2. Traverse the dictionary to output counters.
#
#	Can keep alphabetic case intact, for starters.
#	Unfortunately, can't write a recursive function yet in NLP++!
#	Can traverse iteratively.  Keep a concept pointer to each level
#	we are at. (Only 2 levels).
# Copyright 2000, Text Analysis International
###############################################

@CODE
# Open an output file for writing trend info.
G("trend file") = ofile("trend.txt");

# Get the root concept of the dictionary.
G("sys")		= getconcept(G("root"), "sys" );
G("dict")		= getconcept(G("sys" ), "dict");
G("dict alpha") = getconcept(G("dict"), "a"   );	# Alphabetic dictionary hierarchy.
@@CODE
