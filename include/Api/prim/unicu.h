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

#ifdef LINUX
#include <my_tchar.h>
#else
#include <tchar.h>
#endif

using namespace icu;

namespace unicu
{
    bool isAlphabetic(UChar32 c);
    bool isDigit(UChar32 c);
    bool isWhiteSpace(UChar32 c);
    bool isPunct(UChar32 c);
    bool isPunct128(UChar32 c);
    bool isSingle(UChar32 c);
    bool isGlyph(UChar32 c);
    bool isChinese(UChar32 c);
    bool isNepali(UChar32 c);
    bool isEmoji(UChar32 c);
    bool isEmojiJoiner(UChar32 c);
    bool isEmojiVariation(UChar32 c);
    bool isTamilGlyph(UChar32 c);
    bool isUpper(UChar32 c);
    bool isLower(UChar32 c);
    bool isStrLower(_TCHAR *str);
    bool isStrWhiteSpace(_TCHAR *str);
    bool isStrAlphabetic(_TCHAR *str);
    int strLen(const UChar *str);
    int strCmp(_TCHAR *str1, _TCHAR *str2);
    UChar *contains(_TCHAR *str1, _TCHAR *str2, bool noCase);
    bool isCaps(const UChar *str);
    bool isNumeric(_TCHAR *str);
    bool strToLong(_TCHAR *str, long long &num);
    int charToDigit(UChar ch);
}

