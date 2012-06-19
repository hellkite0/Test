// $Id$

#include "stdafx.h"
#include "Time.h"

#include "UtilityString.h"

#include <sys/timeb.h>
#include <time.h>

using namespace J9;

static Time cBootTime;
static Time cCurrentTime;
static U32 iLastNowAccess;
////////////////////////////////////////////////////////////////////////////////
// Time
// 상당히 문제가 많은 클래스. 사용시 주의 요망.
////////////////////////////////////////////////////////////////////////////////

Time
Time::GetBootTime()
{
	return cBootTime;
}

Time
Time::GetCurrentTime()
{
	if (iLastNowAccess != ::GetTickCount())
	{
		iLastNowAccess = ::GetTickCount();
		cCurrentTime.SyncToSystem();
	}
	return cCurrentTime;
}

Time
Time::GetElapsedFrom(const Time& cTime)
{
	Time cRet = Time() - cTime;
	cRet.UpdateTimeData();
	return cRet;
}

void
Time::Setup()
{
	//cBootTime.SyncToSystem();
	//cCurrentTime.SyncToSystem();
}

Time::Time()
{
	SyncToSystem();
}

void
Time::UpdateTimeData()
{
	struct tm cTm;
	localtime_s(&cTm, &iTime_);
	iYear_ = cTm.tm_year + 1900;
	iMonth_ = cTm.tm_mon + 1;
	iDay_ = cTm.tm_mday;
	iHour_ = cTm.tm_hour;
	iMin_ = cTm.tm_min;
	iSec_ = cTm.tm_sec;
	iYearDay_ = cTm.tm_yday;
	iWeekDay_ = cTm.tm_wday;
}

void
Time::Set(S32 iYear, S32 iMonth, S32 iDay, S32 iHour, S32 iMin, S32 iSec)
{
    struct tm cTm;

    cTm.tm_year = iYear - 1900;
    cTm.tm_mon = iMonth - 1;
    cTm.tm_mday = iDay;
    cTm.tm_hour = iHour;
    cTm.tm_min = iMin;
    cTm.tm_sec = iSec;
    cTm.tm_isdst = -1;

    iTime_ = mktime(&cTm);
    iMTime_ = 0;
	
	UpdateTimeData();
}

void
Time::SyncToSystem()
{
	timeb	cTimeb;

	::ftime(&cTimeb);
	iTime_ = cTimeb.time;

	// this is similar to UpdateTimeData, but don't change it!
	struct tm cTm;
	localtime_s(&cTm, &cTimeb.time);

	iYear_ = cTm.tm_year + 1900;
	iMonth_ = cTm.tm_mon + 1;
	iDay_ = cTm.tm_mday;
	iHour_ = cTm.tm_hour;
	iMin_ = cTm.tm_min;
	iSec_ = cTm.tm_sec;
	iYearDay_ = cTm.tm_yday;
	iWeekDay_ = cTm.tm_wday;
	iMTime_ = cTimeb.millitm;
}

void
Time::Normalize()
{
	 while (iMTime_ >= KILO) {
		iMTime_ -= KILO;
		iTime_ += 1;
    }
}

Time&
Time::operator -= (const Time& rhs)
{
	ASSERT(0 <= iMTime_ && iMTime_ < 1000);
	ASSERT(0 <= rhs.iMTime_ && rhs.iMTime_ < 1000);

	if (iMTime_ < rhs.iMTime_) {
		iMTime_ += KILO;
		iTime_ -= 1;
    }

	iTime_ -= rhs.iTime_;
    return *this;
}

Time
Time::operator - (const Time& rhs)
{
    Time cRet = *this;
    cRet -= rhs;
    return cRet;
}

Time&
Time::operator += (const Time& rhs)
{
    S32 iTime = (iMTime_ + rhs.iMTime_) / KILO;
    S16 iMTime = (iMTime_ + rhs.iMTime_) % KILO;

    iTime_ += (rhs.iTime_ + iTime);
    iMTime_ = iMTime;

    return *this;
}

Time
Time::operator + (const Time& rhs)
{
    Time cRet = *this;
    cRet += rhs;
    return cRet;
}

