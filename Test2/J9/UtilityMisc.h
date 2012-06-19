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
	/// \brief ��ü�� �ּҸ� ��� �Լ�
	///
	/// �ּҸ� ��� &�����ڴ� ������ �Ǿ� ���� �� �ֱ� ������ ��ü�� ����� �޸���
	/// ��ġ�� �˾Ƴ��� ���ؼ��� �� �Լ��� ����ϴ� ���� �����Ѵ�.
	/// ���� &�����ڿ� ���� �����ǵ� �ּҸ� �˾Ƴ���� �ϴ� ��쵵 ���� �� �����Ƿ�,
	/// ������� Ȯ���� �ʿ��ϴ�.
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
	/// \brief	'warning C4127: conditional expression is constant' ��� ���ֱ� ���� ��ƿ��Ƽ
	///
	/// �����Ϸ��� warning level�� ���� �� if�� while�� ���� condition �κп� ����� ����ϸ�
	/// C4127 warning�� �߻��Ѵ� �̸� ���ֱ� ���� Ʈ������ �� �Լ��� ����ϴ� ����
	/// �����Ѵ�.<br>
	/// ex) Always(true), Always(false)
	/// \author		hyojin.lee
	/// \date		2005/05/31
	////////////////////////////////////////////////////////////////////////////////
	inline bool Always(bool b)
	{
		return b;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \brief	�������� assignment�� ���� template �Լ�
	///
	/// ����� ������ UserType�� ����� opearator=()�� �Ʒ��� �־����� ����� ����
	///	�����ϰ�, assign�� Ÿ�Կ� ���ؼ� Convert�Լ��� ��üȭ �ϴ� �������
	/// �������� assignment operator�� �����ϵ��� �Ѵ�.<br>
	///	��ü�� �ܺο��� assignment operator�� �����ϴ� ����� oop�� �������� �ʱ� ������
	/// �̷��� ����� ����� �ܺ� library���� ���ǵ� ��ü�� �������� ������ ��ȯ�Ҷ�
	///	(���� ��� MaxSDK�� Point3��ü�� Vector3��ü�� ��ȯ�� ��)
	///	���� Ư���� ��쿡�� �����ϵ��� �Ѵ�.<br>
	///	UserType�� operator=()�� ���� ��)<br>
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

	// ���������� �������� U32�� ĳ�����ϴ°� �������� �ؾ��ϴ� ���� ���Ƽ�
	// �����ϰ� ����ϱ� ���� ������
	// ������ Ptr�� J9::Ptr�� �ǹ����� �ʴ´�.
	// �� �Լ��� ���ڿ� T* ��� J9::Ptr<T>�� �Ѱ��ָ�
	// �Ƹ� ����� �������� ���� ���̴�.
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
				_LOG(_W("NumberStringCast : �Է°��� ���ڷθ� �������� ���� ���?\r\n"));
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
