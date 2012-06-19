// $Id: Option.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_OPTION_H__
#define __J9_OPTION_H__

namespace J9
{
	// 싱글턴 아니다.
	// 최종 사용자가 싱글턴으로 사용하고 싶으면
	// 이 클래스를 상속받아서 싱글턴으로 지정하면 된다.
	class Option
	{
	private:
		typedef stdext::hash_map<StringW, Any>
		OptionMap;

		enum OptionType
		{
			eOTInvalid,
			eOTBool,
			eOTInteger,
			eOTFloat,
			eOTStringW,
			eOTStringA,
		};

		struct CacheElem
		{
			OptionType	iType;
			void*		pElem;
		};

		typedef stdext::hash_map<StringW, CacheElem>
		CacheMap;

	public:
		Option();
		virtual ~Option() {}

		// 로드는 여러번 불릴 수 있다.
		// 여러 파일에서 옵션을 읽을 수 있다는 소리다.
		void		Load(const WChar* pFileName);
		void		SetOverwrite(bool b) { bOverwrite_ = b; }

		// 이름으로 검색하면 Any객체를 얻을 수 있다.
		const Any&	Get(const StringW& rName) const;

	protected:
		// 자주 사용 하는 애는 Load를 호출하기전에 아래 함수들을 이용하여
		// 캐쉬 리스트에 등록한다
		void		AddCacheElem(const StringW& rName, bool* p);
		void		AddCacheElem(const StringW& rName, S32* p);
		void		AddCacheElem(const StringW& rName, F32* p);
		void		AddCacheElem(const StringW& rName, StringW* p);
		void		AddCacheElem(const StringW& rName, StringA* p);

	private:
		static OptionType	GetType(const StringW& rType);
		void				AddElem(const StringW& rName, const StringW& rType, const StringW& rValue);

	private:
		OptionMap	cOptionMap_;
		CacheMap	cCacheMap_;
		bool		bOverwrite_;
	};
};

#endif//__J9_OPTION_H__