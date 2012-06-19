// $Id: UtilityString.h 8577 2011-05-27 09:56:13Z hyojin.lee $

#ifndef __J9_UTILITY_STRING_H__
#define __J9_UTILITY_STRING_H__

////////////////////////////////////////////////////////////////////////////////
///	\brief	_itoa_s, _i64toa_s, _ui64toa, _itow_s, _i64tow_s, _ui64tow_s�� ���� ������ ���� ũ�⸦ �����Ѵ�.
///
///	_itoa_s, _i64toa_s, _ui64toa, _itow_s, _i64tow_s, _ui64tow_s �Լ����� ������ string���� ��ȯ�ϱ� ���ؼ�
///	���۸� ����Ѵ�. �� �� �ش� �Լ����� �����ϰ� ȣ��� �� �ִ� ������ ũ�⸦ �����Ѵ�.
///	MSDN�� ������ 32bit�� ��� 33 character, 64bit�� ��� 65 character�� �ִ��� �Ǿ��ִ�.
///	�׷��� �����ϰ� ������ ũ�⸦ 128�� �����Ѵ�.
///	\author	hyojin.lee
///	\date	2005-06-29
////////////////////////////////////////////////////////////////////////////////
#define SAFE_BUFFER_SIZE	128
////////////////////////////////////////////////////////////////////////////////
///	\brief	����->���ڿ� ��ȯ�� ����. ���� 10������ �����Ѵ�.
///
///	����->���ڿ� ��ȯ�� ����. ���� 10������ �����Ѵ�.
///	\author	hyojin.lee
///	\date	2005-06-29
////////////////////////////////////////////////////////////////////////////////
#define RADIX				10

namespace J9
{
	namespace StringUtil
	{
		bool					IsDigit(const StringA& r);
		bool					IsDigit(const StringW& r);

		void					FormatV(StringA& r, const AChar* pFormat, va_list cArgList);
		void					FormatV(StringW& r, const WChar* pFormat, va_list cArgList);

		template<typename T>
		std::basic_string<T>	Format(const T* p, ...);

		template<typename T>
		std::basic_string<T>	ToUpper(const std::basic_string<T>& r);

		template<typename T>
		std::basic_string<T>	ToLower(const std::basic_string<T>& r);

		StringA					ToStringA(const StringW& r);
		StringA					ToStringA(const WChar* pString, int iLength = -1);

		StringW					ToStringW(const StringA& r);
		StringW					ToStringW(const AChar* pString, int iLength = -1);

		StringA					ToStringUTF8(const StringW& r);		
		StringA					ToStringUTF8(const WChar* pString, int iLength = -1);
		
		StringW					FromStringUTF8(const StringA& r);
		StringW					FromStringUTF8(const AChar* pString, int iLength = -1);

		template<typename T>
		bool					Replace(std::basic_string<T>& r, const std::basic_string<T>& lhs, const std::basic_string<T>& rhs);

		template<typename T>
		void					ReplaceAll(std::basic_string<T>& r, const std::basic_string<T>& lhs, const std::basic_string<T>& rhs);

		template<typename T>
		std::basic_string<T>	Tokenize(const std::basic_string<T>& rString, const std::basic_string<T>& rToken, Size& iPos);

		template<typename T>
		Size					Tokenize(const std::basic_string<T>& rString, const std::basic_string<T>& rToken, std::vector<std::basic_string<T> >& rVector);

		template<typename DataType, typename CharType>
		void					ToString(const DataType& rData, std::basic_string<CharType>& rString);

		template<typename CharType, typename DataType>
		void					Convert(const std::basic_string<CharType>& rString, DataType& rData);

		template<typename CharType, typename DataType>
		void					ConvertFromHex(const std::basic_string<CharType>& rString, DataType& rData);

		template<typename CharType>
		F32						ConvertToFloat(const std::basic_string<CharType>& rString);

		void					ReplaceLineBreak(StringW& rString);

