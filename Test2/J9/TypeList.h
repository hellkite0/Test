// $Id: TypeList.h 4219 2009-10-21 09:15:14Z hyojin.lee $

#ifndef __J9_TYPE_LIST_H__
#define __J9_TYPE_LIST_H__

// TypeList from Modern C++ Design Addison Wesley
namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	// class template Int2Type
	// Converts each integral constant into a unique type
	// Invocation: Int2Type<v> where v is a compile-time constant integral
	// Defines 'value', an enum that evaluates to v
	////////////////////////////////////////////////////////////////////////////////
	template<S32 iValue>
	struct Int2Type
	{
		enum
		{
			Value = iValue
		};
	};

	////////////////////////////////////////////////////////////////////////////////
	// class template Type2Type
	// Converts each type into a unique, insipid type
	// Invocation Type2Type<T> where T is a type
	// Defines the type OriginalType which maps back to T
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct Type2Type
	{
		typedef T OriginalType;
	};

	////////////////////////////////////////////////////////////////////////////////
	// class template Select
	// Selects one of two types based upon a boolean constant
	// Invocation: Select<flag, T, U>::Result
	// where:
	// flag is a compile-time boolean constant
	// T and U are types
	// Result evaluates to T if flag is true, and to U otherwise.
	////////////////////////////////////////////////////////////////////////////////
	template<bool bFlag, typename T, typename U>
	struct Select
	{
		typedef T Result;
	};

	template<typename T, typename U>
	struct Select<false, T, U>
	{
		typedef U Result;
	};

	////////////////////////////////////////////////////////////////////////////////
	// Helper types Small and Big - guarantee that sizeof(Small) < sizeof(Big)
	////////////////////////////////////////////////////////////////////////////////
	#pragma pack(push)
	#pragma pack(1)
	namespace Private
	{
		template<typename T, typename U>
		struct ConversionHelper
		{
			struct Small
			{
				S8	iDummies[1];
			};
			struct Big
			{
				S8	iDummies[2];
			};

			static Big		Test(...);
			static Small	Test(U);
			static T		MakeT();
		};
	}
	#pragma pack(pop)

	////////////////////////////////////////////////////////////////////////////////
	// class template Conversion
	// Figures out the conversion relationships between two types
	// Invocations (T and U are types):
	// a) Conversion<T, U>::Exists
	// returns (at compile time) true if there is an implicit conversion from T
	// to U (example: Derived to Base)
	// b) Conversion<T, U>::Exists2Way
	// returns (at compile time) true if there are both conversions from T
	// to U and from U to T (example: int to char and back)
	// c) Conversion<T, U>::SameType
	// returns (at compile time) true if T and U represent the same type
	//
	// Caveat: might not work if T and U are in a private inheritance hierarchy.
	////////////////////////////////////////////////////////////////////////////////
	template<typename T, typename U>
	struct Conversion
	{
		typedef Private::ConversionHelper<T, U>	H;
		enum
		{
			Exists = sizeof(typename H::Small) == sizeof(H::Test(H::MakeT())),
			Exists2Way = Exists && Conversion<U, T>::Exists,
			SameType = false
		};
	};

	template<typename T>
	struct Conversion<T, T>
	{
		enum
		{
			Exists = 1,
			Exists2Way = 1,
			SameType = 1
		};
	};

	template<typename T>
	struct Conversion<void, T>
	{
		enum
		{
			Exists = 1,
			Exists2Way = 0,
			SameType = 0
		};
	};

	template<typename T>
	struct Conversion<T, void>
	{
		enum
		{
			Exists = 1,
			Exists2Way = 0,
			SameType = 0
		};
	};

	template<>
	struct Conversion<void, void>
	{
		enum
		{
			Exists = 1,
			Exists2Way = 1,
			SameType = 1
		};
	};
};

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////
#define SUPERSUBCLASS(T, U) \
	(J9::Conversion<const U*, const T*>::Exists && !J9::Conversion<const T*, const void*>::SameType)

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS_STRICT
// Invocation: SUPERSUBCLASS_STRICT(B, D) where B and D are types.
// Returns true if B is a public base of D.
//
// Caveat: might not work if B and D are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////
#define SUPERSUBCLASS_STRICT(B, D) \
	(SUPERSUBCLASS(B, D) && !J9::Conversion<const B, const D>::SameType)

