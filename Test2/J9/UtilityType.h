// $Id$

#ifndef __J9_UTILITY_TYPE_H__
#define __J9_UTILITY_TYPE_H__

	// Direct에서 사용하는 형식과 Just9라이브러리에서 사용하는 형식간의 형 변환을 도와주는 유틸리티들
	namespace SameType
	{
		#define J9_TREAT_AS_SAME_TYPE(Type1, Type2) \
		__inline const Type1&	TX(const Type2& r)	{ return _rc<const Type1&>(r); } \
		__inline       Type1&	TX(      Type2& r)	{ return _rc<      Type1&>(r); } \
		__inline const Type2&	TX(const Type1& r)	{ return _rc<const Type2&>(r); } \
		__inline       Type2&	TX(      Type1& r)	{ return _rc<      Type2&>(r); } \
		__inline const Type1*	TX(const Type2* r)	{ return _rc<const Type1*>(r); } \
		__inline       Type1*	TX(      Type2* r)	{ return _rc<      Type1*>(r); } \
		__inline const Type2*	TX(const Type1* r)	{ return _rc<const Type2*>(r); } \
		__inline       Type2*	TX(      Type1* r)	{ return _rc<      Type2*>(r); } \

#ifdef J9_SOL_USE_DIRECTX
		J9_TREAT_AS_SAME_TYPE(J9::Vector2, D3DXVECTOR2);
		J9_TREAT_AS_SAME_TYPE(J9::Vector3, D3DXVECTOR3);
		J9_TREAT_AS_SAME_TYPE(J9::Vector4, D3DXVECTOR4);
#endif
	}

	using namespace SameType;

#endif//__J9_UTILITY_TYPE_H__