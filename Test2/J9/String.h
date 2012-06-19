// $Id: String.h 8979 2011-07-15 03:14:11Z hyojin.lee $

#ifndef __J9_STRING_H__
#define __J9_STRING_H__

namespace J9
{
	typedef std::wstring	StringW;
	typedef std::string		StringA;

	#ifdef _UNICODE
		typedef StringW	StringT;
	#else
		typedef StringA	StringT;
	#endif

	typedef StringT	String;

	template<typename T>
	inline Size GetBufferSize(const std::basic_string<T>& r, bool bNullTeminated = true)
	{
		return sizeof(std::basic_string<T>::value_type) * (r.length() + (bNullTeminated ? 1 : 0));
	}

	// 함수이지만 명사형으로 이름을 짓는게 조금 더 직관적인 듯 하다
	const StringW& NullStringW();
	const StringA& NullStringA();
#ifdef _UNICODE
	__inline const StringW& NullString() { return NullStringW(); }
#else
	__inline const StringA& NullString() { return NullStringA(); }
#endif

	inline Size
	GetHash(const StringW& r)
	{
		const Size	iShift = 6;
		const Size	iBitsize = sizeof(Size) * 8;
		const Size	iMask = ~0U << (iBitsize - iShift);

		const WChar*	p = r.c_str();
		ASSERT(p);

		Size	iHash = 0;
		for (; *p; ++p)
		{
			iHash = (iHash & iMask) ^ (iHash << iShift) ^ *p;
		}

		return iHash;
	}

	inline Size
	GetIHash(const StringW& r)
	{
		const Size	iShift = 6;
		const Size	iBitsize = sizeof(Size) * 8;
		const Size	iMask = ~0U << (iBitsize - iShift);

		const WChar*	p = r.c_str();

		ASSERT(p);

		Size	iHash = 0;
		for (; *p; ++p)
		{
			WChar	cLowered = ::towlower(*p);
			iHash = (iHash & iMask) ^ (iHash << iShift) ^ cLowered;
		}

		return iHash;
	}

	////////////////////////////////////////////////////////////////////////////////
	// buffer를 사용하는 경우 null-terminated string로 encode, decode 한다
	template<>
	struct TypeTraits_Binary_Custom<Buffer, StringW>
	{
		static Size
		Read(Buffer* pIO, StringW& r)
		{
			ASSERT(pIO->iReadOffset_ < pIO->cBuffer_.size());
			r = StringW(_rc<const WChar*>(&pIO->cBuffer_[pIO->iReadOffset_]));
			Size	iRead = GetBufferSize(r);
			pIO->iReadOffset_ += iRead;
			ASSERT(pIO->iReadOffset_ <= pIO->cBuffer_.size());
			return iRead;
		}

		static Size
		Write(Buffer* pIO, const StringW& r)
		{
			return pIO->Write(GetBufferSize(r), r.c_str());
		}
	};

	template<>
	struct TypeTraits_Binary_Custom<Buffer, StringA>
	{
		static Size
		Read(Buffer* pIO, StringA& r)
		{
			ASSERT(pIO->iReadOffset_ < pIO->cBuffer_.size());
			r = StringA(_rc<const AChar*>(&pIO->cBuffer_[pIO->iReadOffset_]));
			Size	iRead = GetBufferSize(r);
			pIO->iReadOffset_ += iRead;
			ASSERT(pIO->iReadOffset_ <= pIO->cBuffer_.size());
			return iRead;
		}

		static Size
		Write(Buffer* pIO, const StringA& r)
		{
			return pIO->Write(GetBufferSize(r), r.c_str());
		}
	};
	// buffer를 사용하는 경우 null-terminated string로 encode, decode 한다
	////////////////////////////////////////////////////////////////////////////////

	template<typename IO, typename Type>
	struct TypeTraits_Binary_Custom<IO, std::basic_string<Type> >
	{
		static Size
		Read(IO* pIO, std::basic_string<Type>& r)
		{
			r.clear();

			Size	iRead = 0;
			U32		iLen;

			iRead += pIO->Read(sizeof(iLen), &iLen);
			ASSERT(iRead == sizeof(iLen));

			if (iLen == 0)	return iRead;

			std::vector<std::basic_string<Type>::value_type>	cBuffer(iLen);
			iRead += pIO->Read(iLen * sizeof(std::basic_string<Type>::value_type), &cBuffer[0]);
			ASSERT(iRead == (sizeof(iLen) + iLen * sizeof(std::basic_string<Type>::value_type)));
			r.assign(cBuffer.begin(), cBuffer.end());

			return iRead;
		}

		static Size
		Write(IO* pIO, const std::basic_string<Type>& r)
		{
			Size	iWritten = 0;
			U32		iLen = _sc<U32>(r.length());

			iWritten += pIO->Write(sizeof(iLen), &iLen);

			if (iLen == 0)	return iWritten;

			iWritten += pIO->Write(iLen * sizeof(std::basic_string<Type>::value_type), r.c_str());

			return iWritten;
		}
	};

	template<>
	inline Size TypeTraits_UnicodeText::Write(StandardUnicodeFile* pIO, const StringW& r)
	{
		return pIO->Write(r.length() * sizeof(WChar), r.c_str());
	}

	template<>
	inline Size TypeTraits_UnicodeText::Write(StandardUnicodeFile* pIO, const WChar* const& p)
	{
		return pIO->Write(::wcslen(p) * sizeof(Char), p);
	}
};

#endif//__J9_STRING_H__