////////////////////////////////////////////////////////////////////////////////
// macros TYPELIST_1, TYPELIST_2, ... TYPELIST_50
// Each takes a number of arguments equal to its numeric suffix
// The arguments are type names. TYPELIST_NN generates a typelist containing
//   all types passed as arguments, in that order.
// Example: TYPELIST_2(char, int) generates a type containing char and int.
////////////////////////////////////////////////////////////////////////////////
#define TYPELIST_1(T1) \
	J9::Typelist<T1, J9::NullType>

#define TYPELIST_2(T1, T2) \
	J9::Typelist<T1, TYPELIST_1(T2) >

#define TYPELIST_3(T1, T2, T3) \
	J9::Typelist<T1, TYPELIST_2(T2, T3) >

#define TYPELIST_4(T1, T2, T3, T4) \
	J9::Typelist<T1, TYPELIST_3(T2, T3, T4) >

#define TYPELIST_5(T1, T2, T3, T4, T5) \
	J9::Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >

#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
	J9::Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >

#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
	J9::Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >

#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
	J9::Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >

#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	J9::Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >

#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
	J9::Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >

#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11) \
	J9::Typelist<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11) >

#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12) \
	J9::Typelist<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12) >

#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13) \
	J9::Typelist<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13) >

#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14) \
	J9::Typelist<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14) >

#define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15) \
	J9::Typelist<T1, TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15) >

#define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16) \
	J9::Typelist<T1, TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16) >

#define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17) \
	J9::Typelist<T1, TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17) >

#define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18) \
	J9::Typelist<T1, TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18) >

#define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19) \
	J9::Typelist<T1, TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19) >

#define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
	J9::Typelist<T1, TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) >

#define TYPELIST_21(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21) \
	J9::Typelist<T1, TYPELIST_20(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21) >

#define TYPELIST_22(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22) \
	J9::Typelist<T1, TYPELIST_21(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22) >

#define TYPELIST_23(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23) \
	J9::Typelist<T1, TYPELIST_22(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23) >

#define TYPELIST_24(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24) \
	J9::Typelist<T1, TYPELIST_23(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24) >

#define TYPELIST_25(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25) \
	J9::Typelist<T1, TYPELIST_24(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25) >

#define TYPELIST_26(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26) \
	J9::Typelist<T1, TYPELIST_25(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26) >

#define TYPELIST_27(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27) \
	J9::Typelist<T1, TYPELIST_26(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27) >

#define TYPELIST_28(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28) \
	J9::Typelist<T1, TYPELIST_27(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28) >

#define TYPELIST_29(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29) \
	J9::Typelist<T1, TYPELIST_28(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29) >

#define TYPELIST_30(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) \
	J9::Typelist<T1, TYPELIST_29(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) >

#define TYPELIST_31(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31) \
	J9::Typelist<T1, TYPELIST_30(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31) >

#define TYPELIST_32(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32) \
	J9::Typelist<T1, TYPELIST_31(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32) >

#define TYPELIST_33(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33) \
	J9::Typelist<T1, TYPELIST_32(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33) >

#define TYPELIST_34(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34) \
	J9::Typelist<T1, TYPELIST_33(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34) >

#define TYPELIST_35(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35) \
	J9::Typelist<T1, TYPELIST_34(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35) >

#define TYPELIST_36(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36) \
	J9::Typelist<T1, TYPELIST_35(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36) >

#define TYPELIST_37(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37) \
	J9::Typelist<T1, TYPELIST_36(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37) >

#define TYPELIST_38(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38) \
	J9::Typelist<T1, TYPELIST_37(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38) >

