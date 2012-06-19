// $Id: PrimitiveTypeCompare.cpp 1 2008-01-11 09:29:48Z hyojin.lee $

#include "Stdafx.h"

using namespace J9;

/// 같은 그룹에 속하는 원소 2개 단위로 입력받아 그룹으로 묶어준다.
template<typename T>
class Group
{
public:
	typedef Size	ID;
	typedef stdext::hash_map<T, ID>				IDMap;
	// to avoid C4503
	struct ElementSet
	{
		typedef stdext::hash_set<T>				_ElementSet;
		stdext::hash_set<T>	cElement_;
	};
	typedef stdext::hash_map<ID, ElementSet>	GroupMap;

public:
	Group() : iID_(0) {}

	void	Add(const T& lhs, const T& rhs)
	{
		if (cIDs_.find(lhs) == cIDs_.end() && cIDs_.find(rhs) == cIDs_.end())
		{
			ID	iID = iID_++;
			cIDs_[lhs] = iID;
			cIDs_[rhs] = iID;
			cGroups_[iID].cElement_.insert(lhs);
			cGroups_[iID].cElement_.insert(rhs);
		}
		else
		{
			ID	lhsID = SIZE_MAX;
			ID	rhsID = SIZE_MAX;

			IDMap::iterator	iter;

			iter = cIDs_.find(lhs);
			if (iter != cIDs_.end())
				lhsID = iter->second;

			iter = cIDs_.find(rhs);
			if (iter != cIDs_.end())
				rhsID = iter->second;

			if (lhsID != SIZE_MAX && rhsID != SIZE_MAX && lhsID != rhsID)
			{
				if (lhsID != rhsID)
				{
					// 두 원소가 각각 다른 그룹에 속한다
					// 두 그룹을 통합한다. 편의상 lhs그룹에 통합
					GroupMap::iterator	lhsIter = cGroups_.find(lhsID);
					GroupMap::iterator	rhsIter = cGroups_.find(rhsID);

					ASSERT(lhsIter != cGroups_.end() && rhsIter != cGroups_.end());

					// 일단 IDMap을 갱신한다
					ElementSet::_ElementSet::iterator	iter = rhsIter->second.cElement_.begin();
					ElementSet::_ElementSet::iterator	iterEnd = rhsIter->second.cElement_.end();
					for(; iter != iterEnd; ++iter)
					{
						ASSERT(cIDs_.find(*iter) != cIDs_.end() && cIDs_[*iter] == rhsID);
						cIDs_[*iter] = lhsID;
					}

					// rhs의 원소들을 lhs쪽으로 옮긴다.
					lhsIter->second.cElement_.insert(rhsIter->second.cElement_.begin(), rhsIter->second.cElement_.end());
					cGroups_.erase(rhsIter);
				}
				else
				{
					// 같은 그룹에 있는 다시 두놈을 추가하려고 했다.
				}
			}
			else
			{
				if (lhsID != SIZE_MAX)
				{
					cIDs_[rhs] = lhsID;
					cGroups_[lhsID].cElement_.insert(rhs);
				}
				else
				{
					cIDs_[lhs] = rhsID;
					cGroups_[rhsID].cElement_.insert(lhs);
				}
			}
		}
	}

public:
	ID			iID_;
	IDMap		cIDs_;
	ElementSet	cElements_;
	GroupMap	cGroups_;
};

typedef Group<StringW>	StringGroup;

template<typename T1, typename T2>
struct TypeCompare
{
	void operator()(StringGroup& rGroup, const WChar* pT1, const WChar* pT2)
	{
		if (Always(Conversion<const T1*, const T2*>::SameType))
		{
			rGroup.Add(pT1, pT2);
		}
	}
};

