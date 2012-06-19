// $Id: UtilityString.cpp 8577 2011-05-27 09:56:13Z hyojin.lee $

#include "stdafx.h"
#include "UtilityString.h"

#include "Application.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// StringUtil

static const WChar	iChoSungs[19] = {
	_W('ㄱ'), _W('ㄲ'), _W('ㄴ'), _W('ㄷ'), _W('ㄸ'),
	_W('ㄹ'), _W('ㅁ'), _W('ㅂ'), _W('ㅃ'), _W('ㅅ'),
	_W('ㅆ'), _W('ㅇ'), _W('ㅈ'), _W('ㅉ'), _W('ㅊ'),
	_W('ㅋ'), _W('ㅌ'), _W('ㅍ'), _W('ㅎ')
};

static const WChar	iJungSungs[21] = {
	_W('ㅏ'), _W('ㅐ'), _W('ㅑ'), _W('ㅒ'), _W('ㅓ'),
	_W('ㅔ'), _W('ㅕ'), _W('ㅖ'), _W('ㅗ'), _W('ㅘ'),
	_W('ㅙ'), _W('ㅚ'), _W('ㅛ'), _W('ㅜ'), _W('ㅝ'),
	_W('ㅞ'), _W('ㅟ'), _W('ㅠ'), _W('ㅡ'), _W('ㅢ'),
	_W('ㅣ')
};

static const WChar	iJongSungs[28] = {
	_W(''), _W('ㄱ'), _W('ㄲ'), _W('ㄳ'), _W('ㄴ'),
	_W('ㄵ'), _W('ㄶ'), _W('ㄷ'), _W('ㄹ'), _W('ㄺ'),
	_W('ㄻ'), _W('ㄼ'), _W('ㄽ'), _W('ㄾ'), _W('ㄿ'),
	_W('ㅀ'), _W('ㅁ'), _W('ㅂ'), _W('ㅄ'), _W('ㅅ'),
	_W('ㅆ'), _W('ㅇ'), _W('ㅈ'), _W('ㅊ'), _W('ㅋ'),
	_W('ㅌ'), _W('ㅍ'), _W('ㅎ')
};

static const WChar	iUnicodeHangeulStart = 0xac00;
static const int	iNumChosungs = countof(iChoSungs);
static const int	iNumJungsungs = countof(iJungSungs);
static const int	iNumJongsungs = countof(iJongSungs);
static const int	iNumHangeuls = iNumChosungs * iNumJungsungs * iNumJongsungs;

bool
J9::StringUtil::IsHangeul(WChar iChar)
{
	return iUnicodeHangeulStart <= iChar && iChar < (iUnicodeHangeulStart + iNumHangeuls);
}

void
J9::StringUtil::HangeulDivide(WChar iHangeul, WChar& rCho, WChar& rJung, WChar& rJong)
{
	if (!IsHangeul(iHangeul))
	{
		rCho = 0;
		rJung = 0;
		rJong = 0;
		return;
	}

	int	iHangeulIndex = iHangeul - iUnicodeHangeulStart;
	int	iJong = iHangeulIndex % iNumJongsungs;
	int	iJung = ((iHangeulIndex - iJong) / iNumJongsungs) % iNumJungsungs;
	int	iCho = ((iHangeulIndex - iJong) / iNumJongsungs ) / iNumJungsungs;

	ASSERT(iCho < iNumChosungs && iJung < iNumJungsungs && iJong < iNumJongsungs);

	rCho = iChoSungs[iCho];
	rJung = iJungSungs[iJung];
	rJong = iJongSungs[iJong];
}

WChar
J9::StringUtil::HangeulMerge(U32 iCho, U32 iJung, U32 iJong)
{
	if (iNumChosungs <= iCho
		|| iNumJungsungs <= iJung
		|| iNumJongsungs <= iJong)
		return _W('');

	return _sc<WChar>(iCho * (iNumJungsungs * iNumJongsungs) + iJung * iNumJongsungs + iJong);
}

