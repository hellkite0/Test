// $Id$

#ifndef __J9_STRING_KEY_H__
#define __J9_STRING_KEY_H__

#include "StringTraits.h"
#include "UtilityString.h"
#include "Thread.h"

namespace J9
{
	template<typename Type>
	class _StringKey;
	template<typename Type>
	class _PathKey;

	template<typename Type>
	class _KeyTable
	{
	private:
		friend class _StringKey<Type>;
		friend class _PathKey<Type>;
		typedef stdext::hash_set<std::basic_string<Type> >
		Strings;

		static const std::basic_string<Type>*	Find(const Type* pString)
		{
			return Find(std::basic_string<Type>(pString));
		}

		static const std::basic_string<Type>*	Find(const std::basic_string<Type>& rString)
		{
			LOCK_SCOPE(&cLock);
			Strings::const_iterator	cIter = cStrings.find(rString);
			if (cIter != cStrings.end())
			{
				return &(*cIter);
			}

			return &(*cStrings.insert(rString).first);
		}

	private:
		static _KeyTable						cTable;
		static Lock								cLock;
		static Strings							cStrings;
		static const std::basic_string<Type>*	pEmpty;
	};

	template<typename Type>
	class _StringKey
	{
		typedef	_KeyTable<Type>				KeyTable;
		typedef StringTraits<Type, true>	Traits;

	public:
		// constructors
		__inline _StringKey() : p_(KeyTable::pEmpty)
		{
		}

		__inline _StringKey(const _StringKey& r) : p_(r.p_)
		{
		}

		__inline _StringKey(const Type* pString)
		{
			Init(pString);
		}

		__inline _StringKey(const std::basic_string<Type>& rString)
		{
			Init(rString);
		}

		// any나 parameter등에 const void*로 전달한 객체를 다시 생성할 때 사용
		// 스트링이 테이블 안에 들어 있는지에 대한 검사를 생략하므로
		// 생성자를 명시적으로 선언, 사용한다
		__inline explicit _StringKey(const void* p) : p_(_rc<const Type*>(p))
		{
		}

		// assignments
		__inline _StringKey& operator=(const _StringKey& r)
		{
			p_ = r.p_;
			return *this;
		}

		// compares
		__inline bool	operator==(const _StringKey& r) const
		{
			return p_ == r.p_;
		}

		__inline bool	operator==(const _PathKey<Type>& r) const
		{
			CASSERT(0, DONT_DO_THIS_COMPARE);
			return p_ == r.c_str();
		}

		__inline bool	operator!=(const _StringKey& r) const
		{
			return p_ != r.p_;
		}

		__inline bool	operator!=(const _PathKey<Type>& r) const
		{
			CASSERT(0, DONT_DO_THIS_COMPARE);
			return p_ != r.c_str();
		}

		__inline bool	operator<(const _StringKey& r) const
		{
			return p_ < r.p_;
		}

		// for hash
		__inline operator size_t() const
		{
			return _rc<size_t>(p_) ^ 0x97072047;
		}

		// general interfaces
		__inline bool			IsEmpty() const
		{
			return p_ == KeyTable::pEmpty;
		}

		int						Find(const Type* pType) const
		{
		}

		void			Clear()
		{
			p_ = KeyTable::pEmpty;
		}

		// string like interfaces
		__inline const Type*	c_str() const
		{
			return p_->c_str();
		}

		__inline const std::basic_string<Type>&	GetString() const
		{
			return *p_;
		}

		__inline const std::basic_string<Type>*	GetStringPointer() const
		{
			return p_;
		}

		__inline bool			empty() const
		{
			return IsEmpty();
		}

		int						find() const
		{
		}

		void					clear()
		{
			Clear();
		}

	private:
		void					Init(const std::basic_string<Type>& r)
		{
			p_ = KeyTable::Find(r);
		}

	private:
		const std::basic_string<Type>*	p_;
	};

	template<typename Type>
	class _PathKey
	{
		typedef	_KeyTable<Type>				KeyTable;
		typedef StringTraits<Type, false>	Traits;

	public:
		// constuctors
		__inline _PathKey() : p_(KeyTable::pEmpty)
		{
		}

