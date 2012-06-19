// $Id: CommonMacros.h 7509 2010-11-22 02:28:58Z donghyuk.lee $

#ifndef __J9_COMMON_MACROS_H__
#define __J9_COMMON_MACROS_H__

class Dummy;

////////////////////////////////////////////////////////////////////////////////
/// \brief		abbreviation for const_cast
///
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define _cc const_cast

////////////////////////////////////////////////////////////////////////////////
/// \brief		abbreviation for dynamic_cast
///
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define _dc dynamic_cast

////////////////////////////////////////////////////////////////////////////////
/// \brief		abbreviation for static_cast
///
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define _sc static_cast

////////////////////////////////////////////////////////////////////////////////
/// \brief		abbreviation for reinterpret_cast
///
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define _rc reinterpret_cast

////////////////////////////////////////////////////////////////////////////////
/// \brief		클래스에서 해당 멤버의 위치를 알려주는 매크로
///
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define OFFSET_OF(c, x)	(_rc<Size>(&_rc<Dummy&>((_sc<c*>(0)->x))))

////////////////////////////////////////////////////////////////////////////////
/// \brief		__FILE__LINE__ 매크로를 사용하기 위해 필요한 매크로.
///
///				자세한 원리에 대해서 궁금한 사람은 직접 공부해 보도록.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define _QUOTE(x)		#x

////////////////////////////////////////////////////////////////////////////////
/// \brief		__FILE__LINE__ 매크로를 사용하기 위해 필요한 매크로.
///
///				자세한 원리에 대해서 궁금한 사람은 직접 공부해 보도록.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define QUOTE(x)		_QUOTE(x)

////////////////////////////////////////////////////////////////////////////////
/// \brief		전처리기에서 filename.cpp(30) : 과 같은 형태로 출력해 주기 위한
///				매크로
///
///				to do나 note, warning, error 메세지 등을 빌드창에 출력하는데
///				사용할 수 있다.
/// \author		boro
/// \date		2005/04/25
/// \warning	이 매크로를 실행 시점 이후에 참조 되는 것에 사용하면
///				출력 형식이 제대로 나오지 않는다.
///				반드시 #pragma message() 정도의 용도로 사용할 것
////////////////////////////////////////////////////////////////////////////////
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define __FILE_LINE__ __FILE__#__LINE__

////////////////////////////////////////////////////////////////////////////////
/// \brief		warning C4100: 'x' : unreferenced formal parameter워닝을 없애기
///				위한 매크로
///
///				함수의 인자를 사용하지 않는 것이 확실할 때 warning C4100을
///				없애기 위하여 명시적으로 사용되는 매크로 이다.
///				가상 함수를 오버 라이드 하여 구현할 때나,
///				추후에 사용될 것을 고려하여 현재는 사용되지 않는 함수 인자를
///				포함한 형식으로 함수 형 선언을 하는 경우,
///				API callback 등으로 사용하기 위해서 사용되지 않는 인자를
///				포함한 함수 형 선언을 하는 경우등에 사용한다.
///				원칙적으로 사용하지 않는 인자는 함수 형선언에서 없애는 것을
///				권장하지만 피치 못할 경우에 사용한다.
///				사용시 명확한 사용 이유를 밝히는 것을 권장한다.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define J9_UNUSED(x)		(void)(x)
#ifndef _AFX
	#define UNUSED(x)			J9_UNUSED(x)
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief		복사 생성자와 대입 연산자를 사용한 객체 생성을 방지하는 매크로.
///				클래스 선언부에서 사용한다.
///
/// \author		Hyuk
/// \date		2010/05/07
////////////////////////////////////////////////////////////////////////////////
#define DISABLE_COPY_CREATOR(ClassName) \
	private: \
		ClassName(const ClassName&); \
		ClassName& operator =(const ClassName&); \

