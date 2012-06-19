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

	// 주어진 타입의 0에 대응하는 값을 리턴하는 함수
	// 기본 형이 아니면 T::Zero() 함수를 정의해 주어야 한다
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