#define TYPELIST_39(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39) \
	J9::Typelist<T1, TYPELIST_38(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39) >

#define TYPELIST_40(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) \
	J9::Typelist<T1, TYPELIST_39(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) >

#define TYPELIST_41(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41) \
	J9::Typelist<T1, TYPELIST_40(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41) >

#define TYPELIST_42(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42) \
	J9::Typelist<T1, TYPELIST_41(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42) >

#define TYPELIST_43(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43) \
	J9::Typelist<T1, TYPELIST_42(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43) >

#define TYPELIST_44(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44) \
	J9::Typelist<T1, TYPELIST_43(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44) >

#define TYPELIST_45(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45) \
	J9::Typelist<T1, TYPELIST_44(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45) >

#define TYPELIST_46(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46) \
	J9::Typelist<T1, TYPELIST_45(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45) >

#define TYPELIST_47(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47) \
	J9::Typelist<T1, TYPELIST_46(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47) >

#define TYPELIST_48(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47, T48) \
	J9::Typelist<T1, TYPELIST_47(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47, T48) >

#define TYPELIST_49(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47, T48, T49) \
	J9::Typelist<T1, TYPELIST_48(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47, T48, T49) >

#define TYPELIST_50(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) \
	J9::Typelist<T1, TYPELIST_49(T2, T3, T4, T5, T6, T7, T8, T9, T10 \
	, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 \
	, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 \
	, T31, T32, T33, T34, T35, T36, T37, T38, T39, T40 \
	, T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) >

namespace J9
{
	template<typename T, typename U>
	struct Typelist
	{
		typedef T Head;
		typedef U Tail;
	};

	namespace TL
	{
		////////////////////////////////////////////////////////////////////////////////
		// class template Length
		// Computes the length of a typelist
		// Invocation (TList is a typelist):
		// Length<TList>::Value
		// returns a compile-time constant containing the length of TList, not counting
		//   the end terminator (which by convention is NullType)
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList>
		struct Length;

		template<>
		struct Length<NullType>
		{
			enum
			{
				Value = 0
			};
		};

		template<typename T, typename U>
		struct Length< Typelist<T, U> >
		{
			enum
			{
				Value = 1 + Length<U>::Value
			};
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template TypeAt
		// Finds the type at a given index in a typelist
		// Invocation (TList is a typelist and index is a compile-time integral
		//   constant):
		// TypeAt<TList, index>::Result
		// returns the type in position 'index' in TList
		// If you pass an out-of-bounds index, the result is a compile-time error
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, U32 iIndex>
		struct TypeAt;

		template<typename Head, typename Tail>
		struct TypeAt<Typelist<Head, Tail>, 0>
		{
			typedef Head Result;
		};

		template<typename Head, typename Tail, U32 iIndex>
		struct TypeAt<Typelist<Head, Tail>, iIndex>
		{
			typedef typename TypeAt<Tail, iIndex - 1>::Result Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template TypeAtNonStrict
		// Finds the type at a given index in a typelist
		// Invocations (TList is a typelist and index is a compile-time integral
		//   constant):
		// a) TypeAtNonStrict<TList, index>::Result
		// returns the type in position 'index' in TList, or NullType if index is
		//   out-of-bounds
		// b) TypeAtNonStrict<TList, index, D>::Result
		// returns the type in position 'index' in TList, or D if index is out-of-bounds
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, U32 iIndex, typename DefaultType = NullType>
		struct TypeAtNonStrict
		{
			typedef DefaultType Result;
		};

		template<typename Head, typename Tail, typename DefaultType>
		struct TypeAtNonStrict<Typelist<Head, Tail>, 0, DefaultType>
		{
			typedef Head Result;
		};

