/*******************************************************************************
* NAME:	JSONREAD.CPP
* FILE:	lite/jsonread.cpp
* CR:		08/04/26 DD.
* SUBJ:	Parse JSON into knowledge base concepts.
* NOTE:	The inverse of json.cpp's serializer, and of python/json2kbb.py,
*			whose mapping this follows:
*
*			  a JSON object        -> a concept
*			  "key": <primitive>   -> an attribute on the concept: key=value
*			  "key": { object }    -> a child concept named key
*			  "key": [ array ]     -> counted children key1, key2, ...
*			                          a primitive element becomes keyN with
*			                          a "value" attribute
*
*			Doing this in the engine rather than in a python pass matters for
*			the npm and pypi distributions: a python pass shells out to an
*			interpreter those environments have no reason to provide.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"
#include "u_out.h"
#include "lite/lite.h"
#include "lite/global.h"
#include "lite/inline.h"
#include "io.h"
#include "dlist.h"
#include "node.h"
#include "tree.h"
#include "pn.h"
#include "Eana.h"
#include "parse.h"
#include "lite/nlppp.h"
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "json.h"

#include <string>
#include <stdlib.h>

/********************************************
* FN:		JR_WS
* SUBJ:	Skip JSON whitespace.
********************************************/

static void jr_ws(const _TCHAR* &p)
{
while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
}

/********************************************
* FN:		JR_UTF8
* SUBJ:	Append a code point to a string as UTF-8.
********************************************/

static void jr_utf8(std::string &out, unsigned cp)
{
if (cp < 0x80)
	out += (char)cp;
else if (cp < 0x800)
	{
	out += (char)(0xC0 | (cp >> 6));
	out += (char)(0x80 | (cp & 0x3F));
	}
else if (cp < 0x10000)
	{
	out += (char)(0xE0 | (cp >> 12));
	out += (char)(0x80 | ((cp >> 6) & 0x3F));
	out += (char)(0x80 | (cp & 0x3F));
	}
else
	{
	out += (char)(0xF0 | (cp >> 18));
	out += (char)(0x80 | ((cp >> 12) & 0x3F));
	out += (char)(0x80 | ((cp >> 6) & 0x3F));
	out += (char)(0x80 | (cp & 0x3F));
	}
}

/********************************************
* FN:		JR_HEX4
* SUBJ:	Read exactly four hex digits.
********************************************/

static bool jr_hex4(const _TCHAR* &p, /*UP*/ unsigned &cp)
{
cp = 0;
for (int k = 0; k < 4; ++k)
	{
	_TCHAR c = *p;
	if (!isxdigit((unsigned char)c))
		return false;
	unsigned d = (unsigned)(isdigit((unsigned char)c)
					? c - '0'
					: tolower((unsigned char)c) - 'a' + 10);
	cp = cp * 16 + d;
	++p;
	}
return true;
}

/********************************************
* FN:		JR_STRING
* SUBJ:	Read a quoted JSON string, resolving escapes.
* NOTE:	Handles \uXXXX, combining a surrogate pair when the low half
*			follows.  A lone surrogate becomes U+FFFD rather than producing
*			invalid UTF-8.
********************************************/

static bool jr_string(const _TCHAR* &p, /*UP*/ std::string &out)
{
out.clear();
if (*p != '"')
	return false;
++p;

while (*p && *p != '"')
	{
	if (*p != '\\')
		{
		out += *p++;
		continue;
		}
	++p;
	switch (*p)
		{
		case '"':  out += '"';  ++p; break;
		case '\\': out += '\\'; ++p; break;
		case '/':  out += '/';  ++p; break;
		case 'b':  out += '\b'; ++p; break;
		case 'f':  out += '\f'; ++p; break;
		case 'n':  out += '\n'; ++p; break;
		case 'r':  out += '\r'; ++p; break;
		case 't':  out += '\t'; ++p; break;
		case 'u':
			{
			++p;
			unsigned cp = 0;
			if (!jr_hex4(p, /*UP*/ cp))
				return false;
			if (cp >= 0xD800 && cp <= 0xDBFF && p[0] == '\\' && p[1] == 'u')
				{
				const _TCHAR *save = p;
				p += 2;
				unsigned lo = 0;
				if (jr_hex4(p, /*UP*/ lo) && lo >= 0xDC00 && lo <= 0xDFFF)
					cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
				else
					{ p = save; cp = 0xFFFD; }
				}
			else if (cp >= 0xD800 && cp <= 0xDFFF)
				cp = 0xFFFD;
			jr_utf8(out, cp);
			}
			break;
		default:
			return false;		// unknown escape
		}
	}

if (*p != '"')
	return false;
++p;
return true;
}

// Values nest.
static bool jr_value(CG *cg, const _TCHAR* &p, CONCEPT *con, const _TCHAR *key);

/********************************************
* FN:		JR_PRIMITIVE
* SUBJ:	Read a number, true, false or null; store it as an attribute.
* NOTE:	An integer becomes a num, anything with . e or E a float,
*			true/false become 1/0, and null becomes an empty string.
********************************************/

