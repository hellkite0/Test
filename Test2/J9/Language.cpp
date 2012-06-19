// $Id: Language.cpp 8979 2011-07-15 03:14:11Z hyojin.lee $

#include "Stdafx.h"
#include "Language.h"

#include "XML.h"
#include "UtilityString.h"
#include "UtilityFile.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// Language
////////////////////////////////////////////////////////////////////////////////
Language::Language()
: iLanguage_(eLKO),
bOverwrite_(false)
{
}

const WChar*
Language::EnumToString(Language::Languages iLanguage)
{
	switch (iLanguage)
	{
	case Language::eLKO:
		return _W("ko");
	case Language::eLEN:
		return _W("en");
	case Language::eLJP:
		return _W("jp");
	case Language::eLDE:
		return _W("de");
	case Language::eLES:
		return _W("es");
	case Language::eLZH_TW:
		return _W("zh-tw");
	case Language::eLKO_Kor:
		return _W("ko_kor");
	default:
		ASSERT(0);
		return NULL;
	};
}

Language::Languages
Language::StringToEnum(const WChar* pLanguage)
{
	for (U32 i = 0; i < Language::eLNum; ++i)
	{
		if (::wcscmp(EnumToString(_sc<Language::Languages>(i)), pLanguage) == 0)
			return _sc<Language::Languages>(i);
	}

	ASSERT(0);
	return _sc<Language::Languages>(-1);
}

void
Language::SetLanguage(const WChar* pLanguage)
{
	SetLanguage(StringToEnum(pLanguage));
}

void
Language::Load(const WChar* pFileName)
{
	XMLNode	cXML;
	cXML.Load(pFileName);

	const WChar*	pLanguage = EnumToString(GetLanguage());

	if (!pLanguage || wcslen(pLanguage) == 0)
	{
		CHECK(0, eCheckAlways, _W("Can't find correct language key"));
		return;
	}

	Size	iStringNum = 0;
	XMLNodeList	cStrings = cXML.GetChildList(_W("string"), &iStringNum);
	for (Size iString = 0; iString < iStringNum; ++iString)
	{
		XMLNode	cString = cStrings.Get(iString);
		XMLNode	cKey = cString.GetChild(_W("key"));
		XMLNode	cType = cString.GetChild(pLanguage);
		if (cType.IsNull())
			cType = cString.GetChild(_W("value"));

		AddString(cKey.GetData(), cType.IsNull() ? cKey.GetData() : cType.GetData());
	}
}

void
Language::LoadDirectory(StringW& pFilePath)
{
	J9::FileUtil::FileNameVectorW cNames;
	J9::FileUtil::FindFilePackage((pFilePath + _W("*.xml")).c_str(), cNames, false);
	Size iNameSize = cNames.size();

	for (Size iNameIndex = 0; iNameIndex < iNameSize; ++iNameIndex)
	{
		XMLNode	cXML;
		cXML.Load((pFilePath + cNames[iNameIndex]).c_str());
		
		const WChar*	pLanguage = EnumToString(GetLanguage());

		if (!pLanguage || wcslen(pLanguage) == 0)
		{
			CHECK(0, eCheckAlways, _W("Can't find correct language key"));
			return;
		}

		Size	iStringNum = 0;
		XMLNodeList	cStrings = cXML.GetChildList(_W("string"), &iStringNum);
		for (Size iString = 0; iString < iStringNum; ++iString)
		{
			XMLNode	cString = cStrings.Get(iString);
			XMLNode	cKey = cString.GetChild(_W("key"));
			XMLNode	cType = cString.GetChild(pLanguage);
			if (cType.IsNull())
				cType = cString.GetChild(_W("value"));

			AddString(cKey.GetData(), cType.IsNull() ? cKey.GetData() : cType.GetData());
		}
	}
}

void
Language::AddString(const StringW& rKey, const StringW& rString)
{
	Strings::iterator	cIterExist = cStrings_.find(rKey);
	if (!bOverwrite_ && cIterExist != cStrings_.end())
	{
		CHECK(0, eCheckAlways, _W("String already exists. Can't overwrite.\nKey : %s, String : %s\nSelect Ignore to continue."), rKey.c_str(), rString.c_str());
		return;
	}

	StringW	sString = rString;
	StringUtil::ReplaceLineBreak(sString);

	if (cIterExist == cStrings_.end())
		// 없던 항목이니까 새로 만들어 넣자.
        cStrings_[rKey] = sString;
	else
		// 이미 있는 항목이면 값만 바꾸자.
		cIterExist->second = sString;
}

