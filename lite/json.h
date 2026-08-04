/*******************************************************************************
* NAME:	JSON.H
* FILE:	lite/json.h
* CR:		08/04/26 DD.
* SUBJ:	Knowledge base to JSON serialization.
* NOTE:	See json.cpp.  Output is byte-compatible with the JsonKB family in
*			visualText/spec/KBFuncs.nlp, which it replaces.
*******************************************************************************/

#ifndef JSON_H_
#define JSON_H_

// NOTE: include this AFTER consh/cg.h -- CONCEPT and CG come from there.

// Write a concept tree to path as JSON.  True if the file was written.
bool json_write_kb(CG *cg, const _TCHAR *path, CONCEPT *con);

// Parse a JSON document into concepts under parent.  True if it parsed.
bool json_read_kb(CG *cg, const _TCHAR *text, CONCEPT *parent);

#endif