		template<typename Head, typename Tail, U32 iIndex, typename DefaultType>
		struct TypeAtNonStrict<Typelist<Head, Tail>, iIndex, DefaultType>
		{
			typedef
			typename TypeAtNonStrict<Tail, iIndex - 1, DefaultType>::Result
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template IndexOf
		// Finds the index of a type in a typelist
		// Invocation (TList is a typelist and T is a type):
		// IndexOf<TList, T>::Value
		// returns the position of T in TList, or NullType if T is not found in TList
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, typename T>
		struct IndexOf;

		template<typename T>
		struct IndexOf<NullType, T>
		{
			enum
			{
				Value = -1
			};
		};

		template<typename T, typename Tail>
		struct IndexOf<Typelist<T, Tail>, T>
		{
			enum
			{
				Value = 0
			};
		};

		template<typename Head, typename Tail, typename T>
		struct IndexOf<Typelist<Head, Tail>, T>
		{
		private:
			enum
			{
				Temp = IndexOf<Tail, T>::Value,
			};
		public:
			enum
			{
				Value = (Temp == -1 ? -1 : 1 + Temp)
			};
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template Append
		// Appends a type or a typelist to another
		// Invocation (TList is a typelist and T is either a type or a typelist):
		// Append<TList, T>::Result
		// returns a typelist that is TList followed by T and NullType-terminated
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, typename T>
		struct Append;

		template<>
		struct Append<NullType, NullType>
		{
			typedef NullType Result;
		};

		template<typename T>
		struct Append<NullType, T>
		{
			typedef TYPELIST_1(T) Result;
		};

		template<typename Head, typename Tail>
		struct Append<NullType, Typelist<Head, Tail> >
		{
			typedef Typelist<Head, Tail> Result;
		};

		template<typename Head, typename Tail, typename T>
		struct Append<Typelist<Head, Tail>, T>
		{
			typedef
			Typelist<Head, typename Append<Tail, T>::Result>
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template Erase
		// Erases the first occurence, if any, of a type in a typelist
		// Invocation (TList is a typelist and T is a type):
		// Erase<TList, T>::Result
		// returns a typelist that is TList without the first occurence of T
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, typename T>
		struct Erase;

		template<typename T>             // Specialization 1
		struct Erase<NullType, T>
		{
			typedef NullType Result;
		};

		template<typename T, typename Tail>       // Specialization 2
		struct Erase<Typelist<T, Tail>, T>
		{
			typedef Tail Result;
		};

		template<typename Head, typename Tail, typename T> // Specialization 3
		struct Erase<Typelist<Head, Tail>, T>
		{
			typedef
			Typelist<Head, typename Erase<Tail, T>::Result>
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template EraseAll
		// Erases all first occurences, if any, of a type in a typelist
		// Invocation (TList is a typelist and T is a type):
		// EraseAll<TList, T>::Result
		// returns a typelist that is TList without any occurence of T
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, typename T>
		struct EraseAll;

		template<typename T>
		struct EraseAll<NullType, T>
		{
			typedef NullType Result;
		};

		template<typename T, typename Tail>
		struct EraseAll<Typelist<T, Tail>, T>
		{
			// Go all the way down the list removing the type
			typedef typename EraseAll<Tail, T>::Result Result;
		};

		template<typename Head, typename Tail, typename T>
		struct EraseAll<Typelist<Head, Tail>, T>
		{
			// Go all the way down the list removing the type
			typedef
			Typelist<Head, typename EraseAll<Tail, T>::Result>
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template NoDuplicates
		// Removes all duplicate types in a typelist
		// Invocation (TList is a typelist):
		// NoDuplicates<TList, T>::Result
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList>
		struct NoDuplicates;

		template<>
		struct NoDuplicates<NullType>
		{
			typedef NullType Result;
		};

		template<typename Head, typename Tail>
		struct NoDuplicates< Typelist<Head, Tail> >
		{
		private:
			typedef typename NoDuplicates<Tail>::Result	L1;
			typedef typename Erase<L1, Head>::Result	L2;

		public:
			typedef Typelist<Head, L2> Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template Replace
		// Replaces the first occurence of a type in a typelist, with another type
		// Invocation (TList is a typelist, T, U are types):
		// Replace<TList, T, U>::Result
		// returns a typelist in which the first occurence of T is replaced with U
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, typename T, typename U>
		struct Replace;

