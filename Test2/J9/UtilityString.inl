// $Id$

#ifndef __J9_UTILITY_STRING_INL__
#define __J9_UTILITY_STRING_INL__

#include "StringTraits.h"

namespace J9
{
	namespace StringUtil
	{
		inline bool IsDigit(const StringA& r)
		{
			StringA::const_iterator cIter, cIterEnd;
			cIterEnd = r.end();
			for (cIter = r.begin(); cIter != cIterEnd; ++cIter)
				if (::isdigit(*cIter) == 0)
					return false;
			return true;
		}

		inline bool IsDigit(const StringW& r)
		{
			StringW::const_iterator cIter, cIterEnd;
			cIterEnd = r.end();
			for (cIter = r.begin(); cIter != cIterEnd; ++cIter)
				if (::iswdigit(*cIter) == 0)
					return false;
			return true;
		}

		inline void	FormatV(StringA& r, const AChar* pFormat, va_list cArgList)
		{
			r.clear();

			U32	iLen = ::_vscprintf(pFormat, cArgList);
			if (iLen == 0)
			{
				return;
			}

			std::vector<AChar>	cBuffer(iLen + 1);
			_vsnprintf_s(&cBuffer[0], cBuffer.size(), iLen, pFormat, cArgList);
			r.assign(&cBuffer[0], iLen);
		}

		inline void	FormatV(StringW& r, const WChar* pFormat, va_list cArgList)
		{
			r.clear();

			U32	iLen = ::_vscwprintf(pFormat, cArgList);
			if (iLen == 0)
			{
				return;
			}

			std::vector<WChar>	cBuffer(iLen + 1);
			_vsnwprintf_s(&cBuffer[0], cBuffer.size(), iLen, pFormat, cArgList);
			r.assign(&cBuffer[0], iLen);
		}

		template<typename T>
		std::basic_string<T> Format(const T* p, ...)
		{
			va_list cArgList;
			va_start(cArgList, p);
			std::basic_string<T>	s;
			FormatV(s, p, cArgList);
			va_end(cArgList);

			return s;
		}

		template<typename T>
		std::basic_string<T> ToUpper(const std::basic_string<T>& r)
		{
			if (r.empty())
				return std::basic_string<T>();

			std::basic_string<T>	s = r;
			std::transform(s.begin(), s.end(), s.begin(), StringTraits<T>::ToUpper);
			return s;
		}

		template<typename T>
		std::basic_string<T> ToLower(const std::basic_string<T>& r)
		{
			if (r.empty())
				return std::basic_string<T>();

			std::basic_string<T>	s = r;
			std::transform(s.begin(), s.end(), s.begin(), StringTraits<T>::ToLower);
			return s;
		}

		StringA	ToStringA(const StringW& r);
		StringW	ToStringW(const StringA& r);

		StringA ToStringUTF8(const StringW& r);		
		StringW FromStringUTF8(const StringA& r);

		template<typename T>
		bool Replace(std::basic_string<T>& r, const std::basic_string<T>& lhs, const std::basic_string<T>& rhs)
		{
			Size	iPos = r.find(lhs);
			if (iPos == std::basic_string<T>::npos)	return false;
			r.replace(iPos, lhs.length(), rhs);
			return true;
		}
		template<typename T>
		void ReplaceAll(std::basic_string<T>& r, const std::basic_string<T>& lhs, const std::basic_string<T>& rhs)
		{
			if (lhs.compare(rhs) == 0)
				return;
			while (Replace(r, lhs, rhs));
		}

		template<typename T>
		std::basic_string<T> Tokenize(const std::basic_string<T>& rString, const std::basic_string<T>& rToken, Size& iPos)
		{
			Size	iFound = iPos;

			if (rString.size() <= iPos)
			{
				iPos = _sc<Size>(std::basic_string<T>::npos);
				return std::basic_string<T>();
			}

			while (true)
			{
				iFound = rString.find_first_of(rToken, iPos);
				if (iFound != iPos)
					break;
				++iPos;
			}

			std::basic_string<T>	sRet = rString.substr(iPos, iFound - iPos);
			if (iFound == std::basic_string<T>::npos)
			{
				iPos = _sc<Size>(std::basic_string<T>::npos);
			}
			else
			{
				iPos = iFound + 1;
			}

			return sRet;
		}