static bool jr_primitive(CG *cg, const _TCHAR* &p, CONCEPT *con, const _TCHAR *key)
{
if (!strncmp(p, "true", 4))
	{
	p += 4;
	if (cg && con) cg->addVal(con, (_TCHAR*)key, (long long)1);
	return true;
	}
if (!strncmp(p, "false", 5))
	{
	p += 5;
	if (cg && con) cg->addVal(con, (_TCHAR*)key, (long long)0);
	return true;
	}
if (!strncmp(p, "null", 4))
	{
	p += 4;
	if (cg && con) cg->addVal(con, (_TCHAR*)key, (_TCHAR*)"");
	return true;
	}

const _TCHAR *start = p;
if (*p == '-' || *p == '+')
	++p;
bool isflt = false;
while (*p)
	{
	if (isdigit((unsigned char)*p))
		++p;
	else if (*p == '.' || *p == 'e' || *p == 'E')
		{ isflt = true; ++p; }
	else if ((*p == '-' || *p == '+') && (p[-1] == 'e' || p[-1] == 'E'))
		++p;
	else
		break;
	}
if (p == start)
	return false;

std::string num(start, (size_t)(p - start));
if (!cg || !con)
	return true;
if (isflt)
	cg->addVal(con, (_TCHAR*)key, (float)atof(num.c_str()));
else
	cg->addVal(con, (_TCHAR*)key, (long long)atoll(num.c_str()));
return true;
}

/********************************************
* FN:		JR_OBJECT
* SUBJ:	Read a JSON object into a concept's attributes and children.
********************************************/

static bool jr_object(CG *cg, const _TCHAR* &p, CONCEPT *con, bool skip_id)
{
if (*p != '{')
	return false;
++p;
jr_ws(p);
if (*p == '}')
	{ ++p; return true; }

for (;;)
	{
	jr_ws(p);
	std::string key;
	if (!jr_string(p, /*UP*/ key))
		return false;
	jr_ws(p);
	if (*p != ':')
		return false;
	++p;
	// An array element's "id" is the count that jsonwrite synthesized from
	// the concept's name suffix (item1 -> "id": "1").  Storing it as a real
	// attribute would make the next write emit it twice, so drop it: in this
	// format the name already carries it.
	if (skip_id && key == "id")
		{
		std::string ignored;
		jr_ws(p);
		if (*p == '"')
			{ if (!jr_string(p, ignored)) return false; }
		else if (!jr_primitive(cg, p, 0, "id"))
			return false;
		}
	else if (!jr_value(cg, p, con, key.c_str()))
		return false;
	jr_ws(p);
	if (*p == ',')
		{ ++p; continue; }
	if (*p == '}')
		{ ++p; return true; }
	return false;
	}
}

/********************************************
* FN:		JR_ARRAY
* SUBJ:	Read a JSON array as counted child concepts key1, key2, ...
* NOTE:	Matches json2kbb.py: an object element's members become that
*			child's attributes and children; a primitive element becomes
*			keyN carrying a "value" attribute.
********************************************/

static bool jr_array(CG *cg, const _TCHAR* &p, CONCEPT *con, const _TCHAR *key)
{
if (*p != '[')
	return false;
++p;
jr_ws(p);
if (*p == ']')
	{ ++p; return true; }

long n = 0;
for (;;)
	{
	jr_ws(p);
	++n;
	_TCHAR name[MAXSTR];
	_stprintf(name, "%s%ld", key, n);
	CONCEPT *child = (cg && con) ? cg->makeConcept(con, name) : 0;

	if (*p == '{')
		{
		if (!jr_object(cg, p, child, true))
			return false;
		}
	else if (*p == '[')
		{
		// A nested array has no key of its own; reuse the element name.
		if (!jr_array(cg, p, child, key))
			return false;
		}
	else if (*p == '"')
		{
		std::string s;
		if (!jr_string(p, /*UP*/ s))
			return false;
		if (cg && child)
			cg->addVal(child, (_TCHAR*)"value", (_TCHAR*)s.c_str());
		}
	else
		{
		if (!jr_primitive(cg, p, child, "value"))
			return false;
		}

	jr_ws(p);
	if (*p == ',')
		{ ++p; continue; }
	if (*p == ']')
		{ ++p; return true; }
	return false;
	}
}

/********************************************
* FN:		JR_VALUE
* SUBJ:	Read any JSON value and attach it to con under key.
********************************************/

static bool jr_value(CG *cg, const _TCHAR* &p, CONCEPT *con, const _TCHAR *key)
{
jr_ws(p);
if (*p == '{')
	{
	CONCEPT *child = (cg && con) ? cg->makeConcept(con, (_TCHAR*)key) : 0;
	return jr_object(cg, p, child, false);
	}
if (*p == '[')
	return jr_array(cg, p, con, key);
if (*p == '"')
	{
	std::string s;
	if (!jr_string(p, /*UP*/ s))
		return false;
	if (cg && con)
		cg->addVal(con, (_TCHAR*)key, (_TCHAR*)s.c_str());
	return true;
	}
return jr_primitive(cg, p, con, key);
}

/********************************************
* FN:		JSON_READ_KB
* CR:		08/04/26 DD.
* SUBJ:	Parse a JSON document into concepts under parent.
* RET:	True if the whole document parsed.
* NOTE:	Requires a top-level object, and rejects trailing garbage, so a
*			truncated or doubled file is reported rather than silently
*			half-loaded.
********************************************/

bool json_read_kb(CG *cg, const _TCHAR *text, CONCEPT *parent)
{
if (!cg || !text || !parent)
	return false;
const _TCHAR *p = text;
jr_ws(p);
if (*p != '{')
	return false;
if (!jr_object(cg, p, parent, false))
	return false;
jr_ws(p);
return (*p == '\0');
}