		// src�� hello_1234�̰� prefix�� hello_�̸� 1234�� �����Ѵ�.
		// src�� prefix�� �������� ������ U32_MAX�� �����Ѵ�.
		U32						GetIndex(const StringW& rSrc, const StringW& rPrefix);

		bool					CompareWildcardedString(const WChar* pWildcard, const WChar* pStringToCompare, bool bCaseSensitive = false);
		bool					CompareWildcardedString(const AChar* pWildcard, const AChar* pStringToCompare, bool bCaseSensitive = false);

		bool					IsHangeul(WChar iChar);
		void					HangeulDivide(WChar iHangeul, WChar& rCho, WChar& rJung, WChar& rJong);
		WChar					HangeulMerge(U32 iCho, U32 iJung, U32 iJong);
		WChar					GetJongsung(WChar iChar);
		WChar					GetJongsung(const WChar* pString, int iLength = -1);
		bool					HasJongsung(WChar iChar);
		bool					HasJongsung(const WChar* pString, int iLength = -1);
		const WChar*			SelectWithJongsung(const WChar* pWith, const WChar* pWithout, const WChar* pString, int iLength = -1);
		J9::StringW				SelectWithJongsung(const J9::StringW& r);
		J9::StringW				SelectEunNunIGa(const J9::StringW& r);

		J9::StringW				PushNumberComma(S32 iNumber);
		// ��Ʈ�� �� ���� �־��� ���ڵ��� �����Ѵ�
		template<typename T>
		std::basic_string<T>	Trim(const std::basic_string<T>& r, const T* pCharacters);
	};

	template<typename CharType>
	class _ArgParser
	{
		typedef stdext::hash_map<std::basic_string<CharType>, std::basic_string<CharType> >
		ArgMap;

	public:
		_ArgParser() {}
		_ArgParser(const std::basic_string<CharType>& rArg, const CharType* pArgDel = _T("/-"), const Char* pValueDel = _T("=:"), bool bTrimWhiteSpaces = true) { Parse(rArg, pArgDel, pValueDel, bTrimWhiteSpaces); }

		void	Parse(const std::basic_string<CharType>& rArg, const CharType* pArgDel = _T("/-"), const Char* pValueDel = _T("=:"), bool bTrimWhiteSpaces = true)
		{
			Size				iArg = 0;
			std::basic_string<CharType>	sArg;

			while (sArg = StringUtil::Tokenize(rArg, std::basic_string<CharType>(pArgDel), iArg), !sArg.empty())
			{
				if (bTrimWhiteSpaces)
					StringUtil::ReplaceAll(sArg, std::basic_string<CharType>(_W(" ")), std::basic_string<CharType>(_W("")));

				Size	iValue = 0;

				std::basic_string<CharType>	sArgName = StringUtil::Tokenize(sArg, std::basic_string<CharType>(pValueDel), iValue);
				std::basic_string<CharType>	sArgValue = StringUtil::Tokenize(sArg, std::basic_string<CharType>(pValueDel), iValue);

				if (sArgName.empty())	continue;
				cArgs_[sArgName] = sArgValue;
			}
		}

		const std::basic_string<CharType>&	Get(const std::basic_string<CharType>& rArg) const
		{
			static std::basic_string<CharType>	sNull;

			ArgMap::const_iterator	cIter = cArgs_.find(rArg);
			if (cIter == cArgs_.end())
				return sNull;

			return cIter->second;
		}

		bool						Has(const std::basic_string<CharType>& rArg) const
		{
			return cArgs_.find(rArg) != cArgs_.end();
		}

	private:
		ArgMap	cArgs_;
	};

	typedef _ArgParser<WChar>	ArgParserW;
	typedef _ArgParser<AChar>	ArgParserA;
	typedef _ArgParser<Char>	ArgParser;

	// comma separated string
	template<typename CharType>
	class _CSS
	{
	public:
		_CSS() {}
		_CSS(const std::basic_string<CharType>& rStr) { Init(rStr); }