		template<typename T>
		Size Tokenize(const std::basic_string<T>& rString, const std::basic_string<T>& rToken, std::vector<std::basic_string<T> >& rVector)
		{
			std::basic_string<T>	sToken;
			Size					iNum = 0;
			Size					iPos = 0;
			while(sToken = J9::StringUtil::Tokenize(rString, rToken, iPos), !sToken.empty())
			{
				rVector.push_back(sToken);
				++iNum;
			}

			return iNum;
		}

		template<typename CharType, typename DataType>
		void	ConvertFromHex(const std::basic_string<CharType>& rString, DataType& rData);

		template<typename CharType>
		F32		ConvertToFloat(const std::basic_string<CharType>& rString)
		{
			F32	f;
			Convert(rString, f);
			return f;
		}

		template<typename CharType>
		U32		ConvertToU32(const std::basic_string<CharType>& rString)
		{
			U32	i;
			Convert(rString, i);
			return i;
		}

		template<typename CharType>
		S32		ConvertToS32(const std::basic_string<CharType>& rString)
		{
			S32 i;
			Convert(rString, i);
			return i;
		}

		void	ReplaceLineBreak(StringW& rString);

		template<typename CharType>
		bool	StartWith(const std::basic_string<CharType>& rString, const std::basic_string<CharType>& rFind)
		{
			if (rString.length() < rFind.length())
				return false;
			return rString.compare(0, rFind.length(), rFind, 0, rFind.length()) == 0;
		}

		template<typename CharType>
		bool	EndWith(const std::basic_string<CharType>& rString, const std::basic_string<CharType>& rFind)
		{
			if (rString.length() < rFind.length())
				return false;
			return rString.compare(rString.length() - rFind.length(), rFind.length(), rFind, 0, rFind.length()) == 0;
		}

		// src가 hello_1234이고 prefix가 hello_이면 1234를 리턴한다.
		// src가 prefix로 시작하지 않으면 U32_MAX를 리턴한다.
		U32		GetIndex(const StringW& rSrc, const StringW& rPrefix);
		template<>
		inline void
			StringUtil::ToString(const bool& rData, StringA& rString)
		{
			rString = rData ? "true" : "false";
		}

