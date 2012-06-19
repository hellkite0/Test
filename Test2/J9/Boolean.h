// $Id: Boolean.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_BOOLEAN_H__
#define __J9_BOOLEAN_H__

namespace J9
{
	namespace Boolean
	{
		struct And
		{ __forceinline bool operator()(bool lhs, bool rhs) const { return lhs && rhs; } };

		struct Or
		{ __forceinline bool operator()(bool lhs, bool rhs) const { return lhs || rhs; } };

		struct Xor
		{ __forceinline bool operator()(bool lhs, bool rhs) const { return lhs ^ rhs; } };

		struct Not
		{ __forceinline bool operator()(bool b) const { return !b; } };
	};
};

#endif//__J9_BOOLEAN_H__