		template<typename T, typename U>
		struct Replace<NullType, T, U>
		{
			typedef NullType Result;
		};

		template<typename T, typename Tail, typename U>
		struct Replace<Typelist<T, Tail>, T, U>
		{
			typedef Typelist<U, Tail> Result;
		};

		template<typename Head, typename Tail, typename T, typename U>
		struct Replace<Typelist<Head, Tail>, T, U>
		{
			typedef
			Typelist<Head, typename Replace<Tail, T, U>::Result>
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template ReplaceAll
		// Replaces all occurences of a type in a typelist, with another type
		// Invocation (TList is a typelist, T, U are types):
		// Replace<TList, T, U>::Result
		// returns a typelist in which all occurences of T is replaced with U
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, typename T, typename U>
		struct ReplaceAll;

		template<typename T, typename U>
		struct ReplaceAll<NullType, T, U>
		{
			typedef NullType Result;
		};

		template<typename T, typename Tail, typename U>
		struct ReplaceAll<Typelist<T, Tail>, T, U>
		{
			typedef
			Typelist<U, typename ReplaceAll<Tail, T, U>::Result>
			Result;
		};

		template<typename Head, typename Tail, typename T, typename U>
		struct ReplaceAll<Typelist<Head, Tail>, T, U>
		{
			typedef
			Typelist<Head, typename ReplaceAll<Tail, T, U>::Result>
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template Reverse
		// Reverses a typelist
		// Invocation (TList is a typelist):
		// Reverse<TList>::Result
		// returns a typelist that is TList reversed
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList> struct Reverse;

		template<typename T>
		struct Reverse< TYPELIST_1(T) >
		{
			typedef TYPELIST_1(T) Result;
		};

		template<typename Head, typename Tail>
		struct Reverse< Typelist<Head, Tail> >
		{
			typedef
			typename Append<typename Reverse<Tail>::Result, Head>::Result
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template MostDerived
		// Finds the type in a typelist that is the most derived from a given type
		// Invocation (TList is a typelist, T is a type):
		// Replace<TList, T>::Result
		// returns the type in TList that's the most derived from T
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList, typename T> struct MostDerived;

		template<typename T>
		struct MostDerived<NullType, T>
		{
			typedef T Result;
		};

		template<typename Head, typename Tail, typename T>
		struct MostDerived<Typelist<Head, Tail>, T>
		{
		private:
			typedef typename MostDerived<Tail, T>::Result Candidate;

		public:
			typedef
			typename Select<SUPERSUBCLASS(Candidate, Head), Head, Candidate>::Result
			Result;
		};

		////////////////////////////////////////////////////////////////////////////////
		// class template DerivedToFront
		// Arranges the types in a typelist so that the most derived types appear first
		// Invocation (TList is a typelist):
		// DerivedToFront<TList>::Result
		// returns the reordered TList
		////////////////////////////////////////////////////////////////////////////////
		template<typename TList> struct DerivedToFront;

		template<>
		struct DerivedToFront<NullType>
		{
			typedef NullType Result;
		};

		template<typename Head, typename Tail>
		struct DerivedToFront< Typelist<Head, Tail> >
		{
		private:
			typedef
			typename MostDerived<Tail, Head>::Result
			TheMostDerived;

			typedef
			typename Replace<Tail, TheMostDerived, Head>::Result
			L;

		public:
			typedef Typelist<TheMostDerived, L> Result;
		};
	};

	#pragma pack(push)
	#pragma pack(1)
	class PrimitiveHelper
	{
		template<typename T>
		friend class TypeCategory;
	private:
		// 리턴 타입에 따른 카테고리

