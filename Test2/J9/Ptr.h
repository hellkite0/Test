// $Id: Ptr.h 6476 2010-07-16 08:07:16Z hyojin.lee $

#ifndef __J9_PTR_H__
#define __J9_PTR_H__

#include "Exception.h"
#include "TypeTraits.h"

namespace J9
{
	template<typename T>
	class Ptr;

	template<typename T1, typename T2>
	class PtrCaster
	{
	public:
		__forceinline static void		Cast(Ptr<T1>& lhs, const Ptr<T2>& rhs)
		{
			UpCast<SUPERSUBCLASS(T1, T2)>(lhs, rhs);
		}

	private:
		template<bool>
		static void		UpCast(Ptr<T1>& lhs, const Ptr<T2>& rhs);

		template<bool>
		static void		DownCast(Ptr<T1>& lhs, const Ptr<T2>& rhs);

		template<>
		__forceinline static void		UpCast<true>(Ptr<T1>& lhs, const Ptr<T2>& rhs)
		{
			T1*	p = rhs.p_;

			if (_rc<Size>(p) == _rc<Size>(rhs.p_))
			{
				lhs.p_ = p;
				lhs.AddRef();
			}
			else
			{
				// ���� ��ӵ ���� ������ ĳ���� �� �����Ͱ� ����Ű�� �ּҰ�
				// �޶����� ���� ������� ����� ������ �� ����.
				// �� ��� Ptr<>���� ����ȯ�� ����ϸ� �ȵȴ�.
				// ����ڿ��� �˷��־�� �Ѵ�.
#if defined(_DEBUG) || defined(FAST_DEBUG)
				__debugbreak();
#endif
			}
		}

		template<>
		__forceinline static void UpCast<false>(Ptr<T1>& lhs, const Ptr<T2>& rhs)
		{
			DownCast<SUPERSUBCLASS(T2, T1)>(lhs, rhs);
		}

		template<>
		__forceinline static void DownCast<true>(Ptr<T1>& lhs, const Ptr<T2>& rhs)
		{
			T1*	p = _dc<T1*>(rhs.p_);

			if (_rc<Size>(p) == _rc<Size>(rhs.p_))
			{
				lhs.p_ = p;
				lhs.AddRef();
			}
		}

		template<>
		__forceinline static void DownCast<false>(Ptr<T1>& lhs, const Ptr<T2>& rhs)
		{
			// ���⿡ �ɸ��� ĳ���� �Ϸ��� T1�� T2���̿� ��� ��� ���赵 ���°��̴�.
			CASSERT(false, CAST_ERROR);
			UNUSED(lhs);
			UNUSED(rhs);
		}
	};

	struct PtrAllocHint	{};
	#define ALLOC	(*_sc<const J9::PtrAllocHint*>(0))

	////////////////////////////////////////////////////////////////////////////////
	/// \brief Reference Count�� �Ǵ� Smart Pointer
	///
	/// \author		boro
	/// \date		2005/05/03
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class Ptr
	{
		template<typename>
		friend class Ptr;

		template<typename, typename>
		friend class PtrCaster;

	private:
		#pragma pack(push, 1)
		struct Block
		{
			U32		iHead_;
#if (_J9_PTR_DIAGNOSTIC)
			Size	iObjSize_;
#endif
			LONG	iRefCount_;
			U8		cT_[sizeof(T)];
#if (_J9_PTR_DIAGNOSTIC)
			U32		iTail_;
#endif
		};
		#pragma pack(pop)

	public:
		// constructors
		Ptr();
		Ptr(const Ptr& r);
		template<typename T2>
		Ptr(const Ptr<T2>& r);
		Ptr(T* p);
		Ptr(const PtrAllocHint& r);
		template<TARG1>
		Ptr(const PtrAllocHint&, FARG1);
		template<TARG2>
		Ptr(const PtrAllocHint&, FARG2);
		template<TARG3>
		Ptr(const PtrAllocHint&, FARG3);
		template<TARG4>
		Ptr(const PtrAllocHint&, FARG4);
		template<TARG5>
		Ptr(const PtrAllocHint&, FARG5);
		template<TARG6>
		Ptr(const PtrAllocHint&, FARG6);
		template<TARG7>
		Ptr(const PtrAllocHint&, FARG7);

		~Ptr();

		Ptr&		New();
		template<TARG1>
		Ptr&		New(FARG1);
		template<TARG2>
		Ptr&		New(FARG2);
		template<TARG3>
		Ptr&		New(FARG3);
		template<TARG4>
		Ptr&		New(FARG4);
		template<TARG5>
		Ptr&		New(FARG5);
		template<TARG6>
		Ptr&		New(FARG6);
		template<TARG7>
		Ptr&		New(FARG7);

		void		Release();

		Ptr&		operator=(const Ptr& r);
		Ptr&		operator=(T* p);

		template<typename T2>
		__forceinline Ptr&	operator=(const Ptr<T2>& r)
		{
			Release();
			if (r.p_ != NULL)	PtrCaster<T, T2>::Cast(*this, r);

			return *this;
		}

		LONG		GetRefCount() const;

		bool		IsNull() const;

		operator const T*() const;
		operator T*();
		const T*	operator->() const;
		T*			operator->();
		const T&	operator*() const;
		T&			operator*();

		bool		operator==(const Ptr& r) const;

		bool		IsValid() const;

		LONG		AddRef();
		LONG		ReleaseRef();

	private:
		static bool	CheckSig(const T* p);
		static U32	GetHead(const T* p);
		static Size	GetObjSize(const T* p);
		static U32	GetTail(const T* p);

		void		Alloc();
		void		Free();

	private:
		T*	p_;
		//static Size	iCurAlloc_;
		//static Size	iTotalAlloc_;
	};

	//template<typename T>
	//Size Ptr<T>::iCurAlloc_ = 0;
	//template<typename T>
	//Size Ptr<T>::iTotalAlloc_ = 0;

	template<typename IO, typename T>
	struct TypeTraits_Binary_Custom<IO, Ptr<T> >
	{
		static Size
		Read(IO* pIO, Ptr<T>& r)
		{
			r.New();
			return TypeTraits_Binary::Read<IO, T>(pIO, *r);
		}

		static Size
		Read(IO* pIO, Ptr<T>& r, void* pContext)
		{
			r.New();
			return TypeTraits_Binary::Read<IO, T>(pIO, *r, pContext);
		}

		static Size
		Write(IO* pIO, const Ptr<T>& r)
		{
			return TypeTraits_Binary::Write<IO, T>(pIO, *r);
		}

		static Size
		Write(IO* pIO, const Ptr<T>& r, void* pContext)
		{
			return TypeTraits_Binary::Write<IO, T>(pIO, *r, pContext);
		}
	};

	template<typename T>
	struct PtrValueLess
	{
		bool operator()(const Ptr<T>& lhs, const Ptr<T>& rhs) const { return *lhs < *rhs; }
	};

	template<typename T>
	struct PtrAddrLess
	{
		bool operator()(const Ptr<T>& lhs, const Ptr<T>& rhs) const { return lhs.p_ < rhs.p_; }
	};
};

#include "Ptr.inl"

// _Ptr<> �� ��Ŷ�� �̿��ؼ� �Ҵ��� �ּ������� �Ͼ���� �� �� �ִ�.

#endif//__J9_PTR_H__
