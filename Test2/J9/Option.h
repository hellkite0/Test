// $Id: Option.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_OPTION_H__
#define __J9_OPTION_H__

namespace J9
{
	// �̱��� �ƴϴ�.
	// ���� ����ڰ� �̱������� ����ϰ� ������
	// �� Ŭ������ ��ӹ޾Ƽ� �̱������� �����ϸ� �ȴ�.
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

		// �ε�� ������ �Ҹ� �� �ִ�.
		// ���� ���Ͽ��� �ɼ��� ���� �� �ִٴ� �Ҹ���.
		void		Load(const WChar* pFileName);
		void		SetOverwrite(bool b) { bOverwrite_ = b; }

		// �̸����� �˻��ϸ� Any��ü�� ���� �� �ִ�.
		const Any&	Get(const StringW& rName) const;

	protected:
		// ���� ��� �ϴ� �ִ� Load�� ȣ���ϱ����� �Ʒ� �Լ����� �̿��Ͽ�
		// ĳ�� ����Ʈ�� ����Ѵ�
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