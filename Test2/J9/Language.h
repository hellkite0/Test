// $Id: Language.h 8979 2011-07-15 03:14:11Z hyojin.lee $

#ifndef __J9_LANGUAGE_H__
#define __J9_LANGUAGE_H__

#include "StringKey.h"

namespace J9
{
	class Language
	{
		DYNAMIC_SINGLETON(Language);

		typedef stdext::hash_map<PathKeyW, StringW>
		Strings;

	public:
		enum Languages
		{
			eLKO,
			eLEN,
			eLJP,
			eLES,
			eLDE,
			eLZH_TW,			

			eLKO_Kor,

			eLNum,
		};

	public:
		Language();

		static const WChar*	EnumToString(Languages iLanguae);
		static Languages	StringToEnum(const WChar* pLanguage);

		void				SetLanguage(Languages iLang)			{ iLanguage_ = iLang; }
		void				SetLanguage(const WChar* pLanguage);
		Languages			GetLanguage() const						{ return iLanguage_; }

		// 로드는 여러번 불릴 수 있다.
		// 여러 파일에서 옵션을 읽을 수 있다는 소리다.
		void				Load(const WChar* pFileName);
		void				LoadDirectory(StringW& pFilePath);
		void				SetOverwrite(bool b) { bOverwrite_ = b; }

		// va_list 를 사용한 구현, 성능이나 오버로딩 문제? 몰라 ㅡㅡ
		// 이 함수 구현 문제 있어 보임
		//StringW				GetLocalStringF(const PathKeyW rKey, ...) const;

		// va_list를 사용한 구현을 생각 할 수 있으나 성능이나 오버로딩 문제로 그냥 노가다 구현이다.
		// 인자 5개 이상 쓸 일이 있을까? 있으면 난감.
		const StringW&		GetLocalString(PathKeyW rKey) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4, const WChar* pArg5) const;
		// 이 함수 세트들은 위 구현에서 클래스 복사를 하지 않기 위해 필요한 구현들이다.
		// private라도 상관 없으나 어차피 만들어 논거 쓸 일이 있을지 몰라서 밖으로 퍼블릭이다.
		void				GetLocalString(StringW& rString, PathKeyW rKey) const;
		void				GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1) const;
		void				GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2) const;
		void				GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3) const;
		void				GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4) const;
		void				GetLocalString(StringW& rString, PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4, const WChar* pArg5) const;

	private:
		void				AddString(const StringW& rKey, const StringW& rString);

	private:
		Languages			iLanguage_;
		Strings				cStrings_;
		bool				bOverwrite_;
	};
}

#define pLng J9::Language::Instance()

#endif//__J9_LANGUAGE_H__