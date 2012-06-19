// $Id: TypeHelper.h 2170 2009-03-16 02:41:28Z hyojin.lee $

#ifndef __J9_TYPE_HELPER_H__
#define __J9_TYPE_HELPER_H__

namespace J9
{
	template<typename T>
	class TypeHelper_Primitive
	{
	public:
		static T	Zero()
		{
			return 0;
		}
	};

	template<typename T>
	class TypeHelper_Custom
	{
	public:
		static T	Zero()
		{
			return T::Zero();
		}
	};

	// �־��� Ÿ���� 0�� �����ϴ� ���� �����ϴ� �Լ�
	// �⺻ ���� �ƴϸ� T::Zero() �Լ��� ������ �־�� �Ѵ�
	template<typename T>
	class TypeHelper
	{
	public:
		static T	Zero()
		{
			return typename Select<TypeCategory<T>::bPrimitiveType,
				typename TypeHelper_Primitive<T>,
				typename TypeHelper_Custom<T>
			>::Result::Zero();
		}
	};
}

#endif//__J9_TYPE_HELPER_H__
