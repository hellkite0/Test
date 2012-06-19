// $Id: Log.cpp 8340 2011-04-06 06:25:26Z donghyuk.lee $

#include "Stdafx.h"
#include "Log.h"

#include "Time.h"
#include "Socket.h"
#include "UtilityWin32.h"
#include "UtilityString.h"
#include "UtilityFile.h"

using namespace J9;

#ifdef _J9_LOG

enum
{
	MaxSize = S32_MAX,
};

namespace
{
	U32
	MakeFileName(StringW& rName, const WChar* pBase, U32 iSpilit, U32 iMaxSize, U32 iSequence)
	{
		rName = pBase;

		if (iSpilit != _J9_LOG_SPLIT_NONE)
		{
			Time	cTime;
			rName += StringUtil::Format(_W(".%04d%02d%02d%02d%02d%02d")
				, cTime.GetYear(), cTime.GetMonth(), cTime.GetDay()
				, cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
		}
		if (iMaxSize < MaxSize)
		{
			rName += StringUtil::Format(_W(".%04d"), iSequence++);
		}
		rName += _W(".log");

		return iSequence;
	}
};

_Log::_Log()
: iOutput_(Debug::iLogOut),
pFileNameBase_(Debug::pLogDefaultName),
iFileSplit_(Debug::iLogSplit),
iFileMaxSize_(_sc<U32>(Debug::iMaxLogSize)),
iFileSeq_(0),
iLastLoggedTime_(0),
bFlush_(Debug::bLogFlush)
{
}

void
_Log::Reset()
{
	iOutput_ = Debug::iLogOut;
	SetFileNameBase(Debug::pLogDefaultName);
	iFileSplit_ = Debug::iLogSplit;
	iFileMaxSize_ = _sc<U32>(Debug::iMaxLogSize);
	iFileSeq_ = 0;
	iLastLoggedTime_ = 0;
	bFlush_ = Debug::bLogFlush;

}

void
_Log::SetFileNameBase(const WChar* pBase)
{
	if (::wcscmp(pFileNameBase_, pBase))
	{
		if (cFile_.IsOpened())
		{
			cFile_.Close();
		}

		pFileNameBase_ = pBase;
	}
}

void
_Log::WriteLog(const WChar* pLog, U32 iOutput, va_list cArgList, U32 iLogLevel)
{
	StringW	sLog;
	StringUtil::FormatV(sLog, pLog, cArgList);

#ifdef _NEOWIZ_LOGGING_POLICY
	J9::StringW sLogLevel = _W("info");

	if (J9::Debug::iLogLevel == _J9_LOG_LEVEL_NOTHING) return;

	if ((iLogLevel & _J9_LOG_LEVEL_INFO) && (J9::Debug::iLogLevel & _J9_LOG_LEVEL_INFO)) sLogLevel = _W("info");
	else if ((iLogLevel & _J9_LOG_LEVEL_CRITICAL) && (J9::Debug::iLogLevel & _J9_LOG_LEVEL_CRITICAL))  sLogLevel = _W("critical");
	else if ((iLogLevel & _J9_LOG_LEVEL_ALERT) && (J9::Debug::iLogLevel & _J9_LOG_LEVEL_ALERT))  sLogLevel = _W("alert");
	else if ((iLogLevel & _J9_LOG_LEVEL_WARNING) && (J9::Debug::iLogLevel & _J9_LOG_LEVEL_WARNING))  sLogLevel = _W("warn");
	else return;
	
	J9::StringW sDate = StringUtil::Format(_W("%d.%02d.%02d %02d:%02d:%02d.%03d"), Now.GetYear(), Now.GetMonth(), Now.GetDay(), Now.GetHour(), Now.GetMinute(), Now.GetSecond(), Now.GetMSecond());
	sLog = StringUtil::Format(_W("%s,%s,%s,%s,%s"), sHostName_.c_str(), sServiceName_.c_str(), sDate.c_str(), _W("info"), sLog.c_str());
#else
	static bool	bNewLine = true;
	if (bNewLine)
	{
		J9::StringW sDate = StringUtil::Format(_W("%02d%02d %02d:%02d:%02d"), Now.GetMonth(), Now.GetDay(), Now.GetHour(), Now.GetMinute(), Now.GetSecond());
		sLog = StringUtil::Format(_W("%s> %s"), sDate.c_str(), sLog.c_str());
		bNewLine = false;
	}
	if (0 < sLog.size() && sLog.at(sLog.size() - 1) == _W('\n'))
	{
		bNewLine = true;
	}
	UNUSED(iLogLevel);
#endif

	if (sLog.length() > BIG_NUMBER) // max 4k per single line
	{
		sLog = StringUtil::Format(_W("! Too big log [size:%d] : %s\r\n"), sLog.length(), sLog.substr(0, BIG_NUMBER).c_str());
	}

	if (iOutput & _J9_LOG_OUTPUT_STDOUT)
	{
		// console은 wide char 한글을 출력하지 못한다
		// ansi char로 변환해서 출력한다.
		StringA	sAnsi = StringUtil::ToStringA(sLog);
		::fputs(sAnsi.c_str(), stderr);
		//printf("%s", sAnsi.c_str());
	}

	if ((iOutput & _J9_LOG_OUTPUT_DIALOG_WINDOW) && hLogWindow_)
	{
		// display log to dialog window
		LogToDialogWindow(sLog);
	}

	if (iOutput & _J9_LOG_OUTPUT_DEBUGOUT)
	{
		::OutputDebugStringW(sLog.c_str());
	}

	if (iOutput & _J9_LOG_OUTPUT_FILE)
	{
		CheckFile(sLog.length());
		cFile_ << sLog;
		iLastLoggedTime_ = Time().GetTime();
		if (bFlush_)	cFile_.Flush();
	}

	if (iOutput & _J9_LOG_OUTPUT_DB)
	{
	}

	if (iOutput & _J9_LOG_OUTPUT_MESSAGE_BOX)
	{
		::MessageBoxW(NULL, sLog.c_str(), _W("Log"), MB_TASKMODAL | MB_SETFOREGROUND | MB_OK | MB_ICONEXCLAMATION);
	}
}

void
_Log::Log(const WChar* pLog, ...)
{
	va_list cArgList;
	va_start(cArgList, pLog);
	WriteLog(pLog, iOutput_, cArgList, _J9_LOG_LEVEL_INFO);
	va_end(cArgList);
}

void
_Log::LogTo(const WChar* pLog, U32 iOutput, ...)
{
	va_list cArgList;
	va_start(cArgList, iOutput);
	WriteLog(pLog, iOutput, cArgList, _J9_LOG_LEVEL_INFO);
	va_end(cArgList);
}

void
_Log::LogWarning(const WChar* pLog, ...)
{
	va_list cArgList;
	va_start(cArgList, pLog);
	WriteLog(pLog, iOutput_, cArgList, _J9_LOG_LEVEL_WARNING);
	va_end(cArgList);
}

void
_Log::LogWarningTo(const WChar* pLog, U32 iOutput, ...)
{
	va_list cArgList;
	va_start(cArgList, iOutput);
	WriteLog(pLog, iOutput, cArgList, _J9_LOG_LEVEL_WARNING);
	va_end(cArgList);
}

void
_Log::LogAlert(const WChar* pLog, ...)
{
	va_list cArgList;
	va_start(cArgList, pLog);
	WriteLog(pLog, iOutput_, cArgList, _J9_LOG_LEVEL_ALERT);
	va_end(cArgList);
}

void
_Log::LogAlertTo(const WChar* pLog, U32 iOutput, ...)
{
	va_list cArgList;
	va_start(cArgList, iOutput);
	WriteLog(pLog, iOutput, cArgList, _J9_LOG_LEVEL_ALERT);
	va_end(cArgList);
}

void
_Log::LogCritical(const WChar* pLog, ...)
{
	va_list cArgList;
	va_start(cArgList, pLog);
	WriteLog(pLog, iOutput_, cArgList, _J9_LOG_LEVEL_CRITICAL);
	va_end(cArgList);
}

void
_Log::LogCriticalTo(const WChar* pLog, U32 iOutput, ...)
{
	va_list cArgList;
	va_start(cArgList, iOutput);
	WriteLog(pLog, iOutput, cArgList, _J9_LOG_LEVEL_CRITICAL);
	va_end(cArgList);
}


void
_Log::LogWin32Error(const WChar* pAPIName, U32 iError, const DebugInfo& rWhere)
{
	StringW	sFileLine = StringUtil::Format(_W("%s(%u)"), rWhere.pFileName_, rWhere.iLine_);
	StringW	sErrMsg;

	Win32::GetErrorMessage(iError, sErrMsg);

	StringW	sMsg = StringUtil::Format(_W("%s : Error on %s when calling %s, error %s"), sFileLine.c_str(), rWhere.pFuncName_, pAPIName, sErrMsg.c_str());

	Log(sMsg.c_str());
}

void
_Log::CheckFile(Size iCurLength)
{
	if (cFile_.IsOpened() && iFileMaxSize_ < (cFile_.GetPos() + iCurLength * sizeof(WChar)))
		cFile_.Close();

	if (cFile_.IsOpened() && AskSplit())
		cFile_.Close();

	if (!cFile_.IsOpened())
	{
		U32	iFlag = FileOpenWrite | FileOpenCreate | FileOpenNoTrunc;

		StringW	sFileName;

		iFileSeq_ = MakeFileName(sFileName, pFileNameBase_, iFileSplit_, iFileMaxSize_, iFileSeq_);

		StringW sFilePath = pFileNameBase_;
		Size iPos = sFilePath.rfind(_W("\\"));
		if (!FileUtil::Access(sFilePath.c_str()) && iPos != -1)
		{
			FileUtil::CreateDirectory(sFilePath.substr(0, sFilePath.rfind(_W("\\"))));
		}

		cFile_.Open(sFileName.c_str(), iFlag);
	}
}

bool
_Log::AskSplit() const
{
	if (iFileSplit_ == _J9_LOG_SPLIT_NONE)
		return false;

	Time	cCur;
	Time	cLast;

	cLast.SetTime(iLastLoggedTime_);

	if (cLast.GetYear() != cCur.GetYear() && _J9_LOG_SPLIT_MONTHLY <= iFileSplit_)
		return true;

	if (cLast.GetMonth() != cCur.GetMonth() && _J9_LOG_SPLIT_MONTHLY <= iFileSplit_)
		return true;

	if (cLast.GetYearDay() != cCur.GetYearDay() && _J9_LOG_SPLIT_DAILY <= iFileSplit_)
		return true;

	if (cLast.GetHour() != cCur.GetHour() && _J9_LOG_SPLIT_HOURLY <= iFileSplit_)
		return true;

	return false;
}

void
_Log::LogToDialogWindow(StringW sLog)
{
	if (hLogWindow_)
	{
		if (0 < sLog.size() && sLog.at(sLog.size()-1) == _W('\n')) sLog = sLog.substr(0, sLog.size()-1);
		if (0 < sLog.size() && sLog.at(sLog.size()-1) == _W('\r')) sLog = sLog.substr(0, sLog.size()-1);
		SendDlgItemMessage(hLogWindow_, iLogWindowID_, LB_ADDSTRING, 0, (LPARAM)sLog.c_str());
	}

	return;
}

#endif

#ifdef _NEOWIZ_LOGGING_POLICY
void
J9::_Log::SetServiceName( const J9::String sName )
{
	sServiceName_ = J9::StringUtil::ToLower(sName);
}
#endif

