// $Id: Random.h 6296 2010-07-02 03:59:01Z donghyuk.lee $

#ifndef __J9_RANDOM_H__
#define __J9_RANDOM_H__

namespace J9
{
	class RandomFast
	{
	public:
		explicit	RandomFast();
		explicit	RandomFast(U32 iSeed1, U32 iSeed2, U32 iSeed3);

		void		Seed(U32 iSeed1, U32 iSeed2, U32 iSeed3);

		U32			GetU32();
		U32			GetU32(U32 iMin, U32 iMax);
		U32			GetU32(const U32 (&iRange)[2]) { return GetU32(iRange[0], iRange[1]); }

		U32			GetS32();
		S32			GetS32(S32 iMin, S32 iMax);
		S32			GetS32(const S32 (&iRange)[2]) { return GetS32(iRange[0], iRange[1]); }

		// returns 0.0f~1.0f
		F32			GetF32();
		F32			GetF32(F32 fMin, F32 fMax);
		F32			GetF32(const F32 (&fRange)[2]) { return GetF32(fRange[0], fRange[1]); }

		// overloads
		U32			Get(U32 iMin, U32 iMax) { return GetU32(iMin, iMax); }
		S32			Get(S32 iMin, S32 iMax) { return GetS32(iMin, iMax); }
		F32			Get(F32 fMin, F32 fMax) { return GetF32(fMin, fMax); }
		U32			Get(const U32 (&iRange)[2]) { return GetU32(iRange); }
		S32			Get(const S32 (&iRange)[2]) { return GetS32(iRange); }
		F32			Get(const F32 (&fRange)[2]) { return GetF32(fRange); }

	public:
		static RandomFast	cGlobal;

	private:
		static U32	GetNext1(U32 iSeed);
		static U32	GetNext2(U32 iSeed);
		static U32	GetNext3(U32 iSeed);

	private:
		// 일련 번호
		static U32	iSeed0;
		U32			iSeed1_;
		U32			iSeed2_;
		U32			iSeed3_;
	};

	// MersenneTwister random number generator
	// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/
	// Game Programming Gems 4 ss 2-1
	class RandomMT
	{
	private:
		enum
		{
			TableSize = 624,
		};

	public:
		RandomMT();
		RandomMT(U32 iSeed);
		RandomMT(const U32* piSeed);

		// 몇가지 Seed 함수들
		// 차후 암호화에 사용할 수 있을지도
		void		Seed(U32 iSeed)					{	Seed3(iSeed);	}
		void		Seed1(U32 iSeed);
		void		Seed2(U32 iSeed);
		void		Seed3(U32 iSeed);
		void		Seed4(U32 iSeed);
		void		SeedFromArray(const U32* piSeed);

		U32			GetU32();
		U64			GetU64();

	private:
		void		InvalidateTable();
		U32			RegenerateTable();
		U32			MakeU32(U32 iValue) const;

	private:
		U32			iStates_[TableSize];
		U32*		piNext_;
	};

	typedef RandomFast		Random;

	//#define CHECK_RANDOM_SEQUENCE

	#ifdef CHECK_RANDOM_SEQUENCE
	F32 _RAND_F32(const J9::DebugInfo& rInfo);
	#define RAND_F32() _RAND_F32(DEBUG_INFO)
	U32 _RAND_U32(const J9::DebugInfo& rInfo);
	#define RAND_U32() _RAND_U32(DEBUG_INFO)
	#else
	#define RAND_F32() (J9::Random::cGlobal.GetF32())
	#define RAND_U32() (J9::Random::cGlobal.GetU32())
	#endif

	////////////////////////////////////////////////////////////////////////////////
	// RandomGenerator
	// 발생 확률을 지정해 주면, 해당 확률을 이용하여 랜덤으로 결과를 리턴해 준다
	////////////////////////////////////////////////////////////////////////////////
	template<typename Data>
	class RanGen
	{
	public:
		RanGen() : iSum_(0)	{}

		void		Add(U32 iPortion, const Data& r)
		{
			ASSERT(0 < iPortion);
			iSum_ += iPortion;
			cTable_[iSum_] = r;
		}

		const Data&	Get() const
		{
			return Get(RAND_U32());
		}

		// 사용자가 제공하는 난수발생기를 이용한 난수 발생을 지원하기 위한 함수
		// 난수발생기를 제공해야하지만, 난수발생기에서 제공하는 난수만 입력받아도 되지 않을까?
		const Data&	Get(U32 iRandom) const
		{
			ASSERT(!cTable_.empty() && 0 < iSum_);
			iRandom %= iSum_;
			std::map<U32, Data>::const_iterator	iter = cTable_.upper_bound(iRandom);
			ASSERT(iter != cTable_.end());

			return iter->second;
		}

		inline bool IsEmpty() { return cTable_.empty(); }
		inline void Clear()			{ cTable_.clear(); iSum_ = 0;}

	private:
		U32					iSum_;
		std::map<U32, Data>	cTable_;
	};

	// simple gaussian random
	template<typename Type>
	class SGR
	{
	public:
		SGR()
		{
			cRange_[0] = TypeHelper<Type>::Zero();
			cRange_[1] = TypeHelper<Type>::Zero();
			iNumAccumulation_ = 1;
		}

		Type	Get(Random& rRandom) const
		{
			Type	cValue = TypeHelper<Type>::Zero();
			for (U32 i = 0; i < iNumAccumulation_; ++i)
			{
				cValue += rRandom.Get(cRange_[0], cRange_[1]);
			}

			cValue /= iNumAccumulation_;

			return cValue;
		}

	public:
		// min, max value
		Type	cRange_[2];
		U32		iNumAccumulation_;
	};
};

#endif __J9_RANDOM_H__