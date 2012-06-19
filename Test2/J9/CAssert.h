// $Id: CAssert.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_CASSERT_H__
#define __J9_CASSERT_H__

namespace J9
{
#ifdef _J9_CASSERT
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		CASSERT를 구현하기 위해 필요한 것.
	////////////////////////////////////////////////////////////////////////////////
	template<bool>
	struct CompileTimeError;
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		CASSERT를 구현하기 위해 필요한 것.
	////////////////////////////////////////////////////////////////////////////////
	template<>
	struct CompileTimeError<true> {};
#endif
};

#ifdef _J9_CASSERT
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		컴파일 타임에 확인 가능 한 조건을 체크하는 매크로
	///
	///				컴파일 시점에 확인가능 한 조건들을 체크하여
	///				조건이 참인 경우 문제없이 컴파일이 되지만,
	///				조건이 거짓인 경우 msg에 해당하는 에러메세지를 출력하며.
	///				에러를 발생시킨다.\n
	///				CASSERT(sizeof(int)==sizeof(long), OOPS);\n
	///				과 같은 형태로 사용할 수 있다.
	/// \param[in] expr		조건식
	/// \param[in] msg		조건이 거짓인 경우 발생하는 에러 메세지
	/// \author		boro
	/// \date		2005/04/25
	/// \note		_J9_CASSERT가 정의돼 있을 때에만 동작함
	////////////////////////////////////////////////////////////////////////////////
	#define CASSERT(expr, msg) \
	{ J9::CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }
#else
	#define CASSERT(expr, msg)
#endif

#endif