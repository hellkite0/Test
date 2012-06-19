// $Id: Log.h 7835 2011-01-28 06:57:00Z junghyun.bae $

#ifndef __J9_LOG_H__
#define __J9_LOG_H__

#include "Doxygen.h"

namespace J9
{
#ifdef _J9_LOG
	class _Log
	{
		STATIC_SINGLETON(_Log);
	public:
		_Log();

		void			SetDefaultOutput(U32 iOutput)	{ iOutput_ = iOutput; }

		void			SetFileNameBase(const WChar* pBase);
		void			SetFileSplit(U32 iSplit)		{ iFileSplit_ = iSplit; }
		void			SetFileMaxSize(U32 iMaxSize)	{ iFileMaxSize_ = iMaxSize; }
#ifdef _NEOWIZ_LOGGING_POLICY
		void			SetServiceName(const J9::String sName);
		void			SetHostName(const J9::String sName) { sHostName_ = sName; }
#endif

		void			Log(const WChar* pLog, ...);
		void			LogTo(const WChar* pLog, U32 iOutput, ...);
		void			LogWarning(const WChar* pLog, ...);
		void			LogWarningTo(const WChar* pLog, U32 iOutput, ...);
		void			LogAlert(const WChar* pLog, ...);
		void			LogAlertTo(const WChar* pLog, U32 iOutput, ...);
		void			LogCritical(const WChar* pLog, ...);
		void			LogCriticalTo(const WChar* pLog, U32 iOutput, ...);

		void			LogWin32Error(const WChar* pAPIName, U32 iError, const DebugInfo& rWhere);

		void			SetFlush(bool b) { bFlush_ = b; }
		void			Reset();

		void			SetLogWindowHandle(HWND hDlg, U32 iDlgID) { hLogWindow_ = hDlg; iLogWindowID_ = iDlgID; }
		void			LogToDialogWindow(StringW sLog);

	private:
		void			WriteLog(const WChar* pLog, U32 iOutput, va_list cArgList, U32 iLogLevel = _J9_LOG_LEVEL_INFO);
		void			CheckFile(Size iCurLength);
		bool			AskSplit()			const;

	private:
		U32					iOutput_;
		TextFile			cFile_;
		const WChar*		pFileNameBase_;
		U32					iFileSplit_;
		U32					iFileMaxSize_;
		U32					iFileSeq_;

#ifdef _NEOWIZ_LOGGING_POLICY
		J9::StringW			sServiceName_;
		J9::StringW			sHostName_;
#endif

#ifdef _USE_32BIT_TIME_T
		S32					iLastLoggedTime_;
#else
		S64					iLastLoggedTime_;
#endif
		bool				bFlush_;

		HWND				hLogWindow_;
		U32					iLogWindowID_;
	};
#endif
};

#ifdef _J9_LOG
	#define RESET_LOG()					J9::_Log::Instance()->Reset()
	#define SET_LOG_OUTPUT(output)		J9::_Log::Instance()->SetDefaultOutput(output)

	#define SET_LOG_FLUSH(bFlush)		J9::_Log::Instance()->SetFlush(bFlush)
	#define SET_LOG_FILE(str)			J9::_Log::Instance()->SetFileNameBase(str)
	#define SET_LOG_FILE_SPLIT(split)	J9::_Log::Instance()->SetFileSplit(split);
	#define SET_LOG_FILE_SIZE(size)		J9::_Log::Instance()->SetFileMaxSize(size)

	#define _LOG						J9::_Log::Instance()->Log
	#define LOGTO						J9::_Log::Instance()->LogTo
	#define LOG_WIN32_ERROR(api, err)	J9::_Log::Instance()->LogWin32Error(api, err, DEBUG_INFO)

	#define LOGINFO							J9::_Log::Instance()->Log
	#define LOGWARNING						J9::_Log::Instance()->LogWarning
	#define LOGALERT						J9::_Log::Instance()->LogAlert
	#define LOGCRITICAL						J9::_Log::Instance()->LogCritical

#else
	#define RESET_LOG()
	#define SET_LOG_OUTPUT(output)

	#define SET_LOG_FLUSH(bFlush)
	#define SET_LOG_FILE(str)
	#define SET_LOG_FILE_SPLIT(split)
	#define SET_LOG_FILE_SIZE(size)

	#define _LOG						
	#define LOGTO						
	#define LOG_WIN32_ERROR(api, err)


	#define LOGINFO							
	#define LOGWARNING						
	#define LOGALERT						
	#define LOGCRITICAL						

#endif

#define J9_LOG_VERVOSE_LEVEL_NONE		0
#define J9_LOG_VERVOSE_LEVEL_CRITICAL	1
#define J9_LOG_VERVOSE_LEVEL_DEFAULT	2
#define J9_LOG_VERVOSE_LEVEL_DETAIL		3
#define J9_LOG_VERVOSE_LEVEL_ALL		4

#define J9_LOG_VERVOSE_LEVEL			J9_LOG_VERVOSE_LEVEL_DETAIL

// LOG 의 경우 LOG(r, GetClock()) 같은 식의 수행을 했을때, 로그를 찍지 않을경우에
// (r, GetClock()) 에서 GetClock() 이 수행되게 된다..
// 이 경우를 막기 위해 __noop 로 정의 해 보았으나 warning 발생. 다른 방법이 생각나면 수정..-ㅇ-

#if (J9_LOG_VERVOSE_LEVEL_ALL <= J9_LOG_VERVOSE_LEVEL)
	#define	LOG4	_LOG
#else
	#define LOG4
#endif

#if (J9_LOG_VERVOSE_LEVEL_DETAIL <= J9_LOG_VERVOSE_LEVEL)
	#define	LOG3	_LOG
#else
	#define LOG3
#endif

#if (J9_LOG_VERVOSE_LEVEL_DEFAULT <= J9_LOG_VERVOSE_LEVEL)
	#define	LOG2	_LOG
#else
	#define LOG2
#endif

#if (J9_LOG_VERVOSE_LEVEL_CRITICAL <= J9_LOG_VERVOSE_LEVEL)
	#define	LOG1	_LOG
#else
	#define LOG1
#endif

#define LOG	LOG2

#endif//__J9_LOG_H__
