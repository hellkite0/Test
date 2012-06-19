// $Id: PerfomanceCounter.h 4426 2009-11-12 09:51:33Z hyojin.lee $

#ifndef __J9_PERFOMANCE_COUNTER_H__
#define __J9_PERFOMANCE_COUNTER_H__

namespace J9
{
	class PerfomanceCounter
	{
	public:
		PerfomanceCounter();

		void			Start();
		void			End();
		S64				GetMilliSecond() const;
		S64				GetClock() const;

		static S64		GetMilliSecond(S64 iClock);

		void			LogMilliSecond(const WChar* pFormat, S64 iMin = 0) const;
		void			LogClock(const WChar* pFormat, S64 iMin = 0) const;

	private:
		S64				GetCurClock() const;

	private:
		S64				iStart_;
		S64				iEnd_;
	};

	__forceinline void PerfomanceCounter::Start()
	{
		iStart_ = GetCurClock();
	}

	__forceinline void PerfomanceCounter::End()
	{
		iEnd_ = GetCurClock();
	}

	__forceinline S64 PerfomanceCounter::GetCurClock() const
	{
		LARGE_INTEGER	iClock;
		QueryPerformanceCounter(&iClock);
		return iClock.QuadPart;
	}

	class Profiler
	{
	public:
		Profiler(const WChar* pTitle = _W("unnamed"), U32 iInterval = 1000);

		void	Add(S64 iClock);

	private:
		StringW	sTitle_;

		S64		iTotalClockSum_;
		S64		iTotalClockMin_;
		S64		iTotalClockMax_;
		S64		iCurClockSum_;
		S64		iCurClockMax_;
		S64		iCurClockMin_;

		U32		iTotalNum_;
		U32		iCurNum_;
		U32		iInterval_;
		U32		iLastUpdated_;
	};

	void Perf(const DebugInfo& rDebugInfo);
	void PerfReset();
};

#define PERF() J9::Perf(DEBUG_INFO)
#define PERFRESET() J9::PerfReset()

#endif//__J9_PERFOMANCE_COUNTER_H__