void
Language::GetLocalString(StringW& rString, PathKeyW rKey) const
{
	Strings::const_iterator	cIter = cStrings_.find(rKey);

	if (cIter == cStrings_.end())
		rString = rKey.c_str();
	else
        rString = cIter->second;
}

void
Language::GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1) const
{
	GetLocalString(rString, rKey);

	static StringW	sSub = _W("[%1]");
	static Size		iSubLen = sSub.length();

	Size	iPos = rString.find(sSub, 0);

	if (iPos == StringW::npos)	return;

	rString.replace(iPos, iSubLen, pArg1);
}

void
Language::GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2) const
{
	GetLocalString(rString, rKey, pArg1);

	static StringW	sSub = _W("[%2]");
	static Size		iSubLen = sSub.length();

	Size	iPos = rString.find(sSub, 0);

	if (iPos == StringW::npos)	return;

	rString.replace(iPos, iSubLen, pArg2);
}

void
Language::GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3) const
{
	GetLocalString(rString, rKey, pArg1, pArg2);

	static StringW	sSub = _W("[%3]");
	static Size		iSubLen = sSub.length();

	Size	iPos = rString.find(sSub, 0);

	if (iPos == StringW::npos)	return;

	rString.replace(iPos, iSubLen, pArg3);
}

void
Language::GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4) const
{
	GetLocalString(rString, rKey, pArg1, pArg2, pArg3);

	static StringW	sSub = _W("[%4]");
	static Size		iSubLen = sSub.length();

	Size	iPos = rString.find(sSub, 0);

	if (iPos == StringW::npos)	return;

	rString.replace(iPos, iSubLen, pArg4);
}

void
Language::GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4, const WChar* pArg5) const
{
	GetLocalString(rString, rKey, pArg1, pArg2, pArg3, pArg4);

	static StringW	sSub = _W("[%5]");
	static Size		iSubLen = sSub.length();

	Size	iPos = rString.find(sSub, 0);

	if (iPos == StringW::npos)	return;

	rString.replace(iPos, iSubLen, pArg5);
}

const StringW&
Language::GetLocalString(PathKeyW rKey) const
{
	Strings::const_iterator	cIter = cStrings_.find(rKey);

	if (cIter == cStrings_.end())	return rKey.GetString();

	return cIter->second;
}

//StringW
//Language::GetLocalStringF(PathKeyW rKey, ...) const
//{
//	StringW s;
//	GetLocalString(s, rKey);
//
// //sSub가 어떤 스트링으로 초기화 되는지 확인할 필요 있음
//	Size iPos, i = 1;
//	StringW	sSub(_W("[%d]"), i);
//
//	va_list cArgList;
//	va_start(cArgList, s);
//	while (iPos = s.find(sSub, 0), iPos != StringW::npos)
//	{
//		Size	iSubLen = sSub.length();
//		s.replace(iPos, iSubLen, va_arg(cArgList, const WChar*));
//		sSub = StringUtil::Format(_W("[%d]"), ++i);
//	}
//	va_end(cArgList);
//
//	return s;
//}

StringW
Language::GetLocalString(PathKeyW rKey, const WChar* pArg1) const
{
	StringW	s;
	GetLocalString(s, rKey, pArg1);
	return s;
}

StringW
Language::GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2) const
{
	StringW	s;
	GetLocalString(s, rKey, pArg1, pArg2);
	return s;
}

StringW
Language::GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3) const
{
	StringW	s;
	GetLocalString(s, rKey, pArg1, pArg2, pArg3);
	return s;
}

StringW
Language::GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4) const
{
	StringW	s;
	GetLocalString(s, rKey, pArg1, pArg2, pArg3, pArg4);
	return s;
}

StringW
Language::GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4, const WChar* pArg5) const
{
	StringW	s;
	GetLocalString(s, rKey, pArg1, pArg2, pArg3, pArg4, pArg5);
	return s;
}
