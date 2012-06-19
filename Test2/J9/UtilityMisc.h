// $Id: UtilityMisc.h 6497 2010-07-20 06:38:45Z hyojin.lee $

#ifndef __J9_UTILITY_MISC_H__
#define __J9_UTILITY_MISC_H__

namespace J9
{
	template<typename T>
	void	SetZero(T& r)
	{
		::ZeroMemory(&r, sizeof(r));
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \brief 객체의 주소를 얻는 함수
	///
	/// 주소를 얻는 &연산자는 재정의 되어 있을 수 있기 때문에 객체가 저장된 메모리의
	/// 위치를 알아내기 위해서는 이 함수를 사용하는 것을 권장한다.
	/// 물론 &연산자에 의해 재정의된 주소를 알아내어야 하는 경우도 있을 수 있으므로,
	/// 사용자의 확인이 필요하다.
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	const T*	GetAddress(const T& r)
	{
		return _rc<const T*>(&_rc<const NullType&>(r));
	}
	template<typename T>
	T*	GetAddress(T& r)
	{
		return _rc<T*>(&_rc<NullType&>(r));
	}

	template<typename T>
	void SwapBlind(T& lhs, T& rhs)
	{
		U8	pBuffer[sizeof(T)];
		::memcpy(pBuffer, GetAddress(lhs), sizeof(T));
		::memcpy(GetAddress(lhs), GetAddress(rhs), sizeof(T));
		::memcpy(GetAddress(rhs), pBuffer, sizeof(T));
	}

	template<typename T>
	void SwapSemantic(T& lhs, T& rhs)
	{
		T	t = lhs;
		lhs = rhs;
		rhs = t;
	}

	template<typename T>
	void Swap(T& lhs, T& rhs)
	{
		SwapSemantic(lhs, rhs);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \brief	'warning C4127: conditional expression is constant' 경고를 없애기 위한 유틸리티
	///
	/// 컴파일러의 warning level이 높을 때 if나 while문 등의 condition 부분에 상수를 사용하면
	/// C4127 warning이 발생한다 이를 없애기 위한 트릭으로 이 함수를 사용하는 것을
	/// 권장한다.<br>
	/// ex) Always(true), Always(false)
	/// \author		hyojin.lee
	/// \date		2005/05/31
	////////////////////////////////////////////////////////////////////////////////
	inline bool Always(bool b)
	{
		return b;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \brief	범용적인 assignment을 위한 template 함수
	///
	/// 사용자 정의형 UserType의 멤버로 opearator=()를 아래에 주어지는 방법과 같이
	///	정의하고, assign할 타입에 대해서 Convert함수를 구체화 하는 방식으로
	/// 범용적인 assignment operator를 제공하도록 한다.<br>
	///	객체의 외부에서 assignment operator를 구현하는 방법이 oop에 적합하지 않기 때문에
	/// 이러한 방법의 사용은 외부 library에서 정의된 객체를 내부적인 형으로 변환할때
	///	(예를 들어 MaxSDK의 Point3객체를 Vector3객체로 변환할 때)
	///	같은 특정한 경우에만 한정하도록 한다.<br>
	///	UserType의 operator=()의 구현 예)<br>
	///	\code
	/// // declare template opeartor=()
	/// template<typename T>
	/// UserType& operator=(const T& r)
	/// {
	/// 	J9::Convert(*this, r);
	/// }
	/// // self assignment specification
	/// template<>
	/// UserType& operator=(const UserType& r)
	/// {
	/// 	// you should implement self assignment
	/// }
	///	\endcode
	/// \author		hyojin.lee
	/// \date		2005/05/31
	////////////////////////////////////////////////////////////////////////////////
	template<typename T1, typename T2>
	void Convert(T1& lhs, const T2& rhs)
	{
		CASSERT(0, NOT_IMPL);
	}

	inline Size BitFlag(Size i)
	{
		return 0x00000001 << i;
	}

	inline const U8* ParseItem(const U8* pSrc, U8* pTo, U8 cDelim)
	{
		while (cDelim == *pSrc && *pSrc) pSrc++;
		while (cDelim != *pSrc && *pSrc) *pTo++ = *pSrc++;
		return *pTo = 0, pSrc;
	}

	inline const WChar* ParseItem(const WChar* pSrc, WChar* pTo, const WChar cDelim)
	{
		while (cDelim == *pSrc && *pSrc) pSrc++;
		while (cDelim != *pSrc && *pSrc) *pTo++ = *pSrc++;
		return *pTo = 0, pSrc;
	}

	// 포인터형을 정수형인 U32로 캐스팅하는게 생각보다 해야하는 일이 많아서
	// 간단하게 사용하기 위해 만들어둠
	// 여기의 Ptr은 J9::Ptr을 의미하지 않는다.
	// 이 함수의 인자에 T* 대신 J9::Ptr<T>를 넘겨주면
	// 아마 제대로 동작하지 않을 것이다.
	template<typename T>
	__forceinline U32 Ptr2Int(T* p)
	{
		CASSERT(sizeof(T*) == sizeof(U32), ONLY_FOR_32BIT);
		return _sc<U32>(_rc<Size>(p));
	}

	template<typename T>
	__forceinline U32 Ptr2Int(const T* p)
	{
		CASSERT(sizeof(const T*) == sizeof(U32), ONLY_FOR_32BIT);
		return _sc<U32>(_rc<Size>(_cc<T*>(p)));
	}

	StringW		MakeStringFromDebugInfo(const DebugInfo& rWhere);

	static char UUEncodeTable[64] =
	{
		'f', 'L', 'J', '[', 'K', 'o', 'Y', 'v',		'u', 'e', 'A', '2', 'O', 'l', '9', 'z',
		'I', 'q', 'g', 'i', 'C', 't', '6', 'c',		'U', 'D', '1', 'F', 'a', 'W', 'x', 'V',
		'E', 'M', 'N', 'y', 'B', 'r', 'X', 'k',		'T', '3', ']', 'G', 'm', 'P', 's', 'j',
		'4', 'p', 'b', 'R', '0', '7', 'Q', 'd',		'H', 'h', 'S', '5', 'n', 'Z', 'w', '8',
	};

	static unsigned char UUDecodeTable[256];

	inline S32
	UUEncode(const unsigned char* pSrc, S32 iLen, unsigned char* pTarget)
	{
		U8 iBuf[HUGE_NUMBER];
		S32 iOffset = 0;

		J9::SetZero(iBuf);
		memcpy(iBuf, pSrc, iLen);

		for (int i = 0; i < iLen; i += 3)
		{
			pTarget[iOffset++] = UUEncodeTable[(iBuf[i] >> 2) & 0x3f];
			pTarget[iOffset++] = UUEncodeTable[((iBuf[i] << 4) & 0x30) | ((iBuf[i + 1] >> 4) & 0x0f)];
			pTarget[iOffset++] = UUEncodeTable[((iBuf[i + 1] << 2) & 0x3c) | ((iBuf[i + 2] >> 6) & 0x03)];
			pTarget[iOffset++] = UUEncodeTable[iBuf[i + 2] & 0x3f];
		}
		if (iLen % 3 != 0) iOffset -= (3 - (iLen % 3));
		pTarget[iOffset] = '\0';
		return iOffset;
	}

	inline S32
	UUDecode(const unsigned char *pSrc, int iLen, unsigned char *pTarget)
	{
		static bool bDecodingInitialized = false;
		S32 iModValue, iOffset = 0;
		U8 iData[HUGE_NUMBER] = { '\0' };

		if (bDecodingInitialized == false)
		{
			for (int i = 0; i < 256; i++) UUDecodeTable[i] = 64;
			for (int i = 0; i < 64; i++) UUDecodeTable[(int) UUEncodeTable[i]] = (unsigned char) i;
			bDecodingInitialized = true;
		}

		memcpy(iData, pSrc, iLen);
		iData[iLen] = '\0';

		for (int i = 0; i < iLen; i += 4)
		{
			pTarget[iOffset++] = UUDecodeTable[(int) iData[i]] << 2 | UUDecodeTable[(int) iData[i + 1]] >> 4;
			pTarget[iOffset++] = UUDecodeTable[(int) iData[i + 1]] << 4 | UUDecodeTable[(int) iData[i + 2]] >> 2;
			pTarget[iOffset++] = UUDecodeTable[(int) iData[i + 2]] << 6 | UUDecodeTable[(int) iData[i + 3]];
		}

		iModValue = iLen % 4;
		if (iModValue != 0) iOffset -= (4 - iModValue);
		pTarget[iOffset] = '\0';
		return iOffset;
	}

	// http://dodoubt.tistory.com/
	template<typename TO, typename FROM> 
	TO NumberStringCast(const FROM& from)
	{ 
		std::stringstream ss; 
		ss << from; 

		TO result; 
		ss >> result; 

		assert(!ss.fail() && !ss.bad() && ss.eof()); 
		if( ss.fail() || ss.bad() || !ss.eof() ) 
		{
			if (!ss.eof())
				_LOG(_W("NumberStringCast : 입력값이 숫자로만 구성되지 않은 경우?\r\n"));
			else
				_LOG(_W("NumberStringCast : Over or Underflow\r\n"));
		}

		return result; 
	}

	int	RoundUpPowerOf2(int  iNumber);

	void	MemSet4(void* pVoid, int iSet, Size iSize);
	bool	MemCmpCpy(void* pDest, const void* pSrc, Size iSize);
};

#define WHERE_AM_I	MakeStringFromDebugInfo(J9::DebugInfo(_W(__FILE__), __LINE__, _W(__FUNCTION__))).c_str()

#endif//__J9_UTILITY_MISC_H__