		__inline _PathKey(const _PathKey& r) : p_(r.p_)
		{
		}

		__inline _PathKey(const Type* p)
		{
			Init(p);
		}

		__inline _PathKey(const std::basic_string<Type>& r)
		{
			Init(r);
		}

		// any나 parameter등에 const void*로 전달한 객체를 다시 생성할 때 사용
		// 스트링이 테이블 안에 들어 있는지에 대한 검사를 생략하므로
		// 생성자를 명시적으로 선언, 사용한다
		__inline explicit _PathKey(const void* p) : p_(_rc<const std::basic_string<Type>*>(p))
		{
		}

		// assignments
		__inline _PathKey& operator=(const _PathKey& r)
		{
			p_ = r.p_;
			return *this;
		}

		// compares
		__inline bool	operator==(const _PathKey& r) const
		{
			return p_ == r.p_;
		}

		__inline bool	operator==(const _StringKey<Type>& r) const
		{
			CASSERT(0, DONT_DO_THIS_COMPARE);
			return p_ == r.c_str();
		}

		__inline bool	operator!=(const _PathKey& r) const
		{
			return p_ != r.p_;
		}

		__inline bool	operator!=(const _StringKey<Type>& r) const
		{
			CASSERT(0, DONT_DO_THIS_COMPARE);
			return p_ != r.c_str();
		}

		__inline bool	operator<(const _PathKey& r) const
		{
			return p_ < r.p_;
		}

		// for hash
		__inline operator size_t() const
		{
			return _rc<size_t>(p_) ^ 0x97072047;
		}

		// general interfaces
		__inline bool			IsEmpty() const
		{
			return p_ == KeyTable::pEmpty;
		}

		void			Clear()
		{
			p_ = KeyTable::pEmpty;
		}

		// string like interfaces
		__inline const Type*	c_str() const
		{
			return p_->c_str();
		}

		__inline const std::basic_string<Type>&	GetString() const
		{
			return *p_;
		}

		__inline const std::basic_string<Type>*	GetStringPointer() const
		{
			return p_;
		}

		__inline bool			empty() const
		{
			return IsEmpty();
		}

		void			clear()
		{
			Clear();
		}

	private:
		void			Init(const std::basic_string<Type>& r)
		{
			Traits::String	s = J9::StringUtil::ToLower(r);
			J9::StringUtil::ReplaceAll<Type>(s, Traits::BackSlash(), Traits::Slash());
			p_ = KeyTable::Find(s);
		}

	private:
		const std::basic_string<Type>*	p_;
	};

	template<typename IO, typename Type>
	struct TypeTraits_Binary_Custom<IO, _StringKey<Type> >
	{
		static Size
		Read(IO* pIO, _StringKey<Type>& r)
		{
			Size	iRead = 0;

			std::basic_string<Type>	s;
			iRead += TypeTraits_Binary::Read(pIO, s);
			r = s;

			return iRead;
		}

		static Size
		Write(IO* pIO, const _StringKey<Type>& r)
		{
			Size	iWritten = 0;

			std::basic_string<Type>	s(r.c_str());
			iWritten += TypeTraits_Binary::Write(pIO, s);

			return iWritten;
		}
	};

	template<typename IO, typename Type>
	struct TypeTraits_Binary_Custom<IO, _PathKey<Type> >
	{
		static Size
			Read(IO* pIO, _PathKey<Type>& r)
		{
			Size	iRead = 0;

			std::basic_string<Type>	s;
			iRead += TypeTraits_Binary::Read(pIO, s);
			r = J9::_PathKey<Type>(s);

			return iRead;
		}

		static Size
			Write(IO* pIO, const _PathKey<Type>& r)
		{
			Size	iWritten = 0;

			std::basic_string<Type>	s(r.c_str());
			iWritten += TypeTraits_Binary::Write(pIO, s);

			return iWritten;
		}
	};

	typedef _StringKey<WChar>	StringKeyW;
	typedef _StringKey<AChar>	StringKeyA;
	typedef _PathKey<WChar>		PathKeyW;
	typedef _PathKey<AChar>		PathKeyA;
}

#endif//__J9_STRING_KEY_H__