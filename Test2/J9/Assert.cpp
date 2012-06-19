// $Id: Assert.cpp 8340 2011-04-06 06:25:26Z donghyuk.lee $

#include "Stdafx.h"
#include "Assert.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

#include "Application.h"
#include "UtilityString.h"
#include "UtilityFile.h"
#include "Package.h"
#include "Log.h"
#include "CallStack.h"
#include "Mail.h"
#include "Time.h"
#include "Thread.h"

using namespace J9;

U32	Debug::iAssertMessageBoxDeco = (_J9_ASSERT_DECO_FILE_LINE | _J9_ASSERT_DECO_EXPRESSION | _J9_ASSERT_DECO_FUNC_NAME | _J9_ASSERT_DECO_CALL_STACK);
U32	Debug::iAssertDebugoutDeco = (_J9_ASSERT_DECO_FILE_LINE | _J9_ASSERT_DECO_EXPRESSION | _J9_ASSERT_DECO_FUNC_NAME | _J9_ASSERT_DECO_CALL_STACK);
U32	Debug::iAssertStdErrorDeco = (_J9_ASSERT_DECO_EXPRESSION | _J9_ASSERT_DECO_FUNC_NAME | _J9_ASSERT_DECO_FILE_LINE);
U32	Debug::iAssertLogOutDeco = (_J9_ASSERT_DECO_EXPRESSION | _J9_ASSERT_DECO_FUNC_NAME | _J9_ASSERT_DECO_FILE_LINE);
U32	Debug::iAssertOutput = (_J9_ASSERT_OUTPUT_MESSAGE_BOX | _J9_ASSERT_OUTPUT_DEBUGOUT | _J9_ASSERT_OUTPUT_STDERR | _J9_ASSERT_OUTPUT_LOG | _J9_ASSERT_OUTPUT_DUMP );
U32	Debug::iLogOut = (_J9_LOG_OUTPUT_STDOUT | _J9_LOG_OUTPUT_DEBUGOUT | _J9_LOG_OUTPUT_FILE | _J9_LOG_OUTPUT_DB | _J9_LOG_OUTPUT_DIALOG_WINDOW);
U32	Debug::iLogSplit = (_J9_LOG_SPLIT_DAILY);
U32 Debug::iLogLevel = (_J9_LOG_LEVEL_EVERYTHING);

Size			Debug::iMaxLogSize = S32_MAX;
bool			Debug::bLogFlush = false;
const WChar*	Debug::pLogDefaultName = _W("Just9");
const WChar*	Debug::pDumpDefaultName = _W("MiniDump");

MINIDUMP_TYPE
GetDefaultDumpType(const J9::StringW& rProfile)
{
	MINIDUMP_TYPE cMiniDumpType = (MINIDUMP_TYPE) (MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpFilterMemory | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules );

	if (rProfile == _W("just9.xml"))
	{
		return cMiniDumpType;
	}
	else if (rProfile == _W("just9_internal.xml"))
	{
		cMiniDumpType = (MINIDUMP_TYPE) (cMiniDumpType | MiniDumpWithFullMemory);
		return cMiniDumpType;
	}
	else if (rProfile == _W("kru.xml"))
	{
		return cMiniDumpType;
	}
	else if (rProfile == _W("kru_internal.xml"))
	{
		return cMiniDumpType;
	}

	ASSERT(0);
	return cMiniDumpType;
}