////////////////////////////////////////////////////////////////////////////////
/// \brief		std container 에서, 해당 container 내부를 iterating 하는 매크로
///	
///	for 안에서만 유효한, iterator 를 선언하고 for 루프를 만들어 준다.
/// std::map<F32,U32> 같은 애를 쓰게되면.. , 를 기준으로 분리 해 버린다..-_-;
/// 따라서 typedef 하지 않으면 사용할 수 없는 경우가 있다.
///
/// \author		Hyuk
/// \date		2010/05/20
////////////////////////////////////////////////////////////////////////////////
#define FOR_ITER(containertype, containerloop, iteratorprefix)	\
	for (containertype##::iterator iteratorprefix = (containerloop##).begin(), iteratorprefix##End = (containerloop##).end() \
	; iteratorprefix != iteratorprefix##End; ++iteratorprefix)
#define FOR_CITER(containertype, containerloop, iteratorprefix)	\
	for (containertype##::const_iterator iteratorprefix = (containerloop##).begin(), iteratorprefix##End = (containerloop##).end() \
	; iteratorprefix != iteratorprefix##End; ++iteratorprefix)
#define FOR_RITER(containertype, containerloop, iteratorprefix)	\
	for (containertype##::reverse_iterator iteratorprefix = (containerloop##).rbegin(), iteratorprefix##End = (containerloop##).rend() \
	; iteratorprefix != iteratorprefix##End; ++iteratorprefix)
#define FOR_CRITER(containertype, containerloop, iteratorprefix)	\
	for (containertype##::const_reverse_iterator iteratorprefix = (containerloop##).rbegin(), iteratorprefix##End = (containerloop##).rend() \
	; iteratorprefix != iteratorprefix##End; ++iteratorprefix)
#define FOR_VECTOR(containertype, element, indexname) \
	for (Size indexname = 0, indexname##Size = (element).size(); indexname < indexname##Size; ++indexname)

////////////////////////////////////////////////////////////////////////////////
/// \brief		사용자가 직접 생성해야 하는 Singleton 객체임을 선언하고,
///				Singleton에서 사용되는 static function들을 제공하는 매크로
///
///	객체 안에서 사용함으로서, 해당 객체에게 Singleton Interface를 제공한다.
///	이 형식으로 선언된 Singleton은 사용자가 CreateInstance()를 호출하여
///	명시적으로 생성해 주어야 하고, DestroyInstance()를 사용하여 파괴해 주어야 한다.
/// \author		boro
/// \date		2005/04/25
///	\see		DYNAMIC_SINGLETON_IMPL
////////////////////////////////////////////////////////////////////////////////
#define DYNAMIC_SINGLETON(ClassName) \
	public: \
		__inline static bool		HasInstance()						{ return !pInstance_.IsNull(); } \
		__inline static ClassName*	Instance()							{ return pInstance_; } \
		__inline static void		CreateInstance()					{ ASSERT(!HasInstance()); pInstance_ = J9::Ptr<ClassName>(ALLOC); } \
		__inline static void		DestroyInstance()					{ if (HasInstance()) pInstance_ = NULL; } \
		__inline static void		SetInstance(ClassName* pInstance)	{ ASSERT(!HasInstance() && pInstance); pInstance_ = pInstance; } \
	protected: \
		static J9::Ptr<ClassName>	pInstance_; \

////////////////////////////////////////////////////////////////////////////////
/// \brief		실행시에 자동으로 초기화 되는 정적 객체 형식으로 Singleton을 선언하고
///				Singleton에서 사용되는 static function들을 제공하는 매크로
///
///	객체 안에서 사용함으로서, 해당 객체에게 Singleton Interface를 제공한다.
///	사용자가 명시적으로 생성, 파괴를 해 줄 필요가 없는 객체의 선언에 사용된다.
/// \author		boro
/// \date		2005/04/25
///	\see		STATIC_SINGLETON_IMPL
////////////////////////////////////////////////////////////////////////////////
#define STATIC_SINGLETON(ClassName) \
	public: \
		__inline static				ClassName*	Instance()				{ return &cInstance_; } \
	protected: \
		static ClassName	cInstance_; \

////////////////////////////////////////////////////////////////////////////////
/// \brief		사용자에 의해 생성되는 Singleton객체의 구현을 선언하는 매크로
///
///	Singleton객체는 단일한 객체로의 접근을 제공하기 위해 class member로
///	자신의 포인터를 가진다. class member는 class 외부에서 다시한번 정의되어야만
///	자신의 주소를 확보할 수 있다. 이 주소 공간을 확보하기 위해서 class 외부에서
///	이 매크로를 사용해 주어야 한다.
/// \author		boro
/// \date		2005/04/25
///	\see		DYNAMIC_SINGLETON
////////////////////////////////////////////////////////////////////////////////
#define DYNAMIC_SINGLETON_IMPL(ClassName)	J9::Ptr<ClassName> ClassName::pInstance_;

////////////////////////////////////////////////////////////////////////////////
/// \brief		실행시에 자동으로 생성되는 Singleton객체의 구현을 선언하는 매크로
///
///	실행 시점에 초기화 되는 Singleton객체 자신을 class member으로 선언한다.
/// \author		boro
/// \date		2005/04/25
///	\see		STATIC_SINGLETON
////////////////////////////////////////////////////////////////////////////////
#define STATIC_SINGLETON_IMPL(ClassName)	ClassName ClassName::cInstance_;

////////////////////////////////////////////////////////////////////////////////
/// \brief		정적 배열의 원소 갯수를 구하는 매크로
///
/// \author		boro
/// \date		2005/05/16
////////////////////////////////////////////////////////////////////////////////
#define countof(StaticArray)	(sizeof(StaticArray) / sizeof(*StaticArray))

////////////////////////////////////////////////////////////////////////////////
/// \brief		template arguments
///
/// \author		boro
/// \date		2005/05/16
////////////////////////////////////////////////////////////////////////////////
#define TARG1	typename A1
#define TARG2	typename A1, typename A2
#define TARG3	typename A1, typename A2, typename A3
#define TARG4	typename A1, typename A2, typename A3,  typename A4
#define TARG5	typename A1, typename A2, typename A3,  typename A4,  typename A5
#define TARG6	typename A1, typename A2, typename A3,  typename A4,  typename A5, typename A6
#define TARG7	typename A1, typename A2, typename A3,  typename A4,  typename A5, typename A6, typename A7
#define TARG8	typename A1, typename A2, typename A3,  typename A4,  typename A5, typename A6, typename A7, typename A8
#define TARG9	typename A1, typename A2, typename A3,  typename A4,  typename A5, typename A6, typename A7, typename A8, typename A9
#define TARG10	typename A1, typename A2, typename A3,  typename A4,  typename A5, typename A6, typename A7, typename A8, typename A9, typename A10

#define FARG1	const A1& a1
#define FARG2	const A1& a1, const A2& a2
#define FARG3	const A1& a1, const A2& a2, const A3& a3
#define FARG4	const A1& a1, const A2& a2, const A3& a3, const A4& a4
#define FARG5	const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5
#define FARG6	const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6
#define FARG7	const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7
#define FARG8	const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8
#define FARG9	const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9
#define FARG10	const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10
//#define FARG1	typename J9::ArgType<A1>::Type a1
//#define FARG2	typename J9::ArgType<A1>::Type a1, typename J9::ArgType<A2>::Type a2
//#define FARG3	typename J9::ArgType<A1>::Type a1, typename J9::ArgType<A2>::Type a2, typename J9::ArgType<A3>::Type a3
#define PARG1	a1
#define PARG2	a1, a2
#define	PARG3	a1, a2, a3
#define	PARG4	a1, a2, a3, a4
#define	PARG5	a1, a2, a3, a4, a5
#define	PARG6	a1, a2, a3, a4, a5, a6
#define	PARG7	a1, a2, a3, a4, a5, a6, a7
#define	PARG8	a1, a2, a3, a4, a5, a6, a7, a8
#define	PARG9	a1, a2, a3, a4, a5, a6, a7, a8, a9
#define	PARG10	a1, a2, a3, a4, a5, a6, a7, a8, a9, a10

#define _S8(x)	_sc<S8>(x)
#define _U8(x)	_sc<U8>(x)
#define _S16(x)	_sc<S16>(x)
#define _U16(x)	_sc<U16>(x)
#define _S32(x)	_sc<S32>(x)
#define _U32(x)	_sc<U32>(x)
#define _S64(x)	_sc<S64>(x)
#define _U64(x)	_sc<U64>(x)
#define _F32(x)	_sc<F32>(x)
#define _F64(x)	_sc<F64>(x)

////////////////////////////////////////////////////////////////////////////////
/// \brief		warning C4100: 'x' : unreferenced formal parameter워닝을 없애기
///				위한 매크로
///
///				위의 UNUSED 와 같으나 여러개의 변수를 한번에 사용하게 하기위해
///				만들었음. 갯수는 늘려써도 됨
/// \author		shik
/// \date		2009/08/12
////////////////////////////////////////////////////////////////////////////////
template <TARG1> __forceinline void UNUSEDS(FARG1)	{ (void)(a1); }
template <TARG2> __forceinline void UNUSEDS(FARG2)	{ (void)(a1); (void)(a2); }
template <TARG3> __forceinline void UNUSEDS(FARG3)	{ (void)(a1); (void)(a2); (void)(a3); }
template <TARG4> __forceinline void UNUSEDS(FARG4)	{ (void)(a1); (void)(a2); (void)(a3); (void)(a4); }
template <TARG5> __forceinline void UNUSEDS(FARG5)	{ (void)(a1); (void)(a2); (void)(a3); (void)(a4); (void)(a5); }
template <TARG6> __forceinline void UNUSEDS(FARG6)	{ (void)(a1); (void)(a2); (void)(a3); (void)(a4); (void)(a5); (void)(a6); }
template <TARG7> __forceinline void UNUSEDS(FARG7)	{ (void)(a1); (void)(a2); (void)(a3); (void)(a4); (void)(a5); (void)(a6); (void)(a7); }
template <TARG8> __forceinline void UNUSEDS(FARG8)	{ (void)(a1); (void)(a2); (void)(a3); (void)(a4); (void)(a5); (void)(a6); (void)(a7); (void)(a8); }
template <TARG9> __forceinline void UNUSEDS(FARG9)	{ (void)(a1); (void)(a2); (void)(a3); (void)(a4); (void)(a5); (void)(a6); (void)(a7); (void)(a8); (void)(a9); }
template <TARG10>__forceinline void UNUSEDS(FARG10)	{ (void)(a1); (void)(a2); (void)(a3); (void)(a4); (void)(a5); (void)(a6); (void)(a7); (void)(a8); (void)(a9); (void)(a10); }

#define J9_AVOID_LNK4221	namespace { int temp = 0; }

#endif//__J9_COMMON_MACROS_H__