WChar
J9::StringUtil::GetJongsung(WChar iChar)
{
	if (0 <= iChar && iChar <= 0xff)
	{
		if (::iswdigit(iChar))
		{
			switch(iChar)
			{
			case _W('0'):
				return _W('ㅇ');
			case _W('1'):
				return _W('ㄹ');
			case _W('2'):
				return _W('');
			case _W('3'):
				return _W('ㅁ');
			case _W('4'):
				return _W('');
			case _W('5'):
				return _W('');
			case _W('6'):
				return _W('ㄱ');
			case _W('7'):
				return _W('ㄹ');
			case _W('8'):
				return _W('ㄹ');
			case _W('9'):
				return _W('');
			default:
				return _W('');
			}
		}
		else if (::iswalpha(iChar))
		{
			WChar	iLower = ::towlower(iChar);
			switch(iLower)
			{
			case _W('a'):
				return _W('');
			case _W('b'):
				return _W('ㅂ');
			case _W('c'):
				return _W('ㅋ');
			case _W('d'):
				return _W('');
			case _W('e'):
				return _W('');
			case _W('f'):
				return _W('ㅍ');
			case _W('g'):
				return _W('ㅇ');
			case _W('h'):
				return _W('');
			case _W('i'):
				return _W('');
			case _W('j'):
				return _W('ㅈ');
			case _W('k'):
				return _W('ㅋ');
			case _W('l'):
				return _W('ㄹ');
			case _W('m'):
				return _W('ㅁ');
			case _W('n'):
				return _W('ㄴ');
			case _W('o'):
				return _W('');
			case _W('p'):
				return _W('ㅍ');
			case _W('q'):
				return _W('ㅋ');
			case _W('r'):
				return _W('ㄹ');
			case _W('s'):
				return _W('');
			case _W('t'):
				return _W('ㅌ');
			case _W('u'):
				return _W('');
			case _W('v'):
				return _W('ㅂ');
			case _W('w'):
				return _W('');
			case _W('x'):
				return _W('');
			case _W('y'):
				return _W('');
			case _W('z'):
				return _W('');
			default:
				return _W('');
			}
		}
		else
			return _W('');
	}
	else
	{
		WChar	iCho, iJung, iJong;
		HangeulDivide(iChar, iCho, iJung, iJong);
		return iJong;
	}
}

WChar
J9::StringUtil::GetJongsung(const WChar* pString, int iLength)
{
	if (iLength == -1)
		iLength = ::wcslen(pString);
	if (iLength == 0)
		return _W('');

	return GetJongsung(pString[iLength - 1]);
}

bool
J9::StringUtil::HasJongsung(WChar iChar)
{
	if (::iswdigit(iChar))
	{
		switch(iChar)
		{
		case _W('0'):
		case _W('1'):
		case _W('3'):
		case _W('6'):
		case _W('7'):
		case _W('8'):
			return true;

		case _W('2'):
		case _W('4'):
		case _W('5'):
		case _W('9'):
		default:
			return false;
		}
	}
	else if (::iswalpha(iChar))
	{
		WChar	iLower = ::towlower(iChar);
		switch(iLower)
		{
		case _W('b'):
		case _W('c'):
		case _W('f'):
		case _W('g'):
		case _W('j'):
		case _W('k'):
		case _W('l'):
		case _W('m'):
		case _W('n'):
		case _W('p'):
		case _W('q'):
		case _W('r'):
		case _W('t'):
		case _W('v'):
			return true;

		case _W('a'):
		case _W('d'):
		case _W('e'):
		case _W('h'):
		case _W('i'):
		case _W('o'):
		case _W('s'):
		case _W('u'):
		case _W('w'):
		case _W('y'):
		case _W('x'):
		case _W('z'):
		default:
			return false;
		}
	}
	else
	{
		WChar	iCho, iJung, iJong;
		HangeulDivide(iChar, iCho, iJung, iJong);
		return iJong != _W('');
	}
}

bool
J9::StringUtil::HasJongsung(const WChar* pString, int iLength)
{
	if (iLength == -1)
		iLength = ::wcslen(pString);
	if (iLength == 0)
		return false;

	return HasJongsung(pString[iLength - 1]);
}

