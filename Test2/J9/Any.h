// $Id: Any.h 5309 2010-01-22 08:51:21Z hyojin.lee $

#ifndef __J9_ANY_H__
#define __J9_ANY_H__

#include "StringKey.h"

namespace J9
{
	class Any
	{
		template<typename IO, typename Type>
		friend struct J9::TypeTraits_Binary_Custom;
		friend class AnyHelper;

	public:
		enum AnyType
		{
			eATBool,
			eATUnsigned,
			eATSigned,
			eATFloat,
			eATStringW,
			eATStringA,
			eATVoidPtr,
		};

	public:
		Any() : iType_(eATUnsigned), iUnsigned_(0) { CASSERT(sizeof(Any) == 8, SIZE_ERROR_ANY); }
		Any(bool b);
		Any(const Any& r);
		Any(const U32 i);
		Any(const S32 i);
		Any(const F32 f);
		Any(const WChar* p);
		Any(const StringW& r);
		Any(const AChar* p);
		Any(const StringA& r);
		Any(const void* p);
		Any(StringKeyA s);
		Any(StringKeyW s);
		Any(PathKeyA s);
		Any(PathKeyW s);
		~Any() { Clear(); }

		void	Clear();

		// assignment operator
		Any&	operator=(const Any& r);
		Any&	operator=(const bool b);
		Any&	operator=(const U32 i);
		Any&	operator=(const S32 i);
		Any&	operator=(const F32	f);
		Any&	operator=(const StringW& r);
		Any&	operator=(const StringA& r);
		Any&	operator=(const void* p);
		Any&	operator=(StringKeyA s);
		Any&	operator=(StringKeyW s);
		Any&	operator=(PathKeyA s);
		Any&	operator=(PathKeyW s);

		// to eliminate ambiguity
		Any&	operator=(const WChar* p) { *this = StringW(p); return *this; }
		Any&	operator=(const AChar* p) { *this = StringA(p); return *this; }

		operator bool() const;
		operator U32() const;
		operator S32() const;
		operator F32() const;
		operator StringW() const;
		operator StringA() const;
		operator const void*() const;
		operator StringKeyA() const;
		operator StringKeyW() const;
		operator PathKeyA() const;
		operator PathKeyW() const;

		bool	IsNull() const;

		// 비교 연산자
		// 일단 Any안에 같은 타입안에서 같은 값을 가진 경우에만 true를 리턴한다
		bool	operator==(const Any& r) const;
		bool	operator==(const bool b) const;
		bool	operator==(const U32 i) const;
		bool	operator==(const S32 i) const;
		bool	operator==(const F32 f) const;
		bool	operator==(const StringW& r) const;
		bool	operator==(const StringA& r) const;
		//bool	operator==(void* p) const;
		bool	operator==(StringKeyA s) const;
		bool	operator==(StringKeyW s) const;
		bool	operator==(PathKeyA) const;
		bool	operator==(PathKeyW) const;

		AnyType	GetType() const { return _sc<AnyType>(iType_); }

		static Any	cBool_True;
		static Any	cBool_False;
		static Any	cNull_Bool;
		static Any	cNull_Unsigned;
		static Any	cNull_Signed;
		static Any	cNull_Float;
		static Any	cNull_StringW;
		static Any	cNull_StringA;
		static Any	cNull_VoidPtr;

	private:
		bool	IsAllocatedType() const;
		void	Alloc();
		void	AllocStringW();
		void	AllocStringA();
		void	Free();
		void	FreeStringW();
		void	FreeStringA();

		void	ChangeType(AnyType iType);

	private:
		// force to 8 byte
		U32	iType_;
		union
		{
			bool		bBool_;
			U32			iUnsigned_;
			S32			iSigned_;
			F32			fFloat_;
			StringW*	pStringW_;
			StringA*	pStringA_;
			const void*	pVoid_;
		};
	};

	class Anys
	{
		// AS IS
		// string key로 삽입한 애는 indexing도 된다.
		// string key없이 삽입한 애는 당연히 string key로는 검색할 수 없다.
		// 중복된 string key로 삽입되는 경우는
		// 이전에 삽입된 값은 삭제되지 않고,(index로 검색할 수 있다.)
		// string key만 overwrite된다.
		typedef stdext::hash_map<J9::PathKeyW, Size>
		AnyMap;
		typedef std::vector<Any>
		AnyArray;

	public:
		Anys()					{}
		// force link error
		Anys(const Anys& r) : cMap_(r.cMap_), cArray_(r.cArray_) {}

		Anys& operator=(const Anys& r) { this->cMap_ = r.cMap_; this->cArray_ = r.cArray_; return *this; }

		void	Clear();

		void	Add(J9::PathKeyW sName, const Any& rAny);
		void	Add(const Any& rAny);

		const Any&	Get(J9::PathKeyW sName) const;
		const Any&	Get(Size i) const;

		Size	GetSize() const{	return cMap_.size();	}

	private:
		Size	_Add(const Any& rAny);

	private:
		AnyMap		cMap_;
		AnyArray	cArray_;

		static const Any	cNull_;
	};

	template<typename IO>
	struct TypeTraits_Binary_Custom<IO, Any>
	{
		static Size
		Read(IO* pIO, Any& r)
		{
			Size	iRead = 0;

			U8		iType;
			iRead += TypeTraits_Binary::Read(pIO, iType);
			switch (iType)
			{
			case Any::eATUnsigned:
				{
					U32	i;
					iRead += TypeTraits_Binary::Read(pIO, i);
					r = i;
				}
				break;

			case Any::eATSigned:
				{
					S32	i;
					iRead += TypeTraits_Binary::Read(pIO, i);
					r = i;
				}
				break;

			case Any::eATFloat:
				{
					F32	f;
					iRead += TypeTraits_Binary::Read(pIO, f);
					r = f;
				}
				break;

			case Any::eATStringW:
				{
					StringW	s;
					iRead += TypeTraits_Binary::Read(pIO, s);
					r = s;
				}
				break;

			case Any::eATStringA:
				{
					StringA	s;
					iRead += TypeTraits_Binary::Read(pIO, s);
					r = s;
				}
				break;

			default:
				ASSERT(0);
				break;
			}

			return iRead;
		}

		static Size
		Write(IO* pIO, const Any& r)
		{
			Size	iWritten = 0;

			U8		iType = _sc<U8>(r.iType_);
			iWritten += TypeTraits_Binary::Write(pIO, iType);
			switch (iType)
			{
			case Any::eATUnsigned:
				{
					U32	i = r;
					iWritten += TypeTraits_Binary::Write(pIO, i);
				}
				break;

			case Any::eATSigned:
				{
					S32	i = r;
					iWritten += TypeTraits_Binary::Write(pIO, i);
				}
				break;

			case Any::eATFloat:
				{
					F32	f = r;
					iWritten += TypeTraits_Binary::Write(pIO, f);
				}
				break;

			case Any::eATStringW:
				{
					StringW	s = r;
					iWritten += TypeTraits_Binary::Write(pIO, s);
				}
				break;

			case Any::eATStringA:
				{
					StringA	s = r;
					iWritten += TypeTraits_Binary::Write(pIO, s);
				}
				break;

			default:
				ASSERT(0);
				break;
			}

			return iWritten;
		}
	};
}

#endif//__J9_ANY_H__
