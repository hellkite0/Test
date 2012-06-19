// $Id: Random.cpp 8060 2011-02-14 01:24:09Z donghyuk.lee $

#include "stdafx.h"
#include "Random.h"

//#include "Log.h"
#include "UtilityMath.h"
#include "UtilityRandom.h"

using namespace J9;

RandomFast::RandomFast()
{
	Seed(::GetTickCount(), ++iSeed0, ::GetCurrentProcessId());
}

RandomFast::RandomFast(U32 iSeed1, U32 iSeed2, U32 iSeed3)
{
	Seed(iSeed1, iSeed2, iSeed3);
}

void
RandomFast::Seed(U32 iSeed1, U32 iSeed2, U32 iSeed3)
{
	iSeed1_ = (iSeed1 ^ 0xa7e25f61) | 0x00100000;
	iSeed2_ = (iSeed2 ^ 0xc87b051a) | 0x00001000;
	iSeed3_ = (iSeed3 ^ 0x51a8f93b) | 0x00000010;
}

U32
RandomFast::GetU32()
{
	//return (iSeed1_ = GetNext1(iSeed1_))
	//	^ (iSeed2_ = GetNext2(iSeed2_))
	//	^ (iSeed3_ = GetNext3(iSeed3_));

	U32 iResult = (iSeed1_ = GetNext1(iSeed1_)) ^ (iSeed2_ = GetNext2(iSeed2_)) ^ (iSeed3_ = GetNext3(iSeed3_));

	return iResult;
}

F32
RandomFast::GetF32()
{
	//return _sc<F32>(GetU32()) / _sc<F32>(_sc<U32>(U32_MAX));
	F32 fResult = _sc<F32>(GetU32()) / _sc<F32>(_sc<U32>(U32_MAX));

	return fResult;
}

U32
RandomFast::GetU32(U32 iMin, U32 iMax)
{
	ASSERT(iMax != U32_MAX);
	if (iMax == iMin)
		return iMax;

	U32 iResult = (GetU32() % (iMax + 1 - iMin)) + iMin;

	return iResult;
}


S32
RandomFast::GetS32(S32 iMin, S32 iMax)
{
	ASSERT(iMax != S32_MAX);
	if (iMax == iMin)
		return iMax;

	S32 iResult = _sc<S32>(GetU32() % _sc<U32>(iMax + 1 - iMin)) + iMin;

	return iResult;
}

F32
RandomFast::GetF32(F32 fMin, F32 fMax)
{
	F32 fResult = Lerp<F32>(fMin, fMax, GetF32());

	return fResult;
}

inline U32
RandomFast::GetNext1(U32 iSeed)
{
	return (iSeed & ~0x1) << 12 ^ ((iSeed << 13) ^ iSeed) >> 19;
}

inline U32
RandomFast::GetNext2(U32 iSeed)
{
	return (iSeed & ~0x7) << 4 ^ ((iSeed << 2) ^ iSeed) >> 25;
}

inline U32
RandomFast::GetNext3(U32 iSeed)
{
	return (iSeed & ~0xfUL) << 17 ^ ((iSeed << 3) ^ iSeed) >> 11;
}

RandomMT::RandomMT()
{
	Seed1(::GetTickCount());
}

RandomMT::RandomMT(U32 iSeed)
{
	Seed1(iSeed);
}

void
RandomMT::Seed1(U32 iSeed)
{
	// 이건 nevermind가 Z에서 퍼온 Seed만드는 함수
	register U32	iX;
	register U32*	piS = iStates_;
	register U32	iJ;

	iX = (iSeed | 1) & 0xFFFFFFFF;
	*piS++ = iX;
	for (iJ = TableSize; --iJ; *piS++ = (iX *= 69069) & 0xFFFFFFFF)
	{
		// do nothing
	}

	InvalidateTable();
}

void
RandomMT::Seed2(U32 iSeed)
{
	// 이건 boro가 Z에서 퍼온 Seed만드는 함수
	register U32*	piS = iStates_;
	for (register U32 i = TableSize;
		i--;
		*piS    = iSeed & 0xffff0000,
		*piS++ |= ((iSeed *= 69069)++ & 0xffff0000) >> 16,
		(iSeed *= 69069)++)
	{
	}

	InvalidateTable();
}

void
RandomMT::Seed3(U32 iSeed)
{
	// 이건 MersenneTwister 공식 홈페이지에서 퍼온 Seed만드는 함수
	iStates_[0]= iSeed;

	register U32	i;
	for (i = 1; i < TableSize; ++i)
	{
		// See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
		// In the previous versions, MSBs of the seed affect
		// only MSBs of the array mt[].
		// 2002/01/09 modified by Makoto Matsumoto
		iStates_[i] = (1812433253 * (iStates_[i-1] ^ (iStates_[i-1] >> 30)) + i);

		// for >32 bit machines
		//iStates_[i] &= 0xffffffffUL;
	}

	InvalidateTable();
}

void
RandomMT::Seed4(U32 iSeed)
{
	// 요건 모든 시드를 0xffffffff / TableSize의 간격으로 균일하게 설정하는 함수

	// C4100 warning를 피하기 위해서
	++iSeed;
	register U32	i;
	for (i = 0; i < TableSize; ++i)
	{
		iStates_[i]= 0xffffffff / TableSize * i;
	}

	InvalidateTable();
}

void
RandomMT::SeedFromArray(const U32* piSeed)
{
	// Seed the generator with an array of U32 uint32's
	// There are 2^19937-1 possible initial states.  This function allows
	// any one of those to be chosen by providing 19937 bits.  The lower
	// 31 bits of the first element, _dwSeeds[0], are discarded.  Any bits
	// above the lower 32 in each element are also discarded.  Theoretically,
	// the rest of the array can contain any values except all zeroes.

	register U32*		piS = iStates_;
	register const U32*	piB = piSeed;
	register S32		i = TableSize;
	for (; i--; *piS++ = *piB++ & 0xffffffff) {}

	InvalidateTable();
}

U32
RandomMT::MakeU32(U32 iValue) const
{
	iValue ^= (iValue >> 11);
	iValue ^= (iValue <<  7) & 0x9D2C5680;
	iValue ^= (iValue << 15) & 0xEFC60000;
	return (iValue ^ (iValue >> 18));
}

void
RandomMT::InvalidateTable()
{
	// 다음 GetU32가 호출 될 때 테이블을 Regenerate하게 해 준다
	// piNext_값을 테이블의 맨 끝으로 설정한다
	piNext_ = iStates_ + TableSize;
}

U32
RandomMT::RegenerateTable()
{
	#define HI_BIT(fU_)			((fU_) & 0x80000000)			// mask all but highest   bit of u
	#define LO_BIT(fU_)			((fU_) & 0x00000001)			// mask all but lowest    bit of u
	#define LO_BITS(fU_)			((fU_) & 0x7FFFFFFF)			// mask     the highest   bit of u
	#define MIX_BITS(fU_, fV_)		(HI_BIT(fU_)|LO_BITS(fV_))		// move hi bit of u to hi bit of v
	#define MTKEY				(0x9908B0DF)

	register U32*	p0 = iStates_;
	register U32*	p2 = iStates_ + 2;
	register U32*	pM = iStates_ + 397;
	register U32	s0;
	register U32	s1;
	register S32	j;

	s0 = iStates_[0];
	s1 = iStates_[1];
	for (j = 228; --j;)
	{
		*p0++ = *pM++ ^ (MIX_BITS(s0, s1) >> 1) ^ (LO_BIT(s1) ? MTKEY : 0);

		s0 = s1;
		s1 = *p2++;
	}

	pM = iStates_;
	for (j = 397; --j;)
	{
		*p0++ = *pM++ ^ (MIX_BITS(s0, s1) >> 1) ^ (LO_BIT(s1) ? MTKEY : 0);
		s0 = s1;
		s1 = *p2++;
	}

	s1 = iStates_[0];
	*p0 = *pM ^ (MIX_BITS(s0, s1) >> 1) ^ (LO_BIT(s1) ? MTKEY : 0);

	return MakeU32(s1);
}

U32
RandomMT::GetU32()
{
	if (piNext_ == iStates_ + TableSize)
	{
		// Reload에서 이미 하나의 난수를 생성하여 리턴할 것이므로
		piNext_ = iStates_ + 1;

		// GetU32()와 Reload의 return type이 같아서
		// 컴파일러 최적화에 의해서 Reload를 call로 호출하지 않고
		// Reload로 jump하고, Reload에서 바로 이전 위치로 return한다
		// 그래서 추가적인 함수 호출에 의한 오버헤드는
		// Reload의 return type을 void로 하는 것 보다 훨씬 적다
		// 오히려 Reload의 return type을 void로 하면 Reload를 call하는 경우와
		// reload에서 return하는 과정에서 스택을 억세스하고 추가적인 오버헤드가 발생한다
		return RegenerateTable();
	}

	return MakeU32(*piNext_++);
}

F32
_RAND_F32(const J9::DebugInfo& rInfo)
{
	F32	f = J9::Random::cGlobal.GetF32();
	LOG(_W("@@TDO@@ Random f32(%f) : %s(%d) : %s\r\n"), f, rInfo.pFileName_, rInfo.iLine_, rInfo.pFuncName_);
	return f;
}

U32
_RAND_U32(const J9::DebugInfo& rInfo)
{
	U32	u = J9::Random::cGlobal.GetU32();
	LOG(_W("@@TDO@@ Random u32(%u) : %s(%d) : %s\r\n"), u, rInfo.pFileName_, rInfo.iLine_, rInfo.pFuncName_);
	return u;
}