const WChar*
J9::StringUtil::SelectWithJongsung(const WChar* pWith, const WChar* pWithout, const WChar* pString, int iLength)
{
	ASSERT(pWith && pWithout && 0 < ::wcslen(pWith) && 0 < ::wcslen(pWithout));

	WChar	iJongsung = GetJongsung(pString, iLength);
	if (iJongsung == _W(''))
		return pWithout;
	else if (iJongsung == _W('ㄹ') && pWith[0] == _W('으'))
		return pWithout;
	return pWith;
}

StringA
StringUtil::ToStringA(const StringW& r)
{
	return ToStringA(r.c_str(), r.length());
}


J9::StringA
J9::StringUtil::ToStringA(const WChar* pString, int iLength)
{
	if (iLength == 0)
		return StringA();

	if (iLength == -1)
		iLength = ::wcslen(pString);

	int	iNeed = ::WideCharToMultiByte(Application::GetCodePage(), 0, pString, iLength, NULL, 0, NULL, NULL);

	if (iNeed == 0)
	{
		return StringA();
	}
	else
	{
		ASSERT(iNeed < LARGE_NUMBER);
		AChar	pBuffer[LARGE_NUMBER];
		::WideCharToMultiByte(Application::GetCodePage(), 0, pString, iLength, pBuffer, iNeed, NULL, NULL);
		return StringA(pBuffer, pBuffer + iNeed);
	}
}

J9::StringW
J9::StringUtil::SelectWithJongsung(const J9::StringW& r)
{
	struct ReplacePair 
	{
		const WChar* pString_;
		const WChar* pWith_;
		const WChar* pWithout_;
	};

	static const ReplacePair	cPairs[] = {
		{ _W("을(를)"), _W("을"), _W("를") },	// 언젠가는 없애버려야할 임시 태그
		{ _W("(을)를"), _W("을"), _W("를") },	// 언젠가는 없애버려야할 임시 태그
		{ _W("(으)로"), _W("으로"), _W("로") },	// 언젠가는 없애버려야할 임시 태그
		{ _W("{는}"), _W("은"), _W("는") },
		{ _W("{가}"), _W("이"), _W("가") },
		{ _W("{를}"), _W("을"), _W("를") },
		{ _W("{로}"), _W("으로"), _W("로") },
	};

	J9::StringW	sRet = r;
	bool	bReplaced = true;// 첫시도는 해야하니까 true로 초기화
	while(bReplaced)
	{
		bReplaced = false;
		for (int iPair = 0; iPair < countof(cPairs); ++iPair)
		{
			Size	iFound = sRet.find(cPairs[iPair].pString_);
			if (iFound != J9::StringW::npos)
			{
				Size	iEnd = iFound + ::wcslen(cPairs[iPair].pString_);
				J9::StringW	sRemain = sRet.substr(iEnd);
				sRet = sRet.substr(0, iFound);
				const WChar*	pSelected = SelectWithJongsung(cPairs[iPair].pWith_, cPairs[iPair].pWithout_, sRet.c_str(), sRet.length());
				sRet += pSelected;
				sRet += sRemain;
				bReplaced = true;
			}
		}
	}

	return sRet;
}

StringA
StringUtil::ToStringUTF8(const StringW& r)
{
	return ToStringUTF8(r.c_str(), r.length());
}

StringA
StringUtil::ToStringUTF8(const WChar* pString, int iLength)
{
	if (iLength == 0)
		return StringA();

	if (iLength == -1)
		iLength = ::wcslen(pString);

	
	int	iNeed = ::WideCharToMultiByte(CP_UTF8, 0, pString, iLength, NULL, 0, NULL, NULL);

	if (iNeed == 0)
	{
		return StringA();
	}
	
	ASSERT(iNeed < LARGE_NUMBER);
	AChar	pBuffer[LARGE_NUMBER];
	::WideCharToMultiByte(CP_UTF8, 0, pString, iLength, pBuffer, iNeed, NULL, NULL);
	return StringA(pBuffer, pBuffer + iNeed);
}

StringW
StringUtil::FromStringUTF8(const StringA& r)
{
	return FromStringUTF8(r.c_str(), r.length());
}

