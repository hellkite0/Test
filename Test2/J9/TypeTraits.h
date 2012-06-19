// $Id: TypeTraits.h 6230 2010-06-22 09:26:44Z hyojin.lee $

#ifndef __J9_TYPE_TRAITS_H__
#define __J9_TYPE_TRAITS_H__

#include "TypeList.h"

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		IO�� ���Ͽ� ��ü�� read write�ϴ� ����� ����� struct
	///
	///				stream�� ���� �־����� ��ü���� ������ ������� IO�� ����
	///				����ϱ� ���� ���۵� Ŭ����.
	///				�� Ŭ������ ������ TypeTraits�� �����ؾ� �ϴ� �Լ�����
	///				����� ���� ������ abstract class�̴�.
	///				�ٸ� ������ Traits�� �����ϰ� ���� ����ڴ� �� Ŭ������
	///				�Լ����� �籸�� �ϸ� �ȴ�.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	struct TypeTraits
	{
		////////////////////////////////////////////////////////////////////////////////
		/// \brief		IO���� r�� �д´�.
		///
		///				�־��� IO���� r�� �о� ���δ�.
		///	\param[in]	pIO		����IO
		/// \param[out]	r		�о� ���� ��ü�� ���� reference
		/// \return		������ �о���� ũ��
		////////////////////////////////////////////////////////////////////////////////
		template<typename IO, typename T>
		static Size	Read(IO* pIO, T& r)						{ UNUSED(pIO); UNUSED(r); return 0; }
		template<typename IO, typename T>
		static Size	Read(IO* pIO, T& r, void*)				{ UNUSED(pIO); UNUSED(r); return 0; }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief		IO�� r�� ����Ѵ�.
		///
		///				�־��� IO�� r�� ����Ѵ�.
		///	\param[out]	pIO		����IO
		/// \param[in]	r		����� ��ü�� ���� reference
		/// \return		������ ��ϵ� ũ��
		////////////////////////////////////////////////////////////////////////////////
		template<typename IO, typename T>
		static Size	Write(IO* pIO, const T& r)				{ UNUSED(pIO); UNUSED(r); return 0; }
		template<typename IO, typename T>
		static Size	Write(IO* pIO, const T& r, void*)		{ UNUSED(pIO); UNUSED(r); return 0; }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		stream�� ���� �־����� ��ü�� binary format���� �а� ���� traits
	///
	///				�⺻��(PrimitiveTypes)��ü���� �������·� IO�� �����Ѵ�.
	///				�⺻�� �̿��� ��ü���� �� ��ü�� ���� ������ �б�/���� �Լ���
	///				�����ϵ��� �Ѵ�.
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
	/// \brief		�⺻���� ���� ���̳ʸ� ���� ����� ����
	///
	/// ����, �Ǽ����� �⺻���� ���̳ʸ� ���� ������� ������ ���� Ŭ�����̴�.
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
	/// \brief		��ü�� ���̳ʸ� ���� ����� ���� ���θ� ��ü���� ������ �� �ֵ��� �� Ŭ����
	///
	/// ��ü�� ���̳ʸ� ���·� ����� �� �� ������ �� Ŭ������ ��ȭ �ؼ� ��ü�� ũ�⸦ ����� �ش�.<br>
	///	��ü�� ũ��� sizoef�����ڷ� ���� �� �����Ƿ�, ��ü�� ũ�Ⱑ �ƴ� bool���� ������ �ص� ������
	///	�̷��� ����ϴ� ������ ����ڿ��� ��ü�� align���θ� �����ϱ� ���ؼ��̴�.<br>
	///	struct { int a; char b; float c; } �� ���� ��ü�� ũ��� ������ ȯ�濡 ���� �޶�����.
	/// �̷��� ����ü�� ũ�⸦ 9����Ʈ�� ����ϱ� ���ؼ��� #pragma pack() Ű���带 ����ϸ� �ȴ�.
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
	/// \brief		������� Ŀ���� ���� ���� Ŭ������ �״�� ������ �� �ִ�
	///				��ü�鿡 ���� ���̳ʸ� ���� ����� ����
	///
	/// ���ٸ� ����� ��ȭ�� �ʿ���� ������������ ����� ������ ��ü�鿡 ����
	/// ����� �����̴�. ���̳ʸ� �������� �ٷ� ������� �ϱ� ���ؼ���
	/// ����ڰ� �ش� Ÿ���� ������������ ������� �������� ��������� ������ �־�� �Ѵ�.
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
	/// \brief		��ü�� �������������� ������� ��������� ��ȭ �� �ֱ� ���� Ŭ����
	///
	/// Ư���� ������� ����� �Ǿ�� �ϴ� Ŭ������ �� ���ø��� ��ȭ �Ͽ� �����Ѵ�
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
	/// \brief		��ü�� ���� ������ ����¹���� ����ִ� Ŭ����
	///
	/// �⺻���ΰ�, ���̳ʸ� ���ۿ� ���� ��޵� �� �ִ°�, ���� customize�� �ʿ��Ѱ�
	/// ���ο� ���� ���� ����� ������ ����ִ� Ŭ����.
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
	/// \brief		stream�� ���� �־����� ��ü�� unicode text format���� �а� ���� traits
	///
	///				����� �Է����� �־����� ��ü�� �����Ŀ� �ٽ� �о���� ��
	///				�� ��ü ������ �����ϱ� ���� delimeter���� ���� ���� �ʾƼ�
	///				StringW ��ü�� �����ϵ��� �Ѵ�. StringW ��ü ���� multi line�� ��
	///				�� �ְ�, ���� �ϳ��� StringW�� ������ �� �ִ� ������ �����Ƿ�
	///				StringW ���常 �����Ѵ�.
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
/// \brief		��ü�� ���̳ʸ� ���� ����� ���� ���θ�������� ������ �ִ� ��ũ��
///
/// ��ü�� ���̳ʸ� ���·� ����� �� �� ������ �� ��ũ�θ� ����Ͽ� ��������� ������ �־�� �Ѵ�.
/// \param[in]	TYPE	��ü �̸�.
/// \param[in]	SIZE	��ü�� ũ��.
/// \author		boro
/// \date		2005/05/30
/// \see		USE_TYPE_TRAITS_DEFAULT, TypeTraits_Binary_Default
////////////////////////////////////////////////////////////////////////////////
#define USE_TYPE_TRAITS_DEFAULT(TYPE, SIZE) \
	template<> struct J9::TypeTraits_Binary_DefaultHelper<TYPE> \
	{ enum { iSize = SIZE }; };

#endif//__J9_TYPE_TRAITS_H__
