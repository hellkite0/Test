// $Id: Option.cpp 321 2008-03-10 02:11:58Z hyojin.lee $

#include "Stdafx.h"
#include "Any.h"
#include "Option.h"

#include "XML.h"
#include "UtilityString.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// Option
////////////////////////////////////////////////////////////////////////////////
Option::Option()
: bOverwrite_(false)
{
}

void
Option::Load(const WChar* pFileName)
{
	XMLNode	cXML;
	cXML.Load(pFileName);

	Size	iOptionNum = 0;
	XMLNodeList	cOptions = cXML.GetChildList(_W("option"), &iOptionNum);
	for (Size iOption = 0; iOption < iOptionNum; ++iOption)
	{
		XMLNode	cOption = cOptions.Get(iOption);
		XMLNode	cName = cOption.GetChild(_W("name"));
		XMLNode	cType = cOption.GetChild(_W("type"));
		XMLNode	cValue = cOption.GetChild(_W("value"));

		AddElem(cName.GetData(), cType.GetData(), cValue.GetData());
	}
}

const Any&
Option::Get(const StringW& rName) const
{
	OptionMap::const_iterator	iter = cOptionMap_.find(rName);
	ASSERT(iter != cOptionMap_.end());
	return iter->second;
}

void
Option::AddCacheElem(const StringW& rName, bool* p)
{
	CacheElem	cElem;
	cElem.iType = eOTBool;
	cElem.pElem = p;

	cCacheMap_[rName] = cElem;
}

void
Option::AddCacheElem(const StringW& rName, S32* p)
{
	CacheElem	cElem;
	cElem.iType = eOTInteger;
	cElem.pElem = p;

	cCacheMap_[rName] = cElem;
}

void
Option::AddCacheElem(const StringW& rName, F32* p)
{
	CacheElem	cElem;
	cElem.iType = eOTFloat;
	cElem.pElem = p;

	cCacheMap_[rName] = cElem;
}

void
Option::AddCacheElem(const StringW& rName, StringW* p)
{
	CacheElem	cElem;
	cElem.iType = eOTStringW;
	cElem.pElem = p;

	cCacheMap_[rName] = cElem;
}

void
Option::AddCacheElem(const StringW& rName, StringA* p)
{
	CacheElem	cElem;
	cElem.iType = eOTStringA;
	cElem.pElem = p;

	cCacheMap_[rName] = cElem;
}

Option::OptionType
Option::GetType(const StringW& rType)
{
	if (rType.compare(_W("bool")) == 0)
		return eOTBool;
	else if (rType.compare(_W("integer")) == 0)
		return eOTInteger;
	else if (rType.compare(_W("float")) == 0)
		return eOTFloat;
	else if (rType.compare(_W("stringw")) == 0)
		return eOTStringW;
	else if (rType.compare(_W("stringa")) == 0)
		return eOTStringA;
	else
		return eOTInvalid;
}

void
Option::AddElem(const StringW& rName, const StringW& rType, const StringW& rValue)
{
	// �ϴ� Ÿ�� Ȯ��
	OptionType	iType = GetType(rType);
	if (iType == eOTInvalid)
	{
		CHECK(0, eCheckAlways, _W("�� �� ���� �ɼ� Ÿ���Դϴ�.\n�̸� %s, Ÿ�� %s\n����Ϸ��� ���ø� ��������."), rName.c_str(), rType.c_str());
		return;
	}

	OptionMap::iterator	cIterExist = cOptionMap_.find(rName);
	if (!bOverwrite_ && cIterExist != cOptionMap_.end())
	{
		CHECK(0, eCheckAlways, _W("�̹� �����ϴ� �׸��̳�, ����Ⱑ ���Ǿ����� �ʽ��ϴ�.\n�̸� %s, Ÿ�� %s, �� %s\n����Ϸ��� ���ø� ��������."), rName.c_str(), rType.c_str(), rValue.c_str());
		return;
	}

	// �ִϸ� ������
	Any	cAny;
	switch (iType)
	{
	case eOTBool:
		{
			bool	b;
			StringUtil::Convert(rValue, b);
			cAny = b;
		}
		break;
	case eOTInteger:
		{
			S32	i;
			StringUtil::Convert(rValue, i);
			cAny = i;
		}
		break;
	case eOTFloat:
		{
			F32	f;
			StringUtil::Convert(rValue, f);
			cAny = f;
		}
		break;
	case eOTStringW:
		{
			StringW	s;
			s = rValue;
			cAny = s;
		}
		break;
	case eOTStringA:
		{
			StringA	s;
			s = StringUtil::ToStringA(rValue);
			cAny = s;
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	if (cIterExist == cOptionMap_.end())
		// ���� �׸��̴ϱ� ���� ����� ����.
        cOptionMap_[rName] = cAny;
	else
		// �̹� �ִ� �׸��̸� ���� �ٲ���.
		cIterExist->second = cAny;

	// ĳ���ؾ��Ѵٸ� ����
	CacheMap::iterator	iter = cCacheMap_.find(rName);
	if (iter != cCacheMap_.end())
	{
		switch (iter->second.iType)
		{
		case eOTBool:
			ASSERT(cAny.GetType() == Any::eATBool);
			*_rc<bool*>(iter->second.pElem) = cAny;
			break;
		case eOTInteger:
			ASSERT(cAny.GetType() == Any::eATSigned);
			*_rc<S32*>(iter->second.pElem) = cAny;
			break;
		case eOTFloat:
			ASSERT(cAny.GetType() == Any::eATFloat);
			*_rc<F32*>(iter->second.pElem) = cAny;
			break;
		case eOTStringW:
			ASSERT(cAny.GetType() == Any::eATStringW);
			*_rc<StringW*>(iter->second.pElem) = cAny;
			break;
		case eOTStringA:
			ASSERT(cAny.GetType() == Any::eATStringA);
			*_rc<StringA*>(iter->second.pElem) = cAny;
			break;
		default:
			ASSERT(0);
			break;
		}
	}
}