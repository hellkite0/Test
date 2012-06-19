// $Id$

#ifndef __J9_TIME_H__
#define __J9_TIME_H__

namespace J9
{
	// _USE_32BIT_TIME_T 가 define 되어 있다면 time_t 는 S32
	// 그렇지 않다면 time_t 는 S64
	class Time
	{
	public:
		Time();

		static void		Setup();
		static Time		GetBootTime();
		static Time		GetCurrentTime();
		static Time		GetElapsedFrom(const Time& cTime);

		void	SyncToSystem();
		void	UpdateTimeData();
		void	Parse(const J9::StringW& sSQLDateTime);
		void	ParseTime(const J9::String& rSQLDateTime);
		void	Set(S32 iYear, S32 iMonth, S32 iDay, S32 iHour, S32 iMin, S32 iSec);

		void	SetTime(time_t iTime)		{ iTime_ = iTime; UpdateTimeData(); }
		
		virtual  U32	GetYear() const		{ return iYear_; }
		virtual  U32	GetMonth() const	{ return iMonth_; }
		virtual  U32	GetDay() const		{ return iDay_; }
		inline U32	GetHour() const		{ return iHour_; }
		inline U32	GetMinute() const	{ return iMin_; }
		inline U32	GetSecond()	const	{ return iSec_; }
		inline U32	GetMSecond() const	{ return iMTime_; }

		inline U32	GetYearDay() const	{ return iYearDay_; }
		inline U32	GetWeekDay() const	{ return iWeekDay_; }

		time_t		GetTime() const		{ return iTime_; }
		U32			GetTotalMSec() const	{ return _sc<U32>(iTime_ * KILO + iMTime_); }

		static time_t	GetPerformanceTimer();
		static S32		GetDeltaTime(time_t iStart, time_t iEnd);

		J9::StringW	ToString();
		J9::StringW	ToDBString();

		Time&		operator -= (const Time& rhs);
		Time 		operator - (const Time& rhs);
		Time&		operator += (const Time& rhs);
		Time 		operator + (const Time& rhs);
		Time&		operator *= (S32 i);
		Time 		operator * (S32 i);
		bool		operator == (const Time& rhs);
		bool		operator != (const Time& rhs);
		bool		operator > (const Time& rhs);
		bool		operator >= (const Time& rhs);
		bool		operator < (const Time& rhs);
		bool		operator <= (const Time& rhs);

		const WChar*	GetWeekDayString() const;
		const WChar*	GetMonthString() const;

	private:
		void		Normalize();

	private:
		time_t		iTime_;
		S16			iMTime_;

		U32			iYear_;
		U32			iMonth_;
		U32			iDay_;
		U32			iHour_;
		U32			iMin_;
		U32			iSec_;
		U32			iYearDay_;
		U32			iWeekDay_;
	};
}

#define Now			(J9::Time::GetCurrentTime())
#define BootMSec	(J9::Time::GetElapsedFrom(J9::Time::GetBootTime())).GetTotalMSec()

#endif//__J9_TIME_H__