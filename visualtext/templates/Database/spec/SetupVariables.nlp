###############################################
# FILE: SetVariables.pat                      #
# SUBJ: Mark and neutralize double quotes.    #
# AUTH: David de Hilster                      #
# Copyright 2001, Text Analysis International #
###############################################

@CODE
G("enclosed by") = "\"";
G("separator") = ",";
G("lineTerminator") = "\n";
G("escaped by") = "\"";
@@CODE

@NODES _ROOT

@RULES
_enclosedBy <-
    \"	### (1)
    @@

_separator <-
    \,	### (1)
    @@

_lineTerminator <-
    \r [optional]	### (1)
    \n				### (2)
    @@

_escapedBy <-
    \\	### (1)
    @@
