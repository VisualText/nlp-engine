###############################################
# FILE: XMLDecl.pat                           #
# SUBJ: Recognize comment starts, ends , etc. #
# AUTH: Paul Deane                            #
# CREATED: 10/Jan/01 22:56:12                 #
# DATE OF THIS VERSION: 31/Aug/01             #
# Copyright 2001, Text Analysis International #
###############################################

@PATH _ROOT _XMLDecl

@RULES


_Comment <-
	_CommentStart
	_xWILD [min=0 max=0 fail=("_CommentEnd" "_CommentEnd" "_DoubleHyphen")]
	_CommentEnd
	@@

_VersionInfo <-
	_whiteSpace [opt]
	_xWILD [s trig one matches=("version")]
	_whiteSpace [opt]
	\=
	_whiteSpace [opt]
	_PubidLiteral
	@@

_VersionInfo <-
	_whiteSpace [opt]
	_xWILD [s trig one matches=("version")]
	_whiteSpace [opt]
	\=
	_whiteSpace [opt]
	_SystemLiteral
	@@

_EncodingDecl <-
	_whiteSpace [opt]
	_xWILD [s trig one matches=("encoding")]
	_whiteSpace [opt]
	\=
	_whiteSpace [opt]
	_PubidLiteral
	@@

_EncodingDecl <-
	_whiteSpace [opt]
	_xWILD [s trig one matches=("encoding")]
	_whiteSpace [opt]
	\=
	_whiteSpace [opt]
	_SystemLiteral
	@@

_SDDecl <-
	_whiteSpace [opt]
	_xWILD [s trig one matches=("standalone")]
	_whiteSpace [opt]
	\=
	_whiteSpace [opt]
	_PubidLiteral
	@@

_SDDecl <-
	_whiteSpace [opt]
	_xWILD [s trig one matches=("standalone")]
	_whiteSpace [opt]
	\=
	_whiteSpace [opt]
	_SystemLiteral
	@@

@POST
	noop() ;
@@POST
@RULES
	_xNIL <- _whiteSpace
	@@
@@RULES

@RULES
@POST
	"output.txt" << "Error: extra characters in XML declaration" ;
	noop() ;
@@POST
@RULES
	_xNIL <- _xALPHA @@
@@RULES
