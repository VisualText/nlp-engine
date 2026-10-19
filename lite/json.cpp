/*******************************************************************************
* NAME:	JSON.CPP
* FILE:	lite/json.cpp
* CR:		08/04/26 DD.
* SUBJ:	Serialize a knowledge base concept tree to JSON.
* NOTE:	This is a C++ reimplementation of the JsonKB family in
*			visualText/spec/KBFuncs.nlp (JsonKB, JsonKBRecurse, JsonAttributes,
*			JsonStr).  Those are interpreted NLP++, and serializing the KB is
*			where a knowledge-base analyzer actually spends its time: measured
*			on the date-time analyzer, the 13-line output pass that just calls
*			JsonKB() and SaveKB() was 43.9% of total runtime.
*
*			Memoizing the indent helper in NLP++ was tried first and bought
*			only 4.6% -- the cost is spread across interpreted execution
*			itself (roughly 73us per emitted line), so moving the whole walk
*			into C++ is the only lever that removes it.
*
* WARN:	The output must stay BYTE-IDENTICAL to what JsonKB produces, because
*			existing analyzers' output files are checked in and compared.
*			JsonKB and this file were fixed together (4.2.37), so they still
*			agree: an array followed by a plain sibling, two arrays side by
*			side, and a digit inside a counted name -- see separate_con_count
*			and json_recurse below.
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
#include <fstream>

/********************************************
* FN:		JSON_ESCAPE
* CR:		08/04/26 DD.
* SUBJ:	Quote and escape a string, as KBFuncs.nlp's JsonStr does.
* NOTE:	JsonStr escapes exactly two characters, backslash first then the
*			double quote, and wraps the result in quotes.  It does NOT escape
*			control characters, so neither does this -- byte compatibility
*			wins over strict JSON here.  Fixing that belongs with fixing
*			JsonStr itself, in one change, so the two cannot drift.
********************************************/

static std::string json_escape(const _TCHAR *str)
{
std::string out;
out.reserve(str ? (strlen(str) + 8) : 2);
out += '"';
for (const _TCHAR *p = str; p && *p; ++p)
	{
	if (*p == '\\')
		out += "\\\\";
	else if (*p == '"')
		out += "\\\"";
	else
		out += *p;
	}
out += '"';
return out;
}

/********************************************
* FN:		JSON_INDENT
* CR:		08/04/26 DD.
* SUBJ:	The indent for a nesting level.
* NOTE:	KBFuncs.nlp's SpacesStr(n) is RepeatStr(n,"  "), and RepeatStr
*			starts with one copy and appends while n<num -- so it emits
*			max(1,n) copies, NOT n.  SpacesStr(0) is therefore two spaces,
*			not zero.  Reproduced exactly.
********************************************/

static std::string json_indent(long level)
{
long copies = (level < 1 ? 1 : level);
return std::string((size_t)(copies * 2), ' ');
}

/********************************************
* FN:		SEPARATE_CON_COUNT
* CR:		08/04/26 DD.
* SUBJ:	Split a counted concept name into base name and count.
* RET:	True if the name ends in digits; base and num are then set.
* NOTE:	Mirrors SeparateConCount in KBFuncs.nlp.
*
*			The scan stops at the first non-digit from the end, so "a1b2" is
*			base "a1b" and count "2".  Until 4.2.37 both this and the NLP++
*			original kept collecting digits past the letter (the NLP++ else
*			branch was the no-op `L("stop");`), making "a1b2" base "a1" and
*			count "12".
*
*			The loop condition is `i > 0`, so index 0 is never examined: a
*			name that is all digits after the first character still keeps its
*			first character as the base.
********************************************/

static bool separate_con_count(
	const _TCHAR *conname,
	/*UP*/
	std::string &base,
	std::string &num
	)
{
base.clear();
num.clear();
if (!conname || !*conname)
	return false;

// strisdigit(conname): an all-digit name is not a counted concept.
bool alldigit = true;
for (const _TCHAR *p = conname; *p; ++p)
	if (!isdigit((unsigned char)*p))
		{ alldigit = false; break; }
if (alldigit)
	return false;

long len = (long) strlen(conname);
// NOTE: `i > 0`, matching the NLP++ loop, so index 0 is never looked at.
for (long i = len - 1; i > 0; --i)
	{
	if (!isdigit((unsigned char)conname[i]))
		break;
	num.insert(num.begin(), conname[i]);	// prepend, as L("c") + L("num")
	}

if (num.empty())
	return false;

// base = strpiece(conname, 0, len - numlen - 1), inclusive of both ends.
long baselen = len - (long) num.size();
if (baselen < 0)
	baselen = 0;
base.assign(conname, (size_t) baselen);
return true;
}

