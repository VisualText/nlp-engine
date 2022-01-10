#include "StdAfx.h"
#include "machine-min.h" // 10/25/06 AM.
#include "lite/global.h" // 01/24/01 AM.
#include "lite/unitest.h"
#include "unicode/usearch.h"
#include "unicode/schriter.h"
#include "unicode/locid.h"
#include "unicode/coll.h"
#include "unicode/stsearch.h"
#include "unicode/translit.h"
#include "unicode/normalizer2.h"

using icu::UnicodeString;

LITE_API void moose()
{
    UnicodeString str = "This is a test";
    // /* lowercase = "istanbul" */
    // UChar lowercase[] = {0x69, 0x73, 0x74, 0x61, 0x6e, 0x62, 0x75, 0x6c, 0};
    // /* uppercase = "LATIN CAPITAL I WITH DOT ABOVE STANBUL" */
    // UChar uppercase[] = {0x0130, 0x53, 0x54, 0x41, 0x4e, 0x42, 0x55, 0x4C, 0};

    // UnicodeString upper(uppercase);
    // UnicodeString lower(lowercase);

    std::string msg = "xxx";
    str.toUpper();
    str.toUTF8String(msg);
    std::cout << msg << std::endl;
}

// LITE_API void ulUnicodeToLowerCase(unsigned &tgtLen, UChar *&tgt, unsigned srcLen, UChar const *src)
// {
//     UnicodeString unicode(src, srcLen);
//     unicode.toLower();
//     tgtLen = unicode.length();
//     tgt = (UChar *)malloc(tgtLen * 2);
//     unicode.extract(0, tgtLen, tgt);
// }
