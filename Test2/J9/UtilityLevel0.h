// $Id: UtilityLevel0.h 5529 2010-02-08 04:40:26Z euishik.kang $

#ifndef __J9_UTILITY_LEVEL0_H__
#define __J9_UTILITY_LEVEL0_H__

namespace J9
{
	__inline bool	IsValid(F32 f)
	{
		return f == f;
	}

	__inline bool	IsValid(F64 f)
	{
		return f == f;
	}

	// 이 이상한 파일 이름이 생긴 이유는
	// 템플릿 유틸리티의 경우 선언과 구현이 같은데서 이루어져서
	// 유틸리티파일간의 상호 의존이 생기는 경우를 피하기 위해서
	// 같은 성격의 유틸리티라 하더라도 구현 파일의 분리
	// 인클루드 순서를 순차적으로 해야할 경우가 있어서 이다
	//
	// 일단 다른 유틸리티나 파일에의 의존성이 적은 애들을 이리로 끌어 모은다
	// Level1, Level2, Level3이 생기지 않기를 바랄 뿐...

	// 주어진 값이 [min, max] 구간을 벗어나는 경우 min이나 max값으로 대체한다
	// fitting이 일어났을 경우 true를 주어진 값이 원래 범위 안에 있었을 경우 false를 리턴한다
	template<typename T>
	__inline bool Fit(typename J9::ArgType<T>::Type cMin, typename J9::ArgType<T>::Type cMax, T& rValue)
	{
		ASSERT(cMin <= cMax);
		if (rValue < cMin)
		{
			rValue = cMin;
			return true;
		}
		else if (cMax < rValue)
		{
			rValue = cMax;
			return true;
		}

		return false;
	}

	template<>
	__inline bool Fit(typename J9::ArgType<F32>::Type cMin, typename J9::ArgType<F32>::Type cMax, F32& rValue)
	{
		ASSERT(cMin <= cMax);
		if (rValue < cMin)
		{
			rValue = cMin;
			return true;
		}
		else if (cMax < rValue)
		{
			rValue = cMax;
			return true;
		}

		if (!IsValid(rValue))
		{
			J9::Break();
			rValue = cMin;
		}

		return false;
	}

	template<>
	__inline bool Fit(typename J9::ArgType<F64>::Type cMin, typename J9::ArgType<F64>::Type cMax, F64& rValue)
	{
		ASSERT(cMin <= cMax);
		if (rValue < cMin)
		{
			rValue = cMin;
			return true;
		}
		else if (cMax < rValue)
		{
			rValue = cMax;
			return true;
		}
		if (!IsValid(rValue))
		{
			J9::Break();
			rValue = cMin;
		}

		return false;
	}

	// tValue 가 TypeArray 의 0 번이상, 1번이하이면 true...아니면 false.
	template<typename T>
	__inline bool	IsInRange(typename ArgType<T>::Type cValue, typename ArgType<T>::Type cMin, typename ArgType<T>::Type cMax)
	{
		return (cMin <= cValue) && (cValue <= cMax);
	}

	// tValue 가 TypeArray 의 0 번이상, 1번이하이면 true...아니면 false.
	template<typename T>
	__inline bool	IsInRange(typename ArgType<T>::Type cValue, const T (&cRange)[2])
	{
		return IsInRange<T>(cValue, cRange[0], cRange[1]);
	}
}

// standard library의 modff가 정수 부분을 저장할 변수를 요구하는 것 때문에
// 정수 부분이 필요 없는 경우에도 정수 부분을 저장할 변수를 생성하여 포인터를 넘겨줘야 하는
// 귀찮은 문제를 해결하기 위해 만든 오버로딩
F32 modff(F32 f);

#endif//__J9_UTILITY_LEVEL0_H__