/********************************************
* FN:		IS_COUNT_CON
* CR:		08/04/26 DD.
* SUBJ:	Is this attribute name the "<base>_count" of a counted child set?
* NOTE:	Mirrors IsCountCon: true when the concept has a child named
*			"<str>1", in which case the attribute is bookkeeping and is
*			left out of the JSON.
********************************************/

static bool is_count_con(CG *cg, const _TCHAR *str, CONCEPT *con)
{
if (!cg || !str || !con)
	return false;
std::string first(str);
first += "1";

_TCHAR nbuf[MAXSTR];
for (CONCEPT *child = cg->Down(con); child; child = cg->Next(child))
	{
	nbuf[0] = '\0';
	cg->conceptName(child, /*UP*/ nbuf);
	if (first == nbuf)
		return true;
	}
return false;
}

/********************************************
* FN:		JSON_HAS_ATTRIBUTES
* CR:		10/18/26 DD.
* SUBJ:	Would json_attributes emit anything for this concept?
* NOTE:	Mirrors JsonHasAttributes.  An array element writes its "id"
*			first and needs to know whether a comma follows it.
********************************************/

static bool json_has_attributes(CG *cg, CONCEPT *con)
{
_TCHAR abuf[MAXSTR];
for (ATTR *attrs = cg->findAttrs(con); attrs; cg->popAttr(attrs))
	{
	abuf[0] = '\0';
	cg->attrName(attrs, /*UP*/ abuf, MAXSTR);
	if (!is_count_con(cg, abuf, con))
		return true;
	}
return false;
}

/********************************************
* FN:		JSON_ATTRIBUTES
* CR:		08/04/26 DD.
* SUBJ:	Emit a concept's attributes.
* RET:	True if any attribute was emitted.
* NOTE:	Mirrors JsonAttributes.  Only ONE value per attribute is emitted:
*			the value loop in the NLP++ original is commented out.
********************************************/

static bool json_attributes(
	CG *cg,
	std::_t_ofstream &os,
	CONCEPT *con,
	long level
	)
{
ATTR *attrs = cg->findAttrs(con);
if (!attrs)
	return false;

bool first_attr = true;
bool has_attrs = false;
_TCHAR abuf[MAXSTR];
_TCHAR vbuf[MAXLINE];

while (attrs)
	{
	abuf[0] = '\0';
	cg->attrName(attrs, /*UP*/ abuf, MAXSTR);

	if (!is_count_con(cg, abuf, con))
		{
		has_attrs = true;
		VAL *vals = cg->attrVals(attrs);
		if (!first_attr)
			os << _T(",\n");

		os << json_indent(level + 1).c_str() << json_escape(abuf).c_str() << _T(": ");

		int type = cg->attrValType(con, abuf);
		if (type == 1)
			{
			long long num = 0;
			cg->popVal(vals, /*UP*/ num);
			_TCHAR nb[64];
			_stprintf(nb, _T("%lld"), num);
			os << json_escape(nb).c_str();
			}
		else if (type == 2)
			{
			CONCEPT *c = 0;
			cg->popVal(vals, /*UP*/ c);
			vbuf[0] = '\0';
			if (c)
				cg->conceptPath(c, /*UP*/ vbuf);
			os << json_escape(vbuf).c_str();
			}
		else if (type == 3)
			{
			float flt = 0;
			cg->popVal(vals, /*UP*/ flt);
			// str() on a float renders with %f, as Fn::fnStr does.
			_TCHAR fb[64];
			_stprintf(fb, _T("%f"), flt);
			os << json_escape(fb).c_str();
			}
		else
			{
			vbuf[0] = '\0';
			cg->popVal(vals, /*UP*/ vbuf);
			os << json_escape(vbuf).c_str();
			}

		first_attr = false;
		}

	cg->popAttr(attrs);
	}

return has_attrs;
}