void
J9::_CreateMiniDump(EXCEPTION_POINTERS* pException)
{
#ifdef _SERVER
	J9::PThread::bShutdown = true;	
#else
	static bool bChecked = false;

	if (bChecked)
		return;

	bChecked = true;
#endif

	J9::String sFileName;
	J9::Time	cTime;


	StringW sFilePath = J9::Debug::pDumpDefaultName;
	Size iPos = sFilePath.rfind(_W("\\"));
	if (!FileUtil::Access(sFilePath.c_str()) && iPos != -1)
	{
		FileUtil::CreateDirectory(sFilePath.substr(0, sFilePath.rfind(_W("\\"))));
	}
	HANDLE hFile = NULL;
	U32 iCount = 0;
	while (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
	{
		sFileName = StringUtil::Format(_T("%s.%04d%02d%02d%02d%02d%02d_%02d.%s"), J9::Debug::pDumpDefaultName, cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), iCount, _T("dmp"));
		hFile = CreateFile( sFileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if (iCount++ > 10) break;
	}
	
	if (( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ))
	{
		MINIDUMP_EXCEPTION_INFORMATION cMDEI;
		cMDEI.ThreadId = GetCurrentThreadId();
		cMDEI.ExceptionPointers = pException;
		cMDEI.ClientPointers = false;

#ifdef _SERVER		
		MINIDUMP_TYPE cMiniDumpType = (MINIDUMP_TYPE) (MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpFilterMemory | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules );
#else
		VERIFY(PackageManager::HasInstance());

		XMLNode	cProfile = pApp()->GetProfile();
		XMLNode cNode = cProfile.GetChild(_W("dump"));
		MINIDUMP_TYPE cMiniDumpType = (MINIDUMP_TYPE) (MiniDumpWithHandleData | MiniDumpFilterMemory | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules );

		if (cNode.IsNull())
		{
			cMiniDumpType = (MINIDUMP_TYPE) (cMiniDumpType | MiniDumpWithFullMemory);
		}
		else
		{
			U8 i;
			StringW sType = cNode.GetData();
			StringUtil::Convert(sType, i);

			switch (i)
			{
			case 0: // 기본설정(풀덤프)
			case 3: // 풀덤프
				cMiniDumpType = (MINIDUMP_TYPE) (cMiniDumpType | MiniDumpWithFullMemory);
				break;

			case 1:
				CloseHandle(hFile);
				return; //안남긴다.

			case 2:
				break; // 미니덤프
			}
		}
#if defined(_DEBUG) || defined(FAST_DEBUG)
		cMiniDumpType = (MINIDUMP_TYPE) (MiniDumpWithHandleData | MiniDumpFilterMemory | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules | MiniDumpWithFullMemory);
#endif
#endif
		if(!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, cMiniDumpType, (pException != 0) ? &cMDEI: 0, 0, 0 ))
		{
			LOG(_W("MiniDumpWriteDump failed. Error: %u \r\n"), GetLastError());
		}
		else
		{
			LOG(_W("Minidump [%s] created.\r\n"), sFileName.c_str());			
		}
		CloseHandle(hFile);
	}
	else
	{
		LOG(_W("CreateFile failed. Error: %u \r\n"), GetLastError() );
	}

#ifdef _SERVER
	// 서버에서는 유저 후처리등의 crash handle 을 하고 죽이도록 한다.
		/*if (pApp()->IsDebuggerPresent())
		__debugbreak();*/		
#else
	abort();
#endif
}

#ifdef _J9_ASSERT
void
MakeAssertMessage(U32 iFlag, StringW& rMsg, const WChar* pExpr, const J9::DebugInfo& rWhere, const StringW& rCallStack)
{
	rMsg.clear();

	rMsg = _W("Assertion failed\r\n");
	if (iFlag & _J9_ASSERT_DECO_FILE_LINE)
		rMsg += StringUtil::Format(_W("%s(%u)\r\n"), rWhere.pFileName_, rWhere.iLine_);

	if (iFlag & _J9_ASSERT_DECO_EXPRESSION)
		rMsg += StringUtil::Format(_W("Expression : %s\r\n"), pExpr);

	if (iFlag & _J9_ASSERT_DECO_FUNC_NAME)
		rMsg += StringUtil::Format(_W("Function : %s()\r\n"), rWhere.pFuncName_);

	if (iFlag & _J9_ASSERT_DECO_CALL_STACK)
	{
		rMsg += _W("<Call Stack>\r\n");
		rMsg += rCallStack;
	}
}

void
J9::_Assert(bool bCondition, const WChar* pExpr, const DebugInfo& rWhere)
{
	if (!bCondition)
	{
		CallStack	cCallStack;
		StringW		sCallStack;
		StringW		sMsg;

		cCallStack.Capture();
		cCallStack.ExcludeIntermidate();
		cCallStack.Dump(sCallStack);

		if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_MAIL)
		{
			MakeAssertMessage(Debug::iAssertMessageBoxDeco, sMsg, pExpr, rWhere, sCallStack);
			J9::Mail::SendAlert(sMsg);
		}

		if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_DEBUGOUT)
		{
			MakeAssertMessage(Debug::iAssertDebugoutDeco, sMsg, pExpr, rWhere, sCallStack);
			::OutputDebugStringW(sMsg.c_str());
		}

		if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_STDERR)
		{
			MakeAssertMessage(Debug::iAssertStdErrorDeco, sMsg, pExpr, rWhere, sCallStack);
			::fputws(sMsg.c_str(), stderr);
		}

		if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_LOG)
		{
			MakeAssertMessage(Debug::iAssertLogOutDeco, sMsg, pExpr, rWhere, sCallStack);

			LOGTO(_W("%s"), _J9_LOG_OUTPUT_FILE, sMsg.c_str());
		}		

		if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_MESSAGE_BOX)
		{
			S32			iReturn;

			MakeAssertMessage(Debug::iAssertMessageBoxDeco, sMsg, pExpr, rWhere, sCallStack);
			iReturn = ::MessageBoxW(NULL, sMsg.c_str(), _W("Assertion Failed"), MB_TASKMODAL|MB_SETFOREGROUND|MB_ABORTRETRYIGNORE|MB_ICONERROR);

			if (iReturn == IDIGNORE)
			{
				// do nothing
			}
			else if (iReturn == IDRETRY)
			{
				__debugbreak();
			}
			else//iReturn == IDABORT
			{
				if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_DUMP)
				{
					RaiseException(0, 0, 0, NULL);
				}
				else
				{
					::PostQuitMessage(-1);
					::ExitProcess(U32_MAX);
				}
			}
		}

		if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_DUMP)
		{
			if (Application::HasInstance() && pApp()->IsDebuggerPresent())
				__debugbreak();
			else
				RaiseException(0, 0, 0, NULL);
		}

		if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_KILL)
		{
			abort();
		}
	}
}