		template<>
		inline void
			StringUtil::ToString(const char& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_itoa_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const S8& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_itoa_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U8& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_itoa_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const S16& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_itoa_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U16& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_itoa_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const S32& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_itoa_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U32& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_i64toa_s(rData, pBuf, SAFE_BUFFER_SIZE, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const F32& rData, StringA& rString)
		{
			rString = StringUtil::Format("%f", rData);
		}

		template<>
		inline void
			StringUtil::ToString(const S64& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_i64toa_s(rData, pBuf, SAFE_BUFFER_SIZE, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U64& rData, StringA& rString)
		{
			AChar	pBuf[SAFE_BUFFER_SIZE];
			::_ui64toa_s(rData, pBuf, SAFE_BUFFER_SIZE, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const F64& rData, StringA& rString)
		{
			rString = StringUtil::Format("%f", rData);
		}

		template<>
		inline void
			StringUtil::ToString(const char& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_itow_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const bool& rData, StringW& rString)
		{
			rString = rData ? _W("true") : _W("false");
		}

		template<>
		inline void
			StringUtil::ToString(const S8& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_itow_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U8& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_itow_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const S16& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_itow_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U16& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_itow_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const S32& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_itow_s(rData, pBuf, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U32& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_i64tow_s(rData, pBuf, SAFE_BUFFER_SIZE, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const F32& rData, StringW& rString)
		{
			rString = StringUtil::Format(_W("%f"), rData);
		}

		template<>
		inline void
			StringUtil::ToString(const S64& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_i64tow_s(rData, pBuf, SAFE_BUFFER_SIZE, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const U64& rData, StringW& rString)
		{
			WChar	pBuf[SAFE_BUFFER_SIZE];
			::_ui64tow_s(rData, pBuf, SAFE_BUFFER_SIZE, RADIX);
			rString = pBuf;
		}

		template<>
		inline void
			StringUtil::ToString(const F64& rData, StringW& rString)
		{
			rString = StringUtil::Format(_W("%f"), rData);
		}

		////////////////////////////////////////////////////////////////////////////////
		// StringUtil::Convert(StringA)
		////////////////////////////////////////////////////////////////////////////////
		template<>
		inline void
			StringUtil::Convert(const StringA& rString, bool& rData)
		{
			if (rString.empty() || !rString.compare("0") || !rString.compare("false"))
				rData = false;
			else
				rData = true;
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, char& rData)
		{
			rData = _sc<char>(::atoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, S8& rData)
		{
			rData = _sc<S8>(::atoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, U8& rData)
		{
			rData = _sc<U8>(::atoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, S16& rData)
		{
			rData = _sc<S16>(::atoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, U16& rData)
		{
			rData = _sc<U16>(::atoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, S32& rData)
		{
			rData = ::atoi(rString.c_str());
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, U32& rData)
		{
			rData = ::atoi(rString.c_str());
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, F32& rData)
		{
			rData = _sc<F32>(::atof(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, S64& rData)
		{
			//rData = ::_atoi64(rString.c_str());
			rData = _sc<S64>(::_strtoui64(rString.c_str(), NULL, 10));
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, U64& rData)
		{
			rData = ::_strtoui64(rString.c_str(), NULL, 10);
		}

		template<>
		inline void
			StringUtil::Convert(const StringA& rString, F64& rData)
		{
			rData = ::atof(rString.c_str());
		}

		////////////////////////////////////////////////////////////////////////////////
		// StringUtil::Convert(StringW)
		////////////////////////////////////////////////////////////////////////////////
		template<>
		inline void
			StringUtil::Convert(const StringW& rString, bool& rData)
		{
			if (rString.empty() || !rString.compare(_W("0")) || !rString.compare(_W("false")) || !rString.compare(_W("FALSE")))
				rData = false;
			else
				rData = true;
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, char& rData)
		{
			rData = _sc<char>(::_wtoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, S8& rData)
		{
			rData = _sc<S8>(::_wtoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, U8& rData)
		{
			rData = _sc<U8>(::_wtoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, S16& rData)
		{
			rData = _sc<S16>(::_wtoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, U16& rData)
		{
			rData = _sc<U16>(::_wtoi(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, S32& rData)
		{
			rData = ::_wtoi(rString.c_str());
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, U32& rData)
		{
			rData = _sc<U32>(::_wtoi64(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, F32& rData)
		{
			rData = _sc<F32>(::_wtof(rString.c_str()));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, S64& rData)
		{
			//rData = ::_wtoi64(rString.c_str());
			rData = _sc<S64>(::_wcstoui64(rString.c_str(), NULL, 0));
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, U64& rData)
		{
			rData = ::_wcstoui64(rString.c_str(), NULL, 0);		// 3번 인자는 기본값~
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, F64& rData)
		{
			rData = ::_wtof(rString.c_str());
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, StringA& rData)
		{
			rData = ToStringA(rString);
		}

		template<>
		inline void
			StringUtil::Convert(const StringW& rString, StringW& rData)
		{
			rData = rString;
		}

		template<typename T>
		inline std::basic_string<T>	Trim(const std::basic_string<T>& r, const T* pCharacters)
		{
			std::basic_string<T>::size_type	iFirst = r.find_first_not_of(pCharacters);
			if (iFirst == std::basic_string<T>::npos)
				return std::basic_string<T>();
			std::basic_string<T>::size_type	iLast = r.find_last_not_of(pCharacters);
			ASSERT(iLast != std::basic_string<T>::npos && iFirst <= iLast);
			return std::basic_string<T>(r, iFirst, iLast - iFirst + 1);
		}
	};
}

#endif//__J9_UTILITY_STRING_INL__