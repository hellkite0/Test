// $Id: Assert.h 7712 2011-01-20 09:37:47Z hyojin.lee $

#ifndef __J9_ASSERT_H__
#define __J9_ASSERT_H__

#include "Doxygen.h"

namespace J9
{
	namespace Debug
	{
		extern U32	iAssertMessageBoxDeco;
		extern U32	iAssertDebugoutDeco;
		extern U32	iAssertStdErrorDeco;
		extern U32	iAssertLogOutDeco;

		// Assertion fail이 발생할 시 출력하는 방법을 정의한다.
		extern U32	iAssertOutput;

		// Log의 출력 형식을 방법을 정의한다.
		extern U32			iLogOut;
		// Log파일을 쪼개는 방법을 정의한다.
		extern U32			iLogSplit;
		extern const WChar* pLogDefaultName;		
		extern Size			iMaxLogSize;
		extern bool			bLogFlush;
		extern U32			iLogLevel;

		// dump파일의 이름을 정의한다.
		extern const WChar*		pDumpDefaultName;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		ASSERT나 LOG등에 호출자의 정보를 넘겨주기 위한 구조체
	///
	///				이 구조체는 생성시 생성자가 파일, 라인, 함수명을 받아서
	///				저장하고 있는 구조체이다.
	///				매크로를 통해서 ASSERT나 LOG가 불릴 때 생성되어 인자로 넘겨지고
	///				이때 자동으로 호출한 곳의 파일, 라인, 함수명이 전달되어 진다.
	///				이 구조체를 직접 적으로 사용할 일은 거의 없을 것이다.
	/// \author		boro
	/// \date		2005/04/25
	////////////////////////////////////////////////////////////////////////////////
	struct DebugInfo
	{
		const WChar*	pFileName_;
		Size			iLine_;
		const WChar*	pFuncName_;

