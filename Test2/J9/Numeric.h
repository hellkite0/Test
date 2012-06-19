// $Id: Numeric.h 2832 2009-05-18 06:52:23Z hyojin.lee $

#ifndef __J9_NUMERIC_H__
#define __J9_NUMERIC_H__


namespace J9
{
	namespace Numeric
	{
		template<typename _Value>
		struct _Base
		{
			typedef typename _Value					Value;
			typedef typename ArgType<_Value>::Type	ValueArg;
		};

		// type helpers
		template<typename Value>
		struct Zero : public _Base<Value>
		{ __forceinline const Value operator()() const { return _sc<Value>(0); } };

		template<typename Value>
		struct One : public _Base<Value>
		{ __forceinline const Value operator()() const { return _sc<Value>(1); } };

		template<typename Value>
		struct Epsilon : public _Base<Value>
		{ __forceinline const Value operator()() const { CASSERT(0, NOT_IMPL); } };

		template<>
		struct Epsilon<F32> : public _Base<F32>
		{ __forceinline const Value operator()() const { return F32_EPSILON; } };

		template<>
		struct Epsilon<F64> : public _Base<F64>
		{ __forceinline const Value operator()() const { return F64_EPSILON; } };

		template<typename Value>
		struct Sqrt : public _Base<Value>
		{ __forceinline const Value operator()(ValueArg v) const { CASSERT(0, NOT_IMPL); } };

		template<>
		struct Sqrt<F32> : public _Base<F32>
		{ __forceinline const Value operator()(ValueArg v) const { return J9::Sqrt(v); } };

		template<>
		struct Sqrt<F64> : public _Base<F64>
		{ __forceinline const Value operator()(ValueArg v) const { return J9::Sqrt(v); } };

		// assignment operation
		template<typename Value>
		struct Assign : public _Base<Value>
		{ __forceinline void operator()(Value& lhs, ValueArg rhs) const { lhs = rhs; } };

		template<typename Value>
		struct AddAssign : public _Base<Value>
		{ __forceinline void operator()(Value& lhs, ValueArg rhs) const { lhs += rhs; } };

		template<typename Value>
		struct SubAssign : public _Base<Value>
		{ __forceinline void operator()(Value& lhs, ValueArg rhs) const { lhs -= rhs; } };

		template<typename Value>
		struct MulAssign : public _Base<Value>
		{ __forceinline void operator()(Value& lhs, ValueArg rhs) const { lhs *= rhs; } };

		template<typename Value>
		struct DivAssign : public _Base<Value>
		{ __forceinline void operator()(Value& lhs, ValueArg rhs) const { lhs /= rhs; } };

		template<typename Value>
		struct ModAssign : public _Base<Value>
		{
			__forceinline void operator()(Value& lhs, ValueArg rhs)  const
			{
				CASSERT(TypeCategory<Value>::bIntegralType, ONLY_FOR_INTEGRAL_TYPE);
				lhs %= rhs;
			}
		};

		// binary operation
		template<typename Value>
		struct Add : public _Base<Value>
		{ __forceinline Value operator()(ValueArg lhs, ValueArg rhs) const { return lhs + rhs; } };

		template<typename Value>
		struct Sub : public _Base<Value>
		{ __forceinline Value operator()(ValueArg lhs, ValueArg rhs) const { return lhs - rhs; } };

		template<typename Value>
		struct Mul : public _Base<Value>
		{ __forceinline Value operator()(ValueArg lhs, ValueArg rhs) const { return lhs * rhs; } };

		template<typename Value>
		struct Div : public _Base<Value>
		{ __forceinline Value operator()(ValueArg lhs, ValueArg rhs) const { return lhs / rhs; } };

		template<typename Value>
		struct Mod : public _Base<Value>
		{
			__forceinline Value operator()(ValueArg lhs, ValueArg rhs) const
			{
				CASSERT(TypeCategory<Value>::bIntegralType, ONLY_FOR_INTEGRAL_TYPE);
				return lhs % rhs;
			}
		};

		template<typename Value>
		struct Neg : public _Base<Value>
		{ __forceinline Value operator()(ValueArg v) const { return -v; } };
	};
};

#endif//__J9_NUMERIC_H__