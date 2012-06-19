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

		// Assertion fail�� �߻��� �� ����ϴ� ����� �����Ѵ�.
		extern U32	iAssertOutput;

		// Log�� ��� ������ ����� �����Ѵ�.
		extern U32			iLogOut;
		// Log������ �ɰ��� ����� �����Ѵ�.
		extern U32			iLogSplit;
		extern const WChar* pLogDefaultName;		
		extern Size			iMaxLogSize;
		extern bool			bLogFlush;
		extern U32			iLogLevel;

		// dump������ �̸��� �����Ѵ�.
		extern const WChar*		pDumpDefaultName;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		ASSERT�� LOG� ȣ������ ������ �Ѱ��ֱ� ���� ����ü
	///
	///				�� ����ü�� ������ �����ڰ� ����, ����, �Լ����� �޾Ƽ�
	///				�����ϰ� �ִ� ����ü�̴�.
	///				��ũ�θ� ���ؼ� ASSERT�� LOG�� �Ҹ� �� �����Ǿ� ���ڷ� �Ѱ�����
	///				�̶� �ڵ����� ȣ���� ���� ����, ����, �Լ����� ���޵Ǿ� ����.
	///				�� ����ü�� ���� ������ ����� ���� ���� ���� ���̴�.
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
	/// \brief		DebugInfo�� �����ϴ� ��ũ��
	///
	///				DebugInfo�� ���������ִ� ��ũ���̴�.
	///				DebugInfo�� �����ڿ� ����, ����, �Լ����� �Ѱ��ش�.
	/// \author		boro
	/// \date		2005/04/25
	////////////////////////////////////////////////////////////////////////////////
	#define DEBUG_INFO					J9::DebugInfo(_W(__FILE__), __LINE__, _W(__FUNCTION__))

#ifdef _J9_ASSERT
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		��Ÿ�ӿ��� ������ �˻��Ͽ� ������ ������ ��� ����ڿ��� �����Ѵ�.
	///
	///				bCondition�� ������ ��, configuration�� ���ǵ�
	///				#_J9_ASSERT_OUTPUT flag�� ���� ����ڿ��� ���� �߻��� �����Ѵ�.
	/// \param[in]	bCondition		���ǽ�
	/// \param[in]	pExpr			��ũ�ο� ���ؼ� �����Ǵ� ����. bCondition�� �����ϴ� ���ǽ�.
	/// \param[in]	rWhere			��ũ�ο� ���ؼ� �����Ǵ� ����. �Լ��� ȣ���� ���� ����.
	/// \author		boro
	/// \date		2005/04/25
	/// \note		_J9_ASSERT�� ���ǵ� ���� ������ ������
	///				ASSERT(x) ��ũ�θ� ���Ͽ� ���������� ����ϴ� ���� ����.
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
	/// \brief		��Ÿ�ӿ��� ������ �˻��Ͽ� ������ ������ ��� ����ڿ��� �̸� ������ �����޼����� �����Ѵ�.
	///
	///				bCondition�� ������ ��, configuration�� ���ǵ�
	///				#_J9_ASSERT_OUTPUT flag�� ���� ����ڿ��� ���� �߻��� �����Ѵ�.
	/// \param[in]	bCondition		���ǽ�
	/// \param[in]	iLevel			�̸� ������ level���� ���� ������ �޼����� �����Ѵ�.
	/// \param[in]	pFormat			�޼����� ����
	/// \param[in]	...				�޼����� ���ڵ�
	/// \date		2005/09/22
	///	\note		�̰� �̸� �̻ڰ� ������ ���ش�.
	/// \see		_J9_ASSERT_OUTPUT
	////////////////////////////////////////////////////////////////////////////////
	bool	_Check(bool bCondition, CheckLevel iLevel, const WChar* pFormat, ...);

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		��Ÿ�ӿ��� ������ �� �κ��� �޸𸮸� �����Ѵ�.
	///
	/// \param[in]	pException		���� ������
	/// \date		2007/06/22
	/// \see		HANDLE_EXCEPTION, __TRY(), __EXCEPT(), __END_TRY_EXCEPT()
	////////////////////////////////////////////////////////////////////////////////
	void	_CreateMiniDump(EXCEPTION_POINTERS* pException);

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		����� �ϰ� ���� ���� �극��ũ ����Ʈ ���� ���ο� ���� ���� ���� �극��ũ�� �Ǵ�
	///
	/// \date		2009/05/13
	/// \note		���α׷��Ӱ� �ƴ� �Ϲ� ����ڰ� ����� ���� �����޼����� ������� �ʰ�
	///				���α׷��Ӱ� ��������� ������ ���� ���ͷ�Ʈ�� �߻����� �극��ũ�� �Ǵ�
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
	/// \note		MFC�� ASSERT ��ũ�θ� �����Ͽ� ������ �����ϴ� ���� ����� �� ��
	///				|| �����ڸ� ����Ͽ� ������ ���� �� �Ǵ� ��쿡�� DEBUG_INFO����
	///				�����ǵ��� �ϴ� �� ���ɻ��̵��� �ɱ�?
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
	/// \note		CHECK_IGNORE �� ����� ��� ����(IGNORE) �Ͽ����� �ش� ��������
	///             ���ķ� �߻��ϴ� ������ ���õȴ�.
	///             ���� ����� �� ��� ������ �ι� ȣ���ϸ� �ȵ�
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