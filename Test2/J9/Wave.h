// $Id: Wave.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_WAVE_H__
#define __J9_WAVE_H__

namespace J9
{
	class Wave
	{
	public:
		Wave();
		virtual ~Wave();

		void		SetMin(F32 f)		{ fMin_ = f; }
		F32			GetMin() const		{ return fMin_; }
		void		SetMax(F32 f)		{ fMax_ = f; }
		F32			GetMax() const		{ return fMax_; }
		void		SetPeriod(U32 i)	{ iPeriod_ = i; }
		U32			GetPeriod() const	{ return iPeriod_; }
		void		SetStart(U32 i)		{ iStart_ = i; }
		U32			GetStart() const	{ return iStart_; }

		F32			Get(U32 iTime);

		// min을 -1.0f, max를 1.0f로, 주기를 1000으로 설정한 후
		// 1 주기동안 파형을 출력한다.
		// 테스트용이나 파형 확인용으로 사용할 수 있다.
		void		TestDump();

	protected:
		// returns [0.0f~1.0f)
		virtual	F32	_Get(F32 fTime) = 0;

		// returns [-1.0f ~ 1.0f]
		F32			GetTimeInPeriod(U32 iTime) const;

	private:
		F32	fMin_;
		F32	fMax_;
		U32	iPeriod_;
		U32	iStart_;
	};

	class Wave_Sine : public Wave
	{
	protected:
		virtual	F32	_Get(F32 fTime);
	};

	class Wave_Cosine : public Wave
	{
	protected:
		virtual	F32	_Get(F32 fTime);
	};

	class Wave_Square : public Wave
	{
	protected:
		virtual	F32	_Get(F32 fTime);
	};

	class Wave_Ramp : public Wave
	{
	protected:
		virtual	F32	_Get(F32 fTime);
	};

	class Wave_Sawtooth : public Wave
	{
	protected:
		virtual	F32	_Get(F32 fTime);
	};

	class Wave_Triangular : public Wave
	{
	protected:
		virtual	F32	_Get(F32 fTime);
	};

	class Wave_Circular : public Wave
	{
	protected:
		virtual	F32	_Get(F32 fTime);
	};
};

#endif//__J9_WAVE_H__