bool PrimitiveTypeCompare()
{
#define _MAKE_CODE
#define _DO_TEST
#ifdef _MAKE_CODE
	struct TypeInfo
	{
		const WChar*	pName_;
		Size			iSize_;
	};

	#define TYPE_INFO(Type)	{ _W(#Type), sizeof(Type) }


	static const TypeInfo	cTypes[] = {
		TYPE_INFO(bool),
		TYPE_INFO(char),
		TYPE_INFO(signed char),
		TYPE_INFO(unsigned char),
		TYPE_INFO(__int8),
		TYPE_INFO(signed __int8),
		TYPE_INFO(unsigned __int8),
		TYPE_INFO(short),
		TYPE_INFO(signed short),
		TYPE_INFO(unsigned short),
		TYPE_INFO(__int16),
		TYPE_INFO(signed __int16),
		TYPE_INFO(unsigned __int16),
		TYPE_INFO(int),
		TYPE_INFO(signed int),
		TYPE_INFO(unsigned int),
		TYPE_INFO(__int32),
		TYPE_INFO(signed __int32),
		TYPE_INFO(unsigned __int32),
		TYPE_INFO(long),
		TYPE_INFO(signed long),
		TYPE_INFO(unsigned long),
		TYPE_INFO(long int),
		TYPE_INFO(signed long int),
		TYPE_INFO(unsigned long int),
		TYPE_INFO(long long),
		TYPE_INFO(signed long long),
		TYPE_INFO(unsigned long long),
		TYPE_INFO(long long int),
		TYPE_INFO(signed long long int),
		TYPE_INFO(unsigned long long int),
		TYPE_INFO(__int64),
		TYPE_INFO(signed __int64),
		TYPE_INFO(unsigned __int64),
		TYPE_INFO(float),
		TYPE_INFO(double),
		TYPE_INFO(long double),
		TYPE_INFO(wchar_t),
		TYPE_INFO(__wchar_t),
		TYPE_INFO(size_t),
	};

	//for (Size i = 0; i < countof(pTypes); ++i)
	//{
	//	LOG(_W("PRINT_SIZE(%S);\r\n"), pTypes[i]);
	//}

	TextFile	cFile;
	if (cFile.Open(_W("TypeCheck.txt"), FileOpenCreate | FileOpenWrite))
	{
		for (Size i = 0; i < countof(cTypes); ++i)
		{
			for (Size j = i; j < countof(cTypes); ++j)
			{
				if (cTypes[i].iSize_ == cTypes[j].iSize_)
				{
					cFile << StringUtil::Format(_W("TYPE_COMPARE(%s, %s);\r\n"), cTypes[i].pName_, cTypes[j].pName_);
				}
			}
		}
		cFile.Close();
	}

#endif//_MAKE_CODE
#ifdef _DO_TEST
	StringGroup	cGroup;

	#define PRINT_SIZE(T) \
		LOG(_W("%s size is %u\r\n"), _W(#T), sizeof(T));

	#define TYPE_COMPARE(T1, T2)	TypeCompare<T1, T2>()(cGroup, _W(#T1), _W(#T2));

	TYPE_COMPARE(bool, bool);
	TYPE_COMPARE(bool, char);
	TYPE_COMPARE(bool, signed char);
	TYPE_COMPARE(bool, unsigned char);
	TYPE_COMPARE(bool, __int8);
	TYPE_COMPARE(bool, signed __int8);
	TYPE_COMPARE(bool, unsigned __int8);
	TYPE_COMPARE(char, char);
	TYPE_COMPARE(char, signed char);
	TYPE_COMPARE(char, unsigned char);
	TYPE_COMPARE(char, __int8);
	TYPE_COMPARE(char, signed __int8);
	TYPE_COMPARE(char, unsigned __int8);
	TYPE_COMPARE(signed char, signed char);
	TYPE_COMPARE(signed char, unsigned char);
	TYPE_COMPARE(signed char, __int8);
	TYPE_COMPARE(signed char, signed __int8);
	TYPE_COMPARE(signed char, unsigned __int8);
	TYPE_COMPARE(unsigned char, unsigned char);
	TYPE_COMPARE(unsigned char, __int8);
	TYPE_COMPARE(unsigned char, signed __int8);
	TYPE_COMPARE(unsigned char, unsigned __int8);
	TYPE_COMPARE(__int8, __int8);
	TYPE_COMPARE(__int8, signed __int8);
	TYPE_COMPARE(__int8, unsigned __int8);
	TYPE_COMPARE(signed __int8, signed __int8);
	TYPE_COMPARE(signed __int8, unsigned __int8);
	TYPE_COMPARE(unsigned __int8, unsigned __int8);
	TYPE_COMPARE(short, short);
	TYPE_COMPARE(short, signed short);
	TYPE_COMPARE(short, unsigned short);
	TYPE_COMPARE(short, __int16);
	TYPE_COMPARE(short, signed __int16);
	TYPE_COMPARE(short, unsigned __int16);
	TYPE_COMPARE(short, wchar_t);
	TYPE_COMPARE(short, __wchar_t);
	TYPE_COMPARE(signed short, signed short);
	TYPE_COMPARE(signed short, unsigned short);
	TYPE_COMPARE(signed short, __int16);
	TYPE_COMPARE(signed short, signed __int16);
	TYPE_COMPARE(signed short, unsigned __int16);
	TYPE_COMPARE(signed short, wchar_t);
	TYPE_COMPARE(signed short, __wchar_t);
	TYPE_COMPARE(unsigned short, unsigned short);
	TYPE_COMPARE(unsigned short, __int16);
	TYPE_COMPARE(unsigned short, signed __int16);
	TYPE_COMPARE(unsigned short, unsigned __int16);
	TYPE_COMPARE(unsigned short, wchar_t);
	TYPE_COMPARE(unsigned short, __wchar_t);
	TYPE_COMPARE(__int16, __int16);
	TYPE_COMPARE(__int16, signed __int16);
	TYPE_COMPARE(__int16, unsigned __int16);
	TYPE_COMPARE(__int16, wchar_t);
	TYPE_COMPARE(__int16, __wchar_t);
	TYPE_COMPARE(signed __int16, signed __int16);
	TYPE_COMPARE(signed __int16, unsigned __int16);
	TYPE_COMPARE(signed __int16, wchar_t);
	TYPE_COMPARE(signed __int16, __wchar_t);
	TYPE_COMPARE(unsigned __int16, unsigned __int16);
	TYPE_COMPARE(unsigned __int16, wchar_t);
	TYPE_COMPARE(unsigned __int16, __wchar_t);
	TYPE_COMPARE(int, int);
	TYPE_COMPARE(int, signed int);
	TYPE_COMPARE(int, unsigned int);
	TYPE_COMPARE(int, __int32);
	TYPE_COMPARE(int, signed __int32);
	TYPE_COMPARE(int, unsigned __int32);
	TYPE_COMPARE(int, long);
	TYPE_COMPARE(int, signed long);
	TYPE_COMPARE(int, unsigned long);
	TYPE_COMPARE(int, long int);
	TYPE_COMPARE(int, signed long int);
	TYPE_COMPARE(int, unsigned long int);
	TYPE_COMPARE(int, float);
	TYPE_COMPARE(int, size_t);
	TYPE_COMPARE(signed int, signed int);
	TYPE_COMPARE(signed int, unsigned int);
	TYPE_COMPARE(signed int, __int32);
	TYPE_COMPARE(signed int, signed __int32);
	TYPE_COMPARE(signed int, unsigned __int32);
	TYPE_COMPARE(signed int, long);
	TYPE_COMPARE(signed int, signed long);
	TYPE_COMPARE(signed int, unsigned long);
	TYPE_COMPARE(signed int, long int);
	TYPE_COMPARE(signed int, signed long int);
	TYPE_COMPARE(signed int, unsigned long int);
	TYPE_COMPARE(signed int, float);
	TYPE_COMPARE(signed int, size_t);
	TYPE_COMPARE(unsigned int, unsigned int);
	TYPE_COMPARE(unsigned int, __int32);
	TYPE_COMPARE(unsigned int, signed __int32);
	TYPE_COMPARE(unsigned int, unsigned __int32);
	TYPE_COMPARE(unsigned int, long);
	TYPE_COMPARE(unsigned int, signed long);
	TYPE_COMPARE(unsigned int, unsigned long);
	TYPE_COMPARE(unsigned int, long int);
	TYPE_COMPARE(unsigned int, signed long int);
	TYPE_COMPARE(unsigned int, unsigned long int);
	TYPE_COMPARE(unsigned int, float);
	TYPE_COMPARE(unsigned int, size_t);
	TYPE_COMPARE(__int32, __int32);
	TYPE_COMPARE(__int32, signed __int32);
	TYPE_COMPARE(__int32, unsigned __int32);
	TYPE_COMPARE(__int32, long);
	TYPE_COMPARE(__int32, signed long);
	TYPE_COMPARE(__int32, unsigned long);
	TYPE_COMPARE(__int32, long int);
	TYPE_COMPARE(__int32, signed long int);
	TYPE_COMPARE(__int32, unsigned long int);
	TYPE_COMPARE(__int32, float);
	TYPE_COMPARE(__int32, size_t);
	TYPE_COMPARE(signed __int32, signed __int32);
	TYPE_COMPARE(signed __int32, unsigned __int32);
	TYPE_COMPARE(signed __int32, long);
	TYPE_COMPARE(signed __int32, signed long);
	TYPE_COMPARE(signed __int32, unsigned long);
	TYPE_COMPARE(signed __int32, long int);
	TYPE_COMPARE(signed __int32, signed long int);
	TYPE_COMPARE(signed __int32, unsigned long int);
	TYPE_COMPARE(signed __int32, float);
	TYPE_COMPARE(signed __int32, size_t);
	TYPE_COMPARE(unsigned __int32, unsigned __int32);
	TYPE_COMPARE(unsigned __int32, long);
	TYPE_COMPARE(unsigned __int32, signed long);
	TYPE_COMPARE(unsigned __int32, unsigned long);
	TYPE_COMPARE(unsigned __int32, long int);
	TYPE_COMPARE(unsigned __int32, signed long int);
	TYPE_COMPARE(unsigned __int32, unsigned long int);
	TYPE_COMPARE(unsigned __int32, float);
	TYPE_COMPARE(unsigned __int32, size_t);
	TYPE_COMPARE(long, long);
	TYPE_COMPARE(long, signed long);
	TYPE_COMPARE(long, unsigned long);
	TYPE_COMPARE(long, long int);
	TYPE_COMPARE(long, signed long int);
	TYPE_COMPARE(long, unsigned long int);
	TYPE_COMPARE(long, float);
	TYPE_COMPARE(long, size_t);
	TYPE_COMPARE(signed long, signed long);
	TYPE_COMPARE(signed long, unsigned long);
	TYPE_COMPARE(signed long, long int);
	TYPE_COMPARE(signed long, signed long int);
	TYPE_COMPARE(signed long, unsigned long int);
	TYPE_COMPARE(signed long, float);
	TYPE_COMPARE(signed long, size_t);
	TYPE_COMPARE(unsigned long, unsigned long);
	TYPE_COMPARE(unsigned long, long int);
	TYPE_COMPARE(unsigned long, signed long int);
	TYPE_COMPARE(unsigned long, unsigned long int);
	TYPE_COMPARE(unsigned long, float);
	TYPE_COMPARE(unsigned long, size_t);
	TYPE_COMPARE(long int, long int);
	TYPE_COMPARE(long int, signed long int);
	TYPE_COMPARE(long int, unsigned long int);
	TYPE_COMPARE(long int, float);
	TYPE_COMPARE(long int, size_t);
	TYPE_COMPARE(signed long int, signed long int);
	TYPE_COMPARE(signed long int, unsigned long int);
	TYPE_COMPARE(signed long int, float);
	TYPE_COMPARE(signed long int, size_t);
	TYPE_COMPARE(unsigned long int, unsigned long int);
	TYPE_COMPARE(unsigned long int, float);
	TYPE_COMPARE(unsigned long int, size_t);
	TYPE_COMPARE(long long, long long);
	TYPE_COMPARE(long long, signed long long);
	TYPE_COMPARE(long long, unsigned long long);
	TYPE_COMPARE(long long, long long int);
	TYPE_COMPARE(long long, signed long long int);
	TYPE_COMPARE(long long, unsigned long long int);
	TYPE_COMPARE(long long, __int64);
	TYPE_COMPARE(long long, signed __int64);
	TYPE_COMPARE(long long, unsigned __int64);
	TYPE_COMPARE(long long, double);
	TYPE_COMPARE(long long, long double);
	TYPE_COMPARE(signed long long, signed long long);
	TYPE_COMPARE(signed long long, unsigned long long);
	TYPE_COMPARE(signed long long, long long int);
	TYPE_COMPARE(signed long long, signed long long int);
	TYPE_COMPARE(signed long long, unsigned long long int);
	TYPE_COMPARE(signed long long, __int64);
	TYPE_COMPARE(signed long long, signed __int64);
	TYPE_COMPARE(signed long long, unsigned __int64);
	TYPE_COMPARE(signed long long, double);
	TYPE_COMPARE(signed long long, long double);
	TYPE_COMPARE(unsigned long long, unsigned long long);
	TYPE_COMPARE(unsigned long long, long long int);
	TYPE_COMPARE(unsigned long long, signed long long int);
	TYPE_COMPARE(unsigned long long, unsigned long long int);
	TYPE_COMPARE(unsigned long long, __int64);
	TYPE_COMPARE(unsigned long long, signed __int64);
	TYPE_COMPARE(unsigned long long, unsigned __int64);
	TYPE_COMPARE(unsigned long long, double);
	TYPE_COMPARE(unsigned long long, long double);
	TYPE_COMPARE(long long int, long long int);
	TYPE_COMPARE(long long int, signed long long int);
	TYPE_COMPARE(long long int, unsigned long long int);
	TYPE_COMPARE(long long int, __int64);
	TYPE_COMPARE(long long int, signed __int64);
	TYPE_COMPARE(long long int, unsigned __int64);
	TYPE_COMPARE(long long int, double);
	TYPE_COMPARE(long long int, long double);
	TYPE_COMPARE(signed long long int, signed long long int);
	TYPE_COMPARE(signed long long int, unsigned long long int);
	TYPE_COMPARE(signed long long int, __int64);
	TYPE_COMPARE(signed long long int, signed __int64);
	TYPE_COMPARE(signed long long int, unsigned __int64);
	TYPE_COMPARE(signed long long int, double);
	TYPE_COMPARE(signed long long int, long double);
	TYPE_COMPARE(unsigned long long int, unsigned long long int);
	TYPE_COMPARE(unsigned long long int, __int64);
	TYPE_COMPARE(unsigned long long int, signed __int64);
	TYPE_COMPARE(unsigned long long int, unsigned __int64);
	TYPE_COMPARE(unsigned long long int, double);
	TYPE_COMPARE(unsigned long long int, long double);
	TYPE_COMPARE(__int64, __int64);
	TYPE_COMPARE(__int64, signed __int64);
	TYPE_COMPARE(__int64, unsigned __int64);
	TYPE_COMPARE(__int64, double);
	TYPE_COMPARE(__int64, long double);
	TYPE_COMPARE(signed __int64, signed __int64);
	TYPE_COMPARE(signed __int64, unsigned __int64);
	TYPE_COMPARE(signed __int64, double);
	TYPE_COMPARE(signed __int64, long double);
	TYPE_COMPARE(unsigned __int64, unsigned __int64);
	TYPE_COMPARE(unsigned __int64, double);
	TYPE_COMPARE(unsigned __int64, long double);
	TYPE_COMPARE(float, float);
	TYPE_COMPARE(float, size_t);
	TYPE_COMPARE(double, double);
	TYPE_COMPARE(double, long double);
	TYPE_COMPARE(long double, long double);
	TYPE_COMPARE(wchar_t, wchar_t);
	TYPE_COMPARE(wchar_t, __wchar_t);
	TYPE_COMPARE(__wchar_t, __wchar_t);
	TYPE_COMPARE(size_t, size_t);

	TextFile	cResultFile;
	if (cResultFile.Open(_W("TypeCheckResult.txt"), FileOpenCreate | FileOpenWrite))
	{
		StringGroup::GroupMap::iterator	iter = cGroup.cGroups_.begin();
		StringGroup::GroupMap::iterator	iterEnd = cGroup.cGroups_.end();
		for (; iter != iterEnd; ++iter)
		{
			StringGroup::ElementSet::_ElementSet::iterator	iterSet = iter->second.cElement_.begin();
			StringGroup::ElementSet::_ElementSet::iterator	iterSetEnd = iter->second.cElement_.end();

			for (; iterSet != iterSetEnd; ++iterSet)
			{
				cResultFile << StringUtil::Format(_W("%s, "), (*iterSet).c_str());
			}
			cResultFile << _sc<const WChar*>(_W("\r\n"));
		}

		cResultFile.Close();
	}

#endif//_DO_TEST
	return true;
}