/********************************************
* FN:		JSON_RECURSE
* CR:		08/04/26 DD.
* SUBJ:	Emit a concept's children.
* NOTE:	Mirrors JsonKBRecurse: a run of counted siblings with the same
*			base name (date1, date2, ...) is emitted as a JSON array under
*			that name, each element carrying an "id" holding the count.
*
*			The separator is written before each sibling, once it is known
*			what the sibling is.  Until 4.2.37 it was written after, which
*			put the comma inside an array that a plain sibling then closed
*			(with a stray extra brace and no comma after the "]"), so the
*			file was not JSON.  A counted sibling with a different base name
*			(item2 then thing1) was also added to the open array instead of
*			starting its own.  Every shape that was already valid JSON is
*			written byte for byte as before -- including the array that
*			follows a plain sibling opening on the same line as its "},".
********************************************/

static void json_recurse(
	CG *cg,
	std::_t_ofstream &os,
	CONCEPT *parent,
	long level
	)
{
CONCEPT *con = cg->Down(parent);
bool first = true;
bool in_array = false;
std::string array_base;
_TCHAR nbuf[MAXSTR];
std::string base, num;

while (con)
	{
	nbuf[0] = '\0';
	cg->conceptName(con, /*UP*/ nbuf);
	bool counted = separate_con_count(nbuf, /*UP*/ base, num);
	bool continues = counted && in_array && base == array_base;

	if (!first)
		{
		if (continues)
			os << _T(",\n");
		else if (in_array)
			{
			os << _T("\n") << json_indent(level + 1).c_str() << _T("],\n");
			in_array = false;
			}
		else if (counted)
			os << _T(",");
		else
			os << _T(",\n");
		}

	if (counted)
		{
		if (!continues)
			{
			os << json_indent(level + 1).c_str()
				<< json_escape(base.c_str()).c_str() << _T(": [\n");
			in_array = true;
			array_base = base;
			}

		os << json_indent(level + 2).c_str() << _T("{\n");
		os << json_indent(level + 3).c_str() << _T("\"id\": \"")
			<< num.c_str() << _T("\"");
		if (json_has_attributes(cg, con))
			{
			os << _T(",\n");
			json_attributes(cg, os, con, level + 2);
			}

		if (cg->Down(con))
			{
			os << _T(",\n");
			json_recurse(cg, os, con, level + 2);
			}
		else
			os << _T("\n") << json_indent(level + 2).c_str() << _T("}");
		}
	else
		{
		os << json_indent(level + 1).c_str()
			<< json_escape(nbuf).c_str() << _T(": {\n");
		bool hasattrs = json_attributes(cg, os, con, level + 1);

		if (cg->Down(con))
			{
			if (hasattrs)
				os << _T(",\n");
			json_recurse(cg, os, con, level + 1);
			}
		else
			os << _T("\n") << json_indent(level + 1).c_str() << _T("}");
		}

	con = cg->Next(con);
	first = false;
	}

if (in_array)
	os << _T("\n") << json_indent(level + 1).c_str() << _T("]");
os << _T("\n") << json_indent(level).c_str() << _T("}");
}

/********************************************
* FN:		JSON_WRITE_KB
* CR:		08/04/26 DD.
* SUBJ:	Write a concept tree to a file as JSON.
* RET:	True if the file was written.
* NOTE:	Mirrors JsonKB.  The top concept's own attributes are emitted
*			before its children, so a top concept can carry scalar fields
*			alongside nested objects.
*			The stream is opened in TEXT mode so the line endings match what
*			the NLP++ version produced through the engine's own streams.
********************************************/

bool json_write_kb(CG *cg, const _TCHAR *path, CONCEPT *con)
{
if (!cg || !path || !*path || !con)
	return false;

std::_t_ofstream os(TCHAR2CA(path));
if (!os)
	return false;

_TCHAR nbuf[MAXSTR];
nbuf[0] = '\0';
cg->conceptName(con, /*UP*/ nbuf);

// NOTE: the top concept's name is emitted raw between quotes, not through
// JsonStr.  Matching that, quirk and all.
os << _T("{\n") << json_indent(1).c_str() << _T("\"") << nbuf << _T("\": {\n");

bool hasattrs = json_attributes(cg, os, con, 1);
if (cg->Down(con) && hasattrs)
	os << _T(",\n");

json_recurse(cg, os, con, 1);

os << _T("\n}\n");
os.close();
return true;
}