StringW
StringUtil::FromStringUTF8(const AChar* pString, int iLength)
{
	if (iLength == 0)
		return StringW();

	if (iLength == -1)
		iLength = ::strlen(pString);
	int	iNeed = ::MultiByteToWideChar(CP_UTF8, 0, pString, iLength, NULL, 0);

	if (iNeed == 0)
	{
		return StringW();
	}
	else
	{
		ASSERT(iNeed < LARGE_NUMBER);
		WChar	pBuffer[LARGE_NUMBER];
		::MultiByteToWideChar(CP_UTF8, 0, pString, iLength, pBuffer, iNeed);
		return StringW(pBuffer, pBuffer + iNeed);
	}
}

StringW
StringUtil::ToStringW(const StringA& r)
{
	return ToStringW(r.c_str(), r.length());
}

StringW
StringUtil::ToStringW(const AChar* pString, int iLength)
{
	if (iLength == 0)
		return StringW();

	if (iLength == -1)
		iLength = ::strlen(pString);
	int	iNeed = ::MultiByteToWideChar(Application::GetCodePage(), MB_PRECOMPOSED, pString, iLength, NULL, 0);

	if (iNeed == 0)
	{
		return StringW();
	}
	else
	{
		ASSERT(iNeed < LARGE_NUMBER);
		WChar	pBuffer[LARGE_NUMBER];
		::MultiByteToWideChar(Application::GetCodePage(), MB_PRECOMPOSED, pString, iLength, pBuffer, iNeed);
		return StringW(pBuffer, pBuffer + iNeed);
	}
}

