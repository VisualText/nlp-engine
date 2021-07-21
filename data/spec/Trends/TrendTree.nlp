###############################################
# FILE: trend_tree.pat
# SUBJ: Traverse the dictionary tree and dump the frequencies.
# AUTH: AM
# CREATED: 02/Oct/00 13:16:41
# METHOD:	Iteratively traverse the two-level dictionary hierarchy.
# Copyright 2000, Text Analysis International
###############################################

@CODE

G("trend file") << "TRENDS: RAW ALPHABETIC FREQUENCY" << "\n";
G("trend file") << "================================" << "\n";

# TRAVERSE DICTIONARY HIERARCHY. (Has 2 levels of dictionary indices.)
G("dict level 1") = down(G("dict alpha"));				# Get level 1.

while (G("dict level 1"))								# Traverse level 1.
	{
	G("dict level 2") = down(G("dict level 1"));		# Get level 2.
	while (G("dict level 2"))							# Traverse level 2.
		{
		G("dict word") = down(G("dict level 2"));		# Get word concepts.
		while (G("dict word"))
			{
			# Print out words and their frequencies.
			G("freq") = numval(G("dict word"), "frequency");
			if (G("freq") > 0)
				G("trend file")
								<< rightjustifynum(G("freq"),5)
								<< " "
								<< LJ(conceptname(G("dict word")),12)
								<< "\n";
			G("dict word") = next(G("dict word"));		# Get next word.
			}
		G("dict level 2") = next(G("dict level 2"));	# Next level 2 index.
		}
	G("dict level 1") = next(G("dict level 1"));		# Next level 1 index.
	}
@@CODE
