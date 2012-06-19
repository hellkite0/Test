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
	// 64��Ʈ�� Size�� ������ �ʿ䰡 ����
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
/// \brief	�ܺ� API�� �����ϱ� ���� int��
///
/// �� ���� ������ �뵵�� ���� ������� �� ��
/// 32��Ʈ ȯ�濡�� long�� int�� ��� 4����Ʈ ������������
/// C++ ǥ�ؿ� ������ �� ���� ���� �ٸ� ���̾�� �Ѵٰ� �Ǿ��ִ�.
/// ���� int*�� long*������ ����ȯ�� ���� �ʰ�
/// api�� int*�� �����Ϸ��� int�� ���� �� ���ǰ� �ʿ��Ͽ� ������ �д�.
/// \author		hyojin.lee
/// \date		2005/05/26
////////////////////////////////////////////////////////////////////////////////
typedef int							INT;
////////////////////////////////////////////////////////////////////////////////
/// \brief	�ܺ� API�� �����ϱ� ���� unsigned int��
///
/// �� ���� ������ �뵵�� ���� ������� �� ��
/// 32��Ʈ ȯ�濡�� unsigned long�� unsigned int�� ��� 4����Ʈ ������������
/// C++ ǥ�ؿ� ������ �� ���� ���� �ٸ� ���̾�� �Ѵٰ� �Ǿ��ִ�.
/// ���� unsigned int*�� unsigned long*������ ����ȯ�� ���� �ʰ�
/// api�� unsigned int*�� �����Ϸ��� int�� ���� �� ���ǰ� �ʿ��Ͽ� ������ �д�.
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
	/// \brief	�ƹ��͵� �ƴ� dummy type
	///
	///	�����δ� �ƹ��͵� ������ �ʴ� dummy type
	///	void*�� ���� �ǹ̰� ���� Ÿ������ ����� �� �ִ�.
	////////////////////////////////////////////////////////////////////////////////
	struct NullType
	{
	};

	////////////////////////////////////////////////////////////////////////////////
	///	\brief	J9���� �����ϴ� Ÿ�Ե��� ũ�⸦ üũ�Ѵ�.
	///
	///	J9���� �����ϴ� ������ ũ�Ⱑ �ǵ��� �ٿ� �������� üũ�Ѵ�. ������ ��������
	/// üũ�� �����ϴ�.
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