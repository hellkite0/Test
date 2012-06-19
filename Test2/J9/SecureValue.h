// $Id$

#ifndef __J9_SECURE_VALUE_H__
#define __J9_SECURE_VALUE_H__

namespace J9
{
#ifdef J9_SOL_USE_SECURITY_VALUE
#define SECURE_HELPER_SWIZZLE(pData, iSwizzle, iShift) \
	{ \
		int iFirst = (iSwizzle >> iShift) & 0x03; \
		int iSecond = (iSwizzle >> (iShift + 2)) & 0x03; \
		U8*	pU8 = _rc<U8*>(pData); \
		U8	iTemp = pU8[iFirst]; \
		pU8[iFirst] = pU8[iSecond]; \
		pU8[iSecond] = iTemp; \
	}

	class SecureValueHelper
	{
	public:
		__inline static void	Swizzle(U32* pData, U16	iSwizzle, int iSize)
		{
			ASSERT((iSize % 4) == 0);
			U32*	pTo = pData + (iSize / 4);
			while(0 < (pTo - pData))
			{
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 0);
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 4);
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 8);
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 12);
				++pData;
			}
		}

		__inline static void	Unswizzle(U32* pData, U16 iSwizzle, int iSize)
		{
			ASSERT((iSize % 4) == 0);
			U32*	pTo = pData + (iSize / 4);
			while(0 < (pTo - pData))
			{
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 12);
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 8);
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 4);
				SECURE_HELPER_SWIZZLE(pData,iSwizzle, 0);
				++pData;
			}
		}

		__inline static U8	GetSpreadBit(U32 i)
		{
			static const U8	iSpreads[] = {
				0x0f, 0x17, 0x1b, 0x1d, 0x1e, 0x27, 0x2b, 0x2d,
				0x2e, 0x33, 0x35, 0x36, 0x39, 0x3a, 0x3c, 0x47,
				0x4b, 0x4d, 0x4e, 0x53, 0x55, 0x56, 0x59, 0x5a,
				0x5c, 0x63, 0x65, 0x66, 0x69, 0x6a, 0x6c, 0x71,
				0x72, 0x74, 0x78, 0x87, 0x8b, 0x8d, 0x8e, 0x93,
				0x95, 0x96, 0x99, 0x9a, 0x9c, 0xa3, 0xa5, 0xa6,
				0xa9, 0xaa, 0xac, 0xb1, 0xb2, 0xb4, 0xb8, 0xc3,
				0xc5, 0xc6, 0xc9, 0xca, 0xcc, 0xd1, 0xd2, 0xd4,
				0xd8, 0xe1, 0xe2, 0xe4, 0xe8, 0xf0
			};
			return iSpreads[i % countof(iSpreads)];
		}

		__inline static void	Merge(U8* pDest, const U8* pLhs, const U8* pRhs, U8 iSpread, int iSize)
		{
			ASSERT((iSize % 4) == 0);
			iSize /= 4;
			while(iSize-->0)
			{
				*pDest++ = (iSpread & (0x01 << 0)) ? *pLhs++ : *pRhs++;
				*pDest++ = (iSpread & (0x01 << 1)) ? *pLhs++ : *pRhs++;
				*pDest++ = (iSpread & (0x01 << 2)) ? *pLhs++ : *pRhs++;
				*pDest++ = (iSpread & (0x01 << 3)) ? *pLhs++ : *pRhs++;
				*pDest++ = (iSpread & (0x01 << 4)) ? *pLhs++ : *pRhs++;
				*pDest++ = (iSpread & (0x01 << 5)) ? *pLhs++ : *pRhs++;
				*pDest++ = (iSpread & (0x01 << 6)) ? *pLhs++ : *pRhs++;
				*pDest++ = (iSpread & (0x01 << 7)) ? *pLhs++ : *pRhs++;
			}
		}

		__inline static void	Divide(U8* pLhs, U8* pRhs, const U8* pSrc, U8 iSpread, int iSize)
		{
			ASSERT((iSize % 4) == 0);
			iSize /= 4;
			while(iSize-->0)
			{
				if (iSpread & (0x01 << 0)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
				if (iSpread & (0x01 << 1)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
				if (iSpread & (0x01 << 2)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
				if (iSpread & (0x01 << 3)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
				if (iSpread & (0x01 << 4)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
				if (iSpread & (0x01 << 5)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
				if (iSpread & (0x01 << 6)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
				if (iSpread & (0x01 << 7)) { *pLhs++ = *pSrc++; } else { *pRhs++ = *pSrc++; }
			}
		}

		__inline static void	XOR(U32* pDest, U32 iKey, int iSize)
		{
			ASSERT((iSize % 4) == 0);
			U32*	pTo = pDest + (iSize / 4);
			while(0 < (pTo - pDest))
			{
				*pDest++ ^= iKey;
			}
		}
	};

	class SecureValueKey
	{
	public:
		void	Make(U32 iKey)
		{
			iKey_ = iKey;
			iSpreads_[0] = SecureValueHelper::GetSpreadBit(iKey & 0x0000ffff);
			iSpreads_[1] = SecureValueHelper::GetSpreadBit((iKey & 0xffff0000) >> 16);
		}

	public:
		union
		{
			U32	iKey_;
			U16	iSwizzles_[2];
			struct 
			{
				U8	iDummy0_;
				U8	iSpreads_[2];
				U8	iDummy1_;
			};
		};
	};


	template<typename Type>
	class SecureValue
	{
		typedef typename J9::ArgType<Type>::Type	ArgType;

		enum
		{
			eSize = sizeof(Type),
			eAlign4 = ((eSize - 1) / 4 + 1) * 4,
			eBufferSize = eAlign4 * 2,
		};

	public:
		SecureValue()
		{
			Init();
		}

		SecureValue(const SecureValue& r)
		{
			Init();
			Set(r.Get());
		}

		SecureValue(typename ArgType tValue)
		{
			Init();
			Set(tValue);
		}

		~SecureValue()
		{
			delete[] pBuffers_[0];
			delete[] pBuffers_[1];
		}

		// set functions
		__inline SecureValue&	operator=(typename ArgType tValue)
		{
			Set(tValue);
			return *this;
		}

		__inline SecureValue&	operator=(const SecureValue& r)
		{
			Set(r.Get());
			return *this;
		}

		void	Set(typename ArgType tValue)
		{
			U8	iValue0[eAlign4] = { 0 };
			U8	iValue1[eAlign4] = { 0 };

			::memcpy(iValue0, &tValue, eSize);
			::memcpy(iValue1, &tValue, eSize);
			SecureValueHelper::XOR(_rc<U32*>(iValue0), _rc<U32>(pBuffers_[0]) ^ cKey_.iKey_, eAlign4);
			SecureValueHelper::XOR(_rc<U32*>(iValue1), _rc<U32>(pBuffers_[0]) + cKey_.iKey_, eAlign4);
			SecureValueHelper::Swizzle(_rc<U32*>(iValue0), cKey_.iSwizzles_[0], eAlign4);
			SecureValueHelper::Swizzle(_rc<U32*>(iValue1), cKey_.iSwizzles_[1], eAlign4);
			SecureValueHelper::Merge(pBuffers_[0], iValue0, iValue1, cKey_.iSpreads_[0], eAlign4);

			U8	iValue2[eAlign4] = { 0 };
			U8	iValue3[eAlign4] = { 0 };
			::memcpy(iValue2, &tValue, eSize);
			::memcpy(iValue3, &tValue, eSize);
			SecureValueHelper::XOR(_rc<U32*>(iValue2), _rc<U32>(pBuffers_[1]) - cKey_.iKey_, eAlign4);
			SecureValueHelper::XOR(_rc<U32*>(iValue3), _rc<U32>(pBuffers_[1]) ^ cKey_.iKey_, eAlign4);
			SecureValueHelper::Swizzle(_rc<U32*>(iValue2), cKey_.iSwizzles_[1], eAlign4);
			SecureValueHelper::Swizzle(_rc<U32*>(iValue3), cKey_.iSwizzles_[0], eAlign4);
			SecureValueHelper::Merge(pBuffers_[1], iValue2, iValue3, cKey_.iSpreads_[1], eAlign4);
		}

		// get functions
		__inline operator Type() const
		{
			return Get();
		}

		__inline Type	Get() const
		{
			Type	tReturn;
			Get(tReturn);
			return tReturn;
		}

		void	Get(Type& r) const
		{
			U8	iValue0[eAlign4];
			U8	iValue1[eAlign4];

			SecureValueHelper::Divide(iValue0, iValue1, pBuffers_[0], cKey_.iSpreads_[0], eAlign4);
			SecureValueHelper::Unswizzle(_rc<U32*>(iValue0), cKey_.iSwizzles_[0], eAlign4);
			SecureValueHelper::Unswizzle(_rc<U32*>(iValue1), cKey_.iSwizzles_[1], eAlign4);
			SecureValueHelper::XOR(_rc<U32*>(iValue0), _rc<U32>(pBuffers_[0]) ^ cKey_.iKey_, eAlign4);
			SecureValueHelper::XOR(_rc<U32*>(iValue1), _rc<U32>(pBuffers_[0]) + cKey_.iKey_, eAlign4);

			if (memcmp(iValue0, iValue1, eAlign4))
			{
				// 도와주세요~ 해킹당했어요
				pApp()->TeaseBadGuy(cKey_.iKey_ & 0x000000ff);
				return;
			}

			U8	iValue2[eAlign4];
			U8	iValue3[eAlign4];
			SecureValueHelper::Divide(iValue2, iValue3, pBuffers_[1], cKey_.iSpreads_[1], eAlign4);
			SecureValueHelper::Unswizzle(_rc<U32*>(iValue2), cKey_.iSwizzles_[1], eAlign4);
			SecureValueHelper::Unswizzle(_rc<U32*>(iValue3), cKey_.iSwizzles_[0], eAlign4);
			SecureValueHelper::XOR(_rc<U32*>(iValue2), _rc<U32>(pBuffers_[1]) - cKey_.iKey_, eAlign4);
			SecureValueHelper::XOR(_rc<U32*>(iValue3), _rc<U32>(pBuffers_[1]) ^ cKey_.iKey_, eAlign4);

			if (memcmp(iValue2, iValue3, eAlign4))
			{
				// 도와주세요~ 해킹당했어요
				pApp()->TeaseBadGuy(cKey_.iKey_ & 0x000000ff);
				return;
			}

			if (memcmp(iValue0, iValue2, eAlign4))
			{
				// 도와주세요~ 해킹당했어요
				pApp()->TeaseBadGuy(cKey_.iKey_ & 0x000000ff);
				return;
			}

			::memcpy(&r, iValue0, eSize);
		}

	private:
		__inline void	Init()
		{
			pBuffers_[0] = new U8[eBufferSize];
			pBuffers_[1] = new U8[eBufferSize];
			cKey_.Make(J9::Random::cGlobal.GetU32());
		}

	private:
		U8*				pBuffers_[2];
		SecureValueKey	cKey_;
	};
#else
	template<typename Type>
	class SecureValue
	{
		typedef typename J9::ArgType<Type>::Type	ArgType;

	public:
		SecureValue()
		{
		}

		SecureValue(typename ArgType tValue)
		{
			Set(tValue);
		}

		~SecureValue()
		{
		}

		// set functions
		__inline SecureValue&	operator=(typename ArgType tValue)
		{
			Set(tValue);
			return *this;
		}

		void	Set(typename ArgType tValue)
		{
			tValue_ = tValue;
		}

		// get functions
		__inline operator Type() const
		{
			return Get();
		}

		__inline Type	Get() const
		{
			Type	tReturn;
			Get(tReturn);
			return tReturn;
		}

		void	Get(Type& r) const
		{
			r = tValue_;
		}

	private:
		__inline void	Init()
		{
		}

	private:
		Type			tValue_;
	};
#endif
}

#endif//__J9_SECURE_VALUE_H__