		// ��Ʈ���� �Ľ��Ͽ� ���̺� �����Ѵ�
		void Init(const std::basic_string<CharType>& rStr, const J9::StringW& sToken = _W(","))
		{
			sStrings_.clear();
			StringUtil::Tokenize(rStr, sToken, sStrings_);
			// �Ľ̵� ���� �̻��� ��Ʈ���� �䱸�� �� �� ��Ʈ���� �����ֱ�����
			// ���̺� �� ���� �� ��Ʈ���� 1�� �߰��Ѵ�
			sStrings_.push_back(std::basic_string<CharType>());
			iIndex_ = 0;
		}

		// �Ľ̵� ����� �ϳ��� ���̺��� ������ �����Ѵ�
		// �־��� ��� �̻��� ��Ʈ���� �䱸�� ��� �� ��Ʈ���� �����Ѵ�
		const std::basic_string<CharType>&	Get()
		{
			if (sStrings_.size() <= iIndex_)
				return sStrings_[iIndex_ - 1];

			return sStrings_[iIndex_++];
		}

		Size GetSize()
		{
			// �Ľ̵� ���� �̻��� ��Ʈ���� �䱸�� �� �� ��Ʈ���� �����ֱ�����
			// ���̺� �� ���� �� ��Ʈ���� 1�� �߰��ϱ� ������ Size �� 1�� �ٿ��� �����ش�..
			return sStrings_.size() - 1;
		}

		// �Ľ̵� ����� ���� ũ���� ���� �迭�� ���� �־��ִ� �Լ�
		template <typename Type, Size iSize>
		void						Apply(Type (&TypeArray)[iSize])
		{
			for (Size i = 0; i < iSize; ++i)
				StringUtil::Convert(Get(), TypeArray[i]);
		}

		template<typename ConvertType>
		operator ConvertType()	{ ConvertType c; J9::StringUtil::Convert(Get(), c); return c; }

	private:
		std::vector<std::basic_string<CharType> >	sStrings_;
		Size								iIndex_;
	};

	typedef _CSS<WChar>	CSSW;
	typedef _CSS<AChar>	CSSA;
	typedef _CSS<Char>	CSS;

	namespace StringUtil
	{
		template<typename Type, Size iSize>
		void	String2Array(const J9::StringW& rString, Type (&TypeArray)[iSize])
		{
			CSSW	cssw(rString);
			cssw.Apply(TypeArray);
		}

		template<typename Type>
		void	String2Vector(const J9::String& rString, Type& rVector)
		{
			CSSW cssw(rString);
			for (Size i = 0; i < Type::eDimension; ++i)
				rVector.v[i] = cssw;
		}

		template<typename Type>
		void	String2StdVector(const J9::String& rString, std::vector<Type>& rVector)
		{
			CSSW cssw(rString);
			Size iSize = cssw.GetSize();
			rVector.resize(iSize);
			for (Size i = 0; i < iSize; ++i)
				rVector[i] = cssw;
		}

		template<typename KeyType, typename Type>
		void	String2Map(const J9::StringW& rString, stdext::hash_map<KeyType, Type>& rMap)
		{
			CSSW	cssw(rString);
			stdext::hash_map<KeyType, Type>::key_type		cKey;
			stdext::hash_map<KeyType, Type>::mapped_type	cData;
			StringUtil::Convert(cssw.Get(), cKey);
			StringUtil::Convert(cssw.Get(), cData);

			rMap.insert(stdext::hash_map<KeyType, Type>::value_type(cKey, cData));
		}

		template<typename KeyType, typename Type>
		void	String2Map(const J9::StringW& rString, std::map<KeyType, Type>& rMap)
		{
			CSSW	cssw(rString);
			std::map<KeyType, Type>::key_type		cKey;
			std::map<KeyType, Type>::mapped_type	cData;
			StringUtil::Convert(cssw.Get(), cKey);
			StringUtil::Convert(cssw.Get(), cData);

			rMap.insert(std::map<KeyType, Type>::value_type(cKey, cData));
		}
	}
};

#include "UtilityString.inl"

#endif//__J9_UTILITY_STRING_H__