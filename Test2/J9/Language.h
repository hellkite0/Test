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

		// �ε�� ������ �Ҹ� �� �ִ�.
		// ���� ���Ͽ��� �ɼ��� ���� �� �ִٴ� �Ҹ���.
		void				Load(const WChar* pFileName);
		void				LoadDirectory(StringW& pFilePath);
		void				SetOverwrite(bool b) { bOverwrite_ = b; }

		// va_list �� ����� ����, �����̳� �����ε� ����? ���� �Ѥ�
		// �� �Լ� ���� ���� �־� ����
		//StringW				GetLocalStringF(const PathKeyW rKey, ...) const;

		// va_list�� ����� ������ ���� �� �� ������ �����̳� �����ε� ������ �׳� �밡�� �����̴�.
		// ���� 5�� �̻� �� ���� ������? ������ ����.
		const StringW&		GetLocalString(PathKeyW rKey) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4) const;
		StringW				GetLocalString(PathKeyW rKey, const WChar* pArg1, const WChar* pArg2, const WChar* pArg3, const WChar* pArg4, const WChar* pArg5) const;
		// �� �Լ� ��Ʈ���� �� �������� Ŭ���� ���縦 ���� �ʱ� ���� �ʿ��� �������̴�.
		// private�� ��� ������ ������ ����� ��� �� ���� ������ ���� ������ �ۺ��̴�.
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