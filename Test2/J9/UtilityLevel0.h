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

	// �� �̻��� ���� �̸��� ���� ������
	// ���ø� ��ƿ��Ƽ�� ��� ����� ������ �������� �̷������
	// ��ƿ��Ƽ���ϰ��� ��ȣ ������ ����� ��츦 ���ϱ� ���ؼ�
	// ���� ������ ��ƿ��Ƽ�� �ϴ��� ���� ������ �и�
	// ��Ŭ��� ������ ���������� �ؾ��� ��찡 �־ �̴�
	//
	// �ϴ� �ٸ� ��ƿ��Ƽ�� ���Ͽ��� �������� ���� �ֵ��� �̸��� ���� ������
	// Level1, Level2, Level3�� ������ �ʱ⸦ �ٶ� ��...

	// �־��� ���� [min, max] ������ ����� ��� min�̳� max������ ��ü�Ѵ�
	// fitting�� �Ͼ�� ��� true�� �־��� ���� ���� ���� �ȿ� �־��� ��� false�� �����Ѵ�
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

	// tValue �� TypeArray �� 0 ���̻�, 1�������̸� true...�ƴϸ� false.
	template<typename T>
	__inline bool	IsInRange(typename ArgType<T>::Type cValue, typename ArgType<T>::Type cMin, typename ArgType<T>::Type cMax)
	{
		return (cMin <= cValue) && (cValue <= cMax);
	}

	// tValue �� TypeArray �� 0 ���̻�, 1�������̸� true...�ƴϸ� false.
	template<typename T>
	__inline bool	IsInRange(typename ArgType<T>::Type cValue, const T (&cRange)[2])
	{
		return IsInRange<T>(cValue, cRange[0], cRange[1]);
	}
}

// standard library�� modff�� ���� �κ��� ������ ������ �䱸�ϴ� �� ������
// ���� �κ��� �ʿ� ���� ��쿡�� ���� �κ��� ������ ������ �����Ͽ� �����͸� �Ѱ���� �ϴ�
// ������ ������ �ذ��ϱ� ���� ���� �����ε�
F32 modff(F32 f);

#endif//__J9_UTILITY_LEVEL0_H__