////////////////////////////////////////////////////////////////////////////////
// StringUtil::ConvertFromHex(StringA)
////////////////////////////////////////////////////////////////////////////////
template<>
void
StringUtil::ConvertFromHex(const StringA& rString, bool& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, char& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, S8& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, U8& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, S16& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, U16& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, S32& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, U32& rData)
{
	U32	iRet;
	U32	iValue;

	iRet = ::sscanf_s(rString.c_str(), "%x", &iValue);
	if (iRet != 1)
		rData = 0;
	else
		rData = iValue;
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, F32& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, S64& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, U64& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringA& rString, F64& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

////////////////////////////////////////////////////////////////////////////////
// StringUtil::ConvertFromHex(StringW)
////////////////////////////////////////////////////////////////////////////////
template<>
void
StringUtil::ConvertFromHex(const StringW& rString, bool& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, char& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, S8& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, U8& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, S16& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, U16& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, S32& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, U32& rData)
{
	U32	iRet;
	U32	iValue;

	iRet = ::swscanf_s(rString.c_str(), _W("%x"), &iValue);
	if (iRet != 1)
		rData = 0;
	else
		rData = iValue;
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, F32& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, S64& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, U64& rData)
{
	U32	iRet;
	U64	iValue;

	iRet = ::swscanf_s(rString.c_str(), _W("%llx"), &iValue);
	if (iRet != 1)
		rData = 0;
	else
		rData = iValue;
}

template<>
void
StringUtil::ConvertFromHex(const StringW& rString, F64& rData)
{
	UNUSED(rString);	UNUSED(rData);	ASSERT(0);
}

void
StringUtil::ReplaceLineBreak(StringW& rString)
{
	StringUtil::ReplaceAll<WChar>(rString, _W("\\n"), _W("\n"));
}

U32
StringUtil::GetIndex(const StringW& rSrc, const StringW& rPrefix)
{
	if (rSrc.compare(0, rPrefix.length(), rPrefix, 0, rPrefix.length()) != 0)
        return U32_MAX;

	StringW	sNum(rSrc, rPrefix.length());
	U32 i = 0;
	StringUtil::Convert(sNum, i);

	return i;
}

//////////////////////////////////////////////////////////////////////////////////
// : 와일드카드 스트링 비교
// : check wild card string
//   this is an implementation given by poohbear_68 (in www.experts-exchange.com)
//   http://www.experts-exchange.com/Programming/Programming_Languages/Cplusplus/Q_10172266.html
//////////////////////////////////////////////////////////////////////////////////
__inline bool
CompareCharacter(WChar lhs, WChar rhs, bool bCaseSensitive)
{
	return bCaseSensitive
		? (lhs == rhs) : (::tolower(lhs) == ::tolower(rhs));
}

bool
StringUtil::CompareWildcardedString(const WChar* pWildcard, const WChar* pStringToCompare, bool bCaseSensitive)
{
	S32		iWildcardPos = 0, iFilenameCnt = 0;
	S32		iNextWildcardPos, iCnt;
	bool	bFound;

	while (pWildcard[iWildcardPos] && pStringToCompare[iFilenameCnt])
	{
		// process a wild card
		if (pWildcard[iWildcardPos] == _W('*'))
		{
			// check to see if another wildcard exists
			for (iNextWildcardPos = iWildcardPos + 1; pWildcard[iNextWildcardPos]; ++iNextWildcardPos)
			{
				if (pWildcard[iNextWildcardPos] == _W('*'))
				{
					break;
				}
			}

			if (!pWildcard[iNextWildcardPos])
			{
				// arrived at our last wildcard;
				// check to see if ending text matches
				const WChar*	pWildcardEndingStr = pWildcard + iWildcardPos + 1;
				S32			iMatchLen = (S32) ::wcslen(pWildcardEndingStr);

				// errata : if(::wcslen(pWildcardEndingStr) > iMatchLen)
				if ((S32) ::wcslen(pStringToCompare) - iFilenameCnt - iMatchLen < 0)
				{
					return false;
				}

				const WChar*	szFileNameEndingStr;
				szFileNameEndingStr = pStringToCompare + ::wcslen(pStringToCompare) - iMatchLen;

				for (iCnt = 0; iCnt < iMatchLen; ++iCnt)
				{
					if (pWildcardEndingStr[iCnt] != szFileNameEndingStr[iCnt] && pWildcardEndingStr[iCnt] != _W('?'))
					{
						return false;
					}
				}

				return true;
			}
			// skip over double asterisks
			else if (iNextWildcardPos == iWildcardPos + 1)
			{
				++iWildcardPos;
			}
			else
			{
				// another wildcard exists; find earliest occurrence of
				// the text contained between the current wildcard (iWildcardPos)
				// and the next wildcard (iNextWildcardPos)
				bFound = 0;
				while (!bFound)
				{
					for (iCnt = 0; iCnt < iNextWildcardPos - iWildcardPos - 1; ++iCnt)
					{
						if (pStringToCompare[iFilenameCnt + iCnt] == NULL)
						{
							// file name is too short
							return false;
						}

						if (pWildcard[iWildcardPos + iCnt + 1] == _W('?')
							|| CompareCharacter(pStringToCompare[iFilenameCnt + iCnt], pWildcard[iWildcardPos + iCnt + 1], bCaseSensitive))
						{
							bFound = true;
						}
						else
						{
							bFound = false;
							break;
						}

					}

					// If string not found, continuing searching, this time starting
					// one character to the right.
					if (!bFound)
					{
						++iFilenameCnt;
					}
				}

				iFilenameCnt += iNextWildcardPos - iWildcardPos - 1;
				iWildcardPos = iNextWildcardPos;
			}
		}
		else if (pWildcard[iWildcardPos] == _W('?')
			|| CompareCharacter(pWildcard[iWildcardPos], pStringToCompare[iFilenameCnt], bCaseSensitive))
		{
			++iWildcardPos;
			++iFilenameCnt;
		}
		else
		{
			return false;
		}
	}

	while(pStringToCompare[iFilenameCnt] == NULL && pWildcard[iWildcardPos] == _T('*'))
	{
		++iWildcardPos;
	}

	// return true if ends of both strings have been reached
	return pStringToCompare[iFilenameCnt] == NULL && pWildcard[iWildcardPos] == NULL;
}

bool
StringUtil::CompareWildcardedString(const AChar* pWildcard, const AChar* pStringToCompare, bool bCaseSensitive)
{
	S32		iWildcardPos = 0, iFilenameCnt = 0;
	S32		iNextWildcardPos, iCnt;
	bool	bFound;

	while (pWildcard[iWildcardPos] && pStringToCompare[iFilenameCnt])
	{
		// process a wild card
		if (pWildcard[iWildcardPos] == _W('*'))
		{
			// check to see if another wildcard exists
			for (iNextWildcardPos = iWildcardPos + 1; pWildcard[iNextWildcardPos]; ++iNextWildcardPos)
			{
				if (pWildcard[iNextWildcardPos] == _W('*'))
				{
					break;
				}
			}

			if (!pWildcard[iNextWildcardPos])
			{
				// arrived at our last wildcard;
				// check to see if ending text matches
				const AChar*	pWildcardEndingStr = pWildcard + iWildcardPos + 1;
				S32			iMatchLen = (S32) ::strlen(pWildcardEndingStr);

				// errata : if(::wcslen(pWildcardEndingStr) > iMatchLen)
				if ((S32) ::strlen(pStringToCompare) - iFilenameCnt - iMatchLen < 0)
				{
					return false;
				}

				const AChar*	szFileNameEndingStr;
				szFileNameEndingStr = pStringToCompare + ::strlen(pStringToCompare) - iMatchLen;

				for (iCnt = 0; iCnt < iMatchLen; ++iCnt)
				{
					if (pWildcardEndingStr[iCnt] != szFileNameEndingStr[iCnt] && pWildcardEndingStr[iCnt] != _W('?'))
					{
						return false;
					}
				}

				return true;
			}
			// skip over double asterisks
			else if (iNextWildcardPos == iWildcardPos + 1)
			{
				++iWildcardPos;
			}
			else
			{
				// another wildcard exists; find earliest occurrence of
				// the text contained between the current wildcard (iWildcardPos)
				// and the next wildcard (iNextWildcardPos)
				bFound = 0;
				while (!bFound)
				{
					for (iCnt = 0; iCnt < iNextWildcardPos - iWildcardPos - 1; ++iCnt)
					{
						if (pStringToCompare[iFilenameCnt + iCnt] == NULL)
						{
							// file name is too short
							return false;
						}

						if (pWildcard[iWildcardPos + iCnt + 1] == _W('?')
							|| CompareCharacter(pStringToCompare[iFilenameCnt + iCnt], pWildcard[iWildcardPos + iCnt + 1], bCaseSensitive))
						{
							bFound = true;
						}
						else
						{
							bFound = false;
							break;
						}

					}

					// If string not found, continuing searching, this time starting
					// one character to the right.
					if (!bFound)
					{
						++iFilenameCnt;
					}
				}

				iFilenameCnt += iNextWildcardPos - iWildcardPos - 1;
				iWildcardPos = iNextWildcardPos;
			}
		}
		else if (pWildcard[iWildcardPos] == _W('?')
			|| CompareCharacter(pWildcard[iWildcardPos], pStringToCompare[iFilenameCnt], bCaseSensitive))
		{
			++iWildcardPos;
			++iFilenameCnt;
		}
		else
		{
			return false;
		}
	}

	while(pStringToCompare[iFilenameCnt] == NULL && pWildcard[iWildcardPos] == _T('*'))
	{
		++iWildcardPos;
	}

	// return true if ends of both strings have been reached
	return pStringToCompare[iFilenameCnt] == NULL && pWildcard[iWildcardPos] == NULL;
}

J9::StringW
StringUtil::PushNumberComma(S32 iNumber)
{
	if (iNumber == 0) return _W("0");
	StringW sStr = Format(_W("%.0ld"), iNumber);
	StringW sResult;
	U8 iNegativeNumber = 0 <= iNumber ? 0: 1;
	U8 iSize = 0;
	U8 iCount = 0;
	U8 iMod = 0;
	U8 iStartNum;
	if (iNegativeNumber)
		iSize = _sc<U8>(sStr.size() - 1);
	else
		iSize = _sc<U8>(sStr.size());
	iCount = _sc<U8>((iSize) / 3);
	iMod = iSize % 3;
	if (iMod == 0)
	{
		sResult = sStr.substr(0, iNegativeNumber + 3);
		iStartNum = 3 +iNegativeNumber;
		--iCount;
	}
	else
	{
		sResult = sStr.substr(0, iMod + iNegativeNumber);
		iStartNum = iMod +iNegativeNumber;
	}

	
	for (U8 i = 0; i < iCount; ++i)
	{
		sResult += _W(",") + sStr.substr(iStartNum, 3);
		iStartNum += 3;
	}
	return sResult;
}
// StringUtil
////////////////////////////////////////////////////////////////////////////////