		DebugInfo(const WChar* pFIleName, Size iLine, const WChar* pFuncName)
			: pFileName_(pFIleName)
			, iLine_(iLine)
			, pFuncName_(pFuncName)
		{
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		DebugInfo를 생성하는 매크로
	///
	///				DebugInfo를 생성시켜주는 매크로이다.
	///				DebugInfo의 생성자에 파일, 라인, 함수명을 넘겨준다.
	/// \author		boro
	/// \date		2005/04/25
	////////////////////////////////////////////////////////////////////////////////
	#define DEBUG_INFO					J9::DebugInfo(_W(__FILE__), __LINE__, _W(__FUNCTION__))

#ifdef _J9_ASSERT
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		런타임에서 조건을 검사하여 조건이 거짓일 경우 사용자에게 보고한다.
	///
	///				bCondition이 거짓일 때, configuration에 정의된
	///				#_J9_ASSERT_OUTPUT flag에 따라 사용자에게 오류 발생을 보고한다.
	/// \param[in]	bCondition		조건식
	/// \param[in]	pExpr			매크로에 의해서 생성되는 인자. bCondition을 결정하는 조건식.
	/// \param[in]	rWhere			매크로에 의해서 생성되는 인자. 함수를 호출한 곳의 정보.
	/// \author		boro
	/// \date		2005/04/25
	/// \note		_J9_ASSERT가 정의돼 있을 때에만 동작함
	///				ASSERT(x) 매크로를 통하여 간접적으로 사용하는 것을 권장.
	/// \see		_J9_ASSERT_OUTPUT
	////////////////////////////////////////////////////////////////////////////////
	void	_Assert(bool bCondition, const WChar* pExpr, const DebugInfo& rWhere);
	void	_Assert(bool bCondition, const AChar* pExpr, const DebugInfo& rWhere);
#endif
	enum CheckLevel
	{
		eCheckAlways,
		eCheckDebug,
		eCheckCustom,
	};
#if defined(_DEBUG) || defined(FAST_DEBUG)
	#define CHECK_LEVEL	eCheckDebug
#else
	#define CHECK_LEVEL	eCheckAlways
#endif
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		런타임에서 조건을 검사하여 조건이 거짓일 경우 사용자에게 미리 정해진 에러메세지를 보고한다.
	///
	///				bCondition이 거짓일 때, configuration에 정의된
	///				#_J9_ASSERT_OUTPUT flag에 따라 사용자에게 오류 발생을 보고한다.
	/// \param[in]	bCondition		조건식
	/// \param[in]	iLevel			미리 정해진 level보다 높은 수준의 메세지만 보고한다.
	/// \param[in]	pFormat			메세지의 포맷
	/// \param[in]	...				메세지의 인자들
	/// \date		2005/09/22
	///	\note		이거 이름 이쁘게 지으면 상준다.
	/// \see		_J9_ASSERT_OUTPUT
	////////////////////////////////////////////////////////////////////////////////
	bool	_Check(bool bCondition, CheckLevel iLevel, const WChar* pFormat, ...);

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		런타임에서 오류가 난 부분의 메모리를 덤프한다.
	///
	/// \param[in]	pException		예외 포인터
	/// \date		2007/06/22
	/// \see		HANDLE_EXCEPTION, __TRY(), __EXCEPT(), __END_TRY_EXCEPT()
	////////////////////////////////////////////////////////////////////////////////
	void	_CreateMiniDump(EXCEPTION_POINTERS* pException);

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		디버깅 하고 있을 때만 브레이크 포인트 설정 여부에 관계 없이 강제 브레이크를 건다
	///
	/// \date		2009/05/13
	/// \note		프로그래머가 아닌 일반 사용자가 사용할 때는 에러메세지를 출력하지 않고
	///				프로그래머가 디버깅으로 실행할 때만 인터럽트를 발생시켜 브레이크를 건다
	////////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG) || defined(FAST_DEBUG)
	void	Break();
#else
	__inline void	Break() {}
#endif

#define HANDLE_EXCEPTION

#ifdef HANDLE_EXCEPTION
	#define __TRY() __try {
	#define __EXCEPT() } __except (J9::_CreateMiniDump(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER ) {
	#define __END_TRY_EXCEPT() }
	#define __FINALLY() } __finally {
	#define __END_TRY_FINALLY() }
#else
	#define __TRY()
	#define __EXCEPT()
	#define __END_TRY_EXCEPT()
	#define __FINALLY()
	#define __END_TRY_FINALLY()
#endif

#ifdef _J9_ASSERT
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		VERIFY
	////////////////////////////////////////////////////////////////////////////////
	#define J9_VERIFY(expr)				J9::_Assert((expr) != 0, _W(#expr), DEBUG_INFO)
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		ASSERT
	/// \note		MFC의 ASSERT 매크로를 참고하여 형식을 수정하는 것을 고려해 볼 것
	///				|| 연산자를 사용하여 조건이 거짓 이 되는 경우에만 DEBUG_INFO등이
	///				생성되도록 하는 게 성능상이득이 될까?
	////////////////////////////////////////////////////////////////////////////////
	//#define J9_ASSERT(expr)			J9::_Assert((expr) != 0, _W(#expr), DEBUG_INFO)
	#define J9_ASSERT(expr)				if (!((expr) != 0)) J9::_Assert((expr) != 0, _W(#expr), DEBUG_INFO)
#else
	#define J9_VERIFY(expr)				(expr)
	#define J9_ASSERT(expr)				(void)(0)
#endif

#ifndef _AFX
	#define VERIFY(expr)				J9_VERIFY(expr)
	#define ASSERT(expr)				J9_ASSERT(expr)
#endif
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		CHECK, CHECK_IGNORE
	/// \note		CHECK_IGNORE 를 사용할 경우 무시(IGNORE) 하였을때 해당 구간에서
	///             이후로 발생하는 오류는 무시된다.
	///             현재 스펙상 한 펑션 내에서 두번 호출하면 안됨
	////////////////////////////////////////////////////////////////////////////////
	#define CHECK						J9::_Check
	#define CHECK_IGNORE				static bool bNoMoreCheck = false; if (!bNoMoreCheck) bNoMoreCheck = J9::_Check

#ifdef _J9_ASSERT
	#define DEBUG_STRING J9::StringUtil::ToStringW(J9::StringUtil::Format("%s (%d)\r\n %s", __FILE__, __LINE__, __FUNCTION__)).c_str()
#else
	#define DEBUG_STRING _W("")
#endif

};

#ifndef ASSUME
#define ASSUME(expr) do { ASSERT(expr); __analysis_assume(!!(expr)); } while(0)
#endif

#endif//__J9_ASSERT_H__