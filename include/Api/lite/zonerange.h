/*******************************************************************************
Copyright (c) 2026 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ZONERANGE.H
* FILE:	include/Api/lite/zonerange.h
* SUBJ:	Read a -ZONES spec: which zones of a zone-mode run to run.
* NOTE:	Used by Parse, which applies it, and NLP_ENGINE, which checks it.
*
*******************************************************************************/

#ifndef ZONERANGE_H_
#define ZONERANGE_H_

#include <vector>
#include <utility>

// "7", "40-45" or a list such as "3,40-45" into [first, last] pairs. Zone
// numbers count from 1, as in the z040-ana###.tree names. False, with
// ranges cleared, if the spec is not that form.
template <typename CH>
inline bool parseZoneRange(const CH *spec, std::vector<std::pair<long,long>> &ranges)
{
ranges.clear();
if (!spec || !*spec)
	return false;
const CH *p = spec;
auto digit = [](CH c) { return c >= '0' && c <= '9'; };
while (digit(*p))
	{
	long lo = 0;
	while (digit(*p))
		lo = lo * 10 + (*p++ - '0');
	long hi = lo;
	if (*p == '-')
		{
		++p;
		if (!digit(*p))
			break;
		hi = 0;
		while (digit(*p))
			hi = hi * 10 + (*p++ - '0');
		}
	if (lo < 1 || hi < lo)
		break;
	ranges.push_back(std::make_pair(lo, hi));
	if (!*p)
		return true;
	if (*p != ',')
		break;
	++p;
	}
ranges.clear();
return false;
}

#endif