Time&
Time::operator *= (S32 i)
{
    int iMTime = iMTime_ * i;
    iTime_ *= i;

    iTime_ += (time_t) (iMTime_ / KILO);
    iMTime_ = (S16) (iMTime % KILO);

    return *this;
}

Time
Time::operator * (S32 i)
{
    Time cRet = *this;
    cRet *= i;
    return cRet;
}

bool
Time::operator == (const Time& rhs)
{
	if (iTime_ == rhs.iTime_ && iMTime_ == rhs.iMTime_) return true;
	return false;
}

bool
Time::operator != (const Time& rhs)
{
	return !(operator == (rhs));
}

bool
Time::operator > (const Time& rhs)
{
	if (iTime_ > rhs.iTime_) return true;
	if (iTime_ == rhs.iTime_ && iMTime_ > rhs.iMTime_) return true;

	return false;
}

bool
Time::operator >= (const Time& rhs)
{
	return (operator == (rhs) || operator > (rhs));
}

bool
Time::operator < (const Time& rhs)
{
	return !(operator >= (rhs));
}

bool
Time::operator <= (const Time& rhs)
{
	return !(operator > (rhs));
}

void
Time::Parse(const StringW& sSQLDateTime)
{
	//2006-09-26T10:49:13.840
	S32 iYear, iMonth, iDay, iHour, iMin, iSec;
	swscanf_s(sSQLDateTime.c_str(), _W("%d-%d-%dT%d:%d:%d"), &iYear, &iMonth, &iDay, &iHour, &iMin, &iSec);
	Set(iYear, iMonth, iDay, iHour, iMin, iSec);
}

void
Time::ParseTime(const J9::String& rSQLDateTime)
{
	S32 iYear, iMonth, iDay, iHour, iMin, iSec;
	swscanf_s(rSQLDateTime.c_str(), _W("%04d%02d%02d%02d%02d%02d"), &iYear, &iMonth, &iDay, &iHour, &iMin, &iSec);
	Set(iYear, iMonth, iDay, iHour, iMin, iSec);
}

J9::StringW
Time::ToString()
{
	return J9::StringUtil::Format(_W("%04u-%02u-%02uT%02u:%02u:%02u"), GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond());
}

J9::StringW
Time::ToDBString()
{
	J9::String sCurrentTime = J9::StringUtil::Format(_W("%04u%02u%02u%02u%02u%02u"), GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond());
	if (sCurrentTime.length() != 14)
	{
		LOG(_W("! Error on DBTime Format [%s]\r\n"), sCurrentTime);
		sCurrentTime = sCurrentTime.substr(0, 14);
	}
	return sCurrentTime;
}

const WChar*
Time::GetMonthString() const
{
	switch (GetMonth())
	{
	case 1:		return _W("Jan");
	case 2:		return _W("Feb");
	case 3:		return _W("Mar");
	case 4:		return _W("Apr");
	case 5:		return _W("May");
	case 6:		return _W("Jun");
	case 7:		return _W("Jul");
	case 8:		return _W("Aug");
	case 9:		return _W("Sep");
	case 10:	return _W("Oct");
	case 11:	return _W("Nov");
	case 12:	return _W("Dec");
	default:	ASSERT(0); return _W("");
	}
}

const WChar*
Time::GetWeekDayString() const
{
	switch (GetWeekDay())
	{
	case 0:		return _W("Sun");
	case 1:		return _W("Mon");
	case 2:		return _W("Tue");
	case 3:		return _W("Wed");
	case 4:		return _W("Thu");
	case 5:		return _W("Fri");
	case 6:		return _W("Sat");
	default:	ASSERT(0); return _W("");
	}
}

time_t
J9::Time::GetPerformanceTimer()
{
	time_t count;
	QueryPerformanceCounter((LARGE_INTEGER*)&count);
	return count;
}

S32
J9::Time::GetDeltaTime(time_t iStart, time_t iEnd)
{
	static time_t freq = 0;
	if (freq == 0)
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	time_t elapsed = iEnd - iStart;
	return (S32)(elapsed*1000000 / freq);
}