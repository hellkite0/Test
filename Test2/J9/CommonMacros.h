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
/// \brief		Ŭ�������� �ش� ����� ��ġ�� �˷��ִ� ��ũ��
///
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define OFFSET_OF(c, x)	(_rc<Size>(&_rc<Dummy&>((_sc<c*>(0)->x))))

////////////////////////////////////////////////////////////////////////////////
/// \brief		__FILE__LINE__ ��ũ�θ� ����ϱ� ���� �ʿ��� ��ũ��.
///
///				�ڼ��� ������ ���ؼ� �ñ��� ����� ���� ������ ������.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define _QUOTE(x)		#x

////////////////////////////////////////////////////////////////////////////////
/// \brief		__FILE__LINE__ ��ũ�θ� ����ϱ� ���� �ʿ��� ��ũ��.
///
///				�ڼ��� ������ ���ؼ� �ñ��� ����� ���� ������ ������.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define QUOTE(x)		_QUOTE(x)

////////////////////////////////////////////////////////////////////////////////
/// \brief		��ó���⿡�� filename.cpp(30) : �� ���� ���·� ����� �ֱ� ����
///				��ũ��
///
///				to do�� note, warning, error �޼��� ���� ����â�� ����ϴµ�
///				����� �� �ִ�.
/// \author		boro
/// \date		2005/04/25
/// \warning	�� ��ũ�θ� ���� ���� ���Ŀ� ���� �Ǵ� �Ϳ� ����ϸ�
///				��� ������ ����� ������ �ʴ´�.
///				�ݵ�� #pragma message() ������ �뵵�� ����� ��
////////////////////////////////////////////////////////////////////////////////
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define __FILE_LINE__ __FILE__#__LINE__

////////////////////////////////////////////////////////////////////////////////
/// \brief		warning C4100: 'x' : unreferenced formal parameter������ ���ֱ�
///				���� ��ũ��
///
///				�Լ��� ���ڸ� ������� �ʴ� ���� Ȯ���� �� warning C4100��
///				���ֱ� ���Ͽ� ��������� ���Ǵ� ��ũ�� �̴�.
///				���� �Լ��� ���� ���̵� �Ͽ� ������ ����,
///				���Ŀ� ���� ���� ����Ͽ� ����� ������ �ʴ� �Լ� ���ڸ�
///				������ �������� �Լ� �� ������ �ϴ� ���,
///				API callback ������ ����ϱ� ���ؼ� ������ �ʴ� ���ڸ�
///				������ �Լ� �� ������ �ϴ� ��� ����Ѵ�.
///				��Ģ������ ������� �ʴ� ���ڴ� �Լ� �����𿡼� ���ִ� ����
///				���������� ��ġ ���� ��쿡 ����Ѵ�.
///				���� ��Ȯ�� ��� ������ ������ ���� �����Ѵ�.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
#define J9_UNUSED(x)		(void)(x)
#ifndef _AFX
	#define UNUSED(x)			J9_UNUSED(x)
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief		���� �����ڿ� ���� �����ڸ� ����� ��ü ������ �����ϴ� ��ũ��.
///				Ŭ���� ����ο��� ����Ѵ�.
///
/// \author		Hyuk
/// \date		2010/05/07
////////////////////////////////////////////////////////////////////////////////
#define DISABLE_COPY_CREATOR(ClassName) \
	private: \
		ClassName(const ClassName&); \
		ClassName& operator =(const ClassName&); \

////////////////////////////////////////////////////////////////////////////////
/// \brief		std container ����, �ش� container ���θ� iterating �ϴ� ��ũ��
///	
///	for �ȿ����� ��ȿ��, iterator �� �����ϰ� for ������ ����� �ش�.
/// std::map<F32,U32> ���� �ָ� ���ԵǸ�.. , �� �������� �и� �� ������..-_-;
/// ���� typedef ���� ������ ����� �� ���� ��찡 �ִ�.
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
/// \brief		����ڰ� ���� �����ؾ� �ϴ� Singleton ��ü���� �����ϰ�,
///				Singleton���� ���Ǵ� static function���� �����ϴ� ��ũ��
///
///	��ü �ȿ��� ��������μ�, �ش� ��ü���� Singleton Interface�� �����Ѵ�.
///	�� �������� ����� Singleton�� ����ڰ� CreateInstance()�� ȣ���Ͽ�
///	��������� ������ �־�� �ϰ�, DestroyInstance()�� ����Ͽ� �ı��� �־�� �Ѵ�.
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
/// \brief		����ÿ� �ڵ����� �ʱ�ȭ �Ǵ� ���� ��ü �������� Singleton�� �����ϰ�
///				Singleton���� ���Ǵ� static function���� �����ϴ� ��ũ��
///
///	��ü �ȿ��� ��������μ�, �ش� ��ü���� Singleton Interface�� �����Ѵ�.
///	����ڰ� ��������� ����, �ı��� �� �� �ʿ䰡 ���� ��ü�� ���� ���ȴ�.
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
/// \brief		����ڿ� ���� �����Ǵ� Singleton��ü�� ������ �����ϴ� ��ũ��
///
///	Singleton��ü�� ������ ��ü���� ������ �����ϱ� ���� class member��
///	�ڽ��� �����͸� ������. class member�� class �ܺο��� �ٽ��ѹ� ���ǵǾ�߸�
///	�ڽ��� �ּҸ� Ȯ���� �� �ִ�. �� �ּ� ������ Ȯ���ϱ� ���ؼ� class �ܺο���
///	�� ��ũ�θ� ����� �־�� �Ѵ�.
/// \author		boro
/// \date		2005/04/25
///	\see		DYNAMIC_SINGLETON
////////////////////////////////////////////////////////////////////////////////
#define DYNAMIC_SINGLETON_IMPL(ClassName)	J9::Ptr<ClassName> ClassName::pInstance_;

////////////////////////////////////////////////////////////////////////////////
/// \brief		����ÿ� �ڵ����� �����Ǵ� Singleton��ü�� ������ �����ϴ� ��ũ��
///
///	���� ������ �ʱ�ȭ �Ǵ� Singleton��ü �ڽ��� class member���� �����Ѵ�.
/// \author		boro
/// \date		2005/04/25
///	\see		STATIC_SINGLETON
////////////////////////////////////////////////////////////////////////////////
#define STATIC_SINGLETON_IMPL(ClassName)	ClassName ClassName::cInstance_;

////////////////////////////////////////////////////////////////////////////////
/// \brief		���� �迭�� ���� ������ ���ϴ� ��ũ��
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
/// \brief		warning C4100: 'x' : unreferenced formal parameter������ ���ֱ�
///				���� ��ũ��
///
///				���� UNUSED �� ������ �������� ������ �ѹ��� ����ϰ� �ϱ�����
///				�������. ������ �÷��ᵵ ��
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
