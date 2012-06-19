// $Id: TypeTraits.h 6230 2010-06-22 09:26:44Z hyojin.lee $

#ifndef __J9_TYPE_TRAITS_H__
#define __J9_TYPE_TRAITS_H__

#include "TypeList.h"

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		IO를 통하여 객체를 read write하는 방법을 기술한 struct
	///
	///				stream을 통해 주어지는 객체들을 적당한 방법으로 IO를 통해
	///				기록하기 위해 제작된 클래스.
	///				이 클래스는 일종의 TypeTraits가 구현해야 하는 함수들을
	///				기술해 놓은 일종의 abstract class이다.
	///				다른 형식의 Traits를 구현하고 싶은 사용자는 이 클래스의
	///				함수들을 재구현 하면 된다.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	struct TypeTraits
	{
		////////////////////////////////////////////////////////////////////////////////
		/// \brief		IO에서 r을 읽는다.
		///
		///				주어진 IO에서 r을 읽어 들인다.
		///	\param[in]	pIO		파일IO
		/// \param[out]	r		읽어 들일 객체에 대한 reference
		/// \return		실제로 읽어들인 크기
		////////////////////////////////////////////////////////////////////////////////
		template<typename IO, typename T>
		static Size	Read(IO* pIO, T& r)						{ UNUSED(pIO); UNUSED(r); return 0; }
		template<typename IO, typename T>
		static Size	Read(IO* pIO, T& r, void*)				{ UNUSED(pIO); UNUSED(r); return 0; }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief		IO에 r을 기록한다.
		///
		///				주어진 IO에 r을 기록한다.
		///	\param[out]	pIO		파일IO
		/// \param[in]	r		기록할 객체에 대한 reference
		/// \return		실제로 기록된 크기
		////////////////////////////////////////////////////////////////////////////////
		template<typename IO, typename T>
		static Size	Write(IO* pIO, const T& r)				{ UNUSED(pIO); UNUSED(r); return 0; }
		template<typename IO, typename T>
		static Size	Write(IO* pIO, const T& r, void*)		{ UNUSED(pIO); UNUSED(r); return 0; }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		stream을 통해 주어지는 객체를 binary format으로 읽고 쓰는 traits
	///
	///				기본형(PrimitiveTypes)객체들을 이진형태로 IO에 저장한다.
	///				기본형 이외의 객체들은 각 객체에 대한 별개의 읽기/쓰기 함수를
	///				구현하도록 한다.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	struct TypeTraits_Binary
	{
		template<typename IO, typename T>
		static Size	Read(IO* pIO, T& r);
		template<typename IO, typename T>
		static Size	Read(IO* pIO, T& r, void* pContext);

		template<typename IO, typename T>
		static Size	Write(IO* pIO, const T& r);
		template<typename IO, typename T>
		static Size	Write(IO* pIO, const T& r, void* pContext);
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		기본형의 대한 바이너리 파일 입출력 구현
	///
	/// 정수, 실수형등 기본형의 바이너리 파일 입출력을 구현해 놓은 클래스이다.
	/// \author		boro
	/// \date		2005/05/30
	////////////////////////////////////////////////////////////////////////////////
	template<typename IO, typename Type>
	struct TypeTraits_Binary_Primitive
	{
		__forceinline static Size
		Read(IO* pIO, Type& r)					{ return pIO->Read(sizeof(Type), &r); }

		__forceinline static Size
		Write(IO* pIO, const Type& r)			{ return pIO->Write(sizeof(Type), &r); }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		객체의 바이너리 형식 입출력 지원 여부를 객체별로 정의할 수 있도록 한 클래스
	///
	/// 객체가 바이너리 형태로 입출력 될 수 있으면 이 클래스를 상세화 해서 객체의 크기를 명시해 준다.<br>
	///	객체의 크기는 sizoef연산자로 구할 수 있으므로, 객체의 크기가 아닌 bool값만 재정의 해도 되지만
	///	이렇게 사용하는 이유는 사용자에게 객체의 align여부를 강요하기 위해서이다.<br>
	///	struct { int a; char b; float c; } 와 같은 객체의 크기는 컴파일 환경에 따라 달라진다.
	/// 이러한 구조체의 크기를 9바이트로 명시하기 위해서는 #pragma pack() 키워드를 사용하면 된다.
	/// \author		boro
	/// \date		2005/05/30
	/// \see		USE_TYPE_TRAITS_DEFAULT, TypeTraits_Binary_Default
	////////////////////////////////////////////////////////////////////////////////
	template<typename Type>
	struct TypeTraits_Binary_DefaultHelper
	{
		enum { iSize = 0 };
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		명시적인 커스텀 구현 없이 클래스를 그대로 덤프할 수 있는
	///				객체들에 대한 바이너리 파일 입출력 구현
	///
	/// 별다른 입출력 상세화가 필요없이 이진형식으로 입출력 가능한 객체들에 대한
	/// 입출력 구현이다. 바이너리 형식으로 바로 입출력을 하기 위해서는
	/// 사용자가 해당 타입이 이진형식으로 입출력이 가능함을 명시적으로 선언해 주어야 한다.
	/// \author		boro
	/// \date		2005/05/30
	/// \see		TypeTraits_Binary_DefaultHelper, USE_TYPE_TRAITS_DEFAULT
	////////////////////////////////////////////////////////////////////////////////
	template<typename IO, typename Type>
	struct TypeTraits_Binary_Default
	{
		__forceinline static Size
		Read(IO* pIO, Type& r)
		{
			CASSERT(sizeof(Type) == TypeTraits_Binary_DefaultHelper<Type>::iSize, PACKING);
			VERIFY(pIO->Read(sizeof(Type), &r) == sizeof(Type));
			return sizeof(Type);
		}

		__forceinline static Size
		Write(IO* pIO, const Type& r)
		{
			CASSERT(sizeof(Type) == TypeTraits_Binary_DefaultHelper<Type>::iSize, PACKING);
			VERIFY(pIO->Write(sizeof(Type), &r) == sizeof(Type));
			return sizeof(Type);
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		객체의 이진형식으로의 입출력을 명시적으로 상세화 해 주기 위한 클래스
	///
	/// 특별한 방식으로 입출력 되어야 하는 클래스는 이 템플릿을 상세화 하여 구현한다
	/// \author		boro
	/// \date		2005/05/30
	////////////////////////////////////////////////////////////////////////////////
	template<typename IO, typename Type>
	struct TypeTraits_Binary_Custom
	{
		static Size
		Read(IO* pIO, Type& r, void*)
		{
			CASSERT(0, NOT_IMPL);
			return 0;
		}

		static Size
		Write(IO* pIO, const Type& r, void*)
		{
			CASSERT(0, NOT_IMPL);
			return 0;
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		객체에 따른 이진형 입출력방식을 골라주는 클래스
	///
	/// 기본형인가, 바이너리 버퍼와 같이 취급될 수 있는가, 따로 customize가 필요한가
	/// 여부에 따라 파일 입출력 형식을 골라주는 클래스.
	/// \author		boro
	/// \date		2005/05/30
	////////////////////////////////////////////////////////////////////////////////
	template<typename IO, typename Type>
	struct TypeTraits_Binary_Selector
	{
		typedef
		typename Select
		<
			TypeCategory<Type>::bValueType,
			typename TypeTraits_Binary_Primitive<IO, Type>,
			typename Select
			<
				TypeTraits_Binary_DefaultHelper<Type>::iSize != 0,
				typename TypeTraits_Binary_Default<IO, Type>,
				typename TypeTraits_Binary_Custom<IO, Type>
			>::Result
		>::Result
		Traits;
	};

	template<typename IO, typename Type>
	__inline Size TypeTraits_Binary::Read(IO* pIO, Type& r)
	{
		return TypeTraits_Binary_Selector<IO, Type>::Traits::Read(pIO, r);
	}
	template<typename IO, typename Type>
	__inline Size TypeTraits_Binary::Read(IO* pIO, Type& r, void* pContext)
	{
		return TypeTraits_Binary_Selector<IO, Type>::Traits::Read(pIO, r, pContext);
	}
	template<typename IO, typename Type>
	__inline Size TypeTraits_Binary::Write(IO* pIO, const Type& r)
	{
		return TypeTraits_Binary_Selector<IO, Type>::Traits::Write(pIO, r);
	}
	template<typename IO, typename Type>
	__inline Size TypeTraits_Binary::Write(IO* pIO, const Type& r, void* pContext)
	{
		return TypeTraits_Binary_Selector<IO, Type>::Traits::Write(pIO, r, pContext);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		stream을 통해 주어지는 객체를 unicode text format으로 읽고 쓰는 traits
	///
	///				현재는 입력으로 주어지는 객체를 저장후에 다시 읽어들일 때
	///				각 객체 단위를 구분하기 위한 delimeter등이 정해 지지 않아서
	///				StringW 객체만 저장하도록 한다. StringW 객체 또한 multi line이 될
	///				수 있고, 실제 하나의 StringW을 구분할 수 있는 기준이 없으므로
	///				StringW 저장만 지원한다.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	struct TypeTraits_UnicodeText
	{
		template<typename IO, typename T>
		static Size	Read(IO* pIO, T& r);

		template<typename IO, typename T>
		static Size	Write(IO* pIO, const T& r);
	};
};

////////////////////////////////////////////////////////////////////////////////
/// \brief		객체의 바이너리 형식 입출력 지원 여부명시적으로 선언해 주는 매크로
///
/// 객체가 바이너리 형태로 입출력 될 수 있으면 이 매크로를 사용하여 명시적으로 선언해 주어야 한다.
/// \param[in]	TYPE	객체 이름.
/// \param[in]	SIZE	객체의 크기.
/// \author		boro
/// \date		2005/05/30
/// \see		USE_TYPE_TRAITS_DEFAULT, TypeTraits_Binary_Default
////////////////////////////////////////////////////////////////////////////////
#define USE_TYPE_TRAITS_DEFAULT(TYPE, SIZE) \
	template<> struct J9::TypeTraits_Binary_DefaultHelper<TYPE> \
	{ enum { iSize = SIZE }; };

#endif//__J9_TYPE_TRAITS_H__
