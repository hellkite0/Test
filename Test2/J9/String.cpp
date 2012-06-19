// $Id: String.cpp 8979 2011-07-15 03:14:11Z hyojin.lee $

#include "stdafx.h"
#include "String.h"

#include "HashFunc.h"

using namespace J9;

#ifdef _SERVER
template<>
bool
ILess<StringW>::operator()(const StringW& lhs, const StringW& rhs) const
{
	return ::_wcsicmp(lhs.c_str(), rhs.c_str()) < 0;
}
#endif

const StringW&
J9::NullStringW()
{
	static StringW	sNull;
	return sNull;
}

const StringA&
J9::NullStringA()
{
	static StringA	sNull;
	return sNull;
}