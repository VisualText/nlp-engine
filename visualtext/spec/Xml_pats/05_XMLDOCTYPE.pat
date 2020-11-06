###############################################
# FILE: XML DOCTYPE.pat                       #
# SUBJ: Delimit the document type             #
# AUTH: Paul Deane                            #
# CREATED: 11/Jan/01 11:30:32                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _doctypedecl

@RULES

_Comment <-
    _CommentStart [one] ### (1)
    _xWILD [min=0 max=0 fail=("_CommentEnd" "_CommentEnd" "_DoubleHyphen")] ### (2)
    _CommentEnd [one] ### (3)
    @@

_ExternalID <-
    _xWILD [min=1 max=1 matches=("PUBLIC")] ### (1)
    _whiteSpace [one] ### (2)
    _PubidLiteral [one] ### (3)
    _whiteSpace [one] ### (4)
    _PubidLiteral [one] ### (5)
    @@


_ExternalID <-
    _xWILD [one matches=("PUBLIC")] ### (1)
    _whiteSpace [one] ### (2)
    _PubidLiteral [one] ### (3)
    _whiteSpace [one] ### (4)
    _SystemLiteral [one] ### (5)
    @@

_ExternalID <-
    _xWILD [one matches=("SYSTEM")] ### (1)
    _whiteSpace [opt] ### (2)
    _PubidLiteral [one] ### (3)
    @@

_ExternalID <-
    _xWILD [one matches=("SYSTEM")] ### (1)
    _whiteSpace [opt] ### (2)
    _SystemLiteral [one] ### (3)
    @@
@@RULES
@POST
	S("ElementName") = N("ElementName",1) ;
	single() ;
@@POST
@RULES		
_AttlistDecl [unsealed] <-
    _AttlistDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [plus fail=("_EndTag")] ### (3)
    _whiteSpace [opt] ### (4)
    _EndTag [one] ### (5)
    @@

_PCDataStart <-
		\( [one] ### (1)
		_whiteSpace [opt] ### (2)
		\# [one] ### (3)
		_xALPHA [s one matches=("#PCDATA")] ### (4)
		@@

_EntityDecl <-
    _EntityDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [plus fail=("_EndTag" "_CommentEnd" "_EndEmptyTag" "EndDocType")] ### (3)
    _whiteSpace [opt] ### (4)
    _EndTag [one] ### (5)
    @@

_EntityDecl <-
    _EntityDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [plus fail=("_EndTag" "_CommentEnd" "_EndEmptyTag" "EndDocType")] ### (3)
    _whiteSpace [opt] ### (4)
    _CommentEnd [one] ### (5)
    @@

_EntityDecl <-
    _EntityDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [plus fail=("_EndTag" "_CommentEnd" "_EndEmptyTag" "EndDocType")] ### (3)
    _whiteSpace [opt] ### (4)
    _EndEmptyTag [one] ### (5)
    @@


_EntityDecl <-
    _EntityDeclStart [one] ### (1)
    _whiteSpace [opt] ### (2)
    _xWILD [plus fail=("_EndTag" "_CommentEnd" "_EndEmptyTag" "EndDocType")] ### (3)
    _whiteSpace [opt] ### (4)
    _EndDocType [one] ### (5)
    @@

@@RULES
