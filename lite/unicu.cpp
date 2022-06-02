#include "unicu.h"

using namespace unicu;

bool unicu::isAlphabetic(UChar32 c) {
	return u_isUAlphabetic(c) || isNepali(c);
}

bool unicu::isDigit(UChar32 c) {
	return u_isdigit(c);
}

bool unicu::isWhiteSpace(UChar32 c) {
	return u_isUWhiteSpace(c);
}

bool unicu::isPunct(UChar32 c) {
	return u_ispunct(c) || isPunct128(c);
}

bool unicu::isPunct128(UChar32 c) {
	return (
		(33 <= c && c <= 47) ||
		(60 <= c && c <= 62) ||
		(94 <= c && c <= 96) ||
		(124 <= c && c <= 127)
	);
}

bool unicu::isSingle(UChar32 c) {
	return isChinese(c) || isEmoji(c);
}

bool unicu::isChinese(UChar32 c) {
	return (
		(0x4E00 <= c && c <= 0x9FFF) ||
		(0x3400 <= c && c <= 0x4DBF) ||
		(0x20000 <= c && c <= 0x2A6DF) ||
		(0x2A700 <= c && c <= 0x2B73F) ||
		(0x2B740 <= c && c <= 0x2B81F) ||
		(0x2B820 <= c && c <= 0x2CEAF) ||
		(0xF900 <= c && c <= 0xFAFF) ||
		(0x2F800 <= c && c <= 0x2FA1F)
	);
}

bool unicu::isNepali(UChar32 c) {
	return (
		(0x0900 <= c && c <= 0x097F)
	);
}

bool unicu::isEmoji(UChar32 c) {
	return (
		(0x1F600 <= c && c <= 0xE007F)
	);
}

bool unicu::isUpper(UChar32 c) {
	return u_isupper(c);
}

bool unicu::isLower(UChar32 c) {
	return u_islower(c);
}

int unicu::strLen(const UChar *str) {
	return u_strlen(str);
}