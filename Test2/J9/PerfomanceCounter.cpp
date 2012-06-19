// $Id: PerfomanceCounter.cpp 4439 2009-11-16 03:56:27Z hyojin.lee $

#include "stdafx.h"
#include "PerfomanceCounter.h"

#include "Application.h"

#include "Log.h"
#include "UtilityString.h"

#include <mmsystem.h>

using namespace J9;

void
SetupPerfomanceCounter()
{
	static std::vector<HANDLE>	cLocked;

	static bool	bInit = false;

	if (bInit)	return;

	HANDLE	hThread = ::GetCurrentThread();
	if (std::find(cLocked.begin(), cLocked.end(), hThread) == cLocked.end())
	{
		cLocked.push_back(hThread);
		SetThreadAffinityMask(hThread, 1);
	}

	bInit = true;
}

////////////////////////////////////////////////////////////////////////////////
// PerfomanceCounter
////////////////////////////////////////////////////////////////////////////////
PerfomanceCounter::PerfomanceCounter() :
	iStart_(0),
	iEnd_(0)
{
	SetupPerfomanceCounter();
}

S64
PerfomanceCounter::GetClock() const
{
	return iEnd_ - iStart_;
}

S64
PerfomanceCounter::GetMilliSecond() const
{
	return GetMilliSecond(GetClock());
}

S64
PerfomanceCounter::GetMilliSecond(S64 iClock)
{
	LARGE_INTEGER	cClock;

	QueryPerformanceFrequency(&cClock);

	if (cClock.QuadPart == 0)
	{
		return 0;
	}

	return (iClock * 1000 / cClock.QuadPart);
}

void
PerfomanceCounter::LogClock(const WChar* pFormat, S64 iMin) const
{
	S64	iClock = GetClock();
	if (iMin <= iClock)
		LOG(pFormat, GetClock());
}

void
PerfomanceCounter::LogMilliSecond(const WChar* pFormat, S64 iMin) const
{
	S64	iMilliSecond = GetMilliSecond();
	if (iMin <= iMilliSecond)
		LOG(pFormat, iMilliSecond);
}

////////////////////////////////////////////////////////////////////////////////
// Profiler
////////////////////////////////////////////////////////////////////////////////
Profiler::Profiler(const WChar* pTitle, U32 iInterval)
: sTitle_(pTitle),
iTotalClockSum_(0),
iTotalClockMax_(S64_MIN),
iTotalClockMin_(S64_MAX),
iCurClockSum_(0),
iCurClockMax_(S64_MIN),
iCurClockMin_(S64_MAX),
iTotalNum_(0),
iCurNum_(0),
iInterval_(iInterval),
iLastUpdated_(0)
{
}

void
Profiler::Add(S64 iClock)
{
	++iCurNum_;
	iCurClockSum_ += iClock;
	iCurClockMax_ = __max(iClock, iCurClockMax_);
	iCurClockMin_ = __min(iClock, iCurClockMin_);

	++iTotalNum_;
	iTotalClockSum_ += iClock;
	iTotalClockMax_ = __max(iClock, iTotalClockMax_);
	iTotalClockMin_ = __min(iClock, iTotalClockMin_);

	if (iInterval_ <= CurTime() - iLastUpdated_)
	{
		StringW	sStr;

		U32		iInterval = CurTime() - iLastUpdated_;
		F64		fSpend = _sc<F64>(PerfomanceCounter::GetMilliSecond(iCurClockSum_));
		F64		fRatio = fSpend * 100.0 / _sc<F64>(iInterval);

		sStr = StringUtil::Format(_W("========================================\r\n")
			_W("Profiler : %s\r\n")
			_W("========================================\r\n")
			_W("Current\r\n")
			_W("count %u, min %I64d, max %I64d, avg %I64d clock %f(ms)\r\n")
			_W("%f%% of clock used in this loop during this reporting period.\r\n")
			_W("Total\r\n")
			_W("count %u, min %I64d, max %I64d, avg %I64d clock %f(ms)\r\n")
			_W("========================================\r\n"),
			sTitle_.c_str(),
			iCurNum_, iCurClockMin_, iCurClockMax_, iCurClockSum_ / iCurNum_,
			_sc<F64>(PerfomanceCounter::GetMilliSecond(iCurClockSum_ / iCurNum_)) / _sc<F64>(iCurNum_),
			fRatio,
			iTotalNum_, iTotalClockMin_, iTotalClockMax_, iTotalClockSum_ / iTotalNum_,
			_sc<F64>(PerfomanceCounter::GetMilliSecond(iTotalClockSum_ / iTotalNum_)) / _sc<F64>(iTotalNum_));

		::OutputDebugStringW(sStr.c_str());

		iCurNum_ = 0;
		iCurClockSum_ = 0;
		iCurClockMax_ = S64_MIN;
		iCurClockMin_ = S64_MAX;

		iLastUpdated_ = CurTime();
	}
}

U32	iTime1 = 0;

void
J9::Perf(const DebugInfo& rDebugInfo)
{
	U32			iTime2 = ::timeGetTime();
	StringW		sLog;

	sLog = J9::StringUtil::Format(_W("%s(%u) - %s : %ums\n"), rDebugInfo.pFileName_, rDebugInfo.iLine_, rDebugInfo.pFuncName_, iTime2 - iTime1);
	::OutputDebugString(sLog.c_str());

	iTime1 = iTime2;
}

void
J9::PerfReset()
{
	iTime1 = ::timeGetTime();
}
