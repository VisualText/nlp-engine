#include "unicode/unistr.h"
#include "unicode/schriter.h"
#include "unicode/uchriter.h"
#include "unicode/utf8.h"
#include "unicode/uchar.h"
#include "unicode/ustring.h"
#include "unicode/coll.h"
#include "unicode/ucasemap.h"
#include "unicode/brkiter.h"
#include "unicode/locid.h"

using namespace icu;

namespace unicu
{
    bool isAlphabetic(UChar32 c);
    bool isDigit(UChar32 c);
    bool isWhiteSpace(UChar32 c);
    bool isPunct(UChar32 c);
    bool isPunct128(UChar32 c);
    bool isSingle(UChar32 c);
    bool isChinese(UChar32 c);
    bool isEmoji(UChar32 c);
    bool isUpper(UChar32 c);
    bool isLower(UChar32 c);
    int strLen(const UChar *s);
}