		// 표준 타입이 아님
		struct _NonPrimitive	{ U8 _[1]; };
		// 포인터 타입
		struct _Pointer			{ U8 _[2]; };
		// 부호있는 정수 타입
		struct _Signed			{ U8 _[3]; };
		// 부호없는 정수 타입
		struct _Unsigned		{ U8 _[4]; };
		// 특수 정수 타입 (bool, __wchar_t)
		struct _Special			{ U8 _[5]; };
		// 부동 소수형 타입
		struct _Floating			{ U8 _[6]; };

		// 분류 함수
		static _NonPrimitive _PrimitiveType(const void*);

		template<typename T>
		static _Pointer		_PrimitiveType(T const* const*);

		template<typename T>
		static _Pointer		_PrimitiveType(T* const*);

		// 1 byte integral
		static _Signed		_PrimitiveType(char const*);
		static _Signed		_PrimitiveType(signed char const*);
		static _Unsigned	_PrimitiveType(unsigned char const*);
		// 2 byte integral
		static _Signed		_PrimitiveType(signed short const*);
		static _Unsigned	_PrimitiveType(unsigned short const*);
		// 4 byte integral
		static _Signed		_PrimitiveType(signed long const*);
		static _Unsigned	_PrimitiveType(unsigned long const*);
		static _Signed		_PrimitiveType(signed int const*);
		static _Unsigned	_PrimitiveType(unsigned int const*);
		// 8 byte integral
		static _Signed		_PrimitiveType(signed long long const*);
		static _Unsigned	_PrimitiveType(unsigned long long const*);
		// floating
		static _Floating	_PrimitiveType(float const*);
		static _Floating	_PrimitiveType(double const*);
		static _Floating	_PrimitiveType(long double const*);
		// special
		static _Special		_PrimitiveType(bool const*);
		static _Special		_PrimitiveType(__wchar_t const*);

		#define TYPE_CATEGORY_CHECK(TYPE, CATEGORY) \
		(sizeof(PrimitiveHelper::_PrimitiveType(_sc<TYPE*>(0))) \
		== sizeof(PrimitiveHelper::CATEGORY))
	};

	template<typename T>
	class TypeCategory
	{
	public:
		static const bool bPointerType		= TYPE_CATEGORY_CHECK(T, _Pointer);
		static const bool bSignedType		= TYPE_CATEGORY_CHECK(T, _Signed);
		static const bool bUnsignedType		= TYPE_CATEGORY_CHECK(T, _Unsigned);
		static const bool bSpecialType		= TYPE_CATEGORY_CHECK(T, _Special);
		static const bool bFloatingType		= TYPE_CATEGORY_CHECK(T, _Floating);
		static const bool bIntegralType		= bSignedType || bUnsignedType;
		static const bool bNumberType		= bIntegralType || bFloatingType;
		static const bool bValueType		= bNumberType || bSpecialType;
		static const bool bPrimitiveType	= bPointerType || bValueType;
		static const bool bVoidType			= false;
	};

	template<typename T>
	class TypeCategory<T&>
	{
	public:
		static const bool bPointerType = false;
		static const bool bSignedType = false;
		static const bool bUnsignedType = false;
		static const bool bSpecialType = false;
		static const bool bFloatingType = false;
		static const bool bIntegralType = false;
		static const bool bNumberType = false;
		static const bool bValueType = false;
		static const bool bPrimitiveType = false;
		static const bool bVoidType = false;
	};

	template<>
	class TypeCategory<void>
	{
	public:
		static const bool bPointerType = false;
		static const bool bSignedType = false;
		static const bool bUnsignedType = false;
		static const bool bSpecialType = true;
		static const bool bFloatingType = false;
		static const bool bIntegralType = false;
		static const bool bNumberType = false;
		static const bool bValueType = true;
		static const bool bPrimitiveType = true;
		static const bool bVoidType = true;
	};
	#pragma pack(pop)

	template<typename T>
	struct ArgType
	{
		typedef
		typename Select<TypeCategory<T>::bPrimitiveType, T, const T&>::Result
		Type;
	};
};

#endif//__J9_TYPE_LIST_H__