void
J9::_Assert(bool bCondition, const AChar* pExpr, const DebugInfo& rWhere)
{
	_Assert(bCondition, J9::StringUtil::ToStringW(pExpr).c_str(), rWhere);
}

#endif

bool
J9::_Check(bool bCondition, CheckLevel iLevel, const WChar* pFormat, ...)
{
	if (bCondition)				return false;
	if (CHECK_LEVEL < iLevel)	return false;

	StringW	sMsg;

	va_list cArgList;
	va_start(cArgList, pFormat);
	StringUtil::FormatV(sMsg, pFormat, cArgList);
	va_end(cArgList);

	if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_DEBUGOUT)
	{
		::OutputDebugStringW(sMsg.c_str());
		// line break in debug out
		::OutputDebugStringW(_W("\r\n"));
	}

	if (Debug::iAssertOutput &_J9_ASSERT_OUTPUT_STDERR)
	{
		::fputws(sMsg.c_str(), stderr);
	}

	if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_MESSAGE_BOX)
	{
		S32			iReturn;

		iReturn = ::MessageBoxW(NULL, sMsg.c_str(), _W("Assertion Failed"), MB_TASKMODAL|MB_SETFOREGROUND|MB_ABORTRETRYIGNORE|MB_ICONERROR);

		if (iReturn == IDIGNORE)
		{
			// do nothing
			return true;
		}
		else if (iReturn == IDRETRY)
		{
			__debugbreak();
		}
		else//iReturn == IDABORT
		{
			if (Debug::iAssertOutput & _J9_ASSERT_OUTPUT_DUMP)
			{
				RaiseException(0, 0, 0, NULL);
			}
			else	
			{
				::PostQuitMessage(-1);
				::ExitProcess(U32_MAX);
			}
		}
	}
	return false;
}

#if defined(_DEBUG) || defined(FAST_DEBUG)
void
J9::Break()
{
	if (Application::HasInstance() && pApp()->IsDebuggerPresent())
		__debugbreak();
}
#endif

//#include <dbghelp.h>
//void
//J9::CreateMiniDump(EXCEPTION_POINTERS* pException)
//{
//	J9::String sFileName;
//	J9::Time	cTime;
//	sFileName = StringUtil::Format(_W("%s.%04d%02d%02d%02d%02d%02d.%s"), _W("Minidump"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), _W("dmp"));
//
//	HANDLE hFile = CreateFile( sFileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
//
//	if (( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ))
//	{
//		MINIDUMP_EXCEPTION_INFORMATION cMDEI;
//		cMDEI.ThreadId = GetCurrentThreadId();
//		cMDEI.ExceptionPointers = pException;
//		cMDEI.ClientPointers = false;
//		//MINIDUMP_TYPE cMDT = (MINIDUMP_TYPE) (MiniDumpWithFullMemory | MiniDumpWithHandleData |  MiniDumpFilterMemory | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules );
//		MINIDUMP_TYPE cMDT = (MINIDUMP_TYPE) (MiniDumpWithHandleData |  MiniDumpFilterMemory | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules );
//		if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, cMDT, (pException != 0) ? &cMDEI: 0, 0, 0 ))
//		{
//			LOG(_W("MiniDumpWriteDump failed. Error: %u \r\n"), GetLastError());
//		}
//		else
//		{
//			LOG(_W("Minidump created.\r\n"));
//		}
//		CloseHandle(hFile);
//	}
//	else
//	{
//		LOG(_W("CreateFile failed. Error: %u \r\n"), GetLastError() );
//	}
//	abort();
//}
