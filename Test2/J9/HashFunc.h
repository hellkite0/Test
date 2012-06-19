// $Id: HashFunc.h 984 2008-09-24 05:49:35Z hyojin.lee $

#ifndef __J9_HASH_FUNC_H__
#define __J9_HASH_FUNC_H__

namespace J9
{
	// rotation
	// + 는 left, - 는 right 방향으로의 회전을 의미한다
	inline U8 Rotate(U8 iValue, S32 iShift)
	{
		iShift = 0 <= iShift ? iShift & 0x7 : 0x8 - (-iShift & 0x7);
		return _sc<U8>(iValue << iShift | iValue >> (0x8 - iShift));
	}
	inline U16 Rotate(U16 iValue, S32 iShift)
	{
		iShift = 0 <= iShift ? iShift & 0xf : 0x10 - (-iShift & 0xf);
		return _sc<U16>(iValue << iShift | iValue >> (0x10 - iShift));
	}
	inline U32 Rotate(U32 iValue, S32 iShift)
	{
		return 0 <= iShift ? ::_rotl(iValue, iShift) : ::_rotr(iValue, -iShift);
	}
	inline Size Rotate(Size iValue, S32 iShift)
	{
		return 0 <= iShift ? ::_lrotl(_sc<U32>(iValue), iShift) : ::_lrotr(_sc<U32>(iValue), -iShift);
	}
	inline U64 Rotate(U64 iValue, S32 iShift)
	{
		return 0 <= iShift ? ::_rotl64(iValue, iShift) : ::_rotr64(iValue, -iShift);
	}

	template<typename Key, typename Traits = std::less<Key> >
	struct HashCompare
	{
	public:
		enum
		{
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8		// min_buckets = 2 ^^ N, 0 < N
		};

		HashCompare() : cComparator_()
		{
		}

		HashCompare(Traits cComparator) : cComparator_(cComparator)
		{
		}

		Size operator()(const Key& r) const
		{
			__if_exists (Key::GetHash)
			{
				return r.GetHash();
			}
			__if_not_exists (Key::GetHash)
			{
				return GetHash(r);
			}
		}

		bool operator()(const Key& lhs, const Key& rhs) const
		{	// test if _Keyval1 ordered before _Keyval2
			return (cComparator_(lhs, rhs));
		}

	public:
		Traits cComparator_;
	};

	template<typename Type>
	struct ILess
	{
		bool operator()(const Type& lhs, const Type& rhs) const;
	};

	template<typename Key, typename Traits = ILess<Key> >
	struct IHashCompare
	{
	public:
		enum
		{
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8		// min_buckets = 2 ^^ N, 0 < N
		};

		IHashCompare() : cComparator_()
		{
		}

		IHashCompare(Traits cComparator) : cComparator_(cComparator)
		{
		}

		Size operator()(const Key& r) const
		{
			return GetIHash(r);
		}

		bool operator()(const Key& lhs, const Key& rhs) const
		{	// test if _Keyval1 ordered before _Keyval2
			return (cComparator_(lhs, rhs));
		}

	public:
		Traits cComparator_;
	};

	template<typename Key>
	struct BlindHashCompare
	{
	public:
		enum
		{
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8		// min_buckets = 2 ^^ N, 0 < N
		};

		BlindHashCompare()
		{
		}

		Size operator()(const Key& r) const
		{
			Size iHash = 0x97072047;
			const Size* pSize = _rc<const Size*>(GetAddress(r));

			// 4 바이트씩 묶어서 계산
			for (Size i = 0; i < sizeof(Key) / sizeof(Size); ++i)
				iHash = _rotr(_sc<U32>(iHash ^ *pSize++), 5);

			// 4 바이트 미만의 나머지 부분
			const U8* pU8 = _rc<const U8*>(pSize);
			switch (sizeof(Key) & (sizeof(Size) - 1))
			{
				case 3: iHash ^= pU8[2] << 16 | pU8[1] << 8 | pU8[0]; break;
				case 2: iHash ^= pU8[1] << 8 | pU8[0]; break;
				case 1: iHash ^= pU8[0]; break;
				case 0: break;
			}

			return iHash;
		}

		bool operator()(const Key& lhs, const Key& rhs) const
		{
			return ::memcmp(&lhs, &rhs, sizeof(Key)) < 0;
		}
	};

	template<typename T>
	struct HashFunc
	{
		Size operator()(const T& r) const
		{
			return r.GetHash();
		}
	};

	template<typename T>
	struct IHashFunc
	{
		Size operator()(const T& r) const
		{
			return r.GetIHash();
		}
	};

	template<typename T>
	struct BlindHashFunc
	{
		Size operator()(const T& r) const
		{
			Size iHash = 0x97072047;
			const Size* pSize = _rc<const Size*>(GetAddress(r));

			// 4 바이트씩 묶어서 계산
			for (Size i = 0; i < sizeof(T) / sizeof(Size); ++i)
				iHash = _rotr(_sc<U32>(iHash ^ *pSize++), 5);

			// 4 바이트 미만의 나머지 부분
			const U8* pU8 = _rc<const U8*>(pSize);
			switch (sizeof(T) & (sizeof(Size) - 1))
			{
				case 3: iHash ^= pU8[2] << 16 | pU8[1] << 8 | pU8[0]; break;
				case 2: iHash ^= pU8[1] << 8 | pU8[0]; break;
				case 1: iHash ^= pU8[0]; break;
				case 0: break;
			}

			return iHash;
		}
	};

	template<>
	struct HashFunc<void*>
	{
		Size operator()(const void* p) const
		{
			return _rc<Size>(p);
		}
	};

	template<>
	struct HashFunc<F32>
	{
		Size operator()(const F32 p) const
		{
			return _sc<Size>(*_rc<const U32*>(&p));
		}
	};

	inline Size GetHash(const void* p, Size iSize)
	{
		Size iHash = 0x97072047;
		const Size* pSize = _rc<const Size*>(p);

		// 4 바이트씩 묶어서 계산
		for (Size i = 0; i < iSize / sizeof(Size); ++i)
			iHash = _rotr(_sc<U32>(iHash ^ *pSize++), 5);

		// 4 바이트 미만의 나머지 부분
		const U8* pU8 = _rc<const U8*>(pSize);
		switch (iSize & (sizeof(Size) - 1))
		{
		case 3: iHash ^= pU8[2] << 16 | pU8[1] << 8 | pU8[0]; break;
		case 2: iHash ^= pU8[1] << 8 | pU8[0]; break;
		case 1: iHash ^= pU8[0]; break;
		case 0: break;
		}

		return iHash;
	}
};

#endif//__J9_HASH_FUNC_H__