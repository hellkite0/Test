// $Id: TypeDefs.h 4749 2009-12-10 09:43:24Z hyojin.lee $

#ifndef __J9_TYPE_DEFS_H__
#define __J9_TYPE_DEFS_H__

#include "CAssert.h"

typedef signed char					S8;
typedef unsigned char				U8;
typedef signed short				S16;
typedef unsigned short				U16;
typedef signed long					S32;
typedef unsigned long				U32;
typedef signed long long			S64;
typedef unsigned long long			U64;
typedef float						F32;
typedef double						F64;

#ifdef _J9_64BIT
	// 64비트의 Size를 정의할 필요가 있음
	typedef size_t					Size;
#else
	typedef size_t					Size;
#endif

typedef char						AChar;
#ifdef _NATIVE_WCHAR_T_DEFINED
	typedef wchar_t					WChar;
#else
	typedef unsigned short			WChar;
#endif

#ifdef _UNICODE
	typedef WChar	Char;
#else
	typedef AChar	Char;
#endif

#define __W(x)						L ## x
#define _W(x)						__W(x)

//#define _A(x)						x

////////////////////////////////////////////////////////////////////////////////
/// \brief	외부 API에 전달하기 위한 int형
///
/// 이 형은 내부적 용도로 절대 사용하지 말 것
/// 32비트 환경에서 long과 int는 모두 4바이트 정수형이지만
/// C++ 표준에 따르면 두 형은 서로 다른 형이어야 한다고 되어있다.
/// 따라서 int*와 long*사이의 형변환이 되지 않고
/// api에 int*를 전달하려면 int에 대한 형 정의가 필요하여 정의해 둔다.
/// \author		hyojin.lee
/// \date		2005/05/26
////////////////////////////////////////////////////////////////////////////////
typedef int							INT;
////////////////////////////////////////////////////////////////////////////////
/// \brief	외부 API에 전달하기 위한 unsigned int형
///
/// 이 형은 내부적 용도로 절대 사용하지 말 것
/// 32비트 환경에서 unsigned long과 unsigned int는 모두 4바이트 정수형이지만
/// C++ 표준에 따르면 두 형은 서로 다른 형이어야 한다고 되어있다.
/// 따라서 unsigned int*와 unsigned long*사이의 형변환이 되지 않고
/// api에 unsigned int*를 전달하려면 int에 대한 형 정의가 필요하여 정의해 둔다.
/// \author		hyojin.lee
/// \date		2005/05/26
////////////////////////////////////////////////////////////////////////////////
typedef unsigned int				UINT;

// see limit.h

#ifdef _WIN64
	#define SIZE_MIN					U64_MIN
	#define SIZE_MAX					U64_MAX
#else
	#define SIZE_MIN					U32_MIN
	#ifdef SIZE_MAX
		#if SIZE_MAX != UINT_MAX
			#error : something wrong
		#endif
	#else
		#define SIZE_MAX					U32_MAX
	#endif
#endif

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief	아무것도 아닌 dummy type
	///
	///	실제로는 아무것도 가지지 않는 dummy type
	///	void*와 같이 의미가 없는 타입으로 사용할 수 있다.
	////////////////////////////////////////////////////////////////////////////////
	struct NullType
	{
	};

	////////////////////////////////////////////////////////////////////////////////
	///	\brief	J9에서 정의하는 타입들의 크기를 체크한다.
	///
	///	J9에서 정의하는 형들의 크기가 의도된 바와 같은지를 체크한다. 컴파일 시점에서
	/// 체크가 가능하다.
	///	\author	hyojin.lee
	///	\date	2005-06-02
	////////////////////////////////////////////////////////////////////////////////
	class TypeSizeChecker
	{
	private:
		void
		Check(void)
		{
			CASSERT(sizeof(S8) == 1, TYPE_SIZE);
			CASSERT(sizeof(U8) == 1, TYPE_SIZE);
			CASSERT(sizeof(S16) == 2, TYPE_SIZE);
			CASSERT(sizeof(U16) == 2, TYPE_SIZE);
			CASSERT(sizeof(S32) == 4, TYPE_SIZE);
			CASSERT(sizeof(U32) == 4, TYPE_SIZE);
			CASSERT(sizeof(S64) == 8, TYPE_SIZE);
			CASSERT(sizeof(U64) == 8, TYPE_SIZE);
			CASSERT(sizeof(F32) == 4, TYPE_SIZE);
			CASSERT(sizeof(F64) == 8, TYPE_SIZE);
			#ifdef _J9_64BIT
				CASSERT(sizeof(Size) == 8, TYPE_SIZE);
				CASSERT(sizeof(INT) == 8, TYPE_SIZE);
				CASSERT(sizeof(UINT) == 8, TYPE_SIZE);
			#else
				CASSERT(sizeof(Size) == 4, TYPE_SIZE);
				CASSERT(sizeof(INT) == 4, TYPE_SIZE);
				CASSERT(sizeof(UINT) == 4, TYPE_SIZE);
			#endif
			CASSERT(sizeof(AChar) == 1, TYPE_SIZE);
			CASSERT(sizeof(WChar) == 2, TYPE_SIZE);

			#ifdef _UNICODE
				CASSERT(sizeof(Char) == 2, TYPE_SIZE);
			#else
				CASSERT(sizeof(Char) == 1, TYPE_SIZE);
			#endif
		}
	};
};

#endif//__J9_TYPE_DEFS